/**********************************************************************************************************************/
/** Defines the API provided by ADP for the upper layer.
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/

#ifndef __ADP_API_H__
#define __ADP_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "adp_shared_types.h"
#include "adp_api_types.h"

/**********************************************************************************************************************/
/** Forward declaration
 **********************************************************************************************************************/
struct TAdpNotifications;

/**********************************************************************************************************************/
/** Use this function to initialize the ADP layer. The ADP layer should be initialized before doing any other operation.
 * The APIs cannot be mixed, if the stack is initialized in ADP mode then only the ADP functions can be used and if the
 * stack is initialized in MAC mode then only MAC functions can be used.
 * @param pNotifications Structure with callbacks used to notify ADP specific events (if NULL the layer is deinitialized)
 * @param band Working band (should be inline with the hardware)
 **********************************************************************************************************************/
void AdpInitialize(struct TAdpNotifications *pNotifications, enum TAdpBand band);

/**********************************************************************************************************************/
/** This function should be called at least every millisecond in order to allow the G3 stack to run and execute its
 * internal tasks.
 **********************************************************************************************************************/
bool AdpEventHandler(void);

/**********************************************************************************************************************/
/** The AdpDataRequest primitive requests the transfer of an application PDU to another device or multiple devices.
 ***********************************************************************************************************************
 * @param u16NsduLength The size of the NSDU, in bytes; Up to 1280
 * @param pNsdu The NSDU to send; should be a valid IPv6 packet
 * @param u8NsduHandle The handle of the NSDU to transmit. This parameter is used to identify in the AdpDataConfirm
 *                     primitive which request it is concerned with. It can be randomly chosen by the application layer.
 * @param bDiscoverRoute If TRUE, a route discovery procedure will be performed prior to sending the frame if a route
 *                       to the destination is not available in the routing table. If FALSE, no route discovery is performed.
 * @param u8QualityOfService The requested quality of service (QoS) of the frame to send. Allowed values are:
 *				0x00 = normal priority
 *				0x01 = high priority
 **********************************************************************************************************************/
void AdpDataRequest(uint16_t u16NsduLength, const uint8_t *pNsdu,
  uint8_t u8NsduHandle, bool bDiscoverRoute, uint8_t u8QualityOfService);

/**********************************************************************************************************************/
/** The AdpNoIPDataRequest primitive requests the transfer of an application PDU to another device 
 * or multiple devices providing a 6LowPAN 16-bit Destination Address.
 * PDU does not have to be an IPv6 packet, any protocol data can be sent using this primitive
 ***********************************************************************************************************************
 * @param u16ApduLength The size of the APDU, in bytes; Up to 1280
 * @param pApdu The APDU to send; can be any App protocol
 * @param u16DstAddr The Destination Short Address. Can be a unicast address, a 6LowPAN Group Address,
 *                   or the 6LowPAN BROADCAST ADDRESS (0x8001)
 * @param u8ApduHandle The handle of the APDU to transmit. This parameter is used to identify in the AdpDataConfirm
 *                     primitive which request it is concerned with. It can be randomly chosen by the application layer.
 * @param bDiscoverRoute If TRUE, a route discovery procedure will be performed prior to sending the frame if a route
 *                       to the destination is not available in the routing table. If FALSE, no route discovery is performed.
 * @param u8QualityOfService The requested quality of service (QoS) of the frame to send. Allowed values are:
 *				0x00 = normal priority
 *				0x01 = high priority
 **********************************************************************************************************************/
void AdpNoIPDataRequest(uint16_t u16ApduLength, const uint8_t *pApdu,
  uint16_t u16DstAddr, uint8_t u8ApduHandle, bool bDiscoverRoute, uint8_t u8QualityOfService);

/**********************************************************************************************************************/
/** The AdpDataConfirm primitive allows the upper layer to be notified of the completion of an AdpDataRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status code of a previous AdpDataRequest identified by its NsduHandle.
 * @param m_u8NsduHandle The handle of the NSDU confirmed by this primitive.
 **********************************************************************************************************************/
struct TAdpDataConfirm {
  uint8_t m_u8Status;
  uint8_t m_u8NsduHandle;
};
typedef void (*AdpDataConfirm)(struct TAdpDataConfirm *pDataConfirm);

