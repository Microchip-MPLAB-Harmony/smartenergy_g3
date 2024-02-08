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
g3_pal_plc_helpkeyword = "g3_pal_plc_configurations"

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

    global g3PalPlcPVDDMonitor
    g3PalPlcPVDDMonitor = g3PalPlcComponent.createBooleanSymbol("G3_PAL_PLC_PVDD_MONITOR", None)
    g3PalPlcPVDDMonitor.setLabel("PVDD Monitor")
    g3PalPlcPVDDMonitor.setDefaultValue(False)
    g3PalPlcPVDDMonitor.setHelp(g3_pal_plc_helpkeyword)
    if Database.getSymbolValue("drvG3MacRt", "DRV_PLC_MODE") == "PL360":
        g3PalPlcPVDDMonitor.setReadOnly(True)

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

# Handle messages from other components
def handleMessage(messageID, args):
    global g3PalPlcPVDDMonitor
    retDict = {}

    if messageID == "PLC_DEVICE":
        retDict = {"Return": "Success"}
        plcDevice = args["Device"]
        if plcDevice == "PL360":
            g3PalPlcPVDDMonitor.setReadOnly(True)
            g3PalPlcPVDDMonitor.setValue(False)
        else:
            g3PalPlcPVDDMonitor.setReadOnly(False)
    else:
        retDict = {"Return": "UnImplemented Command"}

    return retDict
