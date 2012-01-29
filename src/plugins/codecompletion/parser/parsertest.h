/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTEST_H
#define PARSERTEST_H

class Token;

#include "parser.h"
#include "tokenstree.h"

class ParserTest
{
public:
    static ParserTest* Get()
    {
        if (!s_Inst.get())
            s_Inst.reset(new ParserTest);
        return s_Inst.get();
    }

    void        Init();
    ParserBase* GetParser()     { return m_pClient;     }
    TokensTree* GetTokensTree() { return m_pTokensTree; }

    bool     Start(const wxString& file);
    void     Clear();
    void     PrintTree();
    void     PrintList();
    wxString SerializeTree();

protected:
    ParserTest() : m_pClient(NULL), m_pTokensTree(NULL) { ; }
    virtual ~ParserTest()
    {
        if (m_pTokensTree) { delete m_pTokensTree; m_pTokensTree = NULL; }
        if (m_pClient)     { delete m_pClient;     m_pClient     = NULL; }
    }

    ParserTest(const ParserTest&) { ; }
    ParserTest& operator=(const ParserTest&) { return *this; }
    friend class std::auto_ptr<ParserTest>;
    static std::auto_ptr<ParserTest> s_Inst;

private:
    void PrintTokenTree(Token* token);

    ParserBase* m_pClient;
    TokensTree* m_pTokensTree;
};

#endif // PARSERTEST_H
