# coding: utf-8
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
    g3StackGroup = Database.findGroup("G3 STACK")
    if (g3StackGroup == None):
        g3StackGroup = Database.createGroup(None, "G3 STACK")
    Database.setActiveGroup("G3 STACK")
    g3StackGroup.addComponent("g3_config")

    processor = Variables.get("__PROCESSOR")

    # Set dependencies inactive by default, they will be dinamically activated depending on config
    g3ConfigComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
    g3ConfigComponent.setDependencyEnabled("g3_srv_random_dependency", False)
    g3ConfigComponent.setDependencyEnabled("g3_srv_log_report_dependency", False)
    g3ConfigComponent.setDependencyEnabled("g3_srv_security_dependency", False)
    g3ConfigComponent.setDependencyEnabled("g3_palplc_dependency", False)
    g3ConfigComponent.setDependencyEnabled("g3_palrf_dependency", False)

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
    g3ConfigRole.setVisible(False)
    g3ConfigRole.setDescription("Select G3 Role")
    g3ConfigRole.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])
    g3ConfigRole.setDefaultValue("PAN Device")

    # G3 Task rate control
    g3TaskRate = g3ConfigComponent.createIntegerSymbol("G3_TASK_RATE_MS", None)
    g3TaskRate.setLabel("G3 stack task rate (ms)")
    g3TaskRate.setDescription("Number of milliseconds between G3 stack task processing")
    g3TaskRate.setVisible(getActiveRtos() == "BareMetal")
    g3TaskRate.setDefaultValue(5)
    g3TaskRate.setMin(1)
    g3TaskRate.setMax(20)
    g3TaskRate.setDependencies(showTaskRate, ["HarmonyCore.SELECT_RTOS"])

    # Debug Traces Enable
    global g3DebugEnable
    g3DebugEnable = g3ConfigComponent.createBooleanSymbol("G3_DEBUG_ENABLE", None)
    g3DebugEnable.setLabel("Enable G3 Stack Debug Traces")
    g3DebugEnable.setDescription("Enable/disable G3 Debug messages through SYS_DEBUG Service")
    g3DebugEnable.setDefaultValue(False)
    g3DebugEnable.setVisible(False)
    g3DebugEnable.setDependencies(g3DebugChange, ["G3_DEBUG_ENABLE"])

    # RTOS CONFIG
    g3StackRTOSMenu = g3ConfigComponent.createMenuSymbol("G3_RTOS_MENU", None)
    g3StackRTOSMenu.setLabel("RTOS settings")
    g3StackRTOSMenu.setDescription("RTOS settings")
    g3StackRTOSMenu.setVisible(getActiveRtos() != "BareMetal")
    g3StackRTOSMenu.setDependencies(showRTOSMenu, ["HarmonyCore.SELECT_RTOS"])

    g3StackRTOSStackSize = g3ConfigComponent.createIntegerSymbol("G3_RTOS_STACK_SIZE", g3StackRTOSMenu)
    g3StackRTOSStackSize.setLabel("Stack Size (in bytes)")
    g3StackRTOSStackSize.setDefaultValue(2048)
    g3StackRTOSStackSize.setMin(1024)
    g3StackRTOSStackSize.setMax(16*1024)

    g3StackRTOSTaskPriority = g3ConfigComponent.createIntegerSymbol("G3_RTOS_TASK_PRIORITY", g3StackRTOSMenu)
    g3StackRTOSTaskPriority.setLabel("Task Priority")
    g3StackRTOSTaskPriority.setDefaultValue(1)
    g3StackRTOSTaskPriority.setMin(0)

    g3StackRTOSUseDelay = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_USE_DELAY", g3StackRTOSMenu)
    g3StackRTOSUseDelay.setLabel("Use Task Delay")
    g3StackRTOSUseDelay.setDescription("Specifies whether task delay is used or not")
    g3StackRTOSUseDelay.setDefaultValue(True)
    g3StackRTOSUseDelay.setReadOnly(True)

    g3StackRTOSDelay = g3ConfigComponent.createIntegerSymbol("G3_RTOS_TASK_DELAY_MS", g3StackRTOSUseDelay)
    g3StackRTOSDelay.setLabel("Task Delay in ms")
    g3StackRTOSDelay.setDescription("Specifies the G3 stack task delay in ms")
    g3StackRTOSDelay.setDefaultValue(5)
    g3StackRTOSDelay.setMin(1)
    g3StackRTOSDelay.setMax(20)

    g3StackRTOSMsgQSize = g3ConfigComponent.createIntegerSymbol("G3_RTOS_TASK_MSG_QTY", g3StackRTOSMenu)
    g3StackRTOSMsgQSize.setLabel("Maximum Message Queue Size")
    g3StackRTOSMsgQSize.setDescription("A µC/OS-III task contains an optional internal message queue (if OS_CFG_TASK_Q_EN is set to DEF_ENABLED in os_cfg.h). This argument specifies the maximum number of messages that the task can receive through this message queue. The user may specify that the task is unable to receive messages by setting this argument to 0")
    g3StackRTOSMsgQSize.setDefaultValue(0)
    g3StackRTOSMsgQSize.setMin(0)
    g3StackRTOSMsgQSize.setVisible(getActiveRtos() == "MicriumOSIII")
    g3StackRTOSMsgQSize.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    g3StackRTOSTaskTimeQuanta = g3ConfigComponent.createIntegerSymbol("G3_RTOS_TASK_TIME_QUANTA", g3StackRTOSMenu)
    g3StackRTOSTaskTimeQuanta.setLabel("Task Time Quanta")
    g3StackRTOSTaskTimeQuanta.setDescription("The amount of time (in clock ticks) for the time quanta when Round Robin is enabled. If you specify 0, then the default time quanta will be used which is the tick rate divided by 10.")
    g3StackRTOSTaskTimeQuanta.setDefaultValue(0)
    g3StackRTOSTaskTimeQuanta.setMin(0)
    g3StackRTOSTaskTimeQuanta.setVisible(getActiveRtos() == "MicriumOSIII")
    g3StackRTOSTaskTimeQuanta.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    g3StackRTOSTaskSpecificOpt = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_OPT_NONE", g3StackRTOSMenu)
    g3StackRTOSTaskSpecificOpt.setLabel("Task Specific Options")
    g3StackRTOSTaskSpecificOpt.setDescription("Contains task-specific options. Each option consists of one bit. The option is selected when the bit is set. The current version of µC/OS-III supports the following options:")
    g3StackRTOSTaskSpecificOpt.setDefaultValue(True)
    g3StackRTOSTaskSpecificOpt.setVisible(getActiveRtos() == "MicriumOSIII")
    g3StackRTOSTaskSpecificOpt.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    g3StackRTOSTaskStkChk = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_OPT_STK_CHK", g3StackRTOSTaskSpecificOpt)
    g3StackRTOSTaskStkChk.setLabel("Stack checking is allowed for the task")
    g3StackRTOSTaskStkChk.setDescription("Specifies whether stack checking is allowed for the task")
    g3StackRTOSTaskStkChk.setDefaultValue(True)
    g3StackRTOSTaskStkChk.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["G3_RTOS_TASK_OPT_NONE"])

    g3StackRTOSTaskStkClr = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_OPT_STK_CLR", g3StackRTOSTaskSpecificOpt)
    g3StackRTOSTaskStkClr.setLabel("Stack needs to be cleared")
    g3StackRTOSTaskStkClr.setDescription("Specifies whether the stack needs to be cleared")
    g3StackRTOSTaskStkClr.setDefaultValue(True)
    g3StackRTOSTaskStkClr.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["G3_RTOS_TASK_OPT_NONE"])

    g3StackRTOSTaskSaveFp = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_OPT_SAVE_FP", g3StackRTOSTaskSpecificOpt)
    g3StackRTOSTaskSaveFp.setLabel("Floating-point registers needs to be saved")
    g3StackRTOSTaskSaveFp.setDescription("Specifies whether floating-point registers are saved. This option is only valid if the processor has floating-point hardware and the processor-specific code saves the floating-point registers")
    g3StackRTOSTaskSaveFp.setDefaultValue(False)
    g3StackRTOSTaskSaveFp.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["G3_RTOS_TASK_OPT_NONE"])

    g3StackRTOSTaskNoTls = g3ConfigComponent.createBooleanSymbol("G3_RTOS_TASK_OPT_NO_TLS", g3StackRTOSTaskSpecificOpt)
    g3StackRTOSTaskNoTls.setLabel("TLS (Thread Local Storage) support needed for the task")
    g3StackRTOSTaskNoTls.setDescription("If the caller doesn’t want or need TLS (Thread Local Storage) support for the task being created. If you do not include this option, TLS will be supported by default. TLS support was added in V3.03.00")
    g3StackRTOSTaskNoTls.setDefaultValue(False)
    g3StackRTOSTaskNoTls.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["G3_RTOS_TASK_OPT_NONE"])

    # ADP Configuration
    global g3AdpConfig
    g3AdpConfig = g3ConfigComponent.createMenuSymbol("ADP_CONFIG", None)
    g3AdpConfig.setLabel("ADP Configuration")
    g3AdpConfig.setDescription("ADP Buffers and Table Sizes")
    g3AdpConfig.setVisible(False)

    global g3SixLoWPANConfig
    g3SixLoWPANConfig = g3ConfigComponent.createMenuSymbol("6LOWPAN_CONFIG", g3AdpConfig)
    g3SixLoWPANConfig.setLabel("6LOWPAN Configuration")
    g3SixLoWPANConfig.setDescription("6LOWPAN Buffers and Table Sizes")
    g3SixLoWPANConfig.setVisible(False)

    global g3CountBuffers1280
    g3CountBuffers1280 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_1280", g3SixLoWPANConfig)
    g3CountBuffers1280.setLabel("Number of 1280-byte buffers")
    g3CountBuffers1280.setDescription("Number of 1280-byte buffers for adaptation layer")
    g3CountBuffers1280.setDefaultValue(1)
    g3CountBuffers1280.setMin(1)
    g3CountBuffers1280.setMax(16)

    global g3CountBuffers1280Comment
    g3CountBuffers1280Comment = g3ConfigComponent.createCommentSymbol("ADP_COUNT_BUFFERS_1280_COMMENT", g3CountBuffers1280)
    g3CountBuffers1280Comment.setVisible(True)
    totalMem = (1280 + 50) * g3CountBuffers1280.getValue()
    g3CountBuffers1280Comment.setLabel("Memory used: ~%d bytes" %totalMem)
    g3CountBuffers1280Comment.setDependencies(g3CountBuffers1280CommentDepend, ["ADP_COUNT_BUFFERS_1280"])

    global g3CountBuffers400
    g3CountBuffers400 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_400", g3SixLoWPANConfig)
    g3CountBuffers400.setLabel("Number of 400-byte buffers")
    g3CountBuffers400.setDescription("Number of 400-byte buffers for adaptation layer")
    g3CountBuffers400.setDefaultValue(3)
    g3CountBuffers400.setMin(1)
    g3CountBuffers400.setMax(32)

    global g3CountBuffers400Comment
    g3CountBuffers400Comment = g3ConfigComponent.createCommentSymbol("ADP_COUNT_BUFFERS_400_COMMENT", g3CountBuffers400)
    g3CountBuffers400Comment.setVisible(True)
    totalMem = (400 + 50) * g3CountBuffers400.getValue()
    g3CountBuffers400Comment.setLabel("Memory used: ~%d bytes" %totalMem)
    g3CountBuffers400Comment.setDependencies(g3CountBuffers400CommentDepend, ["ADP_COUNT_BUFFERS_400"])

    global g3CountBuffers100
    g3CountBuffers100 = g3ConfigComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_100", g3SixLoWPANConfig)
    g3CountBuffers100.setLabel("Number of 100-byte buffers")
    g3CountBuffers100.setDescription("Number of 100-byte buffers for adaptation layer")
    g3CountBuffers100.setDefaultValue(3)
    g3CountBuffers100.setMin(1)
    g3CountBuffers100.setMax(64)

    global g3CountBuffers100Comment
    g3CountBuffers100Comment = g3ConfigComponent.createCommentSymbol("ADP_COUNT_BUFFERS_100_COMMENT", g3CountBuffers100)
    g3CountBuffers100Comment.setVisible(True)
    totalMem = (100 + 50) * g3CountBuffers100.getValue()
    g3CountBuffers100Comment.setLabel("Memory used: ~%d bytes" %totalMem)
    g3CountBuffers100Comment.setDependencies(g3CountBuffers100CommentDepend, ["ADP_COUNT_BUFFERS_100"])

    g3FragmentSize = g3ConfigComponent.createIntegerSymbol("ADP_FRAGMENT_SIZE", g3SixLoWPANConfig)
    g3FragmentSize.setLabel("Size of ADP Fragments")
    g3FragmentSize.setDescription("The number of bytes of each ADP fragment. Depends on MAC layer capabilities.")
    if ("WBZ45" in processor) or ("PIC32CX1012BZ" in processor):
        g3FragmentSize.setDefaultValue(100)
    else:
        g3FragmentSize.setDefaultValue(400)
    g3FragmentSize.setMin(64)
    if ("WBZ45" in processor) or ("PIC32CX1012BZ" in processor):
        g3FragmentSize.setMax(100)
    else:
        g3FragmentSize.setMax(400)

    g3FragmentedTransferTableSize = g3ConfigComponent.createIntegerSymbol("ADP_FRAGMENTED_TRANSFER_TABLE_SIZE", g3SixLoWPANConfig)
    g3FragmentedTransferTableSize.setLabel("Fragmented transfer table size")
    g3FragmentedTransferTableSize.setDescription("The number of fragmented transfers which can be handled in parallel")
    g3FragmentedTransferTableSize.setDefaultValue(1)
    g3FragmentedTransferTableSize.setMin(1)
    g3FragmentedTransferTableSize.setMax(16)

    # LOADng Configuration
    global g3ConfigLOADng
    g3ConfigLOADng = g3ConfigComponent.createBooleanSymbol("LOADNG_ENABLE", g3AdpConfig)
    g3ConfigLOADng.setLabel("Enable LOADng Routing")
    g3ConfigLOADng.setVisible(False)
    g3ConfigLOADng.setDescription("Enable LOADng Routing Protocol")
    g3ConfigLOADng.setDependencies(g3LOADngEnable, ["LOADNG_ENABLE"])
    g3ConfigLOADng.setDefaultValue(True)

    adpRoutingTable = g3ConfigComponent.createIntegerSymbol("ADP_ROUTING_TABLE_SIZE", g3ConfigLOADng)
    adpRoutingTable.setLabel("Routing Table size")
    adpRoutingTable.setDescription("Number of entries in the Routing Table for LOADNG")
    adpRoutingTable.setDefaultValue(150)
    adpRoutingTable.setMin(1)
    adpRoutingTable.setMax(1024)
    adpRoutingTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpRoutingSet = g3ConfigComponent.createIntegerSymbol("ADP_ROUTING_SET_SIZE", g3ConfigLOADng)
    adpRoutingSet.setLabel("Routing Set size")
    adpRoutingSet.setDescription("Number of entries in the Routing Set for LOADNG")
    adpRoutingSet.setDefaultValue(30)
    adpRoutingSet.setMin(1)
    adpRoutingSet.setMax(256)
    adpRoutingSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpBlacklistTable = g3ConfigComponent.createIntegerSymbol("ADP_BLACKLIST_TABLE_SIZE", g3ConfigLOADng)
    adpBlacklistTable.setLabel("Blacklist Table size")
    adpBlacklistTable.setDescription("Number of entries in the Blacklist Table for LOADNG")
    adpBlacklistTable.setDefaultValue(20)
    adpBlacklistTable.setMin(1)
    adpBlacklistTable.setMax(256)
    adpBlacklistTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpDestinationAddressSet = g3ConfigComponent.createIntegerSymbol("ADP_DESTINATION_ADDRESS_SET_SIZE", g3ConfigLOADng)
    adpDestinationAddressSet.setLabel("Destination Address Set size")
    adpDestinationAddressSet.setDescription("Number of entries in the Destination Address Set Table")
    adpDestinationAddressSet.setDefaultValue(1)
    adpDestinationAddressSet.setMin(1)
    adpDestinationAddressSet.setMax(128)
    adpDestinationAddressSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    global g3LOADngAdvancedSettings
    g3LOADngAdvancedSettings = g3ConfigComponent.createBooleanSymbol("LOADNG_ADVANCED_SETTINGS", g3ConfigLOADng)
    g3LOADngAdvancedSettings.setLabel("LOADng Advanced Settings")
    g3LOADngAdvancedSettings.setDescription("Enable LOADng Advanced Configuration")
    g3LOADngAdvancedSettings.setDependencies(showSymbol, ["LOADNG_ENABLE"])
    g3LOADngAdvancedSettings.setDefaultValue(False)

    loadngPendingRReqTable = g3ConfigComponent.createIntegerSymbol("LOADNG_PENDING_RREQ_TABLE_SIZE", g3LOADngAdvancedSettings)
    loadngPendingRReqTable.setLabel("Pending RREQ table size")
    loadngPendingRReqTable.setDescription("Number of RREQs/RERRs that can be stored to respect the parameter ADP_IB_RREQ_WAIT")
    loadngPendingRReqTable.setDefaultValue(6)
    loadngPendingRReqTable.setMin(1)
    loadngPendingRReqTable.setMax(10)
    loadngPendingRReqTable.setVisible(False)
    loadngPendingRReqTable.setDependencies(showSymbol, ["LOADNG_ADVANCED_SETTINGS"])

    loadngRRepGenerationTable = g3ConfigComponent.createIntegerSymbol("LOADNG_RREP_GENERATION_TABLE_SIZE", g3LOADngAdvancedSettings)
    loadngRRepGenerationTable.setLabel("RREP generation table size")
    loadngRRepGenerationTable.setDescription("Number of RREQs from different sources, that can be handled at the same time")
    loadngRRepGenerationTable.setDefaultValue(3)
    loadngRRepGenerationTable.setMin(1)
    loadngRRepGenerationTable.setMax(10)
    loadngRRepGenerationTable.setVisible(False)
    loadngRRepGenerationTable.setDependencies(showSymbol, ["LOADNG_ADVANCED_SETTINGS"])

    loadngRReqForwardingTable = g3ConfigComponent.createIntegerSymbol("LOADNG_RREQ_FORWARDING_TABLE_SIZE", g3LOADngAdvancedSettings)
    loadngRReqForwardingTable.setLabel("RREP forwarding table size")
    loadngRReqForwardingTable.setDescription("Number of entries of RREP forwarding table for LOADNG")
    loadngRReqForwardingTable.setDefaultValue(5)
    loadngRReqForwardingTable.setMin(1)
    loadngRReqForwardingTable.setMax(10)
    loadngRReqForwardingTable.setVisible(False)
    loadngRReqForwardingTable.setDependencies(showSymbol, ["LOADNG_ADVANCED_SETTINGS"])

    loadngDiscoverRouteTable = g3ConfigComponent.createIntegerSymbol("LOADNG_DISCOVER_ROUTE_TABLE_SIZE", g3LOADngAdvancedSettings)
    loadngDiscoverRouteTable.setLabel("Discover route table size")
    loadngDiscoverRouteTable.setDescription("Number of route discover that can be handled at the same time")
    loadngDiscoverRouteTable.setDefaultValue(3)
    loadngDiscoverRouteTable.setMin(1)
    loadngDiscoverRouteTable.setMax(10)
    loadngDiscoverRouteTable.setVisible(False)
    loadngDiscoverRouteTable.setDependencies(showSymbol, ["LOADNG_ADVANCED_SETTINGS"])

    # Coordinator LBP Configuration
    g3LbpCoordConfig = g3ConfigComponent.createMenuSymbol("LBP_COORD_CONFIG", g3AdpConfig)
    g3LbpCoordConfig.setLabel("Coordinator LBP Configuration")
    g3LbpCoordConfig.setDescription("Configuration on Coordinator LBP module")
    g3LbpCoordConfig.setVisible(g3ConfigRole.getValue() != "PAN Device")
    g3LbpCoordConfig.setDependencies(showLbpCoordConfig, ["G3_ROLE"])

    g3CoordLbpSlots = g3ConfigComponent.createIntegerSymbol("COORD_LBP_SLOTS", g3LbpCoordConfig)
    g3CoordLbpSlots.setLabel("Number of LBP Slots")
    g3CoordLbpSlots.setDescription("The number of LBP processes that can be handled in parallel from Coordinator")
    g3CoordLbpSlots.setVisible(g3ConfigRole.getValue() != "PAN Device")
    g3CoordLbpSlots.setDefaultValue(5)
    g3CoordLbpSlots.setMin(1)
    g3CoordLbpSlots.setMax(20)
    g3CoordLbpSlots.setDependencies(showLbpCoordConfig, ["G3_ROLE"])

    g3CoordLbpRetries = g3ConfigComponent.createIntegerSymbol("COORD_LBP_RETRIES", g3LbpCoordConfig)
    g3CoordLbpRetries.setLabel("Number Retries for LBP frames")
    g3CoordLbpRetries.setDescription("The number of times an LBP frame is retried when no response is received")
    g3CoordLbpRetries.setVisible(g3ConfigRole.getValue() != "PAN Device")
    g3CoordLbpRetries.setDefaultValue(1)
    g3CoordLbpRetries.setMin(1)
    g3CoordLbpRetries.setMax(5)
    g3CoordLbpRetries.setDependencies(showLbpCoordConfig, ["G3_ROLE"])

    g3CoordInitialKeyIndex = g3ConfigComponent.createIntegerSymbol("COORD_INIT_KEY_INDEX", g3LbpCoordConfig)
    g3CoordInitialKeyIndex.setLabel("Initial Security Key Index")
    g3CoordInitialKeyIndex.setDescription("Security Index to set as active on LBP frames sent to Devices")
    g3CoordInitialKeyIndex.setVisible(g3ConfigRole.getValue() != "PAN Device")
    g3CoordInitialKeyIndex.setDefaultValue(0)
    g3CoordInitialKeyIndex.setMin(0)
    g3CoordInitialKeyIndex.setMax(1)
    g3CoordInitialKeyIndex.setDependencies(showLbpCoordConfig, ["G3_ROLE"])

    # ADP Serialization
    g3AdpSerializationEnable = g3ConfigComponent.createBooleanSymbol("ADP_SERIALIZATION_EN", g3AdpConfig)
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

    # MAC Configuration
    global g3MacConfig
    g3MacConfig = g3ConfigComponent.createMenuSymbol("MAC_CONFIG", None)
    g3MacConfig.setLabel("MAC Configuration")
    g3MacConfig.setDescription("MAC Buffers and Table Sizes")
    g3MacConfig.setVisible(False)

    # MAC PLC Table Sizes
    global g3MacPLCTables
    g3MacPLCTables = g3ConfigComponent.createMenuSymbol("MAC_PLC_TABLES", g3MacConfig)
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
    g3MacRFTables = g3ConfigComponent.createMenuSymbol("MAC_RF_TABLES", g3MacConfig)
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

    global g3MacSerializationEnable
    g3MacSerializationEnable = g3ConfigComponent.createBooleanSymbol("MAC_SERIALIZATION_EN", g3MacConfig)
    g3MacSerializationEnable.setLabel("Enable MAC Serialization")
    g3MacSerializationEnable.setDescription("Enable/disable MAC serialization through USI")
    g3MacSerializationEnable.setVisible(False)
    g3MacSerializationEnable.setDefaultValue(False)

    g3MacSerializationUsiInstance = g3ConfigComponent.createIntegerSymbol("MAC_SERIALIZATION_USI_INSTANCE", g3MacSerializationEnable)
    g3MacSerializationUsiInstance.setLabel("USI Instance")
    g3MacSerializationUsiInstance.setDescription("USI instance used for MAC serialization")
    g3MacSerializationUsiInstance.setDefaultValue(0)
    g3MacSerializationUsiInstance.setMax(0)
    g3MacSerializationUsiInstance.setMin(0)
    g3MacSerializationUsiInstance.setVisible(False)
    g3MacSerializationUsiInstance.setDependencies(showUsiInstance, ["MAC_SERIALIZATION_EN"])

    # Boolean symbols to use in FTLs to generate code
    global g3DeviceRole
    g3DeviceRole = g3ConfigComponent.createBooleanSymbol("G3_DEVICE", None)
    g3DeviceRole.setVisible(False)
    g3DeviceRole.setDefaultValue(True)
    global g3CoordRole
    g3CoordRole = g3ConfigComponent.createBooleanSymbol("G3_COORDINATOR", None)
    g3CoordRole.setVisible(False)
    g3CoordRole.setDefaultValue(False)

    global g3MacPLCPresent
    g3MacPLCPresent = g3ConfigComponent.createBooleanSymbol("MAC_PLC_PRESENT", None)
    g3MacPLCPresent.setVisible(False)
    g3MacPLCPresent.setDefaultValue(True)
    global g3MacRFPresent
    g3MacRFPresent = g3ConfigComponent.createBooleanSymbol("MAC_RF_PRESENT", None)
    g3MacRFPresent.setVisible(False)
    g3MacRFPresent.setDefaultValue(True)

    global g3ADPPresent
    g3ADPPresent = g3ConfigComponent.createBooleanSymbol("ADP_PRESENT", None)
    g3ADPPresent.setVisible(False)
    g3ADPPresent.setDefaultValue(False)

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
    pLbpCoordSource.setSourcePath("g3/src/lbp/source/lbp_coord.c.ftl")
    pLbpCoordSource.setOutputName("lbp_coord.c")
    pLbpCoordSource.setDestPath("stack/g3/adaptation")
    pLbpCoordSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpCoordSource.setType("SOURCE")
    pLbpCoordSource.setEnabled(False)
    pLbpCoordSource.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])
    pLbpCoordSource.setMarkup(True)

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
    pMacWrpHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper.h.ftl")
    pMacWrpHeader.setOutputName("mac_wrapper.h")
    pMacWrpHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpHeader.setType("HEADER")
    pMacWrpHeader.setMarkup(True)

    pMacWrpDefsHeader = g3ConfigComponent.createFileSymbol("MAC_WRAPPER_DEFS_HEADER", None)
    pMacWrpDefsHeader.setSourcePath("g3/src/mac_wrapper/mac_wrapper_defs.h.ftl")
    pMacWrpDefsHeader.setOutputName("mac_wrapper_defs.h")
    pMacWrpDefsHeader.setDestPath("stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_wrapper")
    pMacWrpDefsHeader.setType("HEADER")
    pMacWrpDefsHeader.setMarkup(True)

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
    macPlcMibHeader.setSourcePath("g3/src/mac_plc/mac_plc_mib.h.ftl")
    macPlcMibHeader.setOutputName("mac_plc_mib.h")
    macPlcMibHeader.setDestPath("stack/g3/mac/mac_plc")
    macPlcMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_plc")
    macPlcMibHeader.setType("HEADER")
    macPlcMibHeader.setMarkup(True)

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
    macRfMibHeader.setSourcePath("g3/src/mac_rf/mac_rf_mib.h.ftl")
    macRfMibHeader.setOutputName("mac_rf_mib.h")
    macRfMibHeader.setDestPath("stack/g3/mac/mac_rf")
    macRfMibHeader.setProjectPath("config/" + configName + "/stack/g3/mac/mac_rf")
    macRfMibHeader.setType("HEADER")
    macRfMibHeader.setMarkup(True)

    #### ADP Library Files ######################################################
    global adpLibFile
    adpLibFile = g3ConfigComponent.createLibrarySymbol("G3_ADP_LIBRARY", None)
    adpLibFile.setSourcePath("g3/libs/g3_lib_adp.a")
    adpLibFile.setOutputName("g3_lib_adp.a")
    adpLibFile.setDestPath("stack/g3/adaptation")

    #### ADP header Files ######################################################
    global adpHeader
    adpHeader = g3ConfigComponent.createFileSymbol("G3_ADP_HEADER", None)
    adpHeader.setSourcePath("g3/src/adp/adp.h")
    adpHeader.setOutputName("adp.h")
    adpHeader.setDestPath("stack/g3/adaptation")
    adpHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpHeader.setType("HEADER")

    global adpApiTypesHeader
    adpApiTypesHeader = g3ConfigComponent.createFileSymbol("G3_ADP_API_TYPES_HEADER", None)
    adpApiTypesHeader.setSourcePath("g3/src/adp/adp_api_types.h")
    adpApiTypesHeader.setOutputName("adp_api_types.h")
    adpApiTypesHeader.setDestPath("stack/g3/adaptation")
    adpApiTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpApiTypesHeader.setType("HEADER")

    global adpSharedTypesHeader
    adpSharedTypesHeader = g3ConfigComponent.createFileSymbol("G3_ADP_SHARED_TYPES_HEADER", None)
    adpSharedTypesHeader.setSourcePath("g3/src/adp/adp_shared_types.h")
    adpSharedTypesHeader.setOutputName("adp_shared_types.h")
    adpSharedTypesHeader.setDestPath("stack/g3/adaptation")
    adpSharedTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpSharedTypesHeader.setType("HEADER")

    #### LOADNG Library Files ######################################################
    global LOADngLibFile
    LOADngLibFile = g3ConfigComponent.createLibrarySymbol("G3_LOADNG_LIBRARY", None)
    LOADngLibFile.setSourcePath("g3/libs/g3_lib_loadng.a")
    LOADngLibFile.setOutputName("g3_lib_loadng.a")
    LOADngLibFile.setDestPath("stack/g3/adaptation")

    #### LOADNG header Files ###################################################
    global LOADngHeader
    LOADngHeader = g3ConfigComponent.createFileSymbol("G3_LOADNG_HEADER", None)
    LOADngHeader.setSourcePath("g3/src/loadng/loadng.h")
    LOADngHeader.setOutputName("loadng.h")
    LOADngHeader.setDestPath("stack/g3/adaptation")
    LOADngHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    LOADngHeader.setType("HEADER")

    #### Routing Wrapper Files #################################################
    global routingTypesHeader
    routingTypesHeader = g3ConfigComponent.createFileSymbol("G3_ROUTING_TYPES_HEADER", None)
    routingTypesHeader.setSourcePath("g3/src/routing_wrapper/routing_types.h")
    routingTypesHeader.setOutputName("routing_types.h")
    routingTypesHeader.setDestPath("stack/g3/adaptation")
    routingTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingTypesHeader.setType("HEADER")

    global routingWrapperHeader
    routingWrapperHeader = g3ConfigComponent.createFileSymbol("G3_ROUTING_WRAPPER_HEADER", None)
    routingWrapperHeader.setSourcePath("g3/src/routing_wrapper/routing_wrapper.h")
    routingWrapperHeader.setOutputName("routing_wrapper.h")
    routingWrapperHeader.setDestPath("stack/g3/adaptation")
    routingWrapperHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingWrapperHeader.setType("HEADER")

    global routingWrapperSource
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

    g3StackSystemRtosTasksFile = g3ConfigComponent.createFileSymbol("G3_STACK_SYS_RTOS_TASK", None)
    g3StackSystemRtosTasksFile.setType("STRING")
    g3StackSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    g3StackSystemRtosTasksFile.setSourcePath("g3/templates/system/system_rtos_tasks.c.ftl")
    g3StackSystemRtosTasksFile.setMarkup(True)
    g3StackSystemRtosTasksFile.setEnabled(getActiveRtos() != "BareMetal")
    g3StackSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])

