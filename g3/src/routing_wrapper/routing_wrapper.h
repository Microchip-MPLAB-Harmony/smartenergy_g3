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

#ifndef __ROUTING_API_H__
#define __ROUTING_API_H__

#include "adp.h"

void Routing_Reset(void);

bool Routing_IsDisabled(void);

bool Routing_IsAutoRREQDisabled(void);

bool Routing_AdpDefaultCoordRouteEnabled(void);

uint8_t Routing_AdpRREPWait(void);

uint16_t Routing_GetDiscoverRouteGlobalSeqNo(void);
void Routing_SetDiscoverRouteGlobalSeqNo(uint16_t seqNo);

void RoutingGetMib(uint32_t u32AttributeId, uint16_t u16AttributeIndex, ADP_GET_CFM_PARAMS* pGetConfirm);

void RoutingSetMib(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue, ADP_SET_CFM_PARAMS* pSetConfirm);

ROUTING_TABLE_ENTRY *Routing_AddRouteEntry(ROUTING_TABLE_ENTRY *pNewEntry, bool *pbTableFull);
ROUTING_TABLE_ENTRY *Routing_GetRouteEntry(uint16_t u16DestinationAddress);
uint32_t Routing_GetRouteCount(void);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
typedef void (*ROUTING_WRP_DISCOVER_PATH_CALLBACK)(uint8_t status, ADP_PATH_DESCRIPTOR *pPathDescriptor);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DiscoverPath(uint16_t u16DstAddr, uint8_t u8MetricType, ROUTING_WRP_DISCOVER_PATH_CALLBACK callback);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_NotifyRouteError(uint16_t u16DstAddr, uint16_t u16UnreachableAddress, uint8_t u8ErrorCode);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
typedef void (*ROUTING_WRP_DISCOVER_ROUTE_CALLBACK)(uint8_t u8Status, uint16_t u16DstAddr, uint16_t u16NextHop, void *pUserData);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DiscoverRoute(uint16_t u16DstAddr, uint8_t u8MaxHops, bool bRepair, void *pUserData,
  ROUTING_WRP_DISCOVER_ROUTE_CALLBACK fnctDiscoverCallback);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_ProcessMessage(uint16_t u16MacSrcAddr, uint8_t u8MediaType, ADP_MODULATION_PLC eModulation, uint8_t u8ActiveTones,
  uint8_t u8SubCarriers, uint8_t u8LQI, uint16_t u16MessageLength, uint8_t *pMessageBuffer);

/**********************************************************************************************************************/
/** Refresh the valid time of the route
 **********************************************************************************************************************/
void Routing_RefreshRoute(uint16_t u16DstAddr);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_AddCircularRoute(uint16_t m_u16LastCircularRouteAddress);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Routing_DeleteRoute(uint16_t u16DstAddr);

/**********************************************************************************************************************/
/** returns true if route is known
 **********************************************************************************************************************/
bool Routing_RouteExists(uint16_t u16DestinationAddress);

/**********************************************************************************************************************/
/** Before calling this function, check if route exists
 **********************************************************************************************************************/
uint16_t Routing_GetRouteAndMediaType(uint16_t u16DestinationAddress, uint8_t *pu8MediaType);

/**********************************************************************************************************************/
/** Add new candidate route
 **********************************************************************************************************************/
ROUTING_TABLE_ENTRY *Routing_AddRoute(uint16_t u16DstAddr, uint16_t u16NextHopAddr, uint8_t u8MediaType, bool *pbTableFull);

/**********************************************************************************************************************/
/** Returns true if the address is in the Destination Address Set (CCTT#183)
 **********************************************************************************************************************/
bool Routing_IsInDestinationAddressSet(uint16_t u16Addr);

/**********************************************************************************************************************/
/** Adds node to blacklist for a given medium
 **********************************************************************************************************************/
void Routing_AddBlacklistOnMedium(uint16_t u16Addr, uint8_t u8MediaType);

/**********************************************************************************************************************/
/** Removes a node from blacklist for a given medium
 **********************************************************************************************************************/
void Routing_RemoveBlacklistOnMedium(uint16_t u16Addr, uint8_t u8MediaType);

// *****************************************************************************
/* Function:
    void Routing_Wrapper_Tasks
    (
      void
    )

  Summary:
    Maintains Routing Wrapper State Machine.

  Description:
    Maintains the Routing Wrapper State Machine.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // ...

    while (true)
    {
        Routing_Wrapper_Tasks();
    
        // Do other tasks
    }
    </code>

  Remarks:
    None.
*/
void Routing_Wrapper_Tasks(void);

#endif
