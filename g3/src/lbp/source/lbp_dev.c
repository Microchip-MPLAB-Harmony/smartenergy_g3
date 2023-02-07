/**
 *
 * \file
 *
 * \brief LBP Processes for Device
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

#include "AdpApi.h"
#include "AdpApiTypes.h"
#include "ProcessLbpDev.h"
#include "ProtoLbp.h"
#include "LbpDefs.h"
#include "mac_wrapper.h"
#include "RoutingApi.h"
#include "Random.h"
#include "Timer.h"

#define LOG_LEVEL LOG_LEVEL_ADP
#include "Logger.h"

#define UNUSED(v)          (void)(v)

#define UNSET_KEY   0xFF

struct TLBPContextDev {
	/* Timer to control the join or rekey process if no response is received */
	struct TTimer m_JoinTimer;
	/* Timer to control the delay to reset G3 stack after a Kick is received */
  	struct TTimer m_KickTimer;
	/* information related to joining */
	uint16_t m_u16LbaAddress;
	uint16_t m_u16JoiningShortAddress;
	/*  // keeps authentication context */
	EAP_PSK_CONTEXT m_PskContext;
	/* Media Type to use between LBD and LBA. It is encoded in LBP frames */
	uint8_t m_u8MediaType;
	/* Available MAC layers */
	enum EMacWrpAvailableMacLayers m_AvailableMacLayers;

	/* State of the bootstrap process */
	uint8_t m_u8BootstrapState;
	/* Number of pending message confirms */
	uint8_t m_u8PendingConfirms;
	/* EUI64 address of the device */
	ADP_EXTENDED_ADDRESS m_EUI64Address;
	/* This parameter specifies the PAN ID: 0xFFFF means not connected to PAN */
	uint16_t m_u16PanId;
	/* 16-bit address for new device which is unique inside the PAN */
	uint16_t m_u16ShortAddress;
	/* Holds the GMK keys */
	ADP_GROUP_MASTER_KEY m_GroupMasterKey[2];

	/* Upper layer notifications */
	LBP_NOTIFICATIONS_DEV m_LbpNotifications;
};

struct TLBPContextDev g_LbpContext;

static EAP_PSK_KEY sEapPskKey = {
	{0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}
};
static EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P sIdP = {{0}};

static EAP_PSK_RAND randP = {{0}};

static uint8_t au8LbpBuffer[LBP_MAX_NSDU_LENGTH];

static uint8_t u8NsduHandle = 0;
static uint8_t u8MaxHops = 0;

/* States of Bootstrap process for EndDevice */
#define STATE_BOOT_NOT_JOINED 0x00
#define STATE_BOOT_SENT_FIRST_JOINING 0x01
#define STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE 0x02
#define STATE_BOOT_SENT_EAPPSK_SECOND_JOINING 0x04
#define STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE 0x08
#define STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING 0x10
#define STATE_BOOT_WAIT_ACCEPTED 0x20
#define STATE_BOOT_JOINED 0x40

/* EXT field types */
#define EAP_EXT_TYPE_CONFIGURATION_PARAMETERS 0x02

/* Seconds to wait for reser after Kick reception */
#define KICK_WAIT_RESET_SECONDS   2

/* LBP Callback types */
enum TLbpCallbackTypes {
	LBP_JOIN_CALLBACK = 0,
	LBP_LEAVE_CALLBACK,
	LBP_DUMMY_CALLBACK,
};

enum TLbpCallbackTypes lbpCallbackType;

/**********************************************************************************************************************/

/**
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
static void _SetBootState(uint8_t u8State)
{
	g_LbpContext.m_u8BootstrapState = u8State;

	LOG_DBG(Log("_SetBootState() Set state to %s Pending Confirms: %d",
			u8State == STATE_BOOT_NOT_JOINED ? "STATE_BOOT_NOT_JOINED" :
			u8State == STATE_BOOT_SENT_FIRST_JOINING ? "STATE_BOOT_SENT_FIRST_JOINING" :
			u8State == STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE ? "STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE" :
			u8State == STATE_BOOT_SENT_EAPPSK_SECOND_JOINING ? "STATE_BOOT_SENT_EAPPSK_SECOND_JOINING" :
			u8State == STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE ? "STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE" :
			u8State == STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING ? "STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING" :
			u8State == STATE_BOOT_WAIT_ACCEPTED ? "STATE_BOOT_WAIT_ACCEPTED" :
			u8State == STATE_BOOT_JOINED ? "STATE_BOOT_JOINED" : "?????",
			g_LbpContext.m_u8PendingConfirms
			));

	if (u8State == STATE_BOOT_NOT_JOINED) {
		g_LbpContext.m_u8PendingConfirms = 0;
	}
}

/**********************************************************************************************************************/

/**
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
static bool _IsBootState(uint8_t u8StateMask)
{
	return ((g_LbpContext.m_u8BootstrapState & u8StateMask) != 0) ||
	       (g_LbpContext.m_u8BootstrapState == u8StateMask); /* special case for NotJoined (== 0) */
}

/**********************************************************************************************************************/

/**
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
static bool _Joined(void)
{
	return (g_LbpContext.m_u16ShortAddress != 0xFFFF);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _ForceJoinStatus(bool bJoined)
{
	if (bJoined) {
		/* set back automate state */
		_SetBootState(STATE_BOOT_JOINED);
	} else {
		/* never should be here but check anyway */
		_SetBootState(STATE_BOOT_NOT_JOINED);
		g_LbpContext.m_u16ShortAddress = 0xFFFF;
		g_LbpContext.m_u16JoiningShortAddress = 0xFFFF;
	}
}

/**********************************************************************************************************************/

extern bool AdpMac_SetRcCoordSync(uint16_t u16RcCoord);
extern bool AdpMac_SecurityResetSync(void);
extern bool AdpMac_DeleteGroupMasterKeySync(uint8_t u8KeyId);
extern bool AdpMac_SetGroupMasterKeySync(const ADP_GROUP_MASTER_KEY *pMasterKey);
extern bool AdpMac_SetShortAddressSync(uint16_t u16ShortAddress);
extern bool AdpMac_GetExtendedAddressSync(ADP_EXTENDED_ADDRESS *pExtendedAddress);
extern bool AdpMac_SetExtendedAddressSync(const ADP_EXTENDED_ADDRESS *pExtendedAddress);
extern bool AdpMac_SetPanIdSync(uint16_t u16PanId);

/**
 ***********************************************************************************************************************
 * Local functions to get/set ADP PIBs
 **********************************************************************************************************************/
static uint16_t _GetCoordShortAddress(void)
{
	struct TAdpGetConfirm getConfirm;
	uint16_t u16CoordShortAddress = 0;
	AdpGetRequestSync(ADP_IB_COORD_SHORT_ADDRESS, 0, &getConfirm);
	if (getConfirm.m_u8Status == G3_SUCCESS) {
		memcpy(&u16CoordShortAddress, &getConfirm.m_au8AttributeValue, getConfirm.m_u8AttributeLength);
	}
	return u16CoordShortAddress;
}

static uint8_t _GetActiveKeyIndex(void)
{
	struct TAdpGetConfirm getConfirm;
	AdpGetRequestSync(ADP_IB_ACTIVE_KEY_INDEX, 0, &getConfirm);
	return getConfirm.m_au8AttributeValue[0];
}

static bool _SetPanId(uint16_t u16PanId)
{
	struct TAdpSetConfirm setConfirm;
	uint8_t au8PanIdArray[2];
	bool result = false;

	memcpy(au8PanIdArray, &u16PanId, sizeof(u16PanId));
	// Set on ADP
	AdpSetRequestSync(ADP_IB_MANUF_PAN_ID, 0, sizeof(u16PanId), au8PanIdArray, &setConfirm);
	if (setConfirm.m_u8Status == G3_SUCCESS) {
		// Set on MAC
		result = AdpMac_SetPanIdSync(u16PanId);
	}
	
	return result;
}

