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

def instantiateComponent(g3LbpComponent):
    
    Log.writeInfoMessage("Loading LBP for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
"""     configName = Variables.get("__CONFIGURATION_NAME")
    
    component = Database.getComponentByID("g3_adapt_config")
    Log.writeInfoMessage("------------------------------------------------------------------")
    Log.writeInfoMessage("------------------------------------------------------------------")
    Log.writeInfoMessage("DSY")
    Log.writeInfoMessage(str(component))
    Log.writeInfoMessage("")
    Log.writeInfoMessage("")
    Log.writeInfoMessage("G3_DEVICE Value:")
    Log.writeInfoMessage(str(component.getSymbolValue("G3_DEVICE")))
    Log.writeInfoMessage("G3_COORDINATOR Value:")
    Log.writeInfoMessage(str(component.getSymbolValue("G3_COORDINATOR")))
    devCapable = False
    coordCapable = False
    if (component.getSymbolValue("G3_DEVICE") == True):
        devCapable = True
    if (component.getSymbolValue("G3_COORDINATOR") == True):
        coordCapable = True
    
    Log.writeInfoMessage("------------------------------------------------------------------")
    Log.writeInfoMessage("------------------------------------------------------------------")

    # LBP API
    if coordCapable:
        pLbpCoordHeader = g3LbpComponent.createFileSymbol("LBP_COORD_HEADER", None)
        pLbpCoordHeader.setSourcePath("g3/src/lbp/include/lbp_coord.h")
        pLbpCoordHeader.setOutputName("lbp_coord.h")
        pLbpCoordHeader.setDestPath("stack/g3/adaptation")
        pLbpCoordHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
        pLbpCoordHeader.setType("HEADER")

    if devCapable:
        pLbpDevHeader = g3LbpComponent.createFileSymbol("LBP_DEV_HEADER", None)
        pLbpDevHeader.setSourcePath("g3/src/lbp/include/lbp_dev.h")
        pLbpDevHeader.setOutputName("lbp_dev.h")
        pLbpDevHeader.setDestPath("stack/g3/adaptation")
        pLbpDevHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
        pLbpDevHeader.setType("HEADER")

    pLbpDefsHeader = g3LbpComponent.createFileSymbol("LBP_DEFS_HEADER", None)
    pLbpDefsHeader.setSourcePath("g3/src/lbp/include/lbp_defs.h")
    pLbpDefsHeader.setOutputName("lbp_defs.h")
    pLbpDefsHeader.setDestPath("stack/g3/adaptation")
    pLbpDefsHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpDefsHeader.setType("HEADER")
    
    # LBP Files
    pLbpEapSource = g3LbpComponent.createFileSymbol("EAP_SOURCE", None)
    pLbpEapSource.setSourcePath("g3/src/lbp/source/eap_psk.c")
    pLbpEapSource.setOutputName("eap_psk.c")
    pLbpEapSource.setDestPath("stack/g3/adaptation")
    pLbpEapSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEapSource.setType("SOURCE")
    
    pLbpEapHeader = g3LbpComponent.createFileSymbol("EAP_HEADER", None)
    pLbpEapHeader.setSourcePath("g3/src/lbp/source/eap_psk.h")
    pLbpEapHeader.setOutputName("eap_psk.h")
    pLbpEapHeader.setDestPath("stack/g3/adaptation")
    pLbpEapHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEapHeader.setType("HEADER")

    pLbpEncodeDecodeSource = g3LbpComponent.createFileSymbol("LBP_ENCODE_DECODE_SOURCE", None)
    pLbpEncodeDecodeSource.setSourcePath("g3/src/lbp/source/lbp_encode_decode.c")
    pLbpEncodeDecodeSource.setOutputName("lbp_encode_decode.c")
    pLbpEncodeDecodeSource.setDestPath("stack/g3/adaptation")
    pLbpEncodeDecodeSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEncodeDecodeSource.setType("SOURCE")

    pLbpEncodeDecodeHeader = g3LbpComponent.createFileSymbol("LBP_ENCODE_DECODE_HEADER", None)
    pLbpEncodeDecodeHeader.setSourcePath("g3/src/lbp/source/lbp_encode_decode.h")
    pLbpEncodeDecodeHeader.setOutputName("lbp_encode_decode.h")
    pLbpEncodeDecodeHeader.setDestPath("stack/g3/adaptation")
    pLbpEncodeDecodeHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpEncodeDecodeHeader.setType("HEADER")

    if coordCapable:
        pLbpCoordSource = g3LbpComponent.createFileSymbol("LBP_COORD_SOURCE", None)
        pLbpCoordSource.setSourcePath("g3/src/lbp/source/lbp_coord.c")
        pLbpCoordSource.setOutputName("lbp_coord.c")
        pLbpCoordSource.setDestPath("stack/g3/adaptation")
        pLbpCoordSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
        pLbpCoordSource.setType("SOURCE")

    if devCapable:
        pLbpDevSource = g3LbpComponent.createFileSymbol("LBP_DEV_SOURCE", None)
        pLbpDevSource.setSourcePath("g3/src/lbp/source/lbp_dev.c")
        pLbpDevSource.setOutputName("lbp_dev.c")
        pLbpDevSource.setDestPath("stack/g3/adaptation")
        pLbpDevSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
        pLbpDevSource.setType("SOURCE")

    pLbpVersionHeader = g3LbpComponent.createFileSymbol("LBP_VERSION_HEADER", None)
    pLbpVersionHeader.setSourcePath("g3/src/lbp/source/lbp_version.h")
    pLbpVersionHeader.setOutputName("lbp_version.h")
    pLbpVersionHeader.setDestPath("stack/g3/adaptation")
    pLbpVersionHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pLbpVersionHeader.setType("HEADER") """