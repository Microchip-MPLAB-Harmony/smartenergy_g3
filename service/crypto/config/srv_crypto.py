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

def instantiateComponent(srvCryptoComponent):
    
    Log.writeInfoMessage("Loading Crypto service for G3")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Crypto Files
    pCryptoAesWrpSource = srvCyptoComponent.createFileSymbol("SRV_CRYPTO_AES_WRP_SOURCE", None)
    pCryptoAesWrpSource.setSourcePath("service/crypto/src/aes_wrapper.c")
    pCryptoAesWrpSource.setOutputName("aes_wrapper.c")
    pCryptoAesWrpSource.setDestPath("stack/service/crypto")
    pCryptoAesWrpSource.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoAesWrpSource.setType("SOURCE")
    
    pCryptoAesWrpHeader = srvCyptoComponent.createFileSymbol("SRV_CRYPTO_AES_WRP_HEADER", None)
    pCryptoAesWrpHeader.setSourcePath("service/crypto/aes_wrapper.h")
    pCryptoAesWrpHeader.setOutputName("aes_wrapper.h")
    pCryptoAesWrpHeader.setDestPath("stack/service/crypto")
    pCryptoAesWrpHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoAesWrpHeader.setType("HEADER")
    
    pCryptoCipherWrpSource = srvCyptoComponent.createFileSymbol("SRV_CRYPTO_CIPHER_WRP_SOURCE", None)
    pCryptoCipherWrpSource.setSourcePath("service/crypto/src/cipher_wrapper.c")
    pCryptoCipherWrpSource.setOutputName("cipher_wrapper.c")
    pCryptoCipherWrpSource.setDestPath("stack/service/crypto")
    pCryptoCipherWrpSource.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoCipherWrpSource.setType("SOURCE")
    
    pCryptoCipherWrpHeader = srvCyptoComponent.createFileSymbol("SRV_CRYPTO_CIPHER_WRP_HEADER", None)
    pCryptoCipherWrpHeader.setSourcePath("service/crypto/cipher_wrapper.h")
    pCryptoCipherWrpHeader.setOutputName("cipher_wrapper.h")
    pCryptoCipherWrpHeader.setDestPath("stack/service/crypto")
    pCryptoCipherWrpHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoCipherWrpHeader.setType("HEADER")
    
    pCryptoEaxSource = srvCryptoComponent.createFileSymbol("SRV_CRYPTO_EAX_SOURCE", None)
    pCryptoEaxSource.setSourcePath("service/crypto/src/eax.c")
    pCryptoEaxSource.setOutputName("eax.c")
    pCryptoEaxSource.setDestPath("stack/service/crypto")
    pCryptoEaxSource.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoEaxSource.setType("SOURCE")
    
    pCryptoEaxHeader = srvCryptoComponent.createFileSymbol("SRV_CRYPTO_EAX_HEADER", None)
    pCryptoEaxHeader.setSourcePath("service/crypto/eax.h")
    pCryptoEaxHeader.setOutputName("aex.h")
    pCryptoEaxHeader.setDestPath("stack/service/crypto")
    pCryptoEaxHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoEaxHeader.setType("HEADER")
    
    pCryptoBrgEndianHeader = srvCryptoComponent.createFileSymbol("SRV_CRYPTO_BRG_ENDIAN_HEADER", None)
    pCryptoBrgEndianHeader.setSourcePath("service/crypto/brg_endian.h")
    pCryptoBrgEndianHeader.setOutputName("brg_endian.h")
    pCryptoBrgEndianHeader.setDestPath("stack/service/crypto")
    pCryptoBrgEndianHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoBrgEndianHeader.setType("HEADER")
    
    pCryptoBrgTypesHeader = srvCryptoComponent.createFileSymbol("SRV_CRYPTO_BRG_TYPES_HEADER", None)
    pCryptoBrgTypesHeader.setSourcePath("service/crypto/brg_types.h")
    pCryptoBrgTypesHeader.setOutputName("brg_types.h")
    pCryptoBrgTypesHeader.setDestPath("stack/service/crypto")
    pCryptoBrgTypesHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoBrgTypesHeader.setType("HEADER")
    
    pCryptoModeHdrHeader = srvCryptoComponent.createFileSymbol("SRV_CRYPTO_MODE_HDR_HEADER", None)
    pCryptoModeHdrHeader.setSourcePath("service/crypto/mode_hdr.h")
    pCryptoModeHdrHeader.setOutputName("mode_hdr.h")
    pCryptoModeHdrHeader.setDestPath("stack/service/crypto")
    pCryptoModeHdrHeader.setProjectPath("config/" + configName + "/stack/service/crypto/")
    pCryptoModeHdrHeader.setType("HEADER")    
