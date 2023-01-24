#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "adp.h"
#include "mac_wrapper.h"
#include "routing_types.h"
#include "routing_wrapper.h"
<#if LOADNG_ENABLE == true>
#include "loadng.h"
</#if>
#include "service/log_report/srv_log_report.h"

/**********************************************************************************************************************/
/** Internal variables
 **********************************************************************************************************************/
struct TRoutingTables g_RoutingTables;

<#if LOADNG_ENABLE == true>
struct TQueueElement g_PendingRREQTable[LOADNG_PENDING_RREQ_TABLE_SIZE];
struct TRRepGeneration g_RRepGenerationTable[LOADNG_RREP_GEN_TABLE_SIZE];
  struct TRReqForwarding g_RReqForwardingTable[LOADNG_RREQ_FORWARD_TABLE_SIZE];
struct TDiscoverRouteEntry g_DiscoverRouteTable[LOADNG_DISCOVER_ROUTE_TABLE_SIZE];
struct TAdpRoutingTableEntry g_AdpRoutingTable[G3_ADP_ROUTING_TABLE_SIZE];
struct TAdpBlacklistTableEntry g_AdpBlacklistTable[G3_ADP_BLACKLIST_TABLE_SIZE];
struct TAdpRoutingTableEntry g_AdpRoutingSet[G3_ADP_ROUTING_SET_SIZE];
uint16_t g_AdpDestinationAddressSet[G3_ADP_DESTINATION_ADDR_SET_SIZE];
</#if>

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_Reset(uint8_t u8Band)
{
<#if LOADNG_ENABLE == true>
  // Store the table sizes, to make LoadNG library independent of table sizes.
  g_RoutingTables.m_AdpRoutingTableSize = ADP_ROUTING_TABLE_SIZE;
  g_RoutingTables.m_AdpBlacklistTableSize = ADP_BLACKLIST_TABLE_SIZE;
  g_RoutingTables.m_AdpRoutingSetSize = ADP_ROUTING_SET_SIZE;
  g_RoutingTables.m_AdpDestinationAddressSetSize = ADP_DESTINATION_ADDRESS_SET_TABLE_SIZE;

  g_RoutingTables.m_PendingRREQTableSize = LOADNG_PENDING_RREQ_TABLE_SIZE;
  g_RoutingTables.m_RRepGenerationTableSize = LOADNG_RREP_GENERATION_TABLE_SIZE;
  g_RoutingTables.m_DiscoverRouteTableSize = LOADNG_DISCOVER_ROUTE_TABLE_SIZE;
  g_RoutingTables.m_RReqForwardingTableSize = LOADNG_RREQ_FORWARDING_TABLE_SIZE;

  g_RoutingTables.m_AdpRoutingTable = g_AdpRoutingTable;
  g_RoutingTables.m_AdpBlacklistTable = g_AdpBlacklistTable;
  g_RoutingTables.m_AdpRoutingSet = g_AdpRoutingSet;
  g_RoutingTables.m_AdpDestinationAddressSet = g_AdpDestinationAddressSet;

  g_RoutingTables.m_PendingRREQTable = g_PendingRREQTable;
  g_RoutingTables.m_RRepGenerationTable = g_RRepGenerationTable;
  g_RoutingTables.m_DiscoverRouteTable = g_DiscoverRouteTable;
  g_RoutingTables.m_RReqForwardingTable = g_RReqForwardingTable;

  LOADNG_Reset(u8Band, &g_RoutingTables);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Routing_IsDisabled()
{
<#if LOADNG_ENABLE == true>
  struct TAdpGetConfirm adpGetConfirm;
  RoutingGetMib(ADP_IB_DISABLE_DEFAULT_ROUTING, 0, &adpGetConfirm);
  return (bool)adpGetConfirm.m_au8AttributeValue[0];
<#else>
  return true;
</#if>
}

bool Routing_IsAutoRREQDisabled()
{
<#if LOADNG_ENABLE == true>
  struct TAdpGetConfirm adpGetConfirm;
  RoutingGetMib(ADP_IB_MANUF_DISABLE_AUTO_RREQ, 0, &adpGetConfirm);
  return (bool)adpGetConfirm.m_au8AttributeValue[0];
<#else>
  return true;
</#if>
}

bool Routing_AdpDefaultCoordRouteEnabled()
{
<#if LOADNG_ENABLE == true>
  struct TAdpGetConfirm adpGetConfirm;
  RoutingGetMib(ADP_IB_DEFAULT_COORD_ROUTE_ENABLED, 0, &adpGetConfirm);
  return (bool)adpGetConfirm.m_au8AttributeValue[0];
<#else>
  return false;
</#if>
}

uint8_t Routing_AdpRREPWait()
{
<#if LOADNG_ENABLE == true>
  struct TAdpGetConfirm adpGetConfirm;
  RoutingGetMib(ADP_IB_RREP_WAIT, 0, &adpGetConfirm);
  return adpGetConfirm.m_au8AttributeValue[0];
<#else>
  return 4;
</#if>
}

uint16_t Routing_GetDiscoverRouteGlobalSeqNo()
{
<#if LOADNG_ENABLE == true>
  uint16_t u16Value;
  struct TAdpGetConfirm adpGetConfirm;
  RoutingGetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, &adpGetConfirm);
  memcpy(&u16Value, &adpGetConfirm.m_au8AttributeValue, 2);
  return u16Value;
<#else>
  return 1;
</#if>
}

