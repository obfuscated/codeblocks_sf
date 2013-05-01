/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/checkbox.h>
    #include <wx/checklst.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/radiobox.h>
    #include <wx/xrc/xmlres.h>
    #include "globals.h"
    #include "manager.h"
    #include "logmanager.h"
#endif

#include "insertclassmethoddlg.h"

#include "parser/parser.h"

namespace InsertClassMethodDlgHelper
{
    inline void DoFillMethodsFor(wxCheckListBox* clb, Token* parentToken, const wxString& ns, bool includePrivate,
                                 bool includeProtected, bool includePublic)
    {
        if (!parentToken)
            return;
        TokenTree* tree = parentToken->GetTree();
        if (!tree)
            return;

        // loop ascending the inheritance tree
        tree->RecalcInheritanceChain(parentToken);

        for (TokenIdxSet::const_iterator it = parentToken->m_Children.begin(); it != parentToken->m_Children.end(); ++it)
        {
            int idx = *it;
            const Token* token = tree->at(idx);
            if (!token)
                continue;

            const bool valid =    token->m_TokenKind & (tkFunction | tkConstructor | tkDestructor)
                               && (   (includePrivate && token->m_Scope == tsPrivate)
                                   || (includeProtected && token->m_Scope == tsProtected)
                                   || (includePublic && token->m_Scope == tsPublic) );
            if (valid)
            {
                wxString str;
                str << token->m_FullType << _T(" ") << ns << token->m_Name << token->GetFormattedArgs();
                str.Replace(_T("&"), _T("&&"));
                if (clb->FindString(str) == wxNOT_FOUND)
                    clb->Append(str);
            }
        }

        // inheritance
        for (TokenIdxSet::const_iterator it = parentToken->m_DirectAncestors.begin();
             it != parentToken->m_DirectAncestors.end();
             ++it)
        {
            int idx = *it;
            Token* token = tree->at(idx);
            if (!token)
                continue;
            InsertClassMethodDlgHelper::DoFillMethodsFor(
                clb,
                token,
                ns,
                includePrivate,
                includeProtected,
                includePublic);
        }
    }
}// namespace InsertClassMethodDlgHelper

BEGIN_EVENT_TABLE(InsertClassMethodDlg, wxScrollingDialog)
    EVT_LISTBOX(XRCID("lstClasses"), InsertClassMethodDlg::OnClassesChange)
    EVT_RADIOBOX(XRCID("rbCode"), InsertClassMethodDlg::OnCodeChange)
    EVT_CHECKBOX(XRCID("chkPrivate"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkProtected"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkPublic"), InsertClassMethodDlg::OnFilterChange)
END_EVENT_TABLE()

InsertClassMethodDlg::InsertClassMethodDlg(wxWindow* parent, ParserBase* parser, const wxString& filename) :
    m_Parser(parser),
    m_Decl(true),
    m_Filename(filename)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgInsertClassMethod"),_T("wxScrollingDialog"));
    XRCCTRL(*this, "rbCode", wxRadioBox)->SetSelection(0);
    FillClasses();
}

InsertClassMethodDlg::~InsertClassMethodDlg()
{
    //dtor
}

wxArrayString InsertClassMethodDlg::GetCode() const
{
    wxArrayString array;
    const wxCheckListBox* clb = XRCCTRL(*this, "chklstMethods", wxCheckListBox);

    for (unsigned int i = 0; i < clb->GetCount(); ++i)
    {
        if (clb->IsChecked(i))
        {
            wxString str;
            if (XRCCTRL(*this, "chkAddDoc", wxCheckBox)->IsChecked())
            {
                // add doc block
                str << _T("/** @brief (one liner)\n  *\n  * (documentation goes here)\n  */\n");
            }
            str << clb->GetString(i);
            str.Replace(_T("&&"), _T("&"));
            array.Add(str + (m_Decl ? _T(";\n") : _T("\n{\n\t\n}\n\n")));
        }
    }

    return array;
} // end of GetCode

void InsertClassMethodDlg::FillClasses()
{
    wxListBox* lb = XRCCTRL(*this, "lstClasses", wxListBox);
    lb->Freeze();
    lb->Clear();

    TokenTree* tree = m_Parser->GetTokenTree();
    for (size_t i = 0; i < tree->size(); ++i)
    {
        Token* token = tree->at(i);
        //CCLogger::Get()->DebugLog(wxT("m_Filename=%s, token=%s"), m_Filename.wx_str(), token->m_Filename.wx_str());
        if (token && (token->m_TokenKind & (tkClass | tkTypedef)))
        {
            // TODO: check against file's pair too
            lb->Append(token->m_Name, token);
        }
    }

    lb->Thaw();
    FillMethods();
}

void InsertClassMethodDlg::FillMethods()
{
    wxListBox* lb = XRCCTRL(*this, "lstClasses", wxListBox);
    wxCheckListBox* clb = XRCCTRL(*this, "chklstMethods", wxCheckListBox);
    clb->Clear();

    if (lb->GetSelection() == -1)
        return;

    bool includePrivate = XRCCTRL(*this, "chkPrivate", wxCheckBox)->IsChecked();
    bool includeProtected = XRCCTRL(*this, "chkProtected", wxCheckBox)->IsChecked();
    bool includePublic = XRCCTRL(*this, "chkPublic", wxCheckBox)->IsChecked();

    Token* parentToken = reinterpret_cast<Token*>(lb->GetClientData(lb->GetSelection()));

    clb->Freeze();
    InsertClassMethodDlgHelper::DoFillMethodsFor(
        clb,
        parentToken,
        parentToken ? parentToken->m_Name + _T("::") : _T(""),
        includePrivate,
        includeProtected,
        includePublic);
    clb->Thaw();
}

// events
void InsertClassMethodDlg::OnClassesChange(cb_unused wxCommandEvent& event)
{
    FillMethods();
}

void InsertClassMethodDlg::OnCodeChange(cb_unused wxCommandEvent& event)
{
    m_Decl = XRCCTRL(*this, "rbCode", wxRadioBox)->GetSelection() == 0;
}

void InsertClassMethodDlg::OnFilterChange(cb_unused wxCommandEvent& event)
{
    FillMethods();
}
