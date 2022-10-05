#include <stdbool.h>
#include <stdint.h>
#include <AdpApiTypes.h>
#include <BootstrapWrapper.h>
#include <ProcessLbp.h>
#include <string.h>

void BootstrapWrapper_Reset(void)
{
	LBP_Reset();
}

void BootstrapWrapper_ClearEapContext(void)
{
	LBP_ClearEapContext();
}

void BootstrapWrapper_ForceJoinStatus(bool bJoined)
{
	LBP_ForceJoinStatus(bJoined);
}

void BootstrapWrapper_ForceJoined(uint16_t u16ShortAddress,
		struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify)
{
	LBP_ForceJoined(u16ShortAddress, pEUI64Address, fnctJoinConfirm, fnctKickNotify);
}

void BootstrapWrapper_InitEapPsk(const struct TEapPskKey *pKey)
{
	LBP_InitEapPsk(pKey);
}

#if defined(__PLC_MAC__) && defined(__RF_MAC__)
void BootstrapWrapper_JoinRequest(uint16_t m_u16LbaAddress, uint8_t u8MediaType, struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify)
{
	LBP_JoinRequest(m_u16LbaAddress, u8MediaType, pEUI64Address, fnctJoinConfirm, fnctKickNotify);
}
#else
void BootstrapWrapper_JoinRequest(uint16_t m_u16LbaAddress, struct TAdpExtendedAddress *pEUI64Address,
		BootstrapWrapper_JoinConfirm fnctJoinConfirm,
		BootstrapWrapper_KickNotify fnctKickNotify)
{
	LBP_JoinRequest(m_u16LbaAddress, pEUI64Address, fnctJoinConfirm, fnctKickNotify);
}
#endif

void BootstrapWrapper_LeaveRequest(const struct TAdpExtendedAddress *pEUI64Address, ADP_Common_DataSend_Callback callback)
{
	LBP_LeaveRequest(pEUI64Address, callback);
}

void BootstrapWrapper_ProcessMessage(const struct TAdpAddress *pSrcDeviceAddress, uint16_t u16MessageLength,
		uint8_t *pMessageBuffer, bool bSecurityEnabled)
{
	LBP_ProcessMessage(pSrcDeviceAddress, u16MessageLength, pMessageBuffer, bSecurityEnabled);
}