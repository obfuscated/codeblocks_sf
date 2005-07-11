#ifndef DOCKIT_DEFS_H
#define DOCKIT_DEFS_H

/*
 * If we're using wx in Dynamic Library format do we
 * want wxDockIt to be in DLL form as well?
 */
#if defined(WXUSINGDLL) && \
    (defined(WXMAKING_DOCKIT_DLL) || defined(WXUSING_DOCKIT_DLL))

#if defined(WXMAKING_DOCKIT_DLL)
    // When building the DLL WXDOCKIT_DECLSPEC exports classes
#   define WXDOCKIT_DECLSPEC            WXEXPORT
#elif defined(WXUSING_DOCKIT_DLL)
    // When using the DLL WXDOCKIT_DECLSPEC imports classes
#   define WXDOCKIT_DECLSPEC            WXIMPORT
#endif // defined(WXBUILD_DOCKIT_DLL)

#else
// When building the static library nullify the effect of WXDOCKIT_DECLSPEC
#define WXDOCKIT_DECLSPEC
#endif // WXUSINGDLL && (WXMAKING_DOCKIT_DLL || WXUSING_DOCKIT_DLL)

#endif // DOCKIT_DEFS_H
