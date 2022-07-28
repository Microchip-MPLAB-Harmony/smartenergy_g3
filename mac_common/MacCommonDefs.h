/**********************************************************************************************************************/
/** \addtogroup MacCommon
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains the Common MAC type definitions.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_COMMON_DEFS_H_
#define MAC_COMMON_DEFS_H_

#if defined (__CC_ARM)
  #pragma anon_unions
#endif

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

#define MAC_MAX_MAC_PAYLOAD_SIZE     (400u)
#define MAC_SECURITY_OVERHEAD        (4)
#define MAC_MAX_MAC_HEADER_SIZE      (32)
#define MAC_SECURITY_HEADER_SIZE     (6)

#define MAC_PAN_ID_BROADCAST         (0xFFFFu)
#define MAC_SHORT_ADDRESS_BROADCAST  (0xFFFFu)
#define MAC_SHORT_ADDRESS_UNDEFINED  (0xFFFFu)

#define MAC_KEY_TABLE_ENTRIES         (2)
#define MAC_SECURITY_KEY_LENGTH       (16)

struct TMacSecurityKey {
  bool m_bValid;
  uint8_t m_au8Key[MAC_SECURITY_KEY_LENGTH];
};

typedef uint16_t TPanId;

typedef uint16_t TShortAddress;

// The extended address has to be little endian.
struct TExtendedAddress {
  uint8_t m_au8Address[8];
};

struct TMacMibCommon {
  TPanId m_nPanId;
  struct TExtendedAddress m_ExtendedAddress;
  TShortAddress m_nShortAddress;
  bool m_bPromiscuousMode;
  struct TMacSecurityKey m_aKeyTable[MAC_KEY_TABLE_ENTRIES];
  uint16_t m_u16RcCoord;
  uint8_t m_u8POSTableEntryTtl;
  bool m_bCoordinator;
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

struct TMacAuxiliarySecurityHeader {
  uint8_t m_nSecurityLevel : 3;
  uint8_t m_nKeyIdentifierMode : 2;
  uint8_t m_nReserved : 3;
  uint32_t m_u32FrameCounter;
  uint8_t m_u8KeyIdentifier;
};

enum EMacSecurityLevel {
  MAC_SECURITY_LEVEL_NONE = 0x00,
  MAC_SECURITY_LEVEL_ENC_MIC_32 = 0x05,
};

enum EMacQualityOfService {
  MAC_QUALITY_OF_SERVICE_NORMAL_PRIORITY = 0x00,
  MAC_QUALITY_OF_SERVICE_HIGH_PRIORITY = 0x01,
};

enum EMacTxOptions {
  MAC_TX_OPTION_ACK = 0x01,
};

typedef uint32_t TTimestamp;

#define MAX_TONE_GROUPS 24

struct TToneMap {
  uint8_t m_au8Tm[(MAX_TONE_GROUPS + 7) / 8];
};

struct TPanDescriptor {
  TPanId m_nPanId;
  uint8_t m_u8LinkQuality;
  TShortAddress m_nLbaAddress;
  uint16_t m_u16RcCoord;
};

#pragma pack(push,1)
struct TDeviceTableEntry {
  TShortAddress m_nShortAddress;
  uint32_t m_u32FrameCounter;
};
#pragma pack(pop)

/**********************************************************************************************************************/
/** Description of struct TMcpsDataRequest
 ***********************************************************************************************************************
 * @param m_eSrcAddrMode Source address mode 0, 16, 64 bits
 * @param m_nDstPanId The 16-bit PAN identifier of the entity to which the MSDU is being transferred
 * @param m_DstAddr The individual device address of the entity to which the MSDU is being transferred.
 * @param m_u16MsduLength The number of octets contained in the MSDU to be transmitted by the MAC sublayer entity.
 * @param m_pMsdu The set of octets forming the MSDU to be transmitted by the MAC sublayer entity
 * @param m_u8MsduHandle The handle associated with the MSDU to be transmitted by the MAC sublayer entity.
 * @param m_u8TxOptions Indicate the transmission options for this MSDU:
 *    0: unacknowledged transmission, 1: acknowledged transmission
 * @param m_eSecurityLevel The security level to be used: 0x00 unecrypted; 0x05 encrypted
 * @param m_u8KeyIndex The index of the key to be used.
 * @param u8QualityOfService The QOS (quality of service) parameter of the MSDU to be transmitted by the MAC sublayer
 *      entity
 *        0x00 = normal priority
 *        0x01 = high priority
 **********************************************************************************************************************/
