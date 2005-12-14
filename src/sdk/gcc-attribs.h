// Attributes and builtins used when compiling Code::Blocks with gcc
// No effect (short of being documentary) with other compilers
//
// $Id$

#ifndef GCC_ATTRIBS
#define GCC_ATTRIBS

#if __GNUC__ >= 3


#define likely(x)			__builtin_expect(!!(x),1)
#define unlikely(x)			__builtin_expect(!!(x),0)

#define __prefetch__(x)		__builtin_prefetch(x)

#define __inline__			__attribute__ ((always_inline))
#define __noinline__		__attribute__ ((noinline))
#define __nothrow__			__attribute__ ((nothrow))
#define __deprecated__		__attribute__ ((deprecated))
#define __const__			__attribute__ ((const))
#define __pure__			__attribute__ ((pure))
#define __check_result__	__attribute__ ((warn_unused_result))
#define __malloc__			__attribute__ ((malloc))
#define __constant__(x)		__builtin_constant_p(x)

#ifdef __WIN32__
 #ifdef __fastcall__
  #undef __fastcall__
 #endif
 #define __fastcall__		__attribute__ ((fastcall))
 #define __fastestcall__	__attribute__ ((regparm(3)))
#endif

#else

#define likely(x)
#define unlikely(x)
#define __prefetch__(x)
#define __inline__
#define __noinline__
#define __fastcall__
#define __fastestcall__
#define __pure__
#define __nothrow__
#define __deprecated__
#define __const__
#define __pure__
#define __check_result__
#define __malloc__
#define __constant__(x) false

#endif

#endif
