
<#if ADP_PRESENT == true>
// <editor-fold defaultstate="collapsed" desc="G3 ADP Initialization Data">
/* G3 ADP Buffers and Queues */
static ADP_DATA_PARAMS_BUFFER_1280 g3Adp1280Buffers[G3_ADP_NUM_BUFFERS_1280];
static ADP_DATA_PARAMS_BUFFER_400 g3Adp400Buffers[G3_ADP_NUM_BUFFERS_400];
static ADP_DATA_PARAMS_BUFFER_100 g3Adp100Buffers[G3_ADP_NUM_BUFFERS_100];
static ADP_PROCESS_QUEUE_ENTRY g3AdpProcessQueueEntries[G3_ADP_PROCESS_QUEUE_SIZE];
static ADP_LOWPAN_FRAGMENTED_DATA g3AdpFragmentedTransferTable[G3_ADP_FRAG_TRANSFER_TABLE_SIZE];

/* G3 ADP Initialization Data */
static ADP_INIT g3AdpInitData = {
    /* Pointer to start of 1280-byte buffers */
    .pBuffers1280 = &g3Adp1280Buffers,

    /* Pointer to start of 400-byte buffers */
    .pBuffers400 = &g3Adp400Buffers,

    /* Pointer to start of 100-byte buffers */
    .pBuffers100 = &g3Adp100Buffers,

    /* Pointer to start of process queue entries */
    .pProcessQueueEntries = &g3AdpProcessQueueEntries,

    /* Pointer to start of fragmented transfer entries */
    .pFragmentedTransferEntries = &g3AdpFragmentedTransferTable,

    /* ADP fragmentation size */
    .fragmentSize = G3_ADP_FRAGMENT_SIZE,

    /* Number of 1280-byte buffers */
    .numBuffers1280 = G3_ADP_NUM_BUFFERS_1280,

    /* Number of 400-byte buffers */
    .numBuffers400 = G3_ADP_NUM_BUFFERS_400,

    /* Number of 100-byte buffers */
    .numBuffers100 = G3_ADP_NUM_BUFFERS_100,

    /* Number of process queue entries */
    .numProcessQueueEntries = G3_ADP_PROCESS_QUEUE_SIZE,

    /* Number of fragmented transfer entries */
    .numFragmentedTransferEntries = G3_ADP_FRAG_TRANSFER_TABLE_SIZE,

<#if (HarmonyCore.SELECT_RTOS)?? && (HarmonyCore.SELECT_RTOS != "BareMetal")>
    /* RTOS enabled: ADP task executed always */
    .taskRateMs = 0
<#else>
    /* ADP task rate in milliseconds */
    .taskRateMs = G3_STACK_TASK_RATE_MS
</#if>

};

// </editor-fold>
</#if>