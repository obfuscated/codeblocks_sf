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
#include "globals.h"

static const wxString g_SampleClasses =
	_T("class A_class"
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
	"#define SOME_DEFINITION_2\n\n");

BEGIN_EVENT_TABLE(CCOptionsDlg, wxPanel)
    EVT_UPDATE_UI(-1, CCOptionsDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnColour"), CCOptionsDlg::OnChooseColour)
	EVT_CHECKBOX(XRCID("chkInheritance"), CCOptionsDlg::OnInheritanceToggle)
	EVT_COMBOBOX(XRCID("cmbCBView"), CCOptionsDlg::OnInheritanceToggle)
	EVT_COMMAND_SCROLL(XRCID("sliderDelay"), CCOptionsDlg::OnSliderScroll)
END_EVENT_TABLE()

CCOptionsDlg::CCOptionsDlg(wxWindow* parent, NativeParser* np)
	: m_Parser(this),
	m_pNativeParsers(np)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

	wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgCCSettings"));

	XRCCTRL(*this, "chkLocals", wxCheckBox)->SetValue(m_Parser.Options().followLocalIncludes);
	XRCCTRL(*this, "chkGlobals", wxCheckBox)->SetValue(m_Parser.Options().followGlobalIncludes);
	XRCCTRL(*this, "chkPreprocessor", wxCheckBox)->SetValue(m_Parser.Options().wantPreprocessor);
	XRCCTRL(*this, "chkNoCC", wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_code_completion"), true));
	XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->SetValue(!m_Parser.Options().useSmartSense);
	XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->SetValue(m_Parser.Options().caseSensitive);
	XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_select_one"), false));
	XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_launch"), true));
	XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/auto_launch"), 4));
	XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_matches"), 16384));
	XRCCTRL(*this, "chkInheritance", wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().showInheritance);
	XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_threads"), 1));
	XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->Enable(false);
	XRCCTRL(*this, "cmbCBView", wxComboBox)->SetSelection(m_Parser.ClassBrowserOptions().viewFlat ? 0 : 1);
//	XRCCTRL(*this, "chkUseCache", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_cache"), false));
//	XRCCTRL(*this, "chkAlwaysUpdateCache", wxCheckBox)->SetValue(cfg->ReadBool(_T("/update_cache_always"), false));
//	XRCCTRL(*this, "chkShowCacheProgress", wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_cache_progress"), true));
	XRCCTRL(*this, "chkUseCache", wxCheckBox)->Enable(false);
	XRCCTRL(*this, "chkAlwaysUpdateCache", wxCheckBox)->Enable(false);
	XRCCTRL(*this, "chkShowCacheProgress", wxCheckBox)->Enable(false);

    XRCCTRL(*this, "chkInheritance", wxCheckBox)->Enable(false); // does not work currently

	int timerDelay = cfg->ReadInt(_T("/cc_delay"), 500);
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

void CCOptionsDlg::OnChooseColour(wxCommandEvent& event)
{
	wxColourData data;
	wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

	wxColourDialog dlg(this, &data);
	PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
    	wxColour colour = dlg.GetColourData().GetColour();
	    sender->SetBackgroundColour(colour);
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

void CCOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = !XRCCTRL(*this, "chkNoCC", wxCheckBox)->GetValue();
    bool auto_launch = XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->GetValue();

    XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->Enable(en && auto_launch);
    XRCCTRL(*this, "sliderDelay", wxSlider)->Enable(en);
}

void CCOptionsDlg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // force parser to read its options that we write in the config
	cfg->Write(_T("/use_code_completion"), (bool)!XRCCTRL(*this, "chkNoCC", wxCheckBox)->GetValue());
	cfg->Write(_T("/max_threads"), (int)XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->GetValue());
	cfg->Write(_T("/use_cache"), (bool)XRCCTRL(*this, "chkUseCache", wxCheckBox)->GetValue());
	cfg->Write(_T("/update_cache_always"), (bool)XRCCTRL(*this, "chkAlwaysUpdateCache", wxCheckBox)->GetValue());
	cfg->Write(_T("/show_cache_progress"), (bool)XRCCTRL(*this, "chkShowCacheProgress", wxCheckBox)->GetValue());

	int timerDelay = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue() * 100;
	cfg->Write(_T("/cc_delay"), (int)timerDelay);
	m_Parser.ReadOptions();

    // set all other member options
	m_Parser.Options().followLocalIncludes = XRCCTRL(*this, "chkLocals", wxCheckBox)->GetValue();
	m_Parser.Options().followGlobalIncludes = XRCCTRL(*this, "chkGlobals", wxCheckBox)->GetValue();
	m_Parser.Options().wantPreprocessor = XRCCTRL(*this, "chkPreprocessor", wxCheckBox)->GetValue();
	cfg->Write(_T("/auto_select_one"), (bool)XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->GetValue());
	cfg->Write(_T("/auto_launch"), (bool)XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->GetValue());
	cfg->Write(_T("/auto_launch"), (int)XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->GetValue());
	cfg->Write(_T("/max_matches"), (int)XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->GetValue());
	m_Parser.Options().caseSensitive = XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->GetValue();
	m_Parser.Options().useSmartSense = !XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->GetValue();
//	m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();  // does not work currently
	m_Parser.ClassBrowserOptions().viewFlat = XRCCTRL(*this, "cmbCBView", wxComboBox)->GetSelection() == 0;
	m_Parser.WriteOptions();

    m_pNativeParsers->RereadParserOptions();
}
