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
g3_pal_rf_helpkeyword = "mcc_g3_pal_rf_wbz451_config"

def showSymbol(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

def activatesDependencies(symbol, event):
    # if (event["id"] == "G3_PAL_PLC_PVDD_MONITOR"):
    #     if (event["value"] == True):
    #         if(Database.getComponentByID("srv_pvddmon") == None):
    #             Database.activateComponents(["srv_pvddmon"])
    #     else:
    #         Database.deactivateComponents(["srv_pvddmon"])

    if (event["id"] == "G3_PAL_RF_PHY_SNIFFER_EN"):
        if (event["value"] == True):
            if(Database.getComponentByID("srv_usi") == None):
                Database.activateComponents(["srv_usi"])
            if(Database.getComponentByID("srv_rsniffer") == None):
                Database.activateComponents(["srv_rsniffer"], "G3 STACK")

def showRTOSMenu(symbol, event):
    show_rtos_menu = False

    if (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
        show_rtos_menu = True

    symbol.setVisible(show_rtos_menu)

def enableRTOS(symbol, event):
    enable_rtos = False

    if (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
        enable_rtos = True

    symbol.setValue(enable_rtos)

def commandRtosMicriumOSIIIAppTaskVisibility(symbol, event):
    if (event["value"] == "MicriumOSIII"):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def commandRtosMicriumOSIIITaskOptVisibility(symbol, event):
    symbol.setVisible(event["value"])

def getActiveRtos():
    activeComponents = Database.getActiveComponentIDs()

    for i in range(0, len(activeComponents)):
        if (activeComponents[i] == "FreeRTOS"):
            return "FreeRTOS"
        elif (activeComponents[i] == "ThreadX"):
            return "ThreadX"
        elif (activeComponents[i] == "MicriumOSIII"):
            return "MicriumOSIII"
        elif (activeComponents[i] == "MbedOS"):
            return "MbedOS"                      

def instantiateComponent(g3PalRfComponent):
    
    Log.writeInfoMessage("Loading G3 PAL RF module")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    g3PalRfPHYChannel = g3PalRfComponent.createIntegerSymbol("G3_PAL_RF_PHY_CHANNEL", None)
    g3PalRfPHYChannel.setLabel("RF PHY Channel")
    g3PalRfPHYChannel.setDefaultValue(11)
    g3PalRfPHYChannel.setMin(11)
    g3PalRfPHYChannel.setMax(26)
    g3PalRfPHYChannel.setHelp(g3_pal_rf_helpkeyword)

    g3PalRfPHYPage = g3PalRfComponent.createComboSymbol("G3_PAL_RF_PHY_PAGE", None, ["0", "2", "16", "17"])
    g3PalRfPHYPage.setLabel("RF PHY Page")
    g3PalRfPHYPage.setDefaultValue("0")
    g3PalRfPHYPage.setHelp(g3_pal_rf_helpkeyword)

    g3PalRfPHYPhySnifferEnable = g3PalRfComponent.createBooleanSymbol("G3_PAL_RF_PHY_SNIFFER_EN", None)
    g3PalRfPHYPhySnifferEnable.setLabel("Enable G3 PHY sniffer")
    g3PalRfPHYPhySnifferEnable.setDefaultValue(False)
    g3PalRfPHYPhySnifferEnable.setHelp(g3_pal_rf_helpkeyword)

    g3PalRfPHYUSIInstance = g3PalRfComponent.createIntegerSymbol("G3_PAL_RF_USI_INSTANCE", g3PalRfPHYPhySnifferEnable)
    g3PalRfPHYUSIInstance.setLabel("USI Instance")
    g3PalRfPHYUSIInstance.setDefaultValue(0)
    g3PalRfPHYUSIInstance.setMax(0)
    g3PalRfPHYUSIInstance.setMin(0)
    g3PalRfPHYUSIInstance.setVisible(False)
    g3PalRfPHYUSIInstance.setHelp(g3_pal_rf_helpkeyword)
    g3PalRfPHYUSIInstance.setDependencies(showSymbol, ["G3_PAL_RF_PHY_SNIFFER_EN"])

    g3PalRfPHYDummySymbol = g3PalRfComponent.createBooleanSymbol("G3_PAL_RF_DUMMY", None)
    g3PalRfPHYDummySymbol.setLabel("Dummy")
    g3PalRfPHYDummySymbol.setDefaultValue(False)
    g3PalRfPHYDummySymbol.setVisible(False)
    g3PalRfPHYDummySymbol.setDependencies(activatesDependencies, ["G3_PAL_RF_PHY_SNIFFER_EN"])

    #####################################################################################################################################
    # RTOS CONFIG
    palRFRTOSSupport = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_ENABLE", None)
    palRFRTOSSupport.setLabel("RTOS support")
    palRFRTOSSupport.setDefaultValue(0)
    palRFRTOSSupport.setVisible(False)
    palRFRTOSSupport.setDependencies(enableRTOS, ["HarmonyCore.SELECT_RTOS"])

    palRFRTOSMenu = g3PalRfComponent.createMenuSymbol("PAL_RF_RTOS_MENU", None)
    palRFRTOSMenu.setLabel("RTOS settings")
    palRFRTOSMenu.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSMenu.setDescription("RTOS settings")
    palRFRTOSMenu.setVisible(False)
    palRFRTOSMenu.setDependencies(showRTOSMenu, ["HarmonyCore.SELECT_RTOS"])

    palRFRTOSStackSize = g3PalRfComponent.createIntegerSymbol("PAL_RF_RTOS_STACK_SIZE", palRFRTOSMenu)
    palRFRTOSStackSize.setLabel("Stack Size (in bytes)")
    palRFRTOSStackSize.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSStackSize.setDefaultValue(1024)
    palRFRTOSStackSize.setMin(256)
    palRFRTOSStackSize.setMax(16*1024)

    palRFRTOSMsgQSize = g3PalRfComponent.createIntegerSymbol("PAL_RF_RTOS_TASK_MSG_QTY", palRFRTOSMenu)
    palRFRTOSMsgQSize.setLabel("Maximum Message Queue Size")
    palRFRTOSMsgQSize.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSMsgQSize.setDescription("A µC/OS-III task contains an optional internal message queue (if OS_CFG_TASK_Q_EN is set to DEF_ENABLED in os_cfg.h). This argument specifies the maximum number of messages that the task can receive through this message queue. The user may specify that the task is unable to receive messages by setting this argument to 0")
    palRFRTOSMsgQSize.setDefaultValue(0)
    palRFRTOSMsgQSize.setVisible(getActiveRtos() == "MicriumOSIII")
    palRFRTOSMsgQSize.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    palRFRTOSTaskTimeQuanta = g3PalRfComponent.createIntegerSymbol("PAL_RF_RTOS_TASK_TIME_QUANTA", palRFRTOSMenu)
    palRFRTOSTaskTimeQuanta.setLabel("Task Time Quanta")
    palRFRTOSTaskTimeQuanta.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskTimeQuanta.setDescription("The amount of time (in clock ticks) for the time quanta when Round Robin is enabled. If you specify 0, then the default time quanta will be used which is the tick rate divided by 10.")
    palRFRTOSTaskTimeQuanta.setDefaultValue(0)
    palRFRTOSTaskTimeQuanta.setVisible(getActiveRtos() == "MicriumOSIII")
    palRFRTOSTaskTimeQuanta.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    palRFRTOSTaskPriority = g3PalRfComponent.createIntegerSymbol("PAL_RF_RTOS_TASK_PRIORITY", palRFRTOSMenu)
    palRFRTOSTaskPriority.setLabel("Task Priority")
    palRFRTOSTaskPriority.setHelp(g3_pal_rf_helpkeyword)
    if (Database.getComponentByID("FreeRTOS") == None):
        prio = 2
    else:
        prio = Database.getSymbolValue("FreeRTOS", "FREERTOS_MAX_PRIORITIES")
        
    palRFRTOSTaskPriority.setDefaultValue(prio)

    palRFRTOSTaskSpecificOpt = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_TASK_OPT_NONE", palRFRTOSMenu)
    palRFRTOSTaskSpecificOpt.setLabel("Task Specific Options")
    palRFRTOSTaskSpecificOpt.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskSpecificOpt.setDescription("Contains task-specific options. Each option consists of one bit. The option is selected when the bit is set. The current version of µC/OS-III supports the following options:")
    palRFRTOSTaskSpecificOpt.setDefaultValue(True)
    palRFRTOSTaskSpecificOpt.setVisible(getActiveRtos() == "MicriumOSIII")
    palRFRTOSTaskSpecificOpt.setDependencies(commandRtosMicriumOSIIIAppTaskVisibility, ["HarmonyCore.SELECT_RTOS"])

    palRFRTOSTaskStkChk = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_TASK_OPT_STK_CHK", palRFRTOSTaskSpecificOpt)
    palRFRTOSTaskStkChk.setLabel("Stack checking is allowed for the task")
    palRFRTOSTaskStkChk.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskStkChk.setDescription("Specifies whether stack checking is allowed for the task")
    palRFRTOSTaskStkChk.setDefaultValue(True)
    palRFRTOSTaskStkChk.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["PAL_RF_RTOS_TASK_OPT_NONE"])

    palRFRTOSTaskStkClr = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_TASK_OPT_STK_CLR", palRFRTOSTaskSpecificOpt)
    palRFRTOSTaskStkClr.setLabel("Stack needs to be cleared")
    palRFRTOSTaskStkClr.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskStkClr.setDescription("Specifies whether the stack needs to be cleared")
    palRFRTOSTaskStkClr.setDefaultValue(True)
    palRFRTOSTaskStkClr.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["PAL_RF_RTOS_TASK_OPT_NONE"])

    palRFRTOSTaskSaveFp = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_TASK_OPT_SAVE_FP", palRFRTOSTaskSpecificOpt)
    palRFRTOSTaskSaveFp.setLabel("Floating-point registers needs to be saved")
    palRFRTOSTaskSaveFp.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskSaveFp.setDescription("Specifies whether floating-point registers are saved. This option is only valid if the processor has floating-point hardware and the processor-specific code saves the floating-point registers")
    palRFRTOSTaskSaveFp.setDefaultValue(False)
    palRFRTOSTaskSaveFp.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["PAL_RF_RTOS_TASK_OPT_NONE"])

    palRFRTOSTaskNoTls = g3PalRfComponent.createBooleanSymbol("PAL_RF_RTOS_TASK_OPT_NO_TLS", palRFRTOSTaskSpecificOpt)
    palRFRTOSTaskNoTls.setLabel("TLS (Thread Local Storage) support needed for the task")
    palRFRTOSTaskNoTls.setHelp(g3_pal_rf_helpkeyword)
    palRFRTOSTaskNoTls.setDescription("If the caller doesn’t want or need TLS (Thread Local Storage) support for the task being created. If you do not include this option, TLS will be supported by default. TLS support was added in V3.03.00")
    palRFRTOSTaskNoTls.setDefaultValue(False)
    palRFRTOSTaskNoTls.setDependencies(commandRtosMicriumOSIIITaskOptVisibility, ["PAL_RF_RTOS_TASK_OPT_NONE"])


    #####################################################################################################################################
    # G3 PAL RF FILES 

    g3PalRfSrcFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SOURCE", None)
    g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_wbz45.c.ftl")
    g3PalRfSrcFile.setOutputName("pal_rf.c")
    g3PalRfSrcFile.setDestPath("stack/g3/pal/rf")
    g3PalRfSrcFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfSrcFile.setType("SOURCE")
    g3PalRfSrcFile.setMarkup(True)

    g3PalRfHdrFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_HEADER", None)
    g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_wbz45.h")
    g3PalRfHdrFile.setOutputName("pal_rf.h")
    g3PalRfHdrFile.setDestPath("stack/g3/pal/rf")
    g3PalRfHdrFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfHdrFile.setType("HEADER")

    g3PalRfHdrLocalFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_HEADER_LOCAL", None)
    g3PalRfHdrLocalFile.setSourcePath("pal/rf/pal_rf_wbz45_local.h.ftl")
    g3PalRfHdrLocalFile.setOutputName("pal_rf_local.h")
    g3PalRfHdrLocalFile.setDestPath("stack/g3/pal/rf")
    g3PalRfHdrLocalFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfHdrLocalFile.setType("HEADER")
    g3PalRfHdrLocalFile.setMarkup(True)

    #####################################################################################################################################
    # G3 PAL RF TEMPLATES 
    
    g3PalRfSystemConfigFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_CONFIGURATION", None)
    g3PalRfSystemConfigFile.setType("STRING")
    g3PalRfSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    g3PalRfSystemConfigFile.setSourcePath("pal/rf/templates/system/configuration.h.ftl")
    g3PalRfSystemConfigFile.setMarkup(True)

    g3PalRfSystemDefFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_DEF", None)
    g3PalRfSystemDefFile.setType("STRING")
    g3PalRfSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    g3PalRfSystemDefFile.setSourcePath("pal/rf/templates/system/definitions.h.ftl")
    g3PalRfSystemDefFile.setMarkup(True)

    g3PalRfSystemTasksFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SYS_TASK", None)
    g3PalRfSystemTasksFile.setType("STRING")
    g3PalRfSystemTasksFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    g3PalRfSystemTasksFile.setSourcePath("pal/rf/templates/system/system_tasks.c.ftl")
    g3PalRfSystemTasksFile.setMarkup(True)

    g3PalRfSystemRtosTasksFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SYS_RTOS_TASK", None)
    g3PalRfSystemRtosTasksFile.setType("STRING")
    g3PalRfSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    g3PalRfSystemRtosTasksFile.setSourcePath("pal/rf/templates/system/system_rtos_tasks.c.ftl")
    g3PalRfSystemRtosTasksFile.setMarkup(True)

    