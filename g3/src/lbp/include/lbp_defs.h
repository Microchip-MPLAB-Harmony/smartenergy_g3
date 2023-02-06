/**
 *
 * \file
 *
 * \brief G3 LBP Definitions
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

#ifndef __LBP_DEFS_H__
#define __LBP_DEFS_H__

/* EAP-PSK Defines & Types */
#define NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_S   34
#define NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P   36

#define NETWORK_ACCESS_IDENTIFIER_SIZE_S_ARIB   NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_S
#define NETWORK_ACCESS_IDENTIFIER_SIZE_P_ARIB   NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P

#define NETWORK_ACCESS_IDENTIFIER_SIZE_S_CENELEC_FCC   8
#define NETWORK_ACCESS_IDENTIFIER_SIZE_P_CENELEC_FCC   8

/* LBP status */
enum eLbpStatus {
	LBP_STATUS_OK = 0,
	LBP_STATUS_NOK,
	LBP_STATUS_UNSUPPORTED_PARAMETER,
	LBP_STATUS_INVALID_INDEX,
	LBP_STATUS_INVALID_LENGTH,
	LBP_STATUS_INVALID_VALUE
};

/* List of LBP attribute IDs */
enum eLbpAttribute {
	LBP_IB_IDS         = 0x00000000,
	LBP_IB_IDP         = 0x00000001,
	LBP_IB_PSK         = 0x00000002,
	LBP_IB_GMK         = 0x00000003,
	LBP_IB_REKEY_GMK   = 0x00000004,
	LBP_IB_RANDP       = 0x00000005,
	LBP_IB_MSG_TIMEOUT = 0x00000006
};

/* LBP parameter get confirm */
struct TLbpGetParamConfirm {
	enum eLbpStatus eStatus;
	uint32_t u32AttributeId;
	uint16_t u16AttributeIndex;
	uint8_t u8AttributeLen;
	uint8_t au8AttributeValue[NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P];
};

/* LBP parameter set confirm */
struct TLbpSetParamConfirm {
	enum eLbpStatus eStatus;
	uint32_t u32AttributeId;
	uint16_t u16AttributeIndex;
};

#endif

