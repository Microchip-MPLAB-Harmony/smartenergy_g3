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

#if defined (__CC_ARM)
  #pragma anon_unions
#endif

#define MAC_MAX_MAC_PAYLOAD_SIZE (400u)
#define MAC_SECURITY_OVERHEAD (4)
#define MAC_MAX_MAC_HEADER_SIZE (32)
#define MAC_SECURITY_HEADER_SIZE (6)

#define MAX_TONE_GROUPS 24

struct TToneMap {
  uint8_t m_au8Tm[(MAX_TONE_GROUPS + 7) / 8];
};

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

typedef uint16_t TPanId;

typedef uint16_t TShortAddress;

// The extended address has to be little endian.
struct TExtendedAddress {
  uint8_t m_au8Address[8];
};

enum EMacAddressMode {
  MAC_ADDRESS_MODE_NO_ADDRESS = 0x00,
  MAC_ADDRESS_MODE_SHORT = 0x02,
  MAC_ADDRESS_MODE_EXTENDED = 0x03,
};

struct TMacAddress {
  enum EMacAddressMode m_eAddrMode;
  union {
    TShortAddress m_nShortAddress;
    struct TExtendedAddress m_ExtendedAddress;
  };
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

struct TMacAuxiliarySecurityHeader {
  uint8_t m_nSecurityLevel : 3;
  uint8_t m_nKeyIdentifierMode : 2;
  uint8_t m_nReserved : 3;
  uint32_t m_u32FrameCounter;
  uint8_t m_u8KeyIdentifier;
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

typedef uint32_t TTimestamp;

struct TPanDescriptor {
  TPanId m_nPanId;
  uint8_t m_u8LinkQuality;
  TShortAddress m_nLbaAddress;
  uint16_t m_u16RcCoord;
};

enum EMacStatus {
  MAC_STATUS_SUCCESS = 0x00,
  MAC_STATUS_BEACON_LOSS = 0xE0,
  MAC_STATUS_CHANNEL_ACCESS_FAILURE = 0xE1,
  MAC_STATUS_COUNTER_ERROR = 0xDB,
  MAC_STATUS_DENIED = 0xE2,
  MAC_STATUS_DISABLE_TRX_FAILURE = 0xE3,
  MAC_STATUS_FRAME_TOO_LONG = 0xE5,
  MAC_STATUS_IMPROPER_KEY_TYPE = 0xDC,
  MAC_STATUS_IMPROPER_SECURITY_LEVEL = 0xDD,
  MAC_STATUS_INVALID_ADDRESS = 0xF5,
  MAC_STATUS_INVALID_GTS = 0xE6,
  MAC_STATUS_INVALID_HANDLE = 0xE7,
  MAC_STATUS_INVALID_INDEX = 0xF9,
  MAC_STATUS_INVALID_PARAMETER = 0xE8,
  MAC_STATUS_LIMIT_REACHED = 0xFA,
  MAC_STATUS_NO_ACK = 0xE9,
  MAC_STATUS_NO_BEACON = 0xEA,
  MAC_STATUS_NO_DATA = 0xEB,
  MAC_STATUS_NO_SHORT_ADDRESS = 0xEC,
  MAC_STATUS_ON_TIME_TOO_LONG = 0xF6,
  MAC_STATUS_OUT_OF_CAP = 0xED,
  MAC_STATUS_PAN_ID_CONFLICT = 0xEE,
  MAC_STATUS_PAST_TIME = 0xF7,
  MAC_STATUS_READ_ONLY = 0xFB,
  MAC_STATUS_REALIGNMENT = 0xEF,
  MAC_STATUS_SCAN_IN_PROGRESS = 0xFC,
  MAC_STATUS_SECURITY_ERROR = 0xE4,
  MAC_STATUS_SUPERFRAME_OVERLAP = 0xFD,
  MAC_STATUS_TRACKING_OFF = 0xF8,
  MAC_STATUS_TRANSACTION_EXPIRED = 0xF0,
  MAC_STATUS_TRANSACTION_OVERFLOW = 0xF1,
  MAC_STATUS_TX_ACTIVE = 0xF2,
  MAC_STATUS_UNAVAILABLE_KEY = 0xF3,
  MAC_STATUS_UNSUPPORTED_ATTRIBUTE = 0xF4,
  MAC_STATUS_UNSUPPORTED_LEGACY = 0xDE,
  MAC_STATUS_UNSUPPORTED_SECURITY = 0xDF,
  MAC_STATUS_ALTERNATE_PANID_DETECTION = 0x80,
  MAC_STATUS_QUEUE_FULL = 0xD0,
};

#define MAC_PAN_ID_BROADCAST (0xFFFFu)
#define MAC_SHORT_ADDRESS_BROADCAST (0xFFFFu)
#define MAC_SHORT_ADDRESS_UNDEFINED (0xFFFFu)

#define MAC_SECURITY_KEY_LENGTH (16)

struct TMacSecurityKey {
  bool m_bValid;
  uint8_t m_au8Key[MAC_SECURITY_KEY_LENGTH];
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
