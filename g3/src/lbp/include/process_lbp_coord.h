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

#include "Timer.h"
#include "ProtoEapPsk.h"
#include "AdpSharedTypes.h"

#ifndef __PROCESS_LBP_COORD_H__
#define __PROCESS_LBP_COORD_H__

/* Status codes related to HostInterface processing */
enum e_bootstrap_slot_state {
	BS_STATE_WAITING_JOINNING = 0,
	BS_STATE_SENT_EAP_MSG_1,
	BS_STATE_WAITING_EAP_MSG_2,
	BS_STATE_SENT_EAP_MSG_3,
	BS_STATE_WAITING_EAP_MSG_4,
	BS_STATE_SENT_EAP_MSG_ACCEPTED,
	BS_STATE_SENT_EAP_MSG_DECLINED,
};

/* BOOTSTRAP_NUM_SLOTS defines the number of parallel bootstrap procedures that can be carried out */
#define BOOTSTRAP_NUM_SLOTS 5
#define BOOTSTRAP_MSG_MAX_RETRIES 1

typedef struct {
	enum e_bootstrap_slot_state e_state;
	struct TAdpExtendedAddress m_LbdAddress;
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
} t_bootstrap_slot;

/**********************************************************************************************************************/

/** The LBP_JoinRequestIndication primitive allows the upper layer to be notified of the intention of a device
 * to Join the Network, allowing to reject it or accept it assigning a network short address.
 ***********************************************************************************************************************
 * @param au8LbdAddress Extended Address of device willing to Join the network.
 * @param u16AssignedAddress The 16-bit address to be assigned to joining device.
 *                             Set to 0xFF to reject the joining device.
 * @param bRekeying Indicates whether a rekeying process is in place.
 **********************************************************************************************************************/
typedef void (*LBP_JoinRequestIndication)(uint8_t* au8LbdAddress, uint16_t* u16AssignedAddress);

/**********************************************************************************************************************/

/** The LBP_JoinCompleteIndication primitive allows the upper layer to be notified of the successful completion
 * of a Network Join for a certain device.
 ***********************************************************************************************************************
 * @param au8LbdAddress Extended Address of device that Joined the network.
 * @param u16AssignedAddress The 16-bit address of device that Joined the network.
 **********************************************************************************************************************/
typedef void (*LBP_JoinCompleteIndication)(uint8_t* au8LbdAddress, uint16_t u16AssignedAddress);

/**********************************************************************************************************************/

/** The LBP_LeaveIndication primitive allows the upper layer to be notified when a device leaves the network.
 ***********************************************************************************************************************
 * @param u16NetworkAddress The 16-bit network address of device leaving the network.
 **********************************************************************************************************************/
typedef void (*LBP_LeaveIndication)(uint16_t u16NetworkAddress);

struct TLbpNotificationsCoord {
	LBP_JoinRequestIndication fnctJoinRequestIndication;
	LBP_JoinCompleteIndication fnctJoinCompleteIndication;
	LBP_LeaveIndication fnctLeaveIndication;
};

/**********************************************************************************************************************/


/** The LBP_InitCoord primitive initializes Coordinator LBP module
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void LBP_InitCoord(bool aribBand);

/** The LBP_UpdateBootstrapSlots primitive updates the slots that control bootstrap protocol
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void LBP_UpdateBootstrapSlots(void);

/** The LBP_SetNotificationsCoord primitive sets the callbacks to be invoked when events have to be notified
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void LBP_SetNotificationsCoord(struct TLbpNotificationsCoord *pNotifications);

/** The LBP_KickDevice primitive allows the upper layer to remove a device from the Network.
 ***********************************************************************************************************************
 * @param us_short_address The Network address of device to be removed.
 * @param pEUI64Address The Extended address of device to be removed.
 * @return True if Kick is sent to device, otherwise False.
 **********************************************************************************************************************/
bool LBP_KickDevice(uint16_t us_short_address, struct TAdpExtendedAddress *pEUI64Address);

/** The LBP_Rekey primitive allows the upper layer to start a rekey process for a device in the Network.
 ***********************************************************************************************************************
 * @param us_short_address The Network address of device to be rekeyed.
 * @param pEUI64Address The Extended address of device to be rekeyed.
 * @param bDistribute True in distribution phase, False in activation phase.
 **********************************************************************************************************************/
void LBP_Rekey(uint16_t us_short_address, struct TAdpExtendedAddress *pEUI64Address, bool bDistribute);

/** The LBP_Rekey primitive allows the upper layer to start or stop a rekey phase in LBP module.
 ***********************************************************************************************************************
 * @param bRekey Indicates whether Rekey phase starts or finishes.
 **********************************************************************************************************************/
void LBP_SetRekeyPhase(bool bRekeyStart);

/** The LBP_activate_new_key primitive activates the new GMK after a Rekeying process
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void LBP_activate_new_key(void);

/** The LBP_SetParamCoord primitive allows the upper layer to set the value of a parameter in LBP IB.
 ***********************************************************************************************************************
 * @param u32AttributeId IB attribute ID.
 * @param u16AttributeIndex Index when attribute is a list.
 * @param u8AttributeLen Length of value to set.
 * @param pu8AttributeValue Pointer to array containing value.
 * @param pSetConfirm Result info of the Get operation.
 **********************************************************************************************************************/
void LBP_SetParamCoord(uint32_t u32AttributeId, uint16_t u16AttributeIndex, uint8_t u8AttributeLen,
		const uint8_t *pu8AttributeValue, LBP_SET_PARAM_CONFIRM *pSetConfirm);

#endif

/**********************************************************************************************************************/

/** @}
 **********************************************************************************************************************/
