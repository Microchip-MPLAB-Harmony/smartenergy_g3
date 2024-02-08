# coding: utf-8
"""
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""
g3_pal_rf_helpkeyword = "g3_pal_rf_configurations"

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
    g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_rf215.h.ftl")
    g3PalRfHdrFile.setOutputName("pal_rf.h")
    g3PalRfHdrFile.setDestPath("stack/g3/pal/rf")
    g3PalRfHdrFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfHdrFile.setType("HEADER")
    g3PalRfHdrFile.setMarkup(True)

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
