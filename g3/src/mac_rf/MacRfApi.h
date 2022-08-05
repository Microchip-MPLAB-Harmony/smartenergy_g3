/**********************************************************************************************************************/
/** \addtogroup MacRfSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains data types and functions of the RF MAC API.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef MAC_RF_API_H_
#define MAC_RF_API_H_

#include <MacRfMib.h>

/**********************************************************************************************************************/
/** Description of struct TMlmeGetRequestRF
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeGetRequestRF {
  enum EMacPibAttributeRF m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeGetConfirmRF
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeGetConfirmRF {
  enum EMacStatus m_eStatus;
  enum EMacPibAttributeRF m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeGetRequestRF
 ***********************************************************************************************************************
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 * @param m_PibAttributeValue The value of the attribute.
 **********************************************************************************************************************/
struct TMlmeSetRequestRF {
  enum EMacPibAttributeRF m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
  struct TMacPibValue m_PibAttributeValue;
};

/**********************************************************************************************************************/
/** Description of struct TMlmeSetConfirmRF
 ***********************************************************************************************************************
 * @param m_eStatus The status of the request.
 * @param m_ePibAttribute The attribute id.
 * @param m_u16PibAttributeIndex The index of the element in the table.
 **********************************************************************************************************************/
struct TMlmeSetConfirmRF {
  enum EMacStatus m_eStatus;
  enum EMacPibAttributeRF m_ePibAttribute;
  uint16_t m_u16PibAttributeIndex;
};

struct TMacNotificationsRF;

/**********************************************************************************************************************/
/** Use this function to initialize the RF MAC layer. The RF MAC layer should be initialized before doing any other operation.
 * The APIs cannot be mixed, if the stack is initialized in ADP mode then only the ADP functions can be used and if the
 * stack is initialized in MAC mode then only MAC functions can be used.
 * @param pNotifications Structure with callbacks used by the MAC layer to notify the upper layer about specific events
 * @param pTables Structure containing pointers to, and sizes of Mac RF Tables (defined outside RF Mac layer and accessed through Pointers)
 **********************************************************************************************************************/
void MacInitializeRF(struct TMacNotificationsRF *pNotifications, struct TMacTablesRF *pTables);

/**********************************************************************************************************************/
/** This function should be called at least every millisecond in order to allow the G3 stack to run and execute its
 * internal tasks.
 **********************************************************************************************************************/
void MacEventHandlerRF(void);

/**********************************************************************************************************************/
/** The MacMcpsDataRequestRF primitive requests the transfer of upper layer PDU to another device or multiple devices.
 * Parameters from struct TMcpsDataRequest
 ***********************************************************************************************************************
 * @param pParameters Request parameters
 **********************************************************************************************************************/
void MacMcpsDataRequestRF(struct TMcpsDataRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetRequestRF primitive allows the upper layer to get the value of an attribute from the MAC RF information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeGetRequestRF(struct TMlmeGetRequestRF *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetConfirmRF primitive allows the upper layer to be notified of the completion of a MacMlmeGetRequestRF.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeGetConfirmRF)(struct TMlmeGetConfirmRF *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeGetRequestSyncRF primitive allows the upper layer to get the value of an attribute from the MAC RF information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to results
 **********************************************************************************************************************/
enum EMacStatus MacMlmeGetRequestSyncRF(enum EMacPibAttributeRF eAttribute, uint16_t u16Index, struct TMacPibValue *pValue);

/**********************************************************************************************************************/
/** The MacMlmeSetRequestRF primitive allows the upper layer to set the value of an attribute in the MAC RF information base.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeSetRequestRF(struct TMlmeSetRequestRF *pParameters);


/**********************************************************************************************************************/
/** The MacMlmeSetConfirmRF primitive allows the upper layer to be notified of the completion of a MacMlmeSetRequestRF.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the confirm.
 **********************************************************************************************************************/
typedef void (*MacMlmeSetConfirmRF)(struct TMlmeSetConfirmRF *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeSetRequestSyncRF primitive allows the upper layer to set the value of an attribute in the MAC RF information
 * base in synchronous way.
 ***********************************************************************************************************************
 * @param eAttribute IB identifier
 * @param u16Index IB index
 * @param pValue pointer to IB new values
 **********************************************************************************************************************/
enum EMacStatus MacMlmeSetRequestSyncRF(enum EMacPibAttributeRF eAttribute, uint16_t u16Index, const struct TMacPibValue *pValue);

/**********************************************************************************************************************/
/** The MacMlmeResetRequestRF primitive performs a reset of the mac sublayer and allows the resetting of the RF MIB
 * attributes.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request
 **********************************************************************************************************************/
void MacMlmeResetRequestRF(struct TMlmeResetRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeScanRequestRF primitive allows the upper layer to scan for networks operating in its POS.
 ***********************************************************************************************************************
 * @param pParameters Parameters of the request.
 **********************************************************************************************************************/
void MacMlmeScanRequestRF(struct TMlmeScanRequest *pParameters);

/**********************************************************************************************************************/
/** The MacMlmeStartRequestRF primitive allows the upper layer to request the starting of a new network.
 ***********************************************************************************************************************
 * @param pParameters The parameters of the request
 **********************************************************************************************************************/
void MacMlmeStartRequestRF(struct TMlmeStartRequest *pParameters);

struct TMacNotificationsRF {
  MacMcpsDataConfirm m_McpsDataConfirm;
  MacMcpsDataIndication m_McpsDataIndication;
  MacMlmeGetConfirmRF m_MlmeGetConfirm;
  MacMlmeSetConfirmRF m_MlmeSetConfirm;
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
