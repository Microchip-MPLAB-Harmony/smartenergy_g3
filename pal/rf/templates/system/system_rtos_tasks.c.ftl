
<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>static void lPAL_RF_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 PAL RF */
    <#lt>        PAL_RF_Tasks();
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "ThreadX">
    <#lt>TX_THREAD      lPAL_RF_Task_TCB;
    <#lt>uint8_t*       lPAL_RF_Task_Stk_Ptr;

    <#lt>static void lPAL_RF_Tasks( ULONG thread_input )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 PAL RF */
    <#lt>        PAL_RF_Tasks();
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "MicriumOSIII">
    <#lt>OS_TCB  lPAL_RF_Tasks_TCB;
    <#lt>CPU_STK lPAL_RF_TasksStk[PAL_RF_RTOS_STACK_SIZE];

    <#lt>static void lPAL_RF_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    OS_ERR os_err;
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 PAL RF */
    <#lt>        PAL_RF_Tasks();
    <#lt>    }
    <#lt>}
<#elseif (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "MbedOS">
    <#lt>static void lPAL_RF_Tasks( void *pvParameters )
    <#lt>{
    <#lt>    while(true)
    <#lt>    {
    <#lt>        /* Maintain G3 PAL RF */
    <#lt>        PAL_RF_Tasks();
    <#lt>    }
    <#lt>}
</#if>
