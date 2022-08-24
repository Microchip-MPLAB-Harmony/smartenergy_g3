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

###################### G3 STACK  ######################
def loadModule():
    print("Load Module: Harmony Smart Energy G3 Stack")

    ###########  G3 Stack MAC Layer Configurations  ###########
    ## MAC Wrapper
    g3MacWrpComponent = Module.CreateComponent("g3MacWrapper", "MAC Wrapper", "/SmartEnergy/Stack/G3/MAC Layer", "g3/config/g3_mac_wrapper.py")
    g3MacWrpComponent.addCapability("libMacWrapper", "MAC Wrapper") 
    g3MacWrpComponent.setDisplayType("MAC Layer")
    
    ## MAC Common
    g3MacCommonComponent = Module.CreateComponent("g3MacCommon", "MAC Common", "/SmartEnergy/Stack/G3/MAC Layer", "g3/config/g3_mac_common.py")
    g3MacCommonComponent.addCapability("libMacCommon", "MAC Common")    
    g3MacCommonComponent.addDependency("mac_wrp_mac_common_dependency", "MAC Wrapper", False, True)
    g3MacCommonComponent.setDisplayType("MAC Layer")
    
    ## PLC MAC
    g3PlcMacComponent = Module.CreateComponent("g3PlcMac", "PLC MAC", "/SmartEnergy/Stack/G3/MAC Layer", "g3/config/g3_mac_plc.py")
    g3PlcMacComponent.addCapability("libPlcMac", "PLC MAC") 
    g3PlcMacComponent.addDependency("plc_mac_common_dependency", "MAC Common", False, True)
    g3PlcMacComponent.addDependency("plc_mac_wrapper_dependency", "MAC Wrapper", False, True)
    g3PlcMacComponent.addDependency("plc_srv_random_dependency", "Random", False, True)
    g3PlcMacComponent.addDependency("plc_srv_security_dependency", "SRV_SECURITY", False, True)
    g3PlcMacComponent.addDependency("plc_srv_g3_macrt_dependency", "G3_MAC_RT", False, True)
    g3PlcMacComponent.setDisplayType("MAC Layer")
    
    ## RF MAC
    g3RfMacComponent = Module.CreateComponent("g3RfMac", "RF MAC", "/SmartEnergy/Stack/G3/MAC Layer", "g3/config/g3_mac_rf.py")
    g3RfMacComponent.addCapability("libRfMac", "RF MAC")    
    g3RfMacComponent.addDependency("rf_mac_common_dependency", "MAC Common", False, True)
    g3RfMacComponent.addDependency("rf_mac_wrapper_dependency", "MAC Wrapper", False, True)
    g3RfMacComponent.addDependency("rf_srv_random_dependency", "Random", False, True)
    g3RfMacComponent.addDependency("rf_srv_security_dependency", "SRV_SECURITY", False, True)
    ##g3RfMacComponent.addDependency("rf_srv_g3_pal215_dependency", "G3_PAL_RF", False, True)   ## TBD
    g3RfMacComponent.setDisplayType("MAC Layer")
    
    ###########  G3 LIBRARY Adaptation Layer Configurations  ###########
    ## ADP
    g3AdpComponent = Module.CreateComponent("g3ADP", "ADP", "/SmartEnergy/Stack/G3/Adaptation Layer", "g3/config/g3_adp.py")
    g3AdpComponent.addCapability("libADP", "ADP")   
    g3AdpComponent.addDependency("adp_loadng_dependency", "LOADng", False, True)
    g3AdpComponent.addDependency("adp_srv_random_dependency", "Random", False, True)
    g3AdpComponent.addDependency("adp_srv_security_dependency", "SRV_SECURITY", False, True)
    g3AdpComponent.setDisplayType("Adaptation Layer")
    
    ## LOADng
    g3LOADngComponent = Module.CreateComponent("g3LOADng", "LOADng", "/SmartEnergy/Stack/G3/Adaptation Layer", "g3/config/g3_loadng.py")
    g3LOADngComponent.addCapability("libLOADng", "LOADNG")  
    g3LOADngComponent.addDependency("loadng_srv_random_dependency", "Random", False, True)
    g3LOADngComponent.addDependency("loadng_srv_queue_dependency", "Queue", False, True)
    g3LOADngComponent.setDisplayType("Adaptation Layer")
    
    ## Bootstrap
    g3BootstrapComponent = Module.CreateComponent("g3Bootstrap", "Bootstrap", "/SmartEnergy/Stack/G3/Adaptation Layer", "g3/config/srv_bootstrap.py")
    g3BootstrapComponent.addCapability("libBootstrap", "Bootstrap") 
    g3BootstrapComponent.addDependency("bootstrap_srv_random_dependency", "Random", False, True)
    g3BootstrapComponent.setDisplayType("Adaptation Layer")
    
    ###########  G3 Coordinator Configurations  ###########
    g3CoordinatorComponent = Module.CreateComponent("g3Coordinator", "Coordinator", "/SmartEnergy/Stack/G3/Coordinator", "g3/config/g3_coordinator.py")
    g3CoordinatorComponent.addCapability("libCoordinator", "Coordinator")   
    g3CoordinatorComponent.setDisplayType("Coordinator")
    
    ###########  G3 Stack Services Configurations  ###########   
    ## Security
    srvSecurityComponent = Module.CreateComponent("srvSecurity", "Security", "/SmartEnergy/Stack/Services", "service/security/config/srv_security.py")
    srvSecurityComponent.addCapability("libsrvSecurity", "SRV_SECURITY", True)  
    srvSecurityComponent.addDependency("security_crypto_dependency", "LIB_CRYPTO", True, True)
    srvSecurityComponent.setDisplayType("Security Service")
    
    ############################### G3 STACK CONFIGURATOR #####################################

    g3AutoConfigMacComponent = Module.CreateComponent("g3_mac_config", "G3 MAC Layer Configurator", "/SmartEnergy/Stack/G3/MAC Layer/", "g3/config/g3_configurator_mac.py")
    g3AutoConfigMacComponent.setDisplayType("MAC Configurator")
    
    g3AutoConfigPlcComponent = Module.CreateComponent("g3_mac_plc_config", "G3 PLC MAC Configurator", "/SmartEnergy/Stack/G3/MAC Layer/", "g3/config/g3_configurator_mac_plc.py")
    g3AutoConfigPlcComponent.setDisplayType("PLC MAC Configurator")
    
    g3AutoConfigRfComponent = Module.CreateComponent("g3_mac_rf_config", "G3 RF MAC Configurator", "/SmartEnergy/Stack/G3/MAC Layer/", "g3/config/g3_configurator_mac_rf.py")
    g3AutoConfigRfComponent.setDisplayType("RF MAC Configurator")
    
    g3AutoConfigAdaptComponent = Module.CreateComponent("g3_adapt_config", "G3 Adaptation Layer Configurator", "/SmartEnergy/Stack/G3/Adaptation Layer/", "g3/config/g3_configurator_adapt.py")
    g3AutoConfigAdaptComponent.setDisplayType("Adaptation Configurator")   