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

def instantiateComponent(g3ConfigComponent):
    g3ConfigStackGroup = Database.findGroup("G3 STACK")
    if (g3ConfigStackGroup == None):
        g3ConfigStackGroup = Database.createGroup(None, "G3 STACK")
    Database.setActiveGroup("G3 STACK")

    # Select G3 mode
    g3Modes = ["-- Select a G3 Stack Mode from list --", "G3 Stack (ADP + MAC) Hybrid PLC & RF", "G3 Stack (ADP + MAC) PLC", "G3 Stack (ADP + MAC) RF",
               "G3 MAC Layer Hybrid PLC & RF", "G3 PLC MAC Layer", "G3 RF MAC Layer"]
    g3ConfigMode = g3ConfigComponent.createComboSymbol("G3_MODE", None, g3Modes)
    g3ConfigMode.setLabel("G3 Mode of Operation")
    g3ConfigMode.setVisible(True)
    g3ConfigMode.setDescription("Select G3 Mode")
    g3ConfigMode.setDependencies(g3ConfigSelection, ["G3_MODE"])

    # Select G3 role
    global g3ConfigRole
    g3Roles = ["PAN Device", "PAN Coordinator", "Dynamic (Selected upon Initialization)"]
    g3ConfigRole = g3ConfigComponent.createComboSymbol("G3_ROLE", None, g3Roles)
    g3ConfigRole.setLabel("Role for G3 Node")
    g3ConfigRole.setVisible(True)
    g3ConfigRole.setDescription("Select G3 Role")
    g3ConfigRole.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])
    g3ConfigRole.setDefaultValue("PAN Device")

    g3DebugEnable = g3ConfigComponent.createBooleanSymbol("G3_DEBUG_ENABLE", None)
    g3DebugEnable.setLabel("Enable G3 Stack Debug Traces")
    g3DebugEnable.setDescription("Enable/disable G3 Debug messages through SYS_DEBUG Service")
    g3DebugEnable.setDefaultValue(False)
    g3DebugEnable.setVisible(True)
    g3DebugEnable.setDependencies(g3DebugChange, ["G3_DEBUG_ENABLE"])

    # MAC PLC Table Sizes
    global g3MacPLCTables
    g3MacPLCTables = g3ConfigComponent.createMenuSymbol("MAC_PLC_TABLES", None)
    g3MacPLCTables.setLabel("MAC PLC Table Sizes")
    g3MacPLCTables.setDescription("MAC PLC Table Sizes")
    g3MacPLCTables.setVisible(False)
    
    g3MacPLCDeviceTable = g3ConfigComponent.createIntegerSymbol("MAC_PLC_DEVICE_TABLE_SIZE", g3MacPLCTables)
    g3MacPLCDeviceTable.setLabel("Device Table Size")
    g3MacPLCDeviceTable.setDescription("Security Table where incoming Frame Counters are stored")
    g3MacPLCDeviceTable.setDefaultValue(128)
    g3MacPLCDeviceTable.setMin(16)
    g3MacPLCDeviceTable.setMax(512)

    # MAC RF Table Sizes
    global g3MacRFTables
    g3MacRFTables = g3ConfigComponent.createMenuSymbol("MAC_RF_TABLES", None)
    g3MacRFTables.setLabel("MAC RF Table Sizes")
    g3MacRFTables.setDescription("MAC RF Table Sizes")
    g3MacRFTables.setVisible(False)
    
    g3MacRFDeviceTable = g3ConfigComponent.createIntegerSymbol("MAC_RF_DEVICE_TABLE_SIZE", g3MacRFTables)
    g3MacRFDeviceTable.setLabel("Device Table Size")
    g3MacRFDeviceTable.setDescription("Security Table where incoming Frame Counters are stored")
    g3MacRFDeviceTable.setDefaultValue(128)
    g3MacRFDeviceTable.setMin(16)
    g3MacRFDeviceTable.setMax(512)
    
    g3MacRFPOSTable = g3ConfigComponent.createIntegerSymbol("MAC_RF_POS_TABLE_SIZE", g3MacRFTables)
    g3MacRFPOSTable.setLabel("POS Table Size")
    g3MacRFPOSTable.setDescription("Auxiliary Table where information from Neighbouring nodes is stored")
    g3MacRFPOSTable.setDefaultValue(100)
    g3MacRFPOSTable.setMin(16)
    g3MacRFPOSTable.setMax(512)
    
    g3MacRFDSNTable = g3ConfigComponent.createIntegerSymbol("MAC_RF_DSN_TABLE_SIZE", g3MacRFTables)
    g3MacRFDSNTable.setLabel("Sequence Number Table Size")
    g3MacRFDSNTable.setDescription("Control Table where incoming sequence numbers are stored to avoid duplicated frames processing")
    g3MacRFDSNTable.setDefaultValue(8)
    g3MacRFDSNTable.setMin(4)
    g3MacRFDSNTable.setMax(128)

    g3MacSerializationEnable = g3ConfigComponent.createBooleanSymbol("MAC_SERIALIZATION_EN", None)
    g3MacSerializationEnable.setLabel("Enable MAC Serialization")
    g3MacSerializationEnable.setDescription("Enable/disable MAC serialization through USI")
    g3MacSerializationEnable.setDefaultValue(False)

    g3MacSerializationUsiInstance = g3ConfigComponent.createIntegerSymbol("MAC_SERIALIZATION_USI_INSTANCE", g3MacSerializationEnable)
    g3MacSerializationUsiInstance.setLabel("USI Instance")
    g3MacSerializationUsiInstance.setDescription("USI instance used for MAC serialization")
    g3MacSerializationUsiInstance.setDefaultValue(0)
    g3MacSerializationUsiInstance.setMax(0)
    g3MacSerializationUsiInstance.setMin(0)
    g3MacSerializationUsiInstance.setVisible(False)
    g3MacSerializationUsiInstance.setDependencies(g3ShowUsiInstance, ["MAC_SERIALIZATION_EN"])

    # Boolean symbols to use in FTLs to generate code
    g3DeviceRole = g3ConfigComponent.createBooleanSymbol("G3_DEVICE", None)
    g3DeviceRole.setVisible(False)
    g3DeviceRole.setDefaultValue(True)
    g3CoordRole = g3ConfigComponent.createBooleanSymbol("G3_COORDINATOR", None)
    g3CoordRole.setVisible(False)
    g3CoordRole.setDefaultValue(False)

    g3MacPLC = g3ConfigComponent.createBooleanSymbol("MAC_PLC_PRESENT", None)
    g3MacPLC.setVisible(False)
    g3MacPLC.setDefaultValue(True)
    g3MacRF = g3ConfigComponent.createBooleanSymbol("MAC_RF_PRESENT", None)
    g3MacRF.setVisible(False)
    g3MacRF.setDefaultValue(True)

    g3ADP = g3ConfigComponent.createBooleanSymbol("ADP_PRESENT", None)
    g3ADP.setVisible(False)
    g3ADP.setDefaultValue(True)

    ###########################################################################################################

    # Enable LOADng Configuration
    g3ConfigLOADng = g3ConfigComponent.createBooleanSymbol("LOADNG_ENABLE", None)
    g3ConfigLOADng.setLabel("Enable LOADng Routing")
    g3ConfigLOADng.setVisible(True)
    g3ConfigLOADng.setDescription("Enable LOADng Routing Protocol")
    g3ConfigLOADng.setDependencies(g3LOADngEnable, ["LOADNG_ENABLE"])
    g3ConfigLOADng.setDefaultValue(True)

    loadngPendingRReqTable = g3ConfigComponent.createIntegerSymbol("LOADNG_PENDING_RREQ_TABLE_SIZE", g3ConfigLOADng)
    loadngPendingRReqTable.setLabel("Pending RREQ table size")
    loadngPendingRReqTable.setDescription("Number of RREQs/RERRs that can be stored to respect the parameter ADP_IB_RREQ_WAIT")
    loadngPendingRReqTable.setDefaultValue(6)
    loadngPendingRReqTable.setMin(1)
    loadngPendingRReqTable.setMax(128)
    loadngPendingRReqTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngRRepGenerationTable = g3ConfigComponent.createIntegerSymbol("LOADNG_RREP_GENERATION_TABLE_SIZE", g3ConfigLOADng)
    loadngRRepGenerationTable.setLabel("RREQ generation table size")
    loadngRRepGenerationTable.setDescription("Number of RREQs from different sources, that can be handled at the same time")
    loadngRRepGenerationTable.setDefaultValue(3)
    loadngRRepGenerationTable.setMin(1)
    loadngRRepGenerationTable.setMax(128)
    loadngRRepGenerationTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngRReqForwardingTable = g3ConfigComponent.createIntegerSymbol("LOADNG_RREQ_FORWARDING_TABLE_SIZE", g3ConfigLOADng)
    loadngRReqForwardingTable.setLabel("RREP forwarding table size")
    loadngRReqForwardingTable.setDescription("Number of entries of RREP forwarding table for LOADNG")
    loadngRReqForwardingTable.setDefaultValue(5)
    loadngRReqForwardingTable.setMin(1)
    loadngRReqForwardingTable.setMax(128)
    loadngRReqForwardingTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngDiscoverRouteTable = g3ConfigComponent.createIntegerSymbol("LOADNG_DISCOVER_ROUTE_TABLE_SIZE", g3ConfigLOADng)
    loadngDiscoverRouteTable.setLabel("Discover route table size")
    loadngDiscoverRouteTable.setDescription("Number of route discover that can be handled at the same time")
    loadngDiscoverRouteTable.setDefaultValue(3)
    loadngDiscoverRouteTable.setMin(1)
    loadngDiscoverRouteTable.setMax(128)
    loadngDiscoverRouteTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpRoutingTable = g3ConfigComponent.createIntegerSymbol("ADP_ROUTING_TABLE_SIZE", g3ConfigLOADng)
    adpRoutingTable.setLabel("Routing table size")
    adpRoutingTable.setDescription("Number of entries in the Routing Table for LOADNG")
    adpRoutingTable.setDefaultValue(150)
    adpRoutingTable.setMin(1)
    adpRoutingTable.setMax(1024)
    adpRoutingTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpBlacklistTable = g3ConfigComponent.createIntegerSymbol("ADP_BLACKLIST_TABLE_SIZE", g3ConfigLOADng)
    adpBlacklistTable.setLabel("Blacklist table size")
    adpBlacklistTable.setDescription("Number of entries in the Blacklist Table for LOADNG")
    adpBlacklistTable.setDefaultValue(20)
    adpBlacklistTable.setMin(1)
    adpBlacklistTable.setMax(256)
    adpBlacklistTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpRoutingSet = g3ConfigComponent.createIntegerSymbol("ADP_ROUTING_SET_SIZE", g3ConfigLOADng)
    adpRoutingSet.setLabel("Routing set size")
    adpRoutingSet.setDescription("Number of entries in the Routing Set for LOADNG")
    adpRoutingSet.setDefaultValue(30)
    adpRoutingSet.setMin(1)
    adpRoutingSet.setMax(256)
    adpRoutingSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpDestinationAddressSet = g3ConfigComponent.createIntegerSymbol("ADP_DESTINATION_ADDRESS_SET_SIZE", g3ConfigLOADng)
    adpDestinationAddressSet.setLabel("Destination address set size")
    adpDestinationAddressSet.setDescription("Number of entries in the Destination Address Set Table")
    adpDestinationAddressSet.setDefaultValue(1)
    adpDestinationAddressSet.setMin(1)
    adpDestinationAddressSet.setMax(128)
    adpDestinationAddressSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    g3CountBuffers1280 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_1280", None)
    g3CountBuffers1280.setLabel("Number of 1280-byte buffers")
    g3CountBuffers1280.setDescription("Number of 1280-byte buffers for adaptation layer")
    g3CountBuffers1280.setDefaultValue(1)
    g3CountBuffers1280.setMin(1)
    g3CountBuffers1280.setMax(16)

    g3CountBuffers400 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_400", None)
    g3CountBuffers400.setLabel("Number of 400-byte buffers")
    g3CountBuffers400.setDescription("Number of 400-byte buffers for adaptation layer")
    g3CountBuffers400.setDefaultValue(3)
    g3CountBuffers400.setMin(1)
    g3CountBuffers400.setMax(32)

    g3CountBuffers100 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_100", None)
    g3CountBuffers100.setLabel("Number of 100-byte buffers")
    g3CountBuffers100.setDescription("Number of 100-byte buffers for adaptation layer")
    g3CountBuffers100.setDefaultValue(3)
    g3CountBuffers100.setMin(1)
    g3CountBuffers100.setMax(64)

    g3FragmentedTransferTableSize = g3ConfigComponent.createIntegerSymbol("ADP_FRAGMENTED_TRANSFER_TABLE_SIZE", None)
    g3FragmentedTransferTableSize.setLabel("Fragmented transfer table size")
    g3FragmentedTransferTableSize.setDescription("The number of fragmented transfers which can be handled in parallel")
    g3FragmentedTransferTableSize.setDefaultValue(1)
    g3FragmentedTransferTableSize.setMin(1)
    g3FragmentedTransferTableSize.setMax(16)

    g3AdpSerializationEnable = g3ConfigComponent.createBooleanSymbol("ADP_SERIALIZATION_EN", None)
    g3AdpSerializationEnable.setLabel("Enable ADP and LBP Serialization")
    g3AdpSerializationEnable.setDescription("Enable/disable ADP and LBP serialization through USI")
    g3AdpSerializationEnable.setDefaultValue(False)

    g3AdpSerializationUsiInstance = g3ConfigComponent.createIntegerSymbol("ADP_SERIALIZATION_USI_INSTANCE", g3AdpSerializationEnable)
    g3AdpSerializationUsiInstance.setLabel("USI Instance")
    g3AdpSerializationUsiInstance.setDescription("USI instance used for ADP and LBP serialization")
    g3AdpSerializationUsiInstance.setDefaultValue(0)
    g3AdpSerializationUsiInstance.setMax(0)
    g3AdpSerializationUsiInstance.setMin(0)
    g3AdpSerializationUsiInstance.setVisible(False)
    g3AdpSerializationUsiInstance.setDependencies(showUsiInstance, ["ADP_SERIALIZATION_EN"])

    ########################################################################################################################

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # LBP API
    global pLbpCoordHeader
    pLbpCoordHeader = g3ConfigComponent.createFileSymbol("LBP_COORD_HEADER", None)
    pLbpCoordHeader.setSourcePath("g3/src/lbp/include/lbp_coord.h")
    pLbpCoordHeader.setOutputName("lbp_coord.h")
    pLbpCoordHeader.setDestPath("stack/g3/adaptation")
    pLbpCoordHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpCoordHeader.setType("HEADER")
    pLbpCoordHeader.setEnabled(False)
    pLbpCoordHeader.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])

    global pLbpDevHeader
    pLbpDevHeader = g3ConfigComponent.createFileSymbol("LBP_DEV_HEADER", None)
    pLbpDevHeader.setSourcePath("g3/src/lbp/include/lbp_dev.h")
    pLbpDevHeader.setOutputName("lbp_dev.h")
    pLbpDevHeader.setDestPath("stack/g3/adaptation")
    pLbpDevHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpDevHeader.setType("HEADER")
    pLbpDevHeader.setEnabled(True)
    pLbpDevHeader.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])

    global pLbpDefsHeader
    pLbpDefsHeader = g3ConfigComponent.createFileSymbol("LBP_DEFS_HEADER", None)
    pLbpDefsHeader.setSourcePath("g3/src/lbp/include/lbp_defs.h")
    pLbpDefsHeader.setOutputName("lbp_defs.h")
    pLbpDefsHeader.setDestPath("stack/g3/adaptation")
    pLbpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpDefsHeader.setType("HEADER")
    
    # LBP Files
    global pLbpEapSource
    pLbpEapSource = g3ConfigComponent.createFileSymbol("EAP_SOURCE", None)
    pLbpEapSource.setSourcePath("g3/src/lbp/source/eap_psk.c")
    pLbpEapSource.setOutputName("eap_psk.c")
    pLbpEapSource.setDestPath("stack/g3/adaptation")
    pLbpEapSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEapSource.setType("SOURCE")
    
    global pLbpEapHeader
    pLbpEapHeader = g3ConfigComponent.createFileSymbol("EAP_HEADER", None)
    pLbpEapHeader.setSourcePath("g3/src/lbp/source/eap_psk.h")
    pLbpEapHeader.setOutputName("eap_psk.h")
    pLbpEapHeader.setDestPath("stack/g3/adaptation")
    pLbpEapHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEapHeader.setType("SOURCE")

    global pLbpEncodeDecodeSource
    pLbpEncodeDecodeSource = g3ConfigComponent.createFileSymbol("LBP_ENCODE_DECODE_SOURCE", None)
    pLbpEncodeDecodeSource.setSourcePath("g3/src/lbp/source/lbp_encode_decode.c")
    pLbpEncodeDecodeSource.setOutputName("lbp_encode_decode.c")
    pLbpEncodeDecodeSource.setDestPath("stack/g3/adaptation")
    pLbpEncodeDecodeSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEncodeDecodeSource.setType("SOURCE")

    global pLbpEncodeDecodeHeader
    pLbpEncodeDecodeHeader = g3ConfigComponent.createFileSymbol("LBP_ENCODE_DECODE_HEADER", None)
    pLbpEncodeDecodeHeader.setSourcePath("g3/src/lbp/source/lbp_encode_decode.h")
    pLbpEncodeDecodeHeader.setOutputName("lbp_encode_decode.h")
    pLbpEncodeDecodeHeader.setDestPath("stack/g3/adaptation")
    pLbpEncodeDecodeHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEncodeDecodeHeader.setType("SOURCE")

    global pLbpCoordSource
    pLbpCoordSource = g3ConfigComponent.createFileSymbol("LBP_COORD_SOURCE", None)
    pLbpCoordSource.setSourcePath("g3/src/lbp/source/lbp_coord.c")
    pLbpCoordSource.setOutputName("lbp_coord.c")
    pLbpCoordSource.setDestPath("stack/g3/adaptation")
    pLbpCoordSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpCoordSource.setType("SOURCE")
    pLbpCoordSource.setEnabled(False)
    pLbpCoordSource.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])

    global pLbpDevSource
    pLbpDevSource = g3ConfigComponent.createFileSymbol("LBP_DEV_SOURCE", None)
    pLbpDevSource.setSourcePath("g3/src/lbp/source/lbp_dev.c")
    pLbpDevSource.setOutputName("lbp_dev.c")
    pLbpDevSource.setDestPath("stack/g3/adaptation")
    pLbpDevSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpDevSource.setType("SOURCE")
    pLbpDevSource.setEnabled(True)
    pLbpDevSource.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])

    global pLbpVersionHeader
    pLbpVersionHeader = g3ConfigComponent.createFileSymbol("LBP_VERSION_HEADER", None)
    pLbpVersionHeader.setSourcePath("g3/src/lbp/source/lbp_version.h")
    pLbpVersionHeader.setOutputName("lbp_version.h")
    pLbpVersionHeader.setDestPath("stack/g3/adaptation")
    pLbpVersionHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpVersionHeader.setType("SOURCE")
    
    # MAC Wrapper Files
    pMacWrpSource = g3ConfigComponent.createFileSymbol("MAC_WRAPPER_SOURCE", None)
    pMacWrpSource.setSourcePath("g3/src/mac_wrapper/mac_wrapper.c.ftl")
    pMacWrpSource.setOutputName("mac_wrapper.c")
    pMacWrpSource.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpSource.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpSource.setType("SOURCE")
    pMacWrpSource.setMarkup(True)
    
    pMacWrpHeader = g3ConfigComponent.createFileSymbol("MAC_WRAPPER_HEADER", None)
    pMacWrpHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper.h")
    pMacWrpHeader.setOutputName("mac_wrapper.h")
    pMacWrpHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setType("HEADER")
    pMacWrpHeader.setMarkup(False)
    
    pMacWrpDefsHeader = g3ConfigComponent.createFileSymbol("MAC_WRAPPER_DEFS_HEADER", None)
    pMacWrpDefsHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper_defs.h")
    pMacWrpDefsHeader.setOutputName("mac_wrapper_defs.h")
    pMacWrpDefsHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setType("HEADER")
    
    # MAC Common Files
    pMacCommonSource = g3ConfigComponent.createFileSymbol("MAC_COMMON_SOURCE", None)
    pMacCommonSource.setSourcePath("g3/src/mac_common/mac_common.c.ftl")
    pMacCommonSource.setOutputName("mac_common.c")
    pMacCommonSource.setDestPath("stack/g3/mac/mac_common")
    pMacCommonSource.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonSource.setType("SOURCE")
    pMacCommonSource.setMarkup(True)
    
    pMacCommonHeader = g3ConfigComponent.createFileSymbol("MAC_COMMON_HEADER", None)
    pMacCommonHeader.setSourcePath("g3/src/mac_common/mac_common.h")
    pMacCommonHeader.setOutputName("mac_common.h")
    pMacCommonHeader.setDestPath("stack/g3/mac/mac_common")
    pMacCommonHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonHeader.setType("HEADER")
    
    pMacCommonDefsHeader = g3ConfigComponent.createFileSymbol("MAC_COMMON_DEFS_HEADER", None)
    pMacCommonDefsHeader.setSourcePath("g3/src/mac_common/mac_common_defs.h")
    pMacCommonDefsHeader.setOutputName("mac_common_defs.h")
    pMacCommonDefsHeader.setDestPath("stack/g3/mac/mac_common")
    pMacCommonDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_common")
    pMacCommonDefsHeader.setType("HEADER")

    # MAC PLC Files
    global macPlcLibFile
    macPlcLibFile = g3ConfigComponent.createLibrarySymbol("G3_MAC_PLC_LIBRARY", None)
    macPlcLibFile.setSourcePath("g3/libs/g3_lib_mac_plc.a")
    macPlcLibFile.setOutputName("g3_lib_plc_mac.a")
    macPlcLibFile.setDestPath("stack/g3/mac/mac_plc")
    macPlcLibFile.setEnabled(True)
    
    global macPlcHeader
    macPlcHeader = g3ConfigComponent.createFileSymbol("MAC_PLC_HEADER", None)
    macPlcHeader.setSourcePath("g3/src/mac_plc/mac_plc.h")
    macPlcHeader.setOutputName("mac_plc.h")
    macPlcHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcHeader.setType("HEADER")
    
    global macPlcDefsHeader
    macPlcDefsHeader = g3ConfigComponent.createFileSymbol("MAC_PLC_DEFS_HEADER", None)
    macPlcDefsHeader.setSourcePath("g3/src/mac_plc/mac_plc_defs.h")
    macPlcDefsHeader.setOutputName("mac_plc_defs.h")
    macPlcDefsHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcDefsHeader.setType("HEADER")
    
    global macPlcMibHeader
    macPlcMibHeader = g3ConfigComponent.createFileSymbol("MAC_PLC_MIB_HEADER", None)
    macPlcMibHeader.setSourcePath("g3/src/mac_plc/mac_plc_mib.h")
    macPlcMibHeader.setOutputName("mac_plc_mib.h")
    macPlcMibHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcMibHeader.setType("HEADER")
    
    # MAC RF Files
    global macRfLibFile
    macRfLibFile = g3ConfigComponent.createLibrarySymbol("G3_MAC_RF_LIBRARY", None)
    macRfLibFile.setSourcePath("g3/libs/g3_lib_mac_rf.a")
    macRfLibFile.setOutputName("g3_lib_rf_mac.a")
    macRfLibFile.setDestPath("stack/g3/mac/mac_rf")
    macRfLibFile.setEnabled(True)
    
    global macRfHeader
    macRfHeader = g3ConfigComponent.createFileSymbol("MAC_RF_HEADER", None)
    macRfHeader.setSourcePath("g3/src/mac_rf/mac_rf.h")
    macRfHeader.setOutputName("mac_rf.h")
    macRfHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfHeader.setType("HEADER")
    
    global macRfDefsHeader
    macRfDefsHeader = g3ConfigComponent.createFileSymbol("MAC_RF_DEFS_HEADER", None)
    macRfDefsHeader.setSourcePath("g3/src/mac_rf/mac_rf_defs.h")
    macRfDefsHeader.setOutputName("mac_rf_defs.h")
    macRfDefsHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfDefsHeader.setType("HEADER")
    
    global macRfMibHeader
    macRfMibHeader = g3ConfigComponent.createFileSymbol("MAC_RF_MIB_HEADER", None)
    macRfMibHeader.setSourcePath("g3/src/mac_rf/mac_rf_mib.h")
    macRfMibHeader.setOutputName("mac_rf_mib.h")
    macRfMibHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfMibHeader.setType("HEADER")
    
    #### Routing Wrapper Files #################################################
    routingTypesHeader = g3ConfigComponent.createFileSymbol("G3_ROUTING_TYPES_HEADER", None)
    routingTypesHeader.setSourcePath("g3/src/routing_wrapper/routing_types.h")
    routingTypesHeader.setOutputName("routing_types.h")
    routingTypesHeader.setDestPath("stack/g3/adaptation")
    routingTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingTypesHeader.setType("HEADER")
    
    routingWrapperHeader = g3ConfigComponent.createFileSymbol("G3_ROUTING_WRAPPER_HEADER", None)
    routingWrapperHeader.setSourcePath("g3/src/routing_wrapper/routing_wrapper.h")
    routingWrapperHeader.setOutputName("routing_wrapper.h")
    routingWrapperHeader.setDestPath("stack/g3/adaptation")
    routingWrapperHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingWrapperHeader.setType("HEADER")

    routingWrapperSource = g3ConfigComponent.createFileSymbol("G3_ROUTING_WRAPPER_SOURCE", None)
    routingWrapperSource.setSourcePath("g3/src/routing_wrapper/routing_wrapper.c.ftl")
    routingWrapperSource.setOutputName("routing_wrapper.c")
    routingWrapperSource.setDestPath("stack/g3/adaptation")
    routingWrapperSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingWrapperSource.setType("SOURCE")
    routingWrapperSource.setMarkup(True)

    #### ADP Serialization Files #################################################
    adpSerialHeader = g3ConfigComponent.createFileSymbol("G3_ADP_SERIAL_HEADER", None)
    adpSerialHeader.setSourcePath("g3/src/adp_serial/adp_serial.h")
    adpSerialHeader.setOutputName("adp_serial.h")
    adpSerialHeader.setDestPath("stack/g3/adaptation")
    adpSerialHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpSerialHeader.setType("HEADER")
    adpSerialHeader.setEnabled(False)
    adpSerialHeader.setDependencies(setEnabledFileSymbol, ["ADP_SERIALIZATION_EN"])

    adpSerialSource = g3ConfigComponent.createFileSymbol("G3_ADP_SERIAL_SOURCE", None)
    adpSerialSource.setSourcePath("g3/src/adp_serial/adp_serial.c")
    adpSerialSource.setOutputName("adp_serial.c")
    adpSerialSource.setDestPath("stack/g3/adaptation")
    adpSerialSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpSerialSource.setType("SOURCE")
    adpSerialSource.setEnabled(False)
    adpSerialSource.setDependencies(setEnabledFileSymbol, ["ADP_SERIALIZATION_EN"])

    # G3 STACK TEMPLATES 
    g3StackSystemConfigFile = g3ConfigComponent.createFileSymbol("G3_STACK_CONFIGURATION", None)
    g3StackSystemConfigFile.setType("STRING")
    g3StackSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    g3StackSystemConfigFile.setSourcePath("g3/templates/system/configuration.h.ftl")
    g3StackSystemConfigFile.setMarkup(True)

    g3StackSystemDefFile = g3ConfigComponent.createFileSymbol("G3_STACK_DEF", None)
    g3StackSystemDefFile.setType("STRING")
    g3StackSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    g3StackSystemDefFile.setSourcePath("g3/templates/system/definitions.h.ftl")
    g3StackSystemDefFile.setMarkup(True)

    g3StackSystemDefFile = g3ConfigComponent.createFileSymbol("G3_STACK_DEF_OBJ", None)
    g3StackSystemDefFile.setType("STRING")
    g3StackSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    g3StackSystemDefFile.setSourcePath("g3/templates/system/definitions_objects.h.ftl")
    g3StackSystemDefFile.setMarkup(True)

    plcSymSystemInitDataFile = g3ConfigComponent.createFileSymbol("G3_STACK_INIT_DATA", None)
    plcSymSystemInitDataFile.setType("STRING")
    plcSymSystemInitDataFile.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    plcSymSystemInitDataFile.setSourcePath("g3/templates/system/initialize_data.c.ftl")
    plcSymSystemInitDataFile.setMarkup(True)

    plcSystemInitFile = g3ConfigComponent.createFileSymbol("G3_STACK_INIT", None)
    plcSystemInitFile.setType("STRING")
    plcSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE")
    plcSystemInitFile.setSourcePath("g3/templates/system/initialize.c.ftl")
    plcSystemInitFile.setMarkup(True)

    plcSystemTasksFile = g3ConfigComponent.createFileSymbol("G3_STACK_SYS_TASK", None)
    plcSystemTasksFile.setType("STRING")
    plcSystemTasksFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    plcSystemTasksFile.setSourcePath("g3/templates/system/system_tasks.c.ftl")
    plcSystemTasksFile.setMarkup(True)

