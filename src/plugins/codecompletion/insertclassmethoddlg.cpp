#include <sdk.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/button.h>

#include "manager.h"
#include "messagemanager.h"
#include "globals.h"
#include "insertclassmethoddlg.h"

BEGIN_EVENT_TABLE(InsertClassMethodDlg, wxDialog)
    EVT_LISTBOX(XRCID("lstClasses"), InsertClassMethodDlg::OnClassesChange)
    EVT_RADIOBOX(XRCID("rbCode"), InsertClassMethodDlg::OnCodeChange)
    EVT_CHECKBOX(XRCID("chkPrivate"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkProtected"), InsertClassMethodDlg::OnFilterChange)
    EVT_CHECKBOX(XRCID("chkPublic"), InsertClassMethodDlg::OnFilterChange)
END_EVENT_TABLE()

InsertClassMethodDlg::InsertClassMethodDlg(wxWindow* parent, Parser* parser, const wxString& filename)
    : m_pParser(parser),
    m_Decl(true),
    m_Filename(filename)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgInsertClassMethod"));
    XRCCTRL(*this, "rbCode", wxRadioBox)->SetSelection(0);
    FillClasses();
}

InsertClassMethodDlg::~InsertClassMethodDlg()
{
	//dtor
}

wxArrayString InsertClassMethodDlg::GetCode()
{
    wxArrayString array;
    wxCheckListBox* clb = XRCCTRL(*this, "chklstMethods", wxCheckListBox);

    for (int i = 0; i < clb->GetCount(); ++i)
    {
        if (clb->IsChecked(i))
        {
            wxString str;
            if (XRCCTRL(*this, "chkAddDoc", wxCheckBox)->IsChecked())
            {
                // add doc block
                str << _("/** @brief (one liner)\n  *\n  * (documentation goes here)\n  */\n");
            }
            str << clb->GetString(i);
            str.Replace(_T("&&"), _T("&"));
            array.Add(str + (m_Decl ? _T(";\n") : _T("\n{\n\n}\n\n")));
        }
    }

    return array;
}

void InsertClassMethodDlg::FillClasses()
{
    if (!m_pParser || !m_pParser->Done())
        return;

    wxListBox* lb = XRCCTRL(*this, "lstClasses", wxListBox);
    lb->Freeze();
    lb->Clear();
    TokensTree* tree = m_pParser->GetTokens();
    for (size_t i = 0; i < tree->size(); ++i)
    {
        Token* token = tree->at(i);
        //Manager::Get()->GetMessageManager()->DebugLog("m_Filename=%s, token=%s", m_Filename.c_str(), token->m_Filename.c_str());
        if (token && token->m_TokenKind == tkClass) //&&
            //token->m_Filename == UnixFilename(m_Filename))
            // TODO: check against file's pair too
            lb->Append(token->m_Name, token);
    }
    lb->Thaw();
    FillMethods();
}

void InsertClassMethodDlg::FillMethods()
{
    if (!m_pParser || !m_pParser->Done())
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
    //Manager::Get()->GetMessageManager()->DebugLog("Fill methods for %s", parentToken->m_DisplayName.c_str());

    // loop ascending the inheritance tree

    TokenIdxSet::iterator it;
    int idx;
    Token* token;
    for (it = parentToken->m_Children.begin(); it != parentToken->m_Children.end(); it++)
    {
        idx = *it;
        token = tree->at(idx);
        if (!token)
            continue;

        //Manager::Get()->GetMessageManager()->DebugLog("Evaluating %s", token->m_DisplayName.c_str());
        bool valid = token->m_TokenKind == tkFunction &&
                ((includePrivate && token->m_Scope == tsPrivate) ||
                (includeProtected && token->m_Scope == tsProtected) ||
                (includePublic && token->m_Scope == tsPublic));
        if (valid)
        {
            //Manager::Get()->GetMessageManager()->DebugLog("Adding %s", token->m_DisplayName.c_str());
            // BUG IN WXWIDGETS: wxCheckListBox::Append(string, data) crashes...
            //                   wxCheckListBox::Append(string) does not...
            wxString str;
            str << token->m_Type << _T(" ") << (!m_Decl ? ns : _T("")) << token->m_Name << token->m_Args;
            str.Replace(_T("&"), _T("&&"));
            if (clb->FindString(str) == wxNOT_FOUND)
                clb->Append(str);
        }
    }

    // inheritance
	for (it = parentToken->m_DirectAncestors.begin();it!=parentToken->m_DirectAncestors.end();it++)
    {
        idx = *it;
        token = tree->at(idx);
        if (!token)
            continue;
        DoFillMethodsFor(clb, token, ns, includePrivate, includeProtected, includePublic);
    }
}

// events

void InsertClassMethodDlg::OnClassesChange(wxCommandEvent& event)
{
    FillMethods();
}

void InsertClassMethodDlg::OnCodeChange(wxCommandEvent& event)
{
    m_Decl = XRCCTRL(*this, "rbCode", wxRadioBox)->GetSelection() == 0;
    FillMethods();
}

void InsertClassMethodDlg::OnFilterChange(wxCommandEvent& event)
{
    FillMethods();
}
