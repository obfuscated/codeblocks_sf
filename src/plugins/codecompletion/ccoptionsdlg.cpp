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

#include "ccoptionsdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/treectrl.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/colordlg.h>
#include <configmanager.h>
#include <manager.h>

static const wxString g_SampleClasses = 
	"class A_class"
	"{"
	"	public:"
	"		int someInt_A;"
	"	protected:"
	"		bool mSomeVar_A;"
	"	private:"
	"		char* mData_A;"
	"};"
	"class B_class"
	"{"
	"	public:"
	"		int someInt_B;"
	"	protected:"
	"		bool mSomeVar_B;"
	"	private:"
	"		char* mData_B;"
	"};"
	"class C_class : public A_class"
	"{"
	"	public:"
	"		int someInt_C;"
	"	protected:"
	"		bool mSomeVar_C;"
	"	private:"
	"		char* mData_C;"
	"};"
	"enum SomeEnum"
	"{"
	"	optOne,"
	"	optTwo,"
	"	optThree"
	"};"
	"int x;"
	"int y;"
	"#define SOME_DEFINITION\n"
	"#define SOME_DEFINITION_2\n\n";

BEGIN_EVENT_TABLE(CCOptionsDlg, wxDialog)
	EVT_BUTTON(XRCID("wxID_OK"), CCOptionsDlg::OnOK)
	EVT_BUTTON(XRCID("btnColor"), CCOptionsDlg::OnChooseColor)
	EVT_CHECKBOX(XRCID("chkInheritance"), CCOptionsDlg::OnInheritanceToggle)
	EVT_COMBOBOX(XRCID("cmbCBView"), CCOptionsDlg::OnInheritanceToggle)
	EVT_COMMAND_SCROLL(XRCID("sliderDelay"), CCOptionsDlg::OnSliderScroll)
END_EVENT_TABLE()

CCOptionsDlg::CCOptionsDlg(wxWindow* parent)
	: m_Parser(this)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgCCSettings"));
	XRCCTRL(*this, "spnThreads", wxSpinCtrl)->SetValue(m_Parser.GetMaxThreads());
	XRCCTRL(*this, "chkLocals", wxCheckBox)->SetValue(m_Parser.Options().followLocalIncludes);
	XRCCTRL(*this, "chkGlobals", wxCheckBox)->SetValue(m_Parser.Options().followGlobalIncludes);
	XRCCTRL(*this, "chkPreprocessor", wxCheckBox)->SetValue(m_Parser.Options().wantPreprocessor);
	XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->SetValue(!m_Parser.Options().useSmartSense);
	XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->SetValue(m_Parser.Options().caseSensitive);
	XRCCTRL(*this, "chkInheritance", wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().showInheritance);
	XRCCTRL(*this, "cmbCBView", wxComboBox)->SetSelection(m_Parser.ClassBrowserOptions().viewFlat ? 0 : 1);
	XRCCTRL(*this, "chkUseCache", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/code_completion/use_cache", 0L));
	XRCCTRL(*this, "chkAlwaysUpdateCache", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/code_completion/update_cache_always", 0L));

    wxColour color(ConfigManager::Get()->Read("/code_completion/color/red", 0xFF),
    				ConfigManager::Get()->Read("/code_completion/color/green", 0xFF),
    				ConfigManager::Get()->Read("/code_completion/color/blue", 0xFF)
					);
    XRCCTRL(*this, "btnColor", wxButton)->SetBackgroundColour(color);

	int timerDelay = ConfigManager::Get()->Read("/editor/cc_delay", 500);
	XRCCTRL(*this, "sliderDelay", wxSlider)->SetValue(timerDelay / 100);
	UpdateSliderLabel();
	
	m_Parser.ParseBuffer(g_SampleClasses, true);
	m_Parser.BuildTree(*XRCCTRL(*this, "treeClasses", wxTreeCtrl));
}

CCOptionsDlg::~CCOptionsDlg()
{
}

void CCOptionsDlg::UpdateSliderLabel()
{
	int position = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue();
	wxString lbl;
	if (position >= 10)
		lbl.Printf(_("%d.%d sec"), position / 10, position % 10);
	else
		lbl.Printf(_("%d ms"), position * 100);
	XRCCTRL(*this, "lblDelay", wxStaticText)->SetLabel(lbl);
}

void CCOptionsDlg::OnChooseColor(wxCommandEvent& event)
{
	wxColourData data;
	wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

	wxColourDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK)
    {
    	wxColour color = dlg.GetColourData().GetColour();
	    sender->SetBackgroundColour(color);
    }
}

void CCOptionsDlg::OnInheritanceToggle(wxCommandEvent& event)
{
//	m_Parser.ClassBrowserOptions().showInheritance = event.IsChecked();
	m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();
	m_Parser.ClassBrowserOptions().viewFlat = XRCCTRL(*this, "cmbCBView", wxComboBox)->GetSelection() == 0;
	m_Parser.BuildTree(*XRCCTRL(*this, "treeClasses", wxTreeCtrl));
}

void CCOptionsDlg::OnSliderScroll(wxScrollEvent& event)
{
	UpdateSliderLabel();
}

void CCOptionsDlg::OnOK(wxCommandEvent& event)
{
	m_Parser.SetMaxThreads(XRCCTRL(*this, "spnThreads", wxSpinCtrl)->GetValue());
	m_Parser.Options().followLocalIncludes = XRCCTRL(*this, "chkLocals", wxCheckBox)->GetValue();
	m_Parser.Options().followGlobalIncludes = XRCCTRL(*this, "chkGlobals", wxCheckBox)->GetValue();
	m_Parser.Options().wantPreprocessor = XRCCTRL(*this, "chkPreprocessor", wxCheckBox)->GetValue();
	m_Parser.Options().caseSensitive = XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->GetValue();
	m_Parser.Options().useSmartSense = !XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->GetValue();
	m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();
	m_Parser.ClassBrowserOptions().viewFlat = XRCCTRL(*this, "cmbCBView", wxComboBox)->GetSelection() == 0;
	m_Parser.WriteOptions();

	ConfigManager::Get()->Write("/code_completion/use_cache", XRCCTRL(*this, "chkUseCache", wxCheckBox)->GetValue());
	ConfigManager::Get()->Write("/code_completion/update_cache_always", XRCCTRL(*this, "chkAlwaysUpdateCache", wxCheckBox)->GetValue());

    ConfigManager::Get()->Write("/code_completion/color/red",		XRCCTRL(*this, "btnColor", wxButton)->GetBackgroundColour().Red());
    ConfigManager::Get()->Write("/code_completion/color/green",	XRCCTRL(*this, "btnColor", wxButton)->GetBackgroundColour().Green());
    ConfigManager::Get()->Write("/code_completion/color/blue",	XRCCTRL(*this, "btnColor", wxButton)->GetBackgroundColour().Blue());

	int timerDelay = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue() * 100;
	ConfigManager::Get()->Write("/editor/cc_delay", timerDelay);

	wxDialog::OnOK(event);
}
