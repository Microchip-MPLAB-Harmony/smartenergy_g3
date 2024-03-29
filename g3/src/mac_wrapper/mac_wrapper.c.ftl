/*******************************************************************************
  G3 MAC Wrapper Source File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_wrapper.c

  Summary:
    G3 MAC Wrapper API Source File

  Description:
    This file contains implementation of the API
    to be used by upper layers when accessing G3 MAC layers.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "system/system.h"
#include "configuration.h"
#include "mac_wrapper.h"
#include "mac_wrapper_defs.h"
#include "../mac_common/mac_common.h"
<#if MAC_PLC_PRESENT == true>
#include "../mac_plc/mac_plc.h"
</#if>
<#if MAC_RF_PRESENT == true>
#include "../mac_rf/mac_rf.h"
</#if>
#include "service/log_report/srv_log_report.h"
<#if MAC_SERIALIZATION_EN == true>
#include "service/usi/srv_usi.h"
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

typedef struct
{
    /* State of the MAC Wrapper module */
    MAC_WRP_STATE state;
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* Mac Wrapper instance handle */
    MAC_WRP_HANDLE macWrpHandle;
<#if !(HarmonyCore.SELECT_RTOS)?? || (HarmonyCore.SELECT_RTOS == "BareMetal")>
    /* Time of next task in milliseconds */
    uint32_t nextTaskTimeMs;
</#if>
<#if MAC_SERIALIZATION_EN == true || ADP_SERIALIZATION_EN == true>
    /* PIB serialization debug set length */
    uint8_t debugSetLength;
</#if>
<#if MAC_SERIALIZATION_EN == true>
    /* Mac Serialization handle */
    MAC_WRP_HANDLE macSerialHandle;
    /* USI handle for MAC serialization */
    SRV_USI_HANDLE usiHandle;
    /* Flag to indicate initialize through serial interface */
    bool serialInitialize;
    /* Flag to indicate reset request through serial interface */
    bool serialResetRequest;
    /* Flag to indicate start request through serial interface */
    bool serialStartRequest;
    /* Flag to indicate scan request through serial interface */
    bool serialScanRequest;
</#if>
    /* Flag to indicate scan request in progress */
    bool scanRequestInProgress;
} MAC_WRP_DATA;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
/* Buffer size to store data to be sent as Mac Data Request */
#define HYAL_BACKUP_BUF_SIZE   400U

</#if>
typedef struct
{
    MAC_DATA_REQUEST_PARAMS dataReqParams;
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_MEDIA_TYPE_REQUEST dataReqMediaType;
    uint8_t backupBuffer[HYAL_BACKUP_BUF_SIZE];
    MAC_STATUS firstConfirmStatus;
    bool waitingSecondConfirm;
    uint8_t probingInterval;
</#if>
<#if MAC_SERIALIZATION_EN == true>
    bool serialDataRequest;
</#if>
    bool used;
} MAC_WRP_DATA_REQ_ENTRY;

/* Data Request Queue size */
#define MAC_WRP_DATA_REQ_QUEUE_SIZE   2U

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
typedef struct
{
    uint16_t srcAddress;
    uint16_t msduLen;
    uint16_t crc;
    MAC_WRP_MEDIA_TYPE_INDICATION mediaType;
} HYAL_DUPLICATES_ENTRY;

typedef struct
{
    MAC_STATUS firstScanConfirmStatus;
    bool waitingSecondScanConfirm;
    MAC_STATUS firstResetConfirmStatus;
    bool waitingSecondResetConfirm;
    MAC_STATUS firstStartConfirmStatus;
    bool waitingSecondStartConfirm;
    bool mediaProbing;
} HYAL_DATA;

</#if>
<#if MAC_SERIALIZATION_EN == true>
typedef enum
{
    MAC_WRP_SERIAL_STATUS_SUCCESS = 0,
    MAC_WRP_SERIAL_STATUS_NOT_ALLOWED,
    MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND,
    MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER
} MAC_WRP_SERIAL_STATUS;

typedef enum
{
    /* Generic messages */
    MAC_WRP_SERIAL_MSG_STATUS = 0,

    /* MAC access request messages */
    MAC_WRP_SERIAL_MSG_MAC_INITIALIZE = 50,
    MAC_WRP_SERIAL_MSG_MAC_DATA_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_GET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_SET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_RESET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_SCAN_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_START_REQUEST,

    /* MAC response/indication messages */
    MAC_WRP_SERIAL_MSG_MAC_DATA_CONFIRM = 60,
    MAC_WRP_SERIAL_MSG_MAC_DATA_INDICATION,
    MAC_WRP_SERIAL_MSG_MAC_GET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_SET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_RESET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_SCAN_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_BEACON_NOTIFY,
    MAC_WRP_SERIAL_MSG_MAC_START_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_COMM_STATUS_INDICATION,
    MAC_WRP_SERIAL_MSG_MAC_SNIFFER_INDICATION

} MAC_WRP_SERIAL_MSG_ID;

</#if>

#pragma pack(pop)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

// Data Service Control
static MAC_WRP_DATA_REQ_ENTRY dataReqQueue[MAC_WRP_DATA_REQ_QUEUE_SIZE];

<#if MAC_PLC_PRESENT == true>
#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    ${MAC_PLC_DEVICE_TABLE_SIZE?string}

static MAC_PLC_TABLES macPlcTables;
static MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

</#if>
<#if MAC_RF_PRESENT == true>
#define MAC_MAX_POS_TABLE_ENTRIES_RF        ${MAC_RF_POS_TABLE_SIZE?string}
#define MAC_MAX_DSN_TABLE_ENTRIES_RF        ${MAC_RF_DSN_TABLE_SIZE?string}
#define MAC_MAX_DEVICE_TABLE_ENTRIES_RF     ${MAC_RF_DEVICE_TABLE_SIZE?string}

static MAC_RF_TABLES macRfTables;
static MAC_RF_POS_TABLE_ENTRY macRfPOSTable[MAC_MAX_POS_TABLE_ENTRIES_RF];
static MAC_DEVICE_TABLE_ENTRY macRfDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_RF];
static MAC_RF_DSN_TABLE_ENTRY macRfDsnTable[MAC_MAX_DSN_TABLE_ENTRIES_RF];

</#if>
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
static const uint16_t crc16_tab[256] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

#define HYAL_DUPLICATES_TABLE_SIZE   3U

static HYAL_DUPLICATES_ENTRY hyALDuplicatesTable[HYAL_DUPLICATES_TABLE_SIZE] = {{0}};

static const HYAL_DATA hyalDataDefaults = {
  MAC_STATUS_SUCCESS, // firstScanConfirmStatus
  false, // waitingSecondScanConfirm
  MAC_STATUS_SUCCESS, // firstResetConfirmStatus
  false, // waitingSecondResetConfirm
  MAC_STATUS_SUCCESS, // firstStartConfirmStatus
  false, // waitingSecondStartConfirm
  false, // mediaProbing
};

static HYAL_DATA hyalData;

</#if>
<#if MAC_SERIALIZATION_EN == true>
static uint8_t serialRspBuffer[512];

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetFreeDataReqEntry(void)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if (dataReqQueue[index].used == false)
        {
            found = &dataReqQueue[index];
            dataReqQueue[index].used = true;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetFreeDataReqEntry() Found free data request entry on index %u\r\n", index);
            break;
        }
    }

    return found;
}

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetDataReqEntryByHandle(uint8_t handle)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if ((dataReqQueue[index].used == true) &&
            (dataReqQueue[index].dataReqParams.msduHandle == handle))
        {
            found = &dataReqQueue[index];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetDataReqEntryByHandle() Found matching data request entry on index %u, Handle: 0x%02X\r\n", index, handle);
            break;
        }
    }

    return found;
}

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
static uint16_t lMAC_WRP_HyalCrc16(const uint8_t *dataBuf, uint32_t length)
{
    uint16_t crc = 0;

    // polynom(16): X16 + X12 + X5 + 1 = 0x1021
    while ((length--) > 0U)
    {
        crc = crc16_tab[(uint8_t)(crc >> 8) ^ (*dataBuf ++)] ^ (crc << 8);
    }
    return crc;
}

static bool lMAC_WRP_HyalCheckDuplicates(uint16_t srcAddr, uint8_t *msdu, uint16_t msduLen, MAC_WRP_MEDIA_TYPE_INDICATION mediaType)
{
    bool duplicate = false;
    uint8_t index = 0U;
    uint16_t crc;

    // Calculate CRC for incoming frame
    crc = lMAC_WRP_HyalCrc16(msdu, msduLen);

    // Look for entry in the Duplicates Table
    HYAL_DUPLICATES_ENTRY *entry = &hyALDuplicatesTable[0];
    while (index < HYAL_DUPLICATES_TABLE_SIZE)
    {
        // Look for same fields and different MediaType
        if ((entry->srcAddress == srcAddr) && (entry->msduLen == msduLen) &&
            (entry->crc == crc) && (entry->mediaType != mediaType))
        {
            duplicate = true;
            break;
        }
        index ++;
        entry ++;
    }

    if (!duplicate)
    {
        // Entry not found, store it
        (void) memmove(&hyALDuplicatesTable[1], &hyALDuplicatesTable[0],
            (HYAL_DUPLICATES_TABLE_SIZE - 1U) * sizeof(HYAL_DUPLICATES_ENTRY));
        // Populate the new entry.
        hyALDuplicatesTable[0].srcAddress = srcAddr;
        hyALDuplicatesTable[0].msduLen = msduLen;
        hyALDuplicatesTable[0].crc = crc;
        hyALDuplicatesTable[0].mediaType = mediaType;
    }

    // Return duplicate or not
    return duplicate;
}

static bool lMAC_WRP_IsAttributeInPLCRange(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    uint32_t attrId = (uint32_t) attribute;

    /* Check attribute ID range to distinguish between PLC and RF MAC */
    if (attrId < 0x00000200U)
    {
        /* Standard PLC MAC IB */
        return true;
    }
    else if (attrId < 0x00000400U)
    {
        /* Standard RF MAC IB */
        return false;
    }
    else if (attrId < 0x08000200U)
    {
        /* Manufacturer PLC MAC IB */
        return true;
    }
    else
    {
        /* Manufacturer RF MAC IB */
        return false;
    }
}

static bool lMAC_WRP_CheckRFMediaProbing(uint8_t probingInterval, MAC_ADDRESS dstAddress)
{
    MAC_WRP_POS_ENTRY_RF posEntry;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    uint8_t posTableEntryTtl;
    uint8_t lqiValidTime;

    /* Probing interval has to be greater than 0 */
    if (probingInterval == 0U)
    {
        return false;
    }

    /* Destination addressing has to be Short Addressing */
    if (dstAddress.addressMode != MAC_ADDRESS_MODE_SHORT)
    {
        return false;
    }

    /* Look for entry in RF POS Table */
    status = (MAC_WRP_STATUS) MAC_RF_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF,
        dstAddress.shortAddress, &pibValue);

    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        (void) memcpy((void *) &posEntry, (void *) pibValue.value, sizeof(MAC_WRP_POS_ENTRY_RF));
        status = (MAC_WRP_STATUS) MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL,
            0, &pibValue);

        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            posTableEntryTtl = pibValue.value[0];
            lqiValidTime = (uint8_t)((posEntry.reverseLqiValidTime + 59U) / 60U);
            if ((posTableEntryTtl > lqiValidTime) && ((posTableEntryTtl - lqiValidTime) >= probingInterval))
            {
                /* Conditions met to perform the media probing */
                return true;
            }
        }
    }

    /* If this point is reached, no probing is done */
    return false;
}

