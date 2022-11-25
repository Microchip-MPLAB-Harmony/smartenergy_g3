
/* MAC COMMON Identification */
#define G3_MAC_COMMON_INDEX_0            0
#define G3_MAC_COMMON_INSTANCES_NUMBER   1

<#if MAC_PLC_PRESENT == true>
/* MAC PLC Identification */
#define G3_MAC_PLC_INDEX_0               0
#define G3_MAC_PLC_INSTANCES_NUMBER      1

</#if>
<#if MAC_RF_PRESENT == true>
/* MAC RF Identification */
#define G3_MAC_RF_INDEX_0                0
#define G3_MAC_RF_INSTANCES_NUMBER       1

</#if>
/* MAC Wrapper Identification */
#define G3_MAC_WRP_INDEX_0               0
#define G3_MAC_WRP_INSTANCES_NUMBER      1