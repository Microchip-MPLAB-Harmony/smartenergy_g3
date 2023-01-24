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

    loadngPendingRReqTable = g3ConfigAdaptComponent.createIntegerSymbol("LOADNG_PENDING_RREQ_TABLE_SIZE", g3ConfigLOADng)
    loadngPendingRReqTable.setLabel("Pending RREQ table size")
    loadngPendingRReqTable.setDescription("Number of RREQs/RERRs that can be stored to respect the parameter ADP_IB_RREQ_WAIT")
    loadngPendingRReqTable.setDefaultValue(6)
    loadngPendingRReqTable.setMin(1)
    loadngPendingRReqTable.setMax(128)
    loadngPendingRReqTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngRRepGenerationTable = g3ConfigAdaptComponent.createIntegerSymbol("LOADNG_RREP_GENERATION_TABLE_SIZE", g3ConfigLOADng)
    loadngRRepGenerationTable.setLabel("RREQ generation table size")
    loadngRRepGenerationTable.setDescription("Number of RREQs from different sources, that can be handled at the same time")
    loadngRRepGenerationTable.setDefaultValue(3)
    loadngRRepGenerationTable.setMin(1)
    loadngRRepGenerationTable.setMax(128)
    loadngRRepGenerationTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngRReqForwardingTable = g3ConfigAdaptComponent.createIntegerSymbol("LOADNG_RREQ_FORWARDING_TABLE_SIZE", g3ConfigLOADng)
    loadngRReqForwardingTable.setLabel("RREP forwarding table size")
    loadngRReqForwardingTable.setDescription("Number of entries of RREP forwarding table for LOADNG")
    loadngRReqForwardingTable.setDefaultValue(5)
    loadngRReqForwardingTable.setMin(1)
    loadngRReqForwardingTable.setMax(128)
    loadngRReqForwardingTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    loadngDiscoverRouteTable = g3ConfigAdaptComponent.createIntegerSymbol("LOADNG_DISCOVER_ROUTE_TABLE_SIZE", g3ConfigLOADng)
    loadngDiscoverRouteTable.setLabel("Discover route table size")
    loadngDiscoverRouteTable.setDescription("Number of route discover that can be handled at the same time")
    loadngDiscoverRouteTable.setDefaultValue(3)
    loadngDiscoverRouteTable.setMin(1)
    loadngDiscoverRouteTable.setMax(128)
    loadngDiscoverRouteTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpRoutingTable = g3ConfigAdaptComponent.createIntegerSymbol("ADP_ROUTING_TABLE_SIZE", g3ConfigLOADng)
    adpRoutingTable.setLabel("Routing table size")
    adpRoutingTable.setDescription("Number of entries in the Routing Table for LOADNG")
    adpRoutingTable.setDefaultValue(150)
    adpRoutingTable.setMin(1)
    adpRoutingTable.setMax(1024)
    adpRoutingTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpBlacklistTable = g3ConfigAdaptComponent.createIntegerSymbol("ADP_BLACKLIST_TABLE_SIZE", g3ConfigLOADng)
    adpBlacklistTable.setLabel("Blacklist table size")
    adpBlacklistTable.setDescription("Number of entries in the Blacklist Table for LOADNG")
    adpBlacklistTable.setDefaultValue(20)
    adpBlacklistTable.setMin(1)
    adpBlacklistTable.setMax(256)
    adpBlacklistTable.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpRoutingSet = g3ConfigAdaptComponent.createIntegerSymbol("ADP_ROUTING_SET_SIZE", g3ConfigLOADng)
    adpRoutingSet.setLabel("Routing set size")
    adpRoutingSet.setDescription("Number of entries in the Routing Set for LOADNG")
    adpRoutingSet.setDefaultValue(30)
    adpRoutingSet.setMin(1)
    adpRoutingSet.setMax(256)
    adpRoutingSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

    adpDestinationAddressSet = g3ConfigAdaptComponent.createIntegerSymbol("ADP_DESTINATION_ADDRESS_SET_SIZE", g3ConfigLOADng)
    adpDestinationAddressSet.setLabel("Destination address set size")
    adpDestinationAddressSet.setDescription("Number of entries in the Destination Address Set Table")
    adpDestinationAddressSet.setDefaultValue(1)
    adpDestinationAddressSet.setMin(1)
    adpDestinationAddressSet.setMax(128)
    adpDestinationAddressSet.setDependencies(showSymbol, ["LOADNG_ENABLE"])

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

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    #### Routing Wrapper Files #################################################
    routingWrapperHeader = g3ConfigAdaptComponent.createFileSymbol("G3_ROUTING_WRAPPER_HEADER", None)
    routingWrapperHeader.setSourcePath("g3/src/routing_wrapper/routing_wrapper.h")
    routingWrapperHeader.setOutputName("routing_wrapper.h")
    routingWrapperHeader.setDestPath("stack/g3/adaptation")
    routingWrapperHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingWrapperHeader.setType("HEADER")

    routingWrapperSource = g3ConfigAdaptComponent.createFileSymbol("G3_ROUTING_WRAPPER_SOURCE", None)
    routingWrapperSource.setSourcePath("g3/src/routing_wrapper/routing_wrapper.c.ftl")
    routingWrapperSource.setOutputName("routing_wrapper.c")
    routingWrapperSource.setDestPath("stack/g3/adaptation")
    routingWrapperSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    routingWrapperSource.setType("SOURCE")
    routingWrapperSource.setMarkup(True)

#def finalizeComponent(g3ConfigAdaptComponent):

def showSymbol(symbol, event):
    # Show/hide configuration symbol depending on parent enabled/disabled
    symbol.setVisible(event["value"])

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
