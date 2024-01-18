
    /* Initialize G3 MAC Wrapper Instance */
    sysObj.g3MacWrapper = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0);
<#if ADP_PRESENT == true>

    /* Initialize G3 ADP Instance */
    sysObj.g3Adp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&g3AdpInitData);
</#if>
<#if ADP_SERIALIZATION_EN == true>

    /* Initialize G3 ADP Serialization Instance */
    sysObj.g3AdpSerial = ADP_SERIAL_Initialize(G3_ADP_SERIAL_INDEX_0);
</#if>
