/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxssimplefonteditordlg.h"
#include "wxsfonteditordlg.h"

#include <wx/fontdlg.h>

//(*InternalHeaders(wxsSimpleFontEditorDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxsSimpleFontEditorDlg)
const long wxsSimpleFontEditorDlg::ID_STATICTEXT1 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_STATICLINE2 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_BUTTON1 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_BUTTON3 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_STATICLINE1 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_BUTTON2 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_TEXTCTRL1 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_STATICTEXT2 = wxNewId();
const long wxsSimpleFontEditorDlg::ID_STATICTEXT3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsSimpleFontEditorDlg,wxDialog)
	//(*EventTable(wxsSimpleFontEditorDlg)
	//*)
	EVT_BUTTON(wxID_OK,wxsSimpleFontEditorDlg::OnOK)
END_EVENT_TABLE()

wxsSimpleFontEditorDlg::wxsSimpleFontEditorDlg(wxWindow* parent,wxsFontData& Data,wxWindowID id):
    m_Data(Data)
{
	//(*Initialize(wxsSimpleFontEditorDlg)
	Create(parent, id, _("Font settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Current font"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 0, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(0);
	FontDescription = new wxStaticText(this, ID_STATICTEXT1, _("-- None --"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(FontDescription, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL, _T("ID_STATICLINE2"));
	StaticBoxSizer1->Add(StaticLine2, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this, ID_BUTTON1, _("Change"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button3 = new wxButton(this, ID_BUTTON3, _("Clear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer2->Add(Button3, 1, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	BoxSizer2->Add(StaticLine1, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Advanced"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer2->Add(Button2, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(StaticBoxSizer1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Test area"));
	TestArea = new wxTextCtrl(this, ID_TEXTCTRL1, _("This is sample text"), wxDefaultPosition, wxSize(275,71), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer2->Add(TestArea, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Note:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
	BoxSizer3->Add(StaticText2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("In order to provide best cross-platform compatibility\nyou should use either system-based font or multiple \nface names. You can change it in advanced options."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer3->Add(StaticText3, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	FlexGridSizer1->Add(BoxSizer3, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Center();

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsSimpleFontEditorDlg::OnButton1Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsSimpleFontEditorDlg::OnButton3Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsSimpleFontEditorDlg::OnButton2Click);
	//*)

    m_WorkingCopy = m_Data;
	UpdateFontDescription();
}

wxsSimpleFontEditorDlg::~wxsSimpleFontEditorDlg()
{
    //(*Destroy(wxsSimpleFontEditorDlg)
    //*)
}

void wxsSimpleFontEditorDlg::UpdateFontDescription()
{
    wxString Description = _("-- None --");
    if ( !m_WorkingCopy.IsDefault )
    {
        Description.Clear();
        if ( m_WorkingCopy.HasSysFont )
        {
            Description << wxString::Format(_("System-based (%s)\n"),m_WorkingCopy.SysFont.c_str());
        }
        else if ( m_WorkingCopy.Faces.Count() > 0 )
        {
            Description << wxString::Format(_("Face: %s%s\n"),m_WorkingCopy.Faces[0].c_str(),(m_WorkingCopy.Faces.Count()>1)?_T(", ..."):_T(""));
        }

        if ( m_WorkingCopy.HasSize )
        {
            Description << wxString::Format(_("Size: %d\n"),m_WorkingCopy.Size);
        }
        else if ( m_WorkingCopy.HasRelativeSize && m_WorkingCopy.HasSysFont )
        {
            Description << wxString::Format(_("Relative size: %.2f"),m_WorkingCopy.RelativeSize);
        }

        if ( m_WorkingCopy.HasWeight )
        {
            Description << _("Weight: ");
            switch ( m_WorkingCopy.Weight )
            {
                case wxFONTWEIGHT_BOLD:  Description << _("Bold\n"); break;
                case wxFONTWEIGHT_LIGHT: Description << _("Light\n"); break;
                default:                 Description << _("Normal\n");
            }
        }

        if ( m_WorkingCopy.HasStyle )
        {
            Description << _("Style: ");
            switch ( m_WorkingCopy.Style )
            {
                case wxFONTSTYLE_ITALIC: Description << _("Italic\n"); break;
                case wxFONTSTYLE_SLANT:  Description << _("Slant\n"); break;
                default:                 Description << _("Normal\n");
            }
        }

        if ( m_WorkingCopy.HasUnderlined )
        {
            Description << _("Underlined: ");
            if ( m_WorkingCopy.Underlined ) Description << _("Yes\n");
            else                            Description << _("No\n");
        }
    }

    FontDescription->SetLabel(Description);
    Layout();
    GetSizer()->SetSizeHints(this);
    TestArea->SetFont(m_WorkingCopy.BuildFont());
}

void wxsSimpleFontEditorDlg::OnOK(wxCommandEvent& event)
{
    m_Data = m_WorkingCopy;
    EndModal(wxID_OK);
}


void wxsSimpleFontEditorDlg::OnButton1Click(wxCommandEvent& event)
{
    wxFont Font = ::wxGetFontFromUser(0,m_WorkingCopy.BuildFont());
    if ( !Font.Ok() ) return;

    m_WorkingCopy.Size = Font.GetPointSize();
    m_WorkingCopy.Style = Font.GetStyle();
    m_WorkingCopy.Weight = Font.GetWeight();
    m_WorkingCopy.Underlined = Font.GetUnderlined();
    m_WorkingCopy.Family = Font.GetFamily();
    m_WorkingCopy.Faces.Clear();
    m_WorkingCopy.Faces.Add(Font.GetFaceName());
    m_WorkingCopy.IsDefault = false;
    m_WorkingCopy.HasSize = true;
    m_WorkingCopy.HasStyle = true;
    m_WorkingCopy.HasWeight = true;
    m_WorkingCopy.HasUnderlined = true;
    m_WorkingCopy.HasFamily = true;
    m_WorkingCopy.HasEncoding = false;
    m_WorkingCopy.HasSysFont = false;
    m_WorkingCopy.HasRelativeSize = false;

    UpdateFontDescription();
}

void wxsSimpleFontEditorDlg::OnButton2Click(wxCommandEvent& event)
{
    wxsFontEditorDlg Dlg(this,m_WorkingCopy);
    Dlg.ShowModal();
    UpdateFontDescription();
}

void wxsSimpleFontEditorDlg::OnButton3Click(wxCommandEvent& event)
{
    m_WorkingCopy.IsDefault = true;
    UpdateFontDescription();
}
