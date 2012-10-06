/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include "SpellCheckSettingsPanel.h"

#include "SpellCheckerConfig.h"
#include <configmanager.h>
#include <logmanager.h>
#include <macrosmanager.h>

#include <wx/dir.h>
#include <wx/dirdlg.h>

#include <map>
#include <vector>

//(*InternalHeaders(SpellCheckSettingsPanel)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(SpellCheckSettingsPanel)
//*)

BEGIN_EVENT_TABLE(SpellCheckSettingsPanel,wxPanel)
    //(*EventTable(SpellCheckSettingsPanel)
    //*)
END_EVENT_TABLE()

SpellCheckSettingsPanel::SpellCheckSettingsPanel(wxWindow* parent, SpellCheckerConfig *cfg)
:m_sccfg(cfg)
{
    //(*Initialize(SpellCheckSettingsPanel)
    wxXmlResource::Get()->LoadObject(this,parent,_T("SpellCheckSettingsPanel"),_T("wxPanel"));
    m_checkEnableOnlineSpellChecker = (wxCheckBox*)FindWindow(XRCID("ID_CHECKBOX1"));
    m_checkSpellTooltips = (wxCheckBox*)FindWindow(XRCID("ID_CHECKBOX2"));
    m_checkThesaurusTooltips = (wxCheckBox*)FindWindow(XRCID("ID_CHECKBOX3"));
    StaticText3 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT3"));
    m_choiceDictionary = (wxChoice*)FindWindow(XRCID("ID_CHOICE3"));
    StaticText1 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT1"));
    StaticText2 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT2"));
    StaticText4 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT4"));
    m_TextDictPath = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL1"));
    m_TextThPath = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL2"));
    m_TextBitmapPath = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL3"));
    Button1 = (wxButton*)FindWindow(XRCID("ID_BUTTON_DICTIONARIES"));
    Button2 = (wxButton*)FindWindow(XRCID("ID_BUTTON_THESAURI"));
    Button3 = (wxButton*)FindWindow(XRCID("ID_BUTTON_BITMAPS"));
    HyperlinkCtrl1 = (wxHyperlinkCtrl*)FindWindow(XRCID("ID_HYPERLINKCTRL1"));

    Connect(XRCID("ID_BUTTON_DICTIONARIES"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SpellCheckSettingsPanel::OnChooseDirectory);
    Connect(XRCID("ID_BUTTON_THESAURI"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SpellCheckSettingsPanel::OnChooseDirectory);
    Connect(XRCID("ID_BUTTON_BITMAPS"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SpellCheckSettingsPanel::OnChooseDirectory);
    //*)

    Connect(XRCID("ID_TEXTCTRL1"), wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&SpellCheckSettingsPanel::OnChangeDictPathText);

    m_TextDictPath->SetValue(m_sccfg->GetRawDictionaryPath());
    m_TextThPath->SetValue(m_sccfg->GetRawThesaurusPath());
    m_TextBitmapPath->SetValue(m_sccfg->GetRawBitmapPath());

    InitDictionaryChoice();

    m_TextDictPath->SetToolTip(_T("Path to dictionary (.aff and .dic) files"));
    m_TextThPath->SetToolTip(_T("Path to thesaurus (th_*.dat and th_*.idx) files"));
    m_TextBitmapPath->SetToolTip(_T("Path to dictionary-switcher bitmaps (.png)"));

    Button1->SetToolTip(_T("Select path to dictionary (.aff and .dic) files"));
    Button2->SetToolTip(_T("Select path to thesaurus (th_*.dat and th_*.idx) files"));
    Button3->SetToolTip(_T("Select path to dictionary-switcher bitmaps (.png)"));


    #ifndef wxUSE_STATUSBAR
    // hide since they are not used atm
    StaticText4->Hide();
    m_TextBitmapPath->Hide();
    Button3->Hide();
    #endif
}

SpellCheckSettingsPanel::~SpellCheckSettingsPanel()
{
    //(*Destroy(SpellCheckSettingsPanel)
    //*)
}


void SpellCheckSettingsPanel::InitDictionaryChoice(const wxString &path)
{
    if ( path.IsEmpty() )
        m_sccfg->ScanForDictionaries();
    else
        m_sccfg->ScanForDictionaries(path);
    std::vector<wxString> dics = m_sccfg->GetPossibleDictionaries();
    int sel = m_sccfg->GetSelectedDictionaryNumber();

    m_choiceDictionary->Clear();
    for ( unsigned int i = 0 ; i < dics.size(); i++ )
        m_choiceDictionary->AppendString(m_sccfg->GetLanguageName(dics[i]));

    if ( sel != -1 )
        m_choiceDictionary->Select(sel);

    m_checkEnableOnlineSpellChecker->Enable(!dics.empty());
    m_checkEnableOnlineSpellChecker->SetValue( m_sccfg->GetEnableOnlineChecker() && (!dics.empty()) );
    m_checkSpellTooltips->SetValue( m_sccfg->GetEnableSpellTooltips() && (!dics.empty()) );
    m_checkThesaurusTooltips->SetValue( m_sccfg->GetEnableThesaurusTooltips() && (!dics.empty()) );
}
wxString SpellCheckSettingsPanel::GetTitle() const {return _T("SpellChecker");}

wxString SpellCheckSettingsPanel::GetBitmapBaseName() const {return _T("SpellChecker");}

void SpellCheckSettingsPanel::PostConfig()
{
    m_sccfg->SetEnableOnlineChecker(m_checkEnableOnlineSpellChecker->GetValue());
    m_sccfg->SetEnableSpellTooltips(m_checkSpellTooltips->GetValue());
    m_sccfg->SetEnableThesaurusTooltips(m_checkThesaurusTooltips->GetValue());
    std::vector<wxString> dics = m_sccfg->GetPossibleDictionaries();
    const int sel = m_choiceDictionary->GetSelection();
    if ( sel < static_cast<int>(dics.size()) && sel != wxNOT_FOUND )
    {
        wxString dic = dics[sel];
        if ( !dic.IsEmpty() )
        {
            m_sccfg->SetDictionaryName( dic );
        }
    }

    wxString path;

    path = m_TextBitmapPath->GetValue();
    if ( !path.IsEmpty())
        m_sccfg->SetBitmapPath(path);

    path = m_TextThPath->GetValue();
    if ( !path.IsEmpty())
        m_sccfg->SetThesaurusPath(path);

    path = m_TextDictPath->GetValue();
    if ( !path.IsEmpty())
        m_sccfg->SetDictionaryPath(path);
}

void SpellCheckSettingsPanel::OnApply()
{
    PostConfig(); // to update the configuration object
    m_sccfg->Save(); // save it
}

void SpellCheckSettingsPanel::OnCancel()
{
    m_sccfg->Load(); // to restore the previous settings
}


void SpellCheckSettingsPanel::OnChooseDirectory(wxCommandEvent& event)
{
    wxString message = _T("Choose the directory containing ");
    wxTextCtrl *textctrl;

    if ( event.GetId() == XRCID("ID_BUTTON_DICTIONARIES") )
    {
        message += _T("the dictionaries");
        textctrl = m_TextDictPath;
        //defaultDir = m_TextDictPath->GetValue();
    }
    else if ( event.GetId() == XRCID("ID_BUTTON_THESAURI") )
    {
        message += _T("the thesaurus files");
        textctrl = m_TextThPath;
    }
    else //XRCID("ID_BUTTON_BITMAPS")
    {
        message += _T("the bitmaps");
        textctrl = m_TextBitmapPath;
    }
    wxString path = textctrl->GetValue();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(path);
    wxDirDialog dlg(this, message, path, wxDD_DIR_MUST_EXIST);
    PlaceWindow(&dlg);
    if ( dlg.ShowModal() == wxID_OK )
    {
        textctrl->SetValue( dlg.GetPath() );
        if ( event.GetId() == XRCID("ID_BUTTON_DICTIONARIES") )
        {
            InitDictionaryChoice();
        }
    }
}

void SpellCheckSettingsPanel::OnChangeDictPathText( wxCommandEvent &event)
{
    wxString path = m_TextDictPath->GetValue();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(path);
    if ( wxDir::Exists( path ) )
    {
        InitDictionaryChoice( path );
    }
    else
    {
        m_choiceDictionary->Clear();
        m_checkEnableOnlineSpellChecker->Enable(false);
        m_checkEnableOnlineSpellChecker->SetValue(false);
    }
}