static bool lMAC_WRP_CheckPLCMediaProbing(uint8_t probingInterval, MAC_ADDRESS dstAddress)
{
    MAC_WRP_POS_ENTRY posEntry;
    MAC_WRP_NEIGHBOUR_ENTRY nbEntry;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    uint8_t tmrTtl;
    uint8_t tmrValidTime;

    /* Probing interval has to be greater than 0 */
    if (probingInterval == 0U)
    {
        return false;
    }

    /* Destination addressing has to be Short Addressing */
    if (dstAddress.addressMode != MAC_ADDRESS_MODE_SHORT)
    {
        return false;
    }

    /* Look for entry in POS Table */
    status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT,
        dstAddress.shortAddress, &pibValue);

    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        (void) memcpy((void *) &posEntry, (void *) pibValue.value, sizeof(MAC_WRP_POS_ENTRY));

        /* Look for entry in Neighbour Table to fill TMR Valid time */
        tmrValidTime = 0;
        status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT,
            dstAddress.shortAddress, &pibValue);

        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            (void) memcpy((void *) &nbEntry, (void *) pibValue.value, sizeof(MAC_WRP_NEIGHBOUR_ENTRY));
            tmrValidTime = (uint8_t)((nbEntry.tmrValidTime + 59U) / 60U);
        }

        status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_TMR_TTL, 0, &pibValue);
        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            tmrTtl = pibValue.value[0];
            if ((tmrTtl > tmrValidTime) && ((tmrTtl - tmrValidTime) >= probingInterval))
            {
                /* Conditions met to perform the media probing */
                /* Reset TMR TTL for entry before probing, so TMR is exchanged */
                /* (pibValue is not used, same variable as prevoius can be used) */
                (void) MAC_PLC_SetRequestSync(MAC_PIB_MANUF_RESET_TMR_TTL, dstAddress.shortAddress, &pibValue);
                return true;
            }
        }
    }

    /* If this point is reached, no probing is done */
    return false;
}

</#if>
static bool lMAC_WRP_IsSharedAttribute(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check if attribute in the list of shared between MAC layers */
    if ((attribute == MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS) ||
        (attribute == MAC_WRP_PIB_PAN_ID) ||
        (attribute == MAC_WRP_PIB_PROMISCUOUS_MODE) ||
        (attribute == MAC_WRP_PIB_SHORT_ADDRESS) ||
        (attribute == MAC_WRP_PIB_POS_TABLE_ENTRY_TTL) ||
        (attribute == MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD) ||
        (attribute == MAC_WRP_PIB_RC_COORD) ||
        (attribute == MAC_WRP_PIB_KEY_TABLE))
    {
        /* Shared IB */
        return true;
    }
    else
    {
        /* Non-shared IB */
        return false;
    }
}

<#if MAC_SERIALIZATION_EN == true || ADP_SERIALIZATION_EN == true>
static void lMemcpyToUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux;

    (void) memcpy((uint8_t *) &aux, pSrc, 4);

    *pDst++ = (uint8_t) (aux >> 24);
    *pDst++ = (uint8_t) (aux >> 16);
    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void lMemcpyToUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux;

    (void) memcpy((uint8_t *) &aux, pSrc, 2);

    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void lMemcpyFromUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux;

    aux = ((uint32_t) *pSrc++) << 24;
    aux += ((uint32_t) *pSrc++) << 16;
    aux += ((uint32_t) *pSrc++) << 8;
    aux += (uint32_t) *pSrc;

    (void) memcpy(pDst, (uint8_t *) &aux, 4);
}

static void lMemcpyFromUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux;

    aux = ((uint16_t) *pSrc++) << 8;
    aux += (uint16_t) *pSrc;

    (void) memcpy(pDst, (uint8_t *) &aux, 2);
}

