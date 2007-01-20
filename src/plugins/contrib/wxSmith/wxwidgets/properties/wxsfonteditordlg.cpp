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

#include "wxsfonteditordlg.h"
#include "wxsfontfaceeditordlg.h"
#include <wx/fontmap.h>
#include <wx/fontdlg.h>
#include <wx/settings.h>

#define FT_DFAULT   0x00
#define FT_CUSTOM   0x01
#define FT_SYSTEM   0x02

#define FFAM_DECORATIVE   0x00
#define FFAM_ROMAN        0x01
#define FFAM_SCRIPT       0x02
#define FFAM_SWISS        0x03
#define FFAM_MODERN       0x04
#define FFAM_TELETYPE     0x05

BEGIN_EVENT_TABLE(wxsFontEditorDlg,wxDialog)
	//(*EventTable(wxsFontEditorDlg)
	EVT_CHOICE(ID_CHOICE1,wxsFontEditorDlg::OnUpdateContent)
	EVT_CHECKBOX(ID_CHECKBOX8,wxsFontEditorDlg::OnBaseFontUseChange)
	EVT_CHOICE(ID_CHOICE2,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX2,wxsFontEditorDlg::OnUpdateContent)
	EVT_COMBOBOX(ID_COMBOBOX1,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX1,wxsFontEditorDlg::OnUpdateContent)
	EVT_CHOICE(ID_CHOICE3,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX7,wxsFontEditorDlg::OnUpdateContent)
	EVT_SPINCTRL(ID_SPINCTRL1,wxsFontEditorDlg::OnSizeValChange)
	EVT_CHECKBOX(ID_CHECKBOX6,wxsFontEditorDlg::OnUpdateContent)
	EVT_TEXT(ID_TEXTCTRL1,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX3,wxsFontEditorDlg::OnUpdateContent)
	EVT_RADIOBUTTON(ID_RADIOBUTTON1,wxsFontEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON2,wxsFontEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON3,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX4,wxsFontEditorDlg::OnUpdateContent)
	EVT_RADIOBUTTON(ID_RADIOBUTTON4,wxsFontEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON5,wxsFontEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON6,wxsFontEditorDlg::OnUpdatePreview)
	EVT_CHECKBOX(ID_CHECKBOX5,wxsFontEditorDlg::OnUpdateContent)
	EVT_RADIOBUTTON(ID_RADIOBUTTON7,wxsFontEditorDlg::OnUpdatePreview)
	EVT_RADIOBUTTON(ID_RADIOBUTTON8,wxsFontEditorDlg::OnUpdatePreview)
	EVT_LISTBOX(ID_LISTBOX1,wxsFontEditorDlg::OnUpdateContent)
	EVT_BUTTON(ID_BUTTON4,wxsFontEditorDlg::OnFaceAddClick)
	EVT_BUTTON(ID_BUTTON5,wxsFontEditorDlg::OnFaceDelClick)
	EVT_BUTTON(ID_BUTTON6,wxsFontEditorDlg::OnFaceEditClick)
	EVT_BUTTON(ID_BUTTON8,wxsFontEditorDlg::OnFaceUpClick)
	EVT_BUTTON(ID_BUTTON7,wxsFontEditorDlg::OnFaceDownClick)
	//*)
	EVT_BUTTON(wxID_OK,wxsFontEditorDlg::OnButton2Click)
END_EVENT_TABLE()

