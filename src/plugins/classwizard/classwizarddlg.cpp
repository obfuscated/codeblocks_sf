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
* $Id$
* $Date$
*/

#include "classwizarddlg.h"
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/file.h>

BEGIN_EVENT_TABLE(ClassWizardDlg, wxDialog)
	EVT_UPDATE_UI(-1, ClassWizardDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("wxID_OK"), ClassWizardDlg::OnOKClick)
	EVT_BUTTON(XRCID("wxID_CANCEL"), ClassWizardDlg::OnCancelClick)
	EVT_TEXT(XRCID("txtName"), ClassWizardDlg::OnNameChange)
END_EVENT_TABLE()

ClassWizardDlg::ClassWizardDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgNewClass"));
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
	GuardWord.Replace(".", "_");
	XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}

// events

void ClassWizardDlg::OnNameChange(wxCommandEvent& event)
{
	wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
	name.MakeLower();
	XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + ".h");
	XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + ".cpp");
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
	wxFileName headerFname(m_Header);
	
	wxString buffer;

	// actual creation starts here
	// let's start with the header file
	if (GuardBlock)
	{
		buffer << "#ifndef " << GuardWord << '\n';
		buffer << "#define " << GuardWord << '\n';
		buffer << '\n';
	}

	buffer << "#include <" << AncestorFilename << ">" << '\n';
	buffer << '\n';
	buffer << "class " << Name;
	if (Inherits)
		buffer << " : " << AncestorScope << " " << Ancestor;
	buffer << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "public:" << '\n';
	buffer << '\t' << '\t' << Name << "(" << Constructor << ");" << '\n';
	if (VirtualDestructor)
		buffer << '\t' << '\t' << "virtual ~" << Name << "();" << '\n';
	else
		buffer << '\t' << '\t' << "~" << Name << "();" << '\n';
	buffer << '\t' << "protected:" << '\n';
	buffer << '\t' << "private:" << '\n';
	buffer << "};" << '\n';

	if (GuardBlock)
	{
		buffer << '\n';
		buffer << "#endif // " << GuardWord << '\n';
	}
	buffer << '\n';
	// write buffer to disk
	wxFile hdr(m_Header, wxFile::write);
	hdr.Write(buffer, buffer.Length());
	hdr.Flush();
	// end of header file
	
	// now the implementation file
	buffer.Clear();
	buffer << "#include \"" << headerFname.GetFullName() << "\"" << '\n';
	buffer << '\n';
	buffer << Name << "::" << Name << "(" << Constructor << ")" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//ctor" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	buffer << Name << "::~" << Name << "()" << '\n';
	buffer << "{" << '\n';
	buffer << '\t' << "//dtor" << '\n';
	buffer << "}" << '\n';
	buffer << '\n';
	// write buffer to disk
	wxFile impl(m_Implementation, wxFile::write);
	impl.Write(buffer, buffer.Length());
	impl.Flush();
	// end of implementation file
	
	EndModal(wxID_OK);
}

void ClassWizardDlg::OnCancelClick(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}