void Routing_SetDiscoverRouteGlobalSeqNo(uint16_t seqNo)
{
<#if LOADNG_ENABLE == true>
  struct TAdpSetConfirm adpSetConfirm;
  RoutingSetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2, (uint8_t *)&seqNo, &adpSetConfirm);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void RoutingGetMib(uint32_t u32AttributeId, uint16_t u16AttributeIndex, struct TAdpGetConfirm *pGetConfirm)
{
<#if LOADNG_ENABLE == true>
  LOADNG_GetMib(u32AttributeId, u16AttributeIndex, pGetConfirm);
<#else>
  LOG_DBG(Log("Get request. Attribute: %08X; Index: %u", u32AttributeId, u16AttributeIndex));
  if ((u32AttributeId == ADP_IB_RREP_WAIT) ||
    (u32AttributeId == ADP_IB_ROUTING_TABLE) ||
    (u32AttributeId == ADP_IB_ROUTING_TABLE) ||
    (u32AttributeId == ADP_IB_MANUF_ROUTING_TABLE_COUNT) ||
    (u32AttributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
    (u32AttributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
    (u32AttributeId == ADP_IB_ROUTING_TABLE_ENTRY_TTL) ||
    (u32AttributeId == ADP_IB_KR) ||
    (u32AttributeId == ADP_IB_KM) ||
    (u32AttributeId == ADP_IB_KC) ||
    (u32AttributeId == ADP_IB_KQ) ||
    (u32AttributeId == ADP_IB_KH) ||
    (u32AttributeId == ADP_IB_RREQ_RETRIES) ||
    (u32AttributeId == ADP_IB_DESTINATION_ADDRESS_SET) ||
    (u32AttributeId == ADP_IB_RREQ_WAIT) ||
    (u32AttributeId == ADP_IB_WEAK_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_KRT) ||
    (u32AttributeId == ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER) ||
    (u32AttributeId == ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED) ||
    (u32AttributeId == ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS) ||
    (u32AttributeId == ADP_IB_ADD_REV_LINK_COST) ||
    (u32AttributeId == ADP_IB_PATH_DISCOVERY_TIME) ||
    (u32AttributeId == ADP_IB_METRIC_TYPE) ||
    (u32AttributeId == ADP_IB_LOW_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_HIGH_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_RLC_ENABLED) ||
    (u32AttributeId == ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT)) {
    pGetConfirm->m_u8Status = G3_INVALID_PARAMETER;
  }
  else if (u32AttributeId == ADP_IB_DISABLE_DEFAULT_ROUTING) {
    pGetConfirm->m_u8AttributeLength = 1;
    pGetConfirm->m_au8AttributeValue[0] = 1;   // Disabled by compilation
    pGetConfirm->m_u8Status = G3_SUCCESS;
  }
  else if (u32AttributeId == ADP_IB_DEFAULT_COORD_ROUTE_ENABLED) {
    pGetConfirm->m_u8AttributeLength = 1;
    pGetConfirm->m_au8AttributeValue[0] = 0;   // Disabled by compilation
    pGetConfirm->m_u8Status = G3_SUCCESS;
  }
  else {
    pGetConfirm->m_u8Status = G3_UNSUPPORTED_ATTRIBUTE;
  }
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void RoutingSetMib(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue, struct TAdpSetConfirm *pSetConfirm)
{
<#if LOADNG_ENABLE == true>
  LOADNG_SetMib(u32AttributeId, u16AttributeIndex, u8AttributeLength, pu8AttributeValue, pSetConfirm);
<#else>
  if ((u32AttributeId == ADP_IB_RREP_WAIT) ||
    (u32AttributeId == ADP_IB_BLACKLIST_TABLE) ||
    (u32AttributeId == ADP_IB_ROUTING_TABLE) ||
    (u32AttributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
    (u32AttributeId == ADP_IB_ROUTING_TABLE_ENTRY_TTL) ||
    (u32AttributeId == ADP_IB_MANUF_DISABLE_AUTO_RREQ) ||
    (u32AttributeId == ADP_IB_KR) ||
    (u32AttributeId == ADP_IB_KM) ||
    (u32AttributeId == ADP_IB_KC) ||
    (u32AttributeId == ADP_IB_KQ) ||
    (u32AttributeId == ADP_IB_KH) ||
    (u32AttributeId == ADP_IB_RREQ_RETRIES) ||
    (u32AttributeId == ADP_IB_DESTINATION_ADDRESS_SET) ||
    (u32AttributeId == ADP_IB_RREQ_WAIT) ||
    (u32AttributeId == ADP_IB_WEAK_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_KRT) ||
    (u32AttributeId == ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER) ||
    (u32AttributeId == ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED) ||
    (u32AttributeId == ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS) ||
    (u32AttributeId == ADP_IB_ADD_REV_LINK_COST) ||
    (u32AttributeId == ADP_IB_PATH_DISCOVERY_TIME) ||
    (u32AttributeId == ADP_IB_METRIC_TYPE) ||
    (u32AttributeId == ADP_IB_LOW_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_HIGH_LQI_VALUE) ||
    (u32AttributeId == ADP_IB_RLC_ENABLED) ||
    (u32AttributeId == ADP_IB_MANUF_SET_PHASEDIFF_PREQ_PREP) ||
    (u32AttributeId == ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT)) {
    pSetConfirm->m_u8Status = G3_INVALID_PARAMETER;
  }
  else if (u32AttributeId == ADP_IB_DEFAULT_COORD_ROUTE_ENABLED) {
    pSetConfirm->m_u8Status = G3_READ_ONLY;
  }
  else if (u32AttributeId == ADP_IB_DISABLE_DEFAULT_ROUTING) {
    pSetConfirm->m_u8Status = G3_READ_ONLY;
  }
  else {
    pSetConfirm->m_u8Status = G3_UNSUPPORTED_ATTRIBUTE;
  }
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DiscoverPath(uint16_t u16DstAddr, uint8_t u8MetricType, LOADNG_DiscoverPath_Callback callback)
{
<#if LOADNG_ENABLE == true>
  if (!Routing_IsDisabled()) {
    LOADNG_DiscoverPath(u16DstAddr, u8MetricType, callback);
  }
  else {
    callback(G3_INVALID_REQUEST, NULL);
  }
<#else>
  callback(G3_INVALID_REQUEST, NULL);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_NotifyRouteError(uint16_t u16DstAddr, uint16_t u16UnreachableAddress, uint8_t u8ErrorCode)
{
<#if LOADNG_ENABLE == true>
  LOADNG_NotifyRouteError(u16DstAddr, u16UnreachableAddress, u8ErrorCode);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DiscoverRoute(
  uint16_t u16DstAddr,
  uint8_t u8MaxHops,
  bool bRepair,
  void *pUserData,
  LOADNG_DiscoverRoute_Callback fnctDiscoverCallback
  )
{
<#if LOADNG_ENABLE == true>
  if (!Routing_IsDisabled()) {
    LOADNG_DiscoverRoute(u16DstAddr, u8MaxHops, bRepair, pUserData, fnctDiscoverCallback);
  }
  else {
    fnctDiscoverCallback(G3_ROUTE_ERROR, u16DstAddr, 0xFFFF, NULL);
  }
<#else>
  fnctDiscoverCallback(G3_ROUTE_ERROR, u16DstAddr, 0xFFFF, NULL);
</#if>
}

/**********************************************************************************************************************/
/** Returns true if the address is in the Destination Address Set (CCTT#183)
 **********************************************************************************************************************/
bool Routing_IsInDestinationAddressSet(uint16_t u16Addr)
{
<#if LOADNG_ENABLE == true>
  return LOADNG_IsInDestinationAddressSet(u16Addr);
<#else>
  return false;
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_ProcessMessage(uint16_t u16MacSrcAddr, uint8_t u8MediaType, enum EAdpMac_Modulation eModulation, uint8_t u8ActiveTones, uint8_t u8Subcarriers, uint8_t u8LQI,
  uint16_t u16MessageLength, uint8_t *pMessageBuffer)
{
<#if LOADNG_ENABLE == true>
  LOADNG_ProcessMessage(u16MacSrcAddr, u8MediaType, eModulation, u8ActiveTones, u8Subcarriers, u8LQI, u16MessageLength, pMessageBuffer);
</#if>
}

/**********************************************************************************************************************/
/** Creates a new route
 **********************************************************************************************************************/
struct TAdpRoutingTableEntry *Routing_AddRoute(uint16_t u16DstAddr, uint16_t u16NextHopAddr, uint8_t u8MediaType, bool *pbTableFull)
{
<#if LOADNG_ENABLE == true>
  return LOADNG_AddRoute(u16DstAddr, u16NextHopAddr, u8MediaType, pbTableFull);
<#else>
  *pbTableFull = false;
  return NULL;
</#if>
}

/**********************************************************************************************************************/
/** Refresh the valid time of the route
 * This function is called when a message is sent and confirmed by the MAC layer: also set the bidirectional flag
 **********************************************************************************************************************/
void Routing_RefreshRoute(uint16_t u16DstAddr)
{
<#if LOADNG_ENABLE == true>
  LOADNG_RefreshRoute(u16DstAddr);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_AddCircularRoute(uint16_t m_u16LastCircularRouteAddress)
{
<#if LOADNG_ENABLE == true>
  LOADNG_AddCircularRoute(m_u16LastCircularRouteAddress);
</#if>
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DeleteRoute(uint16_t u16DstAddr)
{
<#if LOADNG_ENABLE == true>
  LOADNG_DeleteRoute(u16DstAddr);
</#if>
}

/**********************************************************************************************************************/
/** Returns true if route is known
 **********************************************************************************************************************/
bool Routing_RouteExists(uint16_t u16DestinationAddress)
{
<#if LOADNG_ENABLE == true>
  return LOADNG_RouteExists(u16DestinationAddress);
<#else>
  return false;
</#if>
}

/**********************************************************************************************************************/
/** Before calling this function, check if route exists
 **********************************************************************************************************************/
uint16_t Routing_GetRouteAndMediaType(uint16_t u16DestinationAddress, uint8_t *pu8MediaType)
{
<#if LOADNG_ENABLE == true>
  return LOADNG_GetRouteAndMediaType(u16DestinationAddress, pu8MediaType);
<#else>
  struct TAdpMacGetConfirm adpMacGetConfirm;
  uint16_t u16AdpShortAddress;
  AdpMacGetRequestSync(MAC_WRP_PIB_SHORT_ADDRESS, 0, &adpMacGetConfirm);
  memcpy(&u16AdpShortAddress, &adpMacGetConfirm.m_au8AttributeValue, 2);
  *pu8MediaType = 0;
  return u16AdpShortAddress;
</#if>
}

/**********************************************************************************************************************/
/** Inserts a route in the routing table
 **********************************************************************************************************************/
struct TAdpRoutingTableEntry *Routing_AddRouteEntry(struct TAdpRoutingTableEntry *pNewEntry, bool *pbTableFull)
{
  struct TAdpRoutingTableEntry *pRet = 0L;
  *pbTableFull = false;
<#if LOADNG_ENABLE == true>
  if (!Routing_IsDisabled()) {
    pRet = LOADNG_AddRouteEntry(pNewEntry, pbTableFull);
  }
</#if>
  return pRet;
}

/**********************************************************************************************************************/
/** Gets a pointer to Route Entry. before calling this function, check if route exists (LOADNG_RouteExists)
 **********************************************************************************************************************/
struct TAdpRoutingTableEntry *Routing_GetRouteEntry(uint16_t u16DestinationAddress)
{
  struct TAdpRoutingTableEntry *pRet = 0L;
<#if LOADNG_ENABLE == true>
  if (!Routing_IsDisabled()) {
    pRet = LOADNG_GetRouteEntry(u16DestinationAddress);
  }
</#if>
  return pRet;
}

/**********************************************************************************************************************/
/** Gets the route count
 **********************************************************************************************************************/
uint32_t Routing_GetRouteCount(void)
{
  uint32_t u32Count;
  u32Count = 0;
<#if LOADNG_ENABLE == true>
  if (!Routing_IsDisabled()) {
    u32Count = LOADNG_GetRouteCount();
  }
</#if>
  return u32Count;
}

/**********************************************************************************************************************/
/** Adds node to blacklist for a given medium
 **********************************************************************************************************************/
void Routing_AddBlacklistOnMedium(uint16_t u16Addr, uint8_t u8MediaType)
{
<#if LOADNG_ENABLE == true>
  LOADNG_AddBlacklistOnMedium(u16Addr, u8MediaType);
</#if>
}

/**********************************************************************************************************************/
/** Removes a node from blacklist for a given medium
 **********************************************************************************************************************/
void Routing_RemoveBlacklistOnMedium(uint16_t u16Addr, uint8_t u8MediaType)
{
<#if LOADNG_ENABLE == true>
  LOADNG_RemoveBlacklistOnMedium(u16Addr, u8MediaType);
</#if>
}