wxsFontEditorDlg::wxsFontEditorDlg(wxWindow* parent,wxsFontData& _Data,wxWindowID id):
    Data(_Data)
{
    Initialized = false;
	//(*Initialize(wxsFontEditorDlg)
	Create(parent,id,_("Advanced font settings"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Font type"));
	FontType = new wxChoice(this,ID_CHOICE1,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHOICE1"));
	FontType->Append(_("Default font"));
	FontType->Append(_("Custom font"));
	FontType->Append(_("System-based font"));
	FontType->SetSelection(0);
	StaticBoxSizer1->Add(FontType,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Settings"));
	FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
	FlexGridSizer2->AddGrowableCol(2);
	BaseFontSizer = new wxBoxSizer(wxHORIZONTAL);
	BaseFontUse = new wxCheckBox(this,ID_CHECKBOX8,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX8"));
	BaseFontUse->SetValue(true);
	BaseFontUse->Disable();
	BaseFontTxt = new wxStaticText(this,ID_STATICTEXT1,_("Base Font:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	BaseFontSizer->Add(BaseFontUse,0,wxALL|wxALIGN_CENTER,5);
	BaseFontSizer->Add(BaseFontTxt,1,wxRIGHT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	BaseFontVal = new wxChoice(this,ID_CHOICE2,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHOICE2"));
	BaseFontVal->Append(_("wxSYS_OEM_FIXED_FONT"));
	BaseFontVal->Append(_("wxSYS_ANSI_FIXED_FONT"));
	BaseFontVal->Append(_("wxSYS_ANSI_VAR_FONT"));
	BaseFontVal->Append(_("wxSYS_SYSTEM_FONT"));
	BaseFontVal->Append(_("wxSYS_DEVICE_DEFAULT_FONT"));
	BaseFontVal->Append(_("wxSYS_DEFAULT_PALETTE"));
	BaseFontVal->Append(_("wxSYS_SYSTEM_FIXED_FONT"));
	BaseFontVal->Append(_("wxSYS_DEFAULT_GUI_FONT"));
	BaseFontVal->SetSelection(0);
	FamUse = new wxCheckBox(this,ID_CHECKBOX2,_("Family:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX2"));
	FamUse->SetValue(false);
	FamVal = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_COMBOBOX1"));
	FamVal->Append(_("Decorative"));
	FamVal->Append(_("Roman"));
	FamVal->Append(_("Script"));
	FamVal->Append(_("Swiss"));
	FamVal->Append(_("Modern"));
	FamVal->Append(_("Teletype"));
	FamVal->SetSelection(0);
	EncodUse = new wxCheckBox(this,ID_CHECKBOX1,_("Font encoding:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX1"));
	EncodUse->SetValue(false);
	EncodVal = new wxChoice(this,ID_CHOICE3,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHOICE3"));
	SizeUse = new wxCheckBox(this,ID_CHECKBOX7,_("Size:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX7"));
	SizeUse->SetValue(false);
	SizeVal = new wxSpinCtrl(this,ID_SPINCTRL1,_("0"),wxDefaultPosition,wxDefaultSize,0,-1000,1000,0,_("ID_SPINCTRL1"));
	RelSizeUse = new wxCheckBox(this,ID_CHECKBOX6,_("Relative size:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX6"));
	RelSizeUse->SetValue(false);
	RelSizeVal = new wxTextCtrl(this,ID_TEXTCTRL1,_("1.0"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) RelSizeVal->SetMaxLength(0);
	FlexGridSizer2->Add(BaseFontSizer,1,wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(BaseFontVal,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(FamUse,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(FamVal,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(EncodUse,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(EncodVal,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(SizeUse,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(SizeVal,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer2->Add(RelSizeUse,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(RelSizeVal,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer2->Add(FlexGridSizer2,0,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL,this,_T(""));
	StyleUse = new wxCheckBox(this,ID_CHECKBOX3,_("Style"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX3"));
	StyleUse->SetValue(false);
	StyleNorm = new wxRadioButton(this,ID_RADIOBUTTON1,_("Normal"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP,wxDefaultValidator,_("ID_RADIOBUTTON1"));
	StyleNorm->SetValue(true);
	StyleItal = new wxRadioButton(this,ID_RADIOBUTTON2,_("Italic"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_RADIOBUTTON2"));
	StyleItal->SetValue(false);
	StyleSlant = new wxRadioButton(this,ID_RADIOBUTTON3,_("Slant"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_RADIOBUTTON3"));
	StyleSlant->SetValue(false);
	StaticBoxSizer3->Add(StyleUse,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer3->Add(StyleNorm,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer3->Add(StyleItal,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer3->Add(StyleSlant,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL,this,_T(""));
	WeightUse = new wxCheckBox(this,ID_CHECKBOX4,_("Weight"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX4"));
	WeightUse->SetValue(false);
	WeightLight = new wxRadioButton(this,ID_RADIOBUTTON4,_("Light"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP,wxDefaultValidator,_("ID_RADIOBUTTON4"));
	WeightLight->SetValue(false);
	WeightNorm = new wxRadioButton(this,ID_RADIOBUTTON5,_("Normal"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_RADIOBUTTON5"));
	WeightNorm->SetValue(true);
	WeightBold = new wxRadioButton(this,ID_RADIOBUTTON6,_("Bold"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_RADIOBUTTON6"));
	WeightBold->SetValue(false);
	StaticBoxSizer4->Add(WeightUse,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer4->Add(WeightLight,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer4->Add(WeightNorm,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer4->Add(WeightBold,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxVERTICAL,this,_T(""));
	UnderUse = new wxCheckBox(this,ID_CHECKBOX5,_("Underlined"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX5"));
	UnderUse->SetValue(false);
	UnderYes = new wxRadioButton(this,ID_RADIOBUTTON7,_("Yes"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP,wxDefaultValidator,_("ID_RADIOBUTTON7"));
	UnderYes->SetValue(false);
	UnderNo = new wxRadioButton(this,ID_RADIOBUTTON8,_("No"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_RADIOBUTTON8"));
	UnderNo->SetValue(true);
	StaticBoxSizer5->Add(UnderUse,0,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer5->Add(UnderYes,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer5->Add(UnderNo,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(StaticBoxSizer3,1,wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	BoxSizer2->Add(StaticBoxSizer4,1,wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(StaticBoxSizer5,1,wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer5->Add(StaticBoxSizer1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer5->Add(StaticBoxSizer2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer5->Add(BoxSizer2,1,wxLEFT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Faces"));
	FaceList = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_("ID_LISTBOX1"));
	FaceList->SetSelection(-1);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	FaceAdd = new wxButton(this,ID_BUTTON4,_("Add"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON4"));
	if (false) FaceAdd->SetDefault();
	FaceDel = new wxButton(this,ID_BUTTON5,_("Delete"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON5"));
	if (false) FaceDel->SetDefault();
	FaceEdit = new wxButton(this,ID_BUTTON6,_("Edit"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON6"));
	if (false) FaceEdit->SetDefault();
	FaceUp = new wxButton(this,ID_BUTTON8,_("Move Up"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON8"));
	if (false) FaceUp->SetDefault();
	FaceDown = new wxButton(this,ID_BUTTON7,_("Move Down"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON7"));
	if (false) FaceDown->SetDefault();
	BoxSizer3->Add(FaceAdd,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(FaceDel,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(FaceEdit,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(FaceUp,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(FaceDown,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer6->Add(FaceList,1,wxLEFT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer6->Add(BoxSizer3,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP,0);
	BoxSizer6->Add(StaticBoxSizer6,1,wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(BoxSizer5,1,wxRIGHT|wxTOP|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer4->Add(BoxSizer6,1,wxALIGN_CENTER|wxEXPAND,5);
	StaticBoxSizer7 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Test area"));
	TestArea = new wxTextCtrl(this,ID_TEXTCTRL2,_("This is sample text"),wxDefaultPosition,wxSize(316,53),0,wxDefaultValidator,_("ID_TEXTCTRL2"));
	if ( 0 ) TestArea->SetMaxLength(0);
	StaticBoxSizer7->Add(TestArea,1,wxALL|wxALIGN_CENTER,5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_OK,_T("")));
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,_T("")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(BoxSizer4,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(StaticBoxSizer7,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(StdDialogButtonSizer1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
    Initialized = true;

    size_t Count = wxFontMapper::Get()->GetSupportedEncodingsCount();
    for ( size_t i = 0; i<Count; i++ )
    {
        wxFontEncoding Enc = wxFontMapper::Get()->GetEncoding(i);
        if ( Enc == wxFONTENCODING_DEFAULT || Enc == wxFONTENCODING_SYSTEM ) continue;
        wxString Name = wxFontMapper::Get()->GetEncodingName(Enc);
        Encodings.Add(Name);
        EncodVal->Append(wxFontMapper::Get()->GetEncodingDescription(Enc));
    }

	ReadData(Data);
	UpdateContent();
	UpdatePreview();
}

wxsFontEditorDlg::~wxsFontEditorDlg()
{
}

void wxsFontEditorDlg::OnButton1Click(wxCommandEvent& event)
{
}

void wxsFontEditorDlg::UpdateContent()
{
    int Type = FontType->GetSelection();

    bool TypeSystem = Type == FT_SYSTEM;
    bool TypeNotDef = Type != FT_DFAULT;
    bool RelSizeEn  = !SizeUse->GetValue();

    BaseFontUse->Show(TypeSystem);
    BaseFontTxt->Show(TypeSystem);
    BaseFontVal->Show(TypeSystem);
    BaseFontVal->Enable(BaseFontUse->GetValue());
    FamUse->Enable(TypeNotDef);
    FamVal->Enable(TypeNotDef && FamUse->GetValue());
    EncodUse->Enable(TypeNotDef);
    EncodVal->Enable(TypeNotDef && EncodUse->GetValue());
    SizeUse->Enable(TypeNotDef);
    SizeVal->Enable(TypeNotDef && SizeUse->GetValue());
    RelSizeUse->Show(TypeSystem);
    RelSizeVal->Show(TypeSystem);
    RelSizeUse->Enable(RelSizeEn);
    RelSizeVal->Enable(RelSizeEn && RelSizeUse->GetValue());
    FaceList->Enable(TypeNotDef);
    FaceAdd->Enable(TypeNotDef);
    FaceDel->Enable(TypeNotDef && FaceList->GetSelection() != wxNOT_FOUND);
    FaceEdit->Enable(TypeNotDef && FaceList->GetSelection() != wxNOT_FOUND);
    FaceUp->Enable(TypeNotDef && FaceList->GetSelection() != wxNOT_FOUND);
    FaceDown->Enable(TypeNotDef && FaceList->GetSelection() != wxNOT_FOUND);
    StyleUse->Enable(TypeNotDef);
    StyleNorm->Enable(TypeNotDef && StyleUse->GetValue());
    StyleItal->Enable(TypeNotDef && StyleUse->GetValue());
    StyleSlant->Enable(TypeNotDef && StyleUse->GetValue());
    WeightUse->Enable(TypeNotDef);
    WeightLight->Enable(TypeNotDef && WeightUse->GetValue());
    WeightNorm->Enable(TypeNotDef && WeightUse->GetValue());
    WeightBold->Enable(TypeNotDef && WeightUse->GetValue());
    UnderUse->Enable(TypeNotDef);
    UnderYes->Enable(TypeNotDef && UnderUse->GetValue());
    UnderNo->Enable(TypeNotDef && UnderUse->GetValue());
    Layout();
}

void wxsFontEditorDlg::UpdatePreview()
{
    wxsFontData TempData;
    StoreData(TempData);
    wxFont Font = TempData.BuildFont();
    if ( FaceList->GetSelection() != wxNOT_FOUND )
    {
        Font.SetFaceName(FaceList->GetStringSelection());
    }
    TestArea->SetFont(Font);
}

void wxsFontEditorDlg::ReadData(wxsFontData& Data)
{
    if ( Data.IsDefault )
    {
        FontType->SetSelection(FT_DFAULT);
        return;
    }

    if ( !Data.SysFont.empty() && Data.HasSysFont )
    {
        FontType->SetSelection(FT_SYSTEM);
        if ( Data.HasSysFont )
        {
            BaseFontUse->SetValue(true);
            if ( !BaseFontVal->SetStringSelection(Data.SysFont) )
            {
                BaseFontVal->SetSelection(0);
            }
        }

        if ( !Data.HasSize && Data.HasRelativeSize )
        {
            RelSizeUse->SetValue(true);
            RelSizeVal->SetValue(wxString::Format(_T("%f"),Data.RelativeSize));
        }
    }
    else
    {
        FontType->SetSelection(FT_CUSTOM);
    }

    if ( Data.HasFamily )
    {
        FamUse->SetValue(true);
        switch ( Data.Family )
        {
            case wxFONTFAMILY_DECORATIVE: FamVal->SetSelection(FFAM_DECORATIVE); break;
            case wxFONTFAMILY_ROMAN:      FamVal->SetSelection(FFAM_ROMAN); break;
            case wxFONTFAMILY_SCRIPT:     FamVal->SetSelection(FFAM_SCRIPT); break;
            case wxFONTFAMILY_SWISS:      FamVal->SetSelection(FFAM_SWISS); break;
            case wxFONTFAMILY_MODERN:     FamVal->SetSelection(FFAM_MODERN); break;
            case wxFONTFAMILY_TELETYPE:   FamVal->SetSelection(FFAM_TELETYPE); break;
            default:                      FamUse->SetValue(false);
        }
    }

    if ( Data.HasEncoding )
    {
        int Index = Encodings.Index(Data.Encoding);
        if ( Index != wxNOT_FOUND )
        {
            EncodUse->SetValue(true);
            EncodVal->SetSelection(Index);
        }
    }

    if ( Data.HasSize )
    {
        SizeUse->SetValue(true);
        SizeVal->SetValue(Data.Size);
    }

    FaceList->Append(Data.Faces);
    FaceList->SetSelection(0);

    if ( Data.HasStyle )
    {
        StyleUse->SetValue(true);
        StyleNorm->SetValue(Data.Style == wxFONTSTYLE_NORMAL);
        StyleItal->SetValue(Data.Style == wxFONTSTYLE_ITALIC);
        StyleSlant->SetValue(Data.Style == wxFONTSTYLE_SLANT);
    }

    if ( Data.HasWeight )
    {
        WeightUse->SetValue(true);
        WeightNorm->SetValue(Data.Weight == wxFONTWEIGHT_NORMAL);
        WeightLight->SetValue(Data.Weight == wxFONTWEIGHT_LIGHT);
        WeightBold->SetValue(Data.Weight == wxFONTWEIGHT_BOLD);
    }

    if ( Data.HasUnderlined )
    {
        UnderUse->SetValue(true);
        UnderYes->SetValue(Data.Underlined);
        UnderNo->SetValue(!Data.Underlined);
    }
}

void wxsFontEditorDlg::StoreData(wxsFontData& Data)
{
    if ( FontType->GetSelection() == FT_DFAULT )
    {
        Data.IsDefault = true;
        Data.HasSize = false;
        Data.HasStyle = false;
        Data.HasWeight = false;
        Data.HasUnderlined = false;
        Data.HasFamily = false;
        Data.HasEncoding = false;
        Data.HasSysFont = false;
        Data.HasRelativeSize = false;
        Data.Faces.Clear();
        return;
    }

    Data.IsDefault = false;
    if ( FontType->GetSelection() == FT_CUSTOM )
    {
        Data.HasSysFont = false;
        Data.HasRelativeSize = false;
    }
    else
    {
        if ( Data.HasSysFont = BaseFontUse->GetValue() )
        {
            Data.SysFont = BaseFontVal->GetStringSelection();
        }

        if ( Data.HasRelativeSize = !SizeVal->GetValue() && RelSizeVal->GetValue() )
        {
            RelSizeVal->GetValue().ToDouble(&Data.RelativeSize);
        }
    }

    if ( Data.HasFamily = FamUse->GetValue() )
    {
        switch ( FamVal->GetSelection() )
        {
            case FFAM_DECORATIVE: Data.Family = wxFONTFAMILY_DECORATIVE; break;
            case FFAM_ROMAN:      Data.Family = wxFONTFAMILY_ROMAN;      break;
            case FFAM_SCRIPT:     Data.Family = wxFONTFAMILY_SCRIPT;     break;
            case FFAM_SWISS:      Data.Family = wxFONTFAMILY_SWISS;      break;
            case FFAM_MODERN:     Data.Family = wxFONTFAMILY_MODERN;     break;
            case FFAM_TELETYPE:   Data.Family = wxFONTFAMILY_TELETYPE;   break;
            default:              Data.HasFamily = false;
        }
    }

    if ( Data.HasEncoding = EncodUse->GetValue() )
    {
        int Index = EncodVal->GetSelection();
        if ( Index == wxNOT_FOUND )
        {
            Data.HasEncoding = false;
        }
        else
        {
            Data.Encoding = Encodings[Index];
        }
    }

    if ( Data.HasSize = SizeUse->GetValue() )
    {
        Data.Size = SizeVal->GetValue();
    }

    if ( Data.HasStyle = StyleUse->GetValue() )
    {
        if ( StyleNorm->GetValue() ) Data.Style = wxFONTSTYLE_NORMAL;
        if ( StyleItal->GetValue() ) Data.Style = wxFONTSTYLE_ITALIC;
        if ( StyleSlant->GetValue() ) Data.Style = wxFONTSTYLE_SLANT;
    }

    if ( Data.HasWeight = WeightUse->GetValue() )
    {
        if ( WeightLight->GetValue() ) Data.Weight = wxFONTWEIGHT_LIGHT;
        if ( WeightNorm->GetValue() ) Data.Weight = wxFONTWEIGHT_NORMAL;
        if ( WeightBold->GetValue() ) Data.Weight = wxFONTWEIGHT_BOLD;
    }

    if ( Data.HasUnderlined = UnderUse->GetValue() )
    {
        if ( UnderYes->GetValue() ) Data.Underlined = true;
        if ( UnderNo->GetValue() ) Data.Underlined = false;
    }

    Data.Faces.Clear();
    size_t Count = FaceList->GetCount();
    for ( size_t i=0; i<Count; i++ )
    {
        Data.Faces.Add(FaceList->GetString(i));
    }
}

void wxsFontEditorDlg::OnUpdateContent(wxCommandEvent& event)
{
    UpdateContent();
    UpdatePreview();
}

void wxsFontEditorDlg::OnButton2Click(wxCommandEvent& event)
{
    StoreData(Data);
    EndModal(wxID_OK);
}

void wxsFontEditorDlg::OnButton1Click1(wxCommandEvent& event)
{
    wxFont Font  = ::wxGetFontFromUser();
    if ( !Font.Ok() ) return;

    FontType->SetSelection(FT_CUSTOM);
    FamUse->SetValue(true);
    switch ( Font.GetFamily() )
    {
        case wxFONTFAMILY_DECORATIVE: FamVal->SetSelection(FFAM_DECORATIVE); break;
        case wxFONTFAMILY_ROMAN:      FamVal->SetSelection(FFAM_ROMAN); break;
        case wxFONTFAMILY_SCRIPT:     FamVal->SetSelection(FFAM_SCRIPT); break;
        case wxFONTFAMILY_SWISS:      FamVal->SetSelection(FFAM_SWISS); break;
        case wxFONTFAMILY_MODERN:     FamVal->SetSelection(FFAM_MODERN); break;
        case wxFONTFAMILY_TELETYPE:   FamVal->SetSelection(FFAM_TELETYPE); break;
        default:                      FamUse->SetValue(false);
    }

    EncodUse->SetValue(false);
    SizeUse->SetValue(true);
    SizeVal->SetValue(Font.GetPointSize());
    RelSizeUse->SetValue(false);

    FaceList->Clear();
    FaceList->Append(Font.GetFaceName());
    FaceList->SetSelection(0);

    StyleNorm->SetValue(Font.GetStyle()==wxFONTSTYLE_NORMAL);
    StyleItal->SetValue(Font.GetStyle()==wxFONTSTYLE_ITALIC);
    StyleSlant->SetValue(Font.GetStyle()==wxFONTSTYLE_SLANT);
    StyleUse->SetValue(Font.GetStyle()!=wxFONTSTYLE_NORMAL);

    WeightLight->SetValue(Font.GetWeight()==wxFONTWEIGHT_LIGHT);
    WeightNorm->SetValue(Font.GetWeight()==wxFONTWEIGHT_NORMAL);
    WeightBold->SetValue(Font.GetWeight()==wxFONTWEIGHT_BOLD);
    WeightUse->SetValue(Font.GetWeight()!=wxFONTWEIGHT_NORMAL);

    UnderYes->SetValue(Font.GetUnderlined());
    UnderNo->SetValue(!Font.GetUnderlined());
    UnderUse->SetValue(!Font.GetUnderlined());

    UpdateContent();
    UpdatePreview();
}

void wxsFontEditorDlg::OnFaceAddClick(wxCommandEvent& event)
{
    wxString Face;
    wxsFontFaceEditorDlg Dlg(this,Face);
    if ( Dlg.ShowModal() == wxID_OK && !Face.empty() )
    {
        FaceList->Append(Face);
        FaceList->SetSelection(FaceList->GetCount()-1);
        UpdateContent();
        UpdatePreview();
    }
}

void wxsFontEditorDlg::OnFaceDelClick(wxCommandEvent& event)
{
    int Index = FaceList->GetSelection();
    if ( Index != wxNOT_FOUND )
    {
        FaceList->Delete(Index);
        if ( Index < FaceList->GetCount() )
        {
            FaceList->SetSelection(Index);
        }
        else
        {
            FaceList->SetSelection(Index-1);
        }
    }
    UpdateContent();
    UpdatePreview();
}

void wxsFontEditorDlg::OnFaceEditClick(wxCommandEvent& event)
{
    int Index = FaceList->GetSelection();
    if ( Index != wxNOT_FOUND )
    {
        wxString Face = FaceList->GetString(Index);
        wxsFontFaceEditorDlg Dlg(this,Face);
        if ( Dlg.ShowModal() == wxID_OK )
        {
            FaceList->SetString(Index,Face);
        }
    }
    UpdatePreview();
}

void wxsFontEditorDlg::OnFaceUpClick(wxCommandEvent& event)
{
    int Index = FaceList->GetSelection();
    if ( Index != wxNOT_FOUND && Index > 0 )
    {
        wxString Face = FaceList->GetString(Index);
        FaceList->Delete(Index);
        FaceList->Insert(Face,Index-1);
    }
    UpdatePreview();
}

void wxsFontEditorDlg::OnFaceDownClick(wxCommandEvent& event)
{
    int Index = FaceList->GetSelection();
    if ( Index != wxNOT_FOUND && Index < FaceList->GetCount()-1 )
    {
        wxString Face = FaceList->GetString(Index);
        FaceList->Delete(Index);
        FaceList->Insert(Face,Index+1);
    }
    UpdatePreview();
}

void wxsFontEditorDlg::OnUpdatePreview(wxCommandEvent& event)
{
    if ( Initialized )
    {
        UpdatePreview();
    }
}

void wxsFontEditorDlg::OnSizeValChange(wxSpinEvent& event)
{
    if ( Initialized )
    {
        UpdatePreview();
    }
}

void wxsFontEditorDlg::OnBaseFontUseChange(wxCommandEvent& event)
{
    BaseFontUse->SetValue(true);
}
