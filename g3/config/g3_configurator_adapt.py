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

def instantiateComponent(g3ConfigAdaptComponent):
    Log.writeInfoMessage("Loading Adaptation Layer Configurator for G3")

    # Enable LOADng Configuration
    g3ConfigLOADng = g3ConfigAdaptComponent.createBooleanSymbol("LOADNG_ENABLE", None)
    g3ConfigLOADng.setLabel("Enable LOADng Routing")
    g3ConfigLOADng.setVisible(True)
    g3ConfigLOADng.setDescription("Enable LOADng Routing Protocol")
    g3ConfigLOADng.setDependencies(g3LOADngEnable, ["LOADNG_ENABLE"])
    g3ConfigLOADng.setDefaultValue(True)

    g3CountBuffers1280 = g3ConfigAdaptComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_1280", None)
    g3CountBuffers1280.setLabel("Number of 1280-byte buffers")
    g3CountBuffers1280.setDescription("Number of 1280-byte buffers for adaptation layer")
    g3CountBuffers1280.setDefaultValue(1)
    g3CountBuffers1280.setMin(1)
    g3CountBuffers1280.setMax(16)

    g3CountBuffers400 = g3ConfigAdaptComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_400", None)
    g3CountBuffers400.setLabel("Number of 400-byte buffers")
    g3CountBuffers400.setDescription("Number of 400-byte buffers for adaptation layer")
    g3CountBuffers400.setDefaultValue(3)
    g3CountBuffers400.setMin(1)
    g3CountBuffers400.setMax(32)

    g3CountBuffers100 = g3ConfigAdaptComponent.createIntegerSymbol("ADP_COUNT_BUFFERS_100", None)
    g3CountBuffers100.setLabel("Number of 100-byte buffers")
    g3CountBuffers100.setDescription("Number of 100-byte buffers for adaptation layer")
    g3CountBuffers100.setDefaultValue(3)
    g3CountBuffers100.setMin(1)
    g3CountBuffers100.setMax(64)

    g3FragmentedTransferTableSize = g3ConfigAdaptComponent.createIntegerSymbol("ADP_FRAGMENTED_TRANSFER_TABLE_SIZE", None)
    g3FragmentedTransferTableSize.setLabel("Fragmented transfer table size")
    g3FragmentedTransferTableSize.setDescription("The number of fragmented transfers which can be handled in parallel")
    g3FragmentedTransferTableSize.setDefaultValue(1)
    g3FragmentedTransferTableSize.setMin(1)
    g3FragmentedTransferTableSize.setMax(16)

#def finalizeComponent(g3ConfigAdaptComponent):

def g3LOADngEnable(symbol, event):
    #g3AdaptGroup = Database.findGroup("ADAPTATION LAYER")
    if (event["value"] == True):
        Database.activateComponents(["g3LOADng"], "ADAPTATION LAYER")
    else:
        Database.deactivateComponents(["g3LOADng"])

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
