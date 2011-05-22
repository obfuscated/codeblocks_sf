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
#include "parser/parser.h"
#include "insertclassmethoddlg.h"

BEGIN_EVENT_TABLE(InsertClassMethodDlg, wxScrollingDialog)
    EVT_LISTBOX(XRCID("lstClasses"), InsertClassMethodDlg::OnClassesChange)
    EVT_RADIOBOX(XRCID("rbCode"), InsertClassMethodDlg::OnCodeChange)
    EVT_CHECKBOX(XRCID("chkPrivate"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkProtected"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkPublic"), InsertClassMethodDlg::OnFilterChange)
END_EVENT_TABLE()

InsertClassMethodDlg::InsertClassMethodDlg(wxWindow* parent, Parser* parser, const wxString& filename) :
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

    for (int i = 0; i < (int)clb->GetCount(); ++i)
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
            array.Add(str + (m_Decl ? _T(";\n") : _T("\n{\n\n}\n\n")));
        }
    }

    return array;
} // end of GetCode

void InsertClassMethodDlg::FillClasses()
{
    if (!m_Parser || !m_Parser->Done())
        return;

    wxListBox* lb = XRCCTRL(*this, "lstClasses", wxListBox);
    lb->Freeze();
    lb->Clear();
    TokensTree* tree = m_Parser->GetTokens();
    for (size_t i = 0; i < tree->size(); ++i)
    {
        Token* token = tree->at(i);
        //Manager::Get()->GetLogManager()->DebugLog("m_Filename=%s, token=%s", m_Filename.c_str(), token->m_Filename.c_str());
        if (token && (token->m_TokenKind & (tkClass | tkTypedef))) //&&
            //token->m_Filename == UnixFilename(m_Filename))
            // TODO: check against file's pair too
            lb->Append(token->m_Name, token);
    }
    lb->Thaw();
    FillMethods();
}

void InsertClassMethodDlg::FillMethods()
{
    if (!m_Parser || !m_Parser->Done())
        return;

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
    DoFillMethodsFor(clb,
                    parentToken,
                    parentToken ? parentToken->m_Name + _T("::") : _T(""),
                    includePrivate,
                    includeProtected,
                    includePublic);
    clb->Thaw();
}

void InsertClassMethodDlg::DoFillMethodsFor(wxCheckListBox* clb,
                                            Token* parentToken,
                                            const wxString& ns,
                                            bool includePrivate,
                                            bool includeProtected,
                                            bool includePublic)
{
    if (!parentToken)
        return;
    TokensTree* tree = parentToken->GetTree();
    if (!tree)
        return;
    //Manager::Get()->GetLogManager()->DebugLog("Fill methods for %s", parentToken->m_DisplayName.c_str());

    // loop ascending the inheritance tree
    tree->RecalcInheritanceChain(parentToken);

    for (TokenIdxSet::iterator it = parentToken->m_Children.begin(); it != parentToken->m_Children.end(); ++it)
    {
        int idx = *it;
        Token* token = tree->at(idx);
        if (!token)
            continue;

        //Manager::Get()->GetLogManager()->DebugLog("Evaluating %s", token->m_DisplayName.c_str());
        bool valid = token->m_TokenKind & (tkFunction | tkConstructor | tkDestructor) &&
                ((includePrivate && token->m_Scope == tsPrivate) ||
                (includeProtected && token->m_Scope == tsProtected) ||
                (includePublic && token->m_Scope == tsPublic));
        if (valid)
        {
            //Manager::Get()->GetLogManager()->DebugLog("Adding %s", token->m_DisplayName.c_str());
            // BUG IN WXWIDGETS: wxCheckListBox::Append(string, data) crashes...
            //                   wxCheckListBox::Append(string) does not...
            wxString str;
            str << token->m_Type << _T(" ") << ns << token->m_Name << token->GetFormattedArgs();
            str.Replace(_T("&"), _T("&&"));
            if (clb->FindString(str) == wxNOT_FOUND)
                clb->Append(str);
        }
    }

    // inheritance
    for (TokenIdxSet::iterator it = parentToken->m_DirectAncestors.begin();it!=parentToken->m_DirectAncestors.end(); ++it)
    {
        int idx = *it;
        Token* token = tree->at(idx);
        if (!token)
            continue;
        DoFillMethodsFor(clb, token, ns, includePrivate, includeProtected, includePublic);
    }
} // end of DoFillMethodsFor

// events

void InsertClassMethodDlg::OnClassesChange(wxCommandEvent& /*event*/)
{
    FillMethods();
}

void InsertClassMethodDlg::OnCodeChange(wxCommandEvent& /*event*/)
{
    m_Decl = XRCCTRL(*this, "rbCode", wxRadioBox)->GetSelection() == 0;
}

void InsertClassMethodDlg::OnFilterChange(wxCommandEvent& /*event*/)
{
    FillMethods();
}
