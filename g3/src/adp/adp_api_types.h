#ifndef __ADP_API_TYPES_H__
#define __ADP_API_TYPES_H__

#if defined (__CC_ARM)
  #pragma anon_unions
#endif

#include <stdint.h>
#include <stdbool.h>

#define ADP_ADDRESS_16BITS 2
#define ADP_ADDRESS_64BITS 8

enum EAdpMac_Modulation {
  MOD_ROBO, MOD_BPSK, MOD_DBPSK, MOD_QPSK, MOD_DQPSK, MOD_8PSK, MOD_D8PSK, MOD_16QAM, MOD_UNKNOWN = 255
};

struct TAdpExtendedAddress {
  uint8_t m_au8Value[8];
};

struct TAdpAddress {
  uint8_t m_u8AddrSize; // ADP_ADDRESS_16BITS(2) or ADP_ADDRESS_64BITS(8)
  union {
    uint16_t m_u16ShortAddr;
    struct TAdpExtendedAddress m_ExtendedAddress;
  };
};

enum TAdpBand {
  ADP_BAND_CENELEC_A = 0, ADP_BAND_CENELEC_B = 1, ADP_BAND_FCC = 2, ADP_BAND_ARIB = 3
};

/**********************************************************************************************************************/
/** PAN descriptor structure specification
 *
 ***********************************************************************************************************************
 * @param u16PanId The 16-bit PAN identifier.
 * @param u8LinkQuality The 8-bit link quality of LBA.
 * @param u16LbaAddress The 16 bit short address of a device in this PAN to be used as the LBA by the associating device.
 * @param u16RcCoord The estimated route cost from LBA to the coordinator.
 **********************************************************************************************************************/
struct TAdpPanDescriptor {
  uint16_t m_u16PanId;
  uint8_t m_u8LinkQuality;
  uint16_t m_u16LbaAddress;
  uint16_t m_u16RcCoord;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
};

/**********************************************************************************************************************/
/** Path discovery
 *
 ***********************************************************************************************************************
 * @param m_u16HopAddress The hop / node address
 * @param m_u8Mns MetricNotSupported: 1 the metric type is not supported by the hop, 0 if supported
 * @param  m_u8LinkCost LinkCost of the node
 * @param  m_u8PhaseDiff Phase Differential on link
 * @param  m_u8Mrx Medium from which request is received
 * @param  m_u8Mtx Medium to which request is transmitted
 * @param  m_u8RsvBits Reserved Bits. Read from incoming frame to propagate them correctly
 **********************************************************************************************************************/
struct THopDescriptor {
  uint16_t m_u16HopAddress;
  uint8_t m_u8Mns;
  uint8_t m_u8LinkCost;
  uint8_t m_u8PhaseDiff;
  uint8_t m_u8Mrx;
  uint8_t m_u8Mtx;
  uint8_t m_u8RsvBits;
};

/**********************************************************************************************************************/
/** Path discovery
 *
 ***********************************************************************************************************************
 * @param m_u16DstAddr The short unicast destination address of the path discovery.
 * @param m_u16ExpectedOrigAddr The expected originator of the path reply
 * @param m_u16OrigAddr The real originator of the path reply
 * @param m_u16RsvBits Reserved Bits. Read from incoming frame to propagate them correctly
 * @param m_u8MetricType Path metric type
 * @param m_u8ForwardHopsCount Number of path hops in the forward table
 * @param m_u8ReverseHopsCount Number of path hops in the reverse table
 * @param m_aForwardPath Table with the information of each hop in forward direction (according to m_u8ForwardHopsCount)
 * @param m_aReversePath Table with the information of each hop in reverse direction (according to m_u8ReverseHopsCount)
 **********************************************************************************************************************/
struct TPathDescriptor {
  uint16_t m_u16DstAddr;
  uint16_t m_u16ExpectedOrigAddr;
  uint16_t m_u16OrigAddr;
  uint16_t m_u16RsvBits;
  uint8_t m_u8MetricType;
  uint8_t m_u8ForwardHopsCount;
  uint8_t m_u8ReverseHopsCount;
  struct THopDescriptor m_aForwardPath[16];
  struct THopDescriptor m_aReversePath[16];
};

