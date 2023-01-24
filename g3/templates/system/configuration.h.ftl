
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
<#if MAC_SERIALIZATION_EN == true>
#define G3_MAC_WRP_SERIAL_USI_INDEX      ${MAC_SERIALIZATION_USI_INSTANCE}
</#if>
<#if g3_adapt_config??>

/* Adaptation Layer Identification */
#define G3_ADP_INDEX_0                   0
#define G3_ADP_INSTANCES_NUMBER          1

/* Number of buffers for Adaptation Layer */
#define G3_ADP_NUM_BUFFERS_1280          ${g3_adapt_config.ADP_COUNT_BUFFERS_1280}
#define G3_ADP_NUM_BUFFERS_400           ${g3_adapt_config.ADP_COUNT_BUFFERS_400}
#define G3_ADP_NUM_BUFFERS_100           ${g3_adapt_config.ADP_COUNT_BUFFERS_100}
#define G3_ADP_PROCESS_QUEUE_SIZE        (G3_ADP_NUM_BUFFERS_1280 + G3_ADP_NUM_BUFFERS_400 + G3_ADP_NUM_BUFFERS_100)
#define G3_ADP_FRAG_TRANSFER_TABLE_SIZE  ${g3_adapt_config.ADP_FRAGMENTED_TRANSFER_TABLE_SIZE}
<#if>