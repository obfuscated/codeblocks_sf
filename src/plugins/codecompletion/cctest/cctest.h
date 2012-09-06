/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCTEST_H
#define CCTEST_H

class Token;

#include "parser.h"
#include "tokenstree.h"

class CCTest
{
public:
    static CCTest* Get()
    {
        if (!s_Inst.get())
            s_Inst.reset(new CCTest);
        return s_Inst.get();
    }

    void        Init();
    ParserBase* GetParser()    { return m_pClient;    }
    TokenTree*  GetTokenTree() { return m_pTokenTree; }

    bool     Start(const wxString& file);
    void     Clear();
    void     PrintTree();
    void     PrintList();
    wxString SerializeTree();

protected:
    CCTest() : m_pClient(NULL), m_pTokenTree(NULL) { ; }
    virtual ~CCTest()
    {
        if (m_pTokenTree)  { delete m_pTokenTree;  m_pTokenTree = NULL; }
        if (m_pClient)     { delete m_pClient;     m_pClient    = NULL; }
    }

    CCTest(const CCTest&) { ; }
    CCTest& operator=(const CCTest&) { return *this; }
    friend class std::auto_ptr<CCTest>;
    static std::auto_ptr<CCTest> s_Inst;

private:
    void PrintTokenTree(Token* token);

    ParserBase* m_pClient;
    TokenTree*  m_pTokenTree;
};

#endif // CCTEST_H
