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

    # Boolean symbols to use in FTLs to generate code
    g3MacPLC = g3ConfigComponent.createBooleanSymbol("G3_DEVICE", None)
    g3MacPLC.setVisible(False)
    g3MacPLC.setDefaultValue(True)
    g3MacRF = g3ConfigComponent.createBooleanSymbol("G3_COORDINATOR", None)
    g3MacRF.setVisible(False)
    g3MacRF.setDefaultValue(False)

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
    g3ConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (g3ConfigAdaptGroup == None):
        g3ConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")
    
    Database.activateComponents(["g3_adapt_config"], "ADAPTATION LAYER")
    component = Database.getComponentByID("g3_adapt_config")
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
    Database.deactivateComponents(["g3_adapt_config", "g3ADP", "g3LOADng", "g3LBP"])
    selectLBPComponents("none")
    Database.disbandGroup("ADAPTATION LAYER")

def removeMACComponents():
    Database.deactivateComponents(["g3_mac_config", "g3MacWrapper", "g3MacCommon", "g3MacPlc", "g3MacRf", "srvSecurity"])
    Database.disbandGroup("MAC LAYER")

def addMACComponents(plc, rf):
    g3ConfigMacGroup = Database.findGroup("MAC LAYER")
    if (g3ConfigMacGroup == None):
        g3ConfigMacGroup = Database.createGroup("G3 STACK", "MAC LAYER")
    
    if (plc and rf):
        Database.activateComponents(["g3_mac_config", "g3MacWrapper", "g3MacCommon", "g3MacPlc", "g3MacRf"], "MAC LAYER")
    elif (plc):
        Database.activateComponents(["g3_mac_config", "g3MacWrapper", "g3MacCommon", "g3MacPlc"], "MAC LAYER")
        Database.deactivateComponents(["g3MacRf"])
    elif (rf):
        Database.activateComponents(["g3_mac_config", "g3MacWrapper", "g3MacCommon", "g3MacRf"], "MAC LAYER")
        Database.deactivateComponents(["g3MacPlc"])
        
    # In every case, add security component
    Database.activateComponents(["srvSecurity"], "G3 STACK")

def g3ConfigSelection(symbol, event):
    if event["value"] == "G3 Stack (ADP + MAC) Hybrid PLC & RF":
        # Complete Hybrid stack, enable components
        addADPComponents()
        addMACComponents(True, True)
        g3ConfigRole.setVisible(True)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Both PLC & RF MAC Layers")
    elif event["value"] == "G3 Stack (ADP + MAC) PLC":
        # Complete PLC stack, enable components
        addADPComponents()
        addMACComponents(True, False)
        g3ConfigRole.setVisible(True)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only PLC MAC Layer")
    elif event["value"] == "G3 Stack (ADP + MAC) RF":
        # Complete RF stack, enable components
        addADPComponents()
        addMACComponents(False, True)
        g3ConfigRole.setVisible(True)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only RF MAC Layer")
    elif event["value"] == "G3 MAC Layer Hybrid PLC & RF":
        # Hybrid MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, True)
        g3ConfigRole.setVisible(False)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Both PLC & RF MAC Layers")
    elif event["value"] == "G3 PLC MAC Layer":
        # PLC MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, False)
        g3ConfigRole.setVisible(False)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only PLC MAC Layer")
    elif event["value"] == "G3 RF MAC Layer":
        # RF MAC, disable and enable components
        removeADPComponents()
        addMACComponents(False, True)
        g3ConfigRole.setVisible(False)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only RF MAC Layer")
    else:
        # Remove all G3 components
        removeADPComponents()
        removeMACComponents()
        g3ConfigRole.setVisible(False)

def g3ConfigRoleChange(symbol, event):
    if event["value"] == "PAN Device":
        # Device role selected
        Database.setSymbolValue("g3_adapt_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_adapt_config", "G3_COORDINATOR", False)
        selectLBPComponents("dev")
    elif event["value"] == "PAN Coordinator":
        # Coordinator role selected
        Database.setSymbolValue("g3_adapt_config", "G3_DEVICE", False)
        Database.setSymbolValue("g3_adapt_config", "G3_COORDINATOR", True)
        selectLBPComponents("coord")
    elif event["value"] == "Dynamic (Selected upon Initialization)":
        # Dynamic role selected
        Database.setSymbolValue("g3_adapt_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_adapt_config", "G3_COORDINATOR", True)
        selectLBPComponents("both")
    else:
        # Unknown option, behave as Device
        Database.setSymbolValue("g3_adapt_config", "G3_DEVICE", True)
        Database.setSymbolValue("g3_adapt_config", "G3_COORDINATOR", False)
        selectLBPComponents("dev")

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
