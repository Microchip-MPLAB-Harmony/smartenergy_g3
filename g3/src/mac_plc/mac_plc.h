/*******************************************************************************
  G3 MAC PLC Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_plc.h

  Summary:
    G3 MAC PLC API Header File

  Description:
    This file contains definitions of the primitives and related types
    to be used by MAC Wrapper when accessing G3 MAC PLC layer.
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

#ifndef _MAC_PLC_H
#define _MAC_PLC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "../mac_common/mac_common.h"
#include "mac_plc_mib.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC PLC Handle

  Summary:
    Identifies an instance of MAC PLC module.

  Description:
    Handle is returned upon MAC PLC Open routine and is used for every other
    API function to identify module instance.

  Remarks:
    None.
*/
typedef uintptr_t MAC_PLC_HANDLE;

// *****************************************************************************
/* Invalid MAC PLC handle value to an instance

  Summary:
    Invalid handle value to a MAC PLC instance.

  Description:
    Defines the invalid handle value to a MAC PLC instance.

  Remarks:
    None.
*/
#define MAC_PLC_HANDLE_INVALID   ((MAC_PLC_HANDLE) (-1))

// *****************************************************************************
/* MAC PLC Bands definition

   Summary:
    Identifies the possible PLC band values.

   Description:
    This enumeration identifies the possible PLC band values.

   Remarks:
    None.
*/
typedef enum
{
    MAC_PLC_BAND_CENELEC_A = 0,
    MAC_PLC_BAND_CENELEC_B = 1,
    MAC_PLC_BAND_FCC = 2,
    MAC_PLC_BAND_ARIB = 3,
} MAC_PLC_BAND;

// *****************************************************************************
/* MAC PLC Callback Handlers Structure

   Summary:
    Set of Event Handler function pointers to receive events from MAC PLC.

   Description:
    Defines the set of callback functions that MAC PLC uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_COMMON_DataConfirm macPlcDataConfirm;
    MAC_COMMON_DataIndication macPlcDataIndication;
    MAC_COMMON_ResetConfirm macPlcResetConfirm;
    MAC_COMMON_BeaconNotifyIndication macPlcBeaconNotifyIndication;
    MAC_COMMON_ScanConfirm macPlcScanConfirm;
    MAC_COMMON_StartConfirm macPlcStartConfirm;
    MAC_COMMON_CommStatusIndication macPlcCommStatusIndication;
    MAC_COMMON_SnifferIndication macPlcMacSnifferIndication;
} MAC_PLC_HANDLERS;

// *****************************************************************************
/* MAC PLC Init Structure

   Summary:
    Initialization Data for MAC PLC to be provided on Initialize routine.

   Description:
    Defines the set of callback functions that MAC PLC uses to generate
    events to upper layer, a pointer to the MAC Tables structure
    and the PLC band to use.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_PLC_HANDLERS macPlcHandlers;
    /* Pointer to MAC Tables */
    MAC_PLC_TABLES *macPlcTables;
    /* PLC working band */
    MAC_PLC_BAND plcBand;
} MAC_PLC_INIT;

// *****************************************************************************
/* MAC PLC State Machine Definition

  Summary:
    Defines the states of the MAC PLC State Machine.

  Description:
    None.

  Remarks:
    None.
*/
typedef enum
{
    MAC_PLC_STATE_IDLE,
    MAC_PLC_STATE_TX,
    MAC_PLC_STATE_WAITING_TX_CFM,
    MAC_PLC_STATE_ERROR,
}MAC_PLC_STATE;