struct TMcpsDataRequest {
  enum EMacAddressMode m_eSrcAddrMode;
  TPanId m_nDstPanId;
  struct TMacAddress m_DstAddr;
  uint16_t m_u16MsduLength;
  const uint8_t *m_pMsdu;
  uint8_t m_u8MsduHandle;
  uint8_t m_u8TxOptions;
  enum EMacSecurityLevel m_eSecurityLevel;
  uint8_t m_u8KeyIndex;
  enum EMacQualityOfService m_eQualityOfService;
};

/**********************************************************************************************************************/
/** Description of struct TMcpsDataConfirm
 ***********************************************************************************************************************
 * @param m_u8MsduHandle The handle associated with the MSDU being confirmed.
 * @param m_eStatus The status of the last MSDU transmission.
 * @param m_nTimestamp The time, in symbols, at which the data were transmitted.
 **********************************************************************************************************************/
struct TMcpsDataConfirm {
  uint8_t m_u8MsduHandle;
  enum EMacStatus m_eStatus;
  TTimestamp m_nTimestamp;
};

/**********************************************************************************************************************/
/** Description of struct TMcpsDataIndication
 ***********************************************************************************************************************
 * @param m_nSrcPanId The 16-bit PAN identifier of the device from which the frame was received.
 * @param m_SrcAddr The address of the device which sent the message.
 * @param m_nDstPanId The 16-bit PAN identifier of the entity to which the MSDU is being transferred.
 * @param m_DstAddr The individual device address of the entity to which the MSDU is being transferred.
 * @param m_u16MsduLength The number of octets contained in the MSDU to be indicated to the upper layer.
 * @param m_pMsdu The set of octets forming the MSDU received by the MAC sublayer entity.
 * @param m_u8MpduLinkQuality The (forward) LQI value measured during reception of the message.
 * @param m_u8Dsn The DSN of the received frame
 * @param m_nTimestamp The absolute time in milliseconds at which the frame was received and constructed, decrypted
 * @param m_eSecurityLevel The security level of the received message: 0x00 unecrypted; 0x05 encrypted
 * @param m_u8KeyIndex The index of the key used.
 * @param u8QualityOfService The QOS (quality of service) parameter of the MSDU received by the MAC sublayer entity
 *        0x00 = normal priority
 *        0x01 = high priority
 * @param m_u8RecvModulation Modulation of the received message.
 * @param m_u8RecvModulationScheme Modulation of the received message.
 * @param m_RecvToneMap The ToneMap of the received message.
 * @param m_u8ComputedModulation Computed modulation of the received message.
 * @param m_u8ComputedModulationScheme Computed modulation of the received message.
 * @param m_ComputedToneMap Compute ToneMap of the received message.
 * @param m_u8PhaseDifferential Phase Differenctial compared to Node that sent the frame.
 **********************************************************************************************************************/
