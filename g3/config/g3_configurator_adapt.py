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

def instantiateComponent(g3AutoConfigAdaptComponent):
   
    g3AutoConfigStackGroup = Database.findGroup("G3 STACK")
    if (g3AutoConfigStackGroup == None):
        g3AutoConfigStackGroup = Database.createGroup(None, "G3 STACK")
        
    g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (g3AutoConfigAdaptGroup == None):
        g3AutoConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")   

    # Enable Adaptation Layer Configurator
    g3AutoConfigAdaptEnable = g3AutoConfigAdaptComponent.createBooleanSymbol("G3_AUTOCONFIG_ADAPT_ENABLE", None)
    g3AutoConfigAdaptEnable.setVisible(False)
    g3AutoConfigAdaptEnable.setDefaultValue(True)

    # Enable ADP
    g3AutoConfigADP = g3AutoConfigAdaptComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_ADP", None)
    g3AutoConfigADP.setLabel("ADP")
    g3AutoConfigADP.setVisible(True)
    g3AutoConfigADP.setDescription("Enable ADP") 
    g3AutoConfigADP.setDependencies(g3AutoConfigADPEnable, ["G3_AUTOCONFIG_ENABLE_ADP"])

    # Enable LOADng
    g3AutoConfigLOADng = g3AutoConfigAdaptComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_LOADNG", None)
    g3AutoConfigLOADng.setLabel("LOADng")
    g3AutoConfigLOADng.setVisible(True)
    g3AutoConfigLOADng.setDescription("Enable LOADng") 
    g3AutoConfigLOADng.setDependencies(g3AutoConfigLOADngEnable, ["G3_AUTOCONFIG_ENABLE_LOADNG"])

    # Enable Bootstrap
    g3AutoConfigBootstrap = g3AutoConfigAdaptComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_BOOTSTRAP", None)
    g3AutoConfigBootstrap.setLabel("Bootstrap")
    g3AutoConfigBootstrap.setVisible(True)
    g3AutoConfigBootstrap.setDescription("Enable Bootstrap") 
    g3AutoConfigBootstrap.setDependencies(g3AutoConfigBootstrapEnable, ["G3_AUTOCONFIG_ENABLE_BOOTSTRAP"])

########################################################################################################
def finalizeComponent(g3AutoConfigAdaptComponent):
    g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    g3AutoConfigAdaptGroup.addComponent(g3AutoConfigAdaptComponent.getID())

    if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_MAC_ENABLE") != True) and (Database.getSymbolValue("g3_coordinator_config", "G3_AUTOCONFIG_COORDINATOR_ENABLE") != True):
        Database.setActiveGroup("ADAPTATION LAYER")  
#######################################################################################################
def enableG3AutoConfigAdapt(enable):

    if(enable == True):
        g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
        if (g3AutoConfigMacGroup == None):
            g3AutoConfigMacGroup = Database.createGroup("G3 STACK", "MAC LAYER")
            
        g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
        if (g3AutoConfigAdaptGroup == None):
            g3AutoConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")

        if(Database.getComponentByID("g3_mac_config") == None):
            res = g3AutoConfigMacGroup.addComponent("g3_mac_config")
            res = Database.activateComponents(["g3_mac_config"], "MAC LAYER", False)

################# Adaptation Layer #########################################  
def g3AutoConfigADPEnable(symbol, event):
    g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    enableG3AutoConfigAdapt(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3ADP"], "ADAPTATION LAYER")  
        g3AutoConfigAdaptGroup.setAttachmentVisible("g3ADP", "libADP")
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER") != True):
            setVal("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER", True)
    else:
        res = Database.deactivateComponents(["g3ADP"])

def g3AutoConfigLOADngEnable(symbol, event):
    g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    enableG3AutoConfigAdapt(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3LOADng"], "ADAPTATION LAYER")  
        g3AutoConfigAdaptGroup.setAttachmentVisible("g3LOADng", "libLOADng")
        if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER") != True):
            setVal("g3_mac_config", "G3_AUTOCONFIG_ENABLE_MAC_WRAPPER", True)
    else:
        res = Database.deactivateComponents(["g3LOADng"])

def g3AutoConfigBootstrapEnable(symbol, event):
    g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
    enableG3AutoConfigAdapt(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3Bootstrap"], "ADAPTATION LAYER")  
        g3AutoConfigAdaptGroup.setAttachmentVisible("g3Bootstrap", "libBootstrap")
    else:
        res = Database.deactivateComponents(["g3Bootstrap"])

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
