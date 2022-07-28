/**********************************************************************************************************************/
/** \addtogroup MacSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains data types and functions of the MAC API.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_API_H_
#define MAC_API_H_

#include "MacMib.h"

/**********************************************************************************************************************/
/** Description of struct TMlmeGetRequest
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeGetRequest {
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeGetConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeGetConfirm {
  enum EMacStatus m_eStatus;
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeGetRequest
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeSetRequest {
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeSetConfirm
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeSetConfirm {
  enum EMacStatus m_eStatus;
  enum EMacPibAttribute m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
};

struct TMacNotifications;

#define BAND_CENELEC_A 0
#define BAND_CENELEC_B 1
#define BAND_FCC 2
#define BAND_ARIB 3

/**********************************************************************************************************************/
/** Use this function to initialize the MAC layer. The MAC layer should be initialized before doing any other operation.
 * The APIs cannot be mixed, if the stack is initialized in ADP mode then only the ADP functions can be used and if the
 * stack is initialized in MAC mode then only MAC functions can be used.
 * @param pNotifications Structure with callbacks used by the MAC layer to notify the upper layer about specific events
 * @param band Working band (should be inline with the hardware)
 * @param pTableSizes Structure containing sizes of Mac Tables (defined outside Mac layer and accessed through Pointers)
 **********************************************************************************************************************/
void MacInitialize(struct TMacNotifications *pNotifications, uint8_t u8Band, struct TMacTables *pTables);

/**********************************************************************************************************************/
/** This function should be called at least every millisecond in order to allow the G3 stack to run and execute its
 * internal tasks.
 **********************************************************************************************************************/
void MacEventHandler(void);

/**********************************************************************************************************************/
/** The MacMcpsDataRequest primitive requests the transfer of upper layer PDU to another device or multiple devices.
 * Parameters from struct TMcpsDataRequest
 ***********************************************************************************************************************
 * @param pParameters Request parameters
 **********************************************************************************************************************/
void MacMcpsDataRequest(struct TMcpsDataRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetRequest primitive allows the upper layer to get the value of an attribute from the MAC information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeGetRequest(struct TMlmeGetRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetConfirm primitive allows the upper layer to be notified of the completion of a MacMlmeGetRequest.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeGetConfirm)(struct TMlmeGetConfirm *pParameters);


/**********************************************************************************************************************/
/** The MacMlmeGetRequestSync primitive allows the upper layer to get the value of an attribute from the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to results
 **********************************************************************************************************************/
enum EMacStatus MacMlmeGetRequestSync(enum EMacPibAttribute eAttribute, uint16_t u16Index, struct TMacPibValue *pValue);


/**********************************************************************************************************************/
/** The MacMlmeSetRequest primitive allows the upper layer to set the value of an attribute in the MAC information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeSetRequest(struct TMlmeSetRequest *pParameters);


/**********************************************************************************************************************/
/** The MacMlmeSetConfirm primitive allows the upper layer to be notified of the completion of a MacMlmeSetRequest.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeSetConfirm)(struct TMlmeSetConfirm *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeSetRequestSync primitive allows the upper layer to set the value of an attribute in the MAC information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to IB new values
 **********************************************************************************************************************/
enum EMacStatus MacMlmeSetRequestSync(enum EMacPibAttribute eAttribute, uint16_t u16Index, const struct TMacPibValue *pValue);

/**********************************************************************************************************************/
/** The MacMlmeResetRequest primitive performs a reset of the mac sublayer and allows the resetting of the MIB
 * attributes.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeResetRequest(struct TMlmeResetRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeScanRequest primitive allows the upper layer to scan for networks operating in its POS.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request.
 **********************************************************************************************************************/
void MacMlmeScanRequest(struct TMlmeScanRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeStartRequest primitive allows the upper layer to request the starting of a new network.
 ***********************************************************************************************************************
 * @param pParameters The parameters of the request
 **********************************************************************************************************************/
void MacMlmeStartRequest(struct TMlmeStartRequest *pParameters);

struct TMacNotifications {
  MacMcpsDataConfirm m_McpsDataConfirm;
  MacMcpsDataIndication m_McpsDataIndication;
  MacMlmeGetConfirm m_MlmeGetConfirm;
  MacMlmeSetConfirm m_MlmeSetConfirm;
  MacMlmeResetConfirm m_MlmeResetConfirm;
  MacMlmeBeaconNotify m_MlmeBeaconNotifyIndication;
  MacMlmeScanConfirm m_MlmeScanConfirm;
  MacMlmeStartConfirm m_MlmeStartConfirm;
  MacMlmeCommStatusIndication m_MlmeCommStatusIndication;
  MacMcpsMacSnifferIndication m_McpsMacSnifferIndication;
};

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