################################################################################
#### Business Logic ####
################################################################################
def selectLBPComponents(role):
    if role == "dev":
        pLbpCoordHeader.setEnabled(False)
        pLbpDevHeader.setEnabled(True)
        pLbpDefsHeader.setEnabled(True)
        pLbpEapSource.setEnabled(True)
        pLbpEapHeader.setEnabled(True)
        pLbpEncodeDecodeSource.setEnabled(True)
        pLbpEncodeDecodeHeader.setEnabled(True)
        pLbpCoordSource.setEnabled(False)
        pLbpDevSource.setEnabled(True)
        pLbpVersionHeader.setEnabled(True)
    elif role == "coord":
        pLbpCoordHeader.setEnabled(True)
        pLbpDevHeader.setEnabled(False)
        pLbpDefsHeader.setEnabled(True)
        pLbpEapSource.setEnabled(True)
        pLbpEapHeader.setEnabled(True)
        pLbpEncodeDecodeSource.setEnabled(True)
        pLbpEncodeDecodeHeader.setEnabled(True)
        pLbpCoordSource.setEnabled(True)
        pLbpDevSource.setEnabled(False)
        pLbpVersionHeader.setEnabled(True)
    elif role == "both":
        pLbpCoordHeader.setEnabled(True)
        pLbpDevHeader.setEnabled(True)
        pLbpDefsHeader.setEnabled(True)
        pLbpEapSource.setEnabled(True)
        pLbpEapHeader.setEnabled(True)
        pLbpEncodeDecodeSource.setEnabled(True)
        pLbpEncodeDecodeHeader.setEnabled(True)
        pLbpCoordSource.setEnabled(True)
        pLbpDevSource.setEnabled(True)
        pLbpVersionHeader.setEnabled(True)
    else: #None
        pLbpCoordHeader.setEnabled(False)
        pLbpDevHeader.setEnabled(False)
        pLbpDefsHeader.setEnabled(False)
        pLbpEapSource.setEnabled(False)
        pLbpEapHeader.setEnabled(False)
        pLbpEncodeDecodeSource.setEnabled(False)
        pLbpEncodeDecodeHeader.setEnabled(False)
        pLbpCoordSource.setEnabled(False)
        pLbpDevSource.setEnabled(False)
        pLbpVersionHeader.setEnabled(False)