def destroyComponent(g3ConfigComponent):
    # Deactivate service and PAL components
    Database.deactivateComponents(["g3PalPlc", "g3PalRf", "srvSecurity", "srvRandom", "srvLogReport", "srvQueue"])

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
    g3ADPPresent.setValue(True)
    adpLibFile.setEnabled(True)
    adpHeader.setEnabled(True)
    adpApiTypesHeader.setEnabled(True)
    adpSharedTypesHeader.setEnabled(True)
    routingTypesHeader.setEnabled(True)
    routingWrapperHeader.setEnabled(True)
    routingWrapperSource.setEnabled(True)
    if g3ConfigLOADng.getValue() == True:
        LOADngLibFile.setEnabled(True)
        LOADngHeader.setEnabled(True)
    else:
        LOADngLibFile.setEnabled(False)
        LOADngHeader.setEnabled(False)
    g3ConfigRole.setVisible(True)
    g3AdpConfig.setVisible(True)
    g3ConfigLOADng.setVisible(True)
    g3SixLoWPANConfig.setVisible(True)

    if g3ConfigRole.getValue() == "PAN Device":
        selectLBPComponents("dev")
    elif g3ConfigRole.getValue() == "PAN Coordinator":
        selectLBPComponents("coord")
    elif g3ConfigRole.getValue() == "Dynamic (Selected upon Initialization)":
        selectLBPComponents("both")
    else:
        selectLBPComponents("dev")

