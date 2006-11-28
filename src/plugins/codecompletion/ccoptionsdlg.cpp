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
#include <wx/regex.h>
#include <configmanager.h>
#include <manager.h>
#include <editpairdlg.h>
#include <globals.h>

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
	EVT_BUTTON(XRCID("btnAddRepl"), CCOptionsDlg::OnAddRepl)
	EVT_BUTTON(XRCID("btnEditRepl"), CCOptionsDlg::OnEditRepl)
	EVT_BUTTON(XRCID("btnDelRepl"), CCOptionsDlg::OnDelRepl)
	EVT_BUTTON(XRCID("btnColour"), CCOptionsDlg::OnChooseColour)
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
	XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/auto_launch_chars"), 4));
	XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_matches"), 16384));
	XRCCTRL(*this, "chkInheritance", wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().showInheritance);
	XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_threads"), 1));
	XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->Enable(false);
	XRCCTRL(*this, "chkFloatCB", wxCheckBox)->SetValue(cfg->ReadBool(_T("/as_floating_window"), false));
	XRCCTRL(*this, "chkNoSB", wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_symbols_browser"), true));

	int timerDelay = cfg->ReadInt(_T("/cc_delay"), 500);
	XRCCTRL(*this, "sliderDelay", wxSlider)->SetValue(timerDelay / 100);
	UpdateSliderLabel();

    const ConfigManagerContainer::StringToStringMap& repl = Tokenizer::GetTokenReplacementsMap();
    ConfigManagerContainer::StringToStringMap::const_iterator it = repl.begin();
    while (it != repl.end())
    {
        XRCCTRL(*this, "lstRepl", wxListBox)->Append(it->first + _T(" -> ") + it->second);
        ++it;
    }

//	m_Parser.ParseBuffer(g_SampleClasses, true);
//	m_Parser.BuildTree(*XRCCTRL(*this, "treeClasses", wxTreeCtrl));
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

bool CCOptionsDlg::ValidateReplacementToken(wxString& from, wxString& to)
{
    wxRegEx re(_T("[A-Za-z_]+[0-9]*[A-Za-z_]*"));
    from.Trim(true).Trim(false);
    to.Trim(true).Trim(false);
    if (!re.Matches(from) || !re.Matches(to))
    {
        cbMessageBox(_("Replacement tokens can only contain alphanumeric characters and underscores..."),
                    _("Error"), wxICON_ERROR);
        return false;
    }
    return true;
}

void CCOptionsDlg::OnAddRepl(wxCommandEvent& event)
{
    wxString key;
    wxString value;
    EditPairDlg dlg(this, key, value, _("Add new replacement token"), EditPairDlg::bmDisable);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (ValidateReplacementToken(key, value))
        {
            Tokenizer::SetReplacementString(key, value);
            XRCCTRL(*this, "lstRepl", wxListBox)->Append(key + _T(" -> ") + value);
        }
    }
}

void CCOptionsDlg::OnEditRepl(wxCommandEvent& event)
{
    wxString key;
    wxString value;

    int sel = XRCCTRL(*this, "lstRepl", wxListBox)->GetSelection();
    if (sel == -1)
        return;

    key = XRCCTRL(*this, "lstRepl", wxListBox)->GetStringSelection();
    value = key;

    key = key.BeforeFirst(_T(' '));
    value = value.AfterLast(_T(' '));

    EditPairDlg dlg(this, key, value, _("Edit replacement token"), EditPairDlg::bmDisable);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (ValidateReplacementToken(key, value))
        {
            Tokenizer::SetReplacementString(key, value);
            XRCCTRL(*this, "lstRepl", wxListBox)->SetString(sel, key + _T(" -> ") + value);
        }
    }
}

void CCOptionsDlg::OnDelRepl(wxCommandEvent& event)
{
    int sel = XRCCTRL(*this, "lstRepl", wxListBox)->GetSelection();
    if (sel == -1)
        return;

    if (cbMessageBox(_("Are you sure you want to delete this replacement token?"),
                    _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
        wxString key = XRCCTRL(*this, "lstRepl", wxListBox)->GetStringSelection();
        key = key.BeforeFirst(_T(' '));
        Tokenizer::RemoveReplacementString(key);
        XRCCTRL(*this, "lstRepl", wxListBox)->Delete(sel);
    }
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
    XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->Enable(en);
    XRCCTRL(*this, "sliderDelay", wxSlider)->Enable(en);
    XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->Enable(en);

    en = !XRCCTRL(*this, "chkNoSB", wxCheckBox)->GetValue();
	XRCCTRL(*this, "chkInheritance", wxCheckBox)->Enable(en);
	XRCCTRL(*this, "chkFloatCB", wxCheckBox)->Enable(en);

    int sel = XRCCTRL(*this, "lstRepl", wxListBox)->GetSelection();
    XRCCTRL(*this, "btnEditRepl", wxButton)->Enable(sel != -1);
    XRCCTRL(*this, "btnDelRepl", wxButton)->Enable(sel != -1);
}

void CCOptionsDlg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // force parser to read its options that we write in the config
	cfg->Write(_T("/use_code_completion"), (bool)!XRCCTRL(*this, "chkNoCC", wxCheckBox)->GetValue());
	cfg->Write(_T("/max_threads"), (int)XRCCTRL(*this, "spnThreadsNum", wxSpinCtrl)->GetValue());

	int timerDelay = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue() * 100;
	cfg->Write(_T("/cc_delay"), (int)timerDelay);
	m_Parser.ReadOptions();

    // set all other member options
	m_Parser.Options().followLocalIncludes = XRCCTRL(*this, "chkLocals", wxCheckBox)->GetValue();
	m_Parser.Options().followGlobalIncludes = XRCCTRL(*this, "chkGlobals", wxCheckBox)->GetValue();
	m_Parser.Options().wantPreprocessor = XRCCTRL(*this, "chkPreprocessor", wxCheckBox)->GetValue();
	cfg->Write(_T("/auto_select_one"), (bool)XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->GetValue());
	cfg->Write(_T("/auto_launch"), (bool)XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->GetValue());
	cfg->Write(_T("/auto_launch_chars"), (int)XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->GetValue());
	cfg->Write(_T("/max_matches"), (int)XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->GetValue());
	m_Parser.Options().caseSensitive = XRCCTRL(*this, "chkCaseSensitive", wxCheckBox)->GetValue();
	m_Parser.Options().useSmartSense = !XRCCTRL(*this, "chkSimpleMode", wxCheckBox)->GetValue();

	cfg->Write(_T("/use_symbols_browser"), (bool)!XRCCTRL(*this, "chkNoSB", wxCheckBox)->GetValue());
	m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();
	cfg->Write(_T("/as_floating_window"), (bool)XRCCTRL(*this, "chkFloatCB", wxCheckBox)->GetValue());
    m_Parser.WriteOptions();

    m_pNativeParsers->RereadParserOptions();
}
