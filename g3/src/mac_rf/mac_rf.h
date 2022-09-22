/*******************************************************************************
  G3 MAC RF Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_rf.h

  Summary:
    G3 MAC RF API Header File

  Description:
    This file contains definitions of the primitives and related types
    to be used by MAC Wrapper when accessing G3 MAC RF layer.
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

#ifndef _MAC_RF_H
#define _MAC_RF_H

#include "../mac_common/mac_common.h"
#include "mac_rf_mib.h"

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
/* MAC RF Handle

  Summary:
    Identifies an instance of MAC RF module.

  Description:
    Handle is returned upon MAC RF Open routine and is used for every other
    API function to identify module instance.

  Remarks:
    None.
*/
typedef uintptr_t MAC_RF_HANDLE;

// *****************************************************************************
/* Invalid MAC RF handle value to an instance

  Summary:
    Invalid handle value to a MAC RF instance.

  Description:
    Defines the invalid handle value to a MAC RF instance.

  Remarks:
    None.
*/
#define MAC_RF_HANDLE_INVALID   ((MAC_RF_HANDLE) (-1))

// *****************************************************************************
/* MAC RF Callback Handlers Structure

   Summary:
    Set of Event Handler function pointers to receive events from MAC RF.

   Description:
    Defines the set of callback functions that MAC RF uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_COMMON_DataConfirm macRfDataConfirm;
    MAC_COMMON_DataIndication macRfDataIndication;
    MAC_COMMON_ResetConfirm macRfResetConfirm;
    MAC_COMMON_BeaconNotifyIndication macRfBeaconNotifyIndication;
    MAC_COMMON_ScanConfirm macRfScanConfirm;
    MAC_COMMON_StartConfirm macRfStartConfirm;
    MAC_COMMON_CommStatusIndication macRfCommStatusIndication;
    MAC_COMMON_SnifferIndication macRfMacSnifferIndication;
} MAC_RF_HANDLERS;

// *****************************************************************************
/* MAC RF Init Structure

   Summary:
    Initialization Data for MAC RF to be provided on Initialize routine.

   Description:
    Defines the set of callback functions that MAC RF uses to generate
    events to upper layer and a pointer to the MAC Tables structure.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_RF_HANDLERS macRfHandlers;
    /* Pointer to MAC Tables */
    MAC_RF_TABLES *macRfTables;
} MAC_RF_INIT;

// *****************************************************************************
/* MAC RF State Machine Definition

  Summary:
    Defines the states of the MAC RF State Machine.

  Description:
    None.

  Remarks:
    None.
*/
typedef enum
{
    MAC_RF_STATE_IDLE,
    MAC_RF_STATE_TX,
    MAC_RF_STATE_WAITING_TX_CFM,
    MAC_RF_STATE_ERROR,
}MAC_RF_STATE;