/**********************************************************************************************************************/
/** The AdpDataIndication primitive is used to transfer received data from the adaptation sublayer to the upper layer.
 ***********************************************************************************************************************
 * @param m_u16NsduLength The size of the NSDU, in bytes; Up to 1280 bytes
 * @param m_pNsdu The received NSDU
 * @param m_u8LinkQualityIndicator The value of the link quality during reception of the frame.
 **********************************************************************************************************************/
struct TAdpDataIndication {
  uint16_t m_u16NsduLength;
  const uint8_t *m_pNsdu;
  uint8_t m_u8LinkQualityIndicator;
};
typedef void (*AdpDataIndication)(struct TAdpDataIndication *pDataIndication);

/**********************************************************************************************************************/
/** The AdpDiscoveryRequest primitive allows the upper layer to scan for networks operating in its POS.
 ***********************************************************************************************************************
 * @param u8Duration The number of seconds the scan shall last.
 **********************************************************************************************************************/
void AdpDiscoveryRequest(uint8_t u8Duration);

/**********************************************************************************************************************/
/** The AdpDiscoveryIndication primitive is generated by the ADP layer to notify the application about the discovery
 * of a new PAN coordinator or LBA
 ***********************************************************************************************************************
 * @param pPanDescriptor PAN descriptor contains information about the PAN
 **********************************************************************************************************************/
typedef void (*AdpDiscoveryIndication)(struct TAdpPanDescriptor *pPanDescriptor);

/**********************************************************************************************************************/
/** The AdpDiscoveryConfirm primitive allows the upper layer to be notified of the completion of an AdpDiscoveryRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the scan request.
 **********************************************************************************************************************/
typedef void (*AdpDiscoveryConfirm)(uint8_t m_u8Status);

/**********************************************************************************************************************/
/** The AdpNetworkStartRequest primitive allows the upper layer to request the starting of a new network. It shall
 * only be invoked by a device designated as the PAN coordinator during the factory process.
 ***********************************************************************************************************************
 * @param u16PanId The PANId of the network to create; determined at the application level
 **********************************************************************************************************************/
void AdpNetworkStartRequest(uint16_t u16PanId);

/**********************************************************************************************************************/
/** The AdpNetworkStartConfirm primitive allows the upper layer to be notified of the completion of an
 * AdpNetworkStartRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 **********************************************************************************************************************/
struct TAdpNetworkStartConfirm {
  uint8_t m_u8Status;
};
typedef void (*AdpNetworkStartConfirm)(
  struct TAdpNetworkStartConfirm *pNetworkStartConfirm);

/**********************************************************************************************************************/
/** The AdpNetworkJoinRequest primitive allows the upper layer to join an existing network.
 ***********************************************************************************************************************
 * @param u16PanId The 16-bit PAN identifier of the network to join.
 * @param u16LbaAddress The 16-bit short address of the device acting as a LoWPAN bootstrap agent (relay)
 * @param u8MediaType The Media Type to use for frame exchange with LBA. Only used in Hybrid Profile.
 **********************************************************************************************************************/
void AdpNetworkJoinRequest(uint16_t u16PanId, uint16_t u16LbaAddress, uint8_t u8MediaType);

/**********************************************************************************************************************/
/** The AdpNetworkJoinConfirm primitive allows the upper layer to be notified of the completion of an
 * AdpNetworkJoinRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 * @param m_u16NetworkAddress The 16-bit network address that was allocated to the device.
 * @param m_u16PanId The 16-bit address of the PAN of which the device is now a member.
 **********************************************************************************************************************/
struct TAdpNetworkJoinConfirm {
  uint8_t m_u8Status;
  uint16_t m_u16NetworkAddress;
  uint16_t m_u16PanId;
};
typedef void (*AdpNetworkJoinConfirm)(
  struct TAdpNetworkJoinConfirm *pNetworkJoinConfirm);

/**********************************************************************************************************************/
/** The AdpNetworkLeaveRequest primitive allows a non-coordinator device to remove itself from the network.
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
void AdpNetworkLeaveRequest(void);

/**********************************************************************************************************************/
/** The AdpNetworkLeaveConfirm primitive allows the upper layer to be notified of the completion of an
 * AdpNetworkLeaveRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 * @param m_ExtendedAddress The 64-bit network address of the device
 **********************************************************************************************************************/
