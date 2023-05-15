
/* G3 stack task rate in milliseconds */
#define G3_STACK_TASK_RATE_MS            ${G3_TASK_RATE_MS}

<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/* G3 stack RTOS configuration */
    <#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
        <#lt>#define G3_STACK_RTOS_STACK_SIZE         ${G3_RTOS_STACK_SIZE / 4}
    <#else>
        <#lt>#define G3_STACK_RTOS_STACK_SIZE         ${G3_RTOS_STACK_SIZE}
    </#if>
    <#lt>#define G3_STACK_RTOS_TASK_PRIORITY      ${G3_RTOS_TASK_PRIORITY}
    <#if HarmonyCore.SELECT_RTOS == "MicriumOSIII">
        <#lt>#define G3_STACK_RTOS_TASK_MSG_QTY       ${G3_RTOS_TASK_MSG_QTY}u
        <#lt>#define G3_STACK_RTOS_TASK_TIME_QUANTA   ${G3_RTOS_TASK_TIME_QUANTA}u
    </#if>

</#if>
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
<#if ADP_PRESENT == true>
/* Adaptation Layer Identification */
#define G3_ADP_INDEX_0                   0
#define G3_ADP_INSTANCES_NUMBER          1

/* Number of buffers for Adaptation Layer */
#define G3_ADP_NUM_BUFFERS_1280          ${ADP_COUNT_BUFFERS_1280}
#define G3_ADP_NUM_BUFFERS_400           ${ADP_COUNT_BUFFERS_400}
#define G3_ADP_NUM_BUFFERS_100           ${ADP_COUNT_BUFFERS_100}
#define G3_ADP_PROCESS_QUEUE_SIZE        (G3_ADP_NUM_BUFFERS_1280 + G3_ADP_NUM_BUFFERS_400 + G3_ADP_NUM_BUFFERS_100)
#define G3_ADP_FRAG_TRANSFER_TABLE_SIZE  ${ADP_FRAGMENTED_TRANSFER_TABLE_SIZE}

</#if>
<#if LOADNG_ENABLE == true>
#define G3_ADP_ROUTING_TABLE_SIZE        ${ADP_ROUTING_TABLE_SIZE}
#define G3_ADP_BLACKLIST_TABLE_SIZE      ${ADP_BLACKLIST_TABLE_SIZE}
#define G3_ADP_ROUTING_SET_SIZE          ${ADP_ROUTING_SET_SIZE}
#define G3_ADP_DESTINATION_ADDR_SET_SIZE ${ADP_DESTINATION_ADDRESS_SET_SIZE}

/* Table sizes for Routing (LOADNG) */
#define LOADNG_PENDING_RREQ_TABLE_SIZE   ${LOADNG_PENDING_RREQ_TABLE_SIZE}
#define LOADNG_RREP_GEN_TABLE_SIZE       ${LOADNG_RREP_GENERATION_TABLE_SIZE}
#define LOADNG_RREQ_FORWARD_TABLE_SIZE   ${LOADNG_RREQ_FORWARDING_TABLE_SIZE}
#define LOADNG_DISCOVER_ROUTE_TABLE_SIZE ${LOADNG_DISCOVER_ROUTE_TABLE_SIZE}

</#if>
<#if ADP_SERIALIZATION_EN == true>
/* ADP Serialization Identification */
#define G3_ADP_SERIAL_INDEX_0            0
#define G3_ADP_SERIAL_INSTANCES_NUMBER   1
#define G3_ADP_SERIAL_USI_INDEX          ${ADP_SERIALIZATION_USI_INSTANCE}
</#if>