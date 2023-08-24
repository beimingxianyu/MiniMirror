#pragma once

#define MM_PLATFORM_WINDOWS 1
#define MM_PLATFORM_LINUX 2
#define MM_PLATFORM_APPLE 3

#define MM_COMPILER_MSVC 1
#define MM_COMPILER_GNUC 2
#define MM_COMPILER_CLANG 3
#define MM_COMPILER_APPLECLANG 4

#define MM_ENDIAN_LITTLE 1
#define MM_ENDIAN_BIG 2

#define MM_ARCH_32 1
#define MM_ARCH_64 2

// Platform
#if defined(__WIN32__) || defined(_WIN32)
#define MM_PLATFORM MM_PLATFORM_WINDOWS
#define MM_PLATFORM_IS_WINDOWS
#elif defined(__APPLE_CC__)
#define MM_PLATFORM MM_PLATFORM_APPLE
#define MM_PLATFORM_IS_APPLE
#else
#define MM_PLATFORM MM_PLATFORM_LINUX
#define MM_PLATFORM_IS_LINUX
#endif

// Compiler
#if defined(__clang__)
#if defined __apple_build_version__
#define MM_COMPILER MM_COMPILER_APPLECLANG
#else
#define MM_COMPILER MM_COMPILER_CLANG
#endif
#define MM_COMPILER_IS_CLANG
#define MM_COMP_VER \
  (((__clang_major__)*100) + (__clang_minor__ * 10) + __clang_patchlevel__)
#elif defined(__GNUC__)
#define MM_COMPILER MM_COMPILER_GNUC
#define MM_COMPILER_IS_GCC
#define MM_COMP_VER \
  (((__GNUC__)*100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#define MM_COMPILER MM_COMPILER_MSVC
#define MM_COMPILER_IS_MSVC
#define MM_COMP_VER _MSC_VER
#else
#error "No known compiler. Abort! Abort!"
#endif

// Architecture
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || \
    defined(__alpha__) || defined(__ia64__) || defined(__s390__) ||     \
    defined(__s390x__) || defined(_LP64) || defined(__LP64__)
#define MM_ARCH_TYPE MM_ARCH_64
#else
#define MM_ARCH_TYPE MM_ARCH_32
#endif

#if defined(MM_COMPILER_IS_MSVC)
#define MM_FUNCTION_NAME __FUNCTION__
#else
#define MM_FUNCTION_NAME __PRETTY_FUNCTION__
#endif