struct TAdpNetworkLeaveConfirm {
  uint8_t m_u8Status;
};
typedef void (*AdpNetworkLeaveConfirm)(
  struct TAdpNetworkLeaveConfirm *pLeaveConfirm);

/**********************************************************************************************************************/
/** The AdpNetworkLeaveIndication primitive is generated by the ADP layer of a non-coordinator device to inform
 * the upper layer that it has been unregistered from the network by the coordinator.
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
typedef void (*AdpNetworkLeaveIndication)(void);

/**********************************************************************************************************************/
/** The AdpResetRequest primitive performs a reset of the adaptation sublayer and allows the resetting of the MIB
 * attributes.
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
void AdpResetRequest(void);

/**********************************************************************************************************************/
/** The AdpResetConfirm primitive allows the upper layer to be notified of the completion of an AdpResetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 **********************************************************************************************************************/
struct TAdpResetConfirm {
  uint8_t m_u8Status;
};
typedef void (*AdpResetConfirm)(struct TAdpResetConfirm *pResetConfirm);

/**********************************************************************************************************************/
/** The AdpGetRequest primitive allows the upper layer to get the value of an attribute from the ADP information base.
 ***********************************************************************************************************************
 * @param u32AttributeId The identifier of the ADP IB attribute to read.
 * @param u16AttributeIndex The index within the table of the specified IB attribute to read.
 **********************************************************************************************************************/
void AdpGetRequest(uint32_t u32AttributeId, uint16_t u16AttributeIndex);

/**********************************************************************************************************************/
/** The AdpGetConfirm primitive allows the upper layer to be notified of the completion of an AdpGetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 * @param m_u32AttributeId The identifier of the IB attribute read
 * @param m_u16AttributeIndex The index within the table of the specified IB attribute read.
 * @param m_u8AttributeLength The length of the value of the attribute read from the IB.
 * @param m_au8AttributeValue The value of the attribute read from the IB.
 **********************************************************************************************************************/

#define ADP_PIB_MAX_VALUE_LENGTH (64)

struct TAdpGetConfirm {
  uint8_t m_u8Status;
  uint32_t m_u32AttributeId;
  uint16_t m_u16AttributeIndex;
  uint8_t m_u8AttributeLength;
  uint8_t m_au8AttributeValue[ADP_PIB_MAX_VALUE_LENGTH];
};
typedef void (*AdpGetConfirm)(struct TAdpGetConfirm *pGetConfirm);

/**********************************************************************************************************************/
/** The AdpGetRequestSync primitive allows the upper layer to get the value of an attribute from the ADP information
 * synchronously.
 ***********************************************************************************************************************
 * @param u32AttributeId The identifier of the ADP IB attribute to read.
 * @param u16AttributeIndex The index within the table of the specified IB attribute to read.
 * @param pGetConfirm Get result.
 **********************************************************************************************************************/
void AdpGetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  struct TAdpGetConfirm *pGetConfirm);

/**********************************************************************************************************************/
/** The AdpMacGetRequest primitive allows the upper layer to get the value of an attribute from the MAC information base.
 * The upper layer cannot access directly the MAC layer while ADP is running
 ***********************************************************************************************************************
 * @param u32AttributeId The identifier of the MAC IB attribute to read.
 * @param u16AttributeIndex The index within the table of the specified IB attribute to read.
 **********************************************************************************************************************/
void AdpMacGetRequest(uint32_t u32AttributeId, uint16_t u16AttributeIndex);

/**********************************************************************************************************************/
/** The AdpMacGetConfirm primitive allows the upper layer to be notified of the completion of an AdpMacGetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the scan request.
 * @param m_u32AttributeId The identifier of the IB attribute read
 * @param m_u16AttributeIndex The index within the table of the specified IB attribute read.
 * @param m_u8AttributeLength The length of the value of the attribute read from the IB.
 * @param m_au8AttributeValue The value of the attribute read from the IB.
 **********************************************************************************************************************/

#define ADP_MAC_PIB_MAX_VALUE_LENGTH (144)

struct TAdpMacGetConfirm {
  uint8_t m_u8Status;
  uint32_t m_u32AttributeId;
  uint16_t m_u16AttributeIndex;
  uint8_t m_u8AttributeLength;
  uint8_t m_au8AttributeValue[ADP_MAC_PIB_MAX_VALUE_LENGTH];
};
typedef void (*AdpMacGetConfirm)(struct TAdpMacGetConfirm *pGetConfirm);

