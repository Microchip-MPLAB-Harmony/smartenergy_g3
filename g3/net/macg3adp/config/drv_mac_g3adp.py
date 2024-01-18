"""*****************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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

drv_macG3Adp_helpkeyword = "mcc_h3_drv_macG3Adp_config"

interface_number = None

def macG3AdpSetVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def macG3AdpSetClear(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        symbol.clearValue()

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print("Set Symbol Failure" + component + ":" + symbol + ":" + str(value))
        return False
    else:
        return True

#Increment symbols of other components
def incVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "INC_SYMBOL", triggerDict) == None):
        print("Increment Symbol Failure" + component + ":" + symbol)
        return False
    else:
        return True

#Increment symbols of other components
def decVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "DEC_SYMBOL", triggerDict) == None):
        print("Decrement Symbol Failure" + component + ":" + symbol)
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

def g3RoleChanged(symbol, event):
    g3Coordinator = False
    if event["value"] == "PAN Coordinator":
        g3Coordinator = True

    setVal("tcpipIPv6", "TCPIP_IPV6_G3_PLC_BORDER_ROUTER", g3Coordinator)
    if interface_number != None:
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_IDX" + str(interface_number), g3Coordinator)

    if not g3Coordinator:
        setVal("tcpipNdp", "TCPIP_RTR_SOLICITATION_INTERVAL", 20)

def instantiateComponent(drvMacG3AdpComponent):
    
    print("G3 MAC ADP Component")   
    configName = Variables.get("__CONFIGURATION_NAME")
    
    device_node = ATDF.getNode('/avr-tools-device-file/devices/device')
    dev_family = str(device_node.getAttribute("family"))
    processor =  Variables.get("__PROCESSOR")  

    if(Database.getComponentByID("srvQueue") == None):
        Database.activateComponents(["srvQueue"])


    # Default TX Packet Queue limit
    drvMacG3AdpTxQueueLimit = drvMacG3AdpComponent.createIntegerSymbol("DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT", None)
    drvMacG3AdpTxQueueLimit.setHelp("drv_macG3Adp_helpkeyword")
    drvMacG3AdpTxQueueLimit.setLabel("TX Queue Limit")
    drvMacG3AdpTxQueueLimit.setDescription("Default TX MAC Packet Queue limit")
    drvMacG3AdpTxQueueLimit.setDefaultValue(5)
    drvMacG3AdpTxQueueLimit.setMin(1)
    drvMacG3AdpTxQueueLimit.setMax(10)
    drvMacG3AdpTxQueueLimit.setDependencies(g3RoleChanged, ["g3_config.G3_ROLE"])

    # Default RX Packet Queue limit
    drvMacG3AdpRxQueueLimit = drvMacG3AdpComponent.createIntegerSymbol("DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT", None)
    drvMacG3AdpRxQueueLimit.setHelp("drv_macG3Adp_helpkeyword")
    drvMacG3AdpRxQueueLimit.setLabel("RX Queue Limit")
    drvMacG3AdpRxQueueLimit.setDescription("Default RX MAC Packet Queue limit")
    drvMacG3AdpRxQueueLimit.setDefaultValue(2)
    drvMacG3AdpRxQueueLimit.setMin(1)
    drvMacG3AdpRxQueueLimit.setMax(10)
    
    # Add drv_mac_g3adp.h file to project
    drvMacG3AdpHeaderFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_HEADER_FILE", None)
    drvMacG3AdpHeaderFile.setSourcePath("g3/net/macg3adp/drv_mac_g3adp.h")
    drvMacG3AdpHeaderFile.setOutputName("drv_mac_g3adp.h")
    drvMacG3AdpHeaderFile.setDestPath("stack/g3/net/macg3adp/")
    drvMacG3AdpHeaderFile.setProjectPath("config/" + configName + "/stack/g3/net/macg3adp/")
    drvMacG3AdpHeaderFile.setType("HEADER")
    drvMacG3AdpHeaderFile.setOverwrite(True)

    # Add drv_mac_g3adp_local.h file to project
    drvMacG3AdpLocalHeaderFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_LOCAL_HEADER_FILE", None)
    drvMacG3AdpLocalHeaderFile.setSourcePath("g3/net/macg3adp/src/drv_mac_g3adp_local.h.ftl")
    drvMacG3AdpLocalHeaderFile.setOutputName("drv_mac_g3adp_local.h")
    drvMacG3AdpLocalHeaderFile.setDestPath("stack/g3/net/macg3adp/src/")
    drvMacG3AdpLocalHeaderFile.setProjectPath("config/" + configName + "/stack/g3/net/macg3adp/src/")
    drvMacG3AdpLocalHeaderFile.setType("HEADER")
    drvMacG3AdpLocalHeaderFile.setMarkup(True)

    # Add drv_mac_g3adp.c file
    drvMacG3AdpSourceFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_SOURCE_FILE", None)
    drvMacG3AdpSourceFile.setSourcePath("g3/net/macg3adp/src/drv_mac_g3adp.c.ftl")
    drvMacG3AdpSourceFile.setOutputName("drv_mac_g3adp.c")
    drvMacG3AdpSourceFile.setDestPath("stack/g3/net/macg3adp/src/")
    drvMacG3AdpSourceFile.setProjectPath("config/" + configName + "/stack/g3/net/macg3adp/src/")
    drvMacG3AdpSourceFile.setType("SOURCE")
    drvMacG3AdpSourceFile.setEnabled(True)
    drvMacG3AdpSourceFile.setMarkup(True)
    
    # Add to definitions.h
    MacG3AdpSystemDefFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_DEFINITIONS_FILE", None)
    MacG3AdpSystemDefFile.setType("STRING")
    MacG3AdpSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    MacG3AdpSystemDefFile.setSourcePath("g3/net/macg3adp/templates/system/system_definitions.h.ftl")
    MacG3AdpSystemDefFile.setMarkup(True)  

def onAttachmentConnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        global interface_number
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))

        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), True)
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")
        incVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")

        g3Role = Database.getSymbolValue("g3_config", "G3_ROLE")
        g3Coordinator = False
        if g3Role == "PAN Coordinator":
            g3Coordinator = True

        setVal("tcpipIPv6", "TCPIP_IPV6_DEFAULT_LINK_MTU", 1280)
        setVal("tcpipIPv6", "TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE", 1280)
        setVal("tcpipIPv6", "TCPIP_IPV6_G3_PLC_SUPPORT", True)
        setVal("tcpipIPv6", "TCPIP_IPV6_G3_PLC_BORDER_ROUTER", g3Coordinator)

        if not g3Coordinator:
            setVal("tcpipNdp", "TCPIP_RTR_SOLICITATION_INTERVAL", 20)

        setVal("tcpipUdp", "TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", 1200)

        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_CONFIG_MULTICAST_IDX" + str(interface_number), True)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX" + str(interface_number), False)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_IDX" + str(interface_number), True)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_IDX" + str(interface_number), g3Coordinator)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_ADV_IDX" + str(interface_number), False)

def onAttachmentDisconnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        global interface_number
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        decVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        interface_number = None
