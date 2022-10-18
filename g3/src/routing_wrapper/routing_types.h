/**********************************************************************************************************************/
/** \addtogroup AdaptationSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains configuration definitions used to tune the memory usage of the Routing layer.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef __ROUTING_TYPES_H__
#define __ROUTING_TYPES_H__

#include "routing.h"
#include "stack/service/queue/srv_queue.h"
#include "system/time/sys_time.h"

#define RERR_CODE_NO_AVAILABLE_ROUTE   0
#define RERR_CODE_HOP_LIMIT_EXCEEDED   1

struct TAdpRoutingTableEntry {
  /// The 16 bit short link layer address of the final destination of a route
  uint16_t m_u16DstAddr;
  /// The 16 bit short link layer addresses of the next hop node to the destination
  uint16_t m_u16NextHopAddr;

  uint16_t m_u16SeqNo;
  uint16_t m_u16RouteCost;
  uint8_t m_u8HopCount : 4;
  uint8_t m_u8WeakLinkCount : 4;

  uint8_t m_u8ValidSeqNo : 1;
  uint8_t m_u8Bidirectional : 1;
  uint8_t m_u8RREPSent : 1;
  uint8_t m_u8MetricType : 2;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType : 1;
#endif

  /// Absolute time in milliseconds when the entry expires
  int32_t m_i32ValidTime;
};

struct TAdpBlacklistTableEntry {
  uint16_t m_u16Addr;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
  /// Absolute time in milliseconds when the entry expires
  int32_t m_i32ValidTime;
};

struct TDiscoverRouteEntry {
  // Callback called when the discovery is finished
  LOADNG_DiscoverRoute_Callback m_fcntCallback;

  // Final destination of the discover
  uint16_t m_u16DstAddr;

  // Max hops parameter
  uint8_t m_u8MaxHops;

  // Repair route: true / false
  bool m_bRepair;

  // User data
  void *m_pUserData;

  // Timer to control the discovery process if no response is received
  struct TTimer m_Timer;

  // Current try number
  uint8_t m_u8Try;

  // Discover route sequence number
  uint16_t m_u16SeqNo;
};

struct TDiscoverPath {
  uint16_t m_u16DstAddr;
  // Callback called when the discovery is finished
  LOADNG_DiscoverPath_Callback m_fnctCallback;
  // Timer to control the discovery process if no response is received
  struct TTimer m_Timer;
};

struct TRRepGeneration {
  uint16_t m_u16OrigAddr;   // RREQ originator (and final destination of RREP)
  uint16_t m_u16DstAddr;   // RREQ destination (and originator of RREP). Unused in SPEC-15
  uint16_t m_u16RREQSeqNum;   // RREQ Sequence number to be able to manage different RREQ from same node
  uint8_t m_u8Flags;   // Flags received from RREQ
  uint8_t m_u8MetricType;   // MetricType received from RREQ
  uint8_t m_bWaitingForAck;   // Flag to indicate entry is waiting for ACK, timer can be expired but RREP were not sent due to channel saturation
  struct TTimer m_Timer;   // Timer to control the RREP sending
};

struct TRReqForwarding {
  uint16_t m_u16OrigAddr;   // RREQ originator
  uint16_t m_u16DstAddr;   // RREQ destination
  uint16_t m_u16SeqNum;   // RREQ Sequence number
  uint16_t m_u16RouteCost;   // RREQ Route Cost
  uint8_t m_u8Flags;   // Flags received from RREQ
  uint8_t m_u8MetricType;   // MetricType received from RREQ
  uint8_t m_u8HopLimit;   // Hop Limit received from RREQ
  uint8_t m_u8HopCount;   // Hop Count left
  uint8_t m_u8WeakLinkCount;   // RREQ Weak Link Count
  uint8_t m_u8RsvBits;   // Reserved bits
  uint8_t m_u8ClusterCounter;   // Cluster Counter
  struct TTimer m_Timer;   // Timer to control the RREQ sending
};

struct TRouteCostParameters {
  enum EAdpMac_Modulation m_eModulation;
  uint8_t m_u8NumberOfActiveTones;
  uint8_t m_u8NumberOfSubCarriers;
  uint8_t m_u8LQI;
};

struct TRoutingTables {
  uint8_t m_PendingRREQTableSize;
  uint8_t m_RRepGenerationTableSize;
  uint8_t m_DiscoverRouteTableSize;
  uint8_t m_RReqForwardingTableSize;
  /**
   * Contains the length of the Routing table
   */
  uint16_t m_AdpRoutingTableSize;
  /**
   * Contains the length of the blacklisted neighbours table
   */
  uint8_t m_AdpBlacklistTableSize;
  /**
   * Contains the length of the routing set
   */
  uint16_t m_AdpRoutingSetSize;
  /**
   * Contains the size of the destination address set
   */
  uint16_t m_AdpDestinationAddressSetSize;
  /**
   * Contains the routing table
   */

  struct TQueueElement *m_PendingRREQTable;
  struct TRRepGeneration *m_RRepGenerationTable;
  struct TDiscoverRouteEntry *m_DiscoverRouteTable;
  struct TRReqForwarding *m_RReqForwardingTable;

  struct TAdpRoutingTableEntry *m_AdpRoutingTable;
  /**
   * Contains the list of the blacklisted neighbours
   */
  struct TAdpBlacklistTableEntry *m_AdpBlacklistTable;
  /**
   * Contains the routing set
   */
  struct TAdpRoutingTableEntry *m_AdpRoutingSet;
  /**
   * Contains the list of destination addresses.
   */
  uint16_t *m_AdpDestinationAddressSet;
};
#endif

