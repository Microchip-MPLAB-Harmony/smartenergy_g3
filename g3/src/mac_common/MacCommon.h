/**********************************************************************************************************************/
/** \addtogroup MacCommon
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains the Common MAC objecs.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_COMMON_H_
#define MAC_COMMON_H_

#include "MacCommonDefs.h"

#define MAC_PIB_MAX_VALUE_LENGTH (144)

struct TMacPibValue {
  uint8_t m_u8Length;
  uint8_t m_au8Value[MAC_PIB_MAX_VALUE_LENGTH];
};

enum EMacCommonPibAttribute {
  MAC_COMMON_PIB_PAN_ID = 0x00000050,
  MAC_COMMON_PIB_PROMISCUOUS_MODE = 0x00000051,
  MAC_COMMON_PIB_SHORT_ADDRESS = 0x00000053,
  MAC_COMMON_PIB_KEY_TABLE = 0x00000071,
  MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL = 0x0000010E,
  MAC_COMMON_PIB_RC_COORD = 0x0000010F,
  MAC_COMMON_PIB_EXTENDED_ADDRESS = 0x08000001
};

/**********************************************************************************************************************/
/** The MacCommonInitialize primitive initializes the common mac sublayer and MIB attributes.
 ***********************************************************************************************************************
 * @param None
 **********************************************************************************************************************/
void MacCommonInitialize(void);


/**********************************************************************************************************************/
/** The MacCommonReset primitive performs a reset of the common mac sublayer and allows the resetting of the MIB
 * attributes.
 ***********************************************************************************************************************
 * @param None
 **********************************************************************************************************************/
void MacCommonReset(void);


/**********************************************************************************************************************/
/** The MacCommonGetRequestSync primitive allows the upper layer to get the value of an attribute from the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to results
 * @return Result of Get Operation
 **********************************************************************************************************************/
enum EMacStatus MacCommonGetRequestSync(enum EMacCommonPibAttribute eAttribute, uint16_t u16Index, struct TMacPibValue *pValue);


/**********************************************************************************************************************/
/** The MacCommonSetRequestSync primitive allows the upper layer to set the value of an attribute in the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to IB new values
 * @return Result of Set Operation
 **********************************************************************************************************************/
enum EMacStatus MacCommonSetRequestSync(enum EMacCommonPibAttribute eAttribute, uint16_t u16Index, const struct TMacPibValue *pValue);


/**********************************************************************************************************************/
/** This primitive gets a millisecond counter from an external time handling module and serves as a time control
 * for both PLC and RF MAC layers.
 ***********************************************************************************************************************
 * @return Current ms counter value
 **********************************************************************************************************************/
uint32_t MacCommonGetMsCounter(void);

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
