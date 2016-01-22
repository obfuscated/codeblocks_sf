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

#include "wxsbitmapiconeditordlg.h"

#include <wx/dcmemory.h>
#include <wx/filedlg.h>

//(*InternalHeaders(wxsBitmapIconEditorDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

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
    _T("wxART_FILE_SAVE"),
    _T("wxART_FILE_SAVE_AS"),
    _T("wxART_PRINT"),
    _T("wxART_HELP"),
    _T("wxART_TIP"),
    _T("wxART_REPORT_VIEW"),
    _T("wxART_LIST_VIEW"),
    _T("wxART_NEW_DIR"),
    _T("wxART_HARDDISK"),
    _T("wxART_FLOPPY"),
    _T("wxART_CDROM"),
    _T("wxART_REMOVABLE"),
    _T("wxART_FOLDER"),
    _T("wxART_FOLDER_OPEN"),
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
    _T("wxART_COPY"),
    _T("wxART_CUT"),
    _T("wxART_PASTE"),
    _T("wxART_DELETE"),
    _T("wxART_NEW"),
    _T("wxART_UNDO"),
    _T("wxART_REDO"),
    _T("wxART_QUIT"),
    _T("wxART_FIND"),
    _T("wxART_FIND_AND_REPLACE"),
    0
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
    0
};


//(*IdInit(wxsBitmapIconEditorDlg)
const long wxsBitmapIconEditorDlg::ID_RADIOBUTTON1 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_RADIOBUTTON2 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_TEXTCTRL1 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_BUTTON3 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_RADIOBUTTON3 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_STATICTEXT1 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_COMBOBOX2 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_STATICTEXT2 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_COMBOBOX1 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_RADIOBUTTON4 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_TEXTCTRL2 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_STATICBITMAP1 = wxNewId();
const long wxsBitmapIconEditorDlg::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsBitmapIconEditorDlg,wxScrollingDialog)
    //(*EventTable(wxsBitmapIconEditorDlg)
    //*)
    EVT_TIMER(-1,wxsBitmapIconEditorDlg::OnTimer)
END_EVENT_TABLE()

