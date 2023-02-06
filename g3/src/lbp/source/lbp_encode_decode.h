/**
 *
 * \file
 *
 * \brief LBP Protocol implementation
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

#ifndef __LBP_H__
#define __LBP_H__

/**********************************************************************************************************************/

/** Next section defines the LPB message types
 ***********************************************************************************************************************
 * Please note that the T and Code are both considered
 **********************************************************************************************************************/

/* / The LBD requests joining a PAN and provides the necessary authentication material. */
#define LBP_JOINING 0x01

/* / Authentication succeeded with delivery of device specific information (DSI) to the LBD */
#define LBP_ACCEPTED 0x09

/* / Authentication in progress. PAN specific information (PSI) may be delivered to the LBD */
#define LBP_CHALLENGE 0x0A

/* / Authentication failed */
#define LBP_DECLINE 0x0B

/* / KICK frame is used by any device to inform the coordinator that it left the PAN. */
#define LBP_KICK_FROM_LBD 0x04

/* / KICK frame is used by a PAN coordinator to force a device to lose its MAC address */
#define LBP_KICK_TO_LBD 0x0C

/* Parameter values */
/* Note: the parameters are already encoded on 1 byte (M field and last bit included) */
#define CONF_PARAM_SHORT_ADDR 0x1D
#define CONF_PARAM_GMK 0x27
#define CONF_PARAM_GMK_ACTIVATION 0x2B
#define CONF_PARAM_GMK_REMOVAL 0x2F
#define CONF_PARAM_RESULT 0x31

/* Parameter result values */
#define RESULT_PARAMETER_SUCCESS 0x00
#define RESULT_MISSING_REQUIRED_PARAMETER 0x01
#define RESULT_INVALID_PARAMETER_VALUE 0x02
#define RESULT_UNKNOWN_PARAMETER_ID 0x03

/* Mask to track the presence of mandatory parameters */
#define CONF_PARAM_SHORT_ADDR_MASK 0x01
#define CONF_PARAM_GMK_MASK 0x02
#define CONF_PARAM_GMK_ACTIVATION_MASK 0x04
#define CONF_PARAM_GMK_REMOVAL_MASK 0x08
#define CONF_PARAM_RESULT_MASK 0x10

/**********************************************************************************************************************/

/** The LBP_Encode_KickFromLBDRequest primitive is used to encode the LBP KICK type message generated by LBD
 ***********************************************************************************************************************
 *
 * @param pEUI64Address 64bits address of Bootstrapping Device (LBD)
 *
 * @param u16MessageLength Length of the pMessageBuffer buffer
 *
 * @param pMessageBuffer (IN/OUT) Encoded message
 *
 * @return The size of the encoded data in buffer
 *
 **********************************************************************************************************************/
