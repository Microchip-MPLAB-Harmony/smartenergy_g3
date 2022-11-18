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
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
typedef struct
{
    uint16_t srcAddress;
    uint16_t msduLen;
    uint16_t crc;
    uint8_t mediaType;
} HYAL_DUPLICATES_ENTRY;

/* Buffer size to store data to be sent as Mac Data Request */
#define HYAL_BACKUP_BUF_SIZE   400

typedef struct
{
  MAC_DATA_REQUEST_PARAMS dataReqParams;
  MAC_WRP_MEDIA_TYPE_REQUEST dataReqMediaType;
  uint8_t backupBuffer[HYAL_BACKUP_BUF_SIZE];
  MAC_STATUS firstConfirmStatus;
  bool waitingSecondConfirm;
  bool used;
} HYAL_DATA_REQUEST;

/* Data Request Queue size */
#define HYAL_DATA_REQ_QUEUE_SIZE   2

typedef struct
{
  // Data Service Control
  HYAL_DATA_REQUEST dataReqQueue[HYAL_DATA_REQ_QUEUE_SIZE];
  MAC_STATUS firstScanConfirmStatus;
  bool waitingSecondScanConfirm;
  MAC_STATUS firstResetConfirmStatus;
  bool waitingSecondResetConfirm;
  MAC_STATUS firstStartConfirmStatus;
  bool waitingSecondStartConfirm;
} HYAL_DATA;

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

<#if MAC_PLC_PRESENT == true>
#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    ${MAC_PLC_DEVICE_TABLE_SIZE?string}

static MAC_PLC_TABLES macPlcTables;
MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

</#if>
<#if MAC_RF_PRESENT == true>
#define MAC_MAX_POS_TABLE_ENTRIES_RF        ${MAC_RF_POS_TABLE_SIZE?string}
#define MAC_MAX_DSN_TABLE_ENTRIES_RF        ${MAC_RF_DSN_TABLE_SIZE?string}
#define MAC_MAX_DEVICE_TABLE_ENTRIES_RF     ${MAC_RF_DEVICE_TABLE_SIZE?string}

static MAC_RF_TABLES macRfTables;
MAC_RF_POS_TABLE_ENTRY macRfPOSTable[MAC_MAX_POS_TABLE_ENTRIES_RF];
MAC_DEVICE_TABLE_ENTRY macRfDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_RF];
MAC_RF_DSN_TABLE_ENTRY macRfDsnTable[MAC_MAX_DSN_TABLE_ENTRIES_RF];

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

#define HYAL_DUPLICATES_TABLE_SIZE   3

static HYAL_DUPLICATES_ENTRY hyALDuplicatesTable[HYAL_DUPLICATES_TABLE_SIZE] = {{0}};

static const HYAL_DATA hyalDataDefaults = {
  {{{{MAC_ADDRESS_MODE_NO_ADDRESS}}}}, // dataReqQueue
  MAC_STATUS_SUCCESS, // firstScanConfirmStatus
  false, // waitingSecondScanConfirm
  MAC_STATUS_SUCCESS, // firstResetConfirmStatus
  false, // waitingSecondResetConfirm
  MAC_STATUS_SUCCESS, // firstStartConfirmStatus
  false, // waitingSecondStartConfirm
};

static HYAL_DATA hyalData;

// *****************************************************************************
// *****************************************************************************
// Section: local functions
// *****************************************************************************
// *****************************************************************************

static uint16_t _hyalCrc16(const uint8_t *dataBuf, uint32_t length)
{
    uint16_t crc = 0;

    // polynom(16): X16 + X12 + X5 + 1 = 0x1021
    while (length--)
    {
        crc = crc16_tab[(crc >> 8) ^ (*dataBuf ++)] ^ ((crc & 0xFF) << 8);
    }
    return crc;
}

static bool _hyalCheckDuplicates(uint16_t srcAddr, uint8_t *msdu, uint16_t msduLen, uint8_t mediaType)
{
    bool duplicate = false;
    uint8_t index = 0;
    uint16_t crc;

    // Calculate CRC for incoming frame
    crc = _hyalCrc16(msdu, msduLen);

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
        memmove(&hyALDuplicatesTable[1], &hyALDuplicatesTable[0],
            (HYAL_DUPLICATES_TABLE_SIZE - 1) * sizeof(HYAL_DUPLICATES_ENTRY));
        // Populate the new entry.
        hyALDuplicatesTable[0].srcAddress = srcAddr;
        hyALDuplicatesTable[0].msduLen = msduLen;
        hyALDuplicatesTable[0].crc = crc;
        hyALDuplicatesTable[0].mediaType = mediaType;
    }

    // Return duplicate or not
    return duplicate;
}

