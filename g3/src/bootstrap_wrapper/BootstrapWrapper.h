/**********************************************************************************************************************/

/** \addtogroup AdaptationSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/

/** This file contains data types and functions to wrap to bootstrap
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef __BOOSTRAP_WRAPPER_H__
#define __BOOSTRAP_WRAPPER_H__

#include "/adp/AdpSharedTypes.h"

/* Maximum size of a bootstrap command */
#define BOOTSTRAP_COMMAND_BUFFER_SIZE 200

/* EAP-PSK Defines & Types */
#define NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_S   34
#define NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P   36

#define NETWORK_ACCESS_IDENTIFIER_SIZE_S_ARIB   NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_S
#define NETWORK_ACCESS_IDENTIFIER_SIZE_P_ARIB   NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P

#define NETWORK_ACCESS_IDENTIFIER_SIZE_S_CENELEC_FCC   8
#define NETWORK_ACCESS_IDENTIFIER_SIZE_P_CENELEC_FCC   8

/**********************************************************************************************************************/

/** The EAP_PSK NetworkAccessIdentifier P & S types
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskNetworkAccessIdentifierP {
	uint8_t uc_size;
	uint8_t m_au8Value[NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_P];
};

struct TEapPskNetworkAccessIdentifierS {
	uint8_t uc_size;
	uint8_t m_au8Value[NETWORK_ACCESS_IDENTIFIER_MAX_SIZE_S];
};

/**********************************************************************************************************************/

/** The EAP_PSK key type
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskKey {
	uint8_t m_au8Value[16];
};

/**********************************************************************************************************************/

/** The EAP_PSK MSK type
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskMSK {
	uint8_t m_au8Value[64];
};

/**********************************************************************************************************************/

/** The EAP_PSK RAND type
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskRand {
	uint8_t m_au8Value[16];
};

/**********************************************************************************************************************/

/** The EAP_PSK NetworkAccessIdentifier
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskNetworkAccessIdentifier {
	uint8_t m_au8Value[36]; /* the size is calculated to be able to store the IdP for ARIB band */
	uint8_t m_u8Length;
};

/**********************************************************************************************************************/

/** The EAP_PSK_Context type keeps information needed for EAP-PSK calls
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
struct TEapPskContext {
	struct TEapPskKey m_Kdk; /* Derivation key */
	struct TEapPskKey m_Ak; /* Authentication key */
	struct TEapPskKey m_Tek; /* Transient key */
	struct TEapPskMSK m_Msk; /* Master Session key */
	struct TEapPskNetworkAccessIdentifier m_IdS;
	struct TEapPskRand m_RandP;
	struct TEapPskRand m_RandS;
};

/**********************************************************************************************************************/

/** The BootstrapWrapper_JoinConfirm primitive allows the upper layer to be notified of the completion of an
 * BootstrapWrapper_JoinRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 * @param m_u16NetworkAddress The 16-bit network address that was allocated to the device.
 * @param m_u16PanId The 16-bit address of the PAN of which the device is now a member.
 **********************************************************************************************************************/
typedef void (*BootstrapWrapper_JoinConfirm)(uint8_t m_u8Status, uint16_t m_u16NetworkAddress, uint16_t m_u16LBAAddress);

/**********************************************************************************************************************/

/** The BootstrapWrapper_KickNotify primitive allows the upper layer to be notified when the device has been kicked
 ***********************************************************************************************************************
 **********************************************************************************************************************/
typedef void (*BootstrapWrapper_KickNotify)(void);

struct TBootstrapNotifications {
	BootstrapWrapper_JoinConfirm fnctJoinConfirm;
	BootstrapWrapper_KickNotify fnctKickNotify;
};

/** The BootstrapWrapper_Reset primitive restarts the bootstrap protocol
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void BootstrapWrapper_Reset(void);

/** The BootstrapWrapper_ClearEapContext primitive is used to clear the EAP context
 ***********************************************************************************************************************
 **********************************************************************************************************************/
void BootstrapWrapper_ClearEapContext(void);

/** The BootstrapWrapper_InitEapPsk primitive allows the upper layer to be notified of the completion of an
 * BootstrapWrapper_JoinRequest.
 ***********************************************************************************************************************
 * @param m_u8Status The status of the request.
 * @param m_u16NetworkAddress The 16-bit network address that was allocated to the device.
 * @param m_u16PanId The 16-bit address of the PAN of which the device is now a member.
 **********************************************************************************************************************/
