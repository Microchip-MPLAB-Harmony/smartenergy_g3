<#--
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
-->
<#if ADP_PRESENT == true>
// <editor-fold defaultstate="collapsed" desc="G3 ADP Initialization Data">
/* G3 ADP Buffers and Queues */
ADP_DATA_PARAMS_BUFFER_1280 g3Adp1280Buffers[G3_ADP_NUM_BUFFERS_1280];
ADP_DATA_PARAMS_BUFFER_400 g3Adp400Buffers[G3_ADP_NUM_BUFFERS_400];
ADP_DATA_PARAMS_BUFFER_100 g3Adp100Buffers[G3_ADP_NUM_BUFFERS_100];
ADP_PROCESS_QUEUE_ENTRY g3AdpProcessQueueEntries[G3_ADP_PROCESS_QUEUE_SIZE];
ADP_LOWPAN_FRAGMENTED_DATA g3AdpFragmentedTransferTable[G3_ADP_FRAG_TRANSFER_TABLE_SIZE];

/* G3 ADP Initialization Data */
ADP_INIT g3AdpInitData = {
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

    /* ADP task rate in milliseconds */
    .taskRateMs = G3_STACK_TASK_RATE_MS
};

// </editor-fold>
</#if>