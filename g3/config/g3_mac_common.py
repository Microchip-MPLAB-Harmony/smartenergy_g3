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

def instantiateComponent(g3MacCommonComponent):
    
    Log.writeInfoMessage("Loading MAC Common for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # MAC Common Files
    pMacCommonSource = g3MacCommonComponent.createFileSymbol("MAC_COMMON_SOURCE", None)
    pMacCommonSource.setSourcePath("g3/src/mac_common/MacCommon.c")
    pMacCommonSource.setOutputName("MacCommon.c")
    pMacCommonSource.setDestPath("stack/g3/mac")
    pMacCommonSource.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonSource.setType("SOURCE")
    
    pMacCommonHeader = g3MacCommonComponent.createFileSymbol("MAC_COMMON_HEADER", None)
    pMacCommonHeader.setSourcePath("g3/src/mac_common/MacCommon.h")
    pMacCommonHeader.setOutputName("MacCommon.h")
    pMacCommonHeader.setDestPath("stack/g3/mac")
    pMacCommonHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonHeader.setType("HEADER")
    
    pMacCommonDefsHeader = g3MacCommonComponent.createFileSymbol("MAC_COMMON_DEFS_HEADER", None)
    pMacCommonDefsHeader.setSourcePath("g3/src/mac_common/MacCommonDefs.h")
    pMacCommonDefsHeader.setOutputName("MacCommonDefs.h")
    pMacCommonDefsHeader.setDestPath("stack/g3/mac")
    pMacCommonDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonDefsHeader.setType("HEADER")