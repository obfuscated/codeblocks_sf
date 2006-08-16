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
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "configmanager.h"
#include "globals.h"
#include "manager.h"
#endif
#include "classwizarddlg.h"

BEGIN_EVENT_TABLE(ClassWizardDlg, wxDialog)
	EVT_UPDATE_UI(-1, ClassWizardDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("wxID_OK"), ClassWizardDlg::OnOKClick)
	EVT_BUTTON(XRCID("wxID_CANCEL"), ClassWizardDlg::OnCancelClick)
	EVT_TEXT(XRCID("txtName"), ClassWizardDlg::OnNameChange)
END_EVENT_TABLE()

ClassWizardDlg::ClassWizardDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgNewClass"));
}

ClassWizardDlg::~ClassWizardDlg()
{
}

void ClassWizardDlg::DoGuardBlock()
{
	m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
	wxFileName headerFname(m_Header);
	wxString GuardWord = headerFname.GetFullName();
	GuardWord.MakeUpper();
	GuardWord.Replace(_T("."), _T("_"));
	XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}

// events

void ClassWizardDlg::OnNameChange(wxCommandEvent& event)
{
	wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
	name.MakeLower();
	XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + _T(".h"));
	XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + _T(".cpp"));
	DoGuardBlock();
}

void ClassWizardDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	bool inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue();
	XRCCTRL(*this, "txtInheritance", wxTextCtrl)->Enable(inherits);
	XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->Enable(inherits);
	XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->Enable(inherits);
}

void ClassWizardDlg::OnOKClick(wxCommandEvent& event)
{
    wxLogNull null_logger; // we do all file checks ourselves

	// set some variable for easy reference
	wxString Name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
	wxString Constructor = XRCCTRL(*this, "txtConstructor", wxTextCtrl)->GetValue();
	bool VirtualDestructor = XRCCTRL(*this, "chkVirtualDestructor", wxCheckBox)->GetValue();

	wxString Ancestor = XRCCTRL(*this, "txtInheritance", wxTextCtrl)->GetValue();
	wxString AncestorFilename = XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->GetValue();
	wxString AncestorScope = XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->GetValue();
	bool Inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue() && !Ancestor.IsEmpty();

	m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
	m_Implementation = XRCCTRL(*this, "txtImplementation", wxTextCtrl)->GetValue();
	bool GuardBlock = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
	wxString GuardWord = XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->GetValue();
	if (GuardWord.IsEmpty())
		DoGuardBlock();
	wxFileName headerFname(UnixFilename(m_Header));

	bool usestabs = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
	int tabsize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
	int eolmode = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/eol/eolmode"), 0);

	wxString buffer;
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

	if (GuardBlock)
	{
		buffer << eolstr;
		buffer << _T("#endif // ") << GuardWord << eolstr;
	}

	// write buffer to disk
	wxFile hdr(UnixFilename(m_Header), wxFile::write);
	if (!hdr.IsOpened())
	{
        wxString msg;
        msg.Printf(_("Could not create header file %s.\nAborting..."), m_Header.c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
	}
	cbWrite(hdr,buffer);
	// end of header file

	// now the implementation file
	buffer.Clear();
	buffer << _T("#include \"") << headerFname.GetFullName() << _T("\"") << eolstr;
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
	// write buffer to disk
	wxFile impl(UnixFilename(m_Implementation), wxFile::write);
	if (!impl.IsOpened())
	{
        wxString msg;
        msg.Printf(_("Could not create implementation file %s.\nAborting..."), m_Implementation.c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
	}
	cbWrite(impl,buffer);
	// end of implementation file

	EndModal(wxID_OK);
}

void ClassWizardDlg::OnCancelClick(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}