</#if>
<#if MAC_SERIALIZATION_EN == true>
static void lMAC_WRP_StringifyMsgStatus(MAC_WRP_SERIAL_STATUS status, MAC_WRP_SERIAL_MSG_ID command)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_STATUS;
    serialRspBuffer[serialRspLen++] = (uint8_t) status;
    serialRspBuffer[serialRspLen++] = (uint8_t) command;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyDataConfirm(MAC_WRP_DATA_CONFIRM_PARAMS* dcParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_DATA_CONFIRM;
    serialRspBuffer[serialRspLen++] = dcParams->msduHandle;
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->status;
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->timestamp;
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyDataIndication(MAC_WRP_DATA_INDICATION_PARAMS* diParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint16_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_DATA_INDICATION;
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->srcPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->srcPanId;

    if (diParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) diParams->srcAddress.shortAddress;
    }
    else if (diParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], diParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->destPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->destPanId;

    if (diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) diParams->destAddress.shortAddress;
    }
    else if (diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], diParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = diParams->linkQuality;
    serialRspBuffer[serialRspLen++] = diParams->dsn;

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->timestamp;

    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->securityLevel;
    serialRspBuffer[serialRspLen++] = diParams->keyIndex;
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->qualityOfService;

    serialRspBuffer[serialRspLen++] = diParams->rxModulation;
    serialRspBuffer[serialRspLen++] = diParams->rxModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->rxToneMap.toneMap, 3);
    serialRspLen += 3U;
    serialRspBuffer[serialRspLen++] = diParams->computedModulation;
    serialRspBuffer[serialRspLen++] = diParams->computedModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->computedToneMap.toneMap, 3);
    serialRspLen += 3U;

    serialRspBuffer[serialRspLen++] = (uint8_t)diParams->mediaType;

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->msduLength >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->msduLength;

    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->msdu, diParams->msduLength);
    serialRspLen += diParams->msduLength;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifySnifferIndication(MAC_WRP_SNIFFER_INDICATION_PARAMS* siParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint16_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SNIFFER_INDICATION;
    serialRspBuffer[serialRspLen++] = siParams->frameType;
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->srcPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->srcPanId;

    if (siParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) siParams->srcAddress.shortAddress;
    }
    else if (siParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], siParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        srcAddrLen = 0U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->destPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->destPanId;

    if (siParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) siParams->destAddress.shortAddress;
    }
    else if (siParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], siParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        dstAddrLen = 0U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
    }

    serialRspBuffer[serialRspLen++] = siParams->linkQuality;
    serialRspBuffer[serialRspLen++] = siParams->dsn;

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->timestamp;

    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->securityLevel;
    serialRspBuffer[serialRspLen++] = siParams->keyIndex;
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->qualityOfService;

    serialRspBuffer[serialRspLen++] = siParams->rxModulation;
    serialRspBuffer[serialRspLen++] = siParams->rxModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->rxToneMap.toneMap, 3);
    serialRspLen += 3U;
    serialRspBuffer[serialRspLen++] = siParams->computedModulation;
    serialRspBuffer[serialRspLen++] = siParams->computedModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->computedToneMap.toneMap, 3);
    serialRspLen += 3U;

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->msduLength >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->msduLength;

    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->msdu, siParams->msduLength);
    serialRspLen += siParams->msduLength;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyResetConfirm(MAC_WRP_RESET_CONFIRM_PARAMS* rcParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_RESET_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) rcParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyBeaconNotIndication(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS* bniParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_BEACON_NOTIFY;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.panId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.panId;
    serialRspBuffer[serialRspLen++] = bniParams->panDescriptor.linkQuality;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.lbaAddress >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.lbaAddress;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.rcCoord >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.rcCoord;
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyScanConfirm(MAC_WRP_SCAN_CONFIRM_PARAMS* scParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SCAN_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) scParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyStartConfirm(MAC_WRP_START_CONFIRM_PARAMS* scParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_START_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) scParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyCommStatusIndication(MAC_WRP_COMM_STATUS_INDICATION_PARAMS* csiParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint8_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_COMM_STATUS_INDICATION;
    serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->panId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->panId;

    if (csiParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->srcAddress.shortAddress;
    }
    else if (csiParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], csiParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    if (csiParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->destAddress.shortAddress;
    }
    else if (csiParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], csiParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->status;
    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->securityLevel;
    serialRspBuffer[serialRspLen++] = csiParams->keyIndex;

    serialRspBuffer[serialRspLen++] = (uint8_t)csiParams->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseInitialize(uint8_t* pData)
{
    if (macWrpData.state == MAC_WRP_STATE_NOT_READY)
    {
        MAC_WRP_BAND band;

        /* Parse initialize message */
        band = (MAC_WRP_BAND) *pData;

        /* Open MAC Wrapper if it has not been opened yet */
        (void) MAC_WRP_Open(G3_MAC_WRP_INDEX_0, band);

        macWrpData.serialInitialize = true;
    }

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseDataRequest(uint8_t* pData)
{
    MAC_WRP_DATA_REQUEST_PARAMS drParams;
    uint8_t srcAddrLen, dstAddrLen;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse data request message */
    drParams.msduHandle = *pData++;
    drParams.securityLevel =  (MAC_WRP_SECURITY_LEVEL) *pData++;
    drParams.keyIndex = *pData++;
    drParams.qualityOfService = (MAC_WRP_QUALITY_OF_SERVICE) *pData++;
    drParams.txOptions = *pData++;
    drParams.destPanId = ((MAC_WRP_PAN_ID) *pData++) << 8;
    drParams.destPanId += (MAC_WRP_PAN_ID) *pData++;
    srcAddrLen = *pData++;
    if (srcAddrLen == 2U)
    {
        drParams.srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    }
    else if (srcAddrLen == 8U)
    {
        drParams.srcAddressMode = MAC_WRP_ADDRESS_MODE_EXTENDED;
    }
    else
    {
        return MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER;
    }

    dstAddrLen = *pData++;
    if (dstAddrLen == 2U)
    {
        drParams.destAddress.addressMode = MAC_WRP_ADDRESS_MODE_SHORT;
        drParams.destAddress.shortAddress = ((MAC_WRP_SHORT_ADDRESS) *pData++) << 8;
        drParams.destAddress.shortAddress += (MAC_WRP_SHORT_ADDRESS) *pData++;
    }
    else if (dstAddrLen == 8U)
    {
        drParams.destAddress.addressMode = MAC_WRP_ADDRESS_MODE_EXTENDED;
        (void) memcpy(&drParams.destAddress.extendedAddress.address, pData, dstAddrLen);
        pData += dstAddrLen;
    }
    else
    {
        return MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER;
    }

    drParams.mediaType = (MAC_WRP_MEDIA_TYPE_REQUEST) *pData++;
    /* No probing on MAC Serial access */
    drParams.probingInterval = 0;

    drParams.msduLength = ((uint16_t)*pData++) << 8;
    drParams.msduLength += (uint16_t)*pData++;
    drParams.msdu = pData;

    /* Send data request to MAC */
    MAC_WRP_DataRequest(macWrpData.macSerialHandle, &drParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseGetRequest(uint8_t* pData)
{
    uint32_t attribute;
    uint16_t index;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_PIB_ATTRIBUTE pibAttr;
    MAC_WRP_STATUS getStatus;
    uint8_t serialRspLen = 0U;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Get PIB from MAC */
    attribute = MAC_WRP_SerialParseGetRequest(pData, &index);
    pibAttr = (MAC_WRP_PIB_ATTRIBUTE) attribute;
    getStatus = MAC_WRP_GetRequestSync(macWrpData.macSerialHandle, pibAttr, index, &pibValue);

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_GET_CONFIRM;
    serialRspLen += MAC_WRP_SerialStringifyGetConfirm(&serialRspBuffer[serialRspLen],
            getStatus, pibAttr, index, pibValue.value, pibValue.length);

    /* Send get confirm through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseSetRequest(uint8_t* pData)
{
    uint16_t index;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_PIB_ATTRIBUTE attribute;
    MAC_WRP_STATUS setStatus;
    uint8_t serialRspLen = 0U;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Set MAC PIB */
    attribute = MAC_WRP_SerialParseSetRequest(pData, &index, &pibValue);
    setStatus = MAC_WRP_SetRequestSync(macWrpData.macSerialHandle, attribute, index, &pibValue);

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SET_CONFIRM;
    serialRspLen += MAC_WRP_SerialStringifySetConfirm(&serialRspBuffer[serialRspLen],
            setStatus, attribute, index);

    /* Send set confirm through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseResetRequest(uint8_t* pData)
{
    MAC_WRP_RESET_REQUEST_PARAMS rrParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse reset request message */
    rrParams.setDefaultPib = (bool) *pData;

    /* Send reset request to MAC */
    macWrpData.serialResetRequest = true;
    MAC_WRP_ResetRequest(macWrpData.macSerialHandle, &rrParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseScanRequest(uint8_t* pData)
{
    MAC_WRP_SCAN_REQUEST_PARAMS srParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse scan request message */
    srParams.scanDuration = ((uint16_t) *pData++) << 8;
    srParams.scanDuration += *pData;

    /* Send scan request to MAC */
    MAC_WRP_ScanRequest(macWrpData.macSerialHandle, &srParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseStartRequest(uint8_t* pData)
{
    MAC_WRP_START_REQUEST_PARAMS srParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse start request message */
    srParams.panId = ((uint16_t) *pData++) << 8;
    srParams.panId += *pData;

    /* Send start request to MAC */
    macWrpData.serialStartRequest = true;
    MAC_WRP_StartRequest(macWrpData.macSerialHandle, &srParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static void lMAC_WRP_CallbackUsiMacProtocol(uint8_t* pData, size_t length)
{
    uint8_t commandAux;
    MAC_WRP_SERIAL_MSG_ID command;
    MAC_WRP_SERIAL_STATUS status = MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND;

    /* Protection for invalid length */
    if (length == 0U)
    {
        return;
    }

    /* Process received message */
    commandAux = (*pData++) & 0x7FU;
    command = (MAC_WRP_SERIAL_MSG_ID) commandAux;

    switch(command)
    {
        case MAC_WRP_SERIAL_MSG_MAC_INITIALIZE:
            status = lMAC_WRP_ParseInitialize(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_DATA_REQUEST:
            status = lMAC_WRP_ParseDataRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_GET_REQUEST:
            status = lMAC_WRP_ParseGetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_SET_REQUEST:
            status = lMAC_WRP_ParseSetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_RESET_REQUEST:
            status = lMAC_WRP_ParseResetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_SCAN_REQUEST:
            status = lMAC_WRP_ParseScanRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_START_REQUEST:
            status = lMAC_WRP_ParseStartRequest(pData);
            break;

        default:
            status = MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND;
            break;
    }

    /* Initialize doesn't have confirm so send status if already initialized.
     * Other messages all have confirm. Send status only if there is a processing error */
    if ((status != MAC_WRP_SERIAL_STATUS_SUCCESS) ||
            ((command == MAC_WRP_SERIAL_MSG_MAC_INITIALIZE) && (macWrpData.serialInitialize == false)))
    {
        lMAC_WRP_StringifyMsgStatus(status, command);
    }
}

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Callbacks from MAC Layers
// *****************************************************************************
// *****************************************************************************

<#if MAC_PLC_PRESENT == true>
static void lMAC_WRP_CallbackMacPlcDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;
    MAC_WRP_DATA_REQ_ENTRY *matchingDataReq;
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    bool sendConfirm = false;
</#if>

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataConfirm() Handle: 0x%02X Status: %u\r\n", dcParams->msduHandle, (uint8_t)dcParams->status);

    /* Get Data Request entry matching confirm */
    matchingDataReq = lMAC_WRP_GetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacPlcDataConfirm() Confirm does not match any previous request!!\r\n");
        return;
    }

    /* Copy dcParams from Mac */
    (void) memcpy((void *) &dataConfirmParams, (void *) dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Check Media Probing */
                if (lMAC_WRP_CheckRFMediaProbing(matchingDataReq->probingInterval, matchingDataReq->dataReqParams.destAddress))
                {
                    /* Perform Media probing */
                    hyalData.mediaProbing = true;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "RF Media Probing\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_RF_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    hyalData.mediaProbing = false;
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            else
            {
                /* Check Dest Address mode and/or RF POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium\r\n");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for RF POS Table entry for %0004X\r\n", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_RF_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF,
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }

                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try RF as Backup Medium\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_RF_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium\r\n");
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            if (hyalData.mediaProbing)
            {
                /* PLC was probed after RF success. Send confirm to upper layer. */
                hyalData.mediaProbing = false;
                dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                if (dcParams->status == MAC_STATUS_SUCCESS)
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                }
                else
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP;
                }
            }
            else
            {
                /* PLC was used as backup medium. Send confirm to upper layer. */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP;
            }

            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            if (matchingDataReq->waitingSecondConfirm)
            {
                /* Second Confirm arrived. Send confirm to upper layer depending on results */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_BOTH;
                if ((matchingDataReq->firstConfirmStatus == MAC_STATUS_SUCCESS) ||
                        (dcParams->status == MAC_STATUS_SUCCESS))
                {
                    /* At least one SUCCESS, send confirm with SUCCESS */
                    dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                }

                /* Send confirm to upper layer */
                sendConfirm = true;
            }
            else
            {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* PLC confirm not expected on RF_NO_BACKUP request. Ignore it */
            matchingDataReq->used = false;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacPlcDataConfirm() called from a MEDIA_TYPE_REQ_RF_NO_BACKUP request!!\r\n");
            break;
        default: /* PLC only */
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
    }

    if (sendConfirm == true)
    {
        /* Release Data Req entry and send confirm to upper layer */
        matchingDataReq->used = false;
<#if MAC_SERIALIZATION_EN == true>
        if (matchingDataReq->serialDataRequest == true)
        {
            lMAC_WRP_StringifyDataConfirm(&dataConfirmParams);
        }
        else
        {
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
</#if>
    }
<#else>
    /* Fill Media Type */
    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;

    /* Release Data Req entry and send confirm to upper layer */
    matchingDataReq->used = false;
<#if MAC_SERIALIZATION_EN == true>
    if (matchingDataReq->serialDataRequest == true)
    {
        lMAC_WRP_StringifyDataConfirm(&dataConfirmParams);
    }
    else
    {
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (lMAC_WRP_HyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu,
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_PLC))
        {
            /* Same frame was received on RF medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on RF medium. Drop indication");
            return;
        }
    }

</#if>
     /* Copy diParams from Mac and fill Media Type */
    (void) memcpy((void *) &dataIndicationParams, (void *) diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
    dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyDataIndication(&dataIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcResetConfirm: Status: %u\r\n", rcParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_RESET_CONFIRM_PARAMS resetConfirmParams;

    if (hyalData.waitingSecondResetConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstResetConfirmStatus == MAC_STATUS_SUCCESS) &&
                (rcParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            resetConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
            }
        }

<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialResetRequest == true)
        {
            lMAC_WRP_StringifyResetConfirm(&resetConfirmParams);
            macWrpData.serialResetRequest = false;
        }
        else
        {
            if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
    }
<#else>
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialResetRequest == true)
    {
        lMAC_WRP_StringifyResetConfirm((void *)rcParams);
        macWrpData.serialResetRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacPlcBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcBeaconNotify: Pan ID: %04X\r\n", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac and fill Media Type */
    (void) memcpy((void *) &notifyIndicationParams, (void *) bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));
    notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyBeaconNotIndication(&notifyIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcScanConfirm: Status: %u\r\n", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;

    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
        }

        /* Clear flag */
        macWrpData.scanRequestInProgress = false;

        /* Send confirm to upper layer */
<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialScanRequest == true)
        {
            lMAC_WRP_StringifyScanConfirm(&scanConfirmParams);
        }
        else
        {
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
<#else>
    /* Clear flag */
    macWrpData.scanRequestInProgress = false;

    /* Send confirm to upper layer */
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialScanRequest == true)
    {
        lMAC_WRP_StringifyScanConfirm((void *)scParams);
    }
    else{
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcStartConfirm: Status: %u\r\n", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_START_CONFIRM_PARAMS startConfirmParams;

    if (hyalData.waitingSecondStartConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstStartConfirmStatus == MAC_STATUS_SUCCESS) &&
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            startConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            }
        }

<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialStartRequest == true)
        {
            lMAC_WRP_StringifyStartConfirm(&startConfirmParams);
            macWrpData.serialStartRequest = false;
        }
        else
        {
            if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
    }
<#else>
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialStartRequest == true)
    {
        lMAC_WRP_StringifyStartConfirm((void *)scParams);
        macWrpData.serialStartRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacPlcCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcCommStatusIndication: Status: %u\r\n", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac and fill Media Type */
    (void) memcpy((void *) &commStatusIndicationParams, (void *) csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));
    commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyCommStatusIndication(&commStatusIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "lMAC_WRP_CallbackMacPlcMacSnifferIndication:  MSDU:");

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifySnifferIndication((void *)siParams);
</#if>
    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((void *)siParams);
    }
}

</#if>
<#if MAC_RF_PRESENT == true>
static void lMAC_WRP_CallbackMacRfDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;
    MAC_WRP_DATA_REQ_ENTRY *matchingDataReq;
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    bool sendConfirm = false;
</#if>

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfDataConfirm() Handle: 0x%02X Status: %u\r\n", dcParams->msduHandle, (uint8_t)dcParams->status);

    /* Get Data Request entry matching confirm */
    matchingDataReq = lMAC_WRP_GetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacRfDataConfirm() Confirm does not match any previous request!!\r\n");
        return;
    }

    /* Copy dcParams from Mac */
    (void) memcpy((void *) &dataConfirmParams, (void *) dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            if (hyalData.mediaProbing)
            {
                /* RF was probed after PLC success. Send confirm to upper layer. */
                hyalData.mediaProbing = false;
                dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                if (dcParams->status == MAC_STATUS_SUCCESS)
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                }
                else
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP;
                }
            }
            else
            {
                /* RF was used as backup medium. Send confirm to upper layer */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP;
            }

            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Check Media Probing */
                if (lMAC_WRP_CheckPLCMediaProbing(matchingDataReq->probingInterval, matchingDataReq->dataReqParams.destAddress))
                {
                    /* Perform Media probing */
                    hyalData.mediaProbing = true;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "PLC Media Probing\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_PLC_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            else
            {
                /* Check Dest Address mode and/or PLC POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium\r\n");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for PLC POS Table entry for %0004X\r\n", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT,
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }

                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try PLC as Backup Medium\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_PLC_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium\r\n");
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            if (matchingDataReq->waitingSecondConfirm)
            {
                /* Second Confirm arrived. Send confirm to upper layer depending on results */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_BOTH;
                if ((matchingDataReq->firstConfirmStatus == MAC_STATUS_SUCCESS) ||
                        (dcParams->status == MAC_STATUS_SUCCESS))
                {
                    /* At least one SUCCESS, send confirm with SUCCESS */
                    dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                }

                /* Send confirm to upper layer */
                sendConfirm = true;
            }
            else
            {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* RF confirm not expected on PLC_NO_BACKUP request. Ignore it */
            matchingDataReq->used = false;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacRfDataConfirm() called from a MEDIA_TYPE_REQ_PLC_NO_BACKUP request!!\r\n");
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
        default: /* RF only */
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
    }

    if (sendConfirm == true)
    {
        /* Release Data Req entry and send confirm to upper layer */
        matchingDataReq->used = false;
<#if MAC_SERIALIZATION_EN == true>
        if (matchingDataReq->serialDataRequest == true)
        {
            lMAC_WRP_StringifyDataConfirm(&dataConfirmParams);
        }
        else
        {
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
</#if>
    }
<#else>
    /* Fill Media Type */
    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;

    /* Release Data Req entry and send confirm to upper layer */
    matchingDataReq->used = false;
<#if MAC_SERIALIZATION_EN == true>
    if (matchingDataReq->serialDataRequest == true)
    {
        lMAC_WRP_StringifyDataConfirm(&dataConfirmParams);
    }
    else
    {
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacRfDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (lMAC_WRP_HyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu,
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_RF))
        {
            /* Same frame was received on PLC medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on PLC medium. Drop indication");
            return;
        }
    }

</#if>
    /* Copy diParams from Mac and fill Media Type */
    (void) memcpy((void *) &dataIndicationParams, (void *) diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
    dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyDataIndication(&dataIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfResetConfirm: Status: %u\r\n", rcParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_RESET_CONFIRM_PARAMS resetConfirmParams;

    if (hyalData.waitingSecondResetConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstResetConfirmStatus == MAC_STATUS_SUCCESS) &&
                (rcParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            resetConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
            }
        }

<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialResetRequest == true)
        {
            lMAC_WRP_StringifyResetConfirm(&resetConfirmParams);
            macWrpData.serialResetRequest = false;
        }
        else
        {
            if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
    }
<#else>
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialResetRequest == true)
    {
        lMAC_WRP_StringifyResetConfirm((void *)rcParams);
        macWrpData.serialResetRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacRfBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfBeaconNotify: Pan ID: %04X\r\n", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac and fill Media Type */
    (void) memcpy((void *) &notifyIndicationParams, (void *) bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));
    notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyBeaconNotIndication(&notifyIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfScanConfirm: Status: %u\r\n", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;

    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
        }

        /* Clear flag */
        macWrpData.scanRequestInProgress = false;

        /* Send confirm to upper layer */
<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialScanRequest == true)
        {
            lMAC_WRP_StringifyScanConfirm(&scanConfirmParams);
        }
        else
        {
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
<#else>
    /* Clear flag */
    macWrpData.scanRequestInProgress = false;

    /* Send confirm to upper layer */
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialScanRequest == true)
    {
        lMAC_WRP_StringifyScanConfirm((void *)scParams);
    }
    else
    {
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacRfStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfStartConfirm: Status: %u\r\n", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_START_CONFIRM_PARAMS startConfirmParams;

    if (hyalData.waitingSecondStartConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstStartConfirmStatus == MAC_STATUS_SUCCESS) &&
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            startConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            }
        }

<#if MAC_SERIALIZATION_EN == true>
        if (macWrpData.serialStartRequest == true)
        {
            lMAC_WRP_StringifyStartConfirm(&startConfirmParams);
            macWrpData.serialStartRequest = false;
        }
        else
        {
            if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
        }
</#if>
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
    }
<#else>
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.serialStartRequest == true)
    {
        lMAC_WRP_StringifyStartConfirm((void *)scParams);
        macWrpData.serialStartRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
        }
    }
<#else>
    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
    }