// List of ADP supported MIB attributes.
enum EAdpPibAttribute {
  ADP_IB_SECURITY_LEVEL = 0x00000000, /* 8 bits */
  ADP_IB_PREFIX_TABLE = 0x00000001, /* [11 to 27] Byte entries */
  ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL = 0x00000002, /* 16 bits */
  ADP_IB_METRIC_TYPE = 0x00000003, /* 8 bits */
  ADP_IB_LOW_LQI_VALUE = 0x00000004, /* 8 bits */
  ADP_IB_HIGH_LQI_VALUE = 0x00000005, /* 8 bits */
  ADP_IB_RREP_WAIT = 0x00000006, /* 8 bits */
  ADP_IB_CONTEXT_INFORMATION_TABLE = 0x00000007, /* [4 to 20] Byte entries */
  ADP_IB_COORD_SHORT_ADDRESS = 0x00000008, /* 16 bits */
  ADP_IB_RLC_ENABLED = 0x00000009, /* 8 bits (bool) */
  ADP_IB_ADD_REV_LINK_COST = 0x0000000A, /* 8 bits */
  ADP_IB_BROADCAST_LOG_TABLE = 0x0000000B, /* 5 Byte entries */
  ADP_IB_ROUTING_TABLE = 0x0000000C, /* 9 Byte entries */
  ADP_IB_UNICAST_RREQ_GEN_ENABLE = 0x0000000D, /* 8 bits (bool) */
  ADP_IB_GROUP_TABLE = 0x0000000E, /* 2 Byte entries */
  ADP_IB_MAX_HOPS = 0x0000000F, /* 8 bits */
  ADP_IB_DEVICE_TYPE = 0x00000010, /* 8 bits */
  ADP_IB_NET_TRAVERSAL_TIME = 0x00000011, /* 8 bits */
  ADP_IB_ROUTING_TABLE_ENTRY_TTL = 0x00000012, /* 16 bits */
  ADP_IB_KR = 0x00000013, /* 8 bits */
  ADP_IB_KM = 0x00000014, /* 8 bits */
  ADP_IB_KC = 0x00000015, /* 8 bits */
  ADP_IB_KQ = 0x00000016, /* 8 bits */
  ADP_IB_KH = 0x00000017, /* 8 bits */
  ADP_IB_RREQ_RETRIES = 0x00000018, /* 8 bits */
  ADP_IB_RREQ_WAIT = 0x00000019, /* 8 bits */
  ADP_IB_WEAK_LQI_VALUE = 0x0000001A, /* 8 bits */
  ADP_IB_KRT = 0x0000001B, /* 8 bits */
  ADP_IB_SOFT_VERSION = 0x0000001C, /* 6 Byte array */
  ADP_IB_SNIFFER_MODE = 0x0000001D, /* 8 bits (bool) */
  ADP_IB_BLACKLIST_TABLE = 0x0000001E, /* 4 Byte entries */
  ADP_IB_BLACKLIST_TABLE_ENTRY_TTL = 0x0000001F, /* 16 bits */
  ADP_IB_MAX_JOIN_WAIT_TIME = 0x00000020, /* 16 bits */
  ADP_IB_PATH_DISCOVERY_TIME = 0x00000021, /* 8 bits */
  ADP_IB_ACTIVE_KEY_INDEX = 0x00000022, /* 8 bits */
  ADP_IB_DESTINATION_ADDRESS_SET = 0x00000023, /* 2 Byte entries */
  ADP_IB_DEFAULT_COORD_ROUTE_ENABLED = 0x00000024, /* 8 bits (bool) */
  ADP_IB_DELAY_LOW_LQI = 0x00000025, /* 16 bits */
  ADP_IB_DELAY_HIGH_LQI = 0x00000026, /* 16 bits */
  ADP_IB_RREQ_JITTER_LOW_LQI = 0x00000027, /* 8 bits */
  ADP_IB_RREQ_JITTER_HIGH_LQI = 0x00000028, /* 8 bits */
  ADP_IB_TRICKLE_DATA_ENABLED = 0x00000029, /* 8 bits */
  ADP_IB_TRICKLE_MIN_LQI_VALUE = 0x0000002A, /* 8 bits */
  ADP_IB_TRICKLE_STEP = 0x0000002B, /* 8 bits */
  ADP_IB_TRICKLE_FACTOR_I_MAX = 0x0000002C, /* 8 bits */
  ADP_IB_TRICKLE_I_MIN = 0x0000002D, /* 16 bits */
  ADP_IB_TRICKLE_MAX_KI = 0x0000002E, /* 8 bits */
  ADP_IB_TRICKLE_ADAPTATIVE_I_MIN = 0x0000002F, /* 8 bits */
  ADP_IB_CLUSTER_TRICKLE_ENABLED = 0x00000030, /* 8 bits */
  ADP_IB_CLUSTER_MIN_LQI = 0x00000031, /* 8 bits */
  ADP_IB_CLUSTER_TRICKLE_K = 0x00000032, /* 8 bits */
  ADP_IB_CLUSTER_RREQ_ROUTE_COST_DEVIATION = 0x00000033, /* 8 bits */
  ADP_IB_CLUSTER_TRICKLE_I = 0x00000034, /* 16 bits */
  ADP_IB_LOW_LQI_VALUE_RF = 0x000000D0, /* 8 bits */
  ADP_IB_HIGH_LQI_VALUE_RF = 0x000000D1, /* 8 bits */
  ADP_IB_KQ_RF = 0x000000D2, /* 8 bits */
  ADP_IB_KH_RF = 0x000000D3, /* 8 bits */
  ADP_IB_KRT_RF = 0x000000D4, /* 8 bits */
  ADP_IB_KDC_RF = 0x000000D5, /* 8 bits */
  ADP_IB_USE_BACKUP_MEDIA = 0x000000D6, /* 8 bits (bool) */
  ADP_IB_WEAK_LQI_VALUE_RF = 0x000000D7, /* 8 bits */
  ADP_IB_DISABLE_DEFAULT_ROUTING = 0x000000F0, /* 8 bits (bool) */
  // manufacturer
  ADP_IB_MANUF_REASSEMBY_TIMER = 0x080000C0, /* 16 bits */
  ADP_IB_MANUF_IPV6_HEADER_COMPRESSION = 0x080000C1, /* 8 bits (bool) */
  ADP_IB_MANUF_EAP_PRESHARED_KEY = 0x080000C2, /* 16 Byte array */
  ADP_IB_MANUF_EAP_NETWORK_ACCESS_IDENTIFIER = 0x080000C3, /* [8 to 36] Byte array */
  ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER = 0x080000C4, /* 8 bits */
  ADP_IB_MANUF_REGISTER_DEVICE = 0x080000C5, /* 29 Byte struct */
  ADP_IB_MANUF_DATAGRAM_TAG = 0x080000C6, /* 16 bits */
  ADP_IB_MANUF_RANDP = 0x080000C7, /* 16 Byte array */
  ADP_IB_MANUF_ROUTING_TABLE_COUNT = 0x080000C8, /* 32 bits */
  ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER = 0x080000C9, /* 16 bits */
  ADP_IB_MANUF_FORCED_NO_ACK_REQUEST = 0x080000CA, /* 8 bits (bool) */
  ADP_IB_MANUF_LQI_TO_COORD = 0x080000CB, /* 8 bits */
  ADP_IB_MANUF_BROADCAST_ROUTE_ALL = 0x080000CC, /* 8 bits (bool) */
  ADP_IB_MANUF_KEEP_PARAMS_AFTER_KICK_LEAVE = 0x080000CD, /* 8 bits (bool) */
  ADP_IB_MANUF_ADP_INTERNAL_VERSION = 0x080000CE, /* 6 Byte array */
  ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED = 0x080000CF, /* 16 bits */
  ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS = 0x080000D0, /* 16 bits */
  ADP_IB_MANUF_IPV6_ULA_DEST_SHORT_ADDRESS = 0x080000D1, /* 16 bits */
  ADP_IB_MANUF_MAX_REPAIR_RESEND_ATTEMPTS = 0x080000D2, /* 8 bits */
  ADP_IB_MANUF_DISABLE_AUTO_RREQ = 0x080000D3, /* 8 bits (bool) */
  ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT = 0x080000D5, /* 16 bits */
  ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_TIMEOUT_COUNT = 0x080000D6, /* 16 bits */
  ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_ROUTE_ERROR_COUNT = 0x080000D7, /* 16 bits */
  ADP_IB_MANUF_PENDING_DATA_IND_SHORT_ADDRESS = 0x080000D8, /* 16 bits */
  ADP_IB_MANUF_GET_BAND_CONTEXT_TONES = 0x080000D9, /* 8 bits */
  ADP_IB_MANUF_UPDATE_NON_VOLATILE_DATA = 0x080000DA, /* 8 bits (bool) */
  ADP_IB_MANUF_DISCOVER_ROUTE_GLOBAL_SEQ_NUM = 0x080000C9, /* 16 bits */
  ADP_IB_MANUF_FRAGMENT_DELAY = 0x080000DC, /* 16 bits */
  ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_ENABLED = 0x080000DD, /* 8 bits (bool) */
  ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_FACTOR = 0x080000DE, /* 16 bits */
  ADP_IB_MANUF_BLACKLIST_TABLE_COUNT = 0x080000DF, /* 16 bits */
  ADP_IB_MANUF_BROADCAST_LOG_TABLE_COUNT = 0x080000E0, /* 16 bits */
  ADP_IB_MANUF_CONTEXT_INFORMATION_TABLE_COUNT = 0x080000E1, /* 16 bits */
  ADP_IB_MANUF_GROUP_TABLE_COUNT = 0x080000E2, /* 16 bits */
  ADP_IB_MANUF_ROUTING_TABLE_ELEMENT = 0x080000E3,  /* 9 Byte entries */
  ADP_IB_MANUF_SET_PHASEDIFF_PREQ_PREP = 0x080000E4,  /* 8 bits (bool) */
  ADP_IB_MANUF_HYBRID_PROFILE = 0x080000E5,  /* 8 bits (bool) */
  ADP_IB_MANUF_LAST_PHASEDIFF = 0x080000E6  /* 8 bits */
};

