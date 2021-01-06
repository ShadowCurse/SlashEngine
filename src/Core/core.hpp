#ifndef SLASHENGINE_CORE_CORE_H_
#define SLASHENGINE_CORE_CORE_H_

#include "slash_pch.hpp"

#ifdef SL_WINDOWS
//  Microsoft
#ifdef SL_BUILD_DLL
#define Slash_API __declspec(dllexport)
#else
#define Slash_API __declspec(dllimport)
#endif
#elif defined SL_LINUX
#ifdef SL_BUILD_DLL
#define Slash_API __attribute__((visibility("default")))
#else
#define Slash_API
#endif
#else
#pragma warning Unknown plaform
#endif

#ifdef SL_ENABLE_ASSERTS
#define SL_ASSERT(x, ...)                                                      \
  {                                                                            \
    if (!(x)) {                                                                \
      SL_ERROR("Assertion Failed: {0}", __VA_ARGS__);                          \
    }                                                                          \
  }
#define SL_CORE_ASSERT(x, ...)                                                 \
  {                                                                            \
    if (!(x)) {                                                                \
      SL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                     \
    }                                                                          \
  }
#else
#define SL_ASSERT(x, ...)
#define SL_CORE_ASSERT(x, ...)
#endif

#endif // SLASHENGINE_CORE_CORE_H_