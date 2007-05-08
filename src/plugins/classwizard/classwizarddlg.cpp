/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "cbeditor.h"
#include "cbproject.h"
#include "configmanager.h"
#include "editormanager.h"
#include "globals.h"
#include "manager.h"
#include "projectmanager.h"
#endif
#include "classwizarddlg.h"
#include <wx/tokenzr.h>
#include <wx/dirdlg.h>

namespace {


void ForceDirectory(const wxFileName & filename)
{
    wxFileName parentname(filename);
    parentname.RemoveLastDir();
    if (filename!=parentname)
        ForceDirectory(parentname);
    if (!wxDirExists(filename.GetPath()))
        wxMkdir(filename.GetPath());
}

}


BEGIN_EVENT_TABLE(ClassWizardDlg, wxDialog)
    EVT_UPDATE_UI(-1, ClassWizardDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("wxID_OK"), ClassWizardDlg::OnOKClick)
    EVT_BUTTON(XRCID("wxID_CANCEL"), ClassWizardDlg::OnCancelClick)
    EVT_TEXT(XRCID("txtName"), ClassWizardDlg::OnNameChange)
    EVT_TEXT(XRCID("txtInheritance"), ClassWizardDlg::OnAncestorChange)
    EVT_BUTTON(XRCID("btnBaseDir"), ClassWizardDlg::OnBaseDirClick)
END_EVENT_TABLE()


ClassWizardDlg::ClassWizardDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgNewClass"));

    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* prj = prjMan->GetActiveProject();
    if (prj)
  	    XRCCTRL(*this, "txtBaseDir", wxTextCtrl)->SetValue(prj->GetBasePath());
    else
  	    XRCCTRL(*this, "txtBaseDir", wxTextCtrl)->SetValue(::wxGetCwd());
}


ClassWizardDlg::~ClassWizardDlg()
{
}


void ClassWizardDlg::DoGuardBlock()
{
    m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
    wxString GuardWord = m_Header;
    GuardWord.MakeUpper();
    while (GuardWord.Replace(_T("."), _T("_")))
        ;
    while (GuardWord.Replace(_T("/"), _T("_")))
        ;
    while (GuardWord.Replace(_T("\\"), _T("_")))
        ;
    XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}

// events

void ClassWizardDlg::OnNameChange(wxCommandEvent& event)
{
    wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
    //name.MakeLower();
    while (name.Replace(_T("::"), _T("/")))
        ;
    XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + _T(".h"));
    XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + _T(".cpp"));
    DoGuardBlock();
}


void ClassWizardDlg::OnAncestorChange(wxCommandEvent& event)
{
    wxString name = XRCCTRL(*this, "txtInheritance", wxTextCtrl)->GetValue();
    name.MakeLower();
    while (name.Replace(_T("::"), _T("/")))
        ;
    XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->SetValue(name + _T(".h"));
    DoGuardBlock();
}


void ClassWizardDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtInheritance", wxTextCtrl)->Enable(inherits);
    XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->Enable(inherits);
    XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->Enable(inherits);

    bool genimpl = XRCCTRL(*this, "chkImplementation", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtImplementation", wxTextCtrl)->Enable(genimpl);

    bool genguard = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->Enable(genguard);
}



