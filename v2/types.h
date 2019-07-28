#ifndef TYPES_H_
#define TYPES_H_

#ifndef _MSC_VER
#include <stdint.h>
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define V2TYPES

typedef int               sInt;
typedef unsigned int      sUInt;
typedef sInt              sBool;
typedef char              sChar;

typedef signed   char     sS8;
typedef signed   short    sS16;
typedef signed   long     sS32;
#ifdef _MSC_VER
typedef signed   __int64  sS64;
#else
typedef int64_t sS64;
#endif

typedef unsigned char     sU8;
typedef unsigned short    sU16;
typedef unsigned long     sU32;
#ifdef _MSC_VER
typedef unsigned __int64  sU64;
#else
typedef uint64_t sU64;
#endif

typedef float             sF32;
typedef double            sF64;

#define sTRUE             1
#define sFALSE            0

//
#ifdef _DEBUG
#if !defined(_MSC_VER) && !defined(__cdecl)
#define __cdecl
#endif
extern void __cdecl printf2(const char *format, ...);
#else
#define printf2
#endif

#ifdef __cplusplus
template<class T> inline T sMin(const T a, const T b) { return (a<b)?a:b;  }
template<class T> inline T sMax(const T a, const T b) { return (a>b)?a:b;  }
template<class T> inline T sClamp(const T x, const T min, const T max) { return sMax(min,sMin(max,x)); }
#endif

#endif
