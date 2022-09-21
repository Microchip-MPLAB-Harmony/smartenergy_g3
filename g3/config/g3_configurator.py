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
    g3Roles = ["PAN Device", "PAN Coordinator"]
    g3ConfigRole = g3ConfigComponent.createComboSymbol("G3_ROLE", None, g3Roles)
    g3ConfigRole.setLabel("Role for G3 Node")
    g3ConfigRole.setVisible(True)
    g3ConfigRole.setDescription("Select G3 Role")
    g3ConfigRole.setDependencies(g3ConfigRoleChange, ["G3_ROLE"])
    g3ConfigRole.setDefaultValue("PAN Device")

    # Add EAP Server Configuration
    g3ConfigEAP = g3ConfigComponent.createBooleanSymbol("EAP_SERVER_ENABLE", g3ConfigRole)
    g3ConfigEAP.setLabel("Add EAP Server Capabilities")
    g3ConfigEAP.setVisible(False)
    g3ConfigEAP.setDescription("Add EAP Server Capabilities example implementation")
    g3ConfigEAP.setDependencies(g3AddEAP, ["EAP_SERVER_ENABLE"])
    g3ConfigEAP.setDefaultValue(False)

################################################################################
#### Business Logic ####
################################################################################

def addADPComponents():
    g3ConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (g3ConfigAdaptGroup == None):
        g3ConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")
    
    Database.activateComponents(["g3_adapt_config"], "ADAPTATION LAYER")
    component = Database.getComponentByID("g3_adapt_config")
    if (component.getSymbolValue("LOADNG_ENABLE") == True):
        Database.activateComponents(["g3ADP", "g3LOADng", "g3Bootstrap"], "ADAPTATION LAYER")
    else:
        Database.activateComponents(["g3ADP", "g3Bootstrap"], "ADAPTATION LAYER")

def removeADPComponents():
    Database.deactivateComponents(["g3_adapt_config", "g3ADP", "g3LOADng", "g3Bootstrap"])
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

def enableEapConfig(symbol):
    component = symbol.getComponent()
    eapSymbol = component.getSymbolByID("EAP_SERVER_ENABLE")
    roleSymbol = component.getSymbolByID("G3_ROLE")
    if roleSymbol.getValue() == "PAN Coordinator":
        eapSymbol.setVisible(True)
        if eapSymbol.getValue() == True:
            Database.activateComponents(["g3EAPServer"], "G3 STACK")
    else:
        Database.deactivateComponents(["g3EAPServer"])

def disableEapConfig(symbol):
    component = symbol.getComponent()
    eapSymbol = component.getSymbolByID("EAP_SERVER_ENABLE")
    eapSymbol.setVisible(False)
    Database.deactivateComponents(["g3EAPServer"])

def g3ConfigSelection(symbol, event):
    if event["value"] == "G3 Stack (ADP + MAC) Hybrid PLC & RF":
        # Complete Hybrid stack, enable components
        addADPComponents()
        addMACComponents(True, True)
        enableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Both PLC & RF MAC Layers")
    elif event["value"] == "G3 Stack (ADP + MAC) PLC":
        # Complete PLC stack, enable components
        addADPComponents()
        addMACComponents(True, False)
        enableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only PLC MAC Layer")
    elif event["value"] == "G3 Stack (ADP + MAC) RF":
        # Complete RF stack, enable components
        addADPComponents()
        addMACComponents(False, True)
        enableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only RF MAC Layer")
    elif event["value"] == "G3 MAC Layer Hybrid PLC & RF":
        # Hybrid MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, True)
        disableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Both PLC & RF MAC Layers")
    elif event["value"] == "G3 PLC MAC Layer":
        # PLC MAC, disable and enable components
        removeADPComponents()
        addMACComponents(True, False)
        disableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only PLC MAC Layer")
    elif event["value"] == "G3 RF MAC Layer":
        # RF MAC, disable and enable components
        removeADPComponents()
        addMACComponents(False, True)
        disableEapConfig(symbol)
        setVal("g3_mac_config", "G3_AVAILABLE_MACS", "Only RF MAC Layer")
    else:
        # Remove all G3 components
        removeADPComponents()
        removeMACComponents()
        disableEapConfig(symbol)

def g3ConfigRoleChange(symbol, event):
    if event["value"] == "PAN Device":
        # Device role selected, disable EAP Server option
        disableEapConfig(symbol)
    elif event["value"] == "PAN Coordinator":
        # Coordinator role selected, enable EAP Server option
        enableEapConfig(symbol)
    else:
        # Unknown option, behave as Device
        disableEapConfig(symbol)

def g3AddEAP(symbol, event):
    if (event["value"] == True):
        Database.activateComponents(["g3EAPServer"], "G3 STACK")
    else:
        Database.deactivateComponents(["g3EAPServer"])

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
