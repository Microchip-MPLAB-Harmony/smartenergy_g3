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

    ## G3 PAL PLC
    g3PalPlcComponent = Module.CreateComponent("g3PalPlc", "G3 PAL PLC", "/SmartEnergy/G3 Stack/PAL", "pal/plc/config/pal_plc.py")
    g3PalPlcComponent.addCapability("g3PalPlc", "G3_PAL_PLC", True)
    g3PalPlcComponent.addDependency("g3PalPlc_G3MacRT_dependency", "DRV_G3_MAC_RT", False, True)
    g3PalPlcComponent.addDependency("g3PalPlc_PCoup_dependency", "PCOUP", True, True)
    g3PalPlcComponent.setDisplayType("G3 PAL PLC")

    ## G3 PAL RF
    g3PalRfComponent = Module.CreateComponent("g3PalRf", "G3 PAL RF", "/SmartEnergy/G3 Stack/PAL", "pal/rf/config/pal_rf.py")
    g3PalRfComponent.addCapability("g3PalRf", "G3_PAL_RF", True)
    g3PalRfComponent.addDependency("g3PalRf_DrvRfPhy_dependency", "DRV_RF_PHY", False, True)
    g3PalRfComponent.addDependency("g3PalRf_sysTime_dependency", "SYS_TIME", True, True)
    g3PalRfComponent.setDisplayType("G3 PAL RF")

    ###########  G3 Stack Configurations  ###########
    g3ConfigComponent = Module.CreateComponent("g3_config", "G3 Stack Configurator", "/SmartEnergy/G3 Stack", "g3/config/g3_configurator.py")
    g3ConfigComponent.setDisplayType("G3 Stack Configurator")
    
    ###########  G3 Stack Adaptation Layer Configurations  ###########
    g3ConfigAdaptComponent = Module.CreateComponent("g3_adapt_config", "G3 Adaptation Layer Configurator", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 2 - Adaptation", "g3/config/g3_configurator_adapt.py")
    g3ConfigAdaptComponent.setDisplayType("Adaptation Configurator")

    ## ADP
    g3AdpComponent = Module.CreateComponent("g3ADP", "ADP", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 2 - Adaptation", "g3/config/g3_adp.py")
    g3AdpComponent.addCapability("libADP", "ADP", True)
    g3AdpComponent.addDependency("adp_loadng_dependency", "LOADng", True, True)
    g3AdpComponent.addDependency("adp_bootstrap_dependency", "Bootstrap", True, True)
    g3AdpComponent.addDependency("adp_mac_wrapper_dependency", "MAC Wrapper", True, True)
    g3AdpComponent.addDependency("adp_srv_random_dependency", "Random", True, True)
    g3AdpComponent.addDependency("adp_srv_log_report_dependency", "Log Report", True, True)
    g3AdpComponent.addDependency("adp_sys_time_dependency", "SYS_TIME", True, True)
    g3AdpComponent.addDependency("adp_srv_timemgmt_dependency", "Time Management", True, True)
    # TBD optional dependencies to serialization and storage
    g3AdpComponent.setDisplayType("Adaptation Layer")
    
    ## LOADng
    g3LOADngComponent = Module.CreateComponent("g3LOADng", "LOADng", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 2 - Adaptation", "g3/config/g3_loadng.py")
    g3LOADngComponent.addCapability("libLOADng", "LOADng", True)
    g3LOADngComponent.addDependency("loadng_mac_wrapper_dependency", "MAC Wrapper", True, True)
    g3LOADngComponent.addDependency("loadng_srv_random_dependency", "Random", True, True)
    g3LOADngComponent.addDependency("loadng_srv_queue_dependency", "Queue", True, True)
    g3LOADngComponent.addDependency("loadng_srv_log_report_dependency", "Log Report", True, True)
    g3LOADngComponent.addDependency("loadng_sys_time_dependency", "SYS_TIME", True, True)
    g3LOADngComponent.addDependency("loadng_srv_timemgmt_dependency", "Time Management", True, True)
    # TBD optional dependencies to serialization and storage
    g3LOADngComponent.setDisplayType("Adaptation Layer")
    
    ## LBP
    g3LbpComponent = Module.CreateComponent("g3LBP", "LBP", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 2 - Adaptation", "g3/config/g3_lbp.py")
    g3LbpComponent.addCapability("libLBP", "LBP", True)
    g3LbpComponent.addDependency("lbp_srv_random_dependency", "Random", True, True)
    g3LbpComponent.addDependency("lbp_srv_security_dependency", "Security", True, True)
    g3LbpComponent.addDependency("lbp_sys_time_dependency", "SYS_TIME", True, True)
    g3LbpComponent.addDependency("lbp_srv_timemgmt_dependency", "Time Management", True, True)
    g3LbpComponent.setDisplayType("Adaptation Layer")

    ###########  G3 Stack MAC Layer Configurations  ###########
    g3ConfigMacComponent = Module.CreateComponent("g3_mac_config", "G3 MAC Layer Configurator", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 1 - MAC", "g3/config/g3_configurator_mac.py")
    g3ConfigMacComponent.setDisplayType("MAC Configurator")
    
    ## MAC Wrapper
    g3MacWrpComponent = Module.CreateComponent("g3MacWrapper", "MAC Wrapper", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 1 - MAC", "g3/config/g3_mac_wrapper.py")
    g3MacWrpComponent.addCapability("libMacWrapper", "MAC Wrapper", True)
    g3MacWrpComponent.addDependency("macwrp_mac_common_dependency", "MAC Common", True, True)
    g3MacWrpComponent.addDependency("macwrp_mac_plc_dependency", "MAC PLC", True, False)
    g3MacWrpComponent.addDependency("macwrp_mac_rf_dependency", "MAC RF", True, False)
    # TBD optional dependencies to logger and serialization
    g3MacWrpComponent.setDisplayType("MAC Layer")
    
    ## MAC Common
    g3MacCommonComponent = Module.CreateComponent("g3MacCommon", "MAC Common", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 1 - MAC", "g3/config/g3_mac_common.py")
    g3MacCommonComponent.addCapability("libMacCommon", "MAC Common", True)
    # TBD optional dependencies to logger    
    g3MacCommonComponent.setDisplayType("MAC Layer")
    
    ## MAC PLC
    g3MacPlcComponent = Module.CreateComponent("g3MacPlc", "MAC PLC", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 1 - MAC", "g3/config/g3_mac_plc.py")
    g3MacPlcComponent.addCapability("libPlcMac", "MAC PLC", True)
    g3MacPlcComponent.addDependency("mac_plc_common_dependency", "MAC Common", True, True)
    g3MacPlcComponent.addDependency("mac_plc_wrapper_dependency", "MAC Wrapper", True, True)
    g3MacPlcComponent.addDependency("plc_srv_random_dependency", "Random", True, True)
    g3MacPlcComponent.addDependency("plc_srv_log_report_dependency", "Log Report", True, True)
    g3MacPlcComponent.addDependency("plc_srv_security_dependency", "Security", True, True)
    g3MacPlcComponent.addDependency("plc_srv_g3_palplc_dependency", "G3_PAL_PLC", True, True)
    g3MacPlcComponent.addDependency("plc_srv_timemgmt_dependency", "Time Management", True, True)
    # TBD optional dependencies to storage
    g3MacPlcComponent.setDisplayType("MAC Layer")
    
    ## MAC RF
    g3MacRfComponent = Module.CreateComponent("g3MacRf", "MAC RF", "/SmartEnergy/G3 Stack/Layer 2 Sublayer 1 - MAC", "g3/config/g3_mac_rf.py")
    g3MacRfComponent.addCapability("libRfMac", "MAC RF", True)
    g3MacRfComponent.addDependency("mac_rf_common_dependency", "MAC Common", True, True)
    g3MacRfComponent.addDependency("mac_rf_wrapper_dependency", "MAC Wrapper", True, True)
    g3MacRfComponent.addDependency("rf_srv_random_dependency", "Random", True, True)
    g3MacRfComponent.addDependency("rf_srv_log_report_dependency", "Log Report", True, True)
    g3MacRfComponent.addDependency("rf_srv_security_dependency", "Security", True, True)
    g3MacRfComponent.addDependency("rf_srv_g3_palrf_dependency", "G3_PAL_RF", True, True)
    g3MacRfComponent.addDependency("rf_srv_timemgmt_dependency", "Time Management", True, True)
    # TBD optional dependencies to storage
    g3MacRfComponent.setDisplayType("MAC Layer")
    
