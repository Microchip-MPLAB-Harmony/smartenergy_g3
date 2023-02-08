/*******************************************************************************
  LBP Device Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_dev.h

  Summary:
    LBP for Device Header File.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage LBP process for device.
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

#ifndef _LBP_DEV_H
#define _LBP_DEV_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/time/sys_time.h"
#include "eap_psk.h"
#include "adp.h"
#include "adp_shared_types.h"
#include "mac_wrapper_defs.h"

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
/* LBP Join Confirm Parameters

   Summary:
    Defines the parameters for the LBP Join Confirm event handler
    function.

   Description:
    The structure contains the fields reported by the LBP Join Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The 16-bit network address that was allocated to the device */
    uint16_t networkAddress;

    /* The 16-bit address of the PAN of which the device is now a member */
    uint16_t panId;

    /* The status of the request */
    uint8_t status;

} LBP_JOIN_CFM_PARAMS;

// *****************************************************************************
/* LBP Join Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Network Join Confirm Event handler function.

  Description:
    This data type defines the required function signature for the LBP Join
    Confirm event handling callback function. The LBP Join Confirm primitive
    allows the upper layer to be notified of the completion of a LBP Join
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Join
    Confirm events back from module.

  Parameters:
    pNetworkJoinCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_LbpJoinConfirm(LBP_JOIN_CFM_PARAMS *params)
    {
        // Check result
        if (params->status == G3_SUCCESS)
        {
            
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_JOIN_CFM_CALLBACK)(LBP_JOIN_CFM_PARAMS* pNetworkJoinCfm);

// *****************************************************************************
/* LBP Leave Indication Event Handler Function Pointer

  Summary:
    Pointer to a Network Leave Indication Event handler function.

  Description:
    This data type defines the required function signature for the LBP Leave
    Indication event handling callback function. The LBP Leave Indication
    primitive is is generated by the ADP layer of a non-coordinator device to
    inform the upper layer that it has been unregistered from the network by the
    coordinator.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive  Leave
    Indication events back from module.

  Parameters:
    None.

  Example:
    <code>
    App_LbpLeaveIndication(void)
    {
        // Handle Network Leave Indication
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_DEV_LEAVE_IND_CALLBACK)(void);

// *****************************************************************************
/* LBP Leave Confirm Event Handler Function Pointer

  Summary:
    Pointer to a LBP Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Leave Confirm event handling callback function. The LBP Leave Confirm
    primitive allows the upper layer to be notified of the completion of a LBP
    Leave Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Leave
    Confirm events back from module.

  Parameters:
    status - The status of the request

  Example:
    <code>
    App_LbpLeaveConfirm(uint8_t status)
    {
        // Check result
        if (status == G3_SUCCESS)
        {
            
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_LEAVE_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* LBP Device Callback Notificatios Structure

   Summary:
    Set of event handler function pointers to receive events from LBP
    Device.

   Description:
    Defines the set of callback functions that LBP Device uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct {
    LBP_JOIN_CFM_CALLBACK joinConfirm;
    LBP_LEAVE_CFM_CALLBACK leaveConfirm;
    LBP_DEV_LEAVE_IND_CALLBACK leaveIndication;

} LBP_NOTIFICATIONS_DEV;

// *****************************************************************************
/* Function:
    void LBP_InitDev(void);

  Summary:
    Restarts the Device LBP module.

  Description:
    This routine restarts the Device LBP module.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    LBP_InitDev();
    </code>

  Remarks:
    This routine must be called before any other Coordinator LBP API function.
*/
void LBP_InitDev(void);

// *****************************************************************************
/* Function:
    void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications);

  Summary:
    Sets Device LBP notifications.

  Description:
    This routine sets the Device LBP notifications. Callback handlers for event
    notification are set in this function.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    pNotifications - Structure with callbacks used to notify Device LBP
                     events

  Returns:
    None.

  Example:
    <code>
    LBP_NOTIFICATIONS_DEV lbpNotifications = {
        .joinConfirm = appLbpJoinConfirm,
        .leaveConfirm = appLbpLeaveConfirm,
        .leaveIndication = appLbpLeaveIndication
    };

    LBP_SetNotificationsDev(&lbpNotifications);
    </code>

  Remarks:
    None.
*/
void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications);

