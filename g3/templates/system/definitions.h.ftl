#include "stack/g3/mac/mac_common/mac_common.h"
#include "stack/g3/mac/mac_common/mac_common_defs.h"
<#if MAC_PLC_PRESENT == true>
#include "stack/g3/mac/mac_plc/mac_plc.h"
#include "stack/g3/mac/mac_plc/mac_plc_defs.h"
#include "stack/g3/mac/mac_plc/mac_plc_mib.h"
</#if>
<#if MAC_RF_PRESENT == true>
#include "stack/g3/mac/mac_rf/mac_rf.h"
#include "stack/g3/mac/mac_rf/mac_rf_defs.h"
#include "stack/g3/mac/mac_rf/mac_rf_mib.h"
</#if>
#include "stack/g3/mac/mac_wrapper/mac_wrapper.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper_defs.h"
<#if ADP_PRESENT == true>
#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/adp_api_types.h"
#include "stack/g3/adaptation/adp_shared_types.h"
<#if ADP_SERIALIZATION_EN == true>
#include "stack/g3/adaptation/adp_serial.h"
</#if>
#include "stack/g3/adaptation/lbp_defs.h"
<#if G3_DEVICE == true>
#include "stack/g3/adaptation/lbp_dev.h"
</#if>
<#if G3_COORDINATOR == true>
#include "stack/g3/adaptation/lbp_coord.h"
</#if>
</#if>