static bool _SetShortAddress(uint16_t u16ShortAddress)
{
	struct TAdpSetConfirm setConfirm;
	uint8_t au8ShortAddrArray[2];
	bool result = false;

	memcpy(au8ShortAddrArray, &u16ShortAddress, sizeof(u16ShortAddress));
	// Set on ADP
	AdpSetRequestSync(ADP_IB_MANUF_SHORT_ADDRESS, 0, sizeof(u16ShortAddress), au8ShortAddrArray, &setConfirm);
	if (setConfirm.m_u8Status == G3_SUCCESS) {
		// Set on MAC
		result = AdpMac_SetShortAddressSync(u16ShortAddress);
	}

	return result;
}

static bool _SetExtendedAddress(ADP_EXTENDED_ADDRESS *pEUI64Address)
{
	struct TAdpSetConfirm setConfirm;
	bool result = false;

	// Set on ADP
	AdpSetRequestSync(ADP_IB_MANUF_EXTENDED_ADDRESS, 0, sizeof(ADP_EXTENDED_ADDRESS), pEUI64Address->m_au8Value, &setConfirm);
	if (setConfirm.m_u8Status == G3_SUCCESS) {
		// Set on MAC
		result = AdpMac_SetExtendedAddressSync(pEUI64Address);
	}

	return result;
}

static void _SetActiveKeyIndex(uint8_t index)
{
	struct TAdpSetConfirm setConfirm;
	AdpSetRequestSync(ADP_IB_ACTIVE_KEY_INDEX, 0, sizeof(index), (uint8_t *)&index, &setConfirm);
}

static uint16_t _GetMaxJoinWaitTime(void)
{
	struct TAdpGetConfirm getConfirm;
	uint16_t u16MaxJoinWaitTime = 0;
	AdpGetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, &getConfirm);
	memcpy(&u16MaxJoinWaitTime, &getConfirm.m_au8AttributeValue, getConfirm.m_u8AttributeLength);
	return u16MaxJoinWaitTime;
}

static void _GetIdP(EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P *p_IdP)
{
	struct TAdpMacGetConfirm getConfirm;
	if (sIdP.m_u8Size == 0) {
		/* IdP not set, use Extended Address */
		AdpMacGetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, &getConfirm);
		p_IdP->m_u8Size = getConfirm.m_u8AttributeLength;
		memcpy(p_IdP->m_au8Value, getConfirm.m_au8AttributeValue, getConfirm.m_u8AttributeLength);
	}
	else {
		/* Use stored IdP */
		p_IdP->m_u8Size = sIdP.m_u8Size;
		memcpy(p_IdP->m_au8Value, sIdP.m_au8Value, sIdP.m_u8Size);
	}
}

static uint8_t _GetAdpMaxHops(void)
{
	struct TAdpGetConfirm getConfirm;
	AdpGetRequestSync(ADP_IB_MAX_HOPS, 0, &getConfirm);
	return getConfirm.m_au8AttributeValue[0];
}