static HYAL_DATA_REQUEST *_hyalGetFreeDataReqEntry(void)
{
    uint8_t index;
    HYAL_DATA_REQUEST *found = NULL;

    for (index = 0; index < HYAL_DATA_REQ_QUEUE_SIZE; index++)
    {
        if (!hyalData.dataReqQueue[index].used)
        {
            found = &hyalData.dataReqQueue[index];
            hyalData.dataReqQueue[index].used = true;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_hyalGetFreeDataReqEntry() Found free HyALDataReqQueueEntry on index %u", index);
            break;
        }
    }

    return found;
}

static HYAL_DATA_REQUEST *_hyalGetDataReqEntryByHandle(uint8_t handle)
{
    uint8_t index;
    HYAL_DATA_REQUEST *found = NULL;

    for (index = 0; index < HYAL_DATA_REQ_QUEUE_SIZE; index++)
    {
        if (hyalData.dataReqQueue[index].used && 
            (hyalData.dataReqQueue[index].dataReqParams.msduHandle == handle))
        {
            found = &hyalData.dataReqQueue[index];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_hyalGetDataReqEntryByHandle() Found matching HyALDataReqQueueEntry on index %u, Handle: 0x%02X", index, handle);
            break;
        }
    }

    return found;
}

static bool _macWrpIsAttributeInPLCRange(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check attribute ID range to distinguish between PLC and RF MAC */
    if (attribute < 0x00000200)
    {
        /* Standard PLC MAC IB */
        return true;
    }
    else if (attribute < 0x00000400)
    {
        /* Standard RF MAC IB */
        return false;
    }
    else if (attribute < 0x08000200)
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

</#if>
static bool _macWrpIsSharedAttribute(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check if attribute in the list of shared between MAC layers */
    if ((attribute == MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS) ||
        (attribute == MAC_WRP_PIB_PAN_ID) ||
        (attribute == MAC_WRP_PIB_PROMISCUOUS_MODE) ||
        (attribute == MAC_WRP_PIB_SHORT_ADDRESS) ||
        (attribute == MAC_WRP_PIB_POS_TABLE_ENTRY_TTL) ||
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

/* ------------------------------------------------ */
/* ---------- Callbacks from MAC Layers ----------- */
/* ------------------------------------------------ */

<#if MAC_PLC_PRESENT == true>
static void _Callback_MacPlcDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcDataConfirm() Handle: 0x%02X Status: %u", dcParams->msduHandle, (uint8_t)dcParams->status);

    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    HYAL_DATA_REQUEST *matchingDataReq;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;

    /* Get Data Request entry matching confirm */
    matchingDataReq = _hyalGetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "_Callback_MacPlcDataConfirm() Confirm does not match any previous request!!");
        return;
    }

    /* Copy dcParams from Mac. Media Type will be filled later */
    memcpy(&dataConfirmParams, dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));
    
    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Send confirm to upper layer */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                /* Release Data Req entry and send confirm */
                matchingDataReq->used = false;
                if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                }
            }
            else
            {
                /* Check Dest Address mode and/or RF POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for RF POS Table entry for %0004X", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_RF_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF, 
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }
                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try RF as Backup Medium");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_RF_DataRequest(&matchingDataReq->dataReqParams);
                }
                else {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium");
                    /* Send confirm to upper layer */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
                }
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            /* PLC was used as backup medium. Send confirm to upper layer */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
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
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
                }
            }
            else {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* Send confirm to upper layer */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* PLC confirm not expected on RF_NO_BACKUP request. Ignore it */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "_Callback_MacPlcDataConfirm() called from a MEDIA_TYPE_REQ_RF_NO_BACKUP request!!");
            break;
        default: /* PLC only */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
            break;
    }
<#else>
    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
    {
        /* Copy dcParams from Mac and fill Media Type */
        memcpy(&dataConfirmParams, dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));
        dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
</#if>
}

