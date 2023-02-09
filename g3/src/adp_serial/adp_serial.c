/*******************************************************************************
  G3 ADP Serialization Source File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_serial.c

  Summary:
    G3 ADP Serialization Source File.

  Description:
    The G3 ADP Serialization allows to serialize the ADP and LBP API through
    USI interface in order to run the application on an external device. This
    file contains the implementation of the G3 ADP Serialization.
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
#include "adp_serial.h"
#include "configuration.h"
#include "adp.h"
#include "lbp_dev.h"
#include "lbp_coord.h"
#include "service/usi/srv_usi.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* USI handle for ADP Serialization */
static SRV_USI_HANDLE adpSerialUsiHandle;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void _Callback_UsiAdpProtocol(uint8_t* pData, size_t length)
{
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ ADP_SERIAL_Initialize (
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
)
{
    /* Validate the request */
    if (index >= G3_ADP_SERIAL_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Initialize variables */
    adpSerialUsiHandle = SRV_USI_HANDLE_INVALID;

    return (SYS_MODULE_OBJ) G3_ADP_SERIAL_INDEX_0; 
}

void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ) G3_ADP_SERIAL_INDEX_0)
    {
        /* Invalid object */
        return;
    }

    if (adpSerialUsiHandle == SRV_USI_HANDLE_INVALID)
    {
        /* Open USI instance for MAC serialization and register callback */
        adpSerialUsiHandle = SRV_USI_Open(G3_ADP_SERIAL_USI_INDEX);
        SRV_USI_CallbackRegister(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, _Callback_UsiAdpProtocol);
    }
}
