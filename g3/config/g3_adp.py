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

def instantiateComponent(g3AdpComponent):
    
    Log.writeInfoMessage("Loading ADP for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    #### Library Files ######################################################

    adpLibFile = g3AdpComponent.createLibrarySymbol("G3_ADP_LIBRARY", None)
    adpLibFile.setSourcePath("g3/libs/g3_lib_adp.a")
    adpLibFile.setOutputName("g3_lib_adp.a")
    adpLibFile.setDestPath("stack/g3/adaptation")
    adpLibFile.setEnabled(True)

    #### ADP header Files ######################################################
    adpHeader = g3AdpComponent.createFileSymbol("G3_ADP_HEADER", None)
    adpHeader.setSourcePath("g3/src/adp/adp.h")
    adpHeader.setOutputName("adp.h")
    adpHeader.setDestPath("stack/g3/adaptation")
    adpHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpHeader.setType("HEADER")

    adpApiTypesHeader = g3AdpComponent.createFileSymbol("G3_ADP_API_TYPES_HEADER", None)
    adpApiTypesHeader.setSourcePath("g3/src/adp/adp_api_types.h")
    adpApiTypesHeader.setOutputName("adp_api_types.h")
    adpApiTypesHeader.setDestPath("stack/g3/adaptation")
    adpApiTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpApiTypesHeader.setType("HEADER")

    adpSharedTypesHeader = g3AdpComponent.createFileSymbol("G3_ADP_SHARED_TYPES_HEADER", None)
    adpSharedTypesHeader.setSourcePath("g3/src/adp/adp_shared_types.h")
    adpSharedTypesHeader.setOutputName("adp_shared_types.h")
    adpSharedTypesHeader.setDestPath("stack/g3/adaptation")
    adpSharedTypesHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    adpSharedTypesHeader.setType("HEADER")