</#if>
</#if>
}

static void lMAC_WRP_CallbackMacRfCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfCommStatusIndication: Status: %u\r\n", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac and fill Media Type */
    (void) memcpy((void *) &commStatusIndicationParams, (void *) csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));
    commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifyCommStatusIndication(&commStatusIndicationParams);
</#if>
    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "lMAC_WRP_CallbackMacRfMacSnifferIndication:  MSDU:");

<#if MAC_SERIALIZATION_EN == true>
    lMAC_WRP_StringifySnifferIndication((void *)siParams);
</#if>
    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((void *)siParams);
    }
}

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: MAC Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index)
{
    /* Validate the request */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    macWrpData.state = MAC_WRP_STATE_NOT_READY;
    macWrpData.macWrpHandle = (MAC_WRP_HANDLE) 0U;
<#if MAC_SERIALIZATION_EN == true>
    macWrpData.macSerialHandle = (MAC_WRP_HANDLE) 1U;
    macWrpData.usiHandle = SRV_USI_HANDLE_INVALID;
    macWrpData.serialInitialize = false;
    macWrpData.serialResetRequest = false;
    macWrpData.serialStartRequest = false;
</#if>
<#if MAC_SERIALIZATION_EN == true || ADP_SERIALIZATION_EN == true>
    macWrpData.debugSetLength = 0;
</#if>
    macWrpData.scanRequestInProgress = false;
    (void) memset(&macWrpData.macWrpHandlers, 0, sizeof(MAC_WRP_HANDLERS));
    for (uint8_t i = 0U; i < MAC_WRP_DATA_REQ_QUEUE_SIZE; i++)
    {
        dataReqQueue[i].used = false;
    }

    return (SYS_MODULE_OBJ)0;
}

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index, MAC_WRP_BAND band)
{
<#if MAC_PLC_PRESENT == true>
    MAC_PLC_INIT plcInitData;
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_RF_INIT rfInitData;
</#if>

    /* Single instance allowed */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Set default HyAL variables */
    hyalData = hyalDataDefaults;
    (void) memset(hyALDuplicatesTable, 0, sizeof(hyALDuplicatesTable));

</#if>
<#if MAC_PLC_PRESENT == true>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Open: Initializing PLC MAC...\r\n");

    plcInitData.macPlcHandlers.macPlcDataConfirm = lMAC_WRP_CallbackMacPlcDataConfirm;
    plcInitData.macPlcHandlers.macPlcDataIndication = lMAC_WRP_CallbackMacPlcDataIndication;
    plcInitData.macPlcHandlers.macPlcResetConfirm = lMAC_WRP_CallbackMacPlcResetConfirm;
    plcInitData.macPlcHandlers.macPlcBeaconNotifyIndication = lMAC_WRP_CallbackMacPlcBeaconNotify;
    plcInitData.macPlcHandlers.macPlcScanConfirm = lMAC_WRP_CallbackMacPlcScanConfirm;
    plcInitData.macPlcHandlers.macPlcStartConfirm = lMAC_WRP_CallbackMacPlcStartConfirm;
    plcInitData.macPlcHandlers.macPlcCommStatusIndication = lMAC_WRP_CallbackMacPlcCommStatusIndication;
    plcInitData.macPlcHandlers.macPlcMacSnifferIndication = lMAC_WRP_CallbackMacPlcMacSnifferIndication;

    (void) memset(macPlcDeviceTable, 0, sizeof(macPlcDeviceTable));

    macPlcTables.macPlcDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_PLC;
    macPlcTables.macPlcDeviceTable = macPlcDeviceTable;

    plcInitData.macPlcTables = &macPlcTables;
    plcInitData.plcBand = (MAC_PLC_BAND) band;
    /* Get PAL index from configuration header */
    plcInitData.palPlcIndex = PAL_PLC_PHY_INDEX;

    MAC_PLC_Init(&plcInitData);

</#if>
<#if MAC_RF_PRESENT == true>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Open: Initializing RF MAC...\r\n");

    rfInitData.macRfHandlers.macRfDataConfirm = lMAC_WRP_CallbackMacRfDataConfirm;
    rfInitData.macRfHandlers.macRfDataIndication = lMAC_WRP_CallbackMacRfDataIndication;
    rfInitData.macRfHandlers.macRfResetConfirm = lMAC_WRP_CallbackMacRfResetConfirm;
    rfInitData.macRfHandlers.macRfBeaconNotifyIndication = lMAC_WRP_CallbackMacRfBeaconNotify;
    rfInitData.macRfHandlers.macRfScanConfirm = lMAC_WRP_CallbackMacRfScanConfirm;
    rfInitData.macRfHandlers.macRfStartConfirm = lMAC_WRP_CallbackMacRfStartConfirm;
    rfInitData.macRfHandlers.macRfCommStatusIndication = lMAC_WRP_CallbackMacRfCommStatusIndication;
    rfInitData.macRfHandlers.macRfMacSnifferIndication = lMAC_WRP_CallbackMacRfMacSnifferIndication;

    (void) memset(macRfPOSTable, 0, sizeof(macRfPOSTable));
    (void) memset(macRfDeviceTable, 0, sizeof(macRfDeviceTable));
    (void) memset(macRfDsnTable, 0, sizeof(macRfDsnTable));

    macRfTables.macRfDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_RF;
    macRfTables.macRfDsnTableSize = MAC_MAX_DSN_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTableSize = MAC_MAX_POS_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTable = macRfPOSTable;
    macRfTables.macRfDeviceTable = macRfDeviceTable;
    macRfTables.macRfDsnTable = macRfDsnTable;

    rfInitData.macRfTables = &macRfTables;
    /* Get PAL index from configuration header */
    rfInitData.palRfIndex = PAL_RF_PHY_INDEX;

    MAC_RF_Init(&rfInitData);

</#if>
    MAC_COMMON_Init();

    macWrpData.state = MAC_WRP_STATE_IDLE;

<#if !(HarmonyCore.SELECT_RTOS)?? || (HarmonyCore.SELECT_RTOS == "BareMetal")>
    macWrpData.nextTaskTimeMs = MAC_WRP_GetMsCounter() + G3_STACK_TASK_RATE_MS;

</#if>
    return macWrpData.macWrpHandle;
}

void MAC_WRP_SetCallbacks(MAC_WRP_HANDLE handle, MAC_WRP_HANDLERS* handlers)
{
    if ((handle == macWrpData.macWrpHandle) && (handlers != NULL))
    {
        macWrpData.macWrpHandlers = *handlers;
    }
}

void MAC_WRP_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ) 0)
    {
        /* Invalid object */
        return;
    }

<#if !(HarmonyCore.SELECT_RTOS)?? || (HarmonyCore.SELECT_RTOS == "BareMetal")>
    if (!MAC_COMMON_TimeIsPast((int32_t) macWrpData.nextTaskTimeMs))
    {
        /* Do nothing */
        return;
    }

    /* Time to execute MAC Wrapper task. Update time for next task. */
    macWrpData.nextTaskTimeMs += G3_STACK_TASK_RATE_MS;

</#if>
<#if MAC_SERIALIZATION_EN == true>
    if (macWrpData.usiHandle == SRV_USI_HANDLE_INVALID)
    {
        /* Open USI instance for MAC serialization and register callback */
        macWrpData.usiHandle = SRV_USI_Open(G3_MAC_WRP_SERIAL_USI_INDEX);
        SRV_USI_CallbackRegister(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, lMAC_WRP_CallbackUsiMacProtocol);
    }

    if ((macWrpData.serialInitialize == true) && (MAC_WRP_Status() == SYS_STATUS_READY))
    {
        /* Send MAC initialization confirm */
        macWrpData.serialInitialize = false;
        lMAC_WRP_StringifyMsgStatus(MAC_WRP_SERIAL_STATUS_SUCCESS, MAC_WRP_SERIAL_MSG_MAC_INITIALIZE);
    }

</#if>
<#if MAC_PLC_PRESENT == true>
    MAC_PLC_Tasks();
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_RF_Tasks();
<#else>
    (void) MAC_COMMON_GetMsCounter(); /* Just to avoid counter overflow */
</#if>
}

SYS_STATUS MAC_WRP_Status(void)
{
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    SYS_STATUS plcStatus = MAC_PLC_Status();
    SYS_STATUS rfStatus = MAC_RF_Status();
    if ((plcStatus == SYS_STATUS_UNINITIALIZED) || (rfStatus == SYS_STATUS_UNINITIALIZED))
    {
        return SYS_STATUS_UNINITIALIZED;
    }
    if ((plcStatus == SYS_STATUS_BUSY) || (rfStatus == SYS_STATUS_BUSY))
    {
        return SYS_STATUS_BUSY;
    }
    else if ((plcStatus == SYS_STATUS_READY) || (rfStatus == SYS_STATUS_READY))
    {
        return SYS_STATUS_READY;
    }
    else
    {
        return SYS_STATUS_ERROR;
    }
<#elseif MAC_PLC_PRESENT == true>
    return MAC_PLC_Status();
<#elseif MAC_RF_PRESENT == true>
    return MAC_RF_Status();
</#if>
}

void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirm;
    MAC_WRP_DATA_REQ_ENTRY *dataReqEntry;

<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM) drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle: 0x%02X Media Type: %02X): ", drParams->msduHandle, drParams->mediaType);

    /* Look for free Data Request Entry */
    dataReqEntry = lMAC_WRP_GetFreeDataReqEntry();

    if (dataReqEntry == NULL)
    {
        /* Too many data requests */
        /* Send confirm to upper layer and return */
        dataConfirm.msduHandle = drParams->msduHandle;
        dataConfirm.status = MAC_WRP_STATUS_QUEUE_FULL;
        dataConfirm.timestamp = 0;
        dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
<#if MAC_SERIALIZATION_EN == true>
        if (handle == macWrpData.macSerialHandle)
        {
            lMAC_WRP_StringifyDataConfirm(&dataConfirm);
        }
        else
        {
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }
</#if>

        return;
    }

    /* Accept request */
    /* Copy data to Mac struct (media type is not copied as it is the last field of drParams) */
    (void) memcpy((void *) &dataReqEntry->dataReqParams, (void *) drParams, sizeof(dataReqEntry->dataReqParams));
