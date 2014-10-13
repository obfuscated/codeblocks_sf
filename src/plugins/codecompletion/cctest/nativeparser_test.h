
#ifndef NATIVEPARSERTEST_H
#define NATIVEPARSERTEST_H

#include "parser.h"
#include "nativeparser_base.h"

class NativeParserTest : public NativeParserBase
{
public:
    NativeParserTest();
    ~NativeParserTest();
    bool TestExpression(wxString&          expression,
                        const TokenIdxSet& searchScope,
                        TokenIdxSet&       result);
    bool Parse(wxString& file, bool isLocalFile);
    ParserBase m_Parser;

    void PrintList();

    wxString SerializeTree();

    void PrintTokenTree(Token* token);

    void PrintTree();

    void Clear();

    void Init();

    bool TestParseAndCodeCompletion(wxString filename, bool isLocalFile = true);

    // run test on the file
    void BatchTest(wxString file);
};

#endif //NATIVEPARSERTEST_H
