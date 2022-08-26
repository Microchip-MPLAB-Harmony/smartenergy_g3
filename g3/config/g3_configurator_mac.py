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

def instantiateComponent(g3AutoConfigMacComponent):
    
    g3AutoConfigStackGroup = Database.findGroup("G3 STACK")
    if (g3AutoConfigStackGroup == None):
        g3AutoConfigStackGroup = Database.createGroup(None, "G3 STACK")
  
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    if (g3AutoConfigMacGroup == None):
        g3AutoConfigMacGroup = Database.createGroup("G3 STACK", "MAC LAYER")   
        
    # Enable MAC Layer Configurator
    g3AutoConfigMacEnable = g3AutoConfigMacComponent.createBooleanSymbol("G3_AUTOCONFIG_MAC_ENABLE", None)
    g3AutoConfigMacEnable.setVisible(False)
    g3AutoConfigMacEnable.setDefaultValue(True)
    
    # Enable MAC PLC
    g3AutoConfigMacPlc = g3AutoConfigMacComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_MAC_PLC", None)
    g3AutoConfigMacPlc.setLabel("PLC MAC")
    g3AutoConfigMacPlc.setVisible(True)
    g3AutoConfigMacPlc.setDescription("Enable PLC MAC") 
    g3AutoConfigMacPlc.setDependencies(g3AutoConfigMacPlcEnable, ["G3_AUTOCONFIG_ENABLE_MAC_PLC"])
    
    # Enable MAC RF
    g3AutoConfigMacRf = g3AutoConfigMacComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_MAC_RF", None)
    g3AutoConfigMacRf.setLabel("RF MAC")
    g3AutoConfigMacRf.setVisible(True)
    g3AutoConfigMacRf.setDescription("Enable RF MAC") 
    g3AutoConfigMacRf.setDependencies(g3AutoConfigMacRfEnable, ["G3_AUTOCONFIG_ENABLE_MAC_RF"])
    
    # Enable MAC Wrapper
    g3AutoConfigMacWrp = g3AutoConfigMacComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_MAC_WRAPPER", None)
    g3AutoConfigMacWrp.setVisible(False)
    g3AutoConfigMacWrp.setDependencies(g3AutoConfigMacWrpEnable, ["G3_AUTOCONFIG_ENABLE_MAC_WRAPPER"])
    
    # Enable MAC Common
    g3AutoConfigMacCommon = g3AutoConfigMacComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_MAC_COMMON", None)
    g3AutoConfigMacCommon.setVisible(False)
    g3AutoConfigMacCommon.setDependencies(g3AutoConfigMacWrpEnable, ["G3_AUTOCONFIG_ENABLE_MAC_COMMON"])
 
########################################################################################################
def finalizeComponent(g3AutoConfigMacComponent):
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    g3AutoConfigMacGroup.addComponent(g3AutoConfigMacComponent.getID())

    if(Database.getSymbolValue("g3_coordinator_config", "G3_AUTOCONFIG_COORDINATOR_ENABLE") != True) and (Database.getSymbolValue("g3_adapt_config", "G3_AUTOCONFIG_ADAPT_ENABLE") != True):
        Database.setActiveGroup("MAC LAYER")  
####################################################################################################### 
def enableG3AutoConfigMac(enable):

    if(enable == True):
        g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
        if (g3AutoConfigMacGroup == None):
            g3AutoConfigMacGroup = Database.createGroup("G3 STACK", "MAC LAYER")

################# MAC Layer #########################################  
def g3AutoConfigMacCommonEnable(symbol, event):
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    enableG3AutoConfigMac(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3MacCommon"], "MAC LAYER")  
        g3AutoConfigMacGroup.setAttachmentVisible("g3MacCommon", "libMacCommon")
    else:
        res = Database.deactivateComponents(["g3MacCommon"])
        
def g3AutoConfigMacWrpEnable(symbol, event):
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    enableG3AutoConfigMac(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3MacWrapper"], "MAC LAYER")  
        g3AutoConfigMacGroup.setAttachmentVisible("g3MacWrapper", "libMacWrapper")
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON") != True):
            Database.setSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON", True)
    else:
        res = Database.deactivateComponents(["g3MacWrapper"])

def g3AutoConfigMacPlcEnable(symbol, event):
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    enableG3AutoConfigMac(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3PlcMac"], "MAC LAYER")  
        g3AutoConfigMacGroup.setAttachmentVisible("g3PlcMac", "libPlcMac")
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON") != True):
            Database.setSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON", True)
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER") != True):
            Database.setSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER", True)
    else:
        res = Database.deactivateComponents(["g3PlcMac"])
        
def g3AutoConfigMacRfEnable(symbol, event):
    g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
    enableG3AutoConfigMac(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3RfMac"], "MAC LAYER")  
        g3AutoConfigMacGroup.setAttachmentVisible("g3RfMac", "libRfMac")
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON") != True):
            Database.setSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_COMMON", True)
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER") != True):
            Database.setSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER", True)
    else:
        res = Database.deactivateComponents(["g3RfMac"])
        
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