def addADPComponents():
    Database.setSymbolValue("g3_config", "ADP_PRESENT", True)
    g3ConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (g3ConfigAdaptGroup == None):
        g3ConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")
    
    Database.activateComponents(["g3_adapt_config"], "ADAPTATION LAYER")
    component = Database.getComponentByID("g3_config")
    if (component.getSymbolValue("LOADNG_ENABLE") == True):
        Database.activateComponents(["g3ADP", "g3LOADng", "g3LBP"], "ADAPTATION LAYER")
    else:
        Database.activateComponents(["g3ADP", "g3LBP"], "ADAPTATION LAYER")
    
    if g3ConfigRole.getValue() == "PAN Device":
        selectLBPComponents("dev")
    elif g3ConfigRole.getValue() == "PAN Coordinator":
        selectLBPComponents("coord")
    elif g3ConfigRole.getValue() == "Dynamic (Selected upon Initialization)":
        selectLBPComponents("both")
    else:
        selectLBPComponents("dev")

def removeADPComponents():
    Database.setSymbolValue("g3_config", "ADP_PRESENT", False)
    Database.deactivateComponents(["g3_adapt_config", "g3ADP", "g3LOADng", "g3LBP"])
    selectLBPComponents("none")
    Database.disbandGroup("ADAPTATION LAYER")