// List of errors returned by the ADP
enum EAdpStatus {
  /// Success
  G3_SUCCESS = 0x00,
  /// Invalid request
  G3_INVALID_REQUEST = 0xA1,
  /// Request failed
  G3_FAILED = 0xA2,
  /// Invalid IPv6 frame
  G3_INVALID_IPV6_FRAME = 0xA3,
  /// Not permited
  G3_NOT_PERMITED = 0xA4,
  /// No route to destination
  G3_ROUTE_ERROR = 0xA5,
  /// Operation timed out
  G3_TIMEOUT = 0xA6,
  /// An attempt to write to a MAC PIB attribute that is in a table failed because the specified table index was out of range.
  G3_INVALID_INDEX = 0xA7,
  /// A parameter in the primitive is either not supported or is out of the valid range.
  G3_INVALID_PARAMETER = 0xA8,
  /// A scan operation failed to find any network beacons.
  G3_NO_BEACON = 0xA9,
  /// A SET/GET request was issued with the identifier of an attribute that is read only.
  G3_READ_ONLY = 0xB0,
  /// A SET/GET request was issued with the identifier of a PIB attribute that is not supported.
  G3_UNSUPPORTED_ATTRIBUTE = 0xB1,
  /// The path discovery has only a part of the path to its desired final destination.
  G3_INCOMPLETE_PATH = 0xB2,
  /// Busy: operation already in progress.
  G3_BUSY = 0xB3,
  /// Not enough resources
  G3_NO_BUFFERS = 0xB4,
  /// Trickle redundancy detected
  G3_TRICKLE_REDUNDANT = 0xB5,
  /// Error internal
  G3_ERROR_INTERNAL = 0xFF
};

struct TGroupMasterKey {
  // The id of the key
  uint8_t m_u8KeyId;
  // The key value
  uint8_t m_au8Key[16];
};
#endif

