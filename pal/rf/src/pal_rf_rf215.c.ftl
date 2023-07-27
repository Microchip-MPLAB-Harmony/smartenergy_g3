/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.c

  Summary:
    RF Platform Abstraction Layer (PAL) Interface source file.

  Description:
    RF Platform Abstraction Layer (PAL) Interface source file. The RF PAL module
    provides a simple interface to manage the RF PHY layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
#include "driver/rf215/drv_rf215.h"
#include "driver/rf215/drv_rf215_definitions.h"
#include "pal_rf.h"
#include "pal_rf_local.h"
<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
#include "service/usi/srv_usi.h"
#include "service/rsniffer/srv_rsniffer.h"
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
static PAL_RF_DATA palRfData = {0};

// *****************************************************************************
// *****************************************************************************
// Section: Local Callbacks
// *****************************************************************************
// *****************************************************************************

static void lPAL_RF_RxIndCallback(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    uint8_t *pData;
    uint16_t len;
    PAL_RF_RX_PARAMETERS rxParameters;
<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
    uint8_t* pRfSnifferData;
    size_t rfSnifferDataSize;
    uint16_t rfPayloadSymbols = 0;
</#if>
    
    pData = indObj->psdu;
    len = indObj->psduLen;
    rxParameters.timeIniCount = indObj->timeIniCount;
    rxParameters.timeEndCount = indObj->timeIniCount + indObj->ppduDurationCount;
    rxParameters.rssi = indObj->rssiDBm;
    rxParameters.fcsOk = indObj->fcsOk;
        
    if (palRfData.rfPhyHandlers.palRfDataIndication != NULL)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(pData, len, &rxParameters);
    }

<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
    // Get payload symbols in the received message
    (void) DRV_RF215_GetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_RX_PAY_SYMBOLS,
            &rfPayloadSymbols);

    // Serialize received RF message
    pRfSnifferData = SRV_RSNIFFER_SerialRxMessage(indObj, &palRfData.rfPhyConfig, 
            rfPayloadSymbols, &rfSnifferDataSize);

    // Send through USI
    SRV_USI_Send_Message(palRfData.srvUsiHandler, SRV_USI_PROT_ID_SNIFF_G3,
            pRfSnifferData, rfSnifferDataSize);
</#if>
}

static void lPAL_RF_TxCfmCallback (DRV_RF215_TX_HANDLE txHandle, DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t ctxt)
{
    PAL_RF_PHY_STATUS status = PAL_RF_PHY_ERROR;
    uint64_t timeIniCount;
    uint64_t timeEndCount;
<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
    uint8_t* pRfSnifferData;
    size_t rfSnifferDataSize;
    uint16_t rfPayloadSymbols = 0;
</#if>
    
    /* Get Frame times */
    timeIniCount = cfmObj->timeIniCount;
    timeEndCount = timeIniCount + cfmObj->ppduDurationCount;
    
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
    
    if (palRfData.rfPhyHandlers.palRfTxConfirm != NULL)
    {
        palRfData.rfPhyHandlers.palRfTxConfirm(status, timeIniCount, timeEndCount);
    }

<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
    // Get payload symbols in the transmitted message
    (void) DRV_RF215_GetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_TX_PAY_SYMBOLS,
            &rfPayloadSymbols);

    // Serialize transmitted RF message
    pRfSnifferData = SRV_RSNIFFER_SerialCfmMessage(cfmObj, txHandle,
            &palRfData.rfPhyConfig, rfPayloadSymbols, &rfSnifferDataSize);

    if ((pRfSnifferData != NULL) && (rfSnifferDataSize != 0U))
    {
        // Send through USI
        SRV_USI_Send_Message(palRfData.srvUsiHandler, SRV_USI_PROT_ID_SNIFF_G3,
                pRfSnifferData, rfSnifferDataSize);
    }
</#if>
}