void ClassWizardDlg::OnOKClick(wxCommandEvent& event)
{
    wxLogNull null_logger; // we do all file checks ourselves

    // set some variable for easy reference
    wxString Name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
    wxArrayString NameSpaces;
    wxStringTokenizer tkz(Name, _T("::"));
    Name=_T("");
    while ( tkz.HasMoreTokens() )
    {
        if (Name!=_T(""))
            NameSpaces.Add(Name);
        Name = tkz.GetNextToken();
    }

    wxString baseDir = XRCCTRL(*this, "txtBaseDir", wxTextCtrl)->GetValue();
    //wxSetWorkingDirectory(baseDir);

    wxString Constructor = XRCCTRL(*this, "txtConstructor", wxTextCtrl)->GetValue();
    bool VirtualDestructor = XRCCTRL(*this, "chkVirtualDestructor", wxCheckBox)->GetValue();

    wxString Ancestor = XRCCTRL(*this, "txtInheritance", wxTextCtrl)->GetValue();
    wxString AncestorFilename = XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->GetValue();
    wxString AncestorScope = XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->GetValue();
    bool Inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue() && !Ancestor.IsEmpty();

    m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
    m_Implementation = XRCCTRL(*this, "txtImplementation", wxTextCtrl)->GetValue();
    bool GenerateImplementation = XRCCTRL(*this, "chkImplementation", wxCheckBox)->GetValue();
    bool GuardBlock = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
    wxString GuardWord = XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->GetValue();
    if (GuardWord.IsEmpty())
        DoGuardBlock();


    wxFileName headerFname(UnixFilename(m_Header));
    wxFileName implementationFname(UnixFilename(m_Implementation));
    headerFname.MakeAbsolute(baseDir);
    implementationFname.MakeAbsolute(baseDir);

    bool usestabs = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
    int tabsize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    int eolmode = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/eol/eolmode"), 0);

    ForceDirectory(headerFname);
    cbEditor * new_ed = Manager::Get()->GetEditorManager()->New(headerFname.GetFullPath());
    wxString buffer = new_ed->GetControl()->GetText();
    wxString tabstr = usestabs ? wxString(_T("\t")) : wxString(_T(' '),tabsize);
    wxString eolstr;
    if(eolmode == 2)
        eolstr = _T("\n");
    else if(eolmode == 1)
        eolstr = _T("\r");
    else
        eolstr = _T("\r\n");


    // actual creation starts here
    // let's start with the header file
    if (GuardBlock)
    {
        buffer << _T("#ifndef ") << GuardWord << eolstr;
        buffer << _T("#define ") << GuardWord << eolstr;
        buffer << eolstr;
    }

    if (!AncestorFilename.IsEmpty())
    {
        buffer << _T("#include <") << AncestorFilename << _T(">") << eolstr;
        buffer << eolstr;
    }

    for (unsigned int i=0; i<NameSpaces.GetCount(); ++i)
        buffer << _T("namespace ") << NameSpaces[i] << _T(" {") << eolstr;
    buffer << eolstr;


    buffer << _T("class ") << Name;
    if (Inherits)
        buffer << _T(" : ") << AncestorScope << _T(" ") << Ancestor;
    buffer << eolstr;
    buffer << _T("{") << eolstr;
    buffer << tabstr << _T("public:") << eolstr;
    buffer << tabstr << tabstr << Name << _T("(") << Constructor << _T(");") << eolstr;
    buffer << tabstr << tabstr;
    if (VirtualDestructor)
        buffer << _T("virtual ");
    buffer << _T('~') << Name << _T("();") << eolstr;
    buffer << tabstr << _T("protected:") << eolstr;
    buffer << tabstr << _T("private:") << eolstr;
    buffer << _T("};") << eolstr;

    buffer << eolstr;
    for (int i=NameSpaces.GetCount(); i>0; --i)
        buffer << _T("} // namespace ") << NameSpaces[i-1] << eolstr;
    buffer << eolstr;

    if (GuardBlock)
    {
        buffer << eolstr;
        buffer << _T("#endif // ") << GuardWord << eolstr;
    }

    new_ed->GetControl()->SetText(buffer);
    if (!new_ed->Save())
    {
        wxString msg;
        msg.Printf(_("Could not save header file %s.\nAborting..."), headerFname.GetFullPath().c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
    }


    if (!GenerateImplementation)
    {
        m_Header = headerFname.GetFullPath();
        m_Implementation = _T("");
        EndModal(wxID_OK);
        return;
    }
    // now the implementation file

    ForceDirectory(implementationFname);
    new_ed = Manager::Get()->GetEditorManager()->New(implementationFname.GetFullPath());
    buffer = new_ed->GetControl()->GetText();

    buffer << _T("#include \"") << m_Header << _T("\"") << eolstr;
    buffer << eolstr;
    for (unsigned int i=0; i<NameSpaces.GetCount(); ++i)
        buffer << _T("namespace ") << NameSpaces[i] << _T(" {") << eolstr;
    buffer << eolstr;

    buffer << Name << _T("::") << Name << _T("(") << Constructor << _T(")") << eolstr;
    buffer << _T("{") << eolstr;
    buffer << tabstr << _T("//ctor") << eolstr;
    buffer << _T("}") << eolstr;
    buffer << eolstr;
    buffer << Name << _T("::~") << Name << _T("()") << eolstr;
    buffer << _T("{") << eolstr;
    buffer << tabstr << _T("//dtor") << eolstr;
    buffer << _T("}") << eolstr;

    buffer << eolstr;
    for (int i=NameSpaces.GetCount(); i>0; --i)
        buffer << _T("} // namespace ") << NameSpaces[i-1] << eolstr;
    buffer << eolstr;

    new_ed->GetControl()->SetText(buffer);
    if (!new_ed->Save())
    {
        wxString msg;
        msg.Printf(_("Could not save implementation file %s.\nAborting..."), implementationFname.GetFullPath().c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
    }

    m_Header= headerFname.GetFullPath();
    m_Implementation= implementationFname.GetFullPath();

    EndModal(wxID_OK);
}


void ClassWizardDlg::OnBaseDirClick(wxCommandEvent& event)
{
    wxString path = XRCCTRL(*this, "txtBaseDir", wxTextCtrl)->GetValue();
    wxDirDialog dlg (this, _T("Choose a directory"), path);
    if (dlg.ShowModal()==wxID_OK)
    {
        path = dlg.GetPath();
        XRCCTRL(*this, "txtBaseDir", wxTextCtrl)->SetValue(path);
    }
}


void ClassWizardDlg::OnCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}
