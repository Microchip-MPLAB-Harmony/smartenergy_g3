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

def instantiateComponent(g3MacPlcComponent):
    
    Log.writeInfoMessage("Loading PLC MAC for G3")
    
    configName = Variables.get("__CONFIGURATION_NAME")

    # MAC PLC Files
    global macPlcLibFile
    macPlcLibFile = g3MacPlcComponent.createLibrarySymbol("G3_MAC_PLC_LIBRARY", None)
    macPlcLibFile.setSourcePath("g3/libs/g3_lib_mac_plc.a")
    macPlcLibFile.setOutputName("g3_lib_plc_mac.a")
    macPlcLibFile.setDestPath("stack/g3/mac/mac_plc")
    macPlcLibFile.setEnabled(True)
    
    macPlcHeader = g3MacPlcComponent.createFileSymbol("MAC_PLC_HEADER", None)
    macPlcHeader.setSourcePath("g3/src/mac_plc/mac_plc.h")
    macPlcHeader.setOutputName("mac_plc.h")
    macPlcHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcHeader.setType("HEADER")
    
    macPlcDefsHeader = g3MacPlcComponent.createFileSymbol("MAC_PLC_DEFS_HEADER", None)
    macPlcDefsHeader.setSourcePath("g3/src/mac_plc/mac_plc_defs.h")
    macPlcDefsHeader.setOutputName("mac_plc_defs.h")
    macPlcDefsHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcDefsHeader.setType("HEADER")
    
    macPlcMibHeader = g3MacPlcComponent.createFileSymbol("MAC_PLC_MIB_HEADER", None)
    macPlcMibHeader.setSourcePath("g3/src/mac_plc/mac_plc_mib.h")
    macPlcMibHeader.setOutputName("mac_plc_mib.h")
    macPlcMibHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcMibHeader.setType("HEADER")
