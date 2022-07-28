/**********************************************************************************************************************/
/** \addtogroup MacSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains data types and functions of the MAC API.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_DEFS_H_
#define MAC_DEFS_H_

#include "../mac_common/MacCommon.h"

enum EModulationType {
  MODULATION_ROBUST = 0x00,
  MODULATION_DBPSK_BPSK = 0x01,
  MODULATION_DQPSK_QPSK = 0x02,
  MODULATION_D8PSK_8PSK = 0x03,
  MODULATION_16_QAM = 0x04,
};

enum EModulationScheme {
  MODULATION_SCHEME_DIFFERENTIAL = 0x00,
  MODULATION_SCHEME_COHERENT = 0x01,
};

struct TMacFc {
  uint16_t m_nFrameType : 3;
  uint16_t m_nSecurityEnabled : 1;
  uint16_t m_nFramePending : 1;
  uint16_t m_nAckRequest : 1;
  uint16_t m_nPanIdCompression : 1;
  uint16_t m_nReserved : 3;
  uint16_t m_nDestAddressingMode : 2;
  uint16_t m_nFrameVersion : 2;
  uint16_t m_nSrcAddressingMode : 2;
};

struct TMacMhr {
  struct TMacFc m_Fc;
  uint8_t m_u8SequenceNumber;
  TPanId m_nDestinationPanIdentifier;
  struct TMacAddress m_DestinationAddress;
  TPanId m_nSourcePanIdentifier;
  struct TMacAddress m_SourceAddress;
  struct TMacAuxiliarySecurityHeader m_SecurityHeader;
};

struct TMacFrame {
  struct TMacMhr m_Header;
  uint16_t m_u16PayloadLength;
  uint8_t *m_pu8Payload;
  uint8_t m_u8PadLength;
  uint16_t m_u16Fcs;
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
