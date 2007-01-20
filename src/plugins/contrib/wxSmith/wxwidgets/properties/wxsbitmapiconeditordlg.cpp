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

#include "wxsbitmapiconeditordlg.h"

#include <wx/dcmemory.h>
#include <wx/filedlg.h>

#define TIMER_DELAY    250

static const wxChar* PredefinedIds[] =
{
    _T("wxART_ADD_BOOKMARK"),
    _T("wxART_DEL_BOOKMARK"),
    _T("wxART_HELP_SIDE_PANEL"),
    _T("wxART_HELP_SETTINGS"),
    _T("wxART_HELP_BOOK"),
    _T("wxART_HELP_FOLDER"),
    _T("wxART_HELP_PAGE"),
    _T("wxART_GO_BACK"),
    _T("wxART_GO_FORWARD"),
    _T("wxART_GO_UP"),
    _T("wxART_GO_DOWN"),
    _T("wxART_GO_TO_PARENT"),
    _T("wxART_GO_HOME"),
    _T("wxART_FILE_OPEN"),
    _T("wxART_PRINT"),
    _T("wxART_HELP"),
    _T("wxART_TIP"),
    _T("wxART_REPORT_VIEW"),
    _T("wxART_LIST_VIEW"),
    _T("wxART_NEW_DIR"),
    _T("wxART_FOLDER"),
    _T("wxART_GO_DIR_UP"),
    _T("wxART_EXECUTABLE_FILE"),
    _T("wxART_NORMAL_FILE"),
    _T("wxART_TICK_MARK"),
    _T("wxART_CROSS_MARK"),
    _T("wxART_ERROR"),
    _T("wxART_QUESTION"),
    _T("wxART_WARNING"),
    _T("wxART_INFORMATION"),
    _T("wxART_MISSING_IMAGE"),
    NULL
};

static const wxChar* PredefinedClients[] =
{
    _T("wxART_TOOLBAR"),
    _T("wxART_MENU"),
    _T("wxART_BUTTON"),
    _T("wxART_FRAME_ICON"),
    _T("wxART_CMN_DIALOG"),
    _T("wxART_HELP_BROWSER"),
    _T("wxART_MESSAGE_BOX"),
    _T("wxART_OTHER"),
    NULL
};


BEGIN_EVENT_TABLE(wxsBitmapIconEditorDlg,wxDialog)
	//(*EventTable(wxsBitmapIconEditorDlg)
	EVT_RADIOBUTTON(ID_RADIOBUTTON1,wxsBitmapIconEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON2,wxsBitmapIconEditorDlg::OnUpdatePreview)
	EVT_TEXT(ID_TEXTCTRL1,wxsBitmapIconEditorDlg::OnFileNameText)
	EVT_BUTTON(ID_BUTTON3,wxsBitmapIconEditorDlg::OnButton3Click)
	EVT_RADIOBUTTON(ID_RADIOBUTTON3,wxsBitmapIconEditorDlg::OnUpdatePreview)
	EVT_COMBOBOX(ID_COMBOBOX2,wxsBitmapIconEditorDlg::OnArtIdSelect)
	EVT_COMBOBOX(ID_COMBOBOX1,wxsBitmapIconEditorDlg::OnArtIdSelect)
	EVT_BUTTON(ID_BUTTON1,wxsBitmapIconEditorDlg::OnButton1Click)
	//*)
	EVT_TIMER(-1,wxsBitmapIconEditorDlg::OnTimer)
END_EVENT_TABLE()

