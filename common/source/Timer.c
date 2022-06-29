#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "Timer.h"

#define LOG_LEVEL LOG_LEVEL_ADP
#include <Logger.h>

// The head of the timers list
struct TTimer *g_pRootTimer = NULL;
struct TTimer *g_pRootTimerMs = NULL;

extern uint32_t oss_get_up_time_ms(void);
extern uint32_t oss_get_up_time_100ms(void);
extern uint32_t oss_get_up_time_10s(void);

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_EventHandler(void)
{
  bool bRestart = false;
  struct TTimer *pT;

  do {
    pT = g_pRootTimer;
    bRestart = false;

    while (pT != 0L) {
      if (Timer_IsPast(pT->m_i32ExpirationTime)) {
        // Timer expired: remove it from the list and call the callback
        Timer_Unregister(pT);

        pT->m_fnctCallback(pT);

        // restart
        bRestart = true;
        break;
      }

      pT = pT->m_pNextTimer;
    }
  } while (bRestart);

  do {
    pT = g_pRootTimerMs;
    bRestart = false;

    while (pT != 0L) {
      if (Timer_IsPastMs(pT->m_i32ExpirationTime)) {
        // Timer expired: remove it from the list and call the callback
        Timer_UnregisterMs(pT);

        pT->m_fnctCallback(pT);

        // restart
        bRestart = true;
        break;
      }

      pT = pT->m_pNextTimer;
    }
  } while (bRestart);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_Register(struct TTimer *pTimer, uint32_t u32TenthsSeconds)
{
  struct TTimer *pT = g_pRootTimer, *pTEnd = NULL;

  // compute the expiration time; if the timer is already registered, it's expiration time will be updated
  pTimer->m_i32ExpirationTime = Timer_SignedSysGetUpTimeTenthsSeconds() + u32TenthsSeconds;

  // check if the timer is already registered
  while ((pT != NULL) && (pT != pTimer)) {
    pTEnd = pT;
    pT = pT->m_pNextTimer;
  }

  // if timer not found, register it
  if (pT == 0L) {
    // add the timer at the end of the list
    if (g_pRootTimer == NULL) {
      g_pRootTimer = pTimer;
    }
    else {
      pTEnd->m_pNextTimer = pTimer;
    }
    pTimer->m_pNextTimer = 0L;
  }
  // else timer already registered; m_i32ExpirationTime already updated
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_RegisterMs(struct TTimer *pTimer, uint32_t u32MilliSeconds)
{
  struct TTimer *pT = g_pRootTimerMs, *pTEnd = NULL;

  // compute the expiration time; if the timer is already registered, it's expiration time will be updated
  pTimer->m_i32ExpirationTime = Timer_SignedSysGetUpTimeMs() + u32MilliSeconds;

  // check if the timer is already registered
  while ((pT != NULL) && (pT != pTimer)) {
    pTEnd = pT;
    pT = pT->m_pNextTimer;
  }

  // if timer not found, register it
  if (pT == 0L) {
    // add the timer at the end of the list
    if (g_pRootTimerMs == NULL) {
      g_pRootTimerMs = pTimer;
    }
    else {
      pTEnd->m_pNextTimer = pTimer;
    }
    pTimer->m_pNextTimer = 0L;
  }
  // else timer already registered; m_i32ExpirationTime already updated
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsRegistered(struct TTimer *pTimer)
{
  struct TTimer *pT = g_pRootTimer;
  while ((pT != NULL) && (pT != pTimer)) {
    pT = pT->m_pNextTimer;
  }
  return (pT == pTimer);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsRegisteredMs(struct TTimer *pTimer)
{
  struct TTimer *pT = g_pRootTimerMs;
  while ((pT != NULL) && (pT != pTimer)) {
    pT = pT->m_pNextTimer;
  }
  return (pT == pTimer);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_Unregister(struct TTimer *pTimer)
{
  if (g_pRootTimer == pTimer) {
    g_pRootTimer = g_pRootTimer->m_pNextTimer;
  }
  else {
    struct TTimer *pT = g_pRootTimer;
    // search the timer
    while ((pT != 0L) && (pT->m_pNextTimer != pTimer)) {
      pT = pT->m_pNextTimer;
    }

    if ((pT != 0L) && (pT->m_pNextTimer == pTimer)) {
      // remove the timer from the list
      pT->m_pNextTimer = pTimer->m_pNextTimer;
    }
    // else the timer is not registered
  }

  pTimer->m_pNextTimer = 0L;
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_UnregisterMs(struct TTimer *pTimer)
{
  if (g_pRootTimerMs == pTimer) {
    g_pRootTimerMs = g_pRootTimerMs->m_pNextTimer;
  }
  else {
    struct TTimer *pT = g_pRootTimerMs;
    // search the timer
    while ((pT != 0L) && (pT->m_pNextTimer != pTimer)) {
      pT = pT->m_pNextTimer;
    }

    if ((pT != 0L) && (pT->m_pNextTimer == pTimer)) {
      // remove the timer from the list
      pT->m_pNextTimer = pTimer->m_pNextTimer;
    }
    // else the timer is not registered
  }

  pTimer->m_pNextTimer = 0L;
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Timer_ResetAll(void)
{
  g_pRootTimer = 0L;
  g_pRootTimerMs = 0L;
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsPast(int32_t i32TimeValue)
{
  return (Timer_SignedSysGetUpTimeTenthsSeconds() - i32TimeValue > 0);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsPastMs(int32_t i32TimeValue)
{
  return (Timer_SignedSysGetUpTimeMs() - i32TimeValue > 0);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsPast10Seconds(int32_t i32TimeValue)
{
  return (Timer_SignedSysGetUpTime10Seconds() - i32TimeValue > 0);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
bool Timer_IsPastCmp(int32_t i32Time1, int32_t i32Time2)
{
  return (i32Time1 - i32Time2 > 0);
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
int32_t Timer_SignedSysGetUpTimeTenthsSeconds(void)
{
  return (int32_t) oss_get_up_time_100ms();
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
int32_t Timer_SignedSysGetUpTimeMs(void)
{
  return (int32_t) oss_get_up_time_ms();
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
int32_t Timer_SignedSysGetUpTime10Seconds(void)
{
  return (int32_t) oss_get_up_time_10s();
}
