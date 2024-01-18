
<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>static void lG3_STACK_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 MAC */
    <#lt>        MAC_WRP_Tasks(sysObj.g3MacWrapper);
<#if ADP_PRESENT == true>

    <#lt>        /* Maintain G3 ADP */
    <#lt>        ADP_Tasks(sysObj.g3Adp);
</#if>
<#if ADP_SERIALIZATION_EN == true>

    <#lt>        /* Maintain G3 ADP Serialization */
    <#lt>        ADP_SERIAL_Tasks(sysObj.g3AdpSerial);
</#if>

    <#lt>        vTaskDelay(G3_STACK_RTOS_TASK_DELAY_MS / portTICK_PERIOD_MS);
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "ThreadX">
    <#lt>TX_THREAD      lG3_STACK_Task_TCB;
    <#lt>uint8_t*       lG3_STACK_Task_Stk_Ptr;

    <#lt>static void lG3_STACK_Tasks( ULONG thread_input )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 MAC */
    <#lt>        MAC_WRP_Tasks(sysObj.g3MacWrapper);
<#if ADP_PRESENT == true>

    <#lt>        /* Maintain G3 ADP */
    <#lt>        ADP_Tasks(sysObj.g3Adp);
</#if>
<#if ADP_SERIALIZATION_EN == true>

    <#lt>        /* Maintain G3 ADP Serialization */
    <#lt>        ADP_SERIAL_Tasks(sysObj.g3AdpSerial);
</#if>

    <#lt>        tx_thread_sleep((ULONG)(G3_STACK_RTOS_TASK_DELAY_MS / (TX_TICK_PERIOD_MS)));
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "MicriumOSIII">
    <#lt>OS_TCB  lG3_STACK_Tasks_TCB;
    <#lt>CPU_STK lG3_STACK_TasksStk[G3_STACK_RTOS_STACK_SIZE];

    <#lt>static void lG3_STACK_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    OS_ERR os_err;
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 MAC */
    <#lt>        MAC_WRP_Tasks(sysObj.g3MacWrapper);
<#if ADP_PRESENT == true>

    <#lt>        /* Maintain G3 ADP */
    <#lt>        ADP_Tasks(sysObj.g3Adp);
</#if>
<#if ADP_SERIALIZATION_EN == true>

    <#lt>        /* Maintain G3 ADP Serialization */
    <#lt>        ADP_SERIAL_Tasks(sysObj.g3AdpSerial);
</#if>

    <#lt>        OSTimeDly(G3_STACK_RTOS_TASK_DELAY_MS, OS_OPT_TIME_DLY, &os_err);
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "MbedOS">
    <#lt>static void lG3_STACK_Tasks( void *pvParameters )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 MAC */
    <#lt>        MAC_WRP_Tasks(sysObj.g3MacWrapper);
<#if ADP_PRESENT == true>

    <#lt>        /* Maintain G3 ADP */
    <#lt>        ADP_Tasks(sysObj.g3Adp);
</#if>
<#if ADP_SERIALIZATION_EN == true>

    <#lt>        /* Maintain G3 ADP Serialization */
    <#lt>        ADP_SERIAL_Tasks(sysObj.g3AdpSerial);
</#if>

    <#lt>        thread_sleep_for((uint32_t)(G3_STACK_RTOS_TASK_DELAY_MS / MBED_OS_TICK_PERIOD_MS));
    <#lt>    }
    <#lt>}
</#if>
