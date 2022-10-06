/**********************************************************************************************************************/
/** \addtogroup AdaptationSublayer
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains types shared by ADP and Routing libs.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/

#ifndef __ADP_SHARED_TYPES_H__
#define __ADP_SHARED_TYPES_H__

#include "AdpApiTypes.h"
#include "system/time/sys_time.h"

/**********************************************************************************************************************/
/** The ADP_Common_DataSend_Callback primitive reports the results of a ADP_Common_DataSend Request
 **********************************************************************************************************************/
typedef void (*ADP_Common_DataSend_Callback)(uint8_t u8Status);

/**********************************************************************************************************************/
/** The MCPS-DATA.confirm primitive reports the results of a MCPS-DATA.request
 **********************************************************************************************************************/
typedef void (*AdpMac_Callback_DataConfirm)(uint8_t u8Status, void *pUserData);

#pragma pack(push,1)

struct TDataSendParameters {
  struct TAdpAddress m_SrcDeviceAddress;
  struct TAdpAddress m_DstDeviceAddress;
  bool m_bDiscoverRoute;
  uint8_t m_u8Handle;
  uint8_t m_u8MaxHops;
  uint8_t m_u8DataType;
  uint8_t m_u8OriginalDataType;
  uint8_t m_u8QualityOfService;
  uint8_t m_u8Security;
  uint8_t m_u8BroadcastSeqNo;
  bool m_bMeshHeaderNeeded;
  bool m_bMulticast;
  uint16_t m_u16DataLength;
  uint16_t m_u16FragmentOffset;
  uint16_t m_u16LastFragmentSize;
  uint8_t m_u8BufferOffset;
  uint16_t m_u16DatagramTag;
  uint16_t m_u16DatagramSize;
  uint8_t m_u8NumRepairReSendAttemps;
  ADP_Common_DataSend_Callback m_fnctCallback;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
};

struct TDataSend1280 {
  struct TDataSendParameters m_SendParameters;
  uint8_t m_au8Data[1281]; // 1280 + 1 extra byte needed for Lowpan IPv6 header (compressed or not)
  struct TTimer m_fragTimer;
};

struct TDataSend400 {
  struct TDataSendParameters m_SendParameters;
  uint8_t m_au8Data[401]; // payload size + extra data for headers + 1 extra byte needed for Lowpan IPv6 header (compressed or not)
};

struct TDataSend100 {
  struct TDataSendParameters m_SendParameters;
  uint8_t m_au8Data[101]; // payload size + extra data for headers + 1 extra byte needed for Lowpan IPv6 header (compressed or not)
};

struct TProcessQueueEntry {
  struct TDataSendParameters *m_pSendParameters;
  uint8_t *m_pData;
  uint16_t m_u16DataSize;
  bool m_bProcessing;
  bool m_bDelayed;
  struct TProcessQueueEntry *m_pNext;
  int32_t m_i32DelayTime;
  int32_t m_i32ValidTime;
  struct TTimer *m_pFragTimer;
};


// The maximum number of fragments which can be used to receive a fragmented message
#define MAX_NUMBER_OF_FRAGMENTS 6

struct TFragmentInfo {
  uint16_t m_u16Offset;
  uint16_t m_u16Size;
};

struct TLowpanFragmentedData {
  uint16_t m_u16DatagramOrigin;
  uint16_t m_u16DatagramTag;
  uint16_t m_u16DatagramSize;
  uint8_t m_au8Data[1281]; // 1280 max IPv6 packet + 1 byte IPv6 6Lowpan header
  struct TFragmentInfo m_Fragments[MAX_NUMBER_OF_FRAGMENTS];
  bool m_bWasCompressed;
  /// Absolute time in milliseconds when the entry expires
  int32_t m_i32ValidTime;
};

struct TUserDataRREQRREP {
  uint8_t m_u8FrameType;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
  uint16_t m_u16DstAddr;
  void * m_pRREPGeneration;
  void * m_pRouteEntry;
};

struct TUserDataPREQ {
  uint16_t m_u16DstAddr;
  uint16_t m_u16OrigAddr;
  uint16_t m_u16NextHopAddr;
  uint16_t m_u16RsvBits;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
  uint8_t m_u8MetricType;
  uint8_t m_u8ForwardHops;
};

struct TUserDataData {
  void *m_generic_pointer;
};

union TUserDataUnion {
  struct TUserDataRREQRREP m_sUserDataRREQRREP;
  struct TUserDataPREQ m_sUserDataPREQ;
  struct TUserDataData m_sUserDataData;
  uint8_t auc_buffer[8];
};

struct TAdpMac_NeighbourDescriptor {
  uint16_t m_u16ShortAddress;
  enum EAdpMac_Modulation m_eModulation;
  uint8_t m_u8ActiveTones;
  uint8_t m_u8SubCarriers;
  uint8_t m_u8Lqi;
};

struct TAdpMac_DataRequest {
  uint8_t m_u8SrcAddrSize;
  struct TAdpAddress m_DstDeviceAddress;
  uint16_t m_u16MsduLength;
  uint8_t m_Msdu[400];
  uint8_t m_u8TxOptions;
  uint8_t m_u8QualityOfService;
  uint8_t m_u8SecurityLevel;
  uint8_t m_u8KeyIndex;
#if defined(__PLC_MAC__) && defined(__RF_MAC__)
  uint8_t m_u8MediaType;
#endif
};

#pragma pack(pop)

/**********************************************************************************************************************/
/** Functions to get pointers and sizes from AdpConf file to populate TDataSendContext
 **********************************************************************************************************************/
struct TDataSend1280* AdpConfGet1280BufPtr(void);
struct TDataSend400* AdpConfGet400BufPtr(void);
struct TDataSend100* AdpConfGet100BufPtr(void);
uint8_t AdpConfGet1280BufCount(void);
uint8_t AdpConfGet400BufCount(void);
uint8_t AdpConfGet100BufCount(void);
struct TProcessQueueEntry* AdpConfGetProcessQueuePtr(void);
uint8_t AdpConfGetProcessQueueCount(void);
struct TLowpanFragmentedData* AdpConfGetFragmentedTransferTablePtr(void);
uint8_t AdpConfGetFragmentedTransferTableCount(void);

#endif

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
