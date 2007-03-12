#if ( !defined (PREP_H) && defined(__cplusplus) )
#define PREP_H

#ifndef wxMAJOR_VERSION
#include <wx/version.h>
#endif


template <int major, int minor> struct wxMinimumVersion { enum { eval = (wxMAJOR_VERSION > major || (wxMAJOR_VERSION == major && wxMINOR_VERSION >= minor))}; };
template <int major, int minor> struct wxExactVersion { enum { eval = (wxMAJOR_VERSION == major && wxMINOR_VERSION == minor)}; };
template <bool b> struct CompileTimeAssertion{};
template<> struct CompileTimeAssertion<true> { static inline void Assert(){}; };

template <int major, int minor = 0, int revision = 0> struct Version { enum { eval = 1000*1000*major + 1000*minor + revision }; };

template <bool cond, class true_t, class false_t> struct TernaryCond { typedef true_t eval; };
template <class true_t, class false_t> struct TernaryCond<false, true_t, false_t> { typedef false_t eval; };

template <typename T> unsigned int array_size(const T& array) { enum {result = sizeof(array) / sizeof(array[0])}; return result; };

namespace platform
{
    enum identifier
    {
        platform_unknown,
        platform_windows,
        platform_linux,
        platform_freebsd,
        platform_netbsd,
        platform_openbsd,
        platform_macos
    };

    // unfortunately we still need to use the preprocessor here...
    #if ( wxUSE_UNICODE )
    const bool unicode = true;
    #else
    const bool unicode = false;
    #endif

    #if   defined ( __WIN32__ )
    const identifier id = platform_windows;
    #elif defined ( __WXMAC__ )
    const identifier id = platform_macos;
    #elif defined ( __linux__ )  || defined ( LINUX )
    const identifier id = platform_linux;
    #elif defined ( FREEBSD )    || defined ( __FREEBSD__ )
    const identifier id = platform_freebsd;
    #elif defined ( NETBSD )     || defined ( __NETBSD__ )
    const identifier id = platform_netbsd;
    #elif defined ( OPENBSD )    || defined ( __OPENBSD__ )
    const identifier id = platform_openbsd;
    #else
    const identifier id = platform_unknown;
    #endif

    const bool windows = (id == platform_windows);
    const bool macos   = (id == platform_macos);
    const bool CBlinux   = (id == platform_linux);
    const bool freebsd = (id == platform_freebsd);
    const bool netbsd  = (id == platform_netbsd);
    const bool openbsd = (id == platform_openbsd);
    const bool CBunix    = (CBlinux | freebsd | netbsd | openbsd);

    const int bits = 8*sizeof(void*);


    #if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2)
    const int gcc = Version<__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__>::eval;
    #else
    const int gcc = 0;
    #endif
};

namespace sdk
{
    const int version = Version<1>::eval;
    const int buildsystem_version = Version<1>::eval;
    const int plugin_api_version = Version<1,11,10>::eval;
};


#endif