// *****************************************************************************
/* Function:
    void LBP_SetParamDev(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLen, const uint8_t *pAttributeValue,
        LBP_SET_PARAM_CONFIRM *pSetConfirm);

  Summary:
    Sets the value of a parameter in LBP IB.

  Description:
    This routine allows the upper layer to set the value of a parameter in LBP
    IB.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    attributeId     - LBP attribute identifier to set

    attributeIndex  - Index to set when attribute is a list

    attributeLen    - Length of value to set

    pAttributeValue - Pointer to array containing value

    pSetConfirm     - Pointer to result information of the set operation

  Returns:
    None.

  Example:
    <code>
    uint8_t psk[16];
    LBP_SET_PARAM_CONFIRM setConfirm;

    LBP_SetParamDev(LBP_IB_PSK, 0, 16, &psk, &setConfirm);

    if (setConfirm.status == LBP_STATUS_OK)
    {

    }
    </code>

  Remarks:
    None.
*/
void LBP_SetParamDev(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLen, const uint8_t *pAttributeValue,
    LBP_SET_PARAM_CONFIRM *pSetConfirm);

// *****************************************************************************
/* Function:
    void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK);

  Summary:
    Forces the device register in the network without going through the
    bootstrap process.

  Description:
    This routine is used for testing purposes to force the device register in
    the network without going through the bootstrap process.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    pEUI64Address - Pointer to EUI64 address of the node

    shortAddress  - The 16-bit network address to be set

    panId         - The 16-bit PAN Id to be set

    pGMK          - Pointer to Group Master Key to set

  Returns:
    None.

  Example:
    <code>
    ADP_EXTENDED_ADDRESS eui64;
    ADP_GROUP_MASTER_KEY gmk;

    LBP_ForceRegister(&eui64, 0x0001, 0x1234, &gmk);
    </code>

  Remarks:
    None.
*/
void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK);

// *****************************************************************************
/* Function:
    void LBP_JoinRequest(uint16_t panId, uint16_t lbaAddress, uint8_t mediaType);

  Summary:
    This primitive allows the upper layer to join an existing network.

  Description:
    The LBP Join Request primitive allows the upper layer to join an existing
    network.
    
    Result is provided in the corresponding LBP Join Confirm callback.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    panId      - The 16-bit PAN identifier of the network to join

    lbaAddress - The 16-bit short address of the device acting as a 6LowPAN
                 bootstrap agent (relay)

    mediaType  - The Media Type to use for frame exchange with LBA. Only used in
                 Hybrid Profile.

  Returns:
    None.

  Example:
    <code>
    App_DiscoveryIndication(ADP_PAN_DESCRIPTOR *panDescriptor)
    {
        uint8_t mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF;

        if (panDescriptor-> == MAC_WRP_MEDIA_TYPE_IND_RF)
        {
            mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_PLC;
        }

        LBP_JoinRequest(panDescriptor->panId, panDescriptor->panId, mediaType);
        // Wait for Join Confirm    
    }
    </code>

  Remarks:
    None.
*/
void LBP_JoinRequest(uint16_t panId, uint16_t lbaAddress, uint8_t mediaType);

// *****************************************************************************
/* Function:
    void LBP_LeaveRequest(void);

  Summary:
    This primitive allows to remove itself from the network.

  Description:
    The LBP Leave Request primitive allows a non-coordinator device to remove
    itself from the network.
    
    Result is provided in the corresponding LBP Leave Confirm callback.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    LBP_LeaveRequest();
    // Wait for Leave Confirm
    </code>

  Remarks:
    None.
*/
void LBP_LeaveRequest(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _LBP_DEV_H
