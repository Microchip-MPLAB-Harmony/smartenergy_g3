/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.c

  Summary:
    Platform Abstraction Layer RF (PAL RF) Interface source file.

  Description:
    Platform Abstraction Layer RF (PAL RF) Interface header.
    The PAL RF module provides a simple interface to manage the RF PHY driver.
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
#include <stdlib.h>
#include <string.h>
#include "configuration.h"
#include "system/time/sys_time.h"
#include "driver/driver.h"
// TBD !!!!!!!!!!!!!!!  #include "driver/rf215/drv_rf215.h"
// TBD !!!!!!!!!!!!!!!  #include "driver/rf215/drv_rf215_definitions.h"
#include "pal_rf.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
static PAL_RF_DATA palRfData;

const PAL_RF_OBJECT_BASE PAL_RF_OBJECT_BASE_Default =
{
    .PAL_RF_Initialize = PAL_RF_Initialize,
    .PAL_RF_Status = PAL_RF_Status,
    .PAL_RF_Open = PAL_RF_Open,
    .PAL_RF_Close = PAL_RF_Close,
    .PAL_RF_TxRequest = PAL_RF_TxRequest,
    .PAL_RF_Reset = PAL_RF_Reset,
    .PAL_RF_GetPhyTime = PAL_RF_GetPhyTime,
    .PAL_RF_GetRfPhyPib = PAL_RF_GetRfPhyPib,
    .PAL_RF_SetRfPhyPib = PAL_RF_SetRfPhyPib,
    .PAL_RF_GetRfPhyPibLength = PAL_RF_GetRfPhyPibLength
};

// *****************************************************************************
// *****************************************************************************
// Section: local functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: local callbacks
// *****************************************************************************
// *****************************************************************************
static void _palRfInitCallback(uintptr_t context)
{
    palRfData.status = SYS_STATUS_READY;
}

static void _palRfRxIndCallback(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    uint8_t *pData;
    uint16_t len;
    PAL_RF_RX_PARAMETERS rxParameters;
    
    pData = indObj->psdu;
    len = indObj->psduLen;
    rxParameters.timeIni = indObj->timeIni;
    rxParameters.timeEnd = indObj->timeIni + indObj->ppduDurationUS;
    rxParameters.rssi = indObj->rssiDBm;
    rxParameters.fcsOk = indObj->fcsOk;
        
    if (palRfData.rfPhyHandlers.palRfDataIndication)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(pData, len, &rxParameters);
    }
}

static void _palRfTxCfmCallback (DRV_RF215_TX_HANDLE txHandle, DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t ctxt)
{
    PAL_RF_PHY_STATUS status = PAL_RF_PHY_ERROR;
    uint32_t timeIni;
    uint32_t timeEnd;
    
    /* Get Frame times */
    timeIni = cfmObj->timeIni;
    timeEnd = timeIni + cfmObj->ppduDurationUS;
    
    switch(cfmObj->txResult)
    {
        case RF215_TX_SUCCESS:
            status = PAL_RF_PHY_SUCCESS;
            break;

        case RF215_TX_BUSY_RX:
        case RF215_TX_BUSY_CHN:
        case RF215_TX_CANCEL_BY_RX:
            status = PAL_RF_PHY_CHANNEL_ACCESS_FAILURE;
            break;

        case RF215_TX_BUSY_TX:
        case RF215_TX_FULL_BUFFERS:
        case RF215_TX_TRX_SLEPT:
            status = PAL_RF_PHY_BUSY_TX;
            break;

        case RF215_TX_INVALID_LEN:
        case RF215_TX_INVALID_DRV_HANDLE:
        case RF215_TX_INVALID_PARAM:
            status = PAL_RF_PHY_INVALID_PARAM;
            break;

        case RF215_TX_ERROR_UNDERRUN:
        case RF215_TX_TIMEOUT:
            status = PAL_RF_PHY_TIMEOUT;
            break;

        case RF215_TX_CANCELLED:
            status = PAL_RF_PHY_TX_CANCELLED;
            break;

        case RF215_TX_ABORTED:
        default:
            status = PAL_RF_PHY_ERROR;
            break;      
    }
    
    if (palRfData.rfPhyHandlers.palRfTxConfirm)
    {
        palRfData.rfPhyHandlers.palRfTxConfirm(status, timeIni, timeEnd);
    }
    
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index, 
        const SYS_MODULE_INIT * const init)
{
    PAL_RF_INIT *palInit = (PAL_RF_INIT *)init;
    
    palRfData.status = SYS_STATUS_BUSY;
    
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;
    
    // 802.15.4 - NOT SUPPORTED
    // DRV_RF215_ReadyStatusCallbackRegister(DRV_RF215_INDEX_0, _palRfInitCallback, 0);
        
    palRfData.rfPhyHandlers.palRfDataIndication = palInit->rfPhyHandlers.palRfDataIndication;
    palRfData.rfPhyHandlers.palRfTxConfirm = palInit->rfPhyHandlers.palRfTxConfirm;
    
    palRfData.rfPhyModScheme = FSK_FEC_OFF;
    
    return (SYS_MODULE_OBJ)0;    
}