uint16_t LBP_Encode_KickFromLBDRequest(const struct TAdpExtendedAddress *pEUI64Address, uint16_t u16MessageLength,
		uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_Encode_KickToLBD primitive is used to encode the LBP KICK type message generated by LBS
 ***********************************************************************************************************************
 *
 * @param pEUI64Address 64bits address of Bootstrapping Device (LBD)
 *
 * @param u16MessageLength Length of the pMessageBuffer buffer
 *
 * @param pMessageBuffer (IN/OUT) Encoded message
 *
 * @return The size of the encoded data in buffer
 *
 **********************************************************************************************************************/
uint16_t LBP_Encode_KickToLBD(const struct TAdpExtendedAddress *pEUI64Address, uint16_t u16MessageLength,
		uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_Encode_JoiningRequest primitive is used to encode the LBP JOINING type message.
 ***********************************************************************************************************************
 *
 * @param au8EUI64Address 64bits address of Bootstrapping Device (LBD)
 *
 * @param u8MediaType Media Type to be encoded in LBP frame
 *
 * @param u16BootStrappingDataLength Length of the bootstrapping data in bytes; the bootstrapping data should be pre-stored
 *                                   in the pMessageBuffer parameter
 *
 * @param u16MessageLength Length of the pMessageBuffer buffer
 *
 * @param pMessageBuffer (IN/OUT) Encoded message
 *
 * @return The size of the encoded data in buffer
 *
 **********************************************************************************************************************/
uint16_t LBP_Encode_JoiningRequest(const struct TAdpExtendedAddress *pEUI64Address, uint8_t u8MediaType,
		uint16_t u16BootStrappingDataLength, uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_Encode_ChallengeRequest primitive is used to encode the LBP CHALLENGE type message.
 ***********************************************************************************************************************
 *
 * @param au8EUI64Address 64bits address of Bootstrapping Device (LBD)
 *
 * @param u8MediaType Media Type to be encoded in LBP frame
 *
 * @param u8DisableBackupMedium Disable Backup Medium flag to be encoded in LBP frame
 *
 * @param u16BootStrappingDataLength Length of the bootstrapping data in bytes; the bootstrapping data should be pre-stored
 *                                   in the pMessageBuffer parameter
 *
 * @param u16MessageLength Length of the pMessageBuffer buffer
 *
 * @param pMessageBuffer (IN/OUT) Encoded message
 *
 * @return The size of the encoded data in buffer
 *
 **********************************************************************************************************************/
uint16_t LBP_Encode_ChallengeRequest(const struct TAdpExtendedAddress *pEUI64Address, uint8_t u8MediaType,
		uint8_t u8DisableBackupMedium, uint16_t u16BootStrappingDataLength, uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_Encode_AcceptedRequest primitive is used to encode the LBP ACCEPTED type message.
 ***********************************************************************************************************************
 *
 * @param au8EUI64Address 64bits address of Bootstrapping Device (LBD)
 *
 * @param u8MediaType Media Type to be encoded in LBP frame
 *
 * @param u8DisableBackupMedium Disable Backup Medium flag to be encoded in LBP frame
 *
 * @param u16BootStrappingDataLength Length of the bootstrapping data in bytes; the bootstrapping data should be pre-stored
 *                                   in the pMessageBuffer parameter
 *
 * @param u16MessageLength Length of the pMessageBuffer buffer
 *
 * @param pMessageBuffer (IN/OUT) Encoded message
 *
 * @return The size of the encoded data in buffer
 *
 **********************************************************************************************************************/
uint16_t LBP_Encode_AcceptedRequest(const struct TAdpExtendedAddress *pEUI64Address, uint8_t u8MediaType,
		uint8_t u8DisableBackupMedium, uint16_t u16BootStrappingDataLength, uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_IsDecline primitive is used to check if received message is DECLINE type.
 ***********************************************************************************************************************
 *
 * @param u16MessageLength Length of the received message
 *
 * @param pMessageBuffer Received message
 *
 * @return true if the message is Decline
 **********************************************************************************************************************/
bool LBP_IsDeclineResponse(uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_IsChallenge primitive is used to check if received message is CHALLENGE type.
 ***********************************************************************************************************************
 *
 * @param u16MessageLength Length of the received message
 *
 * @param pMessageBuffer Received message
 *
 * @return true if the message is Challenge
 **********************************************************************************************************************/
bool LBP_IsChallengeResponse(uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_IsAccepted primitive is used to check if received message is ACCEPTED type.
 ***********************************************************************************************************************
 *
 * @param u16MessageLength Length of the received message
 *
 * @param pMessageBuffer Received message
 *
 * @return true if the message is Challenge
 **********************************************************************************************************************/
bool LBP_IsAcceptedResponse(uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/

/** The LBP_EncodeDecline primitive is used to encode a DECLINE frame.
 ***********************************************************************************************************************
 *
 * @param p_ext_addr Extended Address to fill destination address field
 *
 * @param pMessageBuffer Pointer to buffer where frame is encoded
 *
 * @param u16FreeBufferLength Free available space in buffer to encode
 *
 * @param u8EAPIdentifier EAP Identifier to use in EAP Failure encoding
 *
 * @param u8MediaType Media Type to be encoded in frame
 *
 * @param u8DisableBackupMedium Disable Backup Medium to be encoded in frame
 *
 * @return Encoded frame length
 **********************************************************************************************************************/
uint16_t LBP_EncodeDecline(uint8_t *p_ext_addr, uint8_t *pMessageBuffer, uint16_t u16FreeBufferLength,
		uint8_t u8EAPIdentifier, uint8_t u8MediaType, uint8_t u8DisableBackupMedium);

/**********************************************************************************************************************/

/** The LBP_Decode_Message primitive is used to decode a LBP message
 ***********************************************************************************************************************
 *
 * @param u16MessageLength Length of the received message
 *
 * @param pMessageBuffer Received message
 *
 * @param pu8MessageType The message type (contains concatenated T and Code fields):
 *			T=0,Code=1 JOINING
 *			T=1,Code=1 ACCEPTED
 *			T=1,Code=2 CHALLANGE
 *			T=1,Code=3 DECLINE
 *			T=0,Code=4 KICK
 *			T=1,Code=4 KICK
 *			T=0,Code=5 CONFLICT
 *
 * @param pu16TransactionId Aids in matching responses with requests (12-bit)
 *
 * @param au8EUI64Address [OUT] upon return will contain the A_LBD field
 *
 * @param u16BootStrappingDataLength [OUT] Upon return will contain the length of the bootstrapping data in bytes
 *
 * @param pBootStrappingData [OUT] Upon return will point to the bootstrapping data
 *
 * @return false is the message cannot be decoded
 *
 **********************************************************************************************************************/
bool LBP_Decode_Message(uint16_t u16MessageLength, uint8_t *pMessageBuffer, uint8_t *pu8MessageType,
		struct TAdpExtendedAddress *pEUI64Address, uint16_t *pu16BootStrappingDataLength, uint8_t **pBootStrappingData);
#endif

/**********************************************************************************************************************/

/** @}
 **********************************************************************************************************************/