static void _Callback_MacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (_hyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu, 
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_PLC))
        {
            /* Same frame was received on RF medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on RF medium. Drop indication");
            return;
        }
    }

    /* Copy diParams from Mac. Media Type will be filled later */
    memcpy(&dataIndicationParams, diParams, sizeof(MAC_DATA_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
<#else>
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        /* Copy diParams from Mac and fill Media Type */
        memcpy(&dataIndicationParams, diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
        dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
</#if>
}

static void _Callback_MacPlcResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcResetConfirm: Status: %u", rcParams->status);

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
            if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
            }
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
                if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
                }
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
                if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
                }
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((MAC_WRP_RESET_CONFIRM_PARAMS *)rcParams);
    }
</#if>
}

static void _Callback_MacPlcBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcBeaconNotify: Pan ID: %04X", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac. Media Type will be filled later */
    memcpy(&notifyIndicationParams, bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void _Callback_MacPlcScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcScanConfirm: Status: %u", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;
    
    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((MAC_WRP_SCAN_CONFIRM_PARAMS *)scParams);
    }
</#if>
}

static void _Callback_MacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcStartConfirm: Status: %u", scParams->status);

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
            if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
            }
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
                if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
                }
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
                if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
                }
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((MAC_WRP_START_CONFIRM_PARAMS *)scParams);
    }
</#if>
}

static void _Callback_MacPlcCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcCommStatusIndication: Status: %u", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac. Media Type will be filled later */
    memcpy(&commStatusIndicationParams, csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void _Callback_MacPlcMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "_Callback_MacPlcMacSnifferIndication:  MSDU:");

    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((MAC_WRP_SNIFFER_INDICATION_PARAMS *)siParams);
    }
}

</#if>
<#if MAC_RF_PRESENT == true>
static void _Callback_MacRfDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacRfDataConfirm() Handle: 0x%02X Status: %u", dcParams->msduHandle, (uint8_t)dcParams->status);

    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    HYAL_DATA_REQUEST *matchingDataReq;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;

    /* Get Data Request entry matching confirm */
    matchingDataReq = _hyalGetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "_Callback_MacRfDataConfirm() Confirm does not match any previous request!!");
        return;
    }

    /* Copy dcParams from Mac. Media Type will be filled later */
    memcpy(&dataConfirmParams, dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));
    
    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            /* RF was used as backup medium. Send confirm to upper layer */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Send confirm to upper layer */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                /* Release Data Req entry and send confirm */
                matchingDataReq->used = false;
                if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                }
            }
            else
            {
                /* Check Dest Address mode and/or RF POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for PLC POS Table entry for %0004X", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT, 
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }
                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try PLC as Backup Meduim");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_PLC_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium");
                    /* Send confirm to upper layer */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
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
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                    /* Release Data Req entry and send confirm */
                    matchingDataReq->used = false;
                    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
                    {
                        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
                    }
                }
            }
            else
            {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* RF confirm not expected after a PLC_NO_BACKUP request. Ignore it */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "_Callback_MacPlcDataConfirm() called from a MEDIA_TYPE_REQ_PLC_NO_BACKUP request!!");
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* Send confirm to upper layer */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
            break;
        default: /* RF only */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Release Data Req entry and send confirm */
            matchingDataReq->used = false;
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
            }
            break;
    }
<#else>
    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL) {
        /* Copy dcParams from Mac and fill Media Type */
        memcpy(&dataConfirmParams, dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));
        dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
</#if>
}

static void _Callback_MacRfDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacRfDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (_hyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu, 
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_RF))
        {
            /* Same frame was received on PLC medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on PLC medium. Drop indication");
            return;
        }
    }

    /* Copy diParams from Mac. Media Type will be filled later */
    memcpy(&dataIndicationParams, diParams, sizeof(MAC_DATA_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
<#else>
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        /* Copy diParams from Mac and fill Media Type */
        memcpy(&dataIndicationParams, diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
        dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
</#if>
}

static void _Callback_MacRfResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacRfResetConfirm: Status: %u", rcParams->status);

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
            if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
            }
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
                if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
                }
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
                if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
                }
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((MAC_WRP_RESET_CONFIRM_PARAMS *)rcParams);
    }
</#if>
}

static void _Callback_MacRfBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacRfBeaconNotify: Pan ID: %04X", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac. Media Type will be filled later */
    memcpy(&notifyIndicationParams, bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void _Callback_MacRfScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacRfScanConfirm: Status: %u", scParams->status);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;
    
    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((MAC_WRP_SCAN_CONFIRM_PARAMS *)scParams);
    }
