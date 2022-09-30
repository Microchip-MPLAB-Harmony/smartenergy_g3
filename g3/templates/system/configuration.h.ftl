
/* MAC COMMON Identification */
#define MAC_COMMON_INDEX_0            0
#define MAC_COMMON_INSTANCES_NUMBER   1

<#if MAC_PLC_PRESENT == true>
/* MAC PLC Identification */
#define MAC_PLC_INDEX_0               0
#define MAC_PLC_INSTANCES_NUMBER      1

</#if>
<#if MAC_RF_PRESENT == true>
/* MAC RF Identification */
#define MAC_RF_INDEX_0                0
#define MAC_RF_INSTANCES_NUMBER       1

</#if>
/* MAC Wrapper Identification */
#define MAC_WRP_INDEX_0               0
#define MAC_WRP_INSTANCES_NUMBER      1