void BootstrapWrapper_InitEapPsk(const struct TEapPskKey *pKey);

/** The BootstrapWrapper_ForceJoinStatus primitive allows the upper layer to set the join status
 ***********************************************************************************************************************
 * @param bJoined true to set the status as "joined", false to set it as "not joined"
 **********************************************************************************************************************/
void BootstrapWrapper_ForceJoinStatus(bool bJoined);

/** The BootstrapWrapper_ForceJoined primitive allows the upper layer to set the join status with callbacks:
 *   - fnctJoinConfirm callback will be called once the joining process ends.
 *   - fnctKickNotify callback will be called if the node has been kicked from the network.
 ***********************************************************************************************************************
 * @param m_u16ShortAddress The 16-bit network address to be forced
 * @param pEUI64Address The EUI64 address of the node.
 * @param fnctJoinConfirm Callback to notify the join confirm.
 * @param fnctKickNotify Callback to notify that the node has been kicked from the network.
 **********************************************************************************************************************/
void BootstrapWrapper_ForceJoined(uint16_t u16ShortAddress,
		struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify);

#if defined(__PLC_MAC__) && defined(__RF_MAC__)
/** The BootstrapWrapper_JoinRequest primitive allows the upper layer to init the joining process.
 *  After calling BootstrapWrapper_JoinRequest(), the bootstrap module will create the joining message, and send it
 *  to the network using AdpLbpRequestExt() from ADP.
 *   - fnctJoinConfirm callback will be called once the joining process ends.
 *   - fnctKickNotify callback will be called if the node has been kicked from the network.
 ***********************************************************************************************************************
 * @param m_u16LbaAddress The short address of the LBA.
 * @param u8MediaType The Media Type to use when communiating with LBA.
 * @param pEUI64Address The EUI64 address of the node.
 * @param fnctJoinConfirm Callback to notify the join confirm.
 * @param fnctKickNotify Callback to notify that the node has been kicked from the network.
 **********************************************************************************************************************/
void BootstrapWrapper_JoinRequest(uint16_t m_u16LbaAddress, uint8_t u8MediaType, struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify);
#else
/** The BootstrapWrapper_JoinRequest primitive allows the upper layer to init the joining process.
 *  After calling BootstrapWrapper_JoinRequest(), the bootstrap module will create the joining message, and send it
 *  to the network using AdpLbpRequestExt() from ADP.
 *   - fnctJoinConfirm callback will be called once the joining process ends.
 *   - fnctKickNotify callback will be called if the node has been kicked from the network.
 ***********************************************************************************************************************
 * @param m_u16LbaAddress The short address of the LBA.
 * @param pEUI64Address The EUI64 address of the node.
 * @param fnctJoinConfirm Callback to notify the join confirm.
 * @param fnctKickNotify Callback to notify that the node has been kicked from the network.
 **********************************************************************************************************************/
void BootstrapWrapper_JoinRequest(uint16_t m_u16LbaAddress, struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify);
#endif

/** The BootstrapWrapper_ProcessMessage primitive allows the upper layer to pass the bootstrap messages received from
 *  the network, to the bootstrap module.
 ***********************************************************************************************************************
 * @param pSrcDeviceAddress The Source Device Address.
 * @param u16MessageLength Message Length.
 * @param pMessageBuffer Pointer to message buffer.
 * @param bSecurityEnabled Flag indicating whether security is enabled.
 **********************************************************************************************************************/
void BootstrapWrapper_ProcessMessage(const struct TAdpAddress *pSrcDeviceAddress, uint16_t u16MessageLength,
		uint8_t *pMessageBuffer, bool bSecurityEnabled);

/** The BootstrapWrapper_LeaveRequest primitive allows the upper layer to request the leave from the network.
 ***********************************************************************************************************************
 * @param pEUI64Address The EUI64 address of the node.
 * @param callback Callback to notify that the leave has been sent.
 **********************************************************************************************************************/
void BootstrapWrapper_LeaveRequest(const struct TAdpExtendedAddress *pEUI64Address, ADP_Common_DataSend_Callback callback);

#endif

/**********************************************************************************************************************/

/** @}
 **********************************************************************************************************************/
