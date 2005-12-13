// Attributes and builtins used when compiling Code::Blocks with gcc
// No effect (short of being documentary) with other compilers
//
// $Id$

#ifndef GCC_ATTRIBS
#define GCC_ATTRIBS

#ifdef __GNUC__

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

#define PREFETCH(x) __builtin_prefetch(x)

#define ALWAYS_INLINE __attribute__ ((always_inline));
#define NOTHROW __attribute__ ((nothrow))
#define DEPRECATED __attribute__ ((deprecated))

#ifdef __WIN32__
 #ifdef FASTCALL
  #undef FASTCALL
 #endif
 #define FASTCALL __attribute__ ((fastcall))
#endif

#else

#define likely(x)
#define unlikely(x)
#define PREFETCH(x)
#define ALWAYS_INLINE
#define FASTCALL
#define PURE
#define NOTHROW
#define DEPRECATED

#endif
#endif
