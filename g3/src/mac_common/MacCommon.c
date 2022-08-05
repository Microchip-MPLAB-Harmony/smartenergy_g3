#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <MacCommon.h>
#ifdef __PLC_MAC__
#include <MacRtMib.h>
#endif

extern uint32_t oss_get_up_time_ms(void);

struct TMacMibCommon g_MacMibCommon;

static const struct TMacMibCommon g_MacMibDefaultsCommon = {
  0xFFFF, // m_u16PanId
  {0}, // m_ExtendedAddress
  0xFFFF, // m_u16ShortAddress
  false, // m_bPromiscuousMode
  {0}, // m_aKeyTable
  0xFFFF, // m_u16RcCoord: set RC_COORD to its maximum value of 0xFFFF
  255, // m_u8POSTableEntryTtl
  false, // m_bCoordinator
};

static bool alreadyInitialized = false;

void MacCommonInitialize(void)
{
  if (!alreadyInitialized) {
    g_MacMibCommon = g_MacMibDefaultsCommon;
  }
}

void MacCommonReset(void)
{
  alreadyInitialized = false;
  MacCommonInitialize();
}

static enum EMacStatus MacPibGetExtendedAddress(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = sizeof(g_MacMibCommon.m_ExtendedAddress);
  memcpy(pValue->m_au8Value, &g_MacMibCommon.m_ExtendedAddress, pValue->m_u8Length);
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetExtendedAddress(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  if (pValue->m_u8Length == sizeof(g_MacMibCommon.m_ExtendedAddress)) {
    memcpy(&g_MacMibCommon.m_ExtendedAddress, pValue->m_au8Value, pValue->m_u8Length);
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

static enum EMacStatus MacPibGetPanId(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = sizeof(g_MacMibCommon.m_nPanId);
  memcpy(pValue->m_au8Value, &g_MacMibCommon.m_nPanId, pValue->m_u8Length);
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetPanId(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  if (pValue->m_u8Length == sizeof(g_MacMibCommon.m_nPanId)) {
    memcpy(&g_MacMibCommon.m_nPanId, pValue->m_au8Value, pValue->m_u8Length);
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

static enum EMacStatus MacPibGetPromiscuousMode(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = 1;
  pValue->m_au8Value[0] = (g_MacMibCommon.m_bPromiscuousMode) ? 1 : 0;
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetPromiscuousMode(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  uint8_t u8Value;
  memcpy(&u8Value, pValue->m_au8Value, sizeof(u8Value));
  if ((pValue->m_u8Length == sizeof(u8Value)) && (u8Value <= 1)) {
    g_MacMibCommon.m_bPromiscuousMode = u8Value != 0;
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

static enum EMacStatus MacPibGetShortAddress(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = sizeof(g_MacMibCommon.m_nShortAddress);
  memcpy(pValue->m_au8Value, &g_MacMibCommon.m_nShortAddress, pValue->m_u8Length);
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetShortAddress(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  if (pValue->m_u8Length == sizeof(g_MacMibCommon.m_nShortAddress)) {
    memcpy(&g_MacMibCommon.m_nShortAddress, pValue->m_au8Value, pValue->m_u8Length);
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

static enum EMacStatus MacPibGetRcCoord(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = sizeof(g_MacMibCommon.m_u16RcCoord);
  memcpy(pValue->m_au8Value, &g_MacMibCommon.m_u16RcCoord, pValue->m_u8Length);
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetRcCoord(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  if (pValue->m_u8Length == sizeof(g_MacMibCommon.m_u16RcCoord)) {
    memcpy(&g_MacMibCommon.m_u16RcCoord, pValue->m_au8Value, pValue->m_u8Length);
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

static enum EMacStatus MacPibSetKeyTable(uint16_t u16Index, const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus;
  if (u16Index < MAC_KEY_TABLE_ENTRIES) {
    if (pValue->m_u8Length == MAC_SECURITY_KEY_LENGTH) {
      if (!g_MacMibCommon.m_aKeyTable[u16Index].m_bValid ||
        (memcmp(&g_MacMibCommon.m_aKeyTable[u16Index].m_au8Key, pValue->m_au8Value, MAC_SECURITY_KEY_LENGTH) != 0)) {
        // Set value if invalid entry or different key
        memcpy(&g_MacMibCommon.m_aKeyTable[u16Index].m_au8Key, pValue->m_au8Value, MAC_SECURITY_KEY_LENGTH);
        g_MacMibCommon.m_aKeyTable[u16Index].m_bValid = true;
      }
      eStatus = MAC_STATUS_SUCCESS;
    }
    else if (pValue->m_u8Length == 0) {
      g_MacMibCommon.m_aKeyTable[u16Index].m_bValid = false;
      eStatus = MAC_STATUS_SUCCESS;
    }
    else {
      eStatus = MAC_STATUS_INVALID_PARAMETER;
    }
  }
  else {
    eStatus = MAC_STATUS_INVALID_INDEX;
  }
  return eStatus;
}

static enum EMacStatus MacPibGetPOSTableEntryTtl(struct TMacPibValue *pValue)
{
  pValue->m_u8Length = sizeof(g_MacMibCommon.m_u8POSTableEntryTtl);
  memcpy(pValue->m_au8Value, &g_MacMibCommon.m_u8POSTableEntryTtl, pValue->m_u8Length);
  return MAC_STATUS_SUCCESS;
}

static enum EMacStatus MacPibSetPOSTableEntryTtl(const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus = MAC_STATUS_SUCCESS;
  if (pValue->m_u8Length == sizeof(g_MacMibCommon.m_u8POSTableEntryTtl)) {
    memcpy(&g_MacMibCommon.m_u8POSTableEntryTtl, pValue->m_au8Value, pValue->m_u8Length);
  }
  else {
    eStatus = MAC_STATUS_INVALID_PARAMETER;
  }
  return eStatus;
}

enum EMacStatus MacCommonGetRequestSync(enum EMacCommonPibAttribute eAttribute, uint16_t u16Index, struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus;
  bool bArray = (eAttribute == MAC_COMMON_PIB_KEY_TABLE);
  if (!bArray && (u16Index != 0)) {
    eStatus = MAC_STATUS_INVALID_INDEX;
  }
  else {
    switch (eAttribute) {
      case MAC_COMMON_PIB_PAN_ID:
        eStatus = MacPibGetPanId(pValue);
        break;
      case MAC_COMMON_PIB_PROMISCUOUS_MODE:
        eStatus = MacPibGetPromiscuousMode(pValue);
        break;
      case MAC_COMMON_PIB_SHORT_ADDRESS:
        eStatus = MacPibGetShortAddress(pValue);
        break;
      case MAC_COMMON_PIB_RC_COORD:
        eStatus = MacPibGetRcCoord(pValue);
        break;
      case MAC_COMMON_PIB_EXTENDED_ADDRESS:
        eStatus = MacPibGetExtendedAddress(pValue);
        break;
      case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
        eStatus = MacPibGetPOSTableEntryTtl(pValue);
        break;
      case MAC_COMMON_PIB_KEY_TABLE:
        eStatus = MAC_STATUS_UNAVAILABLE_KEY;
        break;

      default:
        eStatus = MAC_STATUS_UNSUPPORTED_ATTRIBUTE;
        break;
    }
  }

  if (eStatus != MAC_STATUS_SUCCESS) {
    pValue->m_u8Length = 0;
  }
  return eStatus;
}

enum EMacStatus MacCommonSetRequestSync(enum EMacCommonPibAttribute eAttribute, uint16_t u16Index, const struct TMacPibValue *pValue)
{
  enum EMacStatus eStatus;
  bool bArray = (eAttribute == MAC_COMMON_PIB_KEY_TABLE);
  if (!bArray && (u16Index != 0)) {
    eStatus = MAC_STATUS_INVALID_INDEX;
  }
  else {
    switch (eAttribute) {
      case MAC_COMMON_PIB_PAN_ID:
        eStatus = MacPibSetPanId(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_PROMISCUOUS_MODE:
        eStatus = MacPibSetPromiscuousMode(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_SHORT_ADDRESS:
        eStatus = MacPibSetShortAddress(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_RC_COORD:
        eStatus = MacPibSetRcCoord(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_EXTENDED_ADDRESS:
        eStatus = MacPibSetExtendedAddress(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
        eStatus = MacPibSetPOSTableEntryTtl(pValue);
#ifdef __PLC_MAC__
        if (eStatus == MAC_STATUS_SUCCESS) {
          eStatus = (enum EMacStatus) MacRtSetRequestSync((enum EMacRtPibAttribute)eAttribute, u16Index, (const struct TMacRtPibValue *)pValue);
        }
#endif
        break;
      case MAC_COMMON_PIB_KEY_TABLE:
        eStatus = MacPibSetKeyTable(u16Index, pValue);
        break;

      default:
        eStatus = MAC_STATUS_UNSUPPORTED_ATTRIBUTE;
        break;
    }
  }
  return eStatus;
}

uint32_t MacCommonGetMsCounter(void)
{
  return oss_get_up_time_ms();
}

