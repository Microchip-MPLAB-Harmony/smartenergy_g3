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
g3_pal_plc_helpkeyword = "mcc_g3_pal_plc_config"

def showSymbol(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

def activatesDependencies(symbol, event):
    if (event["id"] == "G3_PAL_PLC_PVDD_MONITOR"):
        if (event["value"] == True):
            if(Database.getComponentByID("srv_pvddmon") == None):
                Database.activateComponents(["srv_pvddmon"])
        else:
            Database.deactivateComponents(["srv_pvddmon"])

    if (event["id"] == "G3_PAL_PLC_PHY_SNIFFER_EN"):
        if (event["value"] == True):
            if(Database.getComponentByID("srv_usi") == None):
                Database.activateComponents(["srv_usi"])

def instantiateComponent(g3PalPlcComponent):
    
    Log.writeInfoMessage("Loading G3 PAL PLC module")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    g3PalPlcPhySnifferEnable = g3PalPlcComponent.createBooleanSymbol("G3_PAL_PLC_PHY_SNIFFER_EN", None)
    g3PalPlcPhySnifferEnable.setLabel("Enable G3 PHY sniffer")
    g3PalPlcPhySnifferEnable.setDefaultValue(False)
    g3PalPlcPhySnifferEnable.setHelp(g3_pal_plc_helpkeyword)

    g3PalPlcUSIInstance = g3PalPlcComponent.createIntegerSymbol("G3_PAL_PLC_USI_INSTANCE", g3PalPlcPhySnifferEnable)
    g3PalPlcUSIInstance.setLabel("USI Instance")
    g3PalPlcUSIInstance.setDefaultValue(0)
    g3PalPlcUSIInstance.setMax(0)
    g3PalPlcUSIInstance.setMin(0)
    g3PalPlcUSIInstance.setVisible(False)
    g3PalPlcUSIInstance.setHelp(g3_pal_plc_helpkeyword)
    g3PalPlcUSIInstance.setDependencies(showSymbol, ["G3_PAL_PLC_PHY_SNIFFER_EN"])

    g3PalPlcMacSnifferEnable = g3PalPlcComponent.createBooleanSymbol("G3_PAL_PLC_MAC_SNIFFER_EN", None)
    g3PalPlcMacSnifferEnable.setLabel("Enable G3 MAC sniffer")
    g3PalPlcMacSnifferEnable.setDefaultValue(False)
    g3PalPlcMacSnifferEnable.setHelp(g3_pal_plc_helpkeyword)

    g3PalPlcPVDDMonitor = g3PalPlcComponent.createBooleanSymbol("G3_PAL_PLC_PVDD_MONITOR", None)
    g3PalPlcPVDDMonitor.setLabel("PVDD Monitor")
    g3PalPlcPVDDMonitor.setDefaultValue(False)
    g3PalPlcPVDDMonitor.setHelp(g3_pal_plc_helpkeyword)

    g3PalPlcDummySymbol = g3PalPlcComponent.createBooleanSymbol("G3_PAL_PLC_DUMMY", None)
    g3PalPlcDummySymbol.setLabel("Dummy")
    g3PalPlcDummySymbol.setDefaultValue(False)
    g3PalPlcDummySymbol.setVisible(False)
    g3PalPlcDummySymbol.setDependencies(activatesDependencies, ["G3_PAL_PLC_PVDD_MONITOR", "G3_PAL_PLC_PHY_SNIFFER_EN"])

    #####################################################################################################################################
    # G3 PAL PLC FILES 

    g3PalPlcSrcFile = g3PalPlcComponent.createFileSymbol("G3_PAL_PLC_SOURCE", None)
    g3PalPlcSrcFile.setSourcePath("pal/plc/src/pal_plc.c.ftl")
    g3PalPlcSrcFile.setOutputName("pal_plc.c")
    g3PalPlcSrcFile.setDestPath("stack/g3/pal/plc")
    g3PalPlcSrcFile.setProjectPath("config/" + configName + "/stack/g3/pal/plc/")
    g3PalPlcSrcFile.setType("SOURCE")
    g3PalPlcSrcFile.setMarkup(True)

    g3PalPlcHdrFile = g3PalPlcComponent.createFileSymbol("G3_PAL_PLC_HEADER", None)
    g3PalPlcHdrFile.setSourcePath("pal/plc/pal_plc.h")
    g3PalPlcHdrFile.setOutputName("pal_plc.h")
    g3PalPlcHdrFile.setDestPath("stack/g3/pal/plc")
    g3PalPlcHdrFile.setProjectPath("config/" + configName + "/stack/g3/pal/plc/")
    g3PalPlcHdrFile.setType("HEADER")

    g3PalPlcHdrLocalFile = g3PalPlcComponent.createFileSymbol("G3_PAL_PLC_HEADER_LOCAL", None)
    g3PalPlcHdrLocalFile.setSourcePath("pal/plc/pal_plc_local.h.ftl")
    g3PalPlcHdrLocalFile.setOutputName("pal_plc_local.h")
    g3PalPlcHdrLocalFile.setDestPath("stack/g3/pal/plc")
    g3PalPlcHdrLocalFile.setProjectPath("config/" + configName + "/stack/g3/pal/plc/")
    g3PalPlcHdrLocalFile.setType("HEADER")
    g3PalPlcHdrLocalFile.setMarkup(True)

    #####################################################################################################################################
    # G3 PAL PLC TEMPLATES 
    
    g3PalPlcSystemConfigFile = g3PalPlcComponent.createFileSymbol("G3_PAL_PLC_CONFIGURATION", None)
    g3PalPlcSystemConfigFile.setType("STRING")
    g3PalPlcSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    g3PalPlcSystemConfigFile.setSourcePath("pal/plc/templates/system/configuration.h.ftl")
    g3PalPlcSystemConfigFile.setMarkup(True)

    g3PalPlcSystemDefFile = g3PalPlcComponent.createFileSymbol("G3_PAL_PLC_DEF", None)
    g3PalPlcSystemDefFile.setType("STRING")
    g3PalPlcSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    g3PalPlcSystemDefFile.setSourcePath("pal/plc/templates/system/definitions.h.ftl")
    g3PalPlcSystemDefFile.setMarkup(True)