def macPlcFilesEnabled(enable):
    macPlcLibFile.setEnabled(enable)
    macPlcHeader.setEnabled(enable)
    macPlcDefsHeader.setEnabled(enable)
    macPlcMibHeader.setEnabled(enable)

def macRfFilesEnabled(enable):
    macRfLibFile.setEnabled(enable)
    macRfHeader.setEnabled(enable)
    macRfDefsHeader.setEnabled(enable)
    macRfMibHeader.setEnabled(enable)

def removeMACComponents():
    Database.setSymbolValue("g3_config", "MAC_PLC_PRESENT", False)
    Database.setSymbolValue("g3_config", "MAC_RF_PRESENT", False)
    macPlcFilesEnabled(False)
    macRfFilesEnabled(False)

def addMACComponents(plc, rf):
    Database.setSymbolValue("g3_config", "MAC_PLC_PRESENT", plc)
    Database.setSymbolValue("g3_config", "MAC_RF_PRESENT", rf)
    
    if (plc and rf):
        macPlcFilesEnabled(True)
        macRfFilesEnabled(True)
    elif (plc):
        macPlcFilesEnabled(True)
        macRfFilesEnabled(False)
    elif (rf):
        macPlcFilesEnabled(False)
        macRfFilesEnabled(True)
    
    # In every case, add security component
    Database.activateComponents(["srvSecurity"], "G3 STACK")

