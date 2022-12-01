# coding: utf-8
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
g3_pal_rf_helpkeyword = "mcc_g3_pal_rf_config"


def instantiateComponent(g3PalRfComponent):
    
    Log.writeInfoMessage("Loading G3 PAL RF module")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    g3PalRfDevice = g3PalRfComponent.createStringSymbol("G3_PAL_RF_DEVICE", None)
    g3PalRfDevice.setLabel("G3 PAL RF Device")
    g3PalRfDevice.setReadOnly(True)
    g3PalRfDevice.setHelp(g3_pal_rf_helpkeyword)

    global g3PalRfConfigComment
    g3PalRfConfigComment = g3PalRfComponent.createCommentSymbol("G3_PAL_RF_CONFIG_COMMENT", None)
    g3PalRfConfigComment.setLabel("***Selected RF PHY driver is not compatible with this PAL RF***")
    g3PalRfConfigComment.setVisible(False)
    

    #####################################################################################################################################
    # G3 PAL RF FILES 

    global g3PalRfSrcFile
    g3PalRfSrcFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SOURCE", None)
    g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_rf215.c")
    g3PalRfSrcFile.setOutputName("pal_rf.c")
    g3PalRfSrcFile.setDestPath("stack/g3/pal/rf")
    g3PalRfSrcFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfSrcFile.setType("SOURCE")
    g3PalRfSrcFile.setMarkup(False)

    global g3PalRfHdrFile
    g3PalRfHdrFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_HEADER", None)
    g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_rf215.h")
    g3PalRfHdrFile.setOutputName("pal_rf.h")
    g3PalRfHdrFile.setDestPath("stack/g3/pal/rf")
    g3PalRfHdrFile.setProjectPath("config/" + configName + "/stack/g3/pal/rf/")
    g3PalRfHdrFile.setType("HEADER")
    g3PalRfHdrFile.setMarkup(False)

    #####################################################################################################################################
    # G3 PAL RF TEMPLATES 
    
    g3PalRfSystemConfigFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_CONFIGURATION", None)
    g3PalRfSystemConfigFile.setType("STRING")
    g3PalRfSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    g3PalRfSystemConfigFile.setSourcePath("pal/rf/templates/system/configuration.h.ftl")
    g3PalRfSystemConfigFile.setMarkup(True)

    g3PalRfSystemDefFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_DEF", None)
    g3PalRfSystemDefFile.setType("STRING")
    g3PalRfSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    g3PalRfSystemDefFile.setSourcePath("pal/rf/templates/system/definitions.h.ftl")
    g3PalRfSystemDefFile.setMarkup(True)

    g3PalRfSymSystemDefObjFile = g3PalRfComponent.createFileSymbol("G3_PAL_RF_SYSTEM_DEF_OBJECT", None)
    g3PalRfSymSystemDefObjFile.setType("STRING")
    g3PalRfSymSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    g3PalRfSymSystemDefObjFile.setSourcePath("pal/rf/templates/system/definitions_objects.h.ftl")
    g3PalRfSymSystemDefObjFile.setMarkup(True)

def onAttachmentConnected(source, target):
    global g3PalRfSrcFile
    global g3PalRfHdrFile
    localComponent = source["component"]
    localConnectID = source["id"]
    remoteComponent = target["component"]
    remoteComponentID = remoteComponent.getID()

    if localConnectID == "g3PalRf_DrvRfPhy_dependency":
        deviceUsed = localComponent.getSymbolByID("G3_PAL_RF_DEVICE")
        deviceUsed.setValue(remoteComponentID.upper())
        if (remoteComponentID.upper() == "DRVRF215"):
            g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_rf215.c")
            g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_rf215.h")
            g3PalRfConfigComment.setVisible(False)
        elif (remoteComponentID.upper() == "WBZ45"):  ########################      TBD!!!!!!!!!!!!!!!!
            g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_wbz45.c")
            g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_wbz45.h")
            g3PalRfConfigComment.setVisible(False)
        else:
            g3PalRfConfigComment.setVisible(True)
                
def onAttachmentDisconnected(source, target):
    global g3PalRfSrcFile
    global g3PalRfHdrFile
    localComponent = source["component"]
    localConnectID = source["id"]
    remoteComponent = target["component"]
    remoteComponentID = remoteComponent.getID()

    if localConnectID == "g3PalRf_DrvRfPhy_dependency":
        localComponent.getSymbolByID("G3_PAL_RF_DEVICE").clearValue()
        g3PalRfSrcFile.setSourcePath("pal/rf/src/pal_rf_rf215.c")
        g3PalRfHdrFile.setSourcePath("pal/rf/pal_rf_rf215.h")
        g3PalRfConfigComment.setVisible(False)

    