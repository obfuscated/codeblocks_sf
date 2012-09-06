
#ifndef NATIVEPARSERTEST_H
#define NATIVEPARSERTEST_H

#include "nativeparser_base.h"

class NativeParserTest : public NativeParserBase
{
public:
    NativeParserTest();
    ~NativeParserTest();
    bool TestExpression(wxString& expression,
                        TokenTree * tree,
                        const TokenIdxSet& searchScope,
                        TokenIdxSet&   result);
};

#endif //NATIVEPARSERTEST_H
