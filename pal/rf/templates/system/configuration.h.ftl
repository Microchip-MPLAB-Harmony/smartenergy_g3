/* PAL RF Configuration Options */
#define PAL_RF_PHY_INDEX                     0
<#if G3_PAL_RF_PHY_SNIFFER_EN == true>
#define PAL_RF_PHY_SNIFFER_USI_INSTANCE      SRV_USI_INDEX_${G3_PAL_RF_USI_INSTANCE?string}
</#if>