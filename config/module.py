"""
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""

###################### G3 STACK  ######################
def loadModule():

    print("Load Module: Harmony Smart Energy G3 Stack")
    processor = Variables.get("__PROCESSOR")

    if ("WBZ45" in processor) or ("PIC32CX1012BZ" in processor):
        ## G3 PAL RF for WBZ45
        print("G3: Loading PAL RF for WBZ45")
        g3PalRfComponent = Module.CreateComponent("g3PalRf", "G3 PAL RF", "/SmartEnergy/G3 Stack/PAL", "pal/rf/config/pal_wbz45.py")
        g3PalRfComponent.addCapability("g3PalRf", "G3 PAL RF", True)
        g3PalRfComponent.addDependency("g3PalRf_DrvRfPhy_dependency", "Device_Support", True, True)
        g3PalRfComponent.setDisplayType("G3 PAL RF")
    else:
        ## G3 PAL RF for RF215
        print("G3: Loading PAL RF for RF215")
        g3PalRfComponent = Module.CreateComponent("g3PalRf", "G3 PAL RF", "/SmartEnergy/G3 Stack/PAL", "pal/rf/config/pal_rf215.py")
        g3PalRfComponent.addCapability("g3PalRf", "G3 PAL RF", True)
        g3PalRfComponent.addDependency("g3PalRf_DrvRfPhy_dependency", "DRV_RF_PHY", True, True)
        g3PalRfComponent.addDependency("g3PalRf_sysTime_dependency", "SYS_TIME", True, True)
        g3PalRfComponent.setDisplayType("G3 PAL RF")

    ## G3 PAL PLC
    g3PalPlcComponent = Module.CreateComponent("g3PalPlc", "G3 PAL PLC", "/SmartEnergy/G3 Stack/PAL", "pal/plc/config/pal_plc.py")
    g3PalPlcComponent.addCapability("g3PalPlc", "G3 PAL PLC", True)
    g3PalPlcComponent.addDependency("g3PalPlc_G3MacRT_dependency", "DRV_G3_MAC_RT", True, True)
    g3PalPlcComponent.addDependency("g3PalPlc_PCoup_dependency", "PCOUP", True, True)
    g3PalPlcComponent.setDisplayType("G3 PAL PLC")

    ###########  G3 Stack Configurations  ###########
    global g3ConfigComponent
    g3ConfigComponent = Module.CreateComponent("g3_config", "G3 Stack", "/SmartEnergy/G3 Stack", "g3/config/g3_configurator.py")
    g3ConfigComponent.setDisplayType("G3 Stack")
    g3ConfigComponent.addCapability("g3StackCapability", "G3 Stack", True)
    g3ConfigComponent.addDependency("g3_srv_random_dependency", "SE Random", True, True)
    g3ConfigComponent.addDependency("g3_srv_log_report_dependency", "SE Log Report", True, True)
    g3ConfigComponent.addDependency("g3_srv_security_dependency", "SE Security", True, True)
    g3ConfigComponent.addDependency("g3_srv_queue_dependency", "SE Queue", True, True)
    g3ConfigComponent.addDependency("g3_palplc_dependency", "G3 PAL PLC", True, False)
    g3ConfigComponent.addDependency("g3_palrf_dependency", "G3 PAL RF", True, False)

    ## ADP driver to be used as MAC interface with TCP-IP stack
    g3MacAdpComponent = Module.CreateComponent("drvMacG3Adp", "G3ADPMAC", "/SmartEnergy/G3 Stack/", "g3/net/macg3adp/config/drv_mac_g3adp.py")
    g3MacAdpComponent.addMultiCapability("libdrvMacG3Adp", "MAC", None)
    g3MacAdpComponent.addDependency("g3_adp_dependency", "G3 Stack", True, True)
    g3MacAdpComponent.addDependency("srv_queue_dependency", "SE Queue", True, True)
    # TBD optional dependencies to serialization and storage
    g3MacAdpComponent.setDisplayType("MAC Layer")

