/* $Id$ */
/*
 * Copyright (C) 2017 Oracle Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* In builds inside of the VirtualBox source tree we override the default
 * VBoxVideoIPRT.h using -include, therefore this define must match the one
 * there. */
#ifndef ___VBox_Graphics_VBoxVideoIPRT_h
#define ___VBox_Graphics_VBoxVideoIPRT_h

# include "VBoxVideoErr.h"

#ifndef __cplusplus
typedef enum
{
    false = 0,
    true
} bool;
# define RT_C_DECLS_BEGIN
# define RT_C_DECLS_END
#else
# define RT_C_DECLS_BEGIN extern "C" {
# define RT_C_DECLS_END }
#endif

#if defined(IN_XF86_MODULE) && !defined(NO_ANSIC)
# ifdef __cplusplus
/* xf86Module.h redefines this. */
#  define NULL 0
# endif
RT_C_DECLS_BEGIN
# include "xf86_ansic.h"
RT_C_DECLS_END
#endif  /* defined(IN_XF86_MODULE) && !defined(NO_ANSIC) */
#define __STDC_LIMIT_MACROS  /* define *INT*_MAX on C++ too. */
#include "compiler.h"  /* Pulls in <sdtint.h>.  Must come after xf86_ansic.h on XFree86. */
#include <X11/Xfuncproto.h>
#if defined(IN_XF86_MODULE) && !defined(NO_ANSIC)
/* XFree86 did not have these.  Not that I care much for micro-optimisations
 * in most cases anyway. */
# define _X_LIKELY(x) (x)
# define _X_UNLIKELY(x) (x)
# ifndef offsetof
#  define offsetof(type, member) ( (int)(uintptr_t)&( ((type *)(void *)0)->member) )
# endif
# if !defined(_INT8_T_DECLARED)   && !defined(_INT8_T)   && !defined(__INT8_TYPE__)   && !defined(INT8_MAX)
typedef signed char int8_t;
# endif
# if !defined(_UINT8_T_DECLARED)   && !defined(_UINT8_T)   && !defined(__UINT8_TYPE__)   && !defined(UINT8_MAX)
typedef unsigned char uint8_t;
# endif
# if !defined(_INT16_T_DECLARED)   && !defined(_INT16_T)   && !defined(__INT16_TYPE__)   && !defined(INT16_MAX)
typedef signed short int16_t;
# endif
# if !defined(_UINT16_T_DECLARED)   && !defined(_UINT16_T)   && !defined(__UINT16_TYPE__)   && !defined(UINT16_MAX)
typedef unsigned short uint16_t;
# endif
# if !defined(_INT32_T_DECLARED)   && !defined(_INT32_T)   && !defined(__INT32_TYPE__)   && !defined(INT32_MAX)
typedef signed int int32_t;
# endif
# if !defined(_UINT32_T_DECLARED)   && !defined(_UINT32_T)   && !defined(__UINT32_TYPE__)   && !defined(UINT32_MAX)
typedef unsigned int uint32_t;
# endif
# if !defined(_INT64_T_DECLARED)   && !defined(_INT64_T)   && !defined(__INT64_TYPE__)   && !defined(INT64_MAX)
typedef signed long long int64_t;
# endif
# if !defined(_UINT64_T_DECLARED)   && !defined(_UINT64_T)   && !defined(__UINT64_TYPE__)   && !defined(UINT64_MAX)
typedef unsigned long long uint64_t;
# endif
# ifndef _XSERVER64
#  if !defined(_INTPTR_T_DECLARED)  && !defined(_INTPTR_T)  && !defined(__INTPTR_TYPE__)   && !defined(INTPTR_MAX)
typedef signed long         intptr_t;
#  endif
#  if !defined(_UINTPTR_T_DECLARED)  && !defined(_UINTPTR_T)  && !defined(__UINTPTR_TYPE__)   && !defined(UINTPTR_MAX)
typedef unsigned long       uintptr_t;
#  endif
# else
#  if !defined(_INTPTR_T_DECLARED)  && !defined(_INTPTR_T)  && !defined(__INTPTR_TYPE__)   && !defined(INTPTR_MAX)
typedef int64_t             intptr_t;
#  endif
#  if !defined(_UINTPTR_T_DECLARED)  && !defined(_UINTPTR_T)  && !defined(__UINTPTR_TYPE__)   && !defined(UINTPTR_MAX)
typedef uint64_t            uintptr_t;
#  endif
# endif
#else  /* !(defined(IN_XF86_MODULE) && !defined(NO_ANSIC)) */
# include <stdarg.h>
# include <stddef.h>
# include <stdint.h>
# include <string.h>
#endif  /* !(defined(IN_XF86_MODULE) && !defined(NO_ANSIC)) */

RT_C_DECLS_BEGIN
extern int RTASSERTVAR[1];
RT_C_DECLS_END

#define AssertCompile(expr) \
    extern int RTASSERTVAR[1] __attribute__((__unused__)), \
    RTASSERTVAR[(expr) ? 1 : 0] __attribute__((__unused__))
#define AssertCompileSize(type, size) \
    AssertCompile(sizeof(type) == (size))
