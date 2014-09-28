#include <sdk.h>

#ifndef CB_PRECOMP
#endif

#include "nativeparser_test.h"

#include "parser/cclogger.h"

#include <wx/textfile.h>
#include <wx/tokenzr.h>

#define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_NATIVEPARSERTEST_DEBUG_OUTPUT
        #define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_NATIVEPARSERTEST_DEBUG_OUTPUT
        #define CC_NATIVEPARSERTEST_DEBUG_OUTPUT 2
    #endif
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

bool s_DebugSmartSense = false; // if true, then cclogger will log many debug messages

namespace CCTestAppGlobal
{
    extern wxArrayString s_includeDirs;
    extern wxArrayString s_fileQueue;
    extern wxArrayString s_filesParsed;
}// CCTestAppGlobal

NativeParserTest::NativeParserTest()
{
}

NativeParserTest::~NativeParserTest()
{
}

bool NativeParserTest::TestExpression(wxString&          expression,
                                      const TokenIdxSet& searchScope,
                                      TokenIdxSet&       result)
{
    // find all other matches
    std::queue<ParserComponent> components;
    BreakUpComponents(expression, components);

    ResolveExpression(m_Parser.GetTokenTree(), components, searchScope, result, true, true /*isPrefix*/);

    if (s_DebugSmartSense)
        CCLogger::Get()->DebugLog(F(_T("NativeParserTest::TestExpression, returned %lu results"), static_cast<unsigned long>(result.size())));

    return true;
}

bool NativeParserTest::Parse(wxString & filename)
{
    return m_Parser.Reparse(filename, true);
}

void NativeParserTest::PrintList()
{
    const TokenList* tokens = m_Parser.GetTokenTree()->GetTokens();
    for (TokenList::const_iterator it=tokens->begin(); it!=tokens->end(); ++it)
    {
        Token* token = (*it);
        if (token)
        {
            wxString log;
            log << token->GetTokenKindString() << _T(" ")
                << token->DisplayName()        << _T("\t[")
                << token->m_Line               << _T(",")
                << token->m_ImplLine           << _T("]");
            CCLogger::Get()->Log(log);
        }
    }
}

wxString NativeParserTest::SerializeTree()
{
    return m_Parser.GetTokenTree()->m_Tree.Serialize();
    //return wxEmptyString;
}

void NativeParserTest::PrintTokenTree(Token* token)
{
    if (!token)
        return;

    wxString log;
    if (!token->m_Children.empty()) log << _T("+");
    if (token->m_TokenKind == tkFunction)
        log << token->m_Name << token->m_Args << _T("\t");
    else
        log << token->DisplayName() << _T("\t");
    log << _T("[") << token->m_Line << _T(",") << token->m_ImplLine << _T("]");
    CCLogger::Get()->Log(log);

    TokenIdxSet& ids = token->m_Children;
    for (TokenIdxSet::iterator it=ids.begin(); it!=ids.end(); ++it)
    {
        Token* token_ex = m_Parser.GetTokenTree()->at(*it);
        PrintTokenTree(token_ex); // recursion
    }
}

void NativeParserTest::PrintTree()
{
    const TokenList *tokens = m_Parser.GetTokenTree()->GetTokens();
    for (TokenList::const_iterator it=tokens->begin(); it!=tokens->end(); ++it)
    {
        Token* token = (*it);
        if (token)
        {
          Token* parent = m_Parser.GetTokenTree()->at(token->m_ParentIndex);
          if (!parent)
              PrintTokenTree(token);
        }
    }
}

void NativeParserTest::Clear()
{
    m_Parser.GetTokenTree()->clear();
}