def removeADPComponents():
    g3ADPPresent.setValue(False)
    adpLibFile.setEnabled(False)
    adpHeader.setEnabled(False)
    adpApiTypesHeader.setEnabled(False)
    adpSharedTypesHeader.setEnabled(False)
    routingTypesHeader.setEnabled(False)
    routingWrapperHeader.setEnabled(False)
    routingWrapperSource.setEnabled(False)
    LOADngLibFile.setEnabled(False)
    LOADngHeader.setEnabled(False)
    g3ConfigRole.setVisible(False)
    g3ConfigLOADng.setVisible(False)
    g3SixLoWPANConfig.setVisible(False)
    g3AdpConfig.setVisible(False)
    selectLBPComponents("none")

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
    g3MacPLCPresent.setValue(False)
    g3MacRFPresent.setValue(False)
    macPlcFilesEnabled(False)
    macRfFilesEnabled(False)
    g3MacRFTables.setVisible(False)
    g3MacPLCTables.setVisible(False)
    g3ConfigRole.setVisible(False)
    g3MacSerializationEnable.setVisible(False)
    g3DebugEnable.setVisible(False)
    g3MacConfig.setVisible(False)
    # Deactivate service and PAL components
    Database.deactivateComponents(["g3PalPlc", "g3PalRf", "srvSecurity", "srvRandom", "srvLogReport", "srvQueue"])

