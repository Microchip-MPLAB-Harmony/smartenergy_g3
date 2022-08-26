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

def instantiateComponent(g3AutoConfigCoordinatorComponent):
    
    g3AutoConfigStackGroup = Database.findGroup("G3 STACK")
    if (g3AutoConfigStackGroup == None):
        g3AutoConfigStackGroup = Database.createGroup(None, "G3 STACK")
  
    g3AutoConfigCoordGroup = Database.findGroup("COORDINATOR")
    if (g3AutoConfigCoordGroup == None):
        g3AutoConfigCoordGroup = Database.createGroup("G3 STACK", "COORDINATOR")   
        
    # Enable Coordinator Configurator
    g3AutoConfigCoordConfEnable = g3AutoConfigCoordinatorComponent.createBooleanSymbol("G3_AUTOCONFIG_COORDINATOR_ENABLE", None)
    g3AutoConfigCoordConfEnable.setVisible(False)
    g3AutoConfigCoordConfEnable.setDefaultValue(True)
    
    # Enable Coordinator
    g3AutoConfigCoord = g3AutoConfigCoordinatorComponent.createBooleanSymbol("G3_AUTOCONFIG_ENABLE_COORD", None)
    g3AutoConfigCoord.setLabel("Coordinator")
    g3AutoConfigCoord.setVisible(True)
    g3AutoConfigCoord.setDescription("Enable Coordinator") 
    g3AutoConfigCoord.setDependencies(g3AutoConfigCoordEnable, ["G3_AUTOCONFIG_ENABLE_COORD"])

########################################################################################################
def finalizeComponent(g3AutoConfigCoordinatorComponent):
    g3AutoConfigCoordGroup = Database.findGroup("COORDINATOR")
    g3AutoConfigCoordGroup.addComponent(g3AutoConfigCoordinatorComponent.getID())

    if(Database.getSymbolValue("g3_mac_config", "G3_AUTOCONFIG_MAC_ENABLE") != True) and (Database.getSymbolValue("g3_adapt_config", "G3_AUTOCONFIG_ADAPT_ENABLE") != True):
        Database.setActiveGroup("COORDINATOR")  
#######################################################################################################
def enableG3AutoConfigCoordinator(enable):

    if(enable == True):
        g3AutoConfigMacGroup = Database.findGroup("MAC LAYER")
        if (g3AutoConfigMacGroup == None):
            g3AutoConfigMacGroup = Database.createGroup("G3 STACK", "MAC LAYER")
            
        g3AutoConfigAdaptGroup = Database.findGroup("ADAPTATION LAYER")
        if (g3AutoConfigAdaptGroup == None):
            g3AutoConfigAdaptGroup = Database.createGroup("G3 STACK", "ADAPTATION LAYER")
            
        g3AutoConfigCoordGroup = Database.findGroup("COORDINATOR")
        if (g3AutoConfigCoordGroup == None):
            g3AutoConfigCoordGroup = Database.createGroup("G3 STACK", "COORDINATOR")

        if(Database.getComponentByID("g3_mac_config") == None):
            res = g3AutoConfigMacGroup.addComponent("g3_mac_config")
            res = Database.activateComponents(["g3_mac_config"], "MAC LAYER", False)
            
        if(Database.getComponentByID("g3_adapt_config") == None):
            res = g3AutoConfigAdaptGroup.addComponent("g3_adapt_config")
            res = Database.activateComponents(["g3_adapt_config"], "ADAPTATION LAYER", False)
            
################# Coordinator #########################################  
def g3AutoConfigCoordEnable(symbol, event):
    g3AutoConfigCoordGroup = Database.findGroup("COORDINATOR")
    enableG3AutoConfigCoordinator(True)
    if (event["value"] == True):
        res = Database.activateComponents(["g3Coordinator"], "COORDINATOR")  
        g3AutoConfigCoordGroup.setAttachmentVisible("g3Coordinator", "libCoordinator")
        if(Database.getSymbolValue("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_ADP") != True):
            setVal("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_ADP", True)
        if(Database.getSymbolValue("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_LOADNG") != True):
            setVal("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_LOADNG", True)
        if(Database.getSymbolValue("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_BOOTSTRAP") != True):
            setVal("g3_adapt_config", "G3_AUTOCONFIG_ENABLE_BOOTSTRAP", True)
    else:
        res = Database.deactivateComponents(["g3Coordinator"])
        
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
