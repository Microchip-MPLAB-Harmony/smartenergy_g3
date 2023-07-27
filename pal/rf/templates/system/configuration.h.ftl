/* PAL RF Configuration Options */
#define PAL_RF_PHY_INDEX                      0U
<#if G3_PAL_RF_PHY_SNIFFER_EN == true>
#define PAL_RF_PHY_SNIFFER_USI_INSTANCE       SRV_USI_INDEX_${G3_PAL_RF_USI_INSTANCE?string}
</#if>

<#if __PROCESSOR?contains("WBZ45") || __PROCESSOR?contains("PIC32CX1012BZ")>
    <#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS != "BareMetal">
        <#lt>/* PAL RF RTOS Configuration */
        <#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "FreeRTOS">
            <#lt>#define PAL_RF_RTOS_STACK_SIZE                ${PAL_RF_RTOS_STACK_SIZE / 4}U
        <#else>
            <#lt>#define PAL_RF_RTOS_STACK_SIZE                ${PAL_RF_RTOS_STACK_SIZE}U
        </#if>
            <#lt>#define PAL_RF_RTOS_TASK_PRIORITY             ${PAL_RF_RTOS_TASK_PRIORITY}
            <#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "MicriumOSIII">
                <#lt>#define PAL_RF_RTOS_TASK_MSG_QTY              ${PAL_RF_RTOS_TASK_MSG_QTY}U
                <#lt>#define PAL_RF_RTOS_TASK_TIME_QUANTA          ${PAL_RF_RTOS_TASK_TIME_QUANTA}U
        </#if>

    </#if>
</#if>