def addMACComponents(plc, rf):
    g3MacPLCPresent.setValue(plc)
    g3MacRFPresent.setValue(rf)
    g3MacConfig.setVisible(True)
    g3MacPLCTables.setVisible(plc)
    g3MacRFTables.setVisible(rf)
    g3MacSerializationEnable.setVisible(True)
    g3DebugEnable.setVisible(True)
    macPlcFilesEnabled(plc)
    macRfFilesEnabled(rf)

    # In every case, add security, random and logReport components
    g3StackGroup = Database.findGroup("G3 STACK")
    g3StackGroup.addComponent("srvSecurity")
    g3StackGroup.addComponent("srvRandom")
    g3StackGroup.addComponent("srvLogReport")
    Database.activateComponents(["srvSecurity", "srvRandom", "srvLogReport"], "G3 STACK")

    # Add PAL components depending on MAC availability
    if (plc and rf):
        g3StackGroup.addComponent("g3PalPlc")
        g3StackGroup.addComponent("g3PalRf")
        Database.activateComponents(["g3PalPlc", "g3PalRf"], "G3 STACK")
    elif (plc):
        g3StackGroup.addComponent("g3PalPlc")
        Database.activateComponents(["g3PalPlc"], "G3 STACK")
        Database.deactivateComponents(["g3PalRf"])
    elif (rf):
        g3StackGroup.addComponent("g3PalRf")
        Database.activateComponents(["g3PalRf"], "G3 STACK")
        Database.deactivateComponents(["g3PalPlc"])

