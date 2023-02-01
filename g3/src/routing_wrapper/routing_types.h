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

#include "routing_wrapper.h"
#include "stack/service/queue/srv_queue.h"
#include "system/time/sys_time.h"
#include "mac_wrapper.h"

#define RERR_CODE_NO_AVAILABLE_ROUTE   0
#define RERR_CODE_HOP_LIMIT_EXCEEDED   1

typedef struct {
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
  uint8_t m_u8MediaType : 1;

  /// Absolute time in milliseconds when the entry expires
  int32_t m_i32ValidTime;
} ROUTING_TABLE_ENTRY;

struct TAdpBlacklistTableEntry {
  uint16_t m_u16Addr;
  uint8_t m_u8MediaType;
  /// Absolute time in milliseconds when the entry expires
  int32_t m_i32ValidTime;
};

typedef struct TDiscoverRouteEntry_tag {
  /* Pointer to the previous object of the queue */
  struct TDiscoverRouteEntry_tag *prev;                
    
  /* Pointer to the next object of the queue */
  struct TDiscoverRouteEntry_tag *next;     
    
  // Callback called when the discovery is finished
  ROUTING_WRP_DISCOVER_ROUTE_CALLBACK m_fcntCallback;

  // Final destination of the discover
  uint16_t m_u16DstAddr;

  // Max hops parameter
  uint8_t m_u8MaxHops;

  // Repair route: true / false
  bool m_bRepair;

  // User data
  void *m_pUserData;

  // Timer to control the discovery process if no response is received
  SYS_TIME_HANDLE m_Timer;
  
  // User data recovered by timer expiration
  uintptr_t m_pTimerUserData;  

  // Current try number
  uint8_t m_u8Try;

  // Discover route sequence number
  uint16_t m_u16SeqNo;
} TDiscoverRouteEntry;

struct TDiscoverPath {
  uint16_t m_u16DstAddr;
  // Callback called when the discovery is finished
  ROUTING_WRP_DISCOVER_PATH_CALLBACK m_fnctCallback;
  // Timer to control the discovery process if no response is received
  SYS_TIME_HANDLE m_Timer;
  bool discoverPathTimerExpired;
};

typedef struct TRRepGeneration_tag {
  /* Pointer to the previous object of the queue */
  struct TRRepGeneration_tag *prev;                
    
  /* Pointer to the next object of the queue */
  struct TRRepGeneration_tag *next;   
  
  uint16_t m_u16OrigAddr;   // RREQ originator (and final destination of RREP)
  uint16_t m_u16DstAddr;   // RREQ destination (and originator of RREP). Unused in SPEC-15
  uint16_t m_u16RREQSeqNum;   // RREQ Sequence number to be able to manage different RREQ from same node
  uint8_t m_u8Flags;   // Flags received from RREQ
  uint8_t m_u8MetricType;   // MetricType received from RREQ
  uint8_t m_bWaitingForAck;   // Flag to indicate entry is waiting for ACK, timer can be expired but RREP were not sent due to channel saturation
  SYS_TIME_HANDLE m_Timer;   // Timer to control the RREP sending
  uintptr_t m_pTimerUserData;  // User data recovered by timer expiration
} TRRepGeneration;

typedef struct TRReqForwarding_tag {
  /* Pointer to the previous object of the queue */
  struct TRReqForwarding_tag *prev;                
    
  /* Pointer to the next object of the queue */
  struct TRReqForwarding_tag *next;   
  
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
  SYS_TIME_HANDLE m_Timer;   // Timer to control the RREQ sending
} TRReqForwarding;

struct TRouteCostParameters {
  ADP_MODULATION_PLC m_eModulation;
  uint8_t m_u8NumberOfActiveTones;
  uint8_t m_u8NumberOfSubCarriers;
  uint8_t m_u8LQI;
};

// *****************************************************************************
/* Description of an element in the pending routing request table

  Summary:
   Description of an element in the pending routing request table.

  Description:
    This structure contains the data stored in each element of the pending
    routing request table.

  Remarks:
    None.
*/
typedef struct _pending_rreq_table_element_tag
{
    /* Pointer to the previous object of the queue */
    struct _pending_rreq_table_element_tag *prev;                
    
    /* Pointer to the next object of the queue */
    struct _pending_rreq_table_element_tag *next;                

    /* Pointer to the used data */
    void *userData;   
    
    /* Information about data type (optional information if needed) */
    uint8_t dataType;

} PENDING_RREQ_TABLE_ELEMENT;

typedef struct
{
    uint8_t pendingRREQTableSize;
    uint8_t rrepGenerationTableSize;
    uint8_t discoverRouteTableSize;
    uint8_t rreqForwardingTableSize;
    /**
    * Contains the length of the Routing table
    */
    uint16_t adpRoutingTableSize;
    /**
    * Contains the length of the blacklisted neighbours table
    */
    uint8_t adpBlacklistTableSize;
    /**
    * Contains the length of the routing set
    */
    uint16_t adpRoutingSetSize;
    /**
    * Contains the size of the destination address set
    */
    uint16_t adpDestinationAddressSetSize;
    /**
    * Contains the routing table
    */

    PENDING_RREQ_TABLE_ELEMENT* pendingRREQTable;
    TRRepGeneration* rrepGenerationTable;
    TDiscoverRouteEntry* discoverRouteTable;
    TRReqForwarding* rreqForwardingTable;

    ROUTING_TABLE_ENTRY* adpRoutingTable;
    /**
    * Contains the list of the blacklisted neighbours
    */
    struct TAdpBlacklistTableEntry* adpBlacklistTable;
    /**
    * Contains the routing set
    */
    ROUTING_TABLE_ENTRY* adpRoutingSet;
    /**
    * Contains the list of destination addresses.
    */
    uint16_t* adpDestinationAddressSet;

} ROUTING_TABLES;
#endif