struct TMcpsDataIndication {
  TPanId m_nSrcPanId;
  struct TMacAddress m_SrcAddr;
  TPanId m_nDstPanId;
  struct TMacAddress m_DstAddr;
  uint16_t m_u16MsduLength;
  uint8_t *m_pMsdu;
  uint8_t m_u8MpduLinkQuality;
  uint8_t m_u8Dsn;
  TTimestamp m_nTimestamp;
  enum EMacSecurityLevel m_eSecurityLevel;
  uint8_t m_u8KeyIndex;
  enum EMacQualityOfService m_eQualityOfService;
  uint8_t m_u8RecvModulation;
  uint8_t m_u8RecvModulationScheme;
  struct TToneMap m_RecvToneMap;
  uint8_t m_u8ComputedModulation;
  uint8_t m_u8ComputedModulationScheme;
  struct TToneMap m_ComputedToneMap;
  uint8_t m_u8PhaseDifferential;
};

/**********************************************************************************************************************/
/** Description of struct TMcpsMacSnifferIndication
 ***********************************************************************************************************************
 * @param m_u8FrameType The frame type.
 * @param m_nSrcPanId The 16-bit PAN identifier of the device from which the frame was received.
 * @param m_nSrcPanId The 16-bit PAN identifier of the device from which the frame was received.
 * @param m_SrcAddr The address of the device which sent the message.
 * @param m_nDstPanId The 16-bit PAN identifier of the entity to which the MSDU is being transferred.
 * @param m_DstAddr The individual device address of the entity to which the MSDU is being transferred.
 * @param m_u16MsduLength The number of octets contained in the MSDU to be indicated to the upper layer.
 * @param m_pMsdu The set of octets forming the MSDU received by the MAC sublayer entity.
 * @param m_u8MpduLinkQuality The (forward) LQI value measured during reception of the message.
 * @param m_u8Dsn The DSN of the received frame
 * @param m_nTimestamp The absolute time in milliseconds at which the frame was received and constructed, decrypted
 * @param m_eSecurityLevel The security level of the received message: 0x00 unecrypted; 0x05 encrypted
 * @param m_u8KeyIndex The index of the key used.
 * @param u8QualityOfService The QOS (quality of service) parameter of the MSDU received by the MAC sublayer entity
 *        0x00 = normal priority
 *        0x01 = high priority
 * @param m_u8RecvModulation Modulation of the received message.
 * @param m_u8RecvModulationScheme Modulation of the received message.
 * @param m_RecvToneMap The ToneMap of the received message.
 * @param m_u8ComputedModulation Computed modulation of the received message.
 * @param m_u8ComputedModulationScheme Computed modulation of the received message.
 * @param m_ComputedToneMap Compute ToneMap of the received message.
 * @param m_u8PhaseDifferential Phase Differenctial compared to Node that sent the frame.
 **********************************************************************************************************************/