def g3ConfigSelection(symbol, event):
    symbolComponent = symbol.getComponent()
    g3StackGroup = Database.findGroup("G3 STACK")
    if event["value"] == "G3 Stack (ADP + MAC) Hybrid PLC & RF":
        # Complete Hybrid stack, enable components
        addADPComponents()
        addMACComponents(True, True)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", True)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        if g3ConfigLOADng.getValue() == True:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", True)
            g3StackGroup.addComponent("srvQueue")
            Database.activateComponents(["srvQueue"], "G3 STACK")
        else:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
            Database.deactivateComponents(["srvQueue"])
    elif event["value"] == "G3 Stack (ADP + MAC) PLC":
        # Complete PLC stack, enable components
        addADPComponents()
        addMACComponents(True, False)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", True)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        if g3ConfigLOADng.getValue() == True:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", True)
            g3StackGroup.addComponent("srvQueue")
            Database.activateComponents(["srvQueue"], "G3 STACK")
        else:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
            Database.deactivateComponents(["srvQueue"])
    elif event["value"] == "G3 Stack (ADP + MAC) RF":
        # Complete RF stack, enable components
        addADPComponents()
        addMACComponents(False, True)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", False)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        if g3ConfigLOADng.getValue() == True:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", True)
            g3StackGroup.addComponent("srvQueue")
            Database.activateComponents(["srvQueue"], "G3 STACK")
        else:
            symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
            Database.deactivateComponents(["srvQueue"])
    elif event["value"] == "G3 MAC Layer Hybrid PLC & RF":
        # Hybrid MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, True)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", True)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
        Database.deactivateComponents(["srvQueue"])
    elif event["value"] == "G3 PLC MAC Layer":
        # PLC MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, False)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", True)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
        Database.deactivateComponents(["srvQueue"])
    elif event["value"] == "G3 RF MAC Layer":
        # RF MAC, disable and enable components
        removeADPComponents()
        addMACComponents(False, True)
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", False)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", True)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
        Database.deactivateComponents(["srvQueue"])
    else:
        # Remove all G3 components
        removeADPComponents()
        removeMACComponents()
        symbolComponent.setDependencyEnabled("g3_palplc_dependency", False)
        symbolComponent.setDependencyEnabled("g3_palrf_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_random_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_log_report_dependency", False)
        symbolComponent.setDependencyEnabled("g3_srv_security_dependency", False)