/**********************************************************************************************************************/
/** The AdpMacGetRequestSync primitive allows the upper layer to get the value of an attribute from the MAC information
* base synchronously. The upper layer cannot access directly the MAC layer while ADP is running.
***********************************************************************************************************************
* @param u32AttributeId The identifier of the ADP IB attribute to read.
* @param u16AttributeIndex The index within the table of the specified IB attribute to read.
* @param pGetConfirm Get result.
**********************************************************************************************************************/
void AdpMacGetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  struct TAdpMacGetConfirm *pGetConfirm);

/**********************************************************************************************************************/
/** The AdpSetRequest primitive allows the upper layer to set the value of an attribute in the ADP information base.
 ***********************************************************************************************************************
 * @param u32AttributeId The identifier of the ADP IB attribute set
 * @param u16AttributeIndex The index within the table of the specified IB attribute.
 * @param u8AttributeLength The length of the value of the attribute to set
 * @param pu8AttributeValue The value of the attribute to set
 **********************************************************************************************************************/
void AdpSetRequest(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue);

/**********************************************************************************************************************/
/** The AdpSetConfirm primitive allows the upper layer to be notified of the completion of an AdpSetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the scan request.
 * @param m_u32AttributeId The identifier of the IB attribute set.
 * @param m_u16AttributeIndex The index within the table of the specified IB attribute.
 **********************************************************************************************************************/
struct TAdpSetConfirm {
  uint8_t m_u8Status;
  uint32_t m_u32AttributeId;
  uint16_t m_u16AttributeIndex;
};
typedef void (*AdpSetConfirm)(struct TAdpSetConfirm *pSetConfirm);

/**********************************************************************************************************************/
/** The AdpSetRequestSync primitive allows the upper layer to set the value of an attribute in the ADP information base
* synchronously
***********************************************************************************************************************
* @param u32AttributeId The identifier of the ADP IB attribute set
* @param u16AttributeIndex The index within the table of the specified IB attribute.
* @param u8AttributeLength The length of the value of the attribute to set
* @param pu8AttributeValue The value of the attribute to set
* @param pSetConfirm The set confirm
**********************************************************************************************************************/
void AdpSetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue,
  struct TAdpSetConfirm *pSetConfirm);

/**********************************************************************************************************************/
/** The AdpMacSetRequest primitive allows the upper layer to set the value of an attribute in the MAC information base.
* The upper layer cannot access directly the MAC layer while ADP is running
***********************************************************************************************************************
* @param u32AttributeId The identifier of the ADP IB attribute set
* @param u16AttributeIndex The index within the table of the specified IB attribute.
* @param u8AttributeLength The length of the value of the attribute to set
* @param pu8AttributeValue The value of the attribute to set
**********************************************************************************************************************/
void AdpMacSetRequest(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue);

/**********************************************************************************************************************/
/** The AdpMacSetConfirm primitive allows the upper layer to be notified of the completion of an AdpMacSetRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the scan request.
 * @param m_u32AttributeId The identifier of the IB attribute set.
 * @param m_u16AttributeIndex The index within the table of the specified IB attribute.
 **********************************************************************************************************************/
struct TAdpMacSetConfirm {
  uint8_t m_u8Status;
  uint32_t m_u32AttributeId;
  uint16_t m_u16AttributeIndex;
};
typedef void (*AdpMacSetConfirm)(struct TAdpMacSetConfirm *pSetConfirm);

/**********************************************************************************************************************/
/** The AdpMacSetRequestSync primitive allows the upper layer to set the value of an attribute in the MAC information
 * base synchronously. The upper layer cannot access directly the MAC layer while ADP is running.
 ***********************************************************************************************************************
 * @param u32AttributeId The identifier of the ADP IB attribute set
 * @param u16AttributeIndex The index within the table of the specified IB attribute.
 * @param u8AttributeLength The length of the value of the attribute to set
 * @param pu8AttributeValue The value of the attribute to set
 * @param pSetConfirm The set confirm
 **********************************************************************************************************************/
void AdpMacSetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
  uint8_t u8AttributeLength, const uint8_t *pu8AttributeValue,
  struct TAdpMacSetConfirm *pSetConfirm);

