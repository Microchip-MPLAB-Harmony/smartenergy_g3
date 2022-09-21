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

def instantiateComponent(g3ConfigMacComponent):
    Log.writeInfoMessage("Loading MAC Layer Configurator for G3")
    
    # Available MAC Layer(s)
    g3MACs = ["Both PLC & RF MAC Layers", "Only PLC MAC Layer", "Only RF MAC Layer"]
    g3AvailableMacs = g3ConfigMacComponent.createComboSymbol("G3_AVAILABLE_MACS", None, g3MACs)
    g3AvailableMacs.setLabel("Available MAC Layers")
    g3AvailableMacs.setVisible(True)
    g3AvailableMacs.setReadOnly(True)
    g3AvailableMacs.setDescription("Lists Available MAC Layers")
    g3AvailableMacs.setDefaultValue("Both PLC & RF MAC Layers")
    g3AvailableMacs.setDependencies(g3ChangeAvailableMacs, ["G3_AVAILABLE_MACS"])
    
    g3AvailableMacsComment = g3ConfigMacComponent.createCommentSymbol("G3_AVAILABLE_MACS_COMMENT", None)
    g3AvailableMacsComment.setVisible(True)
    g3AvailableMacsComment.setLabel("***Mac Layers availability is configured in G3 Stack Configurator Component***")

    # Boolean symbols to use in FTLs to generate code
    g3MacPLC = g3ConfigMacComponent.createBooleanSymbol("MAC_PLC_PRESENT", None)
    g3MacPLC.setVisible(False)
    g3MacPLC.setDefaultValue(True)
    g3MacRF = g3ConfigMacComponent.createBooleanSymbol("MAC_RF_PRESENT", None)
    g3MacRF.setVisible(False)
    g3MacRF.setDefaultValue(True)

    # MAC PLC Table Sizes
    g3MacPLCTables = g3ConfigMacComponent.createMenuSymbol("MAC_PLC_TABLES", None)
    g3MacPLCTables.setLabel("MAC PLC Table Sizes")
    g3MacPLCTables.setDescription("MAC PLC Table Sizes")
    g3MacPLCTables.setVisible(False)
    g3MacPLCTables.setDependencies(g3ShowPLCTables, ["G3_AVAILABLE_MACS"])
    
    g3MacPLCDeviceTable = g3ConfigMacComponent.createIntegerSymbol("MAC_PLC_DEVICE_TABLE_SIZE", g3MacPLCTables)
    g3MacPLCDeviceTable.setLabel("Device Table Size")
    g3MacPLCDeviceTable.setDescription("Security Table where incoming Frame Counters are stored")
    g3MacPLCDeviceTable.setDefaultValue(128)
    g3MacPLCDeviceTable.setMin(16)
    g3MacPLCDeviceTable.setMax(512)

    # MAC RF Table Sizes
    g3MacRFTables = g3ConfigMacComponent.createMenuSymbol("MAC_RF_TABLES", None)
    g3MacRFTables.setLabel("MAC RF Table Sizes")
    g3MacRFTables.setDescription("MAC RF Table Sizes")
    g3MacRFTables.setVisible(False)
    g3MacRFTables.setDependencies(g3ShowRFTables, ["G3_AVAILABLE_MACS"])
    
    g3MacRFDeviceTable = g3ConfigMacComponent.createIntegerSymbol("MAC_RF_DEVICE_TABLE_SIZE", g3MacRFTables)
    g3MacRFDeviceTable.setLabel("Device Table Size")
    g3MacRFDeviceTable.setDescription("Security Table where incoming Frame Counters are stored")
    g3MacRFDeviceTable.setDefaultValue(128)
    g3MacRFDeviceTable.setMin(16)
    g3MacRFDeviceTable.setMax(512)
    
    g3MacRFPOSTable = g3ConfigMacComponent.createIntegerSymbol("MAC_RF_POS_TABLE_SIZE", g3MacRFTables)
    g3MacRFPOSTable.setLabel("POS Table Size")
    g3MacRFPOSTable.setDescription("Auxiliary Table where information from Neighbouring nodes is stored")
    g3MacRFPOSTable.setDefaultValue(100)
    g3MacRFPOSTable.setMin(16)
    g3MacRFPOSTable.setMax(500)
    
    g3MacRFDSNTable = g3ConfigMacComponent.createIntegerSymbol("MAC_RF_DSN_TABLE_SIZE", g3MacRFTables)
    g3MacRFDSNTable.setLabel("Sequence Number Table Size")
    g3MacRFDSNTable.setDescription("Control Table where incoming sequence numbers are stored to avoid duplicated frames processing")
    g3MacRFDSNTable.setDefaultValue(8)
    g3MacRFDSNTable.setMin(4)
    g3MacRFDSNTable.setMax(128)

    ############################################################################
    #### Code Generation ####
    ############################################################################
    
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # MAC Wrapper Files
    pMacWrpSource = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_SOURCE", None)
    pMacWrpSource.setSourcePath("g3/src/mac_wrapper/mac_wrapper.c.ftl")
    pMacWrpSource.setOutputName("mac_wrapper.c")
    pMacWrpSource.setDestPath("stack/g3/mac")
    pMacWrpSource.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpSource.setType("SOURCE")
    pMacWrpSource.setMarkup(True)
    
    pMacWrpHeader = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_HEADER", None)
    pMacWrpHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper.h.ftl")
    pMacWrpHeader.setOutputName("mac_wrapper.h")
    pMacWrpHeader.setDestPath("stack/g3/mac")
    pMacWrpHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpHeader.setType("HEADER")
    pMacWrpHeader.setMarkup(True)
    
    pMacWrpDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_DEFS_HEADER", None)
    pMacWrpDefsHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper_defs.h.ftl")
    pMacWrpDefsHeader.setOutputName("mac_wrapper_defs.h")
    pMacWrpDefsHeader.setDestPath("stack/g3/mac")
    pMacWrpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacWrpDefsHeader.setType("HEADER")
    pMacWrpDefsHeader.setMarkup(True)
    
    # MAC Common Files
    pMacCommonSource = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_SOURCE", None)
    pMacCommonSource.setSourcePath("g3/src/mac_common/mac_common.c.ftl")
    pMacCommonSource.setOutputName("mac_common.c")
    pMacCommonSource.setDestPath("stack/g3/mac")
    pMacCommonSource.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonSource.setType("SOURCE")
    pMacCommonSource.setMarkup(True)
    
    pMacCommonHeader = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_HEADER", None)
    pMacCommonHeader.setSourcePath("g3/src/mac_common/mac_common.h")
    pMacCommonHeader.setOutputName("mac_common.h")
    pMacCommonHeader.setDestPath("stack/g3/mac")
    pMacCommonHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonHeader.setType("HEADER")
    
    pMacCommonDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_DEFS_HEADER", None)
    pMacCommonDefsHeader.setSourcePath("g3/src/mac_common/mac_common_defs.h")
    pMacCommonDefsHeader.setOutputName("mac_common_defs.h")
    pMacCommonDefsHeader.setDestPath("stack/g3/mac")
    pMacCommonDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    pMacCommonDefsHeader.setType("HEADER")

    # MAC PLC Files
    global macPlcLibFile
    macPlcLibFile = g3ConfigMacComponent.createLibrarySymbol("G3_MAC_PLC_LIBRARY", None)
    macPlcLibFile.setSourcePath("g3/libs/g3_lib_plc_mac.a")
    macPlcLibFile.setOutputName("g3_lib_plc_mac.a")
    macPlcLibFile.setDestPath("stack/g3/mac")
    macPlcLibFile.setEnabled(True)
    
    macPlcHeader = g3ConfigMacComponent.createFileSymbol("MAC_PLC_HEADER", None)
    macPlcHeader.setSourcePath("g3/src/mac_plc/mac_plc.h")
    macPlcHeader.setOutputName("mac_plc.h")
    macPlcHeader.setDestPath("stack/g3/mac")
    macPlcHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macPlcHeader.setType("HEADER")
    
    macPlcDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_PLC_DEFS_HEADER", None)
    macPlcDefsHeader.setSourcePath("g3/src/mac_plc/mac_plc_defs.h")
    macPlcDefsHeader.setOutputName("mac_plc_defs.h")
    macPlcDefsHeader.setDestPath("stack/g3/mac")
    macPlcDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macPlcDefsHeader.setType("HEADER")
    
    macPlcMibHeader = g3ConfigMacComponent.createFileSymbol("MAC_PLC_MIB_HEADER", None)
    macPlcMibHeader.setSourcePath("g3/src/mac_plc/mac_plc_mib.h")
    macPlcMibHeader.setOutputName("mac_plc_mib.h")
    macPlcMibHeader.setDestPath("stack/g3/mac")
    macPlcMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macPlcMibHeader.setType("HEADER")
    
    # MAC RF Files
    global macRfLibFile
    macRfLibFile = g3ConfigMacComponent.createLibrarySymbol("G3_MAC_RF_LIBRARY", None)
    macRfLibFile.setSourcePath("g3/libs/g3_lib_rf_mac.a")
    macRfLibFile.setOutputName("g3_lib_rf_mac.a")
    macRfLibFile.setDestPath("stack/g3/mac")
    macRfLibFile.setEnabled(True)
    
    macRfHeader = g3ConfigMacComponent.createFileSymbol("MAC_RF_HEADER", None)
    macRfHeader.setSourcePath("g3/src/mac_plc/mac_rf.h")
    macRfHeader.setOutputName("mac_rf.h")
    macRfHeader.setDestPath("stack/g3/mac")
    macRfHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macRfHeader.setType("HEADER")
    
    macRfDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_RF_DEFS_HEADER", None)
    macRfDefsHeader.setSourcePath("g3/src/mac_plc/mac_rf_defs.h")
    macRfDefsHeader.setOutputName("mac_rf_defs.h")
    macRfDefsHeader.setDestPath("stack/g3/mac")
    macRfDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macRfDefsHeader.setType("HEADER")
    
    macRfMibHeader = g3ConfigMacComponent.createFileSymbol("MAC_RF_MIB_HEADER", None)
    macRfMibHeader.setSourcePath("g3/src/mac_plc/mac_rf_mib.h")
    macRfMibHeader.setOutputName("mac_rf_mib.h")
    macRfMibHeader.setDestPath("stack/g3/mac")
    macRfMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/")
    macRfMibHeader.setType("HEADER")