</#if>
}

static void _Callback_MacRfStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacRfStartConfirm: Status: %u", scParams->status);

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
            if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
            }
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
                if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
                }
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
                if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
                {
                    macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
                }
            }
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
    }
<#else>
    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((MAC_WRP_START_CONFIRM_PARAMS *)scParams);
    }
</#if>
}

static void _Callback_MacRfCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacRfCommStatusIndication: Status: %u", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac. Media Type will be filled later */
    memcpy(&commStatusIndicationParams, csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void _Callback_MacRfMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "_Callback_MacRfMacSnifferIndication:  MSDU:");

    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((MAC_WRP_SNIFFER_INDICATION_PARAMS *)siParams);
    }
}

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    bool initError = false;

    /* Validate the request */
    if (index >= MAC_WRP_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (macWrpData.inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    macWrpData.inUse = true;
    macWrpData.state = MAC_WRP_STATE_NOT_READY;

    if (initError)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    else
    {
        return (SYS_MODULE_OBJ)0; 
    }
}

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index)
{
    // Single instance allowed
    if (index >= MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }
    else
    {
        macWrpData.state = MAC_WRP_STATE_IDLE;
        macWrpData.macWrpHandle = (MAC_WRP_HANDLE)0;
        reuturn macWrpData.macWrpHandle;
    }
}

void MAC_WRP_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)0)
    {
        // Invalid object
        return;
    }
<#if MAC_PLC_PRESENT == true>
    MAC_PLC_Tasks();
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_RF_Tasks();
</#if>
}

void MAC_WRP_Init(MAC_WRP_HANDLE handle, MAC_WRP_INIT *init)
{
<#if MAC_PLC_PRESENT == true>
    MAC_PLC_INIT plcInitData;
</#if>
<#if MAC_RF_PRESENT == true>
    MAC_RF_INIT rfInitData;
</#if>

    /* Validate the request */
    if (handle != macWrpData.macWrpHandle)
    {
        return;
    }

    /* Set init data */
    macWrpData.macWrpHandlers = init->macWrpHandlers;
    macWrpData.plcBand = init->plcBand;

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set default HyAL variables
    hyalData = hyalDataDefaults;

</#if>
<#if MAC_PLC_PRESENT == true>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Init: Initializing PLC MAC...");

    plcInitData.macPlcHandlers.macPlcDataConfirm = _Callback_MacPlcDataConfirm;
    plcInitData.macPlcHandlers.macPlcDataIndication = _Callback_MacPlcDataIndication;
    plcInitData.macPlcHandlers.macPlcResetConfirm = _Callback_MacPlcResetConfirm;
    plcInitData.macPlcHandlers.macPlcBeaconNotifyIndication = _Callback_MacPlcBeaconNotify;
    plcInitData.macPlcHandlers.macPlcScanConfirm = _Callback_MacPlcScanConfirm;
    plcInitData.macPlcHandlers.macPlcStartConfirm = _Callback_MacPlcStartConfirm;
    plcInitData.macPlcHandlers.macPlcCommStatusIndication = _Callback_MacPlcCommStatusIndication;
    plcInitData.macPlcHandlers.macPlcMacSnifferIndication = _Callback_MacPlcMacSnifferIndication;

    memset(macPlcDeviceTable, 0, sizeof(macPlcDeviceTable));

    macPlcTables.macPlcDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_PLC;
    macPlcTables.macPlcDeviceTable = macPlcDeviceTable;

    plcInitData.macPlcTables = &macPlcTables;
    plcInitData.plcBand = (MAC_PLC_BAND)init->plcBand;

    MAC_PLC_Init(&plcInitData);

</#if>
<#if MAC_RF_PRESENT == true>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Init: Initializing RF MAC...");

    rfInitData.macRfHandlers.macRfDataConfirm = _Callback_MacRfDataConfirm;
    rfInitData.macRfHandlers.macRfDataIndication = _Callback_MacRfDataIndication;
    rfInitData.macRfHandlers.macRfResetConfirm = _Callback_MacRfResetConfirm;
    rfInitData.macRfHandlers.macRfBeaconNotifyIndication = _Callback_MacRfBeaconNotify;
    rfInitData.macRfHandlers.macRfScanConfirm = _Callback_MacRfScanConfirm;
    rfInitData.macRfHandlers.macRfStartConfirm = _Callback_MacRfStartConfirm;
    rfInitData.macRfHandlers.macRfCommStatusIndication = _Callback_MacRfCommStatusIndication;
    rfInitData.macRfHandlers.macRfMacSnifferIndication = _Callback_MacRfMacSnifferIndication;

    memset(macRfPOSTable, 0, sizeof(macRfPOSTable));
    memset(macRfDeviceTable, 0, sizeof(macRfDeviceTable));
    memset(macRfDsnTable, 0, sizeof(macRfDsnTable));

    macRfTables.macRfDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_RF;
    macRfTables.macRfDsnTableSize = MAC_MAX_DSN_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTableSize = MAC_MAX_POS_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTable = macRfPOSTable;
    macRfTables.macRfDeviceTable = macRfDeviceTable;
    macRfTables.macRfDsnTable = macRfDsnTable;

    rfInitData.macRfTables = &macRfTables;

    MAC_RF_Init(&rfInitData);

</#if>
    MAC_COMMON_Init();
}

