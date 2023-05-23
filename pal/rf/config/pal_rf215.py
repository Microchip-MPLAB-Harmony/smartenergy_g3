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
g3_pal_rf_helpkeyword = "mcc_g3_pal_rf_rf215_config"

def showSymbol(symbol, event):
    symbol.setVisible(event["value"])

    if (event["value"] == True):
        usiInstances = filter(lambda k: "srv_usi_" in k, Database.getActiveComponentIDs())
        symbol.setMax(len(usiInstances) - 1)

def activatesDependencies(symbol, event):
    if (event["id"] == "G3_PAL_RF_PHY_SNIFFER_EN"):
        if (event["value"] == True):
            if(Database.getComponentByID("srv_usi") == None):
                Database.activateComponents(["srv_usi"])
                
            if(Database.getComponentByID("srv_rsniffer") == None):
                Database.activateComponents(["srv_rsniffer"])

def instantiateComponent(g3PalRfComponent):
    
    Log.writeInfoMessage("Loading G3 PAL RF module")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    g3PalRfPhySnifferEnable = g3PalRfComponent.createBooleanSymbol("G3_PAL_RF_PHY_SNIFFER_EN", None)
    g3PalRfPhySnifferEnable.setLabel("Enable RF PHY sniffer")
    g3PalRfPhySnifferEnable.setDefaultValue(False)
    g3PalRfPhySnifferEnable.setHelp(g3_pal_rf_helpkeyword)

    g3PalRfUSIInstance = g3PalRfComponent.createIntegerSymbol("G3_PAL_RF_USI_INSTANCE", g3PalRfPhySnifferEnable)
    g3PalRfUSIInstance.setLabel("USI Instance")
    g3PalRfUSIInstance.setDefaultValue(0)
    g3PalRfUSIInstance.setMax(0)
    g3PalRfUSIInstance.setMin(0)
    g3PalRfUSIInstance.setVisible(False)
    g3PalRfUSIInstance.setHelp(g3_pal_rf_helpkeyword)
    g3PalRfUSIInstance.setDependencies(showSymbol, ["G3_PAL_RF_PHY_SNIFFER_EN"])

    g3PalRfDummySymbol = g3PalRfComponent.createBooleanSymbol("G3_PAL_RF_DUMMY", None)
    g3PalRfDummySymbol.setLabel("Dummy")
    g3PalRfDummySymbol.setDefaultValue(False)
    g3PalRfDummySymbol.setVisible(False)
    g3PalRfDummySymbol.setDependencies(activatesDependencies, ["G3_PAL_RF_PHY_SNIFFER_EN"])
    
    #####################################################################################################################################
    # G3 PAL RF FILES 

    g3PalRfSrcFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SOURCE", None)
    g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_rf215.c.ftl")
    g3PalRfSrcFile.setOutputName("pal_rf.c")
    g3PalRfSrcFile.setDestPath("stack/g3/pal/rf")
    g3PalRfSrcFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfSrcFile.setType("SOURCE")
    g3PalRfSrcFile.setMarkup(True)

    g3PalRfHdrFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_HEADER", None)
    g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_rf215.h")
    g3PalRfHdrFile.setOutputName("pal_rf.h")
    g3PalRfHdrFile.setDestPath("stack/g3/pal/rf")
    g3PalRfHdrFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfHdrFile.setType("HEADER")

    g3PalRfHdrLocalFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_HEADER_LOCAL", None)
    g3PalRfHdrLocalFile.setSourcePath("pal/rf/pal_rf_rf215_local.h.ftl")
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

    g3PalRfSymSystemDefObjFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SYSTEM_DEF_OBJECT", None)
    g3PalRfSymSystemDefObjFile.setType("STRING")
    g3PalRfSymSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    g3PalRfSymSystemDefObjFile.setSourcePath("pal/rf/templates/system/definitions_objects.h.ftl")
    g3PalRfSymSystemDefObjFile.setMarkup(True)
