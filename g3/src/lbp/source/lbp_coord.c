/**
 *
 * \file
 *
 * \brief LBP Processes for Coordinator
 *
 * Copyright (c) 2023 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "adp.h"
#include "adp_api_types.h"
#include "mac_wrapper.h"
#include "eap_psk.h"
#include "lbp_coord.h"
#include "lbp_encode_decode.h"
#include "lbp_defs.h"
#include "conf_bs.h"
#include "Random.h"
#include "Timer.h"
#include "oss_if.h"

#define LOG_LEVEL LOG_LEVEL_ADP
#include "Logger.h"

#define UNUSED(v)          (void)(v)

/* BOOTSTRAP_NUM_SLOTS defines the number of parallel bootstrap procedures that can be carried out */
#define BOOTSTRAP_NUM_SLOTS 5
#define BOOTSTRAP_MSG_MAX_RETRIES 1

/* Set RandS to a fixed value (default is random) */
/* #define FIXED_RAND_S */

/* Status codes related to HostInterface processing */
typedef enum
{
    BS_STATE_WAITING_JOINNING = 0,
    BS_STATE_SENT_EAP_MSG_1,
    BS_STATE_WAITING_EAP_MSG_2,
    BS_STATE_SENT_EAP_MSG_3,
    BS_STATE_WAITING_EAP_MSG_4,
    BS_STATE_SENT_EAP_MSG_ACCEPTED,
    BS_STATE_SENT_EAP_MSG_DECLINED,
} LBP_SLOT_STATE;

typedef struct
{
    LBP_SLOT_STATE e_state;
    ADP_EXTENDED_ADDRESS m_LbdAddress;
    uint16_t us_lba_src_addr;
    uint16_t us_assigned_short_address;
    uint8_t uc_tx_handle;
    uint32_t ul_timeout;
    uint8_t uc_tx_attemps;
    EAP_PSK_RAND m_randS;
    uint32_t ul_nonce;
    uint8_t uc_pending_confirms;
    uint8_t uc_pending_tx_handler;
    uint8_t auc_data[LBP_MAX_NSDU_LENGTH];
    uint16_t us_data_length;
    EAP_PSK_CONTEXT m_PskContext;
    uint8_t m_u8MediaType;
    uint8_t m_u8DisableBackupMedium;

} LBP_SLOT_PARAMS;