void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirm;

    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    HYAL_DATA_REQUEST *hyalDataReq;

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle: 0x%02X Media Type: %02X): ", drParams->msduHandle, drParams->mediaType);

    /* Look for free Data Request Entry */
    hyalDataReq = _hyalGetFreeDataReqEntry();
    
    if (hyalDataReq == NULL)
    {
        /* Too many data requests */
        /* Send confirm to upper layer */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_QUEUE_FULL;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }
    }
    else
    {
        /* Accept request */
        /* Copy parameters from MacWrp struct to Mac struct */
        memcpy(&hyalDataReq->dataReqParams, drParams, sizeof(hyalDataReq->dataReqParams));
        /* Copy MediaType */
        hyalDataReq->dataReqMediaType = drParams->mediaType;
        /* Copy data to backup buffer, just in case backup media has to be used, current pointer will not be valid later */
        if (drParams->msduLength <= HYAL_BACKUP_BUF_SIZE)
        {
            memcpy(hyalDataReq->backupBuffer, drParams->msdu, drParams->msduLength);
        }

        /* Different handling for Broadcast and Unicast requests */
        if ((drParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT) &&
            (drParams->destAddress.shortAddress == MAC_WRP_SHORT_ADDRESS_BROADCAST))
        {
            /* Broadcast */
            /* Overwrite MediaType to both */
            hyalDataReq->dataReqMediaType = MAC_WRP_MEDIA_TYPE_REQ_BOTH;
            /* Set control variable */
            hyalDataReq->waitingSecondConfirm = false;
            /* Request on both Media */
            MAC_PLC_DataRequest(&hyalDataReq->dataReqParams);
            MAC_RF_DataRequest(&hyalDataReq->dataReqParams);
        }
        else
        {
            /* Unicast */
            switch (hyalDataReq->dataReqMediaType)
            {
                case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
                    MAC_PLC_DataRequest(&hyalDataReq->dataReqParams);
                    break;
                case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
                    MAC_RF_DataRequest(&hyalDataReq->dataReqParams);
                    break;
                case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
                    /* Set control variable */
                    hyalDataReq->waitingSecondConfirm = false;
                    /* Request on both Media */
                    MAC_PLC_DataRequest(&hyalDataReq->dataReqParams);
                    MAC_RF_DataRequest(&hyalDataReq->dataReqParams);
                    break;
                case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
                    MAC_PLC_DataRequest(&hyalDataReq->dataReqParams);
                    break;
                case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
                    MAC_RF_DataRequest(&hyalDataReq->dataReqParams);
                    break;
                default: /* PLC only */
                    hyalDataReq->dataReqMediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP;
                    MAC_PLC_DataRequest(&hyalDataReq->dataReqParams);
                    break;
            }
        }
    }
<#elseif MAC_PLC_PRESENT == true>
    MAC_DATA_REQUEST_PARAMS dataReq;
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle %02X): ", drParams->msduHandle);
    // Copy data to Mac struct (media type is not copied as it is the last field of pParameters)
    memcpy(&dataReq, drParams, sizeof(dataReq));
    MAC_PLC_DataRequest(&dataReq);
