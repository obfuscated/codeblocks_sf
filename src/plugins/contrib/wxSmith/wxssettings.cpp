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

#include <wx/colordlg.h>
#include "wxssettings.h"
#include "wxwidgets/wxssizer.h"
#include "wxwidgets/wxsitemeditor.h"

//(*InternalHeaders(wxsSettings)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

// TODO: Replace fixed settings with some modular structure
//       like builders of settings etc.

//(*IdInit(wxsSettings)
const long wxsSettings::ID_STATICTEXT2 = wxNewId();
const long wxsSettings::ID_COMBOBOX1 = wxNewId();
const long wxsSettings::ID_STATICTEXT3 = wxNewId();
const long wxsSettings::ID_BUTTON1 = wxNewId();
const long wxsSettings::ID_STATICTEXT4 = wxNewId();
const long wxsSettings::ID_BUTTON2 = wxNewId();
const long wxsSettings::ID_STATICTEXT5 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON1 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON2 = wxNewId();
const long wxsSettings::ID_STATICTEXT6 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON3 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON4 = wxNewId();
const long wxsSettings::ID_STATICTEXT7 = wxNewId();
const long wxsSettings::ID_SPINCTRL2 = wxNewId();
const long wxsSettings::ID_STATICTEXT8 = wxNewId();
const long wxsSettings::ID_CHECKBOX1 = wxNewId();
const long wxsSettings::ID_CHECKBOX2 = wxNewId();
const long wxsSettings::ID_CHECKBOX3 = wxNewId();
const long wxsSettings::ID_CHECKBOX4 = wxNewId();
const long wxsSettings::ID_STATICTEXT9 = wxNewId();
const long wxsSettings::ID_CHECKBOX5 = wxNewId();
const long wxsSettings::ID_STATICTEXT10 = wxNewId();
const long wxsSettings::ID_CHECKBOX6 = wxNewId();
const long wxsSettings::ID_STATICTEXT12 = wxNewId();
const long wxsSettings::ID_CHOICE1 = wxNewId();
const long wxsSettings::ID_STATICTEXT13 = wxNewId();
const long wxsSettings::ID_SPINCTRL3 = wxNewId();
const long wxsSettings::ID_CHECKBOX8 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsSettings,cbConfigurationPanel)
	//(*EventTable(wxsSettings)
	//*)
END_EVENT_TABLE()

