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
#include "wxssimplefonteditordlg.h"
#include "wxsfonteditordlg.h"

#include <wx/fontdlg.h>

BEGIN_EVENT_TABLE(wxsSimpleFontEditorDlg,wxDialog)
	//(*EventTable(wxsSimpleFontEditorDlg)
	EVT_BUTTON(ID_BUTTON1,wxsSimpleFontEditorDlg::OnButton1Click)
	EVT_BUTTON(ID_BUTTON3,wxsSimpleFontEditorDlg::OnButton3Click)
	EVT_BUTTON(ID_BUTTON2,wxsSimpleFontEditorDlg::OnButton2Click)
	//*)
	EVT_BUTTON(wxID_OK,wxsSimpleFontEditorDlg::OnOK)
END_EVENT_TABLE()

wxsSimpleFontEditorDlg::wxsSimpleFontEditorDlg(wxWindow* parent,wxsFontData& Data,wxWindowID id):
    m_Data(Data)
{
	//(*Initialize(wxsSimpleFontEditorDlg)
	Create(parent,id,_("Font settings"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T(""));
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Current font"));
	FlexGridSizer2 = new wxFlexGridSizer(0,0,0,0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(0);
	FontDescription = new wxStaticText(this,ID_STATICTEXT1,_("-- None --"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	FlexGridSizer2->Add(FontDescription,0,wxALL|wxALIGN_CENTER,5);
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxDefaultSize,wxLI_VERTICAL,_("ID_STATICLINE2"));
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("Change"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON1"));
	if (false) Button1->SetDefault();
	Button3 = new wxButton(this,ID_BUTTON3,_("Clear"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON3"));
	if (false) Button3->SetDefault();
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),0,_("ID_STATICLINE1"));
	Button2 = new wxButton(this,ID_BUTTON2,_("Advanced"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) Button2->SetDefault();
	BoxSizer2->Add(Button1,0,wxALIGN_CENTER,5);
	BoxSizer2->Add(Button3,1,wxTOP|wxALIGN_CENTER,5);
	BoxSizer2->Add(StaticLine1,0,wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(Button2,0,wxTOP|wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(FlexGridSizer2,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(StaticLine2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer1->Add(BoxSizer2,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Test area"));
	TestArea = new wxTextCtrl(this,ID_TEXTCTRL1,_("This is sample text"),wxDefaultPosition,wxSize(275,71),wxTE_MULTILINE,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) TestArea->SetMaxLength(0);
	StaticBoxSizer2->Add(TestArea,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Note:"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE,_("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("In order to provide best cross-platform compatibility\nyou should use either system-based font or multiple \nface names. You can change it in advanced options."),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT3"));
	BoxSizer3->Add(StaticText2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer3->Add(StaticText3,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_OK,_T("")));
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,_T("")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(BoxSizer4,1,wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(BoxSizer3,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,4);
	FlexGridSizer1->Add(BoxSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	this->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Center();
	//*)

    m_WorkingCopy = m_Data;
	UpdateFontDescription();
}

wxsSimpleFontEditorDlg::~wxsSimpleFontEditorDlg()
{
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
    wxFont Font = ::wxGetFontFromUser(NULL,m_WorkingCopy.BuildFont());
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
