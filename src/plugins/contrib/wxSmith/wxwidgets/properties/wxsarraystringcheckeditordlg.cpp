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

#include "wxsarraystringcheckeditordlg.h"

//(*InternalHeaders(wxsArrayStringCheckEditorDlg)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(wxsArrayStringCheckEditorDlg)
const long wxsArrayStringCheckEditorDlg::ID_TEXTCTRL1 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON1 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_STATICLINE1 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_CHECKLISTBOX1 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON2 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON4 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON3 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON5 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_STATICLINE2 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON6 = wxNewId();
const long wxsArrayStringCheckEditorDlg::ID_BUTTON7 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsArrayStringCheckEditorDlg,wxDialog)
	//(*EventTable(wxsArrayStringCheckEditorDlg)
	//*)
	EVT_LISTBOX(ID_CHECKLISTBOX1,wxsArrayStringCheckEditorDlg::OnStringListToggled)
END_EVENT_TABLE()

wxsArrayStringCheckEditorDlg::wxsArrayStringCheckEditorDlg(wxWindow* parent,wxArrayString& _Strings,wxArrayBool& _Bools,wxWindowID id):
    Strings(_Strings),
    Bools(_Bools)
{
	//(*Initialize(wxsArrayStringCheckEditorDlg)
	Create(parent,id,_("Choices:"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	EditArea = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
	BoxSizer2->Add(EditArea,1,wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON1"));
	BoxSizer2->Add(Button1,0,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer1->Add(BoxSizer2,0,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,0);
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),wxLI_HORIZONTAL,_T("ID_STATICLINE1"));
	BoxSizer1->Add(StaticLine1,0,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StringList = new wxCheckListBox(this,ID_CHECKLISTBOX1,wxDefaultPosition,wxSize(180,255),0,NULL,0,wxDefaultValidator,_T("ID_CHECKLISTBOX1"));
	BoxSizer3->Add(StringList,1,wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	Button2 = new wxButton(this,ID_BUTTON2,_("Update"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON2"));
	BoxSizer4->Add(Button2,0,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Button4 = new wxButton(this,ID_BUTTON4,_("Remove"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON4"));
	BoxSizer4->Add(Button4,0,wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Button3 = new wxButton(this,ID_BUTTON3,_("Up"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON3"));
	BoxSizer4->Add(Button3,0,wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Button5 = new wxButton(this,ID_BUTTON5,_("Down"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON5"));
	BoxSizer4->Add(Button5,0,wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer3->Add(BoxSizer4,0,wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL,0);
	BoxSizer1->Add(BoxSizer3,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,0);
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxSize(10,-1),wxLI_HORIZONTAL,_T("ID_STATICLINE2"));
	BoxSizer1->Add(StaticLine2,0,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	Button6 = new wxButton(this,ID_BUTTON6,_("OK"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON6"));
	BoxSizer5->Add(Button6,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Button7 = new wxButton(this,ID_BUTTON7,_("Cancel"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON7"));
	BoxSizer5->Add(Button7,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer1->Add(BoxSizer5,0,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,0);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Centre();
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton1Click);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton1Click);
	Connect(ID_CHECKLISTBOX1,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnStringListToggled);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton2Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton4Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton3Click);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton5Click);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton6Click);
	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringCheckEditorDlg::OnButton7Click);
	//*)

	for ( size_t i = 0; i<Strings.Count(); i++ )
	{
	    StringList->Append(Strings[i]);
	    StringList->Check(i,Bools[i]);
    }
}

wxsArrayStringCheckEditorDlg::~wxsArrayStringCheckEditorDlg()
{
}


void wxsArrayStringCheckEditorDlg::OnButton1Click(wxCommandEvent& event)
{
    StringList->Append(EditArea->GetValue());
}

void wxsArrayStringCheckEditorDlg::OnButton2Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    StringList->SetString(Sel,EditArea->GetValue());
}

void wxsArrayStringCheckEditorDlg::OnButton4Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    StringList->Delete(Sel);
}

void wxsArrayStringCheckEditorDlg::OnButton3Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    if ( Sel < 1 ) return;
    bool Checked = StringList->IsChecked(Sel);
    wxString Str = StringList->GetString(Sel);
    StringList->Delete(Sel);
    StringList->Insert(Str,--Sel);
    StringList->Check(Sel,Checked);
    StringList->SetSelection(Sel);
}

void wxsArrayStringCheckEditorDlg::OnButton5Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    if ( Sel >= (int)StringList->GetCount()-1 ) return;
    bool Checked = StringList->IsChecked(Sel);
    wxString Str = StringList->GetString(Sel);
    StringList->Delete(Sel);
    StringList->Insert(Str,++Sel);
    StringList->Check(Sel,Checked);
    StringList->SetSelection(Sel);
}

void wxsArrayStringCheckEditorDlg::OnButton6Click(wxCommandEvent& event)
{
    size_t Count = StringList->GetCount();
    Strings.Clear();
    Bools.Clear();
    for ( size_t i=0; i<Count; i++ )
    {
        Strings.Add(StringList->GetString(i));
        Bools.Add(StringList->IsChecked(i));
    }
    EndModal(wxID_OK);
}

void wxsArrayStringCheckEditorDlg::OnButton7Click(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void wxsArrayStringCheckEditorDlg::OnStringListToggled(wxCommandEvent& event)
{
    EditArea->SetValue(StringList->GetStringSelection());
}
