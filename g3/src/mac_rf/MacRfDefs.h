/**********************************************************************************************************************/
/** \addtogroup MacRfSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains definitions for the RF MAC API.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_RF_DEFS_H_
#define MAC_RF_DEFS_H_

#include <MacCommon.h>

#define HIE_CID_LEN   3

struct TMacFcRF {
  uint16_t m_nFrameType : 3;
  uint16_t m_nSecurityEnabled : 1;
  uint16_t m_nFramePending : 1;
  uint16_t m_nAckRequest : 1;
  uint16_t m_nPanIdCompression : 1;
  uint16_t m_nReserved : 2;
  uint16_t m_nIEPresent : 1;
  uint16_t m_nDestAddressingMode : 2;
  uint16_t m_nFrameVersion : 2;
  uint16_t m_nSrcAddressingMode : 2;
};

struct TMacMhrRF {
  struct TMacFcRF m_Fc;
  uint8_t m_u8SequenceNumber;
  TPanId m_nDestinationPanIdentifier;
  struct TMacAddress m_DestinationAddress;
  TPanId m_nSourcePanIdentifier;
  struct TMacAddress m_SourceAddress;
  struct TMacAuxiliarySecurityHeader m_SecurityHeader;
};

struct TMacHeaderIERF {
  uint16_t m_Length : 7;
  uint16_t m_ElementID : 8;
  uint16_t m_Type : 1;
  uint8_t m_CID[HIE_CID_LEN];
  uint8_t m_SubID;
};

struct TMacLIIERF {
  struct TMacHeaderIERF m_HeaderIE;
  uint8_t m_u8DutyCycle;
  uint8_t m_u8TxPowerOffset;
};

struct TMacRLQIERF {
  struct TMacHeaderIERF m_HeaderIE;
  uint8_t m_u8ReverseLQI;
};

struct TMacFrameRF {
  struct TMacMhrRF m_Header;
  struct TMacLIIERF m_LiIE;
  struct TMacRLQIERF m_RlqIE;
  uint16_t m_u16PayloadLength;
  uint8_t *m_pu8Payload;
  uint32_t m_u32Fcs;
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