static bool _SetDeviceTypeDev(void)
{
	struct TAdpSetConfirm setConfirm;
	uint8_t u8DevTpe = 0;
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
static void LBP_ClearEapContext(void)
{
	memset(&g_LbpContext.m_PskContext.m_Tek, 0,
			sizeof(g_LbpContext.m_PskContext.m_Tek));

	memset(&g_LbpContext.m_PskContext.m_IdS, 0,
			sizeof(g_LbpContext.m_PskContext.m_IdS));

	memset(&g_LbpContext.m_PskContext.m_RandP, 0,
			sizeof(g_LbpContext.m_PskContext.m_RandP));

	memset(&g_LbpContext.m_PskContext.m_RandS, 0,
			sizeof(g_LbpContext.m_PskContext.m_RandS));
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static bool _ProcessParameters(uint16_t u16DataLength, uint8_t *pData,
		uint8_t *pu8ReceivedParametersMask, uint8_t *pu8ParameterResultLength, uint8_t *pParameterResult)
{
	uint16_t u16Offset = 0;
	uint8_t u8AttrId = 0;
	uint8_t u8AttrLen = 0;
	uint8_t *pAttrValue = 0L;
	uint8_t u8Result = RESULT_PARAMETER_SUCCESS;
	uint8_t u8ParameterMask = 0;

	uint16_t u16ShortAddress = 0;
	ADP_GROUP_MASTER_KEY gmk[2];
	uint8_t u8KeyId;
	uint8_t u8ActiveKeyId = 0;
	uint8_t u8DeleteKeyId = 0;

	// Invalidate gmk indices to further tell which one is written
	gmk[0].keyId = UNSET_KEY;
	gmk[1].keyId = UNSET_KEY;

	/* bootstrapping data carries the configuration parameters: short address, gmk, gmk activation */
	/* decode and process configuration parameters */
	while ((u16Offset < u16DataLength) && (u8Result == RESULT_PARAMETER_SUCCESS)) {
		u8AttrId = pData[u16Offset++];
		u8AttrLen = pData[u16Offset++];
		pAttrValue = &pData[u16Offset];

		if (u16Offset + u8AttrLen <= u16DataLength) {
			/* interpret attribute */
			if (u8AttrId == CONF_PARAM_SHORT_ADDR) { /* Short_Addr */
				if (u8AttrLen == 2) {
					u16ShortAddress = (pAttrValue[0] << 8) | pAttrValue[1];
					u8ParameterMask |= CONF_PARAM_SHORT_ADDR_MASK;
				} else {
					u8Result = RESULT_INVALID_PARAMETER_VALUE;
				}
			} else if (u8AttrId == CONF_PARAM_GMK) { /* Provide a GMK key. On reception, the key is installed in the provided Key Identifier slot. */
				/* Constituted of the following fields: */
				/* id (1 byte): the Key Identifier of the GMK */
				/* gmk (16 bytes): the value of the current GMK */
				if (u8AttrLen == 17) {
					u8KeyId = pAttrValue[0];
					if ((u8KeyId == 0) || (u8KeyId == 1)) {
						gmk[u8KeyId].keyId = u8KeyId;
						memcpy(gmk[u8KeyId].key, &pAttrValue[1], 16);
						u8ParameterMask |= CONF_PARAM_GMK_MASK;
					}
					else {
						u8Result = RESULT_INVALID_PARAMETER_VALUE;
					}
				} else {
					u8Result = RESULT_INVALID_PARAMETER_VALUE;
				}
			} else if (u8AttrId == CONF_PARAM_GMK_ACTIVATION) { /* Indicate the GMK to use for outgoing messages */
				/* Constituted of the following field: */
				/* id (1 byte): the Key Identifier of the active GMK */
				if (u8AttrLen == 1) {
					/* GMK to activate cannot be unset */
					if ((g_LbpContext.m_GroupMasterKey[pAttrValue[0]].keyId == UNSET_KEY) &&
						(gmk[pAttrValue[0]].keyId == UNSET_KEY)) {
						u8Result = RESULT_INVALID_PARAMETER_VALUE;
					} else {
						u8ActiveKeyId = pAttrValue[0];
						u8ParameterMask |= CONF_PARAM_GMK_ACTIVATION_MASK;
					}
				} else {
					u8Result = RESULT_INVALID_PARAMETER_VALUE;
				}
			} else if (u8AttrId == CONF_PARAM_GMK_REMOVAL) { /* Indicate a GMK to delete. */
				/* Constituted of the following field: */
				/* id (1 byte): the Key Identifier of the GMK to delete */
				if (u8AttrLen == 1) {
					u8DeleteKeyId = pAttrValue[0];
					u8ParameterMask |= CONF_PARAM_GMK_REMOVAL_MASK;
				} else {
					u8Result = RESULT_INVALID_PARAMETER_VALUE;
				}
			} else {
				LOG_ERR(Log("_ProcessParameters() Unsupported attribute id %u, len %u", u8AttrId, u8AttrLen));
				u8Result = RESULT_UNKNOWN_PARAMETER_ID;
			}
		} else {
			u8Result = RESULT_INVALID_PARAMETER_VALUE;
		}

		u16Offset += u8AttrLen;
	}

	if (u8Result == RESULT_PARAMETER_SUCCESS) {
		/* verify the validity of parameters */
		bool bParamsValid = true;
		u8AttrId = 0;
		if (!_Joined()) {
			/* if device not joined yet, the following parameters are mandatory: CONF_PARAM_SHORT_ADDR_MASK, CONF_PARAM_GMK_MASK, CONF_PARAM_GMK_ACTIVATION_MASK */
			if ((u8ParameterMask & CONF_PARAM_SHORT_ADDR_MASK) != CONF_PARAM_SHORT_ADDR_MASK) {
				u8AttrId = CONF_PARAM_SHORT_ADDR;
			} else if ((u8ParameterMask & CONF_PARAM_GMK_MASK) != CONF_PARAM_GMK_MASK) {
				u8AttrId = CONF_PARAM_GMK;
			} else if ((u8ParameterMask & CONF_PARAM_GMK_ACTIVATION_MASK) != CONF_PARAM_GMK_ACTIVATION_MASK) {
				u8AttrId = CONF_PARAM_GMK_ACTIVATION;
			}
		} else {
			/* if device already joined, the message should contain one of the following parameters: CONF_PARAM_GMK_MASK, CONF_PARAM_GMK_ACTIVATION_MASK, CONF_PARAM_GMK_REMOVAL_MASK */
			if ((u8ParameterMask & (CONF_PARAM_GMK_MASK | CONF_PARAM_GMK_ACTIVATION_MASK | CONF_PARAM_GMK_REMOVAL_MASK)) != 0) {
				/* one of required parameters has been found; nothing to do */
			} else {
				/* no required parameters was received; just send back an error related to GMK-ACTIVATION (as missing parameters) */
				u8AttrId = CONF_PARAM_GMK_ACTIVATION;
			}
		}

		bParamsValid = (u8AttrId == 0);

		if (!bParamsValid) {
			u8Result = RESULT_MISSING_REQUIRED_PARAMETER;
		} else {
			*pu8ReceivedParametersMask = u8ParameterMask;
			if ((u8ParameterMask & CONF_PARAM_SHORT_ADDR_MASK) == CONF_PARAM_SHORT_ADDR_MASK) {
				/* short address will be set only after receiving the EAP-Success message */
				g_LbpContext.m_u16JoiningShortAddress = u16ShortAddress;
				LOG_DBG(
						Log("_ProcessParameters() ShortAddress %04X", g_LbpContext.m_u16JoiningShortAddress));
			}

			if ((u8ParameterMask & CONF_PARAM_GMK_MASK) == CONF_PARAM_GMK_MASK) {
				if (gmk[0].keyId != UNSET_KEY) {
					g_LbpContext.m_GroupMasterKey[0] = gmk[0];
					LOG_DBG(
						LogBuffer(g_LbpContext.m_GroupMasterKey[0].key, 16, "_ProcessParameters() New GMK (id=%u) ", g_LbpContext.m_GroupMasterKey[0].keyId));
				}
				if (gmk[1].keyId != UNSET_KEY) {
					g_LbpContext.m_GroupMasterKey[1] = gmk[1];
					LOG_DBG(
						LogBuffer(g_LbpContext.m_GroupMasterKey[1].key, 16, "_ProcessParameters() New GMK (id=%u) ", g_LbpContext.m_GroupMasterKey[1].keyId));
				}
			}

			if ((u8ParameterMask & CONF_PARAM_GMK_ACTIVATION_MASK) == CONF_PARAM_GMK_ACTIVATION_MASK) {
				if (_GetActiveKeyIndex() != u8ActiveKeyId) {
					/*  On reception of the GMK-activation parameter, the peer shall empty its DeviceTable (in order to allow re-use of previously allocated short addresses). */
					LOG_DBG(Log("_ProcessParameters() Key-id changed; Reset device table"));
					AdpMac_SecurityResetSync();
				}

				_SetActiveKeyIndex(u8ActiveKeyId);
				LOG_DBG(
						Log("_ProcessParameters() Active GMK id: %u", u8ActiveKeyId));
			}

			if ((u8ParameterMask & CONF_PARAM_GMK_REMOVAL_MASK) == CONF_PARAM_GMK_REMOVAL_MASK) {
				if (AdpMac_DeleteGroupMasterKeySync(u8DeleteKeyId)) {
					/* Mark key as deleted */
					g_LbpContext.m_GroupMasterKey[u8DeleteKeyId].keyId = UNSET_KEY;
					LOG_ERR(Log("_ProcessParameters() GMK id %u was deleted!", u8DeleteKeyId));
				} else {
					LOG_ERR(Log("_ProcessParameters() Cannot delete GMK id: %u!", u8DeleteKeyId));
				}
			}
		}
	}

	/* prepare p-result */
	/* If one or more parameter were invalid or missing, the peer send a message 4 with R = DONE_FAILURE and */
	/* an embedded configuration field with at least one Parameter-result indicating the error. */
	pParameterResult[(*pu8ParameterResultLength)++] = CONF_PARAM_RESULT;
	pParameterResult[(*pu8ParameterResultLength)++] = 2;
	pParameterResult[(*pu8ParameterResultLength)++] = u8Result;
	pParameterResult[(*pu8ParameterResultLength)++] = (u8Result == RESULT_PARAMETER_SUCCESS) ? 0 : u8AttrId;

	return (u8Result == RESULT_PARAMETER_SUCCESS);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Rekey_TimerExpired_Callback(struct TTimer *pTimer)
{
	LOG_DBG(Log("_Rekey_TimerExpired_Callback"));
	UNUSED(pTimer);
	LBP_ClearEapContext();
	if (_Joined()) {
		/* set back automate state */
		_ForceJoinStatus(true);
	} else {
		/* never should be here but check anyway */
		_ForceJoinStatus(false);
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Join_Confirm(uint8_t u8Status)
{
	uint8_t u8MediaType;
	struct TAdpNetworkJoinConfirm joinConfirm;

	LOG_INFO(Log("_Join_Confirm() Status: %u", u8Status));
	Timer_Unregister(&g_LbpContext.m_JoinTimer);
	LBP_ClearEapContext();

	if (u8Status == G3_SUCCESS) {
		_SetBootState(STATE_BOOT_JOINED);
		LOG_INFO(Log("NetworkJoin() JOIN FINISHED SUCCESSFULLY! PAN-ID:0x%04X SHORT-ADDRESS:0x%04X", g_LbpContext.m_u16PanId,
		g_LbpContext.m_u16ShortAddress));

		if ((!Routing_IsDisabled()) && Routing_AdpDefaultCoordRouteEnabled()) {
			bool bTableFull;
			if ((g_LbpContext.m_u8MediaType == MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC) ||
				(g_LbpContext.m_u8MediaType == MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP)) {
				u8MediaType = 1;
			}
			else {
				u8MediaType = 0;
			}
			Routing_AddRoute(_GetCoordShortAddress(), g_LbpContext.m_u16LbaAddress, u8MediaType, &bTableFull);
		}
	} else {
		_SetBootState(STATE_BOOT_NOT_JOINED);
		g_LbpContext.m_u16ShortAddress = 0xFFFF;
		g_LbpContext.m_u16PanId = 0xFFFF;
		_SetShortAddress(0xFFFF);
		_SetPanId(0xFFFF);
	}

	if (g_LbpContext.m_LbpNotifications.joinConfirm) {
		joinConfirm.m_u8Status = u8Status;
		joinConfirm.m_u16NetworkAddress = g_LbpContext.m_u16ShortAddress;
		joinConfirm.m_u16PanId = g_LbpContext.m_u16PanId;
		g_LbpContext.m_LbpNotifications.joinConfirm(&joinConfirm);
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Join_TimerExpired_Callback(struct TTimer *pTimer)
{
	LOG_DBG(Log("NetworkJoin_TimerExpired_Callback"));
	UNUSED(pTimer);
	_Join_Confirm(G3_TIMEOUT);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Join_Callback_DataSend(uint8_t u8Status)
{
	g_LbpContext.m_u8PendingConfirms--;
	if (u8Status == G3_SUCCESS) {
		if (g_LbpContext.m_u8PendingConfirms == 0) {
			/* / message successfully sent: Update state and wait for response or timeout */
			if (_IsBootState(STATE_BOOT_SENT_FIRST_JOINING)) {
				_SetBootState(STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE);
			} else if (_IsBootState(STATE_BOOT_SENT_EAPPSK_SECOND_JOINING)) {
				_SetBootState(STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE);
			} else if (_IsBootState(STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING)) {
				_SetBootState(STATE_BOOT_WAIT_ACCEPTED);
			}
		}
	} else {
		_Join_Confirm(u8Status);
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Kick_TimerExpired_Callback(struct TTimer *pTimer)
{
	// Reset Stack
	LOG_DBG(Log("Kicked; Reseting Stack.."));
	UNUSED(pTimer);

	AdpResetRequest();
	_SetBootState(STATE_BOOT_NOT_JOINED);
	g_LbpContext.m_u16ShortAddress = 0xFFFF;
	g_LbpContext.m_u16PanId = 0xFFFF;
	_SetShortAddress(0xFFFF);
	_SetPanId(0xFFFF);

	if (g_LbpContext.m_LbpNotifications.leaveIndication != NULL) {
		g_LbpContext.m_LbpNotifications.leaveIndication();
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _Kick_Notify(void)
{
	// We have been kicked out of the network
	LOG_DBG(Log("Device get KICKed off the network by the coordinator"));

	// arm a timer and reset the mac after a few milliseconds in order to give time to MAC/PHY to send the ACK
	// related to the Kick message
	g_LbpContext.m_KickTimer.m_fnctCallback = _Kick_TimerExpired_Callback;

	Timer_Register(&g_LbpContext.m_KickTimer, KICK_WAIT_RESET_SECONDS * 10);
}

/**********************************************************************************************************************
*
**********************************************************************************************************************/
static void _Leave_Callback(uint8_t u8Status)
{
	struct TAdpNetworkLeaveConfirm leaveConfirm;

	// Whatever the result, Reset Stack
	AdpResetRequest();
	_SetBootState(STATE_BOOT_NOT_JOINED);
	g_LbpContext.m_u16ShortAddress = 0xFFFF;
	g_LbpContext.m_u16PanId = 0xFFFF;
	_SetShortAddress(0xFFFF);
	_SetPanId(0xFFFF);
	
	if (g_LbpContext.m_LbpNotifications.leaveConfirm) {
		leaveConfirm.m_u8Status = u8Status;
		g_LbpContext.m_LbpNotifications.leaveConfirm(&leaveConfirm);
	}
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called internally (from this file) when the first EAP-PSK message is received
 *
 **********************************************************************************************************************/
static void _Join_Process_Challenge_FirstMessage(const ADP_EXTENDED_ADDRESS *pEUI64Address,
		uint8_t u8EAPIdentifier, uint16_t u16EAPDataLength, uint8_t *pEAPData)
{
	EAP_PSK_RAND randS;
	EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S idS;
	EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P idP;
	_GetIdP(&idP);

	/* In order to detect if is a valid repetition we have to check the 2 elements carried by the first EAP-PSK message: RandS and IdS */
	/* In case of a valid repetition we have to send back the same response message */
	if (EAP_PSK_Decode_Message1(u16EAPDataLength, pEAPData, &randS, &idS)) {
		struct TAdpAddress dstAddr;

		/* encode and send the message T1 */
		uint8_t *pMemoryBuffer = au8LbpBuffer;
		uint16_t u16MemoryBufferLength = sizeof(au8LbpBuffer);
		uint16_t u16RequestLength = 0;

		bool bRepetition = (memcmp(randS.m_au8Value,
				g_LbpContext.m_PskContext.m_RandS.m_au8Value,
				sizeof(randS.m_au8Value)) == 0) &&
				(memcmp(idS.m_au8Value,
				g_LbpContext.m_PskContext.m_IdS.m_au8Value,
				g_LbpContext.m_PskContext.m_IdS.m_u8Size) == 0);

		if (!bRepetition) {
			/* save current values; needed to detect repetitions */
			memcpy(g_LbpContext.m_PskContext.m_RandS.m_au8Value, randS.m_au8Value, sizeof(randS.m_au8Value));
			memcpy(g_LbpContext.m_PskContext.m_IdS.m_au8Value, idS.m_au8Value, idS.m_u8Size);
			g_LbpContext.m_PskContext.m_IdS.m_u8Size = idS.m_u8Size;

			/* process RandP */
			/* use the value from MIB if set by user */
			if (memcmp(&randP,
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) != 0) {
				memcpy(g_LbpContext.m_PskContext.m_RandP.m_au8Value,
						&randP, sizeof(EAP_PSK_RAND));
			} else {
				/* initialize RandP with random content */
				Random128(g_LbpContext.m_PskContext.m_RandP.m_au8Value);
			}

			EAP_PSK_InitializeTEKMSK(&g_LbpContext.m_PskContext.m_RandP,
					&g_LbpContext.m_PskContext);
		}

		u16RequestLength = EAP_PSK_Encode_Message2(&g_LbpContext.m_PskContext, u8EAPIdentifier,
				&randS, &g_LbpContext.m_PskContext.m_RandP,
				&g_LbpContext.m_PskContext.m_IdS, &idP,
				u16MemoryBufferLength, pMemoryBuffer);

		/* Encode now the LBP message */
		u16RequestLength = LBP_Encode_JoiningRequest(pEUI64Address, g_LbpContext.m_u8MediaType, u16RequestLength, u16MemoryBufferLength, pMemoryBuffer);

		dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
		dstAddr.m_u16ShortAddr = g_LbpContext.m_u16LbaAddress;

		/* if this is a rekey procedure (instead of a join) arm here the timer in order to control the rekey; */
		/* messages can be loss and we want to clean the context after a while */
		if (_Joined()) {
			/* start rekey timer */
			g_LbpContext.m_JoinTimer.m_fnctCallback = _Rekey_TimerExpired_Callback;
			Timer_Register(&g_LbpContext.m_JoinTimer, _GetMaxJoinWaitTime() * 10);
		}

		g_LbpContext.m_u8PendingConfirms++;
		_SetBootState(STATE_BOOT_SENT_EAPPSK_SECOND_JOINING);
		lbpCallbackType = LBP_JOIN_CALLBACK;
		AdpLbpRequest(&dstAddr, u16RequestLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
	} else {
		LOG_ERR(Log("ADPM_Network_Join_Request() Invalid server response: EAP_PSK_Decode_Message1"));

		/* Wait for NetworkJoin timeout */
	}
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called internally (from this file) when the third EAP-PSK message is received
 *
 **********************************************************************************************************************/
static void _Join_Process_Challenge_ThirdMessage(uint16_t u16HeaderLength, uint8_t *pHeader,
		const ADP_EXTENDED_ADDRESS *pEUI64Address, uint8_t u8EAPIdentifier, uint16_t u16EAPDataLength,
		uint8_t *pEAPData)
{
	EAP_PSK_RAND randS;
	uint8_t u8PChannelResult = 0;
	uint32_t u32Nonce = 0;
	uint16_t u16PChannelDataLength = 0;
	uint8_t *pPChannelData = 0L;
	UNUSED(pEUI64Address);

	/* If rand_S does not meet with the one that identifies the */
	if (EAP_PSK_Decode_Message3(u16EAPDataLength, pEAPData, &g_LbpContext.m_PskContext,
			u16HeaderLength, pHeader, &randS, &u32Nonce, &u8PChannelResult, &u16PChannelDataLength, &pPChannelData) &&
			(u8PChannelResult == PCHANNEL_RESULT_DONE_SUCCESS)) {
		if (memcmp(randS.m_au8Value, g_LbpContext.m_PskContext.m_RandS.m_au8Value, sizeof(randS.m_au8Value)) == 0) {
			/* encode and send the message T4 */
			struct TAdpAddress dstAddr;

			uint8_t *pMemoryBuffer = au8LbpBuffer;
			uint16_t u16MemoryBufferLength = sizeof(au8LbpBuffer);
			uint16_t u16RequestLength = 0;
			uint8_t u8ReceivedParameters = 0;
			uint8_t au8ParameterResult[10]; /* buffer to encode the parameter result */
			uint8_t u8ParameterResultLength = 0;
			u8PChannelResult = PCHANNEL_RESULT_DONE_FAILURE;

			/* ParameterResult carry config parameters */
			au8ParameterResult[0] = EAP_EXT_TYPE_CONFIGURATION_PARAMETERS;
			u8ParameterResultLength++;

			/* If one or more parameter were invalid or missing, the peer send a message 4 with R = DONE_FAILURE and */
			/* an embedded configuration field with at least one Parameter-result indicating the error. */

			/* check if protected data carries EXT field */
			if (pPChannelData[0] == EAP_EXT_TYPE_CONFIGURATION_PARAMETERS) {
				if (_ProcessParameters(u16PChannelDataLength - 1, &pPChannelData[1],
						&u8ReceivedParameters, &u8ParameterResultLength, au8ParameterResult)) {
					u8PChannelResult = PCHANNEL_RESULT_DONE_SUCCESS;
				}
			} else {
				/* build ParameterResult indicating missing GMK key */
				au8ParameterResult[u8ParameterResultLength++] = CONF_PARAM_RESULT;
				au8ParameterResult[u8ParameterResultLength++] = 2;
				au8ParameterResult[u8ParameterResultLength++] = RESULT_MISSING_REQUIRED_PARAMETER;
				au8ParameterResult[u8ParameterResultLength++] = CONF_PARAM_GMK;
			}

			/* after receiving from the server a valid EAP-PSK message with Nonce */
			/* set to N, the peer will answer with an EAP-PSK message with Nonce set to N+1 */
			u16RequestLength = EAP_PSK_Encode_Message4(&g_LbpContext.m_PskContext, u8EAPIdentifier,
					&randS, u32Nonce + 1, u8PChannelResult, u8ParameterResultLength, au8ParameterResult, u16MemoryBufferLength,
					pMemoryBuffer);

			/* Encode now the LBP message */
			u16RequestLength = LBP_Encode_JoiningRequest(&g_LbpContext.m_EUI64Address, g_LbpContext.m_u8MediaType,
					u16RequestLength, u16MemoryBufferLength, pMemoryBuffer);

			dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
			dstAddr.m_u16ShortAddr = g_LbpContext.m_u16LbaAddress;

			if (u8PChannelResult != PCHANNEL_RESULT_DONE_SUCCESS) {
				LOG_ERR(Log("ADPM_Network_Join_Request() EAP_PSK_Decode_Message3() Invalid parameters"));
				/* wait for timeout */
			}

			g_LbpContext.m_u8PendingConfirms++;
			_SetBootState(STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING);
			lbpCallbackType = LBP_JOIN_CALLBACK;
			AdpLbpRequest(&dstAddr, u16RequestLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
		} else {
			LOG_ERR(Log("ADPM_Network_Join_Request() EAP_PSK_Decode_Message3()"));
			/* wait for timeout */
		}
	} else {
		LOG_ERR(Log("ADPM_Network_Join_Request() EAP_PSK_Decode_Message3()"));
		/* wait for timeout */
	}
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called by the 6LoWPANProtocol when a LBP Challenge message is received
 *
 **********************************************************************************************************************/
static void _Join_Process_Challenge(const ADP_EXTENDED_ADDRESS *pEUI64Address,
		uint16_t u16BootStrappingDataLength, uint8_t *pBootStrappingData)
{
	uint8_t u8Code = 0;
	uint8_t u8Identifier = 0;
	uint8_t u8TSubfield = 0;
	uint16_t u16EAPDataLength = 0;
	uint8_t *pEAPData = 0L;

	if (EAP_PSK_Decode_Message(u16BootStrappingDataLength, pBootStrappingData, &u8Code, &u8Identifier, &u8TSubfield,
			&u16EAPDataLength, &pEAPData)) {
		/* the Challenge is always a Request coming from the LBS */
		if (u8Code == EAP_REQUEST) {
			/* Also only 2 kind of EAP messages are accepted as request: first and third message */
			if (u8TSubfield == EAP_PSK_T0) {
				/* this message can be received in the following states: */
				/* - STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE: */
				/*    as normal bootstrap procedure */
				/* - STATE_BOOT_SENT_FIRST_JOINING or STATE_BOOT_SENT_EAPPSK_SECOND_JOINING or STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING */
				/*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
				/*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
				/*    original Response without reprocessing the Request. */
				/* - STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE: */
				/*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
				/*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
				/*    original Response without reprocessing the Request. */
				/* - STATE_BOOT_JOINED: during rekey procedure */
				if (_IsBootState(STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE | STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE | STATE_BOOT_JOINED | STATE_BOOT_SENT_FIRST_JOINING | STATE_BOOT_SENT_EAPPSK_SECOND_JOINING |
						STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING)) {
					/* enforce the current state in order to correctly update state machine after sending this message */
					_SetBootState(STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE);
					/* the function is able to detect a valid repetition */
					_Join_Process_Challenge_FirstMessage(pEUI64Address, u8Identifier, u16EAPDataLength, pEAPData);
				} else {
					LOG_ERR(Log("_Join_Process_Challenge() Drop unexpected CHALLENGE_1"));
				}
			} else if (u8TSubfield == EAP_PSK_T2) {
				/* this message can be received in the following states: */
				/* - STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE: */
				/*    as normal bootstrap procedure */
				/* - STATE_BOOT_WAIT_ACCEPTED or  STATE_BOOT_SENT_EAPPSK_SECOND_JOINING or STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING: */
				/*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
				/*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
				/*    original Response without reprocessing the Request. */
				if (_IsBootState(STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE | STATE_BOOT_WAIT_ACCEPTED | STATE_BOOT_SENT_EAPPSK_SECOND_JOINING | STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING)) {
					/* enforce the current state in order to correctly update state machine after sending this message */
					_SetBootState(STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE);
					/* hardcoded length of 22 bytes representing: the first 22 bytes of the EAP Request or Response packet used to compute the auth tag */
					_Join_Process_Challenge_ThirdMessage(22, pBootStrappingData, pEUI64Address, u8Identifier, u16EAPDataLength, pEAPData);
				} else {
					LOG_ERR(Log("_Join_Process_Challenge() Drop unexpected CHALLENGE_2"));
				}
			}

			/* else invalid message */
		}

		/* else invalid message */
	}

	/* else decode error */
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called by the 6LoWPANProtocol when a LBP Accepted embedding an EAP message is received
 *
 **********************************************************************************************************************/
static void _Join_Process_Accepted_EAP(const ADP_EXTENDED_ADDRESS *pEUI64Address,
		uint16_t u16BootStrappingDataLength, uint8_t *pBootStrappingData)
{
	uint8_t u8Code = 0;
	uint8_t u8Identifier = 0;
	uint8_t u8TSubfield = 0;
	uint16_t u16EAPDataLength = 0;
	uint8_t *pEAPData = 0L;
	uint8_t u8ActiveKeyId;
	bool bSetResult = false;
	UNUSED(pEUI64Address);

	if (EAP_PSK_Decode_Message(u16BootStrappingDataLength, pBootStrappingData, &u8Code, &u8Identifier, &u8TSubfield,
			&u16EAPDataLength, &pEAPData)) {
		if (u8Code == EAP_SUCCESS) {
			/* set the encryption key into mac layer */
			u8ActiveKeyId = _GetActiveKeyIndex();
			LOG_INFO(
				Log("NetworkJoin() Join / rekey process finish: Set the GMK encryption key %u into the MAC layer", g_LbpContext.m_GroupMasterKey[u8ActiveKeyId].keyId));
			// Check whether one or two keys have been received
			if ((g_LbpContext.m_GroupMasterKey[0].keyId != UNSET_KEY) &&
				(g_LbpContext.m_GroupMasterKey[1].keyId != UNSET_KEY)) {
				// Both GMKs have to be set, acive index the second
				if (u8ActiveKeyId == 0) {
					bSetResult = AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[1]);
					bSetResult &= AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[0]);
				}
				else {
					bSetResult = AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[0]);
					bSetResult &= AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[1]);
				}
			}
			else if (g_LbpContext.m_GroupMasterKey[0].keyId != UNSET_KEY) {
				// GMK 0 has to be set
				bSetResult = AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[0]);
			}
			else if (g_LbpContext.m_GroupMasterKey[1].keyId != UNSET_KEY) {
				// GMK 1 has to be set
				bSetResult = AdpMac_SetGroupMasterKeySync(&g_LbpContext.m_GroupMasterKey[1]);
			}
			if (bSetResult) {
				LOG_DBG(
						Log("NetworkJoin() Join / rekey process finish: Active key id is %u", _GetActiveKeyIndex()));
				/* If joining the network (not yet joined) we have to do more initialisation */
				if (!_Joined()) {
					/* A device shall initialise RC_COORD to 0x7FFF on association. */
					LOG_DBG(Log("NetworkJoin() Join process finish: Set the RcCoord into the MAC layer"));
					if (AdpMac_SetRcCoordSync(0x7FFF)) {
						/* set the short address in the mac layer */
						LOG_DBG(Log("NetworkJoin() Join process finish: Set the ShortAddress 0x%04X into the MAC layer", g_LbpContext.m_u16JoiningShortAddress));

						if (_SetShortAddress(g_LbpContext.m_u16JoiningShortAddress)) {
							g_LbpContext.m_u16ShortAddress = g_LbpContext.m_u16JoiningShortAddress;
							_Join_Confirm(G3_SUCCESS);
						} else {
							_Join_Confirm(G3_FAILED);
						}
					} else {
						_Join_Confirm(G3_FAILED);
					}
				} else {
					/* already joined; this was the rekey procedure; set the state to STATE_BOOT_JOINED */
					_SetBootState(STATE_BOOT_JOINED);
					Timer_Unregister(&g_LbpContext.m_JoinTimer);
				}
			}
		} else {
			LOG_ERR(Log("ADPM_Network_Join_Request() u8Code != EAP_SUCCESS"));
			_Join_Confirm(G3_NOT_PERMITED);
		}
	} else {
		LOG_ERR(Log("ADPM_Network_Join_Request() EAP_PSK_Decode_Message"));

		/* Wait for NetworkJoin timeout */
	}
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called by the 6LoWPANProtocol when a LBP Accepted embedding a Configuration message is received
 *
 **********************************************************************************************************************/
static void _Join_Process_Accepted_Configuration(const ADP_EXTENDED_ADDRESS *pEUI64Address,
		uint16_t u16BootStrappingDataLength, uint8_t *pBootStrappingData)
{
	UNUSED(pEUI64Address);

	/* this must be rekey or key removal */
	/* TODO: we are now supporting only key activation, handle also other parameters */
	if (_Joined()) {
		struct TAdpAddress dstAddr;

		uint8_t *pMemoryBuffer = au8LbpBuffer;
		uint16_t u16MemoryBufferLength = sizeof(au8LbpBuffer);
		uint16_t u16RequestLength = 0;

		uint8_t u8ReceivedParametersMask = 0;
		uint8_t au8ParameterResult[10];
		uint8_t u8ParameterResultLength = 0;

		_ProcessParameters(u16BootStrappingDataLength, pBootStrappingData,
				&u8ReceivedParametersMask, &u8ParameterResultLength, au8ParameterResult);

		memcpy(pMemoryBuffer, au8ParameterResult, u8ParameterResultLength);

		/* Encode now the LBP message */
		u16RequestLength = LBP_Encode_JoiningRequest(&g_LbpContext.m_EUI64Address, g_LbpContext.m_u8MediaType,
				u8ParameterResultLength, u16MemoryBufferLength, pMemoryBuffer);

		dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
		dstAddr.m_u16ShortAddr = g_LbpContext.m_u16LbaAddress;

		lbpCallbackType = LBP_DUMMY_CALLBACK;
		AdpLbpRequest(&dstAddr, u16RequestLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
	}

	/* if not joined, ignore this message */
}

/**********************************************************************************************************************/

/** Network joining
 *
 ***********************************************************************************************************************
 *
 * This function is called by the 6LoWPANProtocol when a LBP Accepted message is received
 *
 * Accepted message can embed: EAP-Success or Configuration parameters
 *
 **********************************************************************************************************************/
static void _Join_Process_Accepted(const ADP_EXTENDED_ADDRESS *pEUI64Address,
		uint16_t u16BootStrappingDataLength, uint8_t *pBootStrappingData)
{
	/* check the first byte of the Bootstrapping data in order to detect */
	/* the type of the embedded message: EAP or Configuration */
	if ((pBootStrappingData[0] & 0x01) == 0x00) {
		/* EAP message */
		/* check state: this message can be received also when JOINED for re-key procedure */
		if (_IsBootState(STATE_BOOT_WAIT_ACCEPTED) || _IsBootState(STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING)) {
			_Join_Process_Accepted_EAP(pEUI64Address, u16BootStrappingDataLength, pBootStrappingData);
		} else {
			LOG_ERR(Log("_Join_Process_Accepted() Drop unexpected Accepted_EAP"));
		}
	} else {
		/* Configuration message */
		if (_IsBootState(STATE_BOOT_JOINED)) {
			_Join_Process_Accepted_Configuration(pEUI64Address, u16BootStrappingDataLength, pBootStrappingData);
		} else {
			LOG_ERR(Log("_Join_Process_Accepted() Drop unexpected Accepted_Configuration"));
		}
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _JoinRequest(void)
{
	struct TAdpAddress dstAddr;

	/* we have to send the Joining request */
	uint8_t *pMemoryBuffer = au8LbpBuffer;
	uint16_t u16MemoryBufferLength = sizeof(au8LbpBuffer);
	uint16_t u16RequestLength = 0;

	/* Reset Joining short address */
	g_LbpContext.m_u16JoiningShortAddress = 0xFFFF;

	/* prepare and send the JoinRequest; no Bootstrapping data for the first request */
	u16RequestLength = LBP_Encode_JoiningRequest(&g_LbpContext.m_EUI64Address, g_LbpContext.m_u8MediaType, 0, /* u16BootStrappingDataLength */
			u16MemoryBufferLength, pMemoryBuffer);

	dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
	dstAddr.m_u16ShortAddr = g_LbpContext.m_u16LbaAddress;

	LOG_INFO(Log("Registering Network-JOIN timer: %u seconds", _GetMaxJoinWaitTime()));

	/* start MaxJoinWait timer */
	g_LbpContext.m_JoinTimer.m_fnctCallback = _Join_TimerExpired_Callback;

	Timer_Register(&g_LbpContext.m_JoinTimer, _GetMaxJoinWaitTime() * 10);

	g_LbpContext.m_u8PendingConfirms++;
	_SetBootState(STATE_BOOT_SENT_FIRST_JOINING);
	lbpCallbackType = LBP_JOIN_CALLBACK;
	AdpLbpRequest(&dstAddr, u16RequestLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _LeaveRequest(const ADP_EXTENDED_ADDRESS *pEUI64Address)
{
	struct TAdpAddress dstAddr;

	uint8_t *pMemoryBuffer = au8LbpBuffer;
	uint16_t u16MemoryBufferLength = sizeof(au8LbpBuffer);
	uint16_t u16RequestLength = 0;

	u16RequestLength = LBP_Encode_KickFromLBDRequest(pEUI64Address, u16MemoryBufferLength, pMemoryBuffer);

	dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
	dstAddr.m_u16ShortAddr = _GetCoordShortAddress();

	lbpCallbackType = LBP_LEAVE_CALLBACK;
	AdpLbpRequest(&dstAddr, u16RequestLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void _ForceJoined(uint16_t u16ShortAddress, uint16_t u16PanId, ADP_EXTENDED_ADDRESS *pEUI64Address)
{
	struct TAdpNetworkJoinConfirm joinConfirm;

	g_LbpContext.m_u16ShortAddress = u16ShortAddress;
	g_LbpContext.m_u16PanId = u16PanId;
	memcpy(&g_LbpContext.m_EUI64Address, pEUI64Address, sizeof(ADP_EXTENDED_ADDRESS));
	_SetBootState(STATE_BOOT_JOINED);
	if (g_LbpContext.m_LbpNotifications.joinConfirm) {
		joinConfirm.m_u8Status = G3_SUCCESS;
		joinConfirm.m_u16NetworkAddress = u16ShortAddress;
		joinConfirm.m_u16PanId = u16PanId;
		g_LbpContext.m_LbpNotifications.joinConfirm(&joinConfirm);
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void AdpLbpConfirmDev(struct TAdpLbpConfirm *pLbpConfirm)
{
	(void)pLbpConfirm;
	if (lbpCallbackType == LBP_JOIN_CALLBACK) {
		_Join_Callback_DataSend(pLbpConfirm->m_u8Status);
	}
	else if (lbpCallbackType == LBP_LEAVE_CALLBACK) {
		_Leave_Callback(pLbpConfirm->m_u8Status);
	}
	else {
		/* Do nothing */
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
static void AdpLbpIndicationDev(struct TAdpLbpIndication *pLbpIndication)
{
	/* The coordinator will never get here as is handled by the application using LDB messages */
	uint8_t pu8MessageType = 0;
	ADP_EXTENDED_ADDRESS eui64Address;
	uint16_t u16BootStrappingDataLength = 0;
	uint8_t *pBootStrappingData = 0L;

	if (LBP_Decode_Message(pLbpIndication->m_u16NsduLength, pLbpIndication->m_pNsdu, &pu8MessageType, &eui64Address, &u16BootStrappingDataLength,
			&pBootStrappingData)) {
		/* if we are not the coordinator and we are in the network and this bootstrap message is not for us */
		if (memcmp(&eui64Address, &g_LbpContext.m_EUI64Address,
				sizeof(ADP_EXTENDED_ADDRESS)) != 0) {
			if (_Joined()) {
				/* LBA (agent): forward the message between server and device */
				struct TAdpAddress dstAddr;
				uint8_t *pMemoryBuffer = au8LbpBuffer;

				/* message should be relayed to the server or to the device */
				if (pu8MessageType == LBP_JOINING) {
					/* Check Src Address matches the one in LBP frame */
					if (memcmp(&eui64Address, &pLbpIndication->m_pSrcAddr->m_ExtendedAddress,
							sizeof(ADP_EXTENDED_ADDRESS)) == 0) {
						/* Check frame coming from LBD is not secured */
						if (pLbpIndication->m_u8SecurityLevel == MAC_WRP_SECURITY_LEVEL_NONE) {
							/* relay to the server */
							dstAddr.m_u8AddrSize = ADP_ADDRESS_16BITS;
							dstAddr.m_u16ShortAddr = _GetCoordShortAddress();
						} else {
							LOG_DBG(Log("Secured frame coming from LBD; Drop frame"));
							return;
						}
					} else {
						LOG_DBG(Log("Source Address different from address in LBP frame; Drop frame"));
						return;
					}
				} else if ((pu8MessageType == LBP_ACCEPTED) || (pu8MessageType == LBP_CHALLENGE)
					 || (pu8MessageType == LBP_DECLINE)) {
					/* Frame must come from a short address between 0x0000 and 0x7FFF */
					if ((pLbpIndication->m_pSrcAddr->m_u8AddrSize == ADP_ADDRESS_16BITS) && (pLbpIndication->m_pSrcAddr->m_u16ShortAddr < 0x8000)) {
						/* Frame must be secured */
						/* Security level already checked for frames between 16-bit addresses */
						/* relay to the device */
						dstAddr.m_u8AddrSize = ADP_ADDRESS_64BITS;
						memcpy(&dstAddr.m_ExtendedAddress, &eui64Address, 8);
					} else {
						LOG_DBG(Log("Invalid Source Address on frame to LBD; Drop frame"));
						return;
					}
				} else {
					/* 'T' bit and 'Code' not matching a valid frame (e.g. Joining to LBD or Challenge from LBD) */
					LOG_DBG(Log("T and CODE fields mismatch; Drop frame"));
					return;
				}

				memcpy(pMemoryBuffer, pLbpIndication->m_pNsdu, pLbpIndication->m_u16NsduLength);

				lbpCallbackType = LBP_DUMMY_CALLBACK;
				AdpLbpRequest(&dstAddr, pLbpIndication->m_u16NsduLength, pMemoryBuffer, u8NsduHandle++, u8MaxHops, true, 0, false);
			} else {
				LOG_ERR(Log("Received message for invalid EUI64; Drop it!"));
			}
		} else {
			/* only end-device will be handled here */
			/* Frame must come from a short address between 0x0000 and 0x7FFF */
			if ((pLbpIndication->m_pSrcAddr->m_u8AddrSize == ADP_ADDRESS_16BITS) && (pLbpIndication->m_pSrcAddr->m_u16ShortAddr < 0x8000)) {
				if (_Joined()) {
					if (pu8MessageType == LBP_CHALLENGE) {
						/* reuse function from join */
						_Join_Process_Challenge(&eui64Address, u16BootStrappingDataLength, pBootStrappingData);
					} else if (pu8MessageType == LBP_ACCEPTED) {
						/* reuse function from join */
						_Join_Process_Accepted(&eui64Address, u16BootStrappingDataLength, pBootStrappingData);
					} else if (pu8MessageType == LBP_DECLINE) {
						LOG_ERR(Log("LBP_DECLINE: Not implemented"));
					} else if (pu8MessageType == LBP_KICK_TO_LBD) {
						_Kick_Notify();
					} else if (pu8MessageType == LBP_KICK_FROM_LBD) {
						LOG_ERR(Log("LBP_KICK_FROM_LBD: Never should be here"));
					} else if (pu8MessageType == LBP_JOINING) {
						LOG_ERR(Log("LBP_JOINING: Never should be here"));
					} else {
						LOG_ERR(Log("Unsupported LBP message: %u", pu8MessageType));
					}
				} else {
					if (pu8MessageType == LBP_CHALLENGE) {
						_Join_Process_Challenge(&eui64Address, u16BootStrappingDataLength, pBootStrappingData);
					} else if (pu8MessageType == LBP_ACCEPTED) {
						_Join_Process_Accepted(&eui64Address, u16BootStrappingDataLength, pBootStrappingData);
					} else if (pu8MessageType == LBP_DECLINE) {
						LOG_INFO(Log("LBP_DECLINE"));

						_Join_Confirm(G3_NOT_PERMITED );
					} else if (pu8MessageType == LBP_KICK_TO_LBD) {
						LOG_ERR(Log("LBP_KICK_TO_LBD: Not joined!"));
					} else if (pu8MessageType == LBP_KICK_FROM_LBD) {
						LOG_ERR(Log("LBP_KICK_FROM_LBD: Never should be here"));
					} else if (pu8MessageType == LBP_JOINING) {
						LOG_ERR(Log("LBP_JOINING: Never should be here"));
					} else {
						LOG_ERR(Log("Unsupported LBP message: %u", pu8MessageType));
					}
				}
			} else {
				LOG_DBG(Log("Invalid Source Address on frame to LBD; Drop frame"));
				return;
			}
		}
	}

	/* else: decoding error */
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_InitDev(void)
{
	struct TAdpNotificationsToLbp notifications;

	notifications.fnctAdpLbpConfirm = AdpLbpConfirmDev;
	notifications.fnctAdpLbpIndication = AdpLbpIndicationDev;

	AdpSetNotificationsToLbp(&notifications);

	memset(&randP, 0, sizeof(randP));
	memset(&g_LbpContext, 0, sizeof(g_LbpContext));
	g_LbpContext.m_u16ShortAddress = 0xFFFF;
	g_LbpContext.m_GroupMasterKey[0].keyId = UNSET_KEY;
	g_LbpContext.m_GroupMasterKey[1].keyId = UNSET_KEY;
	EAP_PSK_Initialize(&sEapPskKey, &g_LbpContext.m_PskContext);
	
	u8MaxHops = _GetAdpMaxHops();
	_SetDeviceTypeDev();
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications)
{
	if (pNotifications != NULL) {
		g_LbpContext.m_LbpNotifications = *pNotifications;
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void LBP_SetParamDev(uint32_t u32AttributeId, uint16_t u16AttributeIndex, uint8_t u8AttributeLen, const uint8_t *pu8AttributeValue,
		LBP_SET_PARAM_CONFIRM *pSetConfirm)
{
	pSetConfirm->attributeId = u32AttributeId;
	pSetConfirm->attributeIndex = u16AttributeIndex;
	pSetConfirm->status = LBP_STATUS_UNSUPPORTED_PARAMETER;

	switch (u32AttributeId) {
	case LBP_IB_IDP:
		if ((u8AttributeLen == LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB) || 
				(u8AttributeLen == LBP_NETWORK_ACCESS_ID_SIZE_P_CENELEC_FCC) || 
				(u8AttributeLen == 0)) { /* 0 to invalidate value */
			sIdP.m_u8Size = u8AttributeLen;
			memcpy(sIdP.m_au8Value, pu8AttributeValue, u8AttributeLen);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_RANDP:
		if (u8AttributeLen == 16) {
			memcpy(randP.m_au8Value, pu8AttributeValue, 16);
			pSetConfirm->status = LBP_STATUS_OK;
		} else {
			/* Wrong parameter size */
			pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
		}

		break;

	case LBP_IB_PSK:
		if (u8AttributeLen == 16) {
			memcpy(sEapPskKey.m_au8Value, pu8AttributeValue, 16);
			EAP_PSK_Initialize(&sEapPskKey, &g_LbpContext.m_PskContext);
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
void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK)
{
	/* Set the information in G3 stack */
	_SetExtendedAddress(pEUI64Address);
	_SetPanId(panId);
	_SetShortAddress(shortAddress);
	AdpMac_SetGroupMasterKeySync(pGMK);

	LOG_DBG(Log("LBP_ForceRegister: ShortAddr: 0x%04X PanId: 0x%04X", shortAddress, panId));
	LOG_DBG(LogBuffer(pEUI64Address->m_au8Value, 8, "LBP_ForceRegister: EUI64: "));
	LOG_DBG(LogBuffer(pGMK->key, 16, "LBP_ForceRegister: Key: "));
	LOG_DBG(Log("LBP_ForceRegister: KeyIndex: %u", pGMK->keyId));

	/* Force state in LBP */
	_ForceJoined(shortAddress, panId, pEUI64Address);
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void AdpNetworkJoinRequest(uint16_t u16PanId, uint16_t u16LbaAddress, uint8_t u8MediaType)
{
	ADP_EXTENDED_ADDRESS extendedAddress;
	uint8_t u8Status = G3_INVALID_REQUEST;
	
	LOG_INFO(Log("AdpNetworkJoinRequest() PanID %04X Lba %04X MediaType %02X", u16PanId, u16LbaAddress, u8MediaType));

	// Get Available MAC layers
	g_LbpContext.m_AvailableMacLayers = MacWrapperGetAvailableMacLayers();

	if (g_LbpContext.m_u8BootstrapState != STATE_BOOT_JOINED) {
		// Before starting the network read the Extended address from the MAC layer
		if (AdpMac_GetExtendedAddressSync(&extendedAddress)) {
			// Remember network info
			_ForceJoinStatus(false); // Not joined
			_SetPanId(u16PanId);
			g_LbpContext.m_u16PanId = u16PanId;
			g_LbpContext.m_u16LbaAddress = u16LbaAddress;
			if (g_LbpContext.m_AvailableMacLayers == MAC_WRP_AVAILABLE_MAC_PLC) {
				g_LbpContext.m_u8MediaType = (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF;
			}
			else if (g_LbpContext.m_AvailableMacLayers == MAC_WRP_AVAILABLE_MAC_RF) {
				g_LbpContext.m_u8MediaType = (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC;
			}
			else {
				// Hybrid Profile
				g_LbpContext.m_u8MediaType = u8MediaType;
			}
			memcpy(&g_LbpContext.m_EUI64Address, &extendedAddress, ADP_ADDRESS_64BITS);
			_SetExtendedAddress(&extendedAddress);

			LOG_DBG(LogBuffer(g_LbpContext.m_EUI64Address.m_au8Value, 8, "ExtendedAddress: "));

			// join the network
			_JoinRequest();

			u8Status = G3_SUCCESS;
		}
		else {
			u8Status = G3_FAILED;
			_Join_Confirm(u8Status);
		}
	}
	else {
		LOG_ERR(Log("AdpNetworkJoinRequest() Network already joined"));
	}
}

/**********************************************************************************************************************/

/**
 **********************************************************************************************************************/
void AdpNetworkLeaveRequest(void)
{
	uint8_t u8Status = G3_SUCCESS;
	struct TAdpNetworkLeaveConfirm leaveConfirm;

	if (g_LbpContext.m_u8BootstrapState == STATE_BOOT_JOINED) {
		_LeaveRequest(&g_LbpContext.m_EUI64Address);
	}
	else {
		u8Status = G3_INVALID_REQUEST;
	}

	if (u8Status != G3_SUCCESS) {
		if (g_LbpContext.m_LbpNotifications.leaveConfirm) {
			leaveConfirm.m_u8Status = u8Status;
			g_LbpContext.m_LbpNotifications.leaveConfirm(&leaveConfirm);
		}
  }
}