/**********************************************************************************************************************/
/** The AdpNetworkStatusIndication primitive allows the next higher layer of a PAN coordinator or a coordinator to be
 * notified when a particular event occurs on the PAN.
 ***********************************************************************************************************************
 * @param m_u16PanId The 16-bit PAN identifier of the device from which the frame was received or to which the frame
 *    was being sent.
 * @param m_SrcDeviceAddress The individual device address of the entity from which the frame causing the error
 *    originated.
 * @param m_DstDeviceAddress The individual device address of the device for which the frame was intended.
 * @param m_u8Status The communications status.
 * @param m_u8SecurityLevel The security level purportedly used by the received frame.
 * @param m_u8KeyIndex The index of the key purportedly used by the originator of the received frame.
 **********************************************************************************************************************/
struct TAdpNetworkStatusIndication {
  uint16_t m_u16PanId;
  struct TAdpAddress m_SrcDeviceAddress;
  struct TAdpAddress m_DstDeviceAddress;
  uint8_t m_u8Status;
  uint8_t m_u8SecurityLevel;
  uint8_t m_u8KeyIndex;
  uint8_t m_u8MediaType;
};

typedef void (*AdpNetworkStatusIndication)(
  struct TAdpNetworkStatusIndication *pNetworkStatusIndication);

/**********************************************************************************************************************/
/** The AdpBufferIndication primitive allows the next higher layer to be notified when the modem has reached its
 * capability limit to perform the next frame..
 ***********************************************************************************************************************
 * @param m_bBufferReady TRUE: modem is ready to receipt more data frame;
 *                       FALSE: modem is not ready, stop sending data frame.
 **********************************************************************************************************************/
struct TAdpBufferIndication {
  bool m_bBufferReady;
};

typedef void (*AdpBufferIndication)(struct TAdpBufferIndication *pBufferIndication);

/**********************************************************************************************************************/
/** The AdpPREQIndication primitive allows the next higher layer to be notified when a PREQ frame is received
 * in unicast mode with Originator Address equal to Coordinator Address and with Destination Address equal to Device Address
 **********************************************************************************************************************/
typedef void (*AdpPREQIndication)(void);

/**********************************************************************************************************************/
/** The AdpRouteDiscoveryRequest primitive allows the upper layer to initiate a route discovery.
 ***********************************************************************************************************************
 * @param u16DstAddr The short unicast destination address of the route discovery.
 * @param u8MaxHops This parameter indicates the maximum number of hops allowed for the route discovery (Range: 0x01 - 0x0E)
 **********************************************************************************************************************/
void AdpRouteDiscoveryRequest(uint16_t u16DstAddr, uint8_t u8MaxHops);

/**********************************************************************************************************************/
/** The AdpRouteDiscoveryConfirm primitive allows the upper layer to be notified of the completion of a
 * AdpRouteDiscoveryRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the route discovery.
 **********************************************************************************************************************/
struct TAdpRouteDiscoveryConfirm {
  uint8_t m_u8Status;
};
typedef void (*AdpRouteDiscoveryConfirm)(
  struct TAdpRouteDiscoveryConfirm *pRouteDiscoveryConfirm);

/**********************************************************************************************************************/
/** The AdpPathDiscoveryRequest primitive allows the upper layer to initiate a path discovery.
 ***********************************************************************************************************************
 * @param u16DstAddr The short unicast destination address of the path discovery.
 * @param u8MetricType The metric type to be used for the path discovery. (Range: 0x00 - 0x0F)
 **********************************************************************************************************************/
void AdpPathDiscoveryRequest(uint16_t u16DstAddr, uint8_t u8MetricType);

/**********************************************************************************************************************/
/** The AdpPathDiscoveryConfirm primitive allows the upper layer to be notified of the completion of a
 * AdpPathDiscoveryRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the path discovery. (status can be INCOMPLETE and the other parameters contain the
 *				discovered path)
 * @param m_u16DstAddr The short unicast destination address of the path discovery.
 * @param m_u16Originator The originator of the path reply
 * @param m_u8PathMetricType Path metric type
 * @param m_u8ForwardHopsCount Number of path hops in the forward table
 * @param m_u8ReverseHopsCount Number of path hops in the reverse table
 * @param m_aForwardPath Table with the information of each hop in forward direction (according to m_u8ForwardHopsCount)
 * @param m_aReversePath Table with the information of each hop in reverse direction (according to m_u8ReverseHopsCount)
 **********************************************************************************************************************/
