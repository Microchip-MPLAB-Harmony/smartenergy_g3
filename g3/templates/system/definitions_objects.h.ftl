    SYS_MODULE_OBJ g3MacWrapper;
<#if (g3_adapt_config)??>
    SYS_MODULE_OBJ g3Adp;
<#if g3_adapt_config.ADP_SERIALIZATION_EN == true>
    SYS_MODULE_OBJ g3AdpSerial;
</#if>
</#if>