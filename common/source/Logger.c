/**********************************************************************************************************************/
/** \addtogroup Common_Module
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************/
/** This file contains implementation of the logger.
 ***********************************************************************************************************************
 *
 * @file
 *
 **********************************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <Logger.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern uint32_t oss_get_up_time_ms(void);

void Log(const char *strFormat, ...)
{
  va_list arglist;

  va_start(arglist, strFormat);
  vprintf(strFormat, arglist);
  va_end(arglist);
}

void LogBuffer(const void *pBuffer, uint32_t u32Length, const char *strFormat, ...)
{
  // this method avoids using printf which is slow
  static const char *HexBytes = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
  const uint8_t *pu8Data = (const uint8_t *) pBuffer;

  va_list vaArgs;
  uint32_t u32Index;

  va_start(vaArgs, strFormat);
  vprintf(strFormat, vaArgs);
  va_end(vaArgs);

  for (u32Index = 0; u32Index < u32Length; u32Index ++) {
    fputc(HexBytes[2 * pu8Data[u32Index]], stdout);
    fputc(HexBytes[2 * pu8Data[u32Index] + 1], stdout);
  }
}

void LogDump(const void *pBuffer, uint32_t u32Length)
{
  // this method avoids using printf which is slow
  static const char *HexBytes = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
  const uint8_t *pu8Data = (const uint8_t *) pBuffer;

  uint32_t u32RemainingData;
  uint32_t u32Index;
  uint32_t u32_dump_offset = 0;
  uint32_t u32RunningTime;
  uint8_t u8Hours, u8Minutes, u8Seconds;
  uint16_t u8Millis;

  /* Print timestamp */
  u32RunningTime = oss_get_up_time_ms();
  u8Millis = u32RunningTime % 1000;
  u32RunningTime /= 1000;
  u8Seconds = u32RunningTime % 60;
  u32RunningTime /= 60;
  u8Minutes = u32RunningTime % 60;
  u32RunningTime /= 60;
  u8Hours = u32RunningTime;
  printf("\r\n%02hhu:%02hhu:%02hhu.%03hu", u8Hours, u8Minutes, u8Seconds, u8Millis);

  u32RemainingData = u32Length;
  printf("\r\n%08x ", u32_dump_offset);
  for (u32Index = 0; u32Index < u32Length; u32Index ++) {
    fputc(HexBytes[2 * pu8Data[u32Index]], stdout);
    fputc(HexBytes[2 * pu8Data[u32Index] + 1], stdout);
    if (u32Index % 16 == 15) {
      u32RemainingData -= 16;
      u32_dump_offset += 16;
      if (u32RemainingData > 0) {
        printf("\r\n%08x ", u32_dump_offset);
      }
    }
    else {
      fputc(0x20, stdout); // Space
    }
  }
}

void LogAssert(const char *file, uint32_t line, const char *condition)
{
  Log("FATAL %s:%d %s\r\n", file, line, condition);
}

// this function returns only the file name from full path
const char *LogGetFileName(const char *fullpath)
{
  const char *ret = fullpath;
  int n;

  int nLen = strlen(fullpath);
  for (n = nLen - 1; n >= 0; n --) {
    if ((fullpath[n] == '\\') || (fullpath[n] == '/')) {
      ret = &fullpath[n + 1];
      break;
    }
  }

  return ret;
}

/**********************************************************************************************************************/
/** @}
 **********************************************************************************************************************/
