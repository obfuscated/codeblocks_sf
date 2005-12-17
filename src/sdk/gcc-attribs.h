// Attributes and builtins used when compiling Code::Blocks with gcc
// No effect (short of being documentary) with other compilers
//
// $Id$

#ifndef GCC_ATTRIBS
#define GCC_ATTRIBS

#if __GNUC__ >= 3	// ----------------------------------

#define likely(x)				__builtin_expect(!!(x),1)
#define unlikely(x)				__builtin_expect(!!(x),0)
#define compiler_constant(x)	__builtin_constant_p(x)
#define cpu_prefetch(x)			__builtin_prefetch(x)

#define ___inline___			__attribute__ ((always_inline))
#define ___noinline___			__attribute__ ((noinline))
#define ___nothrow___			__attribute__ ((nothrow))
#define ___deprecated___		__attribute__ ((deprecated))
#define ___const___				__attribute__ ((const))
#define ___pure___				__attribute__ ((pure))
#define ___checkresult___		__attribute__ ((warn_unused_result))
#define ___malloc___			__attribute__ ((malloc))

#ifdef __WIN32__
 #define ___fastcall___			__attribute__ ((fastcall))
 #define ___fastestcall___		__attribute__ ((regparm(3)))
#else
 #define ___fastcall___
 #define ___fastcall___
#endif

#else				// ----------------------------------

#define likely(x)				(x)
#define unlikely(x)				(x)
#define compiler_constant(x)	false
#define cpu_prefetch(x)

#define ___inline___			inline
#define ___noinline___
#define ___nothrow___
#define ___deprecated___
#define ___const___
#define ___pure___
#define ___checkresult___
#define ___malloc___

#define ___fastcall___
#define ___fastcall___


#endif				// ----------------------------------


#endif