wxsSettings::wxsSettings(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxsSettings)
	wxStaticText* StaticText2;
	wxStaticText* StaticText3;
	wxStaticText* StaticText4;
	wxStaticText* StaticText5;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxPanel"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Editor"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Drag Assistance Type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	DragAssistType = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
	DragAssistType->SetSelection( DragAssistType->Append(_("None")) );
	DragAssistType->Append(_("Simple"));
	DragAssistType->Append(_("Colour Mix"));
	FlexGridSizer2->Add(DragAssistType,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Drag target colour"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	DragTargetCol = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(DragTargetCol,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Drag Parent Colour"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer2->Add(StaticText4,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	DragParentCol = new wxButton(this, ID_BUTTON2, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(DragParentCol,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Palette icon size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer2->Add(StaticText5,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 0, 0, 0);
	Icons16 = new wxRadioButton(this, ID_RADIOBUTTON1, _("16x16"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	FlexGridSizer3->Add(Icons16,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	Icons32 = new wxRadioButton(this, ID_RADIOBUTTON2, _("32x32"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	FlexGridSizer3->Add(Icons32,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(FlexGridSizer3,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Tool icon size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer2->Add(StaticText6,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 0, 0, 0);
	TIcons16 = new wxRadioButton(this, ID_RADIOBUTTON3, _("16x16"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
	FlexGridSizer4->Add(TIcons16,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	TIcons32 = new wxRadioButton(this, ID_RADIOBUTTON4, _("32x32"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON4"));
	FlexGridSizer4->Add(TIcons32,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(FlexGridSizer4,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer6->Add(StaticBoxSizer1,1,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Default sizer settings"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Proportion:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer5->Add(StaticText7,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	spinProportion = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL2"));
	spinProportion->SetValue(_T("0"));
	FlexGridSizer5->Add(spinProportion,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Border flags:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer5->Add(StaticText8,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	chkTop = new wxCheckBox(this, ID_CHECKBOX1, _("Top"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chkTop->SetValue(true);
	BoxSizer2->Add(chkTop,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	chkBottom = new wxCheckBox(this, ID_CHECKBOX2, _("Bottom"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	chkBottom->SetValue(true);
	BoxSizer2->Add(chkBottom,0,wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	chkLeft = new wxCheckBox(this, ID_CHECKBOX3, _("Left"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	chkLeft->SetValue(true);
	BoxSizer2->Add(chkLeft,0,wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	chkRight = new wxCheckBox(this, ID_CHECKBOX4, _("Right"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	chkRight->SetValue(true);
	BoxSizer2->Add(chkRight,0,wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer5->Add(BoxSizer2,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Expand:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer5->Add(StaticText9,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	chkExpand = new wxCheckBox(this, ID_CHECKBOX5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	chkExpand->SetValue(false);
	FlexGridSizer5->Add(chkExpand,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Shaped:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer5->Add(StaticText10,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	chkShaped = new wxCheckBox(this, ID_CHECKBOX6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
	chkShaped->SetValue(false);
	FlexGridSizer5->Add(chkShaped,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Placement:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	FlexGridSizer5->Add(StaticText12,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	choicePlacement = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	choicePlacement->SetSelection( choicePlacement->Append(_("Left-Top")) );
	choicePlacement->Append(_("Top"));
	choicePlacement->Append(_("Right-Top"));
	choicePlacement->Append(_("Left"));
	choicePlacement->Append(_("Center"));
	choicePlacement->Append(_("Right"));
	choicePlacement->Append(_("Left-Bottom"));
	choicePlacement->Append(_("Bottom"));
	choicePlacement->Append(_("Right-Bottom"));
	FlexGridSizer5->Add(choicePlacement,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
	StaticText13 = new wxStaticText(this, ID_STATICTEXT13, _("Border:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	FlexGridSizer5->Add(StaticText13,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	spinBorder = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL3"));
	spinBorder->SetValue(_T("0"));
	FlexGridSizer5->Add(spinBorder,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
	FlexGridSizer5->Add(16,8,1,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	chkBorderDU = new wxCheckBox(this, ID_CHECKBOX8, _("Use dialog units"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
	chkBorderDU->SetValue(false);
	FlexGridSizer5->Add(chkBorderDU,1,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(FlexGridSizer5,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer6->Add(StaticBoxSizer2,1,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer1->Add(FlexGridSizer6,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsSettings::OnDragTargetColClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsSettings::OnDragParentColClick);
	//*)

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    long ColTarget = cfg->ReadInt(_T("/dragtargetcol"),0x608CDFL);;
    long ColParent = cfg->ReadInt(_T("/dragparentcol"),0x0D177BL);;
    DragAssistType->SetSelection(cfg->ReadInt(_T("/dragassisttype"),2));
    DragTargetCol->SetBackgroundColour(wxColour((ColTarget>>16)&0xFF,(ColTarget>>8)&0xFF,ColTarget&0xFF));
    DragParentCol->SetBackgroundColour(wxColour((ColParent>>16)&0xFF,(ColParent>>8)&0xFF,ColParent&0xFF));

    if ( cfg->ReadInt(_T("/tooliconsize"),32L) == 16 )
    {
        TIcons16->SetValue(true);
    }
    else
    {
        TIcons32->SetValue(true);
    }

    if ( cfg->ReadInt(_T("/paletteiconsize"),16L) == 16 )
    {
        Icons16->SetValue(true);
    }
    else
    {
        Icons32->SetValue(true);
    }

    // Creating wxsSizerExtra structure which will be filled with used data
    wxsSizerExtra Extra;

    spinProportion->SetValue(Extra.Proportion);

    chkLeft->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderLeft);
    chkRight->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderRight);
    chkTop->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderTop);
    chkBottom->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderBottom);
    chkExpand->SetValue(Extra.Flags & wxsSizerFlagsProperty::Expand);
    chkShaped->SetValue(Extra.Flags & wxsSizerFlagsProperty::Shaped);

    if ( Extra.Flags & wxsSizerFlagsProperty::AlignBottom )
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            choicePlacement->SetSelection(8);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            choicePlacement->SetSelection(7);
        }
        else
        {
            choicePlacement->SetSelection(6);
        }
    }
    else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterVertical )
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            choicePlacement->SetSelection(5);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            choicePlacement->SetSelection(4);
        }
        else
        {
            choicePlacement->SetSelection(3);
        }
    }
    else
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            choicePlacement->SetSelection(2);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            choicePlacement->SetSelection(1);
        }
        else
        {
            choicePlacement->SetSelection(0);
        }
    }

    spinBorder->SetValue(Extra.Border.Value);
    chkBorderDU->SetValue(Extra.Border.DialogUnits);
}

wxsSettings::~wxsSettings()
{
	//(*Destroy(wxsSettings)
	//*)
}

void wxsSettings::OnDragTargetColClick(wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,DragTargetCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        DragTargetCol->SetBackgroundColour(Col);
    }
}

void wxsSettings::OnDragParentColClick(wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,DragParentCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        DragParentCol->SetBackgroundColour(Col);
    }
}

void wxsSettings::OnApply()
{
    wxColour ColTarget = DragTargetCol->GetBackgroundColour();
    wxColour ColParent = DragParentCol->GetBackgroundColour();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    cfg->Write(_T("/dragtargetcol"),(int)((((int)ColTarget.Red())<<16) + (((long)ColTarget.Green())<<8) + (long)ColTarget.Blue()));
    cfg->Write(_T("/dragparentcol"),(int)((((int)ColParent.Red())<<16) + (((long)ColParent.Green())<<8) + (long)ColParent.Blue()));
    cfg->Write(_T("/dragassisttype"),(int)DragAssistType->GetSelection());
    cfg->Write(_T("/paletteiconsize"),(int)(Icons16->GetValue()?16:32));
    cfg->Write(_T("/tooliconsize"),(int)(TIcons16->GetValue()?16:32));

    int Flags =  (chkLeft->IsChecked()   ? wxsSizerFlagsProperty::BorderLeft   : 0) |
                 (chkRight->IsChecked()  ? wxsSizerFlagsProperty::BorderRight  : 0) |
                 (chkTop->IsChecked()    ? wxsSizerFlagsProperty::BorderTop    : 0) |
                 (chkBottom->IsChecked() ? wxsSizerFlagsProperty::BorderBottom : 0) |
                 (chkExpand->IsChecked() ? wxsSizerFlagsProperty::Expand       : 0) |
                 (chkShaped->IsChecked() ? wxsSizerFlagsProperty::Shaped       : 0);

    switch ( choicePlacement->GetSelection() )
    {
        case 0: Flags |= wxsSizerFlagsProperty::AlignLeft |
                         wxsSizerFlagsProperty::AlignTop; break;
        case 1: Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal |
                         wxsSizerFlagsProperty::AlignTop; break;
        case 2: Flags |= wxsSizerFlagsProperty::AlignRight |
                         wxsSizerFlagsProperty::AlignTop; break;
        case 3: Flags |= wxsSizerFlagsProperty::AlignLeft |
                         wxsSizerFlagsProperty::AlignCenterVertical; break;
        case 4: Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal |
                         wxsSizerFlagsProperty::AlignCenterVertical; break;
        case 5: Flags |= wxsSizerFlagsProperty::AlignRight |
                         wxsSizerFlagsProperty::AlignCenterVertical; break;
        case 6: Flags |= wxsSizerFlagsProperty::AlignLeft |
                         wxsSizerFlagsProperty::AlignBottom; break;
        case 7: Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal |
                         wxsSizerFlagsProperty::AlignBottom; break;
        case 8: Flags |= wxsSizerFlagsProperty::AlignRight |
                         wxsSizerFlagsProperty::AlignBottom; break;
    }

    cfg->Write(_T("/defsizer/flags"), Flags);
    cfg->Write(_T("/defsizer/border"), (int)spinBorder->GetValue());
    cfg->Write(_T("/defsizer/borderdu"),(bool)chkBorderDU->GetValue());

    wxsItemEditor::ConfigChanged();
}
