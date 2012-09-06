#include <sdk.h>

#ifndef CB_PRECOMP
#endif

#include "nativeparser_test.h"

#include "parser/cclogger.h"

#define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_NATIVEPARSERTEST_DEBUG_OUTPUT
    #define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_NATIVEPARSERTEST_DEBUG_OUTPUT
    #define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 2
#endif

#ifdef CC_PARSER_TEST
    #define ADDTOKEN(format, args...) \
            wxLogMessage(F(format, ##args))
    #define TRACE(format, args...) \
            wxLogMessage(F(format, ##args))
    #define TRACE2(format, args...) \
            wxLogMessage(F(format, ##args))
#else
    #if CC_NATIVEPARSERTEST_DEBUG_OUTPUT == 1
        #define ADDTOKEN(format, args...) \
                CCLogger::Get()->AddToken(F(format, ##args))
        #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2(format, args...)
    #elif CC_NATIVEPARSERTEST_DEBUG_OUTPUT == 2
        #define ADDTOKEN(format, args...) \
                CCLogger::Get()->AddToken(F(format, ##args))
        #define TRACE(format, args...)                                              \
            do                                                                      \
            {                                                                       \
                if (g_EnableDebugTrace)                                             \
                    CCLogger::Get()->DebugLog(F(format, ##args));                   \
            }                                                                       \
            while (false)
        #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #else
        #define ADDTOKEN(format, args...)
        #define TRACE(format, args...)
        #define TRACE2(format, args...)
    #endif
#endif

extern bool s_DebugSmartSense;

NativeParserTest::NativeParserTest( )
{
}

NativeParserTest::~NativeParserTest()
{
}

bool NativeParserTest::TestExpression(wxString&          expression,
                                      TokenTree*         tree,
                                      const TokenIdxSet& searchScope,
                                      TokenIdxSet&       result)
{
    // find all other matches
    std::queue<ParserComponent> components;
    BreakUpComponents(expression, components);

    ResolveExpression(tree, components, searchScope, result, true, false);

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("NativeParserTest::TestExpression, returned %lu results"), static_cast<unsigned long>(result.size())));

    return true;
}