wxsBitmapIconEditorDlg::wxsBitmapIconEditorDlg(wxWindow* parent,wxsBitmapIconData& _Data,const wxString& _DefaultClient,wxWindowID id):
    Timer1(NULL),
    DefaultClient(_DefaultClient),
    Data(_Data)
{
	//(*Initialize(wxsBitmapIconEditorDlg)
	Create(parent,id,_("Image editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxRESIZE_BOX);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Image options"));
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	FlexGridSizer1->AddGrowableCol(1);
	NoImage = new wxRadioButton(this,ID_RADIOBUTTON1,_("No image"),wxDefaultPosition,wxDefaultSize,0);
	NoImage->SetValue(true);
	ImageFile = new wxRadioButton(this,ID_RADIOBUTTON2,_("Image From File:"),wxDefaultPosition,wxDefaultSize,0);
	ImageFile->SetValue(false);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	FileName = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) FileName->SetMaxLength(0);
	Button3 = new wxButton(this,ID_BUTTON3,_("..."),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) Button3->SetDefault();
	BoxSizer2->Add(20,1,0);
	BoxSizer2->Add(FileName,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer2->Add(Button3,0,wxRIGHT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	ImageArt = new wxRadioButton(this,ID_RADIOBUTTON3,_("Image from wxArtProvider:"),wxDefaultPosition,wxDefaultSize,0);
	ImageArt->SetValue(false);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Art Id:"),wxDefaultPosition,wxDefaultSize,0);
	ArtId = new wxComboBox(this,ID_COMBOBOX2,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,0);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Art Client:"),wxDefaultPosition,wxDefaultSize,0);
	ArtClient = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,0);
	FlexGridSizer2->Add(StaticText1,1,wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(ArtId,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(StaticText2,1,wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(ArtClient,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(20,1,0);
	BoxSizer4->Add(FlexGridSizer2,1,wxALIGN_CENTER,5);
	FlexGridSizer1->Add(NoImage,1,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
	FlexGridSizer1->Add(1,1,1);
	FlexGridSizer1->Add(ImageFile,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(BoxSizer2,1,wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(ImageArt,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP,5);
	FlexGridSizer1->Add(BoxSizer4,1,wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(FlexGridSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Preview"));
	Preview = new wxStaticBitmap(this,ID_STATICBITMAP1,wxBitmap(),wxDefaultPosition,wxSize(200,200),0);
	StaticBoxSizer2->Add(Preview,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer5->Add(StaticBoxSizer1,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer5->Add(StaticBoxSizer2,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("OK"),wxDefaultPosition,wxDefaultSize,0);
	if (true) Button1->SetDefault();
	Button2 = new wxButton(this,wxID_CANCEL,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	BoxSizer3->Add(Button1,0,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer3->Add(Button2,0,wxALL|wxALIGN_CENTER,5);
	BoxSizer1->Add(BoxSizer5,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer3,0,wxALL|wxALIGN_CENTER,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	for ( const wxChar** Ptr = PredefinedIds; *Ptr; Ptr++ )
	{
	    ArtId->Append(*Ptr);
	}

	for ( const wxChar** Ptr = PredefinedClients; *Ptr; Ptr++ )
	{
	    ArtClient->Append(*Ptr);
	}

	Timer1 = new wxTimer(this);

    ReadData(Data);
    UpdatePreview();

	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
}

wxsBitmapIconEditorDlg::~wxsBitmapIconEditorDlg()
{
}

void wxsBitmapIconEditorDlg::OnTimer(wxTimerEvent& event)
{
    wxsBitmapIconData PreviewData;
    WriteData(PreviewData);
    wxSize PrevSize = Preview->GetSize();
    wxBitmap Tmp(PrevSize.GetWidth(),PrevSize.GetHeight());
    wxBitmap PreviewBmp = PreviewData.GetPreview(wxDefaultSize,DefaultClient);
    wxSize BmpSize(PreviewBmp.GetWidth(),PreviewBmp.GetHeight());
    wxMemoryDC DC;
    DC.SelectObject(Tmp);
    DC.SetBrush(wxColour(0xC0,0xC0,0xC0));
    DC.SetPen(wxColour(0xC0,0xC0,0xC0));
    DC.DrawRectangle(0,0,PrevSize.GetWidth(),PrevSize.GetHeight());

    if ( PreviewBmp.Ok() )
    {
        int X = (PrevSize.GetWidth()  - BmpSize.GetWidth() ) / 2;
        int Y = (PrevSize.GetHeight() - BmpSize.GetHeight()) / 2;
        if ( X < 0 ) X = 0;
        if ( Y < 0 ) Y = 0;
        DC.DrawBitmap(PreviewBmp,X,Y,true);
    }
    Preview->SetBitmap(Tmp);
    Preview->Refresh();
}

void wxsBitmapIconEditorDlg::UpdatePreview()
{
    if ( Timer1 )
    {
        Timer1->Start(TIMER_DELAY,true);
    }
}

void wxsBitmapIconEditorDlg::WriteData(wxsBitmapIconData& Data)
{
    if ( NoImage->GetValue() )
    {
        Data.Id.Clear();
        Data.Client.Clear();
        Data.FileName.Clear();
    }
    else if ( ImageFile->GetValue() )
    {
        Data.Id.Clear();
        Data.Client.Clear();
        Data.FileName = FileName->GetValue();
    }
    else
    {
        Data.Id = ArtId->GetValue();
        Data.Client = Data.Id.empty() ? _T("") : ArtClient->GetValue();
        Data.FileName.Clear();
    }
}

void wxsBitmapIconEditorDlg::ReadData(wxsBitmapIconData& Data)
{
    if ( Data.Id.empty() )
    {
        if ( Data.FileName.empty() )
        {
            NoImage->SetValue(true);
        }
        else
        {
            ImageFile->SetValue(true);
            FileName->SetValue(Data.FileName);
        }
    }
    else
    {
        ImageArt->SetValue(true);
        ArtId->SetValue(Data.Id);
        ArtClient->SetValue(Data.Client);
    }
}

void wxsBitmapIconEditorDlg::OnButton1Click(wxCommandEvent& event)
{
    WriteData(Data);
    EndModal(wxID_OK);
}

void wxsBitmapIconEditorDlg::OnFileNameText(wxCommandEvent& event)
{
    ImageFile->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnButton3Click(wxCommandEvent& event)
{
    wxString FileNameStr = ::wxFileSelector(_("Choose image file"));
    if ( FileNameStr.empty() ) return;
    FileName->SetValue(FileNameStr);
    ImageFile->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnArtIdSelect(wxCommandEvent& event)
{
    ImageArt->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnUpdatePreview(wxCommandEvent& event)
{
    UpdatePreview();
}