################################################################################
#### Business Logic ####
################################################################################

def g3ChangeAvailableMacs(symbol, event):
    if event["value"] == "Both PLC & RF MAC Layers":
        # Both MACs available
        Database.setSymbolValue("g3_mac_config", "MAC_PLC_PRESENT", True)
        Database.setSymbolValue("g3_mac_config", "MAC_RF_PRESENT", True)
    elif event["value"] == "Only PLC MAC Layer":
        # Only PLC MAC
        Database.setSymbolValue("g3_mac_config", "MAC_PLC_PRESENT", True)
        Database.setSymbolValue("g3_mac_config", "MAC_RF_PRESENT", False)
    elif event["value"] == "Only RF MAC Layer":
        # Only RF MAC
        Database.setSymbolValue("g3_mac_config", "MAC_PLC_PRESENT", False)
        Database.setSymbolValue("g3_mac_config", "MAC_RF_PRESENT", True)
    else:
        # Unknown option, behave as both MACs available
        Database.setSymbolValue("g3_mac_config", "MAC_PLC_PRESENT", True)
        Database.setSymbolValue("g3_mac_config", "MAC_RF_PRESENT", True)

def g3ShowPLCTables(symbol, event):
    if event["value"] == "Both PLC & RF MAC Layers":
        # Both MACs available
        symbol.setVisible(True)
    elif event["value"] == "Only PLC MAC Layer":
        # Only PLC MAC
        symbol.setVisible(True)
    elif event["value"] == "Only RF MAC Layer":
        # Only RF MAC
        symbol.setVisible(False)
    else:
        # Unknown option, behave as both MACs available
        symbol.setVisible(True)

def g3ShowRFTables(symbol, event):
    if event["value"] == "Both PLC & RF MAC Layers":
        # Both MACs available
        symbol.setVisible(True)
    elif event["value"] == "Only PLC MAC Layer":
        # Only PLC MAC
        symbol.setVisible(False)
    elif event["value"] == "Only RF MAC Layer":
        # Only RF MAC
        symbol.setVisible(True)
    else:
        # Unknown option, behave as both MACs available
        symbol.setVisible(True)

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
