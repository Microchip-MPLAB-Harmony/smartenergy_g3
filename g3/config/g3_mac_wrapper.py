"""*****************************************************************************
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
*****************************************************************************"""

def instantiateComponent(g3MacWrpComponent):
    
    Log.writeInfoMessage("Loading MAC Wrapper for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # MAC Wrapper Files
    pMacWrpSource = g3MacWrpComponent.createFileSymbol("MAC_WRAPPER_SOURCE", None)
    pMacWrpSource.setSourcePath("g3/src/mac_wrapper/mac_wrapper.c")
    pMacWrpSource.setOutputName("mac_wrapper.c")
    pMacWrpSource.setDestPath("stack/g3/mac")
    pMacWrpSource.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpSource.setType("SOURCE")
    
    pMacWrpHeader = g3MacWrpComponent.createFileSymbol("MAC_WRAPPER_HEADER", None)
    pMacWrpHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper.h")
    pMacWrpHeader.setOutputName("mac_wrapper.h")
    pMacWrpHeader.setDestPath("stack/g3/mac")
    pMacWrpHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpHeader.setType("HEADER")
    
    pMacWrpDefsHeader = g3MacWrpComponent.createFileSymbol("MAC_WRAPPER_DEFS_HEADER", None)
    pMacWrpDefsHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper_defs.h")
    pMacWrpDefsHeader.setOutputName("mac_wrapper_defs.h")
    pMacWrpDefsHeader.setDestPath("stack/g3/mac")
    pMacWrpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpDefsHeader.setType("HEADER")