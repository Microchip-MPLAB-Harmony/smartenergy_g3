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

#ifndef MAC_API_H_
#define MAC_API_H_

#include "MacMib.h"

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
/** Description of struct TMlmeGetRequest
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeGetRequest {
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeGetConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeGetConfirm {
  enum EMacStatus m_eStatus;
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
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
/** Description of struct TMlmeGetRequest
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeSetRequest {
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeSetConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeSetConfirm {
  enum EMacStatus m_eStatus;
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
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

struct TMacNotifications;

#define BAND_CENELEC_A 0
#define BAND_CENELEC_B 1
#define BAND_FCC 2
#define BAND_ARIB 3

/**********************************************************************************************************************/
/** Use this function to initialize the MAC layer. The MAC layer should be initialized before doing any other operation.
 * The APIs cannot be mixed, if the stack is initialized in ADP mode then only the ADP functions can be used and if the
 * stack is initialized in MAC mode then only MAC functions can be used.
 * @param pNotifications Structure with callbacks used by the MAC layer to notify the upper layer about specific events
 * @param band Working band (should be inline with the hardware)
 * @param pTableSizes Structure containing sizes of Mac Tables (defined outside Mac layer and accessed through Pointers)
 **********************************************************************************************************************/
void MacInitialize(struct TMacNotifications *pNotifications, uint8_t u8Band, struct TMacTables *pTables);

/**********************************************************************************************************************/
/** This function should be called at least every millisecond in order to allow the G3 stack to run and execute its
 * internal tasks.
 **********************************************************************************************************************/
void MacEventHandler(void);

/**********************************************************************************************************************/
/** The MacMcpsDataRequest primitive requests the transfer of upper layer PDU to another device or multiple devices.
 * Parameters from struct TMcpsDataRequest
 ***********************************************************************************************************************
 * @param pParameters Request parameters
 **********************************************************************************************************************/
void MacMcpsDataRequest(struct TMcpsDataRequest *pParameters);

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
/** The MacMlmeGetRequest primitive allows the upper layer to get the value of an attribute from the MAC information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeGetRequest(struct TMlmeGetRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetConfirm primitive allows the upper layer to be notified of the completion of a MacMlmeGetRequest.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeGetConfirm)(struct TMlmeGetConfirm *pParameters);


/**********************************************************************************************************************/
/** The MacMlmeGetRequestSync primitive allows the upper layer to get the value of an attribute from the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to results
 **********************************************************************************************************************/
enum EMacStatus MacMlmeGetRequestSync(enum EMacPibAttribute eAttribute, uint16_t u16Index, struct TMacPibValue *pValue);


/**********************************************************************************************************************/
/** The MacMlmeSetRequest primitive allows the upper layer to set the value of an attribute in the MAC information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeSetRequest(struct TMlmeSetRequest *pParameters);


/**********************************************************************************************************************/
/** The MacMlmeSetConfirm primitive allows the upper layer to be notified of the completion of a MacMlmeSetRequest.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeSetConfirm)(struct TMlmeSetConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeSetRequestSync primitive allows the upper layer to set the value of an attribute in the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to IB new values
 **********************************************************************************************************************/
enum EMacStatus MacMlmeSetRequestSync(enum EMacPibAttribute eAttribute, uint16_t u16Index, const struct TMacPibValue *pValue);

/**********************************************************************************************************************/
/** The MacMlmeResetRequest primitive performs a reset of the mac sublayer and allows the resetting of the MIB
 * attributes.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeResetRequest(struct TMlmeResetRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeResetConfirm primitive allows upper layer to be notified of the completion of a MacMlmeResetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 **********************************************************************************************************************/
typedef void (*MacMlmeResetConfirm)(struct TMlmeResetConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeScanRequest primitive allows the upper layer to scan for networks operating in its POS.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request.
 **********************************************************************************************************************/
void MacMlmeScanRequest(struct TMlmeScanRequest *pParameters);

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
/** The MacMlmeStartRequest primitive allows the upper layer to request the starting of a new network.
 ***********************************************************************************************************************
 * @param pParameters The parameters of the request
 **********************************************************************************************************************/
void MacMlmeStartRequest(struct TMlmeStartRequest *pParameters);

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

struct TMacNotifications {
  MacMcpsDataConfirm m_McpsDataConfirm;
  MacMcpsDataIndication m_McpsDataIndication;
  MacMlmeGetConfirm m_MlmeGetConfirm;
  MacMlmeSetConfirm m_MlmeSetConfirm;
  MacMlmeResetConfirm m_MlmeResetConfirm;
  MacMlmeBeaconNotify m_MlmeBeaconNotifyIndication;
  MacMlmeScanConfirm m_MlmeScanConfirm;
  MacMlmeStartConfirm m_MlmeStartConfirm;
  MacMlmeCommStatusIndication m_MlmeCommStatusIndication;
  MacMcpsMacSnifferIndication m_McpsMacSnifferIndication;
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
