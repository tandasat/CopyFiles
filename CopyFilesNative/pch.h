//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//
// macro utilities
//

////////////////////////////////////////////////////////////////////////////////
//
// constants and macros
//

#define LOG_DEBUG(format, ...) LogPrint(__FUNCTION__, (format), __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
//
// types
//

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

////////////////////////////////////////////////////////////////////////////////
//
// implementations
//

// Debug prints
inline HRESULT LogPrint(const char *FunctionName, const char *Format, ...) {
  va_list args;
  va_start(args, Format);
  char logMessage[300];
  auto status = ::vsnprintf_s(logMessage, _TRUNCATE, Format, args);
  va_end(args);
  if (status == -1) {
    return status;
  }

  wchar_t message[400];
  status = ::swprintf_s(message, RTL_NUMBER_OF(message), L"%-20S\t%S\n",
                        FunctionName, logMessage);
  if (!SUCCEEDED(status)) {
    return status;
  }
  ::OutputDebugString(message);
  return status;
}