static void lPAL_RF_InitCallback(uintptr_t context, SYS_STATUS status)
{
    if (status == SYS_STATUS_ERROR)
    {
        palRfData.status = PAL_RF_STATUS_ERROR;
        return;
    }

    palRfData.drvRfPhyHandle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
    
    if (palRfData.drvRfPhyHandle == DRV_HANDLE_INVALID)
    {
        palRfData.status = PAL_RF_STATUS_ERROR;
        return;
    }

    /* Register RF PHY driver callbacks */
    DRV_RF215_RxIndCallbackRegister(palRfData.drvRfPhyHandle, lPAL_RF_RxIndCallback, 0);
    DRV_RF215_TxCfmCallbackRegister(palRfData.drvRfPhyHandle, lPAL_RF_TxCfmCallback, 0);

<#if G3_PAL_RF_PHY_SNIFFER_EN == true>
    /* Get USI handler for RF PHY SNIFFER protocol */
    palRfData.srvUsiHandler = SRV_USI_Open(PAL_RF_PHY_SNIFFER_USI_INSTANCE);

</#if>
<#if G3_PAL_RF_PHY_SNIFFER_EN == true || drvRf215.DRV_RF215_OFDM_EN == true>
    /* Get RF PHY configuration */
    (void) DRV_RF215_GetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_CONFIG, &palRfData.rfPhyConfig);

</#if>
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
    palRfData.rfPhyModSchemeFsk = FSK_FEC_OFF;
    if (palRfData.rfPhyConfig.phyType == PHY_TYPE_FSK)
    {
        palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeFsk;
        palRfData.rfPhyModSchemeOfdm = OFDM_MCS_0;
    }
    else /* PHY_TYPE_OFDM */
    {
        switch (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt)
        {
            case OFDM_BW_OPT_4:
                palRfData.rfPhyModSchemeOfdm = OFDM_MCS_2;
                break;
            
            case OFDM_BW_OPT_3:
                palRfData.rfPhyModSchemeOfdm = OFDM_MCS_1;
                break;
            
            case OFDM_BW_OPT_2:
            case OFDM_BW_OPT_1:
            default:
                palRfData.rfPhyModSchemeOfdm = OFDM_MCS_0;
                break;
        }

        palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeOfdm;
    }
<#elseif drvRf215.DRV_RF215_FSK_EN == true>
    palRfData.rfPhyModScheme = FSK_FEC_OFF;
<#else>
    switch (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt)
    {
        case OFDM_BW_OPT_4:
            palRfData.rfPhyModScheme = OFDM_MCS_2;
            break;
        
        case OFDM_BW_OPT_3:
            palRfData.rfPhyModScheme = OFDM_MCS_1;
            break;
        
        case OFDM_BW_OPT_2:
        case OFDM_BW_OPT_1:
        default:
            palRfData.rfPhyModScheme = OFDM_MCS_0;
            break;
    }
</#if>

    palRfData.status = PAL_RF_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index, 
        const SYS_MODULE_INIT * const init)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
<#if core.COVERITY_SUPPRESS_DEVIATION?? && core.COVERITY_SUPPRESS_DEVIATION>
    <#if core.COMPILER_CHOICE == "XC32">
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunknown-pragmas"
    </#if>
    #pragma coverity compliance block deviate "MISRA C-2012 Rule 11.3" "H3_MISRAC_2012_R_11_3_DR_1"
</#if>
    const PAL_RF_INIT * const palInit = (const PAL_RF_INIT * const)init;
<#if core.COVERITY_SUPPRESS_DEVIATION?? && core.COVERITY_SUPPRESS_DEVIATION>
    #pragma coverity compliance end_block "MISRA C-2012 Rule 11.3"
    <#if core.COMPILER_CHOICE == "XC32">
    #pragma GCC diagnostic pop
    </#if>
</#if>
    /* MISRA C-2012 deviation block end */

    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Check previously initialized */
    if (palRfData.status != PAL_RF_STATUS_UNINITIALIZED)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    palRfData.rfPhyHandlers.palRfDataIndication = palInit->rfPhyHandlers.palRfDataIndication;
    palRfData.rfPhyHandlers.palRfTxConfirm = palInit->rfPhyHandlers.palRfTxConfirm;

    palRfData.status = PAL_RF_STATUS_BUSY;
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;

    DRV_RF215_ReadyStatusCallbackRegister(DRV_RF215_INDEX_0, lPAL_RF_InitCallback, 0);

    return (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX;
}

PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)
{    
    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
    {
        return PAL_RF_HANDLE_INVALID;
    }
    
    /* Check previously initialized */
    if (palRfData.status == PAL_RF_STATUS_UNINITIALIZED)
    {
        return PAL_RF_HANDLE_INVALID;
    }

    return (PAL_RF_HANDLE)&palRfData;
}

PAL_RF_STATUS PAL_RF_Status(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return PAL_RF_STATUS_INVALID_OBJECT;
    }
    
    return palRfData.status;
}
 
void PAL_RF_Deinitialize(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return;
    }
    
    /* Check status */
    if (palRfData.status == PAL_RF_STATUS_UNINITIALIZED)
    {
        return;
    }
    
    palRfData.status = PAL_RF_STATUS_UNINITIALIZED;
    
    DRV_RF215_Close((DRV_HANDLE)palRfData.drvRfPhyHandle);
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;    
}
 
PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
    DRV_RF215_TX_REQUEST_OBJ txReqObj;
    DRV_RF215_TX_RESULT txResult;
    DRV_RF215_TX_HANDLE rfPhyTxReqHandle;
            
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        if (palRfData.rfPhyHandlers.palRfTxConfirm != NULL)
        {
            palRfData.rfPhyHandlers.palRfTxConfirm(PAL_RF_PHY_TRX_OFF, txParameters->timeCount, 
                    txParameters->timeCount);
        }
        return PAL_RF_TX_HANDLE_INVALID;
    }
    
    txReqObj.psdu = pData;
    txReqObj.psduLen = length;
    txReqObj.timeMode = TX_TIME_ABSOLUTE;
    txReqObj.timeCount = txParameters->timeCount;
    txReqObj.txPwrAtt = txParameters->txPowerAttenuation;
    txReqObj.modScheme = palRfData.rfPhyModScheme;
    
    if (txParameters->csmaEnable)
    {
        /* CSMA used: Energy above threshold and carrier sense CCA Mode */
        txReqObj.ccaMode = PHY_CCA_MODE_3;
        /* Programmed TX canceled once RX frame detected */
        txReqObj.cancelByRx = true;
    }
    else
    {
        /* CSMA not used */
        txReqObj.ccaMode = PHY_CCA_OFF;
        txReqObj.cancelByRx = false;
    }
    
    rfPhyTxReqHandle = DRV_RF215_TxRequest(palRfData.drvRfPhyHandle, &txReqObj, &txResult);

<#if G3_PAL_RF_PHY_SNIFFER_EN == true>  
    // Prepare transmission request in sniffer service
    SRV_RSNIFFER_SetTxMessage(&txReqObj, rfPhyTxReqHandle);

</#if>
    if (rfPhyTxReqHandle == DRV_RF215_TX_HANDLE_INVALID)
    {
        DRV_RF215_TX_CONFIRM_OBJ cfmObj;
        
        cfmObj.txResult = txResult;
        cfmObj.timeIniCount = SYS_TIME_Counter64Get();
        cfmObj.ppduDurationCount = 0;
        lPAL_RF_TxCfmCallback(DRV_RF215_TX_HANDLE_INVALID, &cfmObj, 0);
    }
    
    return (PAL_RF_TX_HANDLE)rfPhyTxReqHandle;
}

void PAL_RF_TxCancel(PAL_RF_HANDLE handle, PAL_RF_TX_HANDLE txHandle)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return;
    }
    
    DRV_RF215_TxCancel(palRfData.drvRfPhyHandle, (DRV_RF215_TX_HANDLE)txHandle);
}
 
void PAL_RF_Reset(PAL_RF_HANDLE handle)
{
    uint8_t resetValue = 1;
    
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return;
    }
    
    (void) DRV_RF215_SetPib(palRfData.drvRfPhyHandle, RF215_PIB_DEVICE_RESET, &resetValue);
    (void) DRV_RF215_SetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_STATS_RESET, &resetValue);
}
 
PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT pibResult = PAL_RF_PIB_SUCCESS;

    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }

    switch (pibObj->pib)
    {
        case PAL_RF_PIB_TX_FSK_FEC:
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
            pibObj->pData[0] = (uint8_t) palRfData.rfPhyModSchemeFsk;
<#elseif drvRf215.DRV_RF215_FSK_EN == true>
            pibObj->pData[0] = (uint8_t) palRfData.rfPhyModScheme;
<#else>
            /* FSK disabled in MCC */
            pibResult = PAL_RF_PIB_INVALID_ATTR;
</#if>
            break;

        case PAL_RF_PIB_TX_OFDM_MCS:
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
            pibObj->pData[0] = (uint8_t) palRfData.rfPhyModSchemeOfdm;
<#elseif drvRf215.DRV_RF215_FSK_EN == true>
            /* OFDM disabled in MCC */
            pibResult = PAL_RF_PIB_INVALID_ATTR;
<#else>
            pibObj->pData[0] = (uint8_t) palRfData.rfPhyModScheme;
</#if>
            break;

        default:
            pibResult = (PAL_RF_PIB_RESULT)DRV_RF215_GetPib(palRfData.drvRfPhyHandle,
                    (DRV_RF215_PIB_ATTRIBUTE)pibObj->pib, pibObj->pData);
            break;
    }

    return pibResult;
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT pibResult = PAL_RF_PIB_SUCCESS;

    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }

    switch (pibObj->pib)
    {
        case PAL_RF_PIB_TX_FSK_FEC:
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
            if (pibObj->pData[0] > (uint8_t)PAL_RF_FSK_FEC_ON)
            {
                return PAL_RF_PIB_INVALID_PARAM;
            }

            palRfData.rfPhyModSchemeFsk = (DRV_RF215_PHY_MOD_SCHEME) pibObj->pData[0];

            if (palRfData.rfPhyConfig.phyType == PHY_TYPE_FSK)
            {
                palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeFsk;
            }

<#elseif drvRf215.DRV_RF215_FSK_EN == true>
            if (pibObj->pData[0] > (uint8_t)PAL_RF_FSK_FEC_ON)
            {
                return PAL_RF_PIB_INVALID_PARAM;
            }

            palRfData.rfPhyModScheme = (DRV_RF215_PHY_MOD_SCHEME) pibObj->pData[0];
<#else>
            /* FSK disabled in MCC */
            pibResult = PAL_RF_PIB_INVALID_ATTR;
</#if>
            break;

        case PAL_RF_PIB_TX_OFDM_MCS:
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
            switch ((PAL_RF_OFDM_MCS) pibObj->pData[0])
            {
                case PAL_RF_OFDM_MCS_0:
                    if ((palRfData.rfPhyConfig.phyType == PHY_TYPE_OFDM) &&
                        ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4) ||
                        (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_3)))
                    {
                        return PAL_RF_PIB_INVALID_PARAM;
                    }

                    palRfData.rfPhyModSchemeOfdm = OFDM_MCS_0;
                    break;

                case PAL_RF_OFDM_MCS_1:
                    if ((palRfData.rfPhyConfig.phyType == PHY_TYPE_OFDM) &&
                        (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4))
                    {
                        return PAL_RF_PIB_INVALID_PARAM;
                    }

                    palRfData.rfPhyModSchemeOfdm = OFDM_MCS_1;
                    break;

                case PAL_RF_OFDM_MCS_2:
                case PAL_RF_OFDM_MCS_3:
                case PAL_RF_OFDM_MCS_4:
                case PAL_RF_OFDM_MCS_5:
                case PAL_RF_OFDM_MCS_6:
                    palRfData.rfPhyModSchemeOfdm = (DRV_RF215_PHY_MOD_SCHEME) pibObj->pData[0];
                    break;

                default:
                    pibResult = PAL_RF_PIB_INVALID_PARAM;
                    break;
            }

            if (palRfData.rfPhyConfig.phyType == PHY_TYPE_OFDM)
            {
                palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeOfdm;
            }

