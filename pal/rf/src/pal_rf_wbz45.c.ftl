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
#include "pal_rf.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
static PAL_RF_DATA palRfData = {0};

const PAL_RF_OBJECT_BASE PAL_RF_OBJECT_BASE_Default =
{
    .PAL_RF_Initialize = PAL_RF_Initialize,
    .PAL_RF_HandleGet = PAL_RF_HandleGet,
    .PAL_RF_Status = PAL_RF_Status,
    .PAL_RF_Deinitialize = PAL_RF_Deinitialize,
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

static void _palRfRxIndCallback(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    if (palRfData.rfPhyHandlers.palRfDataIndication)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(pData, len, &rxParameters);
    }
}

static void _palRfTxCfmCallback (DRV_RF215_TX_HANDLE txHandle, DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t ctxt)
{
    
    if (palRfData.rfPhyHandlers.palRfTxConfirm)
    {
        palRfData.rfPhyHandlers.palRfTxConfirm(status, timeIniCount, timeEndCount);
    }
    
}

static void _palRfInitCallback(uintptr_t context, SYS_STATUS status)
{
    if (status == SYS_STATUS_ERROR)
    {
        palRfData.status = SYS_STATUS_ERROR;
        return;
    }

    palRfData.status = SYS_STATUS_READY;
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


    return (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX;
}

PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)
{    
    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
    {
        return PAL_RF_HANDLE_INVALID;
    }

    return (PAL_RF_HANDLE)&palRfData;
}

PAL_RF_STATUS PAL_RF_Status(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return SYS_STATUS_ERROR;
    }
    
    return palRfData.status;
}
 
void PAL_RF_Deinitialize(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return;
    }
    
    palRfData.status = PAL_RF_STATUS_DEINITIALIZED;
    
    
}
 
void PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
            
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        if (palRfData.rfPhyHandlers.palRfTxConfirm)
        {
            palRfData.rfPhyHandlers.palRfTxConfirm(PAL_RF_PHY_TRX_OFF, txParameters->timeCount, 
                    txParameters->timeCount);
        }
        return;
    }
    
}
 
void PAL_RF_Reset(PAL_RF_HANDLE handle)
{
    uint8_t resetValue = 1;
    
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return;
    }
    
}
 
uint64_t PAL_RF_GetPhyTime(PAL_RF_HANDLE handle)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return 0;
    }
    
    return SYS_TIME_Counter64Get();
}

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
}
