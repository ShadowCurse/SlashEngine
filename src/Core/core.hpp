#ifndef SLASHENGINE_CORE_CORE_H_
#define SLASHENGINE_CORE_CORE_H_

#ifdef SL_WINDOWS
//  Microsoft
#ifdef SL_BUILD_DLL
#define Slash_API __declspec(dllexport)
#else
#define Slash_API __declspec(dllimport)
#endif
#elif defined SL_LINUX
#include "slash_pch.hpp"
//  GCC
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

namespace slash {

template <typename T> using Unique = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Unique<T> CreateUnique(Args &&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using Shared = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Shared<T> CreateShared(Args &&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace slash

#endif // SLASHENGINE_CORE_CORE_H_