/*******************************************************************************
  G3 Routing Wrapper Source File

  Company:
    Microchip Technology Inc.

  File Name:
    routing_wrapper.c

  Summary:
    G3 Routing Wrapper API Source File

  Description:
    This file contains implementation of the API to be used by upper layers when
    accessing G3 Routing layers.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "routing_wrapper.h"
<#if LOADNG_ENABLE == true>
#include "loadng.h"
</#if>
#include "service/log_report/srv_log_report.h"

<#if LOADNG_ENABLE == true>
// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

/* Routing tables */
ROUTING_PENDING_RREQ_ENTRY routingWrpPendingRReqTable[LOADNG_PENDING_RREQ_TABLE_SIZE];
ROUTING_RREP_GENERATION_ENTRY routingWrpRRepGenerationTable[LOADNG_RREP_GEN_TABLE_SIZE];
ROUTING_RREQ_FORWARDING_ENTRY routingWrpRReqForwardingTable[LOADNG_RREQ_FORWARD_TABLE_SIZE];
ROUTING_DISCOVER_ROUTE_ENTRY routingWrpDiscoverRouteTable[LOADNG_DISCOVER_ROUTE_TABLE_SIZE];
ROUTING_TABLE_ENTRY routingWrpRoutingTable[G3_ADP_ROUTING_TABLE_SIZE];
ROUTING_BLACKLIST_ENTRY routingWrpBlacklistTable[G3_ADP_BLACKLIST_TABLE_SIZE];
ROUTING_TABLE_ENTRY routingWrpRoutingSet[G3_ADP_ROUTING_SET_SIZE];
uint16_t routingWrpDestinationAddressSet[G3_ADP_DESTINATION_ADDR_SET_SIZE];

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Routing Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

void ROUTING_WRP_Reset(MAC_WRP_HANDLE macWrpHandle)
{
<#if LOADNG_ENABLE == true>
    ROUTING_TABLES routingTables;

    /* Store the table sizes, to make LoadNG library independent of table sizes */
    routingTables.adpRoutingTableSize = G3_ADP_ROUTING_TABLE_SIZE;
    routingTables.adpBlacklistTableSize = G3_ADP_BLACKLIST_TABLE_SIZE;
    routingTables.adpRoutingSetSize = G3_ADP_ROUTING_SET_SIZE;
    routingTables.adpDestinationAddressSetSize = G3_ADP_DESTINATION_ADDR_SET_SIZE;
    routingTables.pendingRREQTableSize = LOADNG_PENDING_RREQ_TABLE_SIZE;
    routingTables.rrepGenerationTableSize = LOADNG_RREP_GEN_TABLE_SIZE;
    routingTables.discoverRouteTableSize = LOADNG_DISCOVER_ROUTE_TABLE_SIZE;
    routingTables.rreqForwardingTableSize = LOADNG_RREQ_FORWARD_TABLE_SIZE;

    /* Store pointers to tables */
    routingTables.adpRoutingTable = routingWrpRoutingTable;
    routingTables.adpBlacklistTable = routingWrpBlacklistTable;
    routingTables.adpRoutingSet = routingWrpRoutingSet;
    routingTables.adpDestinationAddressSet = routingWrpDestinationAddressSet;
    routingTables.pendingRREQTable = routingWrpPendingRReqTable;
    routingTables.rrepGenerationTable = routingWrpRRepGenerationTable;
    routingTables.discoverRouteTable = routingWrpDiscoverRouteTable;
    routingTables.rreqForwardingTable = routingWrpRReqForwardingTable;

    LOADNG_Reset(&routingTables, macWrpHandle);
</#if>
}

bool ROUTING_WRP_IsDisabled()
{
<#if LOADNG_ENABLE == true>
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_DISABLE_DEFAULT_ROUTING, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
<#else>
    return true;
</#if>
}

bool ROUTING_WRP_IsAutoRReqDisabled()
{
<#if LOADNG_ENABLE == true>
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_MANUF_DISABLE_AUTO_RREQ, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
<#else>
    return true;
</#if>
}

