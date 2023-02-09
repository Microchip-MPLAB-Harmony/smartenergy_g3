/*******************************************************************************
  G3 ADP Serialization Header File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_serial.h

  Summary:
    G3 ADP Serialization API Header File.

  Description:
    The G3 ADP Serialization allows to serialize the ADP and LBP API through
    USI interface in order to run the application on an external device. This
    file contains definitions of the API of G3 ADP Serialization.
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

#ifndef _ADP_SERIAL_H
#define _ADP_SERIAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: MAC Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ ADP_SERIAL_Initialize (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    );

  Summary:
    Initializes the ADP Serialization module for the specified index.

  Description:
    This routine initializes the ADP Serialization module for the specified
    index.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized (single instance
            allowed)

    init  - Pointer to the init data structure containing any data necessary to
            initialize the module.

  Returns:
    If successful, returns a valid module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_INIT initData;
    SYS_MODULE_OBJ sysObjAdpSerial;

    sysObjAdpSerial = ADP_SERIAL_Initialize(G3_ADP_SERIAL_INDEX_0, &initData);
    if (sysObjAdpSerial == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other ADP Serialization routine is
    called.
*/
SYS_MODULE_OBJ ADP_SERIAL_Initialize (
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
);

// *****************************************************************************
/* Function:
    void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object);

  Summary:
    Maintains ADP Serialization State Machine.

  Description:
    This routine maintains ADP Serialization State Machine.

  Precondition:
    The ADP_SERIAL_Initialize routine must have been called to obtain a valid
    system object.

  Parameters:
    object - System object handle, returned from the ADP_SERIAL_Initialize
             routine.

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ sysObjAdpSerial; // Returned from ADP_SERIAL_Initialize

    while (true)
    {
        ADP_SERIAL_Tasks(sysObjAdpSerial);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's tasks routine (SYS_Tasks).
*/
void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _ADP_SERIAL_H