// *****************************************************************************
/* MAC RF Data Structure

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
    /* State of the MAC RF module */
    MAC_RF_STATE state;
    /* Callbacks */
    MAC_RF_HANDLERS macRfHandlers;
    /* Pointer to MAC Tables */
    MAC_RF_TABLES *macRfTables;
} MAC_RF_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: MAC RF Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ MAC_RF_Initialize
    (
      const SYS_MODULE_INDEX index,
      const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the MAC RF module.

  Description:
    This routine initializes the MAC RF.
    Callback handlers for event notification are set in this function.
    A Pointer to MAC RF Tables is also set here so MAC library can use them.

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
    // The following code snippet shows an example MAC Wrapper initialization.

    SYS_MODULE_OBJ sysObjMacRf;

    const MAC_RF_INIT macRfInit = {
        .macRfHandlers.macRfDataConfirm = appDataConfirm,
        .macRfHandlers.macRfDataIndication = appDataIndication,
        .macRfHandlers.macRfResetConfirm = appResetConfirm,
        .macRfHandlers.macRfBeaconNotifyIndication = appBeaconIndication,
        .macRfHandlers.macRfScanConfirm = appScanConfirm,
        .macRfHandlers.macRfStartConfirm = NULL, // Start primitive not used
        .macRfHandlers.macRfCommStatusIndication = appCommStatus,
        .macRfHandlers.macRfMacSnifferIndication = NULL, // MAC Sniffer not used
        .macRfTables = &tables, // Variable containing the MAC RF Tables
    };

    sysObjMacRf = MAC_RF_Initialize(MAC_RF_INDEX_0, &macRfInit);
    if (sysObjMacRf == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other MAC RF routine is called.
*/
SYS_MODULE_OBJ MAC_RF_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    MAC_RF_HANDLE MAC_RF_Open
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Opens the specified MAC RF instance and returns a handle to it.

  Description:
    This routine opens the specified MAC RF instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the module.

  Precondition:
    MAC_RF_Initialize routine must have been called before,
    and its returned Object used when calling this function.

  Parameters:
    object - Identifier for the object instance to be opened

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).
    If an error occurs, the return value is MAC_RF_HANDLE_INVALID.

  Example:
    <code>
    MAC_RF_HANDLE handle;
    SYS_MODULE_OBJ sysObjMacRf;

    const MAC_RF_INIT macRfInit = {
        .macRfHandlers.macRfDataConfirm = appDataConfirm,
        .macRfHandlers.macRfDataIndication = appDataIndication,
        .macRfHandlers.macRfResetConfirm = appResetConfirm,
        .macRfHandlers.macRfBeaconNotifyIndication = appBeaconIndication,
        .macRfHandlers.macRfScanConfirm = appScanConfirm,
        .macRfHandlers.macRfStartConfirm = NULL, // Start primitive not used
        .macRfHandlers.macRfCommStatusIndication = appCommStatus,
        .macRfHandlers.macRfMacSnifferIndication = NULL, // MAC Sniffer not used
        .macRfTables = &tables, // Variable containing the MAC RF Tables
    };

    sysObjMacRf = MAC_RF_Initialize(MAC_RF_INDEX_0, &macRfInit);

    handle = MAC_RF_Open(sysObjMacRf);
    if (handle == MAC_RF_HANDLE_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/
MAC_RF_HANDLE MAC_RF_Open(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void MAC_RF_Tasks
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Maintains MAC RF State Machine.

  Description:
    MAC RF State Machine controls MAC layer duties, such as transmitting and
    receiving frames, managing RF medium access or ensure link reliability.

  Precondition:
    MAC_RF_Initialize routine must have been called before,
    and its returned Object used when calling this function.

  Parameters:
    object - Identifier for the object instance

  Returns:
    None.

  Example:
    <code>
    // ...
    SYS_MODULE_OBJ sysObjMacRf;
    sysObjMacRf = MAC_RF_Initialize(MAC_RF_INDEX_0, &macRfInit);
    // ...

    while (true)
    {
        MAC_RF_Tasks(sysObjMacRf);
    
        // Do other tasks
    }
    </code>

  Remarks:
    None.
*/
void MAC_RF_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void MAC_RF_DataRequest
    (
      MAC_RF_HANDLE handle,
      MAC_DATA_REQUEST_PARAMS *drParams
    )

  Summary:
    The MAC_RF_DataRequest primitive requests the transfer of a PDU
    to another device or multiple devices.

  Description:
    DataRequest primitive is used to transfer data to other nodes in the G3
    Network. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC RF Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac RF instance

    drParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
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

    MAC_RF_DataRequest(handle, &params);
    // Wait for Data Confirm
    </code>

  Remarks:
    None.
*/
void MAC_RF_DataRequest(MAC_RF_HANDLE handle, MAC_DATA_REQUEST_PARAMS *drParams);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_RF_GetRequestSync
    (
      MAC_RF_HANDLE handle,
      MAC_RF_PIB_ATTRIBUTE attribute,
      uint16_t index,
      MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_RF_GetRequestSync primitive gets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    GetRequestSync primitive is used to get the value of a PIB.
    Sync suffix indicates that result is provided upon function call return,
    in the pibValue parameter.

  Precondition:
    A valid MAC RF Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac RF instance

    attribute - Identifier of the Attribute to retrieve value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value will be returned

  Returns:
    Result of get operation as a MAC_STATUS code.

  Example:
    <code>
    // ...
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
    // ...

    MAC_STATUS status;
    MAC_PIB_VALUE value;
    status = MAC_RF_GetRequestSync(handle, MAC_PIB_MAX_FRAME_RETRIES_RF, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // Get value from 'value' parameter
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_RF_GetRequestSync(MAC_RF_HANDLE handle,
    MAC_RF_PIB_ATTRIBUTE attribute, uint16_t index, MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_RF_SetRequestSync
    (
      MAC_RF_HANDLE handle,
      MAC_RF_PIB_ATTRIBUTE attribute,
      uint16_t index,
      const MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_RF_SetRequestSync primitive sets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    SetRequestSync primitive is used to set the value of a PIB.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    A valid MAC RF Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac RF instance

    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_STATUS code.

  Example:
    <code>
    // ...
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
    // ...

    MAC_STATUS status;
    const MAC_PIB_VALUE value = {
        .length = 1,
        .value = 6
    };

    status = MAC_RF_SetRequestSync(handle, MAC_PIB_MAX_FRAME_RETRIES_RF, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // PIB correctly set
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_RF_SetRequestSync(MAC_RF_HANDLE handle,
    MAC_RF_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_RF_ResetRequest
    (
      MAC_RF_HANDLE handle,
      MAC_RESET_REQUEST_PARAMS *rstParams
    )

  Summary:
    The MAC_RF_ResetRequest primitive resets the MAC RF module.

  Description:
    Reset operation initializes MAC RF State Machine and PIB to their
    default values. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC RF Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac RF instance

    rstParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
    // ...

    MAC_RESET_REQUEST_PARAMS params = {
        .setDefaultPib = true
    };

    MAC_RF_ResetRequest(handle, &params);

    // Wait for Reset Confirm
    </code>

  Remarks:
    None.
*/
void MAC_RF_ResetRequest(MAC_RF_HANDLE handle, MAC_RESET_REQUEST_PARAMS *rstParams);

// *****************************************************************************
/* Function:
    MAC_RF_ScanRequest
    (
      MAC_RF_HANDLE handle,
      MAC_SCAN_REQUEST_PARAMS *scanParams
    )

  Summary:
    The MAC_RF_ScanRequest primitive sets MAC layer in Network Scan mode.

  Description:
    Scan operation asks MAC layer to send a Beacon Request frame and wait
    for incoming Beacon frames.
    During the Scan period, Beacons received will be notified by means of
    MAC_COMMON_BeaconNotifyIndication callback.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC RF Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac RF instance

    scanParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
    // ...

    MAC_SCAN_REQUEST_PARAMS params = {
        .scanDuration = 15
    };

    MAC_RF_ScanRequest(handle, &params);

    // Wait for Scan Confirm
    </code>

  Remarks:
    None.
*/
void MAC_RF_ScanRequest(MAC_RF_HANDLE handle, MAC_SCAN_REQUEST_PARAMS *scanParams);

// *****************************************************************************
/* Function:
    MAC_RF_StartRequest
    (
      MAC_RF_HANDLE handle,
      MAC_START_REQUEST_PARAMS *startParams
    )

  Summary:
    The MAC_RF_StartRequest primitive starts a G3 Network and sets the device
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
    MAC_RF_HANDLE handle;
    handle = MAC_RF_Open(sysObjMacRf);
    // ...

    MAC_START_REQUEST_PARAMS params = {
        .panId = 0x1234
    };

    MAC_RF_StartRequest(handle, &params);

    // Wait for Start Confirm
    </code>

  Remarks:
    None.
*/
void MAC_RF_StartRequest(MAC_RF_HANDLE handle, MAC_START_REQUEST_PARAMS *srParams);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_RF_H

/*******************************************************************************
 End of File
*/