def g3ConfigSelection(symbol, event):
    if event["value"] == "G3 Stack (ADP + MAC) Hybrid PLC & RF":
        # Complete Hybrid stack, enable components
        addADPComponents()
        addMACComponents(True, True)
        g3ConfigRole.setVisible(True)
        g3MacPLCTables.setVisible(True)
        g3MacRFTables.setVisible(True)
    elif event["value"] == "G3 Stack (ADP + MAC) PLC":
        # Complete PLC stack, enable components
        addADPComponents()
        addMACComponents(True, False)
        g3ConfigRole.setVisible(True)
        g3MacPLCTables.setVisible(True)
        g3MacRFTables.setVisible(False)
    elif event["value"] == "G3 Stack (ADP + MAC) RF":
        # Complete RF stack, enable components
        addADPComponents()
        addMACComponents(False, True)
        g3ConfigRole.setVisible(True)
        g3MacPLCTables.setVisible(False)
        g3MacRFTables.setVisible(True)
    elif event["value"] == "G3 MAC Layer Hybrid PLC & RF":
        # Hybrid MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, True)
        g3ConfigRole.setVisible(False)
        g3MacPLCTables.setVisible(True)
        g3MacRFTables.setVisible(True)
    elif event["value"] == "G3 PLC MAC Layer":
        # PLC MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, False)
        g3ConfigRole.setVisible(False)
        g3MacPLCTables.setVisible(True)
        g3MacRFTables.setVisible(False)
    elif event["value"] == "G3 RF MAC Layer":
        # RF MAC, disable and enable components
        removeADPComponents()
        addMACComponents(False, True)
        g3ConfigRole.setVisible(False)
        g3MacPLCTables.setVisible(False)
        g3MacRFTables.setVisible(True)
    else:
        # Remove all G3 components
        removeADPComponents()
        removeMACComponents()
        g3MacPLCTables.setVisible(False)
        g3MacRFTables.setVisible(False)
        g3ConfigRole.setVisible(False)