<#if MAC_SERIALIZATION_EN == true>
    if (handle == macWrpData.macSerialHandle)
    {
        dataReqEntry->serialDataRequest = true;
    }
    else
    {
        dataReqEntry->serialDataRequest = false;
    }

</#if>
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Copy MediaType */
    dataReqEntry->dataReqMediaType = drParams->mediaType;
    /* Copy Probing Interval */
    dataReqEntry->probingInterval = drParams->probingInterval;
    /* Copy data to backup buffer, just in case backup media has to be used, current pointer will not be valid later */
    if (drParams->msduLength <= HYAL_BACKUP_BUF_SIZE)
    {
        (void) memcpy(dataReqEntry->backupBuffer, drParams->msdu, drParams->msduLength);
    }

    switch (dataReqEntry->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            MAC_RF_DataRequest(&dataReqEntry->dataReqParams);
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            /* Set control variable */
            dataReqEntry->waitingSecondConfirm = false;
            /* Request on both Media */
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            MAC_RF_DataRequest(&dataReqEntry->dataReqParams);
            break;
        default: /* PLC only */
            dataReqEntry->dataReqMediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP;
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            break;
    }
<#elseif MAC_PLC_PRESENT == true>
    MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
<#elseif MAC_RF_PRESENT == true>
    MAC_RF_DataRequest(&dataReqEntry->dataReqParams);
</#if>
}

MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue)
{
<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        // Handle error
        pibValue->length = 0U;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_GetRequestSync: Attribute: %08X; Index: %u\r\n", attribute, index);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else if (lMAC_WRP_IsAttributeInPLCRange(attribute))
    {
        /* Get from PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* Get from RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_GetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
<#elseif MAC_PLC_PRESENT == true>
    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* RF Available IB has to be handled here */
        if (attribute == MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE)
        {
            pibValue->length = 1U;
            pibValue->value[0] = 0U;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Get from PLC MAC */
            return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
        }
    }
<#elseif MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* PLC Available IB has to be handled here */
        if (attribute == MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE)
        {
            pibValue->length = 1U;
            pibValue->value[0] = 0U;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Get from RF MAC */
            return (MAC_WRP_STATUS)(MAC_RF_GetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
        }
    }
</#if>
}

MAC_WRP_STATUS MAC_WRP_SetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_WRP_PIB_VALUE *pibValue)
{
<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        // Handle error
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pibValue->value, pibValue->length, "MAC_WRP_SetRequestSync: Attribute: %08X; Index: %u; Value: ", attribute, index);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else if (lMAC_WRP_IsAttributeInPLCRange(attribute))
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_SetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
<#elseif MAC_PLC_PRESENT == true>
    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
<#elseif MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_SetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
</#if>
}

void MAC_WRP_ResetRequest(MAC_WRP_HANDLE handle, MAC_WRP_RESET_REQUEST_PARAMS *rstParams)
{
<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_RESET_CONFIRM_PARAMS resetConfirm;
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            resetConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_ResetRequest: Set default PIB: %u\r\n", rstParams->setDefaultPib);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set control variable
    hyalData.waitingSecondResetConfirm = false;
</#if>
<#if MAC_PLC_PRESENT == true>
    // Reset PLC MAC
    MAC_PLC_ResetRequest((void *)rstParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Reset RF MAC
    MAC_RF_ResetRequest((void *)rstParams);
</#if>
    // Reset Common MAC if IB has to be reset
    if (rstParams->setDefaultPib)
    {
        MAC_COMMON_Reset();
    }
}

void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams)
{
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirm;

<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            scanConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }

        return;
    }

    if (macWrpData.scanRequestInProgress == true)
    {
        /* Scan request already in progress */
        /* Send confirm to upper layer and return */
        scanConfirm.status = MAC_WRP_STATUS_DENIED;
<#if MAC_SERIALIZATION_EN == true>
        if (handle == macWrpData.macSerialHandle)
        {
            lMAC_WRP_StringifyScanConfirm(&scanConfirm);
        }
        else
        {
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
            }
        }
<#else>
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }
</#if>

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_ScanRequest: Duration: %u\r\n", scanParams->scanDuration);

    // Set control variable
    macWrpData.scanRequestInProgress = true;
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    hyalData.waitingSecondScanConfirm = false;
</#if>
<#if MAC_SERIALIZATION_EN == true>
    if (handle == macWrpData.macSerialHandle)
    {
        macWrpData.serialScanRequest = true;
    }
    else
    {
        macWrpData.serialScanRequest = false;
    }

</#if>
<#if MAC_PLC_PRESENT == true>
    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((void *)scanParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Set RF MAC on Scan state
    MAC_RF_ScanRequest((void *)scanParams);
</#if>
}

void MAC_WRP_StartRequest(MAC_WRP_HANDLE handle, MAC_WRP_START_REQUEST_PARAMS *startParams)
{
<#if MAC_SERIALIZATION_EN == true>
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
<#else>
    if (handle != macWrpData.macWrpHandle)
</#if>
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_START_CONFIRM_PARAMS startConfirm;
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            startConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_StartRequest: Pan ID: %u\r\n", startParams->panId);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set control variable
    hyalData.waitingSecondStartConfirm = false;
</#if>
<#if MAC_PLC_PRESENT == true>
    // Start Network on PLC MAC
    MAC_PLC_StartRequest((void *)startParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Start Network on PLC MAC
    MAC_RF_StartRequest((void *)startParams);
</#if>
}

MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle)
{
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    return MAC_WRP_AVAILABLE_MAC_BOTH;
<#elseif MAC_PLC_PRESENT == true>
    return MAC_WRP_AVAILABLE_MAC_PLC;
<#elseif MAC_RF_PRESENT == true>
    return MAC_WRP_AVAILABLE_MAC_RF;
</#if>
}

<#if MAC_SERIALIZATION_EN == true || ADP_SERIALIZATION_EN == true>
uint32_t MAC_WRP_SerialParseGetRequest(uint8_t* pData, uint16_t* index)
{
    uint32_t attribute;
    uint16_t attrIndexAux;

    attribute = ((uint32_t) *pData++) << 24;
    attribute += ((uint32_t) *pData++) << 16;
    attribute += ((uint32_t) *pData++) << 8;
    attribute += (uint32_t) *pData++;

    attrIndexAux = ((uint16_t) *pData++) << 8;
    attrIndexAux += (uint16_t) *pData;
    *index = attrIndexAux;

    return attribute;
}

/* MISRA C-2012 deviation block start */
<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
/* MISRA C-2012 Rule 16.4 deviated 6 times. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */
<#else>
/* MISRA C-2012 Rule 16.4 deviated 4 times. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */
</#if>
<#if core.COVERITY_SUPPRESS_DEVIATION?? && core.COVERITY_SUPPRESS_DEVIATION>
<#if core.COMPILER_CHOICE == "XC32">
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
</#if>
#pragma coverity compliance block deviate "MISRA C-2012 Rule 16.4" "H3_MISRAC_2012_R_16_4_DR_1"
</#if>

uint8_t MAC_WRP_SerialStringifyGetConfirm (
    uint8_t *serialData,
    MAC_WRP_STATUS getStatus,
    MAC_WRP_PIB_ATTRIBUTE attribute,
    uint16_t index,
    uint8_t* pibValue,
    uint8_t pibLength
)
{
    uint8_t serialRspLen = 0;
<#if MAC_PLC_PRESENT == true>
    MAC_WRP_NEIGHBOUR_ENTRY* pNeighbourEntry;
    MAC_WRP_POS_ENTRY* pPosEntry;
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_WRP_POS_ENTRY_RF* pPosEntryRF;
</#if>

    serialData[serialRspLen++] = (uint8_t) getStatus;
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 24);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 16);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 8);
    serialData[serialRspLen++] = (uint8_t) attribute;
    serialData[serialRspLen++] = (uint8_t) (index >> 8);
    serialData[serialRspLen++] = (uint8_t) index;

    serialData[serialRspLen++] = pibLength;

    if (getStatus == MAC_WRP_STATUS_SUCCESS)
    {
        switch (attribute)
        {
            /* 8-bit IBs */
            case MAC_WRP_PIB_PROMISCUOUS_MODE:
            case MAC_WRP_PIB_POS_TABLE_ENTRY_TTL:
            case MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD:
            case MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE:
            case MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE:
<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_BSN:
            case MAC_WRP_PIB_DSN:
            case MAC_WRP_PIB_MAX_BE:
            case MAC_WRP_PIB_MAX_CSMA_BACKOFFS:
            case MAC_WRP_PIB_MAX_FRAME_RETRIES:
            case MAC_WRP_PIB_MIN_BE:
            case MAC_WRP_PIB_SECURITY_ENABLED:
            case MAC_WRP_PIB_TIMESTAMP_SUPPORTED:
            case MAC_WRP_PIB_HIGH_PRIORITY_WINDOW_SIZE:
            case MAC_WRP_PIB_FREQ_NOTCHING:
            case MAC_WRP_PIB_CSMA_FAIRNESS_LIMIT:
            case MAC_WRP_PIB_TMR_TTL:
            case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL:
            case MAC_WRP_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH:
            case MAC_WRP_PIB_A:
            case MAC_WRP_PIB_K:
            case MAC_WRP_PIB_MIN_CW_ATTEMPTS:
            case MAC_WRP_PIB_CENELEC_LEGACY_MODE:
            case MAC_WRP_PIB_FCC_LEGACY_MODE:
            case MAC_WRP_PIB_BROADCAST_MAX_CW_ENABLE:
            case MAC_WRP_PIB_PLC_DISABLE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE:
            case MAC_WRP_PIB_MANUF_LAST_RX_MOD_SCHEME:
            case MAC_WRP_PIB_MANUF_LAST_RX_MOD_TYPE:
            case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER:
            case MAC_WRP_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
            case MAC_WRP_PIB_MANUF_SLEEP_MODE:
            case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI:
</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_DSN_RF:
            case MAC_WRP_PIB_MAX_BE_RF:
            case MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF:
            case MAC_WRP_PIB_MAX_FRAME_RETRIES_RF:
            case MAC_WRP_PIB_MIN_BE_RF:
            case MAC_WRP_PIB_TIMESTAMP_SUPPORTED_RF:
            case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL_RF:
            case MAC_WRP_PIB_COUNTER_OCTETS_RF:
            case MAC_WRP_PIB_USE_ENHANCED_BEACON_RF:
            case MAC_WRP_PIB_EB_HEADER_IE_LIST_RF:
            case MAC_WRP_PIB_EB_FILTERING_ENABLED_RF:
            case MAC_WRP_PIB_EBSN_RF:
            case MAC_WRP_PIB_EB_AUTO_SA_RF:
            case MAC_WRP_PIB_OPERATING_MODE_RF:
            case MAC_WRP_PIB_DUTY_CYCLE_USAGE_RF:
            case MAC_WRP_PIB_DUTY_CYCLE_THRESHOLD_RF:
            case MAC_WRP_PIB_FREQUENCY_BAND_RF:
            case MAC_WRP_PIB_TRANSMIT_ATTEN_RF:
            case MAC_WRP_PIB_ADAPTIVE_POWER_STEP_RF:
            case MAC_WRP_PIB_ADAPTIVE_POWER_HIGH_BOUND_RF:
            case MAC_WRP_PIB_ADAPTIVE_POWER_LOW_BOUND_RF:
            case MAC_WRP_PIB_DISABLE_PHY_RF:
            case MAC_WRP_PIB_MANUF_SECURITY_RESET_RF:
            case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF:
            case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF:
            case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF:
            case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER_RF:
            case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI_RF:
</#if>
                serialData[serialRspLen++] = pibValue[0];
                break;

            /* 16-bit IBs */
            case MAC_WRP_PIB_PAN_ID:
            case MAC_WRP_PIB_SHORT_ADDRESS:
            case MAC_WRP_PIB_RC_COORD:
<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_POS_RECENT_ENTRIES:
            case MAC_WRP_PIB_MANUF_COORD_SHORT_ADDRESS:
            case MAC_WRP_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE:
            case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_COUNT:
            case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT:
            case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_PLC:
</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_CHANNEL_NUMBER_RF:
            case MAC_WRP_PIB_DUTY_CYCLE_PERIOD_RF:
            case MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF:
            case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT_RF:
            case MAC_WRP_PIB_POS_RECENT_ENTRIES_RF:
            case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_RF:
</#if>
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                break;

            /* 32-bit IBs */
<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_FRAME_COUNTER:
            case MAC_WRP_PIB_TX_DATA_PACKET_COUNT:
            case MAC_WRP_PIB_RX_DATA_PACKET_COUNT:
            case MAC_WRP_PIB_TX_CMD_PACKET_COUNT:
            case MAC_WRP_PIB_RX_CMD_PACKET_COUNT:
            case MAC_WRP_PIB_CSMA_FAIL_COUNT:
            case MAC_WRP_PIB_CSMA_NO_ACK_COUNT:
            case MAC_WRP_PIB_RX_DATA_BROADCAST_COUNT:
            case MAC_WRP_PIB_TX_DATA_BROADCAST_COUNT:
            case MAC_WRP_PIB_BAD_CRC_COUNT:
            case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT:
            case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT:
            case MAC_WRP_PIB_MANUF_RX_MAC_REPETITION_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT:
            case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT:
            case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT:
            case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT:
            case MAC_WRP_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT:
</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_FRAME_COUNTER_RF:
            case MAC_WRP_PIB_RETRY_COUNT_RF:
            case MAC_WRP_PIB_MULTIPLE_RETRY_COUNT_RF:
            case MAC_WRP_PIB_TX_FAIL_COUNT_RF:
            case MAC_WRP_PIB_TX_SUCCESS_COUNT_RF:
            case MAC_WRP_PIB_FCS_ERROR_COUNT_RF:
            case MAC_WRP_PIB_SECURITY_FAILURE_COUNT_RF:
            case MAC_WRP_PIB_DUPLICATE_FRAME_COUNT_RF:
            case MAC_WRP_PIB_RX_SUCCESS_COUNT_RF:
            case MAC_WRP_PIB_MANUF_ACK_TX_DELAY_RF:
            case MAC_WRP_PIB_MANUF_ACK_RX_WAIT_TIME_RF:
            case MAC_WRP_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF:
            case MAC_WRP_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF:
            case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT_RF:
            case MAC_WRP_PIB_MANUF_TX_DATA_PACKET_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_DATA_PACKET_COUNT_RF:
            case MAC_WRP_PIB_MANUF_TX_CMD_PACKET_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_CMD_PACKET_COUNT_RF:
            case MAC_WRP_PIB_MANUF_CSMA_FAIL_COUNT_RF:
            case MAC_WRP_PIB_MANUF_RX_DATA_BROADCAST_COUNT_RF:
            case MAC_WRP_PIB_MANUF_TX_DATA_BROADCAST_COUNT_RF:
            case MAC_WRP_PIB_MANUF_BAD_CRC_COUNT_RF:
</#if>
                lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], pibValue);
                serialRspLen += 4U;
                break;

            /* Tables and lists */
            case MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS:
                (void) memcpy(&serialData[serialRspLen], pibValue, 8U);
                serialRspLen += 8U;
                break;

