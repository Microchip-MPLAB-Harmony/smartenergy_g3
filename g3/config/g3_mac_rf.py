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

def instantiateComponent(g3MacRfComponent):
    
    Log.writeInfoMessage("Loading PLC MAC for G3")
    
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # MAC RF Files
    global macRfLibFile
    macRfLibFile = g3MacRfComponent.createLibrarySymbol("G3_MAC_RF_LIBRARY", None)
    macRfLibFile.setSourcePath("g3/libs/g3_lib_mac_rf.a")
    macRfLibFile.setOutputName("g3_lib_rf_mac.a")
    macRfLibFile.setDestPath("stack/g3/mac/mac_rf")
    macRfLibFile.setEnabled(True)
    
    macRfHeader = g3MacRfComponent.createFileSymbol("MAC_RF_HEADER", None)
    macRfHeader.setSourcePath("g3/src/mac_rf/mac_rf.h")
    macRfHeader.setOutputName("mac_rf.h")
    macRfHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfHeader.setType("HEADER")
    
    macRfDefsHeader = g3MacRfComponent.createFileSymbol("MAC_RF_DEFS_HEADER", None)
    macRfDefsHeader.setSourcePath("g3/src/mac_rf/mac_rf_defs.h")
    macRfDefsHeader.setOutputName("mac_rf_defs.h")
    macRfDefsHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfDefsHeader.setType("HEADER")
    
    macRfMibHeader = g3MacRfComponent.createFileSymbol("MAC_RF_MIB_HEADER", None)
    macRfMibHeader.setSourcePath("g3/src/mac_rf/mac_rf_mib.h")
    macRfMibHeader.setOutputName("mac_rf_mib.h")
    macRfMibHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfMibHeader.setType("HEADER")