<#elseif MAC_RF_PRESENT == true>
    MAC_DATA_REQUEST_PARAMS dataReq;
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle %02X): ", drParams->msduHandle);
    // Copy data to Mac struct (media type is not copied as it is the last field of pParameters)
    memcpy(&dataReq, drParams, sizeof(dataReq));
    MAC_RF_DataRequest(&dataReq);
</#if>
}

MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue)
{
    if (handle != macWrpData.macWrpHandle)
    {
        // Handle error
        pibValue->length = 0;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_GetRequestSync: Attribute: %08X; Index: %u", attribute, index);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
    else if (_macWrpIsAttributeInPLCRange(attribute))
    {
        /* Get from PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* Get from RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_GetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
<#elseif MAC_PLC_PRESENT == true>
    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* RF Available IB has to be handled here */
        if (eAttribute == MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE) 
        {
            pibValue->length = 1;
            pibValue->value[0] = 0;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else 
        {
            /* Get from PLC MAC */
            return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)eAttribute, u16Index, (MAC_PIB_VALUE *)pibValue));
        }
    }
<#elseif MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common or RF MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* PLC Available IB has to be handled here */
        if (eAttribute == MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE) 
        {
            pibValue->length = 1;
            pibValue->value[0] = 0;
            return MAC_WRP_STATUS_SUCCESS;  
        }
        else 
        {
            /* Get from RF MAC */
            return (MAC_WRP_STATUS)(MAC_RF_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)eAttribute, u16Index, (MAC_PIB_VALUE *)pibValue));
        }
    }
</#if>
}

MAC_WRP_STATUS MAC_WRP_SetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_WRP_PIB_VALUE *pibValue)
{
    if (handle != macWrpData.macWrpHandle)
    {
        // Handle error
        pibValue->length = 0;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pibValue->value, pibValue->length, "MAC_WRP_SetRequestSync: Attribute: %08X; Index: %u; Value: ", attribute, index);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
    else if (_macWrpIsAttributeInPLCRange(attribute))
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* Set to RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_SetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
<#elseif MAC_PLC_PRESENT == true>
    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
<#elseif MAC_RF_PRESENT == true>
    /* Check attribute ID range to redirect to Common or RF MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* Set to RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_SetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
</#if>
}

void MAC_WRP_ResetRequest(MAC_WRP_HANDLE handle, MAC_WRP_RESET_REQUEST_PARAMS *rstParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_RESET_CONFIRM_PARAMS resetConfirm;
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            resetConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.resetConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_ResetRequest: Set default PIB: %u", rstParams->setDefaultPib);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set control variable
    hyalData.waitingSecondResetConfirm = false;
</#if>
<#if MAC_PLC_PRESENT == true>
    // Reset PLC MAC
    MAC_PLC_ResetRequest((MAC_RESET_REQUEST_PARAMS *)rstParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Reset RF MAC
    MAC_RF_ResetRequest((MAC_RESET_REQUEST_PARAMS *)rstParams);
</#if>
    // Reset Common MAC if IB has to be reset
    if (rstParams->setDefaultPib)
    {
        MAC_COMMON_Reset();
    }
}

void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirm;
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            scanConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.scanConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_ScanRequest: Duration: %u", scanParams->scanDuration);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set control variable
    hyalData.waitingSecondScanConfirm = false;
</#if>
<#if MAC_PLC_PRESENT == true>
    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((MAC_SCAN_REQUEST_PARAMS *)scanParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Set RF MAC on Scan state
    MAC_RF_ScanRequest((MAC_SCAN_REQUEST_PARAMS *)scanParams);
</#if>
}

void MAC_WRP_StartRequest(MAC_WRP_HANDLE handle, MAC_WRP_START_REQUEST_PARAMS *startParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_START_CONFIRM_PARAMS startConfirm;
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            startConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.startConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_StartRequest: Pan ID: %u", startParams->panId);

<#if MAC_PLC_PRESENT == true && MAC_RF_PRESENT == true>
    // Set control variable
    hyalData.waitingSecondStartConfirm = false;
</#if>
<#if MAC_PLC_PRESENT == true>
    // Start Network on PLC MAC
    MAC_PLC_StartRequest((MAC_START_REQUEST_PARAMS *)startParams);
</#if>
<#if MAC_RF_PRESENT == true>
    // Start Network on PLC MAC
    MAC_RF_StartRequest((MAC_START_REQUEST_PARAMS *)startParams);
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

/*******************************************************************************
 End of File
*/