struct TMcpsMacSnifferIndication {
  uint8_t m_u8FrameType;
  TPanId m_nSrcPanId;
  struct TMacAddress m_SrcAddr;
  TPanId m_nDstPanId;
  struct TMacAddress m_DstAddr;
  uint16_t m_u16MsduLength;
  uint8_t *m_pMsdu;
  uint8_t m_u8MpduLinkQuality;
  uint8_t m_u8Dsn;
  TTimestamp m_nTimestamp;
  enum EMacSecurityLevel m_eSecurityLevel;
  uint8_t m_u8KeyIndex;
  enum EMacQualityOfService m_eQualityOfService;
  uint8_t m_u8RecvModulation;
  uint8_t m_u8RecvModulationScheme;
  struct TToneMap m_RecvToneMap;
  uint8_t m_u8ComputedModulation;
  uint8_t m_u8ComputedModulationScheme;
  struct TToneMap m_ComputedToneMap;
  uint8_t m_u8PhaseDifferential;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeBeaconNotifyIndication
 ***********************************************************************************************************************
 * @param m_PanDescriptor The PAN descriptor
 **********************************************************************************************************************/
struct TMlmeBeaconNotifyIndication {
  struct TPanDescriptor m_PanDescriptor;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeResetRequest
 ***********************************************************************************************************************
 * @param m_bSetDefaultPib True to reset the PIB to the default values, false otherwise
 **********************************************************************************************************************/
struct TMlmeResetRequest {
  bool m_bSetDefaultPib;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeResetConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 **********************************************************************************************************************/
struct TMlmeResetConfirm {
  enum EMacStatus m_eStatus;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeScanRequest
 ***********************************************************************************************************************
 * @param m_u16ScanDuration Duration of the scan in seconds
 **********************************************************************************************************************/
struct TMlmeScanRequest {
  uint16_t m_u16ScanDuration;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeScanConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 **********************************************************************************************************************/
struct TMlmeScanConfirm {
  enum EMacStatus m_eStatus;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeCommStatusIndication
 ***********************************************************************************************************************
 * @param m_nPanId The PAN identifier of the device from which the frame was received or to which the frame was being
 *    sent.
 * @param m_SrcAddr The individual device address of the entity from which the frame causing the error originated.
 * @param m_DstAddr The individual device address of the device for which the frame was intended.
 * @param m_eStatus The communications status.
 * @param m_eSecurityLevel The security level purportedly used by the received frame.
 * @param m_u8KeyIndex The index of the key purportedly used by the originator of the received frame.
 **********************************************************************************************************************/
struct TMlmeCommStatusIndication {
  TPanId m_nPanId;
  struct TMacAddress m_SrcAddr;
  struct TMacAddress m_DstAddr;
  enum EMacStatus m_eStatus;
  enum EMacSecurityLevel m_eSecurityLevel;
  uint8_t m_u8KeyIndex;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeStartRequest
 ***********************************************************************************************************************
 * @param m_nPanId The pan id.
 **********************************************************************************************************************/
struct TMlmeStartRequest {
  TPanId m_nPanId;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeStartConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 **********************************************************************************************************************/
struct TMlmeStartConfirm {
  enum EMacStatus m_eStatus;
};

/**********************************************************************************************************************/
/** The MacMcpsDataConfirm primitive allows the upper layer to be notified of the completion of a MacMcpsDataRequest.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm
 **********************************************************************************************************************/
typedef void (*MacMcpsDataConfirm)(struct TMcpsDataConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMcpsDataIndication primitive is used to transfer received data from the mac sublayer to the upper layer.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the data indication.
 **********************************************************************************************************************/
typedef void (*MacMcpsDataIndication)(struct TMcpsDataIndication *pParameters);

/**********************************************************************************************************************/
/** The MacMcpsMacSnifferIndication primitive is used to transfer received data from the mac sublayer to the upper layer.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the data indication.
 **********************************************************************************************************************/
typedef void (*MacMcpsMacSnifferIndication)(struct TMcpsMacSnifferIndication *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeResetConfirm primitive allows upper layer to be notified of the completion of a MacMlmeResetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 **********************************************************************************************************************/
typedef void (*MacMlmeResetConfirm)(struct TMlmeResetConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeBeaconNotify primitive is generated by the MAC layer to notify the application about the discovery
 * of a new PAN coordinator or LBA
 ***********************************************************************************************************************
 * @param pPanDescriptor PAN descriptor contains information about the PAN
 **********************************************************************************************************************/
typedef void (*MacMlmeBeaconNotify)(struct TMlmeBeaconNotifyIndication *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeScanConfirm primitive allows the upper layer to be notified of the completion of a MacMlmeScanRequest.
 ***********************************************************************************************************************
 * @param pParameters The parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeScanConfirm)(struct TMlmeScanConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeStartConfirm primitive allows the upper layer to be notified of the completion of a TMlmeStartRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 **********************************************************************************************************************/
typedef void (*MacMlmeStartConfirm)(struct TMlmeStartConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeCommStatusIndication primitive allows the MAC layer to notify the next higher layer when a particular
 * event occurs on the PAN.
 ***********************************************************************************************************************
 * @param pParameters The parameters of the indication
 **********************************************************************************************************************/
typedef void (*MacMlmeCommStatusIndication)(struct TMlmeCommStatusIndication *pParameters);

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
