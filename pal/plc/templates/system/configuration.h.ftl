/* PAL PLC Configuration Options */
#define PAL_PLC_PHY_INDEX                     0
<#if G3_PAL_PLC_PHY_SNIFFER_EN == true>
#define PAL_PLC_PHY_SNIFFER_USI_INSTANCE      SRV_USI_INDEX_${G3_PAL_PLC_USI_INSTANCE?string}
</#if>