bool ROUTING_WRP_IsDefaultCoordRouteEnabled()
{
<#if LOADNG_ENABLE == true>
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_DEFAULT_COORD_ROUTE_ENABLED, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
<#else>
    return false;
</#if>
}

uint8_t ROUTING_WRP_GetRRepWait()
{
<#if LOADNG_ENABLE == true>
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_RREP_WAIT, 0, &getConfirm);
    return getConfirm.attributeValue[0];
<#else>
    return 4;
</#if>
}

uint16_t ROUTING_WRP_GetDiscoverRouteGlobalSeqNo()
{
<#if LOADNG_ENABLE == true>
    uint16_t u16Value;
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, &getConfirm);
    memcpy(&u16Value, &getConfirm.attributeValue, 2);
    return u16Value;
<#else>
    return 1;
</#if>
}

void ROUTING_WRP_SetDiscoverRouteGlobalSeqNo(uint16_t seqNo)
{
<#if LOADNG_ENABLE == true>
    ADP_SET_CFM_PARAMS setConfirm;
    ROUTING_WRP_SetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
        (uint8_t*) &seqNo, &setConfirm);
</#if>
}

void ROUTING_WRP_GetMib(uint32_t attributeId, uint16_t attributeIndex,
    ADP_GET_CFM_PARAMS* pGetConfirm)
{
<#if LOADNG_ENABLE == true>
    LOADNG_GetMib(attributeId, attributeIndex, pGetConfirm);
<#else>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Get request. Attribute: %08X; Index: %u \r\n", attributeId, attributeIndex);
    if ((attributeId == ADP_IB_RREP_WAIT) ||
        (attributeId == ADP_IB_ROUTING_TABLE) ||
        (attributeId == ADP_IB_ROUTING_TABLE) ||
        (attributeId == ADP_IB_MANUF_ROUTING_TABLE_COUNT) ||
        (attributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
        (attributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
        (attributeId == ADP_IB_ROUTING_TABLE_ENTRY_TTL) ||
        (attributeId == ADP_IB_KR) ||
        (attributeId == ADP_IB_KM) ||
        (attributeId == ADP_IB_KC) ||
        (attributeId == ADP_IB_KQ) ||
        (attributeId == ADP_IB_KH) ||
        (attributeId == ADP_IB_RREQ_RETRIES) ||
        (attributeId == ADP_IB_DESTINATION_ADDRESS_SET) ||
        (attributeId == ADP_IB_RREQ_WAIT) ||
        (attributeId == ADP_IB_WEAK_LQI_VALUE) ||
        (attributeId == ADP_IB_KRT) ||
        (attributeId == ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER) ||
        (attributeId == ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED) ||
        (attributeId == ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS) ||
        (attributeId == ADP_IB_ADD_REV_LINK_COST) ||
        (attributeId == ADP_IB_PATH_DISCOVERY_TIME) ||
        (attributeId == ADP_IB_METRIC_TYPE) ||
        (attributeId == ADP_IB_LOW_LQI_VALUE) ||
        (attributeId == ADP_IB_HIGH_LQI_VALUE) ||
        (attributeId == ADP_IB_RLC_ENABLED) ||
        (attributeId == ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT))
    {
        pGetConfirm->status = G3_INVALID_PARAMETER;
    }
    else if (attributeId == ADP_IB_DISABLE_DEFAULT_ROUTING)
    {
        pGetConfirm->attributeLength = 1;
        pGetConfirm->attributeValue[0] = 1;   // Disabled by compilation
        pGetConfirm->status = G3_SUCCESS;
    }
    else if (attributeId == ADP_IB_DEFAULT_COORD_ROUTE_ENABLED)
    {
        pGetConfirm->attributeLength = 1;
        pGetConfirm->attributeValue[0] = 0;   // Disabled by compilation
        pGetConfirm->status = G3_SUCCESS;
    }
    else
    {
        pGetConfirm->status = G3_UNSUPPORTED_ATTRIBUTE;
    }
</#if>
}

void ROUTING_WRP_SetMib(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue,
    ADP_SET_CFM_PARAMS* pSetConfirm)
{
<#if LOADNG_ENABLE == true>
    LOADNG_SetMib(attributeId, attributeIndex, attributeLength, pAttributeValue, pSetConfirm);
<#else>
    if ((attributeId == ADP_IB_RREP_WAIT) ||
        (attributeId == ADP_IB_BLACKLIST_TABLE) ||
        (attributeId == ADP_IB_ROUTING_TABLE) ||
        (attributeId == ADP_IB_UNICAST_RREQ_GEN_ENABLE) ||
        (attributeId == ADP_IB_ROUTING_TABLE_ENTRY_TTL) ||
        (attributeId == ADP_IB_MANUF_DISABLE_AUTO_RREQ) ||
        (attributeId == ADP_IB_KR) ||
        (attributeId == ADP_IB_KM) ||
        (attributeId == ADP_IB_KC) ||
        (attributeId == ADP_IB_KQ) ||
        (attributeId == ADP_IB_KH) ||
        (attributeId == ADP_IB_RREQ_RETRIES) ||
        (attributeId == ADP_IB_DESTINATION_ADDRESS_SET) ||
        (attributeId == ADP_IB_RREQ_WAIT) ||
        (attributeId == ADP_IB_WEAK_LQI_VALUE) ||
        (attributeId == ADP_IB_KRT) ||
        (attributeId == ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER) ||
        (attributeId == ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED) ||
        (attributeId == ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS) ||
        (attributeId == ADP_IB_ADD_REV_LINK_COST) ||
        (attributeId == ADP_IB_PATH_DISCOVERY_TIME) ||
        (attributeId == ADP_IB_METRIC_TYPE) ||
        (attributeId == ADP_IB_LOW_LQI_VALUE) ||
        (attributeId == ADP_IB_HIGH_LQI_VALUE) ||
        (attributeId == ADP_IB_RLC_ENABLED) ||
        (attributeId == ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT))
    {
        pSetConfirm->status = G3_INVALID_PARAMETER;
    }
    else if (attributeId == ADP_IB_DEFAULT_COORD_ROUTE_ENABLED)
    {
        pSetConfirm->status = G3_READ_ONLY;
    }
    else if (attributeId == ADP_IB_DISABLE_DEFAULT_ROUTING)
    {
        pSetConfirm->status = G3_READ_ONLY;
    }
    else
    {
        pSetConfirm->status = G3_UNSUPPORTED_ATTRIBUTE;
    }
</#if>
}

void ROUTING_WRP_DiscoverPath(uint16_t dstAddr, uint8_t metricType,
    ROUTING_WRP_DISCOVER_PATH_CALLBACK callback)
{
<#if LOADNG_ENABLE == true>
    if (ROUTING_WRP_IsDisabled() == false)
    {
        LOADNG_DiscoverPath(dstAddr, metricType, callback);
    }
    else
    {
        callback(G3_INVALID_REQUEST, NULL);
    }
<#else>
    callback(G3_INVALID_REQUEST, NULL);
</#if>
}

void ROUTING_WRP_NotifyRouteError(uint16_t dstAddr, uint16_t unreachableAddress,
    uint8_t errorCode)
{
<#if LOADNG_ENABLE == true>
    LOADNG_NotifyRouteError(dstAddr, unreachableAddress, errorCode);
</#if>
}

void ROUTING_WRP_DiscoverRoute(uint16_t dstAddr, uint8_t maxHops, bool repair,
    void *pUserData, ROUTING_WRP_DISCOVER_ROUTE_CALLBACK callback)
{
<#if LOADNG_ENABLE == true>
    if (ROUTING_WRP_IsDisabled() == false)
    {
        LOADNG_DiscoverRoute(dstAddr, maxHops, repair, pUserData, callback);
    }
    else
    {
        callback(G3_ROUTE_ERROR, dstAddr, 0xFFFF, NULL);
    }
<#else>
    callback(G3_ROUTE_ERROR, dstAddr, 0xFFFF, NULL);
</#if>
}

bool ROUTING_WRP_IsInDestinationAddressSet(uint16_t addr)
{
<#if LOADNG_ENABLE == true>
    return LOADNG_IsInDestinationAddressSet(addr);
<#else>
    return false;
</#if>
}

void ROUTING_WRP_ProcessMessage(uint16_t macSrcAddr, uint8_t mediaType,
    ADP_MODULATION_PLC modulation, uint8_t activeTones, uint8_t subCarriers,
    uint8_t lqi, uint16_t messageLength, uint8_t *pMessageBuffer)
{
<#if LOADNG_ENABLE == true>
    LOADNG_ProcessMessage(macSrcAddr, mediaType, modulation, activeTones,
        subCarriers, lqi, messageLength, pMessageBuffer);
</#if>
}

ROUTING_TABLE_ENTRY *ROUTING_WRP_AddRoute(uint16_t dstAddr,
    uint16_t nextHopAddr, uint8_t mediaType, bool *pTableFull)
{
<#if LOADNG_ENABLE == true>
    return LOADNG_AddRoute(dstAddr, nextHopAddr, mediaType, pTableFull);
<#else>
    *pTableFull = false;
    return NULL;
</#if>
}

void ROUTING_WRP_RefreshRoute(uint16_t dstAddr)
{
<#if LOADNG_ENABLE == true>
    LOADNG_RefreshRoute(dstAddr);
</#if>
}

void ROUTING_WRP_AddCircularRoute(uint16_t lastCircularRouteAddress)
{
<#if LOADNG_ENABLE == true>
    LOADNG_AddCircularRoute(lastCircularRouteAddress);
</#if>
}

void ROUTING_WRP_DeleteRoute(uint16_t dstAddr)
{
<#if LOADNG_ENABLE == true>
    LOADNG_DeleteRoute(dstAddr);
</#if>
}

bool ROUTING_WRP_RouteExists(uint16_t destinationAddress)
{
<#if LOADNG_ENABLE == true>
    return LOADNG_RouteExists(destinationAddress);
<#else>
    return false;
</#if>
}

uint16_t ROUTING_WRP_GetRouteAndMediaType(uint16_t destinationAddress,
    uint8_t *pMediaType)
{
<#if LOADNG_ENABLE == true>
    return LOADNG_GetRouteAndMediaType(destinationAddress, pMediaType);
<#else>
    ADP_MAC_GET_CFM_PARAMS getConfirm;
    uint16_t u16AdpShortAddress;
    ADP_MacGetRequestSync(MAC_WRP_PIB_SHORT_ADDRESS, 0, &getConfirm);
    memcpy(&u16AdpShortAddress, &getConfirm.attributeValue, 2);
    *pMediaType = 0;
    return u16AdpShortAddress;
</#if>
}

ROUTING_TABLE_ENTRY *ROUTING_WRP_GetRouteEntry(uint16_t destinationAddress)
{
    ROUTING_TABLE_ENTRY *pRet = NULL;
<#if LOADNG_ENABLE == true>
    if (ROUTING_WRP_IsDisabled() == false)
    {
        pRet = LOADNG_GetRouteEntry(destinationAddress);
    }
</#if>
    return pRet;
}

void ROUTING_WRP_AddBlacklistOnMedium(uint16_t addr, uint8_t mediaType)
{
<#if LOADNG_ENABLE == true>
    LOADNG_AddBlacklistOnMedium(addr, mediaType);
</#if>
}

void ROUTING_WRP_RemoveBlacklistOnMedium(uint16_t addr, uint8_t mediaType)
{
<#if LOADNG_ENABLE == true>
    LOADNG_RemoveBlacklistOnMedium(addr, mediaType);
</#if>
}

bool Routing_IsRouterTo(uint16_t addr)
{
<#if LOADNG_ENABLE == true>
    return LOADNG_IsRouterTo(addr);
<#else>
    return false;
</#if>
}

void ROUTING_WRP_Tasks(void)
{
<#if LOADNG_ENABLE == true>
    LOADNG_Tasks();
</#if>
}