static uint8_t u8NsduHandle = 0;
static uint8_t u8MaxHops = 0;
static uint16_t u16MsgTimeoutSeconds = 300;
static uint8_t u8CurrKeyIndex = 0;
static uint8_t au8CurrGMK[16]  = {0xAF, 0x4D, 0x6D, 0xCC, 0xF1, 0x4D, 0xE7, 0xC1, 0xC4, 0x23, 0x5E, 0x6F, 0xEF, 0x6C, 0x15, 0x1F};
static uint8_t au8RekeyGMK[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
static EAP_PSK_KEY sEapPskKey = {
	{0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}
};
static EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdS;
static const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdSArib = {LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB,
	{0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
	0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
	0x53, 0x4D}};
static const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdSCenFcc = {LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC,
	{0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18}};
static bool bAribBand;
static uint8_t u8EAPIdentifier = 0;
static bool bRekey;
static uint16_t u16InternalAssignedAddress = 0;

static LBP_NOTIFICATIONS_COORD lbpNotifications = {NULL};

static LBP_SLOT_PARAMS bootstrap_slots[BOOTSTRAP_NUM_SLOTS];


/**********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * Local functions to get/set ADP PIBs
 **********************************************************************************************************************/
static uint8_t _GetAdpMaxHops(void)
{
	struct TAdpGetConfirm getConfirm;
	AdpGetRequestSync(ADP_IB_MAX_HOPS, 0, &getConfirm);
	return getConfirm.m_au8AttributeValue[0];
}

static bool _SetDeviceTypeCoord(void)
{
	struct TAdpSetConfirm setConfirm;
	uint8_t u8DevTpe = 1;
	bool result = false;

	// Set on ADP
	AdpSetRequestSync(ADP_IB_DEVICE_TYPE, 0, sizeof(u8DevTpe), &u8DevTpe, &setConfirm);
	if (setConfirm.m_u8Status == G3_SUCCESS) {
		result = true;
	}

	return result;
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _log_show_slot_status(LBP_SLOT_PARAMS *pSlot)
{
	LOG_DBG(Log(
			"[BS] Updating slot with LBD_ADDR: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X, \
			state: %hu, handler: %hu  pending_cfrms: %hu  Timeout: %u, Current_Time: %u ",
			pSlot->m_LbdAddress.m_au8Value[0], pSlot->m_LbdAddress.m_au8Value[1],
			pSlot->m_LbdAddress.m_au8Value[2], pSlot->m_LbdAddress.m_au8Value[3],
			pSlot->m_LbdAddress.m_au8Value[4], pSlot->m_LbdAddress.m_au8Value[5],
			pSlot->m_LbdAddress.m_au8Value[6], pSlot->m_LbdAddress.m_au8Value[7],
			pSlot->e_state, pSlot->uc_tx_handle, pSlot->uc_pending_confirms,
			pSlot->ul_timeout, oss_get_up_time_ms()));

	UNUSED(pSlot);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void  _init_bootstrap_slots(void)
{
	uint8_t uc_i;

	for (uc_i = 0; uc_i < BOOTSTRAP_NUM_SLOTS; uc_i++) {
		bootstrap_slots[uc_i].e_state =  BS_STATE_WAITING_JOINNING;
		bootstrap_slots[uc_i].uc_pending_confirms = 0;
		bootstrap_slots[uc_i].uc_tx_handle =  0xff;
		bootstrap_slots[uc_i].ul_nonce =  0;

		memset(bootstrap_slots[uc_i].m_LbdAddress.m_au8Value, 0xff, 8);
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static LBP_SLOT_PARAMS *_get_bootstrap_slot_by_index(uint8_t uc_index)
{
	return &bootstrap_slots[uc_index];
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static uint8_t _get_next_nsdu_handler(void)
{
	return u8NsduHandle++;
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _set_keying_table(uint8_t u8KeyIndex, uint8_t *key)
{
	AdpMacSetRequest(MAC_WRP_PIB_KEY_TABLE, u8KeyIndex, 16, key);
	AdpSetRequest(ADP_IB_ACTIVE_KEY_INDEX, 0, sizeof(u8KeyIndex), &u8KeyIndex);
	u8CurrKeyIndex = u8KeyIndex;
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static uint8_t _process_accepted_GMK_activation(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT_PARAMS *p_bs_slot)
{
	unsigned char *pMemoryBuffer = &p_bs_slot->auc_data[0];
	unsigned short u16MemoryBufferLength = sizeof(p_bs_slot->auc_data);
	uint8_t pdata[3];
	uint8_t uc_result = 1;
	uint8_t u8NewKeyIndex;

	/* Get current key index and set the new one to the other */
	if (u8CurrKeyIndex == 0) {
		u8NewKeyIndex = 1;
	} else {
		u8NewKeyIndex = 0;
	}

	LOG_DBG(Log("[BS] Accepted(GMK-activation)."));

	/* prepare the protected data carring the key and short addr */
	pdata[0] = CONF_PARAM_GMK_ACTIVATION;
	pdata[1] = 0x01;
	pdata[2] = u8NewKeyIndex; /* key id */

	p_bs_slot->us_data_length = EAP_PSK_Encode_GMK_Activation(
			pdata,                 /* PCHANNEL data */
			u16MemoryBufferLength,
			pMemoryBuffer);

	/* Encode now the LBP message */
	p_bs_slot->us_data_length = LBP_Encode_AcceptedRequest(
			pLBPEUI64Address,
			p_bs_slot->m_u8MediaType,
			p_bs_slot->m_u8DisableBackupMedium,
			p_bs_slot->us_data_length,
			u16MemoryBufferLength,
			pMemoryBuffer);

	return(uc_result);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _processJoining0(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT_PARAMS *p_bs_slot)
{
	uint8_t *pMemoryBuffer = &p_bs_slot->auc_data[0];
	uint16_t u16MemoryBufferLength = sizeof(p_bs_slot->auc_data);

	LOG_DBG(Log("[BS] Process Joining 0."));

	EAP_PSK_Initialize(&sEapPskKey, &p_bs_slot->m_PskContext);

	/* initialize RandS */
	uint8_t i;
	for (i = 0; i < sizeof(p_bs_slot->m_randS.m_au8Value); i++) {
		p_bs_slot->m_randS.m_au8Value[i] = Random32() & 0xFF;
	}
#ifdef FIXED_RAND_S
	uint8_t randS[16] = {0x11, 0x84, 0x8D, 0x16, 0xBC, 0x76, 0x76, 0xF6, 0x35, 0x65, 0x90, 0x12, 0x08, 0x2B, 0x3A, 0x97};
	memcpy(p_bs_slot->m_randS.m_au8Value, randS, 16);
#endif

	p_bs_slot->us_data_length = EAP_PSK_Encode_Message1(
			u8EAPIdentifier,
			&p_bs_slot->m_randS,
			&sIdS,
			u16MemoryBufferLength,
			pMemoryBuffer
			);

	u8EAPIdentifier++;

	p_bs_slot->us_data_length = LBP_Encode_ChallengeRequest(
			pLBPEUI64Address,
			p_bs_slot->m_u8MediaType,
			p_bs_slot->m_u8DisableBackupMedium,
			p_bs_slot->us_data_length,
			u16MemoryBufferLength,
			pMemoryBuffer
			);
}

/**
 * \brief _processJoiningEAPT1.
 *
 */
static bool _processJoiningEAPT1(ADP_EXTENDED_ADDRESS au8LBPEUI64Address, uint16_t u16EAPDataLength,
		uint8_t *pEAPData, LBP_SLOT_PARAMS *p_bs_slot)
{
	EAP_PSK_RAND randS;
	EAP_PSK_RAND randP;
	uint8_t *pMemoryBuffer = &p_bs_slot->auc_data[0];
	uint16_t u16MemoryBufferLength = sizeof(p_bs_slot->auc_data);
	uint8_t pdata[50];
	uint16_t u16PDataLen = 0;
	uint16_t u16ShortAddr;
	uint8_t u8NewKeyIndex;

	LOG_DBG(Log("[BS] Process Joining EAP T1."));

	if (EAP_PSK_Decode_Message2(bAribBand, u16EAPDataLength, pEAPData, &p_bs_slot->m_PskContext, &sIdS, &randS, &randP)) {
		LOG_DBG(Log("[BS] Decoded Message2."));

		if (memcmp(randS.m_au8Value, p_bs_slot->m_randS.m_au8Value, sizeof(randS.m_au8Value)) != 0) {
			LOG_DBG(Log("[BS] ERROR: Bad RandS received"));
			return false;
		}

		EAP_PSK_InitializeTEKMSK(&randP, &p_bs_slot->m_PskContext);

		/* encode and send the message T2 */
		u16ShortAddr = p_bs_slot->us_assigned_short_address;

		/* prepare the protected data carring the key and short addr */
		pdata[u16PDataLen++] = 0x02; /* ext field */

		if (!bRekey) {
			pdata[u16PDataLen++] = CONF_PARAM_SHORT_ADDR;
			pdata[u16PDataLen++] = 2;
			pdata[u16PDataLen++] = (uint8_t)((u16ShortAddr & 0xFF00) >> 8);
			pdata[u16PDataLen++] = (uint8_t)(u16ShortAddr & 0x00FF);

			pdata[u16PDataLen++] = CONF_PARAM_GMK;
			pdata[u16PDataLen++] = 17;
			pdata[u16PDataLen++] = u8CurrKeyIndex; /* key id */
			memcpy(&pdata[u16PDataLen], au8CurrGMK, 16); /* key */
			u16PDataLen += 16;

			pdata[u16PDataLen++] = CONF_PARAM_GMK_ACTIVATION;
			pdata[u16PDataLen++] = 1;
			pdata[u16PDataLen++] = u8CurrKeyIndex; /* key id */
		}
		else {
			/* Get current key index and set the new one to the other */
			if (u8CurrKeyIndex == 0) {
				u8NewKeyIndex = 1;
			} else {
				u8NewKeyIndex = 0;
			}

			pdata[u16PDataLen++] = CONF_PARAM_GMK;
			pdata[u16PDataLen++] = 17;
			pdata[u16PDataLen++] = u8NewKeyIndex; /* key id */
			memcpy(&pdata[u16PDataLen], au8RekeyGMK, 16); /* key */
			u16PDataLen += 16;
		}

		LOG_DBG(Log("[BS] Encoding Message3."));
		p_bs_slot->us_data_length = EAP_PSK_Encode_Message3(
				&p_bs_slot->m_PskContext,
				u8EAPIdentifier,
				&randS,
				&randP,
				&sIdS,
				p_bs_slot->ul_nonce,
				PCHANNEL_RESULT_DONE_SUCCESS,
				u16PDataLen,
				pdata,
				u16MemoryBufferLength,
				pMemoryBuffer
				);

		u8EAPIdentifier++;
		p_bs_slot->ul_nonce++;

		/* Encode now the LBP message */
		p_bs_slot->us_data_length = LBP_Encode_ChallengeRequest(
				&au8LBPEUI64Address,
				p_bs_slot->m_u8MediaType,
				p_bs_slot->m_u8DisableBackupMedium,
				p_bs_slot->us_data_length,
				u16MemoryBufferLength,
				pMemoryBuffer
				);

		return true;
	}
	else {
		LOG_DBG(Log("[BS] ERROR: _processJoiningEAPT1."));
		return false;
	}
}

/**
 * \brief _processJoiningEAPT3.
 *
 */
static bool _processJoiningEAPT3(ADP_EXTENDED_ADDRESS au8LBPEUI64Address, uint8_t *pBootStrappingData, uint16_t u16EAPDataLength,
		uint8_t *pEAPData, LBP_SLOT_PARAMS *p_bs_slot)
{
	EAP_PSK_RAND randS;
	uint8_t u8PChannelResult = 0;
	uint32_t u32Nonce = 0;
	uint16_t u16PChannelDataLength = 0;
	uint8_t *pPChannelData = 0L;
	uint8_t *pMemoryBuffer = &p_bs_slot->auc_data[0];
	uint16_t u16MemoryBufferLength = sizeof(p_bs_slot->auc_data);

	LOG_DBG(Log("[BS] Process Joining EAP T3."));

	if (EAP_PSK_Decode_Message4(u16EAPDataLength, pEAPData, &p_bs_slot->m_PskContext,
			22, pBootStrappingData, &randS, &u32Nonce, &u8PChannelResult,
			&u16PChannelDataLength, &pPChannelData)) {
		LOG_DBG(Log("[BS] Decoded Message4."));
		LOG_DBG(Log("[BS] Encoding Accepted."));
		/* encode and send the message T2 */
		p_bs_slot->us_data_length = EAP_PSK_Encode_EAP_Success(
				u8EAPIdentifier,
				u16MemoryBufferLength,
				pMemoryBuffer
				);

		if (memcmp(randS.m_au8Value, p_bs_slot->m_randS.m_au8Value, sizeof(randS.m_au8Value)) != 0) {
			LOG_DBG(Log("[BS] ERROR: Bad RandS received"));
			return false;
		}

		u8EAPIdentifier++;

		/* Encode now the LBP message */
		p_bs_slot->us_data_length = LBP_Encode_AcceptedRequest(
				&au8LBPEUI64Address,
				p_bs_slot->m_u8MediaType,
				p_bs_slot->m_u8DisableBackupMedium,
				p_bs_slot->us_data_length,
				u16MemoryBufferLength,
				pMemoryBuffer
				);

		return true;
	}
	else {
		LOG_DBG(Log("[BS] ERROR: _processJoiningEAPT3."));
		return false;
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static bool _timeout_is_past(uint32_t ul_timeout_value)
{
	return (((int32_t)oss_get_up_time_ms()) - (int32_t)ul_timeout_value > 0);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _initialize_bootstrap_message(LBP_SLOT_PARAMS *p_bs_slot)
{
	p_bs_slot->us_data_length = 0;
	memset(p_bs_slot->auc_data, 0, sizeof(p_bs_slot->auc_data));
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static LBP_SLOT_PARAMS *_get_bootstrap_slot_by_addr(uint8_t *p_eui64)
{
	uint8_t uc_i;
	LBP_SLOT_PARAMS *p_out_slot = NULL;

	/* Check if the lbd is already started */
	for (uc_i = 0; uc_i < BOOTSTRAP_NUM_SLOTS; uc_i++) {
		if (!memcmp(bootstrap_slots[uc_i].m_LbdAddress.m_au8Value, p_eui64, 8)) {
			p_out_slot = &bootstrap_slots[uc_i];
			LOG_DBG(Log("[BS] _get_bootstrap_slot_by_addr --> Slot in use found: %d ", uc_i));
			break;
		}
	}
	/* If lbd not in progress find free slot */
	if (!p_out_slot) {
		for (uc_i = 0; uc_i < BOOTSTRAP_NUM_SLOTS; uc_i++) {
			if (bootstrap_slots[uc_i].e_state ==  BS_STATE_WAITING_JOINNING) {
				p_out_slot = &bootstrap_slots[uc_i];
				LOG_DBG(Log("[BS] _get_bootstrap_slot_by_addr --> Slot free found: %d ", uc_i));
				break;
			}
		}
	}

	if (!p_out_slot) {
		LOG_DBG(Log("[BS] _get_bootstrap_slot_by_addr --> Slot not found "));
	}

	return p_out_slot;
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_Rekey(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address, bool distribute)
{
	struct TAdpAddress dstAddr;
	uint8_t uc_handle;

	LBP_SLOT_PARAMS *p_bs_slot = _get_bootstrap_slot_by_addr(pEUI64Address->m_au8Value);

	if (p_bs_slot) {
		_initialize_bootstrap_message(p_bs_slot);
		/* DisableBackupFlag and MediaType set to 0x0 in Rekeying frames */
		p_bs_slot->m_u8DisableBackupMedium = 0;
		p_bs_slot->m_u8MediaType = 0;
		if (distribute) {
			/* If re-keying in GMK distribution phase */
			/* Send ADPM-LBP.Request(EAPReq(mes1)) to each registered device */
			_processJoining0(pEUI64Address, p_bs_slot);
			p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_1;
			/* Fill slot address just in case a free slot was returned */
			memcpy(p_bs_slot->m_LbdAddress.m_au8Value, pEUI64Address->m_au8Value, ADP_ADDRESS_64BITS);
		} else {
			/* GMK activation phase */
			_process_accepted_GMK_activation(pEUI64Address, p_bs_slot);
			p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_ACCEPTED;
		}

		/* Send the previously prepared message */
		dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
		dstAddr.m_u16ShortAddr = shortAddress;

		if (p_bs_slot->uc_pending_confirms > 0) {
			p_bs_slot->uc_pending_tx_handler = p_bs_slot->uc_tx_handle;
		}

		uc_handle = _get_next_nsdu_handler();
		p_bs_slot->ul_timeout = oss_get_up_time_ms() + 1000 * u16MsgTimeoutSeconds;
		p_bs_slot->uc_tx_handle = uc_handle;
		p_bs_slot->uc_tx_attemps = 0;
		p_bs_slot->uc_pending_confirms++;
		LOG_DBG(Log("[BS] AdpLbpRequest Called, handler: %d ", p_bs_slot->uc_tx_handle));
		AdpLbpRequest((struct TAdpAddress const *)&dstAddr,          /* Destination address */
				p_bs_slot->us_data_length,                   /* NSDU length */
				&p_bs_slot->auc_data[0],                     /* NSDU */
				uc_handle,                                   /* NSDU handle */
				u8MaxHops,                            /* Max. Hops */
				true,                                        /* Discover route */
				0,                                           /* QoS */
				false);                                      /* Security enable */
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_SetRekeyPhase(bool rekeyStart)
{
	bRekey = rekeyStart;
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_ActivateNewKey(void)
{
	uint8_t u8NewKeyIndex;

	/* Get current key index and set the new one to the other */
	if (u8CurrKeyIndex == 0) {
		u8NewKeyIndex = 1;
	} else {
		u8NewKeyIndex = 0;
	}

	/* Set GMK from Rekey GMK */
	memcpy(au8CurrGMK, au8RekeyGMK, 16);
	/* Set key table using the new index and new GMK */
	_set_keying_table(u8NewKeyIndex, au8CurrGMK);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_UpdateBootstrapSlots(void)
{
	uint8_t uc_i;
	for (uc_i = 0; uc_i < BOOTSTRAP_NUM_SLOTS; uc_i++) {
		if (bootstrap_slots[uc_i].e_state != BS_STATE_WAITING_JOINNING) {
			if (_timeout_is_past(bootstrap_slots[uc_i].ul_timeout)) {
				if (bootstrap_slots[uc_i].uc_pending_confirms == 0) {
					if (bootstrap_slots[uc_i].uc_tx_attemps < BOOTSTRAP_MSG_MAX_RETRIES) {
						bootstrap_slots[uc_i].uc_tx_attemps++;
						if (bootstrap_slots[uc_i].e_state == BS_STATE_WAITING_EAP_MSG_2) {
							bootstrap_slots[uc_i].e_state = BS_STATE_SENT_EAP_MSG_1;
							LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_1"));
							_log_show_slot_status(&bootstrap_slots[uc_i]);
						} else if (bootstrap_slots[uc_i].e_state == BS_STATE_WAITING_EAP_MSG_4) {
							bootstrap_slots[uc_i].e_state = BS_STATE_SENT_EAP_MSG_3;
							LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_3"));
							_log_show_slot_status(&bootstrap_slots[uc_i]);
						}

						struct TAdpAddress dstAddr;

						if (bootstrap_slots[uc_i].us_data_length > 0) {
							if (bootstrap_slots[uc_i].us_lba_src_addr == 0xFFFF) {
								dstAddr.m_u8AddrSize = 8;
								memcpy(dstAddr.m_ExtendedAddress.m_au8Value, &bootstrap_slots[uc_i].m_LbdAddress.m_au8Value, 8);
							} else {
								dstAddr.m_u8AddrSize = 2;
								dstAddr.m_u16ShortAddr = bootstrap_slots[uc_i].us_lba_src_addr;
							}

							if (bootstrap_slots[uc_i].uc_pending_confirms > 0) {
								bootstrap_slots[uc_i].uc_pending_tx_handler = bootstrap_slots[uc_i].uc_tx_handle;
							}

							bootstrap_slots[uc_i].uc_tx_handle = _get_next_nsdu_handler();
							bootstrap_slots[uc_i].ul_timeout = oss_get_up_time_ms() + 1000 * u16MsgTimeoutSeconds;
							bootstrap_slots[uc_i].uc_pending_confirms++;

							LOG_DBG(Log("[BS] Timeout detected. Re-sending MSG for slot: %d Attempt: %d ", uc_i,
									bootstrap_slots[uc_i].uc_tx_attemps));
							_log_show_slot_status(&bootstrap_slots[uc_i]);
							LOG_DBG(Log("[BS] AdpLbpRequest Called, handler: %d ", bootstrap_slots[uc_i].uc_tx_handle));
							AdpLbpRequest((struct TAdpAddress const *)&dstAddr,         /* Destination address */
									bootstrap_slots[uc_i].us_data_length,       /* NSDU length */
									&bootstrap_slots[uc_i].auc_data[0],         /* NSDU */
									bootstrap_slots[uc_i].uc_tx_handle,         /* NSDU handle */
									u8MaxHops,                           /* Max. Hops */
									true,                                       /* Discover route */
									0,                                          /* QoS */
									false);                                     /* Security enable */
						}
					} else {
						LOG_DBG(Log("[BS] Reset slot %d:  ", uc_i));
						bootstrap_slots[uc_i].e_state = BS_STATE_WAITING_JOINNING;
						bootstrap_slots[uc_i].uc_pending_confirms = 0;
						bootstrap_slots[uc_i].ul_nonce =  0;
						bootstrap_slots[uc_i].ul_timeout = 0xFFFFFFFF;
					}
				} else { /* Pending confirm then increase timeout time */
					LOG_DBG(Log("[BS] NEVER SHOUL BE HERE --> Reset slot %d:  ", uc_i));
					bootstrap_slots[uc_i].e_state = BS_STATE_WAITING_JOINNING;
					bootstrap_slots[uc_i].uc_pending_confirms = 0;
					bootstrap_slots[uc_i].ul_nonce =  0;
					bootstrap_slots[uc_i].ul_timeout = 0xFFFFFFFF;
				}
			}
		}
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void AdpLbpConfirmCoord(struct TAdpLbpConfirm *pLbpConfirm)
{
	uint8_t uc_i;

	LBP_SLOT_PARAMS *p_current_slot = NULL;

	bool b_is_accepted_confirm = false;

	for (uc_i = 0; uc_i < BOOTSTRAP_NUM_SLOTS; uc_i++) {
		LBP_SLOT_PARAMS *p_slot = _get_bootstrap_slot_by_index(uc_i);

		if (p_slot->uc_pending_confirms == 1 && pLbpConfirm->m_u8NsduHandle == p_slot->uc_tx_handle && p_slot->e_state != BS_STATE_WAITING_JOINNING) {
			LOG_DBG(Log("[BS] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).",
					p_slot->m_LbdAddress.m_au8Value[0], p_slot->m_LbdAddress.m_au8Value[1],
					p_slot->m_LbdAddress.m_au8Value[2], p_slot->m_LbdAddress.m_au8Value[3],
					p_slot->m_LbdAddress.m_au8Value[4], p_slot->m_LbdAddress.m_au8Value[5],
					p_slot->m_LbdAddress.m_au8Value[6], p_slot->m_LbdAddress.m_au8Value[7]));

			p_current_slot = p_slot;
			p_slot->uc_pending_confirms--;
			if (pLbpConfirm->m_u8Status == G3_SUCCESS) {
				switch (p_slot->e_state) {
				case BS_STATE_SENT_EAP_MSG_1:
					p_slot->e_state = BS_STATE_WAITING_EAP_MSG_2;
					LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_EAP_MSG_2"));
					_log_show_slot_status(p_slot);
					break;

				case BS_STATE_SENT_EAP_MSG_3:
					p_slot->e_state = BS_STATE_WAITING_EAP_MSG_4;
					LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_EAP_MSG_4"));
					_log_show_slot_status(p_slot);
					break;

				case BS_STATE_SENT_EAP_MSG_ACCEPTED:
					p_slot->e_state = BS_STATE_WAITING_JOINNING;
					p_slot->ul_nonce =  0;
					LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
					_log_show_slot_status(p_slot);
					b_is_accepted_confirm = true;
					break;

				case BS_STATE_SENT_EAP_MSG_DECLINED:
					p_slot->e_state = BS_STATE_WAITING_JOINNING;
					p_slot->ul_nonce =  0;
					LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
					_log_show_slot_status(p_slot);
					break;

				default:
					p_slot->e_state = BS_STATE_WAITING_JOINNING;
					p_slot->ul_nonce =  0;
					LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
					_log_show_slot_status(p_slot);
					break;
				}
			} else {
				p_slot->e_state = BS_STATE_WAITING_JOINNING;
				p_slot->ul_nonce =  0;
				LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
				_log_show_slot_status(p_slot);
			}
		//Check if confirm received is for first request (uc_pending_tx_handler)
		} else if (p_slot->uc_pending_confirms == 2 && pLbpConfirm->m_u8NsduHandle == p_slot->uc_pending_tx_handler) {
			LOG_DBG(Log("[BS] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).",
					p_slot->m_LbdAddress.m_au8Value[0], p_slot->m_LbdAddress.m_au8Value[1],
					p_slot->m_LbdAddress.m_au8Value[2], p_slot->m_LbdAddress.m_au8Value[3],
					p_slot->m_LbdAddress.m_au8Value[4], p_slot->m_LbdAddress.m_au8Value[5],
					p_slot->m_LbdAddress.m_au8Value[6], p_slot->m_LbdAddress.m_au8Value[7]));
			p_slot->uc_pending_confirms--;
			_log_show_slot_status(p_slot);
			p_current_slot = p_slot;
		//Check if confirm received is for last request (uc_pending_tx_handler)
		} else if (p_slot->uc_pending_confirms == 2 && pLbpConfirm->m_u8NsduHandle == p_slot->uc_tx_handle) {
			LOG_DBG(Log("[BS] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).",
					p_slot->m_LbdAddress.m_au8Value[0], p_slot->m_LbdAddress.m_au8Value[1],
					p_slot->m_LbdAddress.m_au8Value[2], p_slot->m_LbdAddress.m_au8Value[3],
					p_slot->m_LbdAddress.m_au8Value[4], p_slot->m_LbdAddress.m_au8Value[5],
					p_slot->m_LbdAddress.m_au8Value[6], p_slot->m_LbdAddress.m_au8Value[7]));
			p_slot->uc_pending_confirms--;
			p_slot->uc_tx_handle = p_slot->uc_pending_tx_handler;
			_log_show_slot_status(p_slot);
			p_current_slot = p_slot;
		}
	}

	if (!p_current_slot) {
		LOG_DBG(Log("[BS] AdpNotification_LbpConfirm from unkown node, status: %d  handler: %d ",
				pLbpConfirm->m_u8Status, pLbpConfirm->m_u8NsduHandle));
		return;
	} else {
		p_current_slot->ul_timeout = oss_get_up_time_ms() + 1000 * u16MsgTimeoutSeconds;
	}

	if (pLbpConfirm->m_u8Status == G3_SUCCESS && b_is_accepted_confirm) {
		/* Upper layer indications */
		if (lbpNotifications.joinCompleteIndication != NULL) {
			lbpNotifications.joinCompleteIndication(p_current_slot->m_LbdAddress.m_au8Value, p_current_slot->us_assigned_short_address);
		}
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void AdpLbpIndicationCoord(struct TAdpLbpIndication *pLbpIndication)
{
	uint16_t u16OrigAddress;
	uint8_t u8MessageType;
	uint8_t *pBootStrappingData;
	uint16_t u16BootStrappingDataLength;
	struct TAdpAddress dstAddr;

	/* Embedded EAP message */
	uint8_t u8Code = 0;
	uint8_t u8Identifier = 0;
	uint8_t u8TSubfield = 0;
	uint16_t u16EAPDataLength = 0;
	uint8_t *pEAPData = 0L;
	ADP_EXTENDED_ADDRESS m_current_LbdAddress;
	LBP_SLOT_PARAMS *p_bs_slot;
	uint8_t u8MediaType;
	uint8_t u8DisableBackupMedium;

	if (pLbpIndication->m_pSrcAddr->m_u8AddrSize == ADP_ADDRESS_64BITS) {
		/* When directly communicating with the LBD(using extended addressing) this field is set to 0xFFFF. */
		u16OrigAddress = 0xFFFF;
		/* Check frame coming from LBD is not secured */
		if (pLbpIndication->m_u8SecurityLevel != MAC_WRP_SECURITY_LEVEL_NONE) {
			return;
		}
	}
	else {
		/* When frame comes from Joined device or its LBA, m_pSrcAddr contains the Originator Address */
		u16OrigAddress = pLbpIndication->m_pSrcAddr->m_u16ShortAddr;
	}

	LOG_DBG(Log("LBPIndication_Notify(): LbaAddr: 0x%04X NsduLength: %hu.",u16OrigAddress, pLbpIndication->m_u16NsduLength));

	u8MessageType = ((pLbpIndication->m_pNsdu[0] & 0xF0) >> 4);
	u8MediaType = ((pLbpIndication->m_pNsdu[0] & 0x08) >> 3);
	u8DisableBackupMedium = ((pLbpIndication->m_pNsdu[0] & 0x04) >> 2);
	pBootStrappingData = &pLbpIndication->m_pNsdu[10];
	u16BootStrappingDataLength = pLbpIndication->m_u16NsduLength - 10;

	memcpy(m_current_LbdAddress.m_au8Value, &pLbpIndication->m_pNsdu[2], 8);

	/* Check differet conditions if frame comes directly from LBD or from LBA */
	if (u16OrigAddress == 0xFFFF) {
		/* Frame from LBD */
		/* Originator address must be equal to the one in LBP frame */
		if (memcmp(&m_current_LbdAddress, &pLbpIndication->m_pSrcAddr->m_ExtendedAddress, sizeof(ADP_EXTENDED_ADDRESS)) != 0) {
			return;
		}
		/* Frame type must be equal to JOINING */
		if (u8MessageType != LBP_JOINING) {
			return;
		}
	}
	else {
		/* Frame from LBA */
		/* Src address must be between 0x0000 and 0x7FFF */
		if (u16OrigAddress > 0x7FFF) {
			return;
		}
		/* Security level already checked for frames between 16-bit addresses */
	}

	p_bs_slot = _get_bootstrap_slot_by_addr(m_current_LbdAddress.m_au8Value);
	if (p_bs_slot) {
		_initialize_bootstrap_message(p_bs_slot);
	}

	if (u8MessageType == LBP_JOINING) {
		LOG_DBG(Log("[BS] Processing incoming LBP_JOINING... LBD Address: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X ",
				m_current_LbdAddress.m_au8Value[0], m_current_LbdAddress.m_au8Value[1],
				m_current_LbdAddress.m_au8Value[2], m_current_LbdAddress.m_au8Value[3],
				m_current_LbdAddress.m_au8Value[4], m_current_LbdAddress.m_au8Value[5],
				m_current_LbdAddress.m_au8Value[6], m_current_LbdAddress.m_au8Value[7]));

		/* Check the bootstrapping data in order to see the progress of the joining process */
		if (u16BootStrappingDataLength == 0) {
			LOG_DBG(Log("[BS] First joining request."));
			/* This is the first joining request. Responded only if no other BS was in progress. */
			if (p_bs_slot) {
				if (p_bs_slot->e_state == BS_STATE_WAITING_JOINNING) {
					/* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
					p_bs_slot->m_u8MediaType = u8MediaType;
					p_bs_slot->m_u8DisableBackupMedium = u8DisableBackupMedium;
					/* Store information to be used in Address Assign function */
					p_bs_slot->us_lba_src_addr = u16OrigAddress;
					memcpy(p_bs_slot->m_LbdAddress.m_au8Value, m_current_LbdAddress.m_au8Value,
							sizeof(m_current_LbdAddress.m_au8Value));
					/* Check with upper layer if the joining device is accepted */
					if (lbpNotifications.joinRequestIndication != NULL) {
						lbpNotifications.joinRequestIndication(m_current_LbdAddress.m_au8Value);
					}
					else {
						/* If there is no configured callback, assign addresses sequentially */
						u16InternalAssignedAddress++;
						LBP_ShortAddressAssign(m_current_LbdAddress.m_au8Value, u16InternalAssignedAddress);
					}
					/* Exit function, response will be sent on Address Assign function */
					return;
				}
				} else {
					LOG_DBG(Log("[BS] Repeated message processing Joining, silently ignored "));
				}
			} else {
				LOG_DBG(Log("[BS] No slots available to process the request. Ignored."));
			}
		} else {
			/* Check if the message comes from a device currently under BS */
			if (p_bs_slot) {
				/* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
				p_bs_slot->m_u8MediaType = u8MediaType;
				p_bs_slot->m_u8DisableBackupMedium = u8DisableBackupMedium;
				/* check the type of the bootstrap data */
				if ((pBootStrappingData[0] & 0x01) != 0x01) {
					LOG_DBG(Log("[BS] Successive joining request."));
					if (EAP_PSK_Decode_Message(
							u16BootStrappingDataLength,
							pBootStrappingData,
							&u8Code,
							&u8Identifier,
							&u8TSubfield,
							&u16EAPDataLength,
							&pEAPData)) {
						if (u8Code == EAP_RESPONSE) {
							if (u8TSubfield == EAP_PSK_T1 && (p_bs_slot->e_state == BS_STATE_WAITING_EAP_MSG_2 ||
									p_bs_slot->e_state == BS_STATE_SENT_EAP_MSG_1)) {
								if (_processJoiningEAPT1(m_current_LbdAddress, u16EAPDataLength, pEAPData, p_bs_slot) != 1) {
									/* Abort current BS process */
									LOG_DBG(Log("[BS] LBP error processing EAP T1."));
									p_bs_slot->e_state = BS_STATE_WAITING_JOINNING;
									p_bs_slot->uc_pending_confirms = 0;
									p_bs_slot->ul_nonce =  0;

									LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
								} else {
									p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_3;
									LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_3"));
								}
							} else if (u8TSubfield == EAP_PSK_T3 &&
									(p_bs_slot->e_state == BS_STATE_WAITING_EAP_MSG_4 || p_bs_slot->e_state ==
									BS_STATE_SENT_EAP_MSG_3)) {
								if (_processJoiningEAPT3(m_current_LbdAddress, pBootStrappingData, u16EAPDataLength, pEAPData,
										p_bs_slot)) {
									p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_ACCEPTED;
									LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_ACCEPTED"));
								} else {
									LOG_DBG(Log("[BS] LBP error processing EAP T3."));
									p_bs_slot->e_state = BS_STATE_WAITING_JOINNING;
									p_bs_slot->uc_pending_confirms = 0;
									p_bs_slot->ul_nonce =  0;
									LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
								}
							} else {
								/* Abort current BS process */
								LOG_DBG(Log("[BS] LBP protocol error."));
								p_bs_slot->e_state = BS_STATE_WAITING_JOINNING;
								p_bs_slot->uc_pending_confirms = 0;
								p_bs_slot->ul_nonce =  0;
								LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
							}
						}
					} else {
						/* Abort current BS process */
						LOG_DBG(Log("[BS] ERROR decoding message."));
						p_bs_slot->e_state = BS_STATE_WAITING_JOINNING;
						p_bs_slot->uc_pending_confirms = 0;
						p_bs_slot->ul_nonce =  0;
						LOG_DBG(Log("[BS] Slot updated to BS_STATE_WAITING_JOINNING"));
						_log_show_slot_status(p_bs_slot);
					}
				}
			} else {
				LOG_DBG(Log("[BS] Concurrent successive joining received. Ignored."));
			}
		}
	} else if (u8MessageType == LBP_KICK_FROM_LBD) {
		/* Upper layer LEAVE callback will be invoked later */
	} else {
		LOG_DBG(Log("[BS] ERROR: unknown incoming message."));
	}

	if (p_bs_slot != NULL) {
		if (p_bs_slot->us_data_length > 0) {
			if (u16OrigAddress == 0xFFFF) {
				dstAddr.m_u8AddrSize = ADP_ADDRESS_64BITS;
				memcpy(dstAddr.m_ExtendedAddress.m_au8Value, p_bs_slot->m_LbdAddress.m_au8Value, 8);
			} else {
				dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
				dstAddr.m_u16ShortAddr = u16OrigAddress;
			}

			if (p_bs_slot->uc_pending_confirms > 0) {
				p_bs_slot->uc_pending_tx_handler = p_bs_slot->uc_tx_handle;
			}

			p_bs_slot->uc_tx_handle = _get_next_nsdu_handler();
			p_bs_slot->ul_timeout = oss_get_up_time_ms() + 1000 * u16MsgTimeoutSeconds;
			p_bs_slot->uc_tx_attemps = 0;
			p_bs_slot->uc_pending_confirms++;

			_log_show_slot_status(p_bs_slot);
			LOG_DBG(Log("[BS] AdpLbpRequest Called, handler: %d ", p_bs_slot->uc_tx_handle));
			AdpLbpRequest((struct TAdpAddress const *)&dstAddr,         /* Destination address */
					p_bs_slot->us_data_length,                  /* NSDU length */
					&p_bs_slot->auc_data[0],                    /* NSDU */
					p_bs_slot->uc_tx_handle,                    /* NSDU handle */
					u8MaxHops,                           /* Max. Hops */
					true,                                       /* Discover route */
					0,                                          /* QoS */
					false);                                     /* Security enable */
		}
	}

	/* Upper layer indications */
	if (u8MessageType == LBP_KICK_FROM_LBD) {
		if (lbpNotifications.leaveIndication != NULL) {
			lbpNotifications.leaveIndication(u16OrigAddress);
		}
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_InitCoord(bool aribBand)
{
	struct TAdpNotificationsToLbp notifications;

	notifications.fnctAdpLbpConfirm = AdpLbpConfirmCoord;
	notifications.fnctAdpLbpIndication = AdpLbpIndicationCoord;

	AdpSetNotificationsToLbp(&notifications);

	bRekey = false;

	bAribBand = aribBand;
	if (aribBand) {
		sIdS.m_u8Size = LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB;
		memcpy(sIdS.m_au8Value, sIdSArib.m_au8Value, LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB);
	} else {
		sIdS.m_u8Size = LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC;
		memcpy(sIdS.m_au8Value, sIdSCenFcc.m_au8Value, LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC);
	}

	_set_keying_table(INITIAL_KEY_INDEX, au8CurrGMK);

	_init_bootstrap_slots();

	u8MaxHops = _GetAdpMaxHops();

	_SetDeviceTypeCoord();
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_SetNotificationsCoord(LBP_NOTIFICATIONS_COORD *pNotifications)
{
	if (pNotifications != NULL) {
		lbpNotifications = *pNotifications;
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
bool LBP_KickDevice(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address)
{
	struct TAdpAddress dstAddr;
	uint8_t au8Data[10];
	uint16_t u16Length;

	/* Send KICK to the device */
	dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
	dstAddr.m_u16ShortAddr = shortAddress;

	u16Length = LBP_Encode_KickToLBD(pEUI64Address, sizeof(au8Data), au8Data);

	/* If message was properly encoded, send it */
	if (u16Length) {
		AdpLbpRequest((struct TAdpAddress const *)&dstAddr,     /* Destination address */
				u16Length,                              /* NSDU length */
				au8Data,                                /* NSDU */
				_get_next_nsdu_handler(),               /* NSDU handle */
				u8MaxHops,                       /* Max. Hops */
				true,                                   /* Discover route */
				0,                                      /* QoS */
				false);                                 /* Security enable */

		return true;
	} else {
		return false;
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_SetParamCoord(uint32_t attributeId, uint16_t attributeIndex, uint8_t attributeLen, const uint8_t *pAttributeValue,
		LBP_SET_PARAM_CONFIRM *pSetConfirm)
{
	pSetConfirm->attributeId = attributeId;
	pSetConfirm->attributeIndex = attributeIndex;
	pSetConfirm->status = LBP_STATUS_UNSUPPORTED_PARAMETER;

	switch (attributeId) {
	case LBP_IB_IDS:
		if ((attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB) || (attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC)) {
			sIdS.m_u8Size = attributeLen;
			memcpy(sIdS.m_au8Value, pAttributeValue, attributeLen);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_PSK:
		if (attributeLen == 16) {
			memcpy(sEapPskKey.m_au8Value, pAttributeValue, 16);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			/* Wrong parameter size */
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_GMK:
		if (attributeLen == 16) {
			/* Set GMK on LBP module */
			memcpy(au8CurrGMK, pAttributeValue, 16);
			/* Set key table on G3 stack using the new index and new GMK */
			_set_keying_table(attributeIndex, (uint8_t *)pAttributeValue);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			/* Wrong parameter size */
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_REKEY_GMK:
		if (attributeLen == 16) {
			memcpy(au8RekeyGMK, pAttributeValue, 16);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			/* Wrong parameter size */
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_MSG_TIMEOUT:
		if (attributeLen == 2) {
			u16MsgTimeoutSeconds = ((pAttributeValue[1] << 8) | pAttributeValue[0]);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			/* Wrong parameter size */
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	default:
		/* Unknown LBP parameter */
		break;
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_ShortAddressAssign(uint8_t *pExtAddress, uint16_t assignedAddress)
{
	t_bootstrap_slot *p_bs_slot;
	struct TAdpAddress dstAddr;
	
	/* Get slot from extended address*/
	p_bs_slot = _get_bootstrap_slot_by_addr(pExtAddress);

	if (p_bs_slot) {
		if (assignedAddress == 0xFFFF) {
			// Device rejected
			p_bs_slot->us_data_length = LBP_EncodeDecline(p_bs_slot->m_LbdAddress.m_au8Value, &p_bs_slot->auc_data[0], sizeof(p_bs_slot->auc_data),
				u8EAPIdentifier, p_bs_slot->m_u8MediaType, p_bs_slot->m_u8DisableBackupMedium);
			p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_DECLINED;
			u8EAPIdentifier++;
			LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_DECLINED"));
		} else {
			p_bs_slot->us_assigned_short_address = assignedAddress;
			_processJoining0(&p_bs_slot->m_LbdAddress, p_bs_slot);
			p_bs_slot->e_state = BS_STATE_SENT_EAP_MSG_1;
			LOG_DBG(Log("[BS] Slot updated to BS_STATE_SENT_EAP_MSG_1"));
		}
		
		if (p_bs_slot->us_data_length > 0) {
			if (p_bs_slot->us_lba_src_addr == 0xFFFF) {
				dstAddr.m_u8AddrSize = ADP_ADDRESS_64BITS;
				memcpy(dstAddr.m_ExtendedAddress.m_au8Value, p_bs_slot->m_LbdAddress.m_au8Value, 8);
			} else {
				dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
				dstAddr.m_u16ShortAddr = p_bs_slot->us_lba_src_addr;
			}

			if (p_bs_slot->uc_pending_confirms > 0) {
				p_bs_slot->uc_pending_tx_handler = p_bs_slot->uc_tx_handle;
			}

			p_bs_slot->uc_tx_handle = _get_next_nsdu_handler();
			p_bs_slot->ul_timeout = oss_get_up_time_ms() + 1000 * u16MsgTimeoutSeconds;
			p_bs_slot->uc_tx_attemps = 0;
			p_bs_slot->uc_pending_confirms++;

			_log_show_slot_status(p_bs_slot);
			LOG_DBG(Log("[BS] AdpLbpRequest Called, handler: %d ", p_bs_slot->uc_tx_handle));
			AdpLbpRequest((struct TAdpAddress const *)&dstAddr, /* Destination address */
					p_bs_slot->us_data_length,                  /* NSDU length */
					&p_bs_slot->auc_data[0],                    /* NSDU */
					p_bs_slot->uc_tx_handle,                    /* NSDU handle */
					u8MaxHops,                                  /* Max. Hops */
					true,                                       /* Discover route */
					0,                                          /* QoS */
					false);                                     /* Security enable */
		}
	}
}