#define AssertPtrNullReturnVoid(a) do { } while(0)

#if !defined(IN_XF86_MODULE) && defined(DEBUG)
# include <assert.h>
# define Assert assert
# define AssertFailed() assert(0)
# define AssertMsg(expr, msg) \
  do { \
      if (!(expr)) xf86ErrorF msg; \
      assert((expr)); \
  } while (0)
# define AssertPtr assert
# define AssertRC(expr) assert (!expr)
#else
# define Assert(expr) do { } while(0)
# define AssertFailed() do { } while(0)
# define AssertMsg(expr, msg) do { } while(0)
# define AssertPtr(expr) do { } while(0)
# define AssertRC(expr) do { } while(0)
#endif

#define DECLCALLBACK(type) type
#define DECLCALLBACKMEMBER(type, name) type (* name)
#if __GNUC__ >= 4
# define DECLHIDDEN(type) __attribute__((visibility("hidden"))) type
#else
# define DECLHIDDEN(type) type
#endif
#define DECLINLINE(type) static __inline__ type

#define _1K 1024
#define ASMCompilerBarrier mem_barrier
#define RT_BIT(bit)                             ( 1U << (bit) )
#define RT_BOOL(Value)                          ( !!(Value) )
#define RT_BZERO(pv, cb)    do { memset((pv), 0, cb); } while (0)
#define RT_CLAMP(Value, Min, Max)               ( (Value) > (Max) ? (Max) : (Value) < (Min) ? (Min) : (Value) )
#define RT_ELEMENTS(aArray)                     ( sizeof(aArray) / sizeof((aArray)[0]) )
#define RTIOPORT unsigned short
#define RT_NOREF(...)       (void)(__VA_ARGS__)
#define RT_OFFSETOF(type, member) offsetof(type, member)
#define RT_ZERO(Obj)        RT_BZERO(&(Obj), sizeof(Obj))
#define VALID_PTR(ptr)    (   (uintptr_t)(ptr) + 0x1000U >= 0x2000U )
#ifndef INT16_C
# define INT16_C(Value) (Value)
#endif
#ifndef UINT16_C
# define UINT16_C(Value) (Value)
#endif
#ifndef INT32_C
# define INT32_C(Value) (Value ## U)
#endif
#ifndef UINT32_C
# define UINT32_C(Value) (Value ## U)
#endif
#ifndef INT16_MAX
# define INT16_MAX INT16_C(0x7fff)
#endif
#ifndef UINT16_MAX
# define UINT16_MAX UINT16_C(0xffff)
#endif
#ifndef INT32_MAX
# define INT32_MAX INT32_C(0x7fffffff)
#endif
#ifndef UINT32_MAX
# define UINT32_MAX UINT32_C(0xffffffff)
#endif

#define likely _X_LIKELY
#define unlikely _X_UNLIKELY

/**
 * A point in a two dimentional coordinate system.
 */
typedef struct RTPOINT
{
    /** X coordinate. */
    int32_t     x;
    /** Y coordinate. */
    int32_t     y;
} RTPOINT;

/**
 * Rectangle data type, double point.
 */
typedef struct RTRECT
{
    /** left X coordinate. */
    int32_t     xLeft;
    /** top Y coordinate. */
    int32_t     yTop;
    /** right X coordinate. (exclusive) */
    int32_t     xRight;
    /** bottom Y coordinate. (exclusive) */
    int32_t     yBottom;
} RTRECT;

/**
 * Rectangle data type, point + size.
 */
typedef struct RTRECT2
{
    /** X coordinate.
     * Unless stated otherwise, this is the top left corner. */
    int32_t     x;
    /** Y coordinate.
     * Unless stated otherwise, this is the top left corner.  */
    int32_t     y;
    /** The width.
     * Unless stated otherwise, this is to the right of (x,y) and will not
     * be a negative number. */
    int32_t     cx;
    /** The height.
     * Unless stated otherwise, this is down from (x,y) and will not be a
     * negative number. */
    int32_t     cy;
} RTRECT2;

/**
 * The size of a rectangle.
 */
typedef struct RTRECTSIZE
{
    /** The width (along the x-axis). */
    uint32_t    cx;
    /** The height (along the y-axis). */
    uint32_t    cy;
} RTRECTSIZE;

/** @name Port I/O helpers
 * @{ */

/** Write an 8-bit value to an I/O port. */
#define VBVO_PORT_WRITE_U8(Port, Value) \
    outb(Port, Value)
/** Write a 16-bit value to an I/O port. */
#define VBVO_PORT_WRITE_U16(Port, Value) \
    outw(Port, Value)
/** Write a 32-bit value to an I/O port. */
#define VBVO_PORT_WRITE_U32(Port, Value) \
    outl(Port, Value)
/** Read an 8-bit value from an I/O port. */
#define VBVO_PORT_READ_U8(Port) \
    inb(Port)
/** Read a 16-bit value from an I/O port. */
#define VBVO_PORT_READ_U16(Port) \
    inw(Port)
/** Read a 32-bit value from an I/O port. */
#define VBVO_PORT_READ_U32(Port) \
    inl(Port)

/** @}  */

#endif
