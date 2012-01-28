/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTEST_H
#define PARSERTEST_H

class TokensTree;
class Token;

class ParserTest
{
public:
     ParserTest();
    ~ParserTest();

    bool     Start(const wxString& file);
    void     Clear();
    void     PrintTree();
    void     PrintList();
    wxString SerializeTree();

private:
    void PrintTokenTree(Token* token);

private:
    TokensTree* m_pTokensTree;
};

#endif // PARSERTEST_H
