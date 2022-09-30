#include "g3/src/mac_common/mac_common.h"
#include "g3/src/mac_common/mac_common_defs.h"
<#if MAC_PLC_PRESENT == true>
#include "g3/src/mac_plc/mac_plc.h"
#include "g3/src/mac_plc/mac_plc_defs.h"
#include "g3/src/mac_plc/mac_plc_mib.h"
</#if>
<#if MAC_RF_PRESENT == true>
#include "g3/src/mac_rf/mac_rf.h"
#include "g3/src/mac_rf/mac_rf_defs.h"
#include "g3/src/mac_rf/mac_rf_mib.h"
</#if>
#include "g3/src/mac_wrapper/mac_wrapper.h"
#include "g3/src/mac_wrapper/mac_wrapper_defs.h"