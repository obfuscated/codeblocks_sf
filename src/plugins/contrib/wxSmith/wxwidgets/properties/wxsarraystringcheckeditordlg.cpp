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

BEGIN_EVENT_TABLE(wxsArrayStringCheckEditorDlg,wxDialog)
	//(*EventTable(wxsArrayStringCheckEditorDlg)
	EVT_TEXT_ENTER(ID_TEXTCTRL1,wxsArrayStringCheckEditorDlg::OnButton1Click)
	EVT_BUTTON(ID_BUTTON1,wxsArrayStringCheckEditorDlg::OnButton1Click)
	EVT_CHECKLISTBOX(ID_CHECKLISTBOX1,wxsArrayStringCheckEditorDlg::OnStringListToggled)
	EVT_BUTTON(ID_BUTTON2,wxsArrayStringCheckEditorDlg::OnButton2Click)
	EVT_BUTTON(ID_BUTTON4,wxsArrayStringCheckEditorDlg::OnButton4Click)
	EVT_BUTTON(ID_BUTTON3,wxsArrayStringCheckEditorDlg::OnButton3Click)
	EVT_BUTTON(ID_BUTTON5,wxsArrayStringCheckEditorDlg::OnButton5Click)
	EVT_BUTTON(ID_BUTTON6,wxsArrayStringCheckEditorDlg::OnButton6Click)
	EVT_BUTTON(ID_BUTTON7,wxsArrayStringCheckEditorDlg::OnButton7Click)
	//*)
	EVT_LISTBOX(ID_CHECKLISTBOX1,wxsArrayStringCheckEditorDlg::OnStringListToggled)
END_EVENT_TABLE()

wxsArrayStringCheckEditorDlg::wxsArrayStringCheckEditorDlg(wxWindow* parent,wxArrayString& _Strings,wxArrayBool& _Bools,wxWindowID id):
    Strings(_Strings),
    Bools(_Bools)
{
	//(*Initialize(wxsArrayStringCheckEditorDlg)
	Create(parent,id,_("Choices:"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	EditArea = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) EditArea->SetMaxLength(0);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button1->SetDefault();
	BoxSizer2->Add(EditArea,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(Button1,0,wxALL|wxALIGN_CENTER,5);
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StringList = new wxCheckListBox(this,ID_CHECKLISTBOX1,wxDefaultPosition,wxSize(180,255),0,NULL,0);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	Button2 = new wxButton(this,ID_BUTTON2,_("Update"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	Button4 = new wxButton(this,ID_BUTTON4,_("Remove"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button4->SetDefault();
	Button3 = new wxButton(this,ID_BUTTON3,_("Up"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button3->SetDefault();
	Button5 = new wxButton(this,ID_BUTTON5,_("Down"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button5->SetDefault();
	BoxSizer4->Add(Button2,0,wxALL|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button4,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button5,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer3->Add(StringList,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(BoxSizer4,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP,0);
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxSize(10,-1),0);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	Button6 = new wxButton(this,ID_BUTTON6,_("OK"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button6->SetDefault();
	Button7 = new wxButton(this,ID_BUTTON7,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button7->SetDefault();
	BoxSizer5->Add(Button6,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer5->Add(Button7,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer1->Add(BoxSizer2,0,wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer1->Add(StaticLine1,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer3,1,wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer1->Add(StaticLine2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer5,0,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,0);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
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
    if ( Sel >= StringList->GetCount()-1 ) return;
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