wxsBitmapIconEditorDlg::wxsBitmapIconEditorDlg(wxWindow* parent,wxsBitmapIconData& _Data,const wxString& _DefaultClient):
    Timer1(0),
    DefaultClient(_DefaultClient),
    Data(_Data)
{
    //(*Initialize(wxsBitmapIconEditorDlg)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, _("Image editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Image options"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    NoImage = new wxRadioButton(this, ID_RADIOBUTTON1, _("No image"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
    FlexGridSizer1->Add(NoImage, 0, wxEXPAND, 5);
    ImageFile = new wxRadioButton(this, ID_RADIOBUTTON2, _("Image From File:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
    FlexGridSizer1->Add(ImageFile, 0, wxTOP|wxEXPAND, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2->Add(20,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    FileName = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer2->Add(FileName, 1, wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(this, ID_BUTTON3, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer2->Add(Button3, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer2, 0, wxTOP|wxEXPAND, 5);
    ImageArt = new wxRadioButton(this, ID_RADIOBUTTON3, _("Image from wxArtProvider:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
    FlexGridSizer1->Add(ImageArt, 0, wxTOP|wxALIGN_TOP, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer4->Add(20,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Art Id:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer2->Add(StaticText1, 0, wxALIGN_CENTER_VERTICAL, 5);
    ArtId = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_SORT, wxDefaultValidator, _T("ID_COMBOBOX2"));
    FlexGridSizer2->Add(ArtId, 0, wxLEFT|wxEXPAND, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Art Client:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer2->Add(StaticText2, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    ArtClient = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_SORT, wxDefaultValidator, _T("ID_COMBOBOX1"));
    FlexGridSizer2->Add(ArtClient, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    BoxSizer4->Add(FlexGridSizer2, 1, wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer4, 0, wxTOP|wxEXPAND, 5);
    ImageCode = new wxRadioButton(this, ID_RADIOBUTTON4, _("Code"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON4"));
    FlexGridSizer1->Add(ImageCode, 0, wxTOP|wxEXPAND, 5);
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer7->Add(20,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    CodeText = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer7->Add(CodeText, 1, wxEXPAND, 5);
    FlexGridSizer1->Add(BoxSizer7, 1, wxTOP|wxEXPAND, 5);
    StaticBoxSizer1->Add(FlexGridSizer1, 1, wxEXPAND, 5);
    BoxSizer5->Add(StaticBoxSizer1, 0, wxEXPAND, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Preview"));
    Preview = new wxStaticBitmap(this, ID_STATICBITMAP1, wxNullBitmap, wxDefaultPosition, wxSize(200,200), wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    StaticBoxSizer2->Add(Preview, 1, wxEXPAND, 5);
    BoxSizer5->Add(StaticBoxSizer2, 1, wxLEFT|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer5, 1, wxEXPAND, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    Button1->SetDefault();
    BoxSizer3->Add(Button1, 0, wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
    BoxSizer3->Add(Button2, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_RADIOBUTTON1,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnUpdatePreview));
    Connect(ID_RADIOBUTTON2,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnUpdatePreview));
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnFileNameText));
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnButton3Click));
    Connect(ID_RADIOBUTTON3,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnUpdatePreview));
    Connect(ID_COMBOBOX2,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnArtIdSelect));
    Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnArtIdSelect));
    Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnCodeTextText));
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsBitmapIconEditorDlg::OnButton1Click));
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
    //(*Destroy(wxsBitmapIconEditorDlg)
    //*)
}

void wxsBitmapIconEditorDlg::OnTimer(cb_unused wxTimerEvent& event)
{
    wxsBitmapIconData PreviewData;
    WriteData(PreviewData);
    wxSize PrevSize = Preview->GetSize();
    wxBitmap Tmp(PrevSize.GetWidth(),PrevSize.GetHeight());
    wxBitmap PreviewBmp = PreviewData.GetPreview(wxDefaultSize,DefaultClient);
    wxMemoryDC DC;
    DC.SelectObject(Tmp);
    DC.SetBrush(wxColour(0xC0,0xC0,0xC0));
    DC.SetPen(wxColour(0xC0,0xC0,0xC0));
    DC.DrawRectangle(0,0,PrevSize.GetWidth(),PrevSize.GetHeight());

    if ( PreviewBmp.Ok() )
    {
        wxSize BmpSize(PreviewBmp.GetWidth(),PreviewBmp.GetHeight());
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

void wxsBitmapIconEditorDlg::WriteData(wxsBitmapIconData& _Data)
{
    if ( NoImage->GetValue() )
    {
        _Data.Id.Clear();
        _Data.Client.Clear();
        _Data.CodeText.Clear();
        _Data.FileName.Clear();
    }
    else if ( ImageFile->GetValue() )
    {
        _Data.Id.Clear();
        _Data.Client.Clear();
        _Data.CodeText.Clear();
        _Data.FileName = FileName->GetValue();
    }
    else if ( ImageCode->GetValue() )
    {
        _Data.Id.Clear();
        _Data.Client.Clear();
        _Data.CodeText = CodeText->GetValue();
        _Data.FileName.Clear();
    }
    else
    {
        _Data.Id = ArtId->GetValue();
        _Data.Client = Data.Id.empty() ? _T("") : ArtClient->GetValue();
        _Data.CodeText.Clear();
        _Data.FileName.Clear();
    }
}

void wxsBitmapIconEditorDlg::ReadData(wxsBitmapIconData& _Data)
{
    if ( _Data.Id.empty() )
    {
        if ( _Data.FileName.empty() && _Data.CodeText.empty() )
        {
            NoImage->SetValue(true);
        }
        else if( !_Data.CodeText.empty() )
        {
            ImageCode->SetValue(true);
            CodeText->SetValue(_Data.CodeText);
        }
        else
        {
            ImageFile->SetValue(true);
            FileName->SetValue(_Data.FileName);
        }
    }
    else
    {
        ImageArt->SetValue(true);
        ArtId->SetValue(_Data.Id);
        ArtClient->SetValue(_Data.Client);
    }
}

void wxsBitmapIconEditorDlg::OnButton1Click(cb_unused wxCommandEvent& event)
{
    WriteData(Data);
    EndModal(wxID_OK);
}

void wxsBitmapIconEditorDlg::OnFileNameText(cb_unused wxCommandEvent& event)
{
    ImageFile->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnButton3Click(cb_unused wxCommandEvent& event)
{
    wxString FileNameStr = ::wxFileSelector(_("Choose image file"));
    if ( FileNameStr.empty() ) return;
    FileName->SetValue(FileNameStr);
    ImageFile->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnArtIdSelect(cb_unused wxCommandEvent& event)
{
    ImageArt->SetValue(true);
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnUpdatePreview(cb_unused wxCommandEvent& event)
{
    UpdatePreview();
}

void wxsBitmapIconEditorDlg::OnCodeTextText(cb_unused wxCommandEvent& event)
{
    UpdatePreview();
}