void NativeParserTest::Init()
{
    // Keep this in sync with CodeCompletion::LoadTokenReplacements()

    // for GCC
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD"),                     _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_STD_D"),                   _T("std"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE"),  _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NESTED_NAMESPACE"),    _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE"),         _T("+namespace std {"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE"),           _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE_TR1"),       _T("}"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE_TR1"),     _T("namespace tr1 {"));

    // for GCC 4.6.x
    Tokenizer::SetReplacementString(_T("_GLIBCXX_VISIBILITY"),              _T("+"));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE_VERSION"), _T(""));
    Tokenizer::SetReplacementString(_T("_GLIBCXX_END_NAMESPACE_VERSION"),   _T(""));

    // for VC
    Tokenizer::SetReplacementString(_T("_STD_BEGIN"),                       _T("namespace std {"));
    Tokenizer::SetReplacementString(_T("_STD_END"),                         _T("}"));
    Tokenizer::SetReplacementString(_T("_STDEXT_BEGIN"),                    _T("namespace std {"));
    Tokenizer::SetReplacementString(_T("_STDEXT_END"),                      _T("}"));

    // for wxWidgets
    Tokenizer::SetReplacementString(_T("BEGIN_EVENT_TABLE"),                _T("-END_EVENT_TABLE"));
    Tokenizer::SetReplacementString(_T("WXDLLEXPORT"),                      _T(""));
    Tokenizer::SetReplacementString(_T("WXDLLIMPORT"),                      _T(""));
    Tokenizer::SetReplacementString(_T("WXEXPORT"),                         _T(""));
    Tokenizer::SetReplacementString(_T("WXIMPORT"),                         _T(""));


    // initialize the include files
    // second, try taking include directories into account
    for (size_t i=0; i<CCTestAppGlobal::s_includeDirs.GetCount(); i++)
    {
        wxString include_dir = CCTestAppGlobal::s_includeDirs.Item(i);
        m_Parser.AddIncludeDir(include_dir);
    }
}

bool NativeParserTest::TestParseAndCodeCompletion(wxString filename)
{
    Clear();//clear the tree
    bool parseResult = false;
    parseResult = Parse(filename);
    if(!parseResult)
        return false;
    int passCount = 0;
    int failCount = 0;

    wxString testResult;
    wxString message = wxString::Format(_T("********************************************************\n  Testing in file: %s\n********************************************************"),filename.wx_str());
    wxLogMessage(message);
    testResult<<message<<wxT("\n");


    // read the test cases of CodeCompletion test
    wxTextFile source;
    source.Open(filename);
    wxString str;

    for ( str = source.GetLastLine();
          source.GetCurrentLine() > 0;
          str = source.GetPrevLine() )
    {

        // a test case should be put in a line, and start with the double slash
        if (str.StartsWith(_T("//")))
        {
            // do tests here, example of line is below
            // tc.St    //StaticVoid
            // remove the beginning "//"
            str.Remove(0,2);

            // find the second "//", the string after the second double slash are the
            // the result should be listed
            wxString expression;
            wxString match;
            int pos = str.Find(_T("//"));
            if (pos == wxNOT_FOUND)
                break;
            expression = str.Mid(0,pos);
            match = str.Mid(pos+2);// the remaining string

            expression.Trim();
            expression.Trim(true);
            match.Trim();
            match.Trim(true);

            wxArrayString suggestList;
            // the match can have many items, like: AAA,BBBB
            wxStringTokenizer tkz(match, wxT(","));
            while ( tkz.HasMoreTokens() )
            {
                wxString token = tkz.GetNextToken();
                suggestList.Add(token);
            }

            TokenIdxSet searchScope;
            searchScope.insert(-1);
            TokenIdxSet result;
            TestExpression(expression,searchScope,result);

            // loop the suggestList to see it is in the result Tokens
            for (size_t i=0;i<suggestList.GetCount();i++)
            {
                wxString element = suggestList[i];
                bool pass = false; // pass the test?
                for (TokenIdxSet::const_iterator it = result.begin();
                     it != result.end();
                     ++it)
                {
                    const Token* token = m_Parser.GetTokenTree()->at(*it);
                    if (!token || token->m_Name.IsEmpty())
                        continue;
                    if (element.IsSameAs(token->m_Name))
                    {
                        message = wxString::Format(_T("-PASS: %s  %s"),expression.wx_str(),element.wx_str());
                        testResult<<message<<wxT("\n");
                        wxLogMessage(message);
                        pass = true;
                        passCount++;
                    }

                }
                if (pass == false)
                {
                    message = wxString::Format(_T("*FAIL: %s  %s"),expression.wx_str(),element.wx_str());
                    testResult<<message<<wxT("\n");
                    wxLogMessage(message);
                    failCount++;
                }
            }
            // wxLogMessage(_T("Result have %lu matches"), static_cast<unsigned long>(result.size()));
        }
        else
            break; // if the line is not started with //, then we just stop testing
    }

    // report the test result here again in the last stage, further more, we can show this in another text control

    wxLogMessage(wxT("--------------------------------------------------------\nTotal %d tests, %d PASS, %d FAIL\n--------------------------------------------------------"), passCount+failCount, passCount, failCount);

    return true;
}

void NativeParserTest::BatchTest(wxString file)
{
    TestParseAndCodeCompletion(file);
}