struct TAdpPathDiscoveryConfirm {
  uint8_t m_u8Status;
  uint16_t m_u16DstAddr;
  uint16_t m_u16OrigAddr;
  uint8_t m_u8MetricType;
  uint8_t m_u8ForwardHopsCount;
  uint8_t m_u8ReverseHopsCount;
  struct THopDescriptor m_aForwardPath[16];
  struct THopDescriptor m_aReversePath[16];
};
typedef void (*AdpPathDiscoveryConfirm)(
  struct TAdpPathDiscoveryConfirm *pPathDiscoveryConfirm);

/**********************************************************************************************************************/
/** The AdpLbpRequest primitive allows the upper layer of the client to send the LBP message to the server modem.
 ***********************************************************************************************************************
 * @param pDstAddr 16-bit address of LBA or LBD or 64 bit address (extended address of LBD)
 * @param u16NsduLength The size of the NSDU, in bytes
 * @param pNsdu The NSDU to send
 * @param u8NsduHandle The handle of the NSDU to transmit. This parameter is used to identify in the AdpLbpConfirm
 *                     primitive which request is concerned. It can be randomly chosen by the application layer.
 * @param u8MaxHops The number of times the frame will be repeated by network routers.
 * @param bDiscoveryRoute If TRUE, a route discovery procedure will be performed prior to sending the frame if a route
 *                        to the destination is not available in the routing table. If FALSE, no route discovery is performed.
 * @param u8QualityOfService The requested quality of service (QoS) of the frame to send. Allowed values are:
 *					0x00 = standard priority
 *					0x01 = high priority
 * @param bSecurityEnable If TRUE, this parameter enables the MAC layer security for sending the frame.
 **********************************************************************************************************************/
void AdpLbpRequest(const struct TAdpAddress *pDstAddr, uint16_t u16NsduLength,
  uint8_t *pNsdu, uint8_t u8NsduHandle, uint8_t u8MaxHops,
  bool bDiscoveryRoute, uint8_t u8QualityOfService, bool bSecurityEnable);

/**********************************************************************************************************************/
/** The AdpLbpRequestExt primitive allows the LBP layer of the client to send the LBP message to the server modem.
    This primitive extends the capabilities of AdpLbpRequest().
 ***********************************************************************************************************************
 * @param u8SrcAddressSize source address size (ADP_ADDRESS_16BITS or ADP_ADDRESS_64BITS)
 * @param pDstAddr 16-bit address of LBA or LBD or 64 bit address (extended address of LBD)
 * @param u16NsduLength The size of the NSDU, in bytes
 * @param pNsdu The NSDU to send
 * @param u8MaxHops The number of times the frame will be repeated by network routers.
 * @param bMulticast If TRUE, the frame will be sent as multicast.
 * @param bSecurityEnable If TRUE, this parameter enables the MAC layer security for sending the frame;
                          otherwise auto will be used.
 * @param callback Callback function.
 **********************************************************************************************************************/
void AdpLbpRequestExt(uint8_t u8SrcAddressSize, const struct TAdpAddress *pDstAddr,
		uint16_t u16NsduLength, uint8_t *pNsdu,
		bool bMulticast, /* = false */
		bool bSecurityEnable,
		ADP_Common_DataSend_Callback callback);

/**********************************************************************************************************************/
/** The AdpLbpConfirm primitive primitive allows the upper layer to be notified of the completion of a AdpLbpRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status code of a previous AdpLbpRequest identified by its NsduHandle.
 * @param m_u8NsduHandle The handle of the NSDU confirmed by this primitive.
 **********************************************************************************************************************/
struct TAdpLbpConfirm {
  uint8_t m_u8Status;
  uint8_t m_u8NsduHandle;
};
typedef void (*AdpLbpConfirm)(struct TAdpLbpConfirm *pLbpConfirm);