def g3ConfigRoleChange(symbol, event):
    if event["value"] == "PAN Device":
        # Device role selected
        g3DeviceRole.setValue(True)
        g3CoordRole.setValue(False)
        selectLBPComponents("dev")
    elif event["value"] == "PAN Coordinator":
        # Coordinator role selected
        g3DeviceRole.setValue(False)
        g3CoordRole.setValue(True)
        selectLBPComponents("coord")
    elif event["value"] == "Dynamic (Selected upon Initialization)":
        # Dynamic role selected
        g3DeviceRole.setValue(True)
        g3CoordRole.setValue(True)
        selectLBPComponents("both")
    else:
        # Unknown option, behave as Device
        g3DeviceRole.setValue(True)
        g3CoordRole.setValue(False)
        selectLBPComponents("dev")

def g3LOADngEnable(symbol, event):
    symbolComponent = symbol.getComponent()
    g3StackGroup = Database.findGroup("G3 STACK")
    if (event["value"] == True):
        LOADngLibFile.setEnabled(True)
        LOADngHeader.setEnabled(True)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", True)
        g3StackGroup.addComponent("srvQueue")
        Database.activateComponents(["srvQueue"], "G3 STACK")
    else:
        LOADngLibFile.setEnabled(False)
        LOADngHeader.setEnabled(False)
        symbolComponent.setDependencyEnabled("g3_srv_queue_dependency", False)
        Database.deactivateComponents(["srvQueue"])

