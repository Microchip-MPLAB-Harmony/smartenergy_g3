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

def instantiateComponent(g3BootstrapComponent):
    
    Log.writeInfoMessage("Loading Bootstrap for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Bootstrap Files
    pBootstrapSource = g3BootstrapComponent.createFileSymbol("BOOTSTRAP_SOURCE", None)
    pBootstrapSource.setSourcePath("g3/src/bootstrap/bootstrap.c")
    pBootstrapSource.setOutputName("bootstrap.c")
    pBootstrapSource.setDestPath("stack/g3/")
    pBootstrapSource.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pBootstrapSource.setType("SOURCE")
    
    pBootstrapHeader = g3BootstrapComponent.createFileSymbol("BOOTSTRAP_HEADER", None)
    pBootstrapHeader.setSourcePath("g3/src/bootstrap/bootstrap.h")
    pBootstrapHeader.setOutputName("bootstrap.h")
    pBootstrapHeader.setDestPath("stack/g3")
    pBootstrapHeader.setProjectPath("config/" + configName + "/stack/g3/adaptation")
    pBootstrapHeader.setType("HEADER")