/**********************************************************************************************************************/
/** The AdpLbpIndication primitive is used to transfer a received LBP frame from the ADP layer to the upper layer.
 ***********************************************************************************************************************
 * @param m_u16SrcAddr Address of the LBA. When directly communicating with the LBD (using extended addressing), this
 *    field is set to 0xFFFF
 * @param m_u16NsduLength The size of the NSDU, in bytes; Range: 0 - 1280
 * @param m_pNsdu The NSDU received
 * @param m_u8LinkQualityIndicator The value of the link quality during reception of the frame.
 * @param m_bSecurityEnabled TRUE if the frame was received with a security level greater or equal to adpSecurityLevel,
 *		FALSE otherwise.
 **********************************************************************************************************************/
struct TAdpLbpIndication {
  uint16_t m_u16SrcAddr;
  uint16_t m_u16NsduLength;
  uint8_t *m_pNsdu;
  uint8_t m_u8LinkQualityIndicator;
  bool m_bSecurityEnabled;
};
typedef void (*AdpLbpIndication)(struct TAdpLbpIndication *pLbpIndication);

/**********************************************************************************************************************/
/** The AdpUpdNonVolatileDataIndication primitive allows the next higher layer to be notified when non-volatile stored
 * data must be updated to protect the system in case of critical failure.
 **********************************************************************************************************************/
struct TAdpNonVolatileData {
  uint32_t m_u32FrameCounter;
  uint32_t m_u32FrameCounterRF;
  uint16_t m_u16DiscoverSeqNumber;
  uint8_t m_u8BroadcastSeqNumber;
};
typedef void (*AdpUpdNonVolatileDataIndication)(struct TAdpNonVolatileData *pNonVolatileData);

/**********************************************************************************************************************/
/** The AdpRouteNotFoundIndication primitive is used to indicate the upper layer that a route is not available
 ***********************************************************************************************************************
 * @param m_u16SrcAddr Address of the frame originator
 * @param m_u16DestAddr Address of the frame final destination.
 * @param m_u16DestAddr Address of the next hop to frame final destination.
 * @param m_u16NsduLength The size of the NSDU failed to be delivered, in bytes; Range: 0 - 1280.
 * @param m_pNsdu The NSDU failed to be delivered.
 **********************************************************************************************************************/
struct TAdpRouteNotFoundIndication {
  uint16_t m_u16SrcAddr;
  uint16_t m_u16DestAddr;
  uint16_t m_u16NextHopAddr;
  uint16_t m_u16PreviousHopAddr;
  uint16_t m_u16RouteCost;
  uint8_t m_u8HopCount;
  uint8_t m_u8WeakLinkCount;
  bool m_bRouteJustBroken;
  bool m_bCompressedHeader;
  uint16_t m_u16NsduLength;
  uint8_t *m_pNsdu;
};
typedef void (*AdpRouteNotFoundIndication)(struct TAdpRouteNotFoundIndication *pBrokenRouteIndication);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
struct TAdpNotifications {
  AdpDataConfirm fnctAdpDataConfirm;
  AdpDataIndication fnctAdpDataIndication;
  AdpDiscoveryConfirm fnctAdpDiscoveryConfirm;
  AdpDiscoveryIndication fnctAdpDiscoveryIndication;
  AdpNetworkStartConfirm fnctAdpNetworkStartConfirm;
  AdpNetworkJoinConfirm fnctAdpNetworkJoinConfirm;
  AdpNetworkLeaveIndication fnctAdpNetworkLeaveIndication;
  AdpNetworkLeaveConfirm fnctAdpNetworkLeaveConfirm;
  AdpResetConfirm fnctAdpResetConfirm;
  AdpSetConfirm fnctAdpSetConfirm;
  AdpMacSetConfirm fnctAdpMacSetConfirm;
  AdpGetConfirm fnctAdpGetConfirm;
  AdpMacGetConfirm fnctAdpMacGetConfirm;
  AdpLbpConfirm fnctAdpLbpConfirm;
  AdpLbpIndication fnctAdpLbpIndication;
  AdpRouteDiscoveryConfirm fnctAdpRouteDiscoveryConfirm;
  AdpPathDiscoveryConfirm fnctAdpPathDiscoveryConfirm;
  AdpNetworkStatusIndication fnctAdpNetworkStatusIndication;
  AdpBufferIndication fnctAdpBufferIndication;
  AdpPREQIndication fnctAdpPREQIndication;
  AdpUpdNonVolatileDataIndication fnctAdpUpdNonVolatileDataIndication;
  AdpRouteNotFoundIndication fnctAdpRouteNotFoundIndication;
};
#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/