<#elseif drvRf215.DRV_RF215_FSK_EN == true>
            /* OFDM disabled in MCC */
            pibResult = PAL_RF_PIB_INVALID_ATTR;
<#else>
            switch ((PAL_RF_OFDM_MCS) pibObj->pData[0])
            {
                case PAL_RF_OFDM_MCS_0:
                    if ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4) ||
                        (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_3))
                    {
                        return PAL_RF_PIB_INVALID_PARAM;
                    }

                    palRfData.rfPhyModScheme = OFDM_MCS_0;
                    break;

                case PAL_RF_OFDM_MCS_1:
                    if (palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4)
                    {
                        return PAL_RF_PIB_INVALID_PARAM;
                    }

                    palRfData.rfPhyModScheme = OFDM_MCS_1;
                    break;

                case PAL_RF_OFDM_MCS_2:
                case PAL_RF_OFDM_MCS_3:
                case PAL_RF_OFDM_MCS_4:
                case PAL_RF_OFDM_MCS_5:
                case PAL_RF_OFDM_MCS_6:
                    palRfData.rfPhyModScheme = (DRV_RF215_PHY_MOD_SCHEME) pibObj->pData[0];
                    break;

                default:
                    pibResult = PAL_RF_PIB_INVALID_PARAM;
                    break;
            }

</#if>
            break;

        default:
            pibResult = (PAL_RF_PIB_RESULT)DRV_RF215_SetPib(palRfData.drvRfPhyHandle,
                    (DRV_RF215_PIB_ATTRIBUTE)pibObj->pib, pibObj->pData);

<#if G3_PAL_RF_PHY_SNIFFER_EN == true || drvRf215.DRV_RF215_OFDM_EN == true>
            if ((pibResult == PAL_RF_PIB_SUCCESS) &&
                ((pibObj->pib == PAL_RF_PIB_PHY_CONFIG) ||
                (pibObj->pib == PAL_RF_PIB_PHY_BAND_OPERATING_MODE)))
            {
                /* Update RF PHY configuration */
                (void) DRV_RF215_GetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_CONFIG, &palRfData.rfPhyConfig);
<#if drvRf215.DRV_RF215_FSK_EN == true && drvRf215.DRV_RF215_OFDM_EN == true>
                if (palRfData.rfPhyConfig.phyType == PHY_TYPE_FSK)
                {
                    palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeFsk;
                }
                else /* PHY_TYPE_OFDM */
                {
                    if ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4) &&
                        (palRfData.rfPhyModSchemeOfdm < OFDM_MCS_2))
                    {
                        palRfData.rfPhyModSchemeOfdm = OFDM_MCS_2;
                    }
                    else
                    {
                        if ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_3) &&
                            (palRfData.rfPhyModSchemeOfdm < OFDM_MCS_1))
                        {
                            palRfData.rfPhyModSchemeOfdm = OFDM_MCS_1;
                        }
                    }

                    palRfData.rfPhyModScheme = palRfData.rfPhyModSchemeOfdm;
                }
<#elseif drvRf215.DRV_RF215_OFDM_EN == true>
                if ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_4) &&
                    (palRfData.rfPhyModScheme < OFDM_MCS_2))
                {
                    palRfData.rfPhyModScheme = OFDM_MCS_2;
                }
                else
                {
                    if ((palRfData.rfPhyConfig.phyTypeCfg.ofdm.opt == OFDM_BW_OPT_3) &&
                        (palRfData.rfPhyModScheme < OFDM_MCS_1))
                    {
                        palRfData.rfPhyModScheme = OFDM_MCS_1;
                    }
                }
</#if>
            }

</#if>
            break;
    }

    return pibResult;
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, PAL_RF_PIB_ATTRIBUTE attribute)
{
    uint8_t pibLen = 0;

    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return 0;
    }

    switch (attribute)
    {
        case PAL_RF_PIB_TX_FSK_FEC:
        case PAL_RF_PIB_TX_OFDM_MCS:
            pibLen = 1;
            break;

        default:
            pibLen = DRV_RF215_GetPibSize((DRV_RF215_PIB_ATTRIBUTE)attribute);
            break;
    }

    return pibLen;
}