def g3ConfigRoleChange(symbol, event):
    if event["value"] == "PAN Device":
        # Device role selected
        Database.setSymbolValue("g3_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_config", "G3_COORDINATOR", False)
        selectLBPComponents("dev")
    elif event["value"] == "PAN Coordinator":
        # Coordinator role selected
        Database.setSymbolValue("g3_config", "G3_DEVICE", False)
        Database.setSymbolValue("g3_config", "G3_COORDINATOR", True)
        selectLBPComponents("coord")
    elif event["value"] == "Dynamic (Selected upon Initialization)":
        # Dynamic role selected
        Database.setSymbolValue("g3_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_config", "G3_COORDINATOR", True)
        selectLBPComponents("both")
    else:
        # Unknown option, behave as Device
        Database.setSymbolValue("g3_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_config", "G3_COORDINATOR", False)
        selectLBPComponents("dev")

def g3DebugChange(symbol, event):
    if event["value"] == True:
        # Debug traces enabled
        setVal("srvLogReport", "ENABLE_TRACES", True)
    else:
        # Debug traces disabled
        setVal("srvLogReport", "ENABLE_TRACES", False)

def g3ShowUsiInstance(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

def showSymbol(symbol, event):
    # Show/hide configuration symbol depending on parent enabled/disabled
    symbol.setVisible(event["value"])

def showUsiInstance(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

def setEnabledFileSymbol(symbol, event):
    # Enable/disable file symbol depending on parent enabled/disabled
    symbol.setEnabled(event["value"])

def g3LOADngEnable(symbol, event):
    #g3AdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (event["value"] == True):
        Database.activateComponents(["g3LOADng"], "ADAPTATION LAYER")
    else:
        Database.deactivateComponents(["g3LOADng"])

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