// *****************************************************************************
/* MAC PLC Data Structure

   Summary:
    Object used to keep any data required for an instance of the module.

   Description:
    Contains status of module state machine, runtime variables, pointer to
    Mac Tables and callback definitions.

   Remarks:
    None.
*/
typedef struct
{
    /* Flag to indicate this object is in use  */
    bool inUse;
    /* State of the MAC PLC module */
    MAC_PLC_STATE state;
    /* Callbacks */
    MAC_PLC_HANDLERS macPlcHandlers;
    /* Pointer to MAC Tables */
    MAC_PLC_TABLES *macPlcTables;
    /* PLC working band */
    MAC_PLC_BAND plcBand;
} MAC_PLC_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: MAC PLC Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ MAC_PLC_Initialize
    (
      const SYS_MODULE_INDEX index,
      const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the MAC PLC module.

  Description:
    This routine initializes the MAC PLC.
    Callback handlers for event notification are set in this function.
    A Pointer to MAC PLC Tables is also set here so MAC library can use them.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized (single instance allowed)

    init  - Pointer to the init data structure containing any data necessary to
            initialize the module.

  Returns:
    If successful, returns a valid module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // The following code snippet shows an example MAC PLC initialization.

    SYS_MODULE_OBJ sysObjMacPlc;

    const MAC_PLC_INIT macPlcInit = {
        .macPlcHandlers.macPlcDataConfirm = appDataConfirm,
        .macPlcHandlers.macPlcDataIndication = appDataIndication,
        .macPlcHandlers.macPlcResetConfirm = appResetConfirm,
        .macPlcHandlers.macPlcBeaconNotifyIndication = appBeaconIndication,
        .macPlcHandlers.macPlcScanConfirm = appScanConfirm,
        .macPlcHandlers.macPlcStartConfirm = NULL, // Start primitive not used
        .macPlcHandlers.macPlcCommStatusIndication = appCommStatus,
        .macPlcHandlers.macPlcMacSnifferIndication = NULL, // MAC Sniffer not used
        .macPlcTables = &tables, // Variable containing the MAC PLC Tables
        .plcBand = MAC_PLC_BAND_CENELEC_A,
    };

    sysObjMacPlc = MAC_PLC_Initialize(MAC_PLC_INDEX_0, &macPlcInit);
    if (sysObjMacPlc == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other MAC PLC routine is called.
*/
SYS_MODULE_OBJ MAC_PLC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    MAC_PLC_HANDLE MAC_PLC_Open
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Opens the specified MAC PLC instance and returns a handle to it.

  Description:
    This routine opens the specified MAC PLC instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the module.

  Precondition:
    MAC_PLC_Initialize routine must have been called before,
    and its returned Object used when calling this function.

  Parameters:
    object - Identifier for the object instance to be opened

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).
    If an error occurs, the return value is MAC_PLC_HANDLE_INVALID.

  Example:
    <code>
    MAC_PLC_HANDLE handle;
    SYS_MODULE_OBJ sysObjMacPlc;

    const MAC_PLC_INIT macPlcInit = {
        .macPlcHandlers.macPlcDataConfirm = appDataConfirm,
        .macPlcHandlers.macPlcDataIndication = appDataIndication,
        .macPlcHandlers.macPlcResetConfirm = appResetConfirm,
        .macPlcHandlers.macPlcBeaconNotifyIndication = appBeaconIndication,
        .macPlcHandlers.macPlcScanConfirm = appScanConfirm,
        .macPlcHandlers.macPlcStartConfirm = NULL, // Start primitive not used
        .macPlcHandlers.macPlcCommStatusIndication = appCommStatus,
        .macPlcHandlers.macPlcMacSnifferIndication = NULL, // MAC Sniffer not used
        .macPlcTables = &tables, // Variable containing the MAC PLC Tables
        .plcBand = MAC_PLC_BAND_CENELEC_A,
    };

    sysObjMacPlc = MAC_PLC_Initialize(MAC_PLC_INDEX_0, &macPlcInit);

    handle = MAC_PLC_Open(sysObjMacPlc);
    if (handle == MAC_PLC_HANDLE_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/
MAC_PLC_HANDLE MAC_PLC_Open(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void MAC_PLC_Tasks
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Maintains MAC PLC State Machine.

  Description:
    MAC PLC State Machine controls MAC layer duties, such as transmitting and
    receiving frames, managing PLC medium access or ensure link reliability.

  Precondition:
    MAC_PLC_Initialize routine must have been called before,
    and its returned Object used when calling this function.

  Parameters:
    object - Identifier for the object instance

  Returns:
    None.

  Example:
    <code>
    // ...
    SYS_MODULE_OBJ sysObjMacPlc;
    sysObjMacPlc = MAC_PLC_Initialize(MAC_PLC_INDEX_0, &macPlcInit);
    // ...

    while (true)
    {
        MAC_PLC_Tasks(sysObjMacPlc);
    
        // Do other tasks
    }
    </code>

  Remarks:
    None.
*/
void MAC_PLC_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void MAC_PLC_DataRequest
    (
      MAC_PLC_HANDLE handle,
      MAC_DATA_REQUEST_PARAMS *drParams
    )

  Summary:
    The MAC_PLC_DataRequest primitive requests the transfer of a PDU
    to another device or multiple devices.

  Description:
    DataRequest primitive is used to transfer data to other nodes in the G3
    Network. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC PLC Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac PLC instance

    drParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_DATA_REQUEST_PARAMS params = {
        .srcAddressMode = MAC_ADDRESS_MODE_SHORT,
        .destPanId = 0x1234,
        .destAddress = 0x0002,
        .msduLength = 20,
        .msdu = &txBuffer[0],
        .msduHandle = appHandle++,
        .txOptions = MAC_TX_OPTION_ACK,
        .securityLevel = MAC_SECURITY_LEVEL_ENC_MIC_32,
        .keyIndex = 0,
        .qualityOfService = MAC_QUALITY_OF_SERVICE_NORMAL_PRIORITY,
    };

    MAC_PLC_DataRequest(handle, &params);
    // Wait for Data Confirm
    </code>

  Remarks:
    None.
*/
void MAC_PLC_DataRequest(MAC_PLC_HANDLE handle, MAC_DATA_REQUEST_PARAMS *drParams);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_PLC_GetRequestSync
    (
      MAC_PLC_HANDLE handle,
      MAC_PLC_PIB_ATTRIBUTE attribute,
      uint16_t index,
      MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_PLC_GetRequestSync primitive gets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    GetRequestSync primitive is used to get the value of a PIB.
    Sync suffix indicates that result is provided upon function call return,
    in the pibValue parameter.

  Precondition:
    A valid MAC PLC Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac PLC instance

    attribute - Identifier of the Attribute to retrieve value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value will be returned

  Returns:
    Result of get operation as a MAC_STATUS code.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_STATUS status;
    MAC_PIB_VALUE value;
    status = MAC_PLC_GetRequestSync(handle, MAC_PIB_MAX_FRAME_RETRIES, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // Get value from 'value' parameter
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_PLC_GetRequestSync(MAC_PLC_HANDLE handle,
    MAC_PLC_PIB_ATTRIBUTE attribute, uint16_t index, MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_PLC_SetRequestSync
    (
      MAC_PLC_HANDLE handle,
      MAC_PLC_PIB_ATTRIBUTE attribute,
      uint16_t index,
      const MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_PLC_SetRequestSync primitive sets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    SetRequestSync primitive is used to set the value of a PIB.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    A valid MAC PLC Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac PLC instance

    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_STATUS code.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_STATUS status;
    const MAC_PIB_VALUE value = {
        .length = 1,
        .value = 6
    };

    status = MAC_PLC_SetRequestSync(handle, MAC_PIB_MAX_FRAME_RETRIES, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // PIB correctly set
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_PLC_SetRequestSync(MAC_PLC_HANDLE handle,
    MAC_PLC_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_PLC_ResetRequest
    (
      MAC_PLC_HANDLE handle,
      MAC_RESET_REQUEST_PARAMS *rstParams
    )

  Summary:
    The MAC_PLC_ResetRequest primitive resets the MAC PLC module.

  Description:
    Reset operation initializes MAC PLC State Machine and PIB to their
    default values. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC PLC Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac PLC instance

    rstParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_RESET_REQUEST_PARAMS params = {
        .setDefaultPib = true
    };

    MAC_PLC_ResetRequest(handle, &params);

    // Wait for Reset Confirm
    </code>

  Remarks:
    None.
*/
void MAC_PLC_ResetRequest(MAC_PLC_HANDLE handle, MAC_RESET_REQUEST_PARAMS *rstParams);

// *****************************************************************************
/* Function:
    MAC_PLC_ScanRequest
    (
      MAC_PLC_HANDLE handle,
      MAC_SCAN_REQUEST_PARAMS *scanParams
    )

  Summary:
    The MAC_PLC_ScanRequest primitive sets MAC layer in Network Scan mode.

  Description:
    Scan operation asks MAC layer to send a Beacon Request frame and wait
    for incoming Beacon frames.
    During the Scan period, Beacons received will be notified by means of
    MAC_COMMON_BeaconNotifyIndication callback.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC PLC Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac PLC instance

    scanParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_SCAN_REQUEST_PARAMS params = {
        .scanDuration = 15
    };

    MAC_PLC_ScanRequest(handle, &params);

    // Wait for Scan Confirm
    </code>

  Remarks:
    None.
*/
void MAC_PLC_ScanRequest(MAC_PLC_HANDLE handle, MAC_SCAN_REQUEST_PARAMS *scanParams);

// *****************************************************************************
/* Function:
    MAC_PLC_StartRequest
    (
      MAC_PLC_HANDLE handle,
      MAC_START_REQUEST_PARAMS *startParams
    )

  Summary:
    The MAC_PLC_StartRequest primitive starts a G3 Network and sets the device
    as the PAN Coordinator.

  Description:
    Start operation asks MAC layer to start a G3 Network, turning the device
    into the PAN Coordinator of such Network, and setting the PAN Identifier.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    startParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_PLC_HANDLE handle;
    handle = MAC_PLC_Open(sysObjMacPlc);
    // ...

    MAC_START_REQUEST_PARAMS params = {
        .panId = 0x1234
    };

    MAC_PLC_StartRequest(handle, &params);

    // Wait for Start Confirm
    </code>

  Remarks:
    None.
*/
void MAC_PLC_StartRequest(MAC_PLC_HANDLE handle, MAC_START_REQUEST_PARAMS *startParams);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_PLC_H

/*******************************************************************************
 End of File
*/
