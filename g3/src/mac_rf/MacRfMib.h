/**********************************************************************************************************************/
/** \addtogroup MacRfSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains the RF MAC MIB.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_RF_MIB_H_
#define MAC_RF_MIB_H_

#include <MacRfDefs.h>

#define MAC_PIB_OPERATING_MODE_RF_MIN_VALUE                  1
#define MAC_PIB_OPERATING_MODE_RF_MAX_VALUE                  4

#define MAC_PIB_CHANNEL_NUMBER_RF_MAX_VALUE                  7279

#define MAC_PIB_FREQUENCY_BAND_RF_MAX_VALUE                  6

#define MAC_PIB_TRANSMIT_ATTEN_RF_MAX_VALUE                  64

#define MAC_PIB_ADAPTIVE_POWER_STEP_RF_MAX_VALUE             15

#define MAC_PIB_DUTY_CYCLE_PERIOD_RF_MIN_VALUE               1
#define MAC_PIB_DUTY_CYCLE_LIMIT_RF_MIN_VALUE                1
#define MAC_PIB_DUTY_CYCLE_THRESHOLD_RF_MIN_VALUE            1

#define MAC_PIB_MANUF_ACK_TX_DELAY_RF_MIN_VALUE              1000 // us
#define MAC_PIB_MANUF_ACK_TX_DELAY_RF_MAX_VALUE              9000 // us
#define MAC_PIB_MANUF_ACK_RX_WAIT_TIME_RF_MIN_VALUE          20000 // us
#define MAC_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF_MIN_VALUE     20000 // us
#define MAC_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF_MIN_VALUE    85000 // us

#pragma pack(push,1)

struct TPOSEntryRF {
  TShortAddress m_nShortAddress;
  uint8_t m_u8ForwardLqi;
  uint8_t m_u8ReverseLqi;
  uint8_t m_u8DutyCycle;
  uint8_t m_u8ForwardTxPowerOffset;
  uint8_t m_u8ReverseTxPowerOffset;
  uint16_t m_u16POSValidTime;
};

struct TDsnTableEntryRF {
  struct TMacAddress m_Address;
  uint8_t m_u8Dsn;
  uint8_t m_u8DsnValidTime;
};

#pragma pack(pop)

// RF Mac layer Table Pointers and Sizes
struct TMacTablesRF {
  // Pointers
  struct TPOSEntryRF *m_PosTableRF;
  struct TDeviceTableEntry *m_DeviceTableRF;
  struct TDsnTableEntryRF *m_DsnTableRF;
  // Sizes
  uint16_t m_MacPosTableSizeRF;
  uint16_t m_MacDeviceTableSizeRF;
  uint16_t m_MacDsnTableSizeRF;
};

struct TMacMibRF {
  uint32_t m_u32RetryCountRF;
  uint32_t m_u32MultipleRetryCountRF;
  uint32_t m_u32TxFailCountRF;
  uint32_t m_u32TxSuccessCountRF;
  uint32_t m_u32FcsErrorCountRF;
  uint32_t m_u32SecurityFailureCountRF;
  uint32_t m_u32DuplicateFrameCountRF;
  uint32_t m_u32RxSuccessCountRF;
  uint32_t m_u32NackCountRF;
  uint32_t m_u32TxDataPacketCountRF;
  uint32_t m_u32RxDataPacketCountRF;
  uint32_t m_u32TxCmdPacketCountRF;
  uint32_t m_u32RxCmdPacketCountRF;
  uint32_t m_u32CsmaFailCountRF;
  uint32_t m_u32RxDataBroadcastCountRF;
  uint32_t m_u32TxDataBroadcastCountRF;
  uint32_t m_u32BadCrcCountRF;
  struct TPOSEntryRF *m_aPOSTableRF;
  uint16_t m_u16POSTableSizeRF;
  struct TDeviceTableEntry *m_aDeviceTableRF;
  uint16_t m_u16DeviceTableSizeRF;
  struct TDsnTableEntryRF *m_aDsnTableRF;
  uint16_t m_u16DsnTableSizeRF;
  uint8_t m_u8DuplicateDetectionTtlRF;
  uint8_t m_u8EBsnRF;
  uint8_t m_u8DsnRF;
  uint32_t m_u32FrameCounterRF;
  bool m_bLBPFrameReceivedRF;
  bool m_bLNGFrameReceivedRF;
  bool m_bBCNFrameReceivedRF;
  uint32_t m_u32RxOtherDestinationCountRF;
  uint32_t m_u32RxInvalidFrameLengthCountRF;
  uint32_t m_u32RxWrongAddrModeCountRF;
  uint32_t m_u32RxUnsupportedSecurityCountRF;
  uint32_t m_u32RxWrongKeyIdCountRF;
  uint32_t m_u32RxInvalidKeyCountRF;
  uint32_t m_u32RxWrongFCCountRF;
  uint32_t m_u32RxDecryptionErrorCountRF;
  bool m_bMacSnifferRF;
  uint8_t m_u8MaxBeRF;
  uint8_t m_u8MaxCsmaBackoffsRF;
  uint8_t m_u8MaxFrameRetriesRF;
  uint8_t m_u8MinBeRF;
  uint8_t m_u8OperatingModeRF; // Fixed in this spec version
  uint16_t m_u16ChannelNumberRF; // Fixed in this spec version
  uint8_t m_u8DutyCycleUsageRF;
  uint16_t m_u16DutyCyclePeriodRF;
  uint16_t m_u16DutyCycleLimitRF;
  uint8_t m_u8DutyCycleThresholdRF;
  uint32_t m_u32AckTxDelayRF;
  uint32_t m_u32AckRxWaitTimeRF;
  uint32_t m_u32AckConfirmWaitTimeRF;
  uint32_t m_u32DataConfirmWaitTimeRF;
  bool m_bDisablePhyRF;
  uint8_t m_u8FrequencyBandRF;
  uint8_t m_u8TransmitAttenRF;
  uint8_t m_u8AdaptivePowerStepRF;
  uint8_t m_u8AdaptivePowerHighBoundRF;
  uint8_t m_u8AdaptivePowerLowBoundRF;
  uint8_t m_u8BeaconRandomizationWindowLength;
};

enum EMacPibAttributeRF {
  MAC_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH_RF = 0x00000111,
  MAC_PIB_DSN_RF = 0x00000200,
  MAC_PIB_MAX_BE_RF = 0x00000201,
  MAC_PIB_MAX_CSMA_BACKOFFS_RF = 0x00000202,
  MAC_PIB_MAX_FRAME_RETRIES_RF = 0x00000203,
  MAC_PIB_MIN_BE_RF = 0x00000204,
  MAC_PIB_TIMESTAMP_SUPPORTED_RF = 0x00000205,
  MAC_PIB_DEVICE_TABLE_RF = 0x00000206,
  MAC_PIB_FRAME_COUNTER_RF = 0x00000207,
  MAC_PIB_DUPLICATE_DETECTION_TTL_RF = 0x00000208,
  MAC_PIB_COUNTER_OCTETS_RF = 0x00000209,
  MAC_PIB_RETRY_COUNT_RF = 0x0000020A,
  MAC_PIB_MULTIPLE_RETRY_COUNT_RF = 0x0000020B,
  MAC_PIB_TX_FAIL_COUNT_RF = 0x0000020C,
  MAC_PIB_TX_SUCCESS_COUNT_RF = 0x0000020D,
  MAC_PIB_FCS_ERROR_COUNT_RF = 0x0000020E,
  MAC_PIB_SECURITY_FAILURE_COUNT_RF = 0x0000020F,
  MAC_PIB_DUPLICATE_FRAME_COUNT_RF = 0x00000210,
  MAC_PIB_RX_SUCCESS_COUNT_RF = 0x00000211,
  MAC_PIB_NACK_COUNT_RF = 0x00000212,
  MAC_PIB_USE_ENHANCED_BEACON_RF = 0x00000213,
  MAC_PIB_EB_HEADER_IE_LIST_RF = 0x00000214,
  MAC_PIB_EB_PAYLOAD_IE_LIST_RF = 0x00000215,
  MAC_PIB_EB_FILTERING_ENABLED_RF = 0x00000216,
  MAC_PIB_EBSN_RF = 0x00000217,
  MAC_PIB_EB_AUTO_SA_RF = 0x00000218,
  MAC_PIB_SEC_SECURITY_LEVEL_LIST_RF = 0x0000021A,

  MAC_PIB_POS_TABLE_RF = 0x0000021D,
  MAC_PIB_OPERATING_MODE_RF = 0x0000021E,
  MAC_PIB_CHANNEL_NUMBER_RF = 0x0000021F,
  MAC_PIB_DUTY_CYCLE_USAGE_RF = 0x00000220,
  MAC_PIB_DUTY_CYCLE_PERIOD_RF = 0x00000221,
  MAC_PIB_DUTY_CYCLE_LIMIT_RF = 0x00000222,
  MAC_PIB_DUTY_CYCLE_THRESHOLD_RF = 0x00000223,
  MAC_PIB_DISABLE_PHY_RF = 0x00000224,
  MAC_PIB_FREQUENCY_BAND_RF = 0x00000225,
  MAC_PIB_TRANSMIT_ATTEN_RF = 0x00000226,
  MAC_PIB_ADAPTIVE_POWER_STEP_RF = 0x00000227,
  MAC_PIB_ADAPTIVE_POWER_HIGH_BOUND_RF = 0x00000228,
  MAC_PIB_ADAPTIVE_POWER_LOW_BOUND_RF = 0x00000229,

  // Manufacturer specific
  // Resets the device table upon a GMK activation.
  MAC_PIB_MANUF_SECURITY_RESET_RF = 0x08000203,
  // Indicates whether an LBP frame for other destination has been received
  MAC_PIB_MANUF_LBP_FRAME_RECEIVED_RF = 0x08000204,
  // Indicates whether an LBP frame for other destination has been received
  MAC_PIB_MANUF_LNG_FRAME_RECEIVED_RF = 0x08000205,
  // Indicates whether an Beacon frame from other nodes has been received
  MAC_PIB_MANUF_BCN_FRAME_RECEIVED_RF = 0x08000206,
  // Gets number of discarded packets due to Other Destination
  MAC_PIB_MANUF_RX_OTHER_DESTINATION_COUNT_RF = 0x08000207,
  // Gets number of discarded packets due to Invalid Frame Lenght
  MAC_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT_RF = 0x08000208,
  // Gets number of discarded packets due to Wrong Addressing Mode
  MAC_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT_RF = 0x08000209,
  // Gets number of discarded packets due to Unsupported Security
  MAC_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT_RF = 0x0800020A,
  // Gets number of discarded packets due to Wrong Key Id
  MAC_PIB_MANUF_RX_WRONG_KEY_ID_COUNT_RF = 0x0800020B,
  // Gets number of discarded packets due to Invalid Key
  MAC_PIB_MANUF_RX_INVALID_KEY_COUNT_RF = 0x0800020C,
  // Gets number of discarded packets due to Wrong Frame Counter
  MAC_PIB_MANUF_RX_WRONG_FC_COUNT_RF = 0x0800020D,
  // Gets number of discarded packets due to Decryption Error
  MAC_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT_RF = 0x0800020E,
  // Gets number of transmitted Data packets
  MAC_PIB_MANUF_TX_DATA_PACKET_COUNT_RF = 0x0800020F,
  // Gets number of received Data packets
  MAC_PIB_MANUF_RX_DATA_PACKET_COUNT_RF = 0x08000210,
  // Gets number of transmitted Command packets
  MAC_PIB_MANUF_TX_CMD_PACKET_COUNT_RF = 0x08000211,
  // Gets number of received Command packets
  MAC_PIB_MANUF_RX_CMD_PACKET_COUNT_RF = 0x08000212,
  // Gets number of Channel Access failures
  MAC_PIB_MANUF_CSMA_FAIL_COUNT_RF = 0x08000213,
  // Gets number of received broadcast packets
  MAC_PIB_MANUF_RX_DATA_BROADCAST_COUNT_RF = 0x08000214,
  // Gets number of transmitted broadcast packets
  MAC_PIB_MANUF_TX_DATA_BROADCAST_COUNT_RF = 0x08000215,
  // Gets number of received packets with wrong CRC
  MAC_PIB_MANUF_BAD_CRC_COUNT_RF = 0x08000216,
  // Enables MAC Sniffer
  MAC_PIB_MANUF_ENABLE_MAC_SNIFFER_RF = 0x08000217,
  // Gets number of valid elements in the POS Table
  MAC_PIB_MANUF_POS_TABLE_COUNT_RF = 0x08000218,
  // Gets internal MAC version
  MAC_PIB_MANUF_MAC_INTERNAL_VERSION_RF = 0x08000219,
  // Resets MAC statistics
  MAC_PIB_MANUF_RESET_MAC_STATS_RF = 0x0800021A,
  // Provides access to POS table by short address (referenced as index)
  MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF = 0x0800021B,
  // Configures time between a received frame and the transmission of its ACK
  MAC_PIB_MANUF_ACK_TX_DELAY_RF = 0x0800021C,
  // Configures time to wait for a requested ACK before timing out
  MAC_PIB_MANUF_ACK_RX_WAIT_TIME_RF = 0x0800021D,
  // Configures time to wait for an ACK Confirm before timing out
  MAC_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF = 0x0800021E,
  // Configures time to wait for a Data Confirm before timing out
  MAC_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF = 0x0800021F,
  // Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID
  MAC_PIB_MANUF_PHY_PARAM_RF = 0x08000220
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
