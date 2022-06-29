#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "QueueMng.h"

#define LOG_LEVEL LOG_LEVEL_ADP
#include <Logger.h>

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
void Queue_Push(
  struct TQueueElement *pQueueRoot,
  struct TQueueElement *pQueueElement
  )
{
  if ((pQueueRoot != NULL) && (pQueueElement != NULL) && (pQueueRoot != pQueueElement)) {
    struct TQueueElement *pT = pQueueRoot, *pTEnd = NULL;

    // check if the element is not already in the queue
    while ((pT != NULL) && (pT != pQueueElement)) {
      pTEnd = pT;
      pT = pT->m_pNext;
    }

    // if queue element not found, add it
    if (pT == 0L) {
      // add the timer at the end of the list
      pTEnd->m_pNext = pQueueElement;
      pQueueElement->m_pNext = 0L;
    }
    // else element already in the queue
  }
}

/**********************************************************************************************************************/
/**
 **********************************************************************************************************************/
struct TQueueElement *Queue_Pop(
  struct TQueueElement *pQueueRoot
  )
{
  struct TQueueElement *pRet = NULL;

  if ((pQueueRoot != NULL) && (pQueueRoot->m_pNext != NULL)) {
    pRet = pQueueRoot->m_pNext;
    pQueueRoot->m_pNext = pRet->m_pNext;
    pRet->m_pNext = NULL;
  }

  return pRet;
}
