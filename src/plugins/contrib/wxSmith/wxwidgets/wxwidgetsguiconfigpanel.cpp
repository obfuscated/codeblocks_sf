/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxwidgetsguiconfigpanel.h"
#include "../wxsproject.h"

#include <wx/tokenzr.h>
#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(wxWidgetsGUIConfigPanel,cbConfigurationPanel)
	//(*EventTable(wxWidgetsGUIConfigPanel)
	EVT_BUTTON(ID_BUTTON1,wxWidgetsGUIConfigPanel::OnButton1Click)
	EVT_BUTTON(ID_BUTTON2,wxWidgetsGUIConfigPanel::OnButton2Click)
	EVT_CHECKBOX(ID_CHECKBOX2,wxWidgetsGUIConfigPanel::OnInitAllChange)
	//*)
END_EVENT_TABLE()

wxWidgetsGUIConfigPanel::wxWidgetsGUIConfigPanel(wxWindow* parent,wxWidgetsGUI* GUI): m_GUI(GUI)
{
    static const wxWindowID id = -1;
	//(*Initialize(wxWidgetsGUIConfigPanel)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,0,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Automatically loaded resources:"));
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Each line will be passed as file name to\nwxXmlResource::Get()->Load."),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE,_("ID_STATICTEXT1"));
	AutoLoad = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxSize(218,102),wxTE_MULTILINE,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) AutoLoad->SetMaxLength(0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add file"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_("ID_BUTTON1"));
	if (false) Button1->SetDefault();
	Button2 = new wxButton(this,ID_BUTTON2,_("Clear all"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) Button2->SetDefault();
	BoxSizer3->Add(Button1,1,wxALIGN_CENTER,5);
	BoxSizer3->Add(Button2,1,wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(StaticText1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(AutoLoad,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(BoxSizer3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Main resource"));
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	MainRes = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxSize(201,21),0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX1"));
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Window selected here will be created and\nshown while initializing application.\nYou should use one of frames here.\nUsing dialog as main resource is also\nacceptable but may lead to problems\nwith terminating application\nafter closing main dialog."),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE,_("ID_STATICTEXT2"));
	FlexGridSizer1->Add(MainRes,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticText2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	StaticBoxSizer2->Add(FlexGridSizer1,1,wxALIGN_CENTER,5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("XRC system initialization"));
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	InitAll = new wxCheckBox(this,ID_CHECKBOX2,_("Call wxXmlResource::InitAllHandlers()"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX2"));
	InitAll->SetValue(false);
	InitAllNecessary = new wxCheckBox(this,ID_CHECKBOX1,_("Only when necessary"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX1"));
	InitAllNecessary->SetValue(false);
	InitAllNecessary->Disable();
	BoxSizer5->Add(InitAll,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer5->Add(InitAllNecessary,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer3->Add(BoxSizer5,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(StaticBoxSizer3,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(BoxSizer4,0,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer2,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

    for ( size_t i=0; i<m_GUI->m_LoadedResources.Count(); ++i )
    {
        AutoLoad->AppendText(m_GUI->m_LoadedResources[i]);
        AutoLoad->AppendText(_T("\n"));
    }

    MainRes->Append(_("-- None --"));
    wxArrayString Resources;
    m_GUI->EnumerateMainResources(Resources);
    MainRes->Append(Resources);
    if ( m_GUI->m_MainResource.empty() )
    {
        MainRes->SetValue(_("-- None --"));
    }
    else
    {
        MainRes->SetValue(m_GUI->m_MainResource);
    }

    InitAll->SetValue(m_GUI->m_CallInitAll);
    InitAllNecessary->Enable(m_GUI->m_CallInitAll);
    InitAllNecessary->SetValue(m_GUI->m_CallInitAllNecessary);
}

wxWidgetsGUIConfigPanel::~wxWidgetsGUIConfigPanel()
{
}


void wxWidgetsGUIConfigPanel::OnInitAllChange(wxCommandEvent& event)
{
    InitAllNecessary->Enable(InitAll->GetValue());
}

void wxWidgetsGUIConfigPanel::OnButton1Click(wxCommandEvent& event)
{
    wxString FileName = ::wxFileSelector(
        _("Select resource file"),
        _T(""),_T(""),_T(""),
        _("XRC files (*.xrc)|*.xrc|"
          "Zipped files (*.zip)|*.zip|"
          "All files (*)|*"),
        wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY);

    if ( !FileName.empty() )
    {
        wxFileName FN(FileName);
        FN.MakeRelativeTo(m_GUI->GetProjectPath());
        wxString Ext = FN.GetExt();
        FileName = FN.GetFullPath();
        if ( Ext == _T("zip") || Ext == _T("ZIP") )
        {
            FileName.Append(_T("#zip:*.xrc"));
        }
        wxString CurrentContent = AutoLoad->GetValue();
        if ( CurrentContent.Length() && ( CurrentContent[CurrentContent.Length()-1] != _T('\n') ) )
        {
            AutoLoad->AppendText(_T("\n"));
        }
        AutoLoad->AppendText(FileName);
        AutoLoad->AppendText(_T("\n"));
    }
}

void wxWidgetsGUIConfigPanel::OnButton2Click(wxCommandEvent& event)
{
    AutoLoad->Clear();
}

wxString wxWidgetsGUIConfigPanel::GetTitle() const
{
    return _T("wxWidgets GUI Configuration");
}

wxString wxWidgetsGUIConfigPanel::GetBitmapBaseName() const
{
    return wxEmptyString;
}

void wxWidgetsGUIConfigPanel::OnApply()
{
    wxStringTokenizer Tokens(AutoLoad->GetValue(),_T("\n"));
    m_GUI->m_LoadedResources.Clear();
    while ( Tokens.HasMoreTokens() )
    {
        m_GUI->m_LoadedResources.Add(Tokens.GetNextToken());
    }
    m_GUI->m_MainResource = MainRes->GetValue();
    m_GUI->m_CallInitAll = InitAll->GetValue();
    m_GUI->m_CallInitAllNecessary = m_GUI->m_CallInitAll && InitAllNecessary->GetValue();
    m_GUI->NotifyChange();
    m_GUI->OnRebuildApplicationCode();
}

void wxWidgetsGUIConfigPanel::OnCancel()
{
}