def g3DebugChange(symbol, event):
    if event["value"] == True:
        # Debug traces enabled
        setVal("srvLogReport", "ENABLE_TRACES", True)
    else:
        # Debug traces disabled
        setVal("srvLogReport", "ENABLE_TRACES", False)

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

def g3CountBuffers1280CommentDepend(symbol, event):
    totalMem = (1280 + 50) * g3CountBuffers1280.getValue()
    g3CountBuffers1280Comment.setLabel("Memory used: ~%d bytes" %totalMem)

def g3CountBuffers400CommentDepend(symbol, event):
    totalMem = (400 + 50) * g3CountBuffers400.getValue()
    g3CountBuffers400Comment.setLabel("Memory used: ~%d bytes" %totalMem)

def g3CountBuffers100CommentDepend(symbol, event):
    totalMem = (100 + 50) * g3CountBuffers100.getValue()
    g3CountBuffers100Comment.setLabel("Memory used: ~%d bytes" %totalMem)

def showLbpCoordConfig(symbol, event):
    symbol.setVisible(event["value"] != "PAN Device")

def showTaskRate(symbol, event):
    symbol.setVisible(event["value"] == "BareMetal")

def showRTOSMenu(symbol, event):
    symbol.setVisible(event["value"] != "BareMetal")

def genRtosTask(symbol, event):
    symbol.setEnabled(event["value"] != "BareMetal")

def commandRtosMicriumOSIIIAppTaskVisibility(symbol, event):
    symbol.setVisible(event["value"] == "MicriumOSIII")

def commandRtosMicriumOSIIITaskOptVisibility(symbol, event):
    symbol.setVisible(event["value"])

def getActiveRtos():
    return Database.getSymbolValue("HarmonyCore", "SELECT_RTOS")

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
