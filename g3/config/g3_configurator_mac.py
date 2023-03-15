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
    g3MacRFPOSTable.setMax(512)
    
    g3MacRFDSNTable = g3ConfigMacComponent.createIntegerSymbol("MAC_RF_DSN_TABLE_SIZE", g3MacRFTables)
    g3MacRFDSNTable.setLabel("Sequence Number Table Size")
    g3MacRFDSNTable.setDescription("Control Table where incoming sequence numbers are stored to avoid duplicated frames processing")
    g3MacRFDSNTable.setDefaultValue(8)
    g3MacRFDSNTable.setMin(4)
    g3MacRFDSNTable.setMax(128)

    g3MacSerializationEnable = g3ConfigMacComponent.createBooleanSymbol("MAC_SERIALIZATION_EN", None)
    g3MacSerializationEnable.setLabel("Enable MAC Serialization")
    g3MacSerializationEnable.setDescription("Enable/disable MAC serialization through USI")
    g3MacSerializationEnable.setDefaultValue(False)

    g3MacSerializationUsiInstance = g3ConfigMacComponent.createIntegerSymbol("MAC_SERIALIZATION_USI_INSTANCE", g3MacSerializationEnable)
    g3MacSerializationUsiInstance.setLabel("USI Instance")
    g3MacSerializationUsiInstance.setDescription("USI instance used for MAC serialization")
    g3MacSerializationUsiInstance.setDefaultValue(0)
    g3MacSerializationUsiInstance.setMax(0)
    g3MacSerializationUsiInstance.setMin(0)
    g3MacSerializationUsiInstance.setVisible(False)
    g3MacSerializationUsiInstance.setDependencies(g3ShowUsiInstance, ["MAC_SERIALIZATION_EN"])

    ############################################################################
    #### Code Generation ####
    ############################################################################
    
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # MAC Wrapper Files
    pMacWrpSource = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_SOURCE", None)
    pMacWrpSource.setSourcePath("g3/src/mac_wrapper/mac_wrapper.c.ftl")
    pMacWrpSource.setOutputName("mac_wrapper.c")
    pMacWrpSource.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpSource.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpSource.setType("SOURCE")
    pMacWrpSource.setMarkup(True)
    
    pMacWrpHeader = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_HEADER", None)
    pMacWrpHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper.h")
    pMacWrpHeader.setOutputName("mac_wrapper.h")
    pMacWrpHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setType("HEADER")
    pMacWrpHeader.setMarkup(False)
    
    pMacWrpDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_WRAPPER_DEFS_HEADER", None)
    pMacWrpDefsHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper_defs.h")
    pMacWrpDefsHeader.setOutputName("mac_wrapper_defs.h")
    pMacWrpDefsHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setType("HEADER")
    
    # MAC Common Files
    pMacCommonSource = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_SOURCE", None)
    pMacCommonSource.setSourcePath("g3/src/mac_common/mac_common.c.ftl")
    pMacCommonSource.setOutputName("mac_common.c")
    pMacCommonSource.setDestPath("stack/g3/mac/mac_common")
    pMacCommonSource.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonSource.setType("SOURCE")
    pMacCommonSource.setMarkup(True)
    
    pMacCommonHeader = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_HEADER", None)
    pMacCommonHeader.setSourcePath("g3/src/mac_common/mac_common.h")
    pMacCommonHeader.setOutputName("mac_common.h")
    pMacCommonHeader.setDestPath("stack/g3/mac/mac_common")
    pMacCommonHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonHeader.setType("HEADER")
    
    pMacCommonDefsHeader = g3ConfigMacComponent.createFileSymbol("MAC_COMMON_DEFS_HEADER", None)
    pMacCommonDefsHeader.setSourcePath("g3/src/mac_common/mac_common_defs.h")
    pMacCommonDefsHeader.setOutputName("mac_common_defs.h")
    pMacCommonDefsHeader.setDestPath("stack/g3/mac/mac_common")
    pMacCommonDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonDefsHeader.setType("HEADER")

    #####################################################################################################################################
    # G3 STACK TEMPLATES 
    
    g3StackSystemConfigFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_CONFIGURATION", None)
    g3StackSystemConfigFile.setType("STRING")
    g3StackSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    g3StackSystemConfigFile.setSourcePath("g3/templates/system/configuration.h.ftl")
    g3StackSystemConfigFile.setMarkup(True)

    g3StackSystemDefFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_DEF", None)
    g3StackSystemDefFile.setType("STRING")
    g3StackSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    g3StackSystemDefFile.setSourcePath("g3/templates/system/definitions.h.ftl")
    g3StackSystemDefFile.setMarkup(True)

    g3StackSystemDefFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_DEF_OBJ", None)
    g3StackSystemDefFile.setType("STRING")
    g3StackSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    g3StackSystemDefFile.setSourcePath("g3/templates/system/definitions_objects.h.ftl")
    g3StackSystemDefFile.setMarkup(True)

    plcSymSystemInitDataFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_INIT_DATA", None)
    plcSymSystemInitDataFile.setType("STRING")
    plcSymSystemInitDataFile.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    plcSymSystemInitDataFile.setSourcePath("g3/templates/system/initialize_data.c.ftl")
    plcSymSystemInitDataFile.setMarkup(True)

    plcSystemInitFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_INIT", None)
    plcSystemInitFile.setType("STRING")
    plcSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE")
    plcSystemInitFile.setSourcePath("g3/templates/system/initialize.c.ftl")
    plcSystemInitFile.setMarkup(True)

    plcSystemTasksFile = g3ConfigMacComponent.createFileSymbol("G3_STACK_SYS_TASK", None)
    plcSystemTasksFile.setType("STRING")
    plcSystemTasksFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    plcSystemTasksFile.setSourcePath("g3/templates/system/system_tasks.c.ftl")
    plcSystemTasksFile.setMarkup(True)

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

def g3ShowUsiInstance(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print("Set Symbol Failure" + component + ":" + symbol + ":" + str(value))
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print("handleMessage: Set Symbol")
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
