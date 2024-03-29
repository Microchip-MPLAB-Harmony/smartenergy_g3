/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.h

  Summary:
    PLC Platform Abstraction Layer (PAL) Interface Local header file.

  Description:
    PLC Platform Abstraction Layer (PAL) Interface Local header file.
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

#ifndef PAL_PLC_LOCAL_H
#define PAL_PLC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "stack/g3/pal/plc/pal_plc.h"
#include "service/pcoup/srv_pcoup.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

<#if G3_PAL_PLC_PHY_SNIFFER_EN == true>
// *****************************************************************************
/* PAL PLC Phy Sniffer Data

  Summary:
    Defines the data includes in a message received via PLC PHY sniffer.

  Description:
    This data type defines the data included in a PLC PHY sniffer.

  Remarks:
    None.
*/
typedef struct
{
    // Header data
    MAC_RT_PHY_SNIFFER_HEADER header;

    // Payload data
    uint8_t data[MAC_RT_PHY_DATA_MAX_SIZE];

} PAL_PLC_PHY_SNIFFER;

</#if>
// *****************************************************************************
/* PAL PLC Data

  Summary:
    Holds PAL PLC internal data.

  Description:
    This data type defines the all data required to handle the PAL PLC module.

  Remarks:
    None.
*/
typedef struct
{
    DRV_HANDLE drvG3MacRtHandle;

    PAL_PLC_HANDLERS initHandlers;

    PAL_PLC_STATUS status;

    MAC_RT_BAND plcBand;

    SRV_PLC_PCOUP_BRANCH plcBranch;

    MAC_RT_PIB_OBJ plcPIB;

    MAC_RT_MIB_INIT_OBJ mibInitData;

    uint8_t statsErrorUnexpectedKey;

    uint8_t statsErrorReset;

    bool waitingTxCfm;

    bool restartMib;

    bool coordinator;

    bool pvddMonTxEnable;

} PAL_PLC_DATA;

#pragma pack(pop)

#endif // #ifndef PAL_PLC_LOCAL_H
/*******************************************************************************
 End of File
*/