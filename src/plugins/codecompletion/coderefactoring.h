/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CODEREFACTORING_H
#define CODEREFACTORING_H

#include "nativeparser.h"

struct crSearchData
{
    int pos;
    int line;
    wxString text;
    crSearchData(int pos_in, int line_in, const wxString& text_in) :
        pos(pos_in),
        line(line_in),
        text(text_in)
    {}
};

class CodeRefactoring
{
    typedef std::list<crSearchData> SearchDataList;
    typedef std::map<wxString, SearchDataList> SearchDataMap;

public:
    CodeRefactoring(NativeParser& np);
    virtual ~CodeRefactoring();

    void FindReferences();
    void RenameSymbols();

private:
    size_t SearchInFiles(const wxArrayString& files, const wxString& targetText);
    size_t VerifyResult(const TokenIdxSet& targetResult, const wxString& targetText, bool isLocalVariable);
    void Find(cbStyledTextCtrl* control, const wxString& file, const wxString& target);
    wxString GetSymbolUnderCursor();

    void DoFindReferences();
    void DoRenameSymbols(const wxString& targetText, const wxString& replaceText);

private:
    void GetAllProjectFiles(wxArrayString& files, cbProject* project);
    void GetOpenedFiles(wxArrayString& files);
    bool Parse();

private:
    NativeParser& m_NativeParser;
    SearchDataMap m_SearchDataMap;
};

#endif // CODEREFACTORING_H