SYS_STATUS PAL_RF_Status(SYS_MODULE_OBJ object)
{
    if (object != 0)
    {
        return SYS_STATUS_ERROR;
    }
    
    return palRfData.status;
}
 
PAL_RF_HANDLE PAL_RF_Open(const SYS_MODULE_INDEX drvIndex, 
        const DRV_IO_INTENT intent)
{
    if (palRfData.status == SYS_STATUS_BUSY)
    {
        /* Not initialized yet */
        return PAL_RF_HANDLE_INVALID;
    }
    
    if (palRfData.drvRfPhyHandle != DRV_HANDLE_INVALID)
    {
        /* Already open */
        return (PAL_RF_HANDLE)palRfData.drvRfPhyHandle; 
    }
    
    //palRfData.drvRfPhyHandle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
    // 802.15.4 - NOT SUPPORTED
    
    if (palRfData.drvRfPhyHandle == DRV_HANDLE_INVALID)
    {
        palRfData.status = SYS_STATUS_ERROR;
        return PAL_RF_HANDLE_INVALID;
    }
    
    /* Register RF PHY driver callbacks */   // 802.15.4 - NOT SUPPORTED - How to set Callbacks??
    DRV_RF215_RxIndCallbackRegister(palRfData.drvRfPhyHandle, _palRfRxIndCallback, 0);
    DRV_RF215_TxCfmCallbackRegister(palRfData.drvRfPhyHandle, _palRfTxCfmCallback, 0);
    
    return (PAL_RF_HANDLE)palRfData.drvRfPhyHandle;
}
 
void PAL_RF_Close(PAL_RF_HANDLE handle)
{
    if (handle != (PAL_RF_HANDLE)palRfData.drvRfPhyHandle)
    {
        return;
    }
    
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;
    // 802.15.4 - NOT SUPPORTED
}
 
void PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
    Frame_info_t txFrameInfo;
    Csma_mode_t  txCsmaMode;
    Bool frameRetry = false;
    Retval_t txResult;
            
    if (handle != (PAL_RF_HANDLE)palRfData.drvRfPhyHandle)
    {
        if (palRfData.rfPhyHandlers.palRfTxConfirm)
        {
            palRfData.rfPhyHandlers.palRfTxConfirm(PAL_RF_PHY_TRX_OFF, txParameters->time, 
                    txParameters->time);
        }
        return;
    }
    
    txFrameInfo ?? // 802.15.4 TBD
    // txReqObj.psdu = pData;
    // txReqObj.psduLen = length;
    // txReqObj.timeMode = TX_TIME_ABSOLUTE;
    // txReqObj.time = txParameters->time;
    // txReqObj.txPwrAtt = txParameters->txPowerAttenuation;
    // txReqObj.modScheme = palRfData.rfPhyModScheme;
    
    if (txParameters->csmaEnable)
    {
        /* CSMA used: 802.15.4  TBD */
        txCsmaMode = ??;
    }
    else
    {
        /* CSMA not used: 802.15.4  TBD */
        txCsmaMode = 0;
    }
    
    txResult = PHY_TxFrame(&txFrame, txCsmaMode, frameRetry);
    
    if (txResult != PHY_BUSY)
    {
        DRV_RF215_TX_CONFIRM_OBJ cfmObj;
        
        cfmObj.txResult = txResult;
        cfmObj.timeIni = SYS_TIME_Counter64Get();
        cfmObj.ppduDurationUS = 0;
        _palRfTxCfmCallback(DRV_RF215_TX_HANDLE_INVALID, &cfmObj, 0);
    }
}
 
void PAL_RF_Reset(PAL_RF_HANDLE handle)
{
    if (handle != palRfData.drvRfPhyHandle)
    {
        return;
    }
    
    PHY_reset();  // 802.15.4 API
}
 
uint64_t PAL_RF_GetPhyTime(PAL_RF_HANDLE handle)
{
    if (handle != palRfData.drvRfPhyHandle)
    {
        return 0;
    }
    
    return SYS_TIME_Counter64Get();
}

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != palRfData.drvRfPhyHandle)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != SYS_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
    return (PAL_RF_PIB_RESULT)PHY_PibGet(pibObj->pib, pibObj->pData);    // 802.15.4 API
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != palRfData.drvRfPhyHandle)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != SYS_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
    return (PAL_RF_PIB_RESULT)PHY_PibSet(pibObj->pib, pibObj->pData);    // 802.15.4 API
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute)
{
    if (handle != palRfData.drvRfPhyHandle)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    return 0;      // 802.15.4 API . NOT USED
}