<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_MANUF_DEVICE_TABLE:
</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_DEVICE_TABLE_RF:
</#if>
                /* panId */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                /* shortAddress */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], &pibValue[2]);
                serialRspLen += 2U;
                /* frameCounter */
                lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], &pibValue[4]);
                serialRspLen += 4U;
                break;

<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION:
            case MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION:
</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION_RF:
</#if>
                /* Version */
                (void) memcpy(&serialData[serialRspLen], pibValue, 6U);
                serialRspLen += 6U;
                break;

<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_NEIGHBOUR_TABLE:
            case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT:
                pNeighbourEntry = (void*) pibValue;
                serialData[serialRspLen++] = (uint8_t) (pNeighbourEntry->shortAddress >> 8);
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->shortAddress;
                (void) memcpy(&serialData[serialRspLen], pNeighbourEntry->toneMap.toneMap, (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U);
                serialRspLen += (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->modulationType;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->txGain;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->txRes;
                (void) memcpy(&serialData[serialRspLen], pNeighbourEntry->txCoef.txCoef, 6U);
                serialRspLen += 6U;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->modulationScheme;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->phaseDifferential;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->lqi;
                serialData[serialRspLen++] = (uint8_t) (pNeighbourEntry->tmrValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->tmrValidTime;
                /* Length has to be incremented by 2 due to bitfields in the entry are serialized in separate fields */
                serialData[7] = pibLength + 2U;
                break;

            case MAC_WRP_PIB_POS_TABLE:
            case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT:
                pPosEntry = (void*) pibValue;
                serialData[serialRspLen++] = (uint8_t) (pPosEntry->shortAddress >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntry->shortAddress;
                serialData[serialRspLen++] = (uint8_t) pPosEntry->lqi;
                serialData[serialRspLen++] = (uint8_t) (pPosEntry->posValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntry->posValidTime;
                break;

            case MAC_WRP_PIB_TONE_MASK:
                (void) memcpy(&serialData[serialRspLen], pibValue, (MAC_WRP_MAX_TONES + 7U) / 8U);
                serialRspLen += (MAC_WRP_MAX_TONES + 7U) / 8U;
                break;

            case MAC_WRP_PIB_MANUF_BAND_INFORMATION:
                /* flMax */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                /* band */
                serialData[serialRspLen++] = pibValue[2];
                /* tones */
                serialData[serialRspLen++] = pibValue[3];
                /* carriers */
                serialData[serialRspLen++] = pibValue[4];
                /* tonesInCarrier */
                serialData[serialRspLen++] = pibValue[5];
                /* flBand */
                serialData[serialRspLen++] = pibValue[6];
                /* maxRsBlocks */
                serialData[serialRspLen++] = pibValue[7];
                /* txCoefBits */
                serialData[serialRspLen++] = pibValue[8];
                /* pilotsFreqSpa */
                serialData[serialRspLen++] = pibValue[9];
                break;

            case MAC_WRP_PIB_MANUF_FORCED_TONEMAP:
            case MAC_WRP_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE:
                (void) memcpy(&serialData[serialRspLen], pibValue, 3U);
                serialRspLen += 3U;
                break;

            case MAC_WRP_PIB_MANUF_DEBUG_SET:
                (void) memcpy(&serialData[serialRspLen], pibValue, 7U);
                serialRspLen += 7U;
                break;

            case MAC_WRP_PIB_MANUF_DEBUG_READ:
                (void) memcpy(&serialData[serialRspLen], pibValue, macWrpData.debugSetLength);
                serialRspLen += macWrpData.debugSetLength;
                break;

</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_SEC_SECURITY_LEVEL_LIST_RF:
                /* 4 Byte entries. */
                (void) memcpy(&serialData[serialRspLen], pibValue, 4U);
                serialRspLen += 4U;
                break;

            case MAC_WRP_PIB_POS_TABLE_RF: /* 11 Byte entries. */
            case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT_RF:
                pPosEntryRF = (void*) pibValue;
                serialData[serialRspLen++] = (uint8_t) (pPosEntryRF->shortAddress >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->shortAddress;
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->forwardLqi;
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->reverseLqi;
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->dutyCycle;
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->forwardTxPowerOffset;
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->reverseTxPowerOffset;
                serialData[serialRspLen++] = (uint8_t) (pPosEntryRF->posValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->posValidTime;
                serialData[serialRspLen++] = (uint8_t) (pPosEntryRF->reverseLqiValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntryRF->reverseLqiValidTime;
                break;

</#if>
            case MAC_WRP_PIB_KEY_TABLE:
                /* response will be MAC_WRP_STATUS_UNAVAILABLE_KEY */
                break;

<#if MAC_PLC_PRESENT == true>
            case MAC_WRP_PIB_MANUF_SECURITY_RESET:
            case MAC_WRP_PIB_MANUF_RESET_MAC_STATS:
                /* Response will be mac_status_denied */
                break;

            case MAC_WRP_PIB_MANUF_PHY_PARAM:
                switch ((MAC_WRP_PHY_PARAM) index)
                {
                    case MAC_WRP_PHY_PARAM_VERSION:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL_BYTES:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL_ERRORS:
                    case MAC_WRP_PHY_PARAM_BAD_BUSY_TX:
                    case MAC_WRP_PHY_PARAM_TX_BAD_BUSY_CHANNEL:
                    case MAC_WRP_PHY_PARAM_TX_BAD_LEN:
                    case MAC_WRP_PHY_PARAM_TX_BAD_FORMAT:
                    case MAC_WRP_PHY_PARAM_TX_TIMEOUT:
                    case MAC_WRP_PHY_PARAM_RX_TOTAL:
                    case MAC_WRP_PHY_PARAM_RX_TOTAL_BYTES:
                    case MAC_WRP_PHY_PARAM_RX_RS_ERRORS:
                    case MAC_WRP_PHY_PARAM_RX_EXCEPTIONS:
                    case MAC_WRP_PHY_PARAM_RX_BAD_LEN:
                    case MAC_WRP_PHY_PARAM_RX_BAD_CRC_FCH:
                    case MAC_WRP_PHY_PARAM_RX_FALSE_POSITIVE:
                    case MAC_WRP_PHY_PARAM_RX_BAD_FORMAT:
                    case MAC_WRP_PHY_PARAM_TIME_BETWEEN_NOISE_CAPTURES:
                        lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], pibValue);
                        serialRspLen += 4U;
                        break;

                    case MAC_WRP_PHY_PARAM_LAST_MSG_RSSI:
                    case MAC_WRP_PHY_PARAM_ACK_TX_CFM:
                    case MAC_WRP_PHY_PARAM_LAST_MSG_DURATION:
                        lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                        serialRspLen += 2U;
                        break;

                    case MAC_WRP_PHY_PARAM_ENABLE_AUTO_NOISE_CAPTURE:
                    case MAC_WRP_PHY_PARAM_DELAY_NOISE_CAPTURE_AFTER_RX:
                    case MAC_WRP_PHY_PARAM_CFG_AUTODETECT_BRANCH:
                    case MAC_WRP_PHY_PARAM_CFG_IMPEDANCE:
                    case MAC_WRP_PHY_PARAM_RRC_NOTCH_ACTIVE:
                    case MAC_WRP_PHY_PARAM_RRC_NOTCH_INDEX:
                    case MAC_WRP_PHY_PARAM_PLC_DISABLE:
                    case MAC_WRP_PHY_PARAM_NOISE_PEAK_POWER:
                    case MAC_WRP_PHY_PARAM_LAST_MSG_LQI:
                    case MAC_WRP_PHY_PARAM_PREAMBLE_NUM_SYNCP:
                        serialData[serialRspLen++] = pibValue[0];
                        break;

                    default:
                        break;
                }
                break;

</#if>
<#if MAC_RF_PRESENT == true>
            case MAC_WRP_PIB_EB_PAYLOAD_IE_LIST_RF:
                /* This IB is an empty array */
                break;

            case MAC_WRP_PIB_MANUF_RESET_MAC_STATS_RF:
                /* Response will be mac_status_denied */
                break;

            case MAC_WRP_PIB_MANUF_PHY_PARAM_RF:
                switch ((MAC_WRP_PHY_PARAM_RF) index)
                {
                    case MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_FREQ_HZ:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL_BYTES:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TOTAL:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_TX:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_RX:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_CHN:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_LEN:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_FORMAT:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TIMEOUT:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_ABORTED:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_CFM_NOT_HANDLED:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL_BYTES:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_TOTAL:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_FALSE_POSITIVE:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_LEN:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FORMAT:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FCS_PAY:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_ABORTED:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_OVERRIDE:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_IND_NOT_HANDLED:
                        lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], pibValue);
                        serialRspLen += 4U;
                        break;

                    case MAC_WRP_RF_PHY_PARAM_DEVICE_ID:
                    case MAC_WRP_RF_PHY_PARAM_PHY_BAND_OPERATING_MODE:
                    case MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_NUM:
                    case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_US:
                    case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_SYMBOLS:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TURNAROUND_TIME:
                    case MAC_WRP_RF_PHY_PARAM_PHY_TX_PAY_SYMBOLS:
                    case MAC_WRP_RF_PHY_PARAM_PHY_RX_PAY_SYMBOLS:
                    case MAC_WRP_RF_PHY_PARAM_MAC_UNIT_BACKOFF_PERIOD:
                        lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                        serialRspLen += 2U;
                        break;

                    case MAC_WRP_RF_PHY_PARAM_DEVICE_RESET:
                    case MAC_WRP_RF_PHY_PARAM_TRX_RESET:
                    case MAC_WRP_RF_PHY_PARAM_TRX_SLEEP:
                    case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_DBM:
                    case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_SENSITIVITY:
                    case MAC_WRP_RF_PHY_PARAM_PHY_SENSITIVITY:
                    case MAC_WRP_RF_PHY_PARAM_PHY_MAX_TX_POWER:
                    case MAC_WRP_RF_PHY_PARAM_PHY_STATS_RESET:
                    case MAC_WRP_RF_PHY_PARAM_TX_FSK_FEC:
                    case MAC_WRP_RF_PHY_PARAM_TX_OFDM_MCS:
                    case MAC_WRP_RF_PHY_PARAM_SET_CONTINUOUS_TX_MODE:
                        serialData[serialRspLen++] = pibValue[0];
                        break;

                    case MAC_WRP_RF_PHY_PARAM_FW_VERSION:
                        (void) memcpy(&serialData[serialRspLen], pibValue, 6);
                        serialRspLen += 6U;
                        break;

                    default:
                        break;
                }
                break;

</#if>
            default:
                break;
        }
    }

    return serialRspLen;
}

MAC_WRP_PIB_ATTRIBUTE MAC_WRP_SerialParseSetRequest (
    uint8_t* pData,
    uint16_t* index,
    MAC_WRP_PIB_VALUE* pibValue
)
{
    uint8_t pibLenCnt = 0;
    uint32_t attrAux;
    MAC_WRP_PIB_ATTRIBUTE attribute;
    uint16_t attrIndexAux;
<#if MAC_PLC_PRESENT == true>
    MAC_WRP_NEIGHBOUR_ENTRY pNeighbourEntry;
    MAC_WRP_POS_ENTRY pPosEntry;
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_WRP_POS_ENTRY_RF pPosEntryRF;
</#if>

    attrAux = ((uint32_t) *pData++) << 24;
    attrAux += ((uint32_t) *pData++) << 16;
    attrAux += ((uint32_t) *pData++) << 8;
    attrAux += (uint32_t) *pData++;
    attribute = (MAC_WRP_PIB_ATTRIBUTE) attrAux;

    attrIndexAux = ((uint16_t) *pData++) << 8;
    attrIndexAux += (uint16_t) *pData++;
    *index = attrIndexAux;

    pibValue->length = *pData++;

    switch (attribute)
    {
        /* 8-bit IBs */
        case MAC_WRP_PIB_PROMISCUOUS_MODE:
        case MAC_WRP_PIB_POS_TABLE_ENTRY_TTL:
        case MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD:
        case MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE:
        case MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE:
<#if MAC_PLC_PRESENT == true>
        case MAC_WRP_PIB_BSN:
        case MAC_WRP_PIB_DSN:
        case MAC_WRP_PIB_MAX_BE:
        case MAC_WRP_PIB_MAX_CSMA_BACKOFFS:
        case MAC_WRP_PIB_MAX_FRAME_RETRIES:
        case MAC_WRP_PIB_MIN_BE:
        case MAC_WRP_PIB_HIGH_PRIORITY_WINDOW_SIZE:
        case MAC_WRP_PIB_FREQ_NOTCHING:
        case MAC_WRP_PIB_CSMA_FAIRNESS_LIMIT:
        case MAC_WRP_PIB_TMR_TTL:
        case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL:
        case MAC_WRP_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH:
        case MAC_WRP_PIB_A:
        case MAC_WRP_PIB_K:
        case MAC_WRP_PIB_MIN_CW_ATTEMPTS:
        case MAC_WRP_PIB_BROADCAST_MAX_CW_ENABLE:
        case MAC_WRP_PIB_PLC_DISABLE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE:
        case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER:
        case MAC_WRP_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
        case MAC_WRP_PIB_MANUF_SLEEP_MODE:
        case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI:
</#if>
<#if MAC_RF_PRESENT == true>
        case MAC_WRP_PIB_DSN_RF:
        case MAC_WRP_PIB_MAX_BE_RF:
        case MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF:
        case MAC_WRP_PIB_MAX_FRAME_RETRIES_RF:
        case MAC_WRP_PIB_MIN_BE_RF:
        case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL_RF:
        case MAC_WRP_PIB_EBSN_RF:
        case MAC_WRP_PIB_OPERATING_MODE_RF:
        case MAC_WRP_PIB_DUTY_CYCLE_USAGE_RF:
        case MAC_WRP_PIB_DUTY_CYCLE_THRESHOLD_RF:
        case MAC_WRP_PIB_FREQUENCY_BAND_RF:
        case MAC_WRP_PIB_TRANSMIT_ATTEN_RF:
        case MAC_WRP_PIB_ADAPTIVE_POWER_STEP_RF:
        case MAC_WRP_PIB_ADAPTIVE_POWER_HIGH_BOUND_RF:
        case MAC_WRP_PIB_ADAPTIVE_POWER_LOW_BOUND_RF:
        case MAC_WRP_PIB_DISABLE_PHY_RF:
        case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF:
        case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF:
        case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF:
        case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER_RF:
        case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI_RF:
</#if>
            pibValue->value[0] = *pData;
            break;

        /* 16-bit IBs */
        case MAC_WRP_PIB_PAN_ID:
        case MAC_WRP_PIB_SHORT_ADDRESS:
        case MAC_WRP_PIB_RC_COORD:
<#if MAC_PLC_PRESENT == true>
        case MAC_WRP_PIB_POS_RECENT_ENTRIES:
        case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_PLC:
</#if>
<#if MAC_RF_PRESENT == true>
        case MAC_WRP_PIB_CHANNEL_NUMBER_RF:
        case MAC_WRP_PIB_DUTY_CYCLE_PERIOD_RF:
        case MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF:
        case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT_RF:
        case MAC_WRP_PIB_POS_RECENT_ENTRIES_RF:
        case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_RF:
</#if>
            lMemcpyFromUsiEndianessUint16(pibValue->value, pData);
            break;

        /* 32-bit IBs */
<#if MAC_PLC_PRESENT == true>
        case MAC_WRP_PIB_FRAME_COUNTER:
        case MAC_WRP_PIB_TX_DATA_PACKET_COUNT:
        case MAC_WRP_PIB_RX_DATA_PACKET_COUNT:
        case MAC_WRP_PIB_TX_CMD_PACKET_COUNT:
        case MAC_WRP_PIB_RX_CMD_PACKET_COUNT:
        case MAC_WRP_PIB_CSMA_FAIL_COUNT:
        case MAC_WRP_PIB_CSMA_NO_ACK_COUNT:
        case MAC_WRP_PIB_RX_DATA_BROADCAST_COUNT:
        case MAC_WRP_PIB_TX_DATA_BROADCAST_COUNT:
        case MAC_WRP_PIB_BAD_CRC_COUNT:
        case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT:
        case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT:
        case MAC_WRP_PIB_MANUF_RX_MAC_REPETITION_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT:
        case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT:
        case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT:
        case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT:
        case MAC_WRP_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT:
</#if>
<#if MAC_RF_PRESENT == true>
        case MAC_WRP_PIB_FRAME_COUNTER_RF:
        case MAC_WRP_PIB_RETRY_COUNT_RF:
        case MAC_WRP_PIB_MULTIPLE_RETRY_COUNT_RF:
        case MAC_WRP_PIB_TX_FAIL_COUNT_RF:
        case MAC_WRP_PIB_TX_SUCCESS_COUNT_RF:
        case MAC_WRP_PIB_FCS_ERROR_COUNT_RF:
        case MAC_WRP_PIB_SECURITY_FAILURE_COUNT_RF:
        case MAC_WRP_PIB_DUPLICATE_FRAME_COUNT_RF:
        case MAC_WRP_PIB_RX_SUCCESS_COUNT_RF:
        case MAC_WRP_PIB_MANUF_ACK_TX_DELAY_RF:
        case MAC_WRP_PIB_MANUF_ACK_RX_WAIT_TIME_RF:
        case MAC_WRP_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF:
        case MAC_WRP_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF:
        case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT_RF:
        case MAC_WRP_PIB_MANUF_TX_DATA_PACKET_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_DATA_PACKET_COUNT_RF:
        case MAC_WRP_PIB_MANUF_TX_CMD_PACKET_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_CMD_PACKET_COUNT_RF:
        case MAC_WRP_PIB_MANUF_CSMA_FAIL_COUNT_RF:
        case MAC_WRP_PIB_MANUF_RX_DATA_BROADCAST_COUNT_RF:
        case MAC_WRP_PIB_MANUF_TX_DATA_BROADCAST_COUNT_RF:
        case MAC_WRP_PIB_MANUF_BAD_CRC_COUNT_RF:
</#if>
            lMemcpyFromUsiEndianessUint32(pibValue->value, pData);
            break;

        /* Tables and lists */
        case MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS:
            /* m_au8Address */
            (void) memcpy(pibValue->value, pData, 8);
            break;

        case MAC_WRP_PIB_KEY_TABLE:
            (void) memcpy(pibValue->value, pData, MAC_WRP_SECURITY_KEY_LENGTH);
            break;

<#if MAC_PLC_PRESENT == true>
        case MAC_WRP_PIB_NEIGHBOUR_TABLE:
            pNeighbourEntry.shortAddress = ((uint16_t) *pData++) << 8;
            pNeighbourEntry.shortAddress += (uint16_t) *pData++;
            (void) memcpy(pNeighbourEntry.toneMap.toneMap, pData, (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U);
            pData += (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U;
            pNeighbourEntry.modulationType = *pData++;
            pNeighbourEntry.txGain = *pData++;
            pNeighbourEntry.txRes = *pData++;
            (void) memcpy(pNeighbourEntry.txCoef.txCoef, pData, 6U);
            pData += 6U;
            pNeighbourEntry.modulationScheme = *pData++;
            pNeighbourEntry.phaseDifferential = *pData++;
            pNeighbourEntry.lqi  = *pData++;
            pNeighbourEntry.tmrValidTime = ((uint16_t) *pData++) << 8;
            pNeighbourEntry.tmrValidTime += (uint16_t) *pData;
            (void) memcpy((void *) pibValue->value, (void *) &pNeighbourEntry, sizeof(MAC_WRP_NEIGHBOUR_ENTRY));
            /* Struct saves 2 bytes with bit-fields */
            pibValue->length -= 2U;
            break;

        case MAC_WRP_PIB_POS_TABLE:
            pPosEntry.shortAddress = ((uint16_t) *pData++) << 8;
            pPosEntry.shortAddress += (uint16_t) *pData++;
            pPosEntry.lqi  = *pData++;
            pPosEntry.posValidTime = ((uint16_t) *pData++) << 8;
            pPosEntry.posValidTime += (uint16_t) *pData;
            (void) memcpy((void *) pibValue->value, (void *) &pPosEntry, sizeof(MAC_WRP_POS_ENTRY));
            break;

        case MAC_WRP_PIB_TONE_MASK:
            (void) memcpy(pibValue->value, pData, (MAC_WRP_MAX_TONES + 7U) / 8U);
            break;

        case MAC_WRP_PIB_MANUF_FORCED_TONEMAP:
        case MAC_WRP_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE:
            pibValue->value[pibLenCnt++] = *pData++;
            pibValue->value[pibLenCnt++] = *pData++;
            pibValue->value[pibLenCnt] = *pData;
            break;

        case MAC_WRP_PIB_MANUF_DEBUG_SET:
        {
            uint16_t debugLength;
            (void) memcpy(pibValue->value, pData, 7U);
            lMemcpyFromUsiEndianessUint16((void *) &debugLength, &pData[5]);
            if (debugLength > MAC_PIB_MAX_VALUE_LENGTH)
            {
                macWrpData.debugSetLength = MAC_PIB_MAX_VALUE_LENGTH;
            }
            else
            {
                macWrpData.debugSetLength = (uint8_t) debugLength;
            }
            break;
        }

</#if>
<#if MAC_RF_PRESENT == true>
        case MAC_WRP_PIB_POS_TABLE_RF: /* 11 Byte entries. */
            pPosEntryRF.shortAddress = ((uint16_t) *pData++) << 8;
            pPosEntryRF.shortAddress += (uint16_t) *pData++;
            pPosEntryRF.forwardLqi  = *pData++;
            pPosEntryRF.reverseLqi  = *pData++;
            pPosEntryRF.dutyCycle  = *pData++;
            pPosEntryRF.forwardTxPowerOffset = *pData++;
            pPosEntryRF.reverseTxPowerOffset = *pData++;
            pPosEntryRF.posValidTime = ((uint16_t) *pData++) << 8;
            pPosEntryRF.posValidTime += (uint16_t) *pData++;
            pPosEntryRF.reverseLqiValidTime = ((uint16_t) *pData++) << 8;
            pPosEntryRF.reverseLqiValidTime += (uint16_t) *pData;
            (void) memcpy((void *) pibValue->value, (void *) &pPosEntryRF, sizeof(MAC_WRP_POS_ENTRY_RF));
            break;

</#if>
<#if MAC_PLC_PRESENT == true>
        case MAC_WRP_PIB_SECURITY_ENABLED:
        case MAC_WRP_PIB_TIMESTAMP_SUPPORTED:
        case MAC_WRP_PIB_CENELEC_LEGACY_MODE:
        case MAC_WRP_PIB_FCC_LEGACY_MODE:
        case MAC_WRP_PIB_MANUF_DEVICE_TABLE:
        case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT:
        case MAC_WRP_PIB_MANUF_BAND_INFORMATION:
        case MAC_WRP_PIB_MANUF_COORD_SHORT_ADDRESS:
        case MAC_WRP_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE:
        case MAC_WRP_PIB_MANUF_LAST_RX_MOD_SCHEME:
        case MAC_WRP_PIB_MANUF_LAST_RX_MOD_TYPE:
        case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_COUNT:
        case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT:
        case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION:
        case MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION:
        case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT:
            /* MAC_WRP_STATUS_READ_ONLY */
            break;

        case MAC_WRP_PIB_MANUF_SECURITY_RESET:
            /* If length is 0 then DeviceTable is going to be reset else response will be MAC_WRP_STATUS_INVALID_PARAMETER */
            break;
        case MAC_WRP_PIB_MANUF_RESET_MAC_STATS:
            /* If length is 0 then MAC Statistics will be reset */
            break;

        case MAC_WRP_PIB_MANUF_PHY_PARAM:
            switch ((MAC_WRP_PHY_PARAM) attrIndexAux)
            {
                case MAC_WRP_PHY_PARAM_TX_TOTAL:
                case MAC_WRP_PHY_PARAM_TX_TOTAL_BYTES:
                case MAC_WRP_PHY_PARAM_TX_TOTAL_ERRORS:
                case MAC_WRP_PHY_PARAM_BAD_BUSY_TX:
                case MAC_WRP_PHY_PARAM_TX_BAD_BUSY_CHANNEL:
                case MAC_WRP_PHY_PARAM_TX_BAD_LEN:
                case MAC_WRP_PHY_PARAM_TX_BAD_FORMAT:
                case MAC_WRP_PHY_PARAM_TX_TIMEOUT:
                case MAC_WRP_PHY_PARAM_RX_TOTAL:
                case MAC_WRP_PHY_PARAM_RX_TOTAL_BYTES:
                case MAC_WRP_PHY_PARAM_RX_RS_ERRORS:
                case MAC_WRP_PHY_PARAM_RX_EXCEPTIONS:
                case MAC_WRP_PHY_PARAM_RX_BAD_LEN:
                case MAC_WRP_PHY_PARAM_RX_BAD_CRC_FCH:
                case MAC_WRP_PHY_PARAM_RX_FALSE_POSITIVE:
                case MAC_WRP_PHY_PARAM_RX_BAD_FORMAT:
                case MAC_WRP_PHY_PARAM_TIME_BETWEEN_NOISE_CAPTURES:
                    lMemcpyFromUsiEndianessUint32(pibValue->value, pData);
                    break;

                case MAC_WRP_PHY_PARAM_LAST_MSG_RSSI:
                case MAC_WRP_PHY_PARAM_ACK_TX_CFM:
                case MAC_WRP_PHY_PARAM_LAST_MSG_DURATION:
                    lMemcpyFromUsiEndianessUint16(pibValue->value, pData);
                    break;

                case MAC_WRP_PHY_PARAM_ENABLE_AUTO_NOISE_CAPTURE:
                case MAC_WRP_PHY_PARAM_DELAY_NOISE_CAPTURE_AFTER_RX:
                case MAC_WRP_PHY_PARAM_CFG_AUTODETECT_BRANCH:
                case MAC_WRP_PHY_PARAM_CFG_IMPEDANCE:
                case MAC_WRP_PHY_PARAM_RRC_NOTCH_ACTIVE:
                case MAC_WRP_PHY_PARAM_RRC_NOTCH_INDEX:
                case MAC_WRP_PHY_PARAM_PLC_DISABLE:
                case MAC_WRP_PHY_PARAM_LAST_MSG_LQI:
                case MAC_WRP_PHY_PARAM_PREAMBLE_NUM_SYNCP:
                    pibValue->value[0] = *pData;
                    break;

                case MAC_WRP_PHY_PARAM_VERSION:
                case MAC_WRP_PHY_PARAM_NOISE_PEAK_POWER:
                    /* MAC_WRP_STATUS_READ_ONLY */
                    break;

                default:
                    break;
            }
            break;

</#if>
<#if MAC_RF_PRESENT == true>
        case MAC_WRP_PIB_TIMESTAMP_SUPPORTED_RF:
        case MAC_WRP_PIB_DEVICE_TABLE_RF:
        case MAC_WRP_PIB_COUNTER_OCTETS_RF:
        case MAC_WRP_PIB_USE_ENHANCED_BEACON_RF:
        case MAC_WRP_PIB_EB_HEADER_IE_LIST_RF:
        case MAC_WRP_PIB_EB_PAYLOAD_IE_LIST_RF:
        case MAC_WRP_PIB_EB_FILTERING_ENABLED_RF:
        case MAC_WRP_PIB_EB_AUTO_SA_RF:
        case MAC_WRP_PIB_SEC_SECURITY_LEVEL_LIST_RF:
        case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION_RF:
        case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT_RF:
            /* MAC_WRP_STATUS_READ_ONLY */
            break;

        case MAC_WRP_PIB_MANUF_SECURITY_RESET_RF:
            /* If length is 0 then DeviceTableRF is going to be reset else response will be MAC_WRP_STATUS_INVALID_PARAMETER */
            break;
        case MAC_WRP_PIB_MANUF_RESET_MAC_STATS_RF:
            /* If length is 0 then MAC Statistics will be reset */
            break;

        case MAC_WRP_PIB_MANUF_PHY_PARAM_RF:
            switch ((MAC_WRP_PHY_PARAM_RF) attrIndexAux)
            {
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL_BYTES:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TOTAL:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_TX:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_RX:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_CHN:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_LEN:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_FORMAT:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TIMEOUT:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_ABORTED:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_CFM_NOT_HANDLED:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL_BYTES:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_TOTAL:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_FALSE_POSITIVE:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_LEN:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FORMAT:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FCS_PAY:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_ABORTED:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_OVERRIDE:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_IND_NOT_HANDLED:
                    lMemcpyFromUsiEndianessUint32(pibValue->value, pData);
                    break;

                case MAC_WRP_RF_PHY_PARAM_PHY_BAND_OPERATING_MODE:
                case MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_NUM:
                case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_US:
                case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_SYMBOLS:
                case MAC_WRP_RF_PHY_PARAM_PHY_TX_PAY_SYMBOLS:
                case MAC_WRP_RF_PHY_PARAM_PHY_RX_PAY_SYMBOLS:
                    lMemcpyFromUsiEndianessUint16(pibValue->value, pData);
                    break;

                case MAC_WRP_RF_PHY_PARAM_DEVICE_RESET:
                case MAC_WRP_RF_PHY_PARAM_TRX_RESET:
                case MAC_WRP_RF_PHY_PARAM_TRX_SLEEP:
                case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_DBM:
                case MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_SENSITIVITY:
                case MAC_WRP_RF_PHY_PARAM_PHY_STATS_RESET:
                case MAC_WRP_RF_PHY_PARAM_TX_FSK_FEC:
                case MAC_WRP_RF_PHY_PARAM_TX_OFDM_MCS:
                case MAC_WRP_RF_PHY_PARAM_SET_CONTINUOUS_TX_MODE:
                    pibValue->value[0] = *pData;
                    break;

                case MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_FREQ_HZ:
                case MAC_WRP_RF_PHY_PARAM_FW_VERSION:
                case MAC_WRP_RF_PHY_PARAM_DEVICE_ID:
                case MAC_WRP_RF_PHY_PARAM_PHY_SENSITIVITY:
                case MAC_WRP_RF_PHY_PARAM_PHY_MAX_TX_POWER:
                case MAC_WRP_RF_PHY_PARAM_PHY_TURNAROUND_TIME:
                case MAC_WRP_RF_PHY_PARAM_MAC_UNIT_BACKOFF_PERIOD:
                    /* MAC_WRP_STATUS_READ_ONLY */
                    break;

                default:
                    break;
            }
            break;

</#if>
        default:
            break;
    }

    return attribute;
}

<#if core.COVERITY_SUPPRESS_DEVIATION?? && core.COVERITY_SUPPRESS_DEVIATION>
#pragma coverity compliance end_block "MISRA C-2012 Rule 16.4"
<#if core.COMPILER_CHOICE == "XC32">
#pragma GCC diagnostic pop
</#if>
</#if>
/* MISRA C-2012 deviation block end */

uint8_t MAC_WRP_SerialStringifySetConfirm (
    uint8_t *serialData,
    MAC_WRP_STATUS setStatus,
    MAC_WRP_PIB_ATTRIBUTE attribute,
    uint16_t index
)
{
    uint8_t serialRspLen = 0;

    serialData[serialRspLen++] = (uint8_t) setStatus;
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 24);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 16);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 8);
    serialData[serialRspLen++] = (uint8_t) attribute;
    serialData[serialRspLen++] = (uint8_t) (index >> 8);
    serialData[serialRspLen++] = (uint8_t) index;

    return serialRspLen;
}

</#if>
uint32_t MAC_WRP_GetMsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetMsCounter();
}

bool MAC_WRP_TimeIsPast(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPast(timeValue);
}

uint32_t MAC_WRP_GetSecondsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetSecondsCounter();
}

bool MAC_WRP_TimeIsPastSeconds(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPastSeconds(timeValue);
}

/*******************************************************************************
 End of File
*/
