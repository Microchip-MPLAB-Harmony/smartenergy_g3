"""*****************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

interfaceNum = []

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
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Increment symbols of other components
def incVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "INC_SYMBOL", triggerDict) == None):
        print "Increment Symbol Failure" + component + ":" + symbol
        return False
    else:
        return True

#Increment symbols of other components
def decVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "DEC_SYMBOL", triggerDict) == None):
        print "Decrement Symbol Failure" + component + ":" + symbol
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

def instantiateComponent(drvMacG3AdpComponent):
    
    print("G3 MAC ADP Component")   
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    device_node = ATDF.getNode('/avr-tools-device-file/devices/device')
    dev_family = str(device_node.getAttribute("family"))
    processor =  Variables.get("__PROCESSOR")  

    if(Database.getComponentByID("srvQueue") == None):
        Database.activateComponents(["srvQueue"])
        
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
    drvMacG3AdpLocalHeaderFile.setSourcePath("g3/net/macg3adp/src/drv_mac_g3adp_local.h")
    drvMacG3AdpLocalHeaderFile.setOutputName("drv_mac_g3adp_local.h")
    drvMacG3AdpLocalHeaderFile.setDestPath("stack/g3/net/macg3adp/src/")
    drvMacG3AdpLocalHeaderFile.setProjectPath("config/" + configName + "/stack/g3/net/macg3adp/src/")
    drvMacG3AdpLocalHeaderFile.setType("HEADER")
    drvMacG3AdpLocalHeaderFile.setOverwrite(True)

    # Add drv_mac_g3adp.c file
    drvMacG3AdpSourceFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_SOURCE_FILE", None)
    drvMacG3AdpSourceFile.setSourcePath("g3/net/macg3adp/src/drv_mac_g3adp.c")
    drvMacG3AdpSourceFile.setOutputName("drv_mac_g3adp.c")
    drvMacG3AdpSourceFile.setOverwrite(True)
    drvMacG3AdpSourceFile.setDestPath("stack/g3/net/macg3adp/src/")
    drvMacG3AdpSourceFile.setProjectPath("config/" + configName + "/stack/g3/net/macg3adp/src/")
    drvMacG3AdpSourceFile.setType("SOURCE")
    drvMacG3AdpSourceFile.setEnabled(True)
    
    # Add to definitions.h
    MacG3AdpSystemDefFile = drvMacG3AdpComponent.createFileSymbol("DRV_MAC_G3ADP_DEFINITIONS_FILE", None)
    MacG3AdpSystemDefFile.setType("STRING")
    MacG3AdpSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    MacG3AdpSystemDefFile.setSourcePath("g3/net/macg3adp/templates/system/system_definitions.h.ftl")
    MacG3AdpSystemDefFile.setMarkup(True)  

def onAttachmentConnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)

        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), True)
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")
        incVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_CONFIG_MULTICAST_IDX" + str(interface_number), True)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX" + str(interface_number), True)
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX" + str(interface_number), "fd00::781D:00:1122:3344:5566")
        setVal("tcpipNetConfig_" + str(interface_number), "TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX" + str(interface_number), "fe80::781D:ff:fe00:0000")
    #elif (source["id"] == "g3_adp_dependency"): 
        # G3 ADP configuration
    
        
def onAttachmentDisconnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")
        decVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        
        # setVal("tcpipNetConfig", "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX" + str(interface_number), False)
        # setVal("tcpipNetConfig", "TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX" + str(interface_number), "")
        # setVal("tcpipNetConfig", "TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX" + str(interface_number), "")
    #elif (source["id"] == "g3_adp_dependency"): 
        # G3 ADP configuration
        

# def drvMacMPUConfig(symbol, event):
#     global drvGmacNoCacheMemRegSize
#     global noCache_MPU_index
#     coreComponent = Database.getComponentByID("core")
    
#     if (noCache_MPU_index != 0xff): 
#         if(Database.getSymbolValue("drvGmac", "DRV_GMAC_NO_CACHE_CONFIG") == True):
#             Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), True)
#             Database.setSymbolValue("core", ("MPU_Region_Name" + str(noCache_MPU_index)), "GMAC Descriptor")
#             Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Address"), Database.getSymbolValue("drvGmac", "DRV_GMAC_NOCACHE_MEM_ADDRESS"))
#             index = drvGmacNoCacheMemRegSize.getValue()
#             keyval = drvGmacNoCacheMemRegSize.getKeyValue(index)
#             key = drvGmacNoCacheMemRegSize.getKeyForValue(keyval)
#             setVal("tcpipStack", "TCPIP_STACK_NOCACHE_MEM_ADDRESS", Database.getSymbolValue("drvGmac", "DRV_GMAC_NOCACHE_MEM_ADDRESS"))
#             setVal("tcpipStack", "TCPIP_STACK_NOCACHE_SIZE", key)
#             setVal("tcpipStack", "TCPIP_STACK_NO_CACHE_CONFIG", True)
#             mpuRegSize = coreComponent.getSymbolByID("MPU_Region_" + str(noCache_MPU_index) + "_Size")
#             mpuRegSize.setSelectedKey(key)
#         else:
#             Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), False)
#             setVal("tcpipStack", "TCPIP_STACK_NO_CACHE_CONFIG", False)

# def initNoCacheMPU():
#     global noCache_MPU_index
#     if (noCache_MPU_index == 0xff): 
#         # Enable MPU Setting for GMAC descriptor
#         if(Database.getSymbolValue("core", "CoreUseMPU") != True):
#             Database.setSymbolValue("core", "CoreUseMPU", True)
#         if(Database.getSymbolValue("core", "CoreMPU_DEFAULT") != True):  
#             Database.setSymbolValue("core", "CoreMPU_DEFAULT", True)            
#         mpuNumRegions = Database.getSymbolValue("core", "MPU_NUMBER_REGIONS")
        
#         for i in range(0,mpuNumRegions):
#             if(Database.getSymbolValue("core", ("MPU_Region_" + str(i) + "_Enable")) != True):
#                 noCache_MPU_index = i
#                 Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), True)
#                 Database.setSymbolValue("core", ("MPU_Region_Name" + str(noCache_MPU_index)), "GMAC Descriptor")
#                 Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Type"), 5)
#                 Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Size"), 7)
#                 Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Access"), 3)
#                 Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Address"), Database.getSymbolValue("drvGmac", "DRV_GMAC_NOCACHE_MEM_ADDRESS"))
#                 break
                


# def destroyComponent(drvMacG3AdpComponent):
#     global gmac_periphID
#     global noCache_MPU_index
#     Database.setSymbolValue("drvGmac", "TCPIP_USE_ETH_MAC", False, 2)    
#     setVal("core", "GMAC_INTERRUPT_ENABLE", False)
#     if(gmac_periphID == "11046") or (gmac_periphID == "44152"): # SAME70 or SAMV71 or SAMA5D2
#         setVal("core", "GMAC_Q1_INTERRUPT_ENABLE", False)
#         setVal("core", "GMAC_Q2_INTERRUPT_ENABLE", False)
#     if(gmac_periphID == "11046"): # SAME70, SAMV71, SAMRH71   
#         setVal("core", "GMAC_Q3_INTERRUPT_ENABLE", False)
#         setVal("core", "GMAC_Q4_INTERRUPT_ENABLE", False)
#         setVal("core", "GMAC_Q5_INTERRUPT_ENABLE", False)
#         setVal("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), False)

