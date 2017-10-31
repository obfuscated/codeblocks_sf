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

#include <wx/colordlg.h>
#include "wxssettings.h"
#include "wxwidgets/wxssizer.h"
#include "wxwidgets/wxsitemeditor.h"

#include <prep.h>

//(*InternalHeaders(wxsSettings)
#include <wx/intl.h>
#include <wx/string.h>
//*)

// TODO: Replace fixed settings with some modular structure
//       like builders of settings etc.

//(*IdInit(wxsSettings)
const long wxsSettings::ID_CHECKBOX11 = wxNewId();
const long wxsSettings::ID_CHOICE2 = wxNewId();
const long wxsSettings::ID_COMBOBOX1 = wxNewId();
const long wxsSettings::ID_BUTTON1 = wxNewId();
const long wxsSettings::ID_BUTTON2 = wxNewId();
const long wxsSettings::ID_CHECKBOX7 = wxNewId();
const long wxsSettings::ID_SPINCTRL1 = wxNewId();
const long wxsSettings::ID_CHECKBOX9 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON1 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON2 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON3 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON4 = wxNewId();
const long wxsSettings::ID_SPINCTRL2 = wxNewId();
const long wxsSettings::ID_CHECKBOX1 = wxNewId();
const long wxsSettings::ID_CHECKBOX2 = wxNewId();
const long wxsSettings::ID_CHECKBOX3 = wxNewId();
const long wxsSettings::ID_CHECKBOX4 = wxNewId();
const long wxsSettings::ID_CHECKBOX5 = wxNewId();
const long wxsSettings::ID_CHECKBOX6 = wxNewId();
const long wxsSettings::ID_CHOICE1 = wxNewId();
const long wxsSettings::ID_SPINCTRL3 = wxNewId();
const long wxsSettings::ID_CHECKBOX8 = wxNewId();
const long wxsSettings::ID_CHECKBOX10 = wxNewId();
const long wxsSettings::ID_CHECKBOX12 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON5 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON6 = wxNewId();
const long wxsSettings::ID_RADIOBUTTON7 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsSettings,cbConfigurationPanel)
    //(*EventTable(wxsSettings)
    //*)
END_EVENT_TABLE()

wxsSettings::wxsSettings(wxWindow* parent,cb_unused wxWindowID id)
{
    //(*Initialize(wxsSettings)
    wxStaticText* StaticText10;
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxStaticText* StaticText9;
    wxFlexGridSizer* FlexGridSizer4;
    wxStaticText* StaticText13;
    wxStaticText* StaticText2;
    wxStaticText* StaticText14;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer3;
    wxStaticText* StaticText6;
    wxFlexGridSizer* FlexGridSizer5;
    wxStaticText* StaticText8;
    wxStaticText* StaticText11;
    wxFlexGridSizer* FlexGridSizer2;
    wxStaticText* StaticText1;
    wxStaticText* StaticText3;
    wxFlexGridSizer* FlexGridSizer7;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxStaticText* StaticText5;
    wxStaticText* StaticText7;
    wxBoxSizer* BoxSizer1;
    wxStaticText* StaticText15;
    wxStaticText* StaticText12;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxStaticText* StaticText4;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer6->AddGrowableCol(0);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Resource and properties browser"));
    FlexGridSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer7->AddGrowableCol(1);
    StaticText14 = new wxStaticText(this, wxID_ANY, _("Unique IDs only:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer7->Add(StaticText14, 1, wxALIGN_CENTER_VERTICAL, 5);
    m_UniqueIDsOnly = new wxCheckBox(this, ID_CHECKBOX11, _("(uncheck if you want to assign the same ID to different widgets)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
    m_UniqueIDsOnly->SetValue(true);
    FlexGridSizer7->Add(m_UniqueIDsOnly, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(this, wxID_ANY, _("Placement:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer7->Add(StaticText10, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_BrowserPlacements = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    m_BrowserPlacements->Append(_("Add inside Management pane"));
    m_BrowserPlacements->Append(_("One dockable pane (both browsers on one pane)"));
    m_BrowserPlacements->Append(_("Two dockable panes (each browser on it\'s pane)"));
    FlexGridSizer7->Add(m_BrowserPlacements, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    FlexGridSizer7->Add(-1,-1,0, wxEXPAND, 5);
    StaticText11 = new wxStaticText(this, wxID_ANY, _("Placement will change after restart"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer7->Add(StaticText11, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer7, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer6->Add(StaticBoxSizer3, 1, wxEXPAND, 4);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Editor"));
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer2->AddGrowableCol(1);
    StaticText2 = new wxStaticText(this, wxID_ANY, _("Drag assistance type:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText2, 0, wxALIGN_CENTER_VERTICAL, 5);
    m_DragAssistType = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
    m_DragAssistType->Append(_("None"));
    m_DragAssistType->Append(_("Simple"));
    m_DragAssistType->Append(_("Colour Mix"));
    FlexGridSizer2->Add(m_DragAssistType, 1, wxLEFT|wxEXPAND, 5);
    StaticText3 = new wxStaticText(this, wxID_ANY, _("Drag target colour:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText3, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_DragTargetCol = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer2->Add(m_DragTargetCol, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText4 = new wxStaticText(this, wxID_ANY, _("Drag parent colour:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText4, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_DragParentCol = new wxButton(this, ID_BUTTON2, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer2->Add(m_DragParentCol, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    m_UseGrid = new wxCheckBox(this, ID_CHECKBOX7, _("Snap to grid:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    m_UseGrid->SetValue(false);
    FlexGridSizer2->Add(m_UseGrid, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_GridSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("8"), wxDefaultPosition, wxDefaultSize, 0, 2, 100, 8, _T("ID_SPINCTRL1"));
    m_GridSize->SetValue(_T("8"));
    FlexGridSizer2->Add(m_GridSize, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText1 = new wxStaticText(this, wxID_ANY, _("Add new items continously:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText1, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Continous = new wxCheckBox(this, ID_CHECKBOX9, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
    m_Continous->SetValue(false);
    FlexGridSizer2->Add(m_Continous, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText5 = new wxStaticText(this, wxID_ANY, _("Palette icon size:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText5, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer3->AddGrowableCol(0);
    FlexGridSizer3->AddGrowableCol(1);
    m_Icons16 = new wxRadioButton(this, ID_RADIOBUTTON1, _("16x16"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
    FlexGridSizer3->Add(m_Icons16, 1, wxEXPAND, 5);
    m_Icons32 = new wxRadioButton(this, ID_RADIOBUTTON2, _("32x32"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
    FlexGridSizer3->Add(m_Icons32, 1, wxLEFT|wxEXPAND, 5);
    FlexGridSizer2->Add(FlexGridSizer3, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText6 = new wxStaticText(this, wxID_ANY, _("Tool icon size:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer2->Add(StaticText6, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer4->AddGrowableCol(0);
    FlexGridSizer4->AddGrowableCol(1);
    m_TIcons16 = new wxRadioButton(this, ID_RADIOBUTTON3, _("16x16"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
    FlexGridSizer4->Add(m_TIcons16, 1, wxEXPAND, 5);
    m_TIcons32 = new wxRadioButton(this, ID_RADIOBUTTON4, _("32x32"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON4"));
    FlexGridSizer4->Add(m_TIcons32, 1, wxLEFT|wxEXPAND, 5);
    FlexGridSizer2->Add(FlexGridSizer4, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticBoxSizer1->Add(FlexGridSizer2, 1, wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer6->Add(StaticBoxSizer1, 1, wxEXPAND, 4);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Default sizer settings"));
    FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer5->AddGrowableCol(1);
    StaticText7 = new wxStaticText(this, wxID_ANY, _("Proportion:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer5->Add(StaticText7, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Proportion = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL2"));
    m_Proportion->SetValue(_T("0"));
    FlexGridSizer5->Add(m_Proportion, 0, wxLEFT|wxEXPAND, 5);
    StaticText8 = new wxStaticText(this, wxID_ANY, _("Border flags:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer5->Add(StaticText8, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    m_BorderTop = new wxCheckBox(this, ID_CHECKBOX1, _("Top"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_BorderTop->SetValue(true);
    BoxSizer2->Add(m_BorderTop, 1, wxALIGN_CENTER_VERTICAL, 5);
    m_BorderBottom = new wxCheckBox(this, ID_CHECKBOX2, _("Bottom"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_BorderBottom->SetValue(true);
    BoxSizer2->Add(m_BorderBottom, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_BorderLeft = new wxCheckBox(this, ID_CHECKBOX3, _("Left"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_BorderLeft->SetValue(true);
    BoxSizer2->Add(m_BorderLeft, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_BorderRight = new wxCheckBox(this, ID_CHECKBOX4, _("Right"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    m_BorderRight->SetValue(true);
    BoxSizer2->Add(m_BorderRight, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(BoxSizer2, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText9 = new wxStaticText(this, wxID_ANY, _("Auto-size:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer5->Add(StaticText9, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_SizeExpand = new wxCheckBox(this, ID_CHECKBOX5, _("Expand"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    m_SizeExpand->SetValue(false);
    BoxSizer1->Add(m_SizeExpand, 1, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    m_SizeShaped = new wxCheckBox(this, ID_CHECKBOX6, _("Shaped"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    m_SizeShaped->SetValue(false);
    BoxSizer1->Add(m_SizeShaped, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(BoxSizer1, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText12 = new wxStaticText(this, wxID_ANY, _("Placement:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer5->Add(StaticText12, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Placement = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    m_Placement->Append(_("Left-Top"));
    m_Placement->Append(_("Top"));
    m_Placement->Append(_("Right-Top"));
    m_Placement->Append(_("Left"));
    m_Placement->Append(_("Center"));
    m_Placement->Append(_("Right"));
    m_Placement->Append(_("Left-Bottom"));
    m_Placement->Append(_("Bottom"));
    m_Placement->Append(_("Right-Bottom"));
    FlexGridSizer5->Add(m_Placement, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText13 = new wxStaticText(this, wxID_ANY, _("Border:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer5->Add(StaticText13, 0, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    m_Border = new wxSpinCtrl(this, ID_SPINCTRL3, _T("8"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 8, _T("ID_SPINCTRL3"));
    m_Border->SetValue(_T("8"));
    BoxSizer3->Add(m_Border, 1, wxALIGN_TOP, 5);
    m_BorderDU = new wxCheckBox(this, ID_CHECKBOX8, _("Use dialog units"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    m_BorderDU->SetValue(false);
    BoxSizer3->Add(m_BorderDU, 1, wxLEFT|wxEXPAND, 5);
    FlexGridSizer5->Add(BoxSizer3, 0, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticBoxSizer2->Add(FlexGridSizer5, 1, wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer6->Add(StaticBoxSizer2, 1, wxEXPAND, 4);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL, this, _("Other settings"));
    m_RemovePrefix = new wxCheckBox(this, ID_CHECKBOX10, _("Auto remove variable prefix for event handler function"), wxDefaultPosition, wxSize(323,14), 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
    m_RemovePrefix->SetValue(false);
    StaticBoxSizer4->Add(m_RemovePrefix, 0, wxEXPAND, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    m_UseI18N = new wxCheckBox(this, ID_CHECKBOX12, _("Use I18N for strings"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
    m_UseI18N->SetValue(true);
    BoxSizer4->Add(m_UseI18N, 0, wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1 = new wxFlexGridSizer(0, 5, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->Add(0,0,1, wxEXPAND, 5);
    StaticText15 = new wxStaticText(this, wxID_ANY, _("Non-I18N string:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText15, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_NoneI18N_T = new wxRadioButton(this, ID_RADIOBUTTON5, _("_T(\"\")"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON5"));
    m_NoneI18N_T->SetValue(true);
    FlexGridSizer1->Add(m_NoneI18N_T, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_NoneI18NwxT = new wxRadioButton(this, ID_RADIOBUTTON6, _("wxT(\"\")"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON6"));
    FlexGridSizer1->Add(m_NoneI18NwxT, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_NoneI18N = new wxRadioButton(this, ID_RADIOBUTTON7, _("\"\""), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON7"));
    FlexGridSizer1->Add(m_NoneI18N, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(FlexGridSizer1, 1, wxLEFT|wxEXPAND, 5);
    StaticBoxSizer4->Add(BoxSizer4, 0, wxTOP|wxEXPAND, 5);
    FlexGridSizer6->Add(StaticBoxSizer4, 1, wxEXPAND, 5);
    SetSizer(FlexGridSizer6);
    FlexGridSizer6->Fit(this);
    FlexGridSizer6->SetSizeHints(this);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsSettings::OnDragTargetColClick));
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsSettings::OnDragParentColClick));
    Connect(ID_CHECKBOX7,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(wxsSettings::OnUseGridClick));
    //*)

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    m_UniqueIDsOnly->SetValue(cfg->ReadBool(_T("/uniqueids"),true));
    m_InitialPlacement = cfg->ReadInt(_T("/browserplacements"),0);
    m_BrowserPlacements->SetSelection(m_InitialPlacement);

    long ColTarget = cfg->ReadInt(_T("/dragtargetcol"),0x608CDFL);;
    long ColParent = cfg->ReadInt(_T("/dragparentcol"),0x0D177BL);;
    m_DragAssistType->SetSelection(cfg->ReadInt(_T("/dragassisttype"),2));
    m_DragTargetCol->SetBackgroundColour(wxColour((ColTarget>>16)&0xFF,(ColTarget>>8)&0xFF,ColTarget&0xFF));
    m_DragParentCol->SetBackgroundColour(wxColour((ColParent>>16)&0xFF,(ColParent>>8)&0xFF,ColParent&0xFF));

    if ( cfg->ReadInt(_T("/tooliconsize"),32L) == 16 )
    {
        m_TIcons16->SetValue(true);
    }
    else
    {
        m_TIcons32->SetValue(true);
    }

    if ( cfg->ReadInt(_T("/paletteiconsize"),16L) == 16 )
    {
        m_Icons16->SetValue(true);
    }
    else
    {
        m_Icons32->SetValue(true);
    }

    int GridSize = cfg->ReadInt(_T("/gridsize"),8);
    if ( GridSize > 1 )
    {
        m_UseGrid->SetValue(true);
        m_GridSize->SetValue(GridSize);
    }
    else
    {
        if ( GridSize<-1 )
        {
            m_GridSize->SetValue(-GridSize);
        }
        m_GridSize->Disable();
    }

    m_Continous->SetValue(cfg->ReadBool(_T("/continousinsert"),false));
    m_RemovePrefix->SetValue(cfg->ReadBool(_T("/removeprefix"),false));

    m_UseI18N->SetValue(cfg->ReadBool(_T("/useI18N"),true));
    switch (cfg->ReadInt(_T("/noneI18N"),0))
    {
      case 0: m_NoneI18N_T->SetValue(true);  break;
      case 1: m_NoneI18NwxT->SetValue(true); break;
      case 2: m_NoneI18N->SetValue(true);    break;
      default:                               break;
    }

    // Creating wxsSizerExtra structure which will be filled with used data
    wxsSizerExtra Extra;

    m_Proportion->SetValue(Extra.Proportion);

    m_BorderLeft->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderLeft);
    m_BorderRight->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderRight);
    m_BorderTop->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderTop);
    m_BorderBottom->SetValue(Extra.Flags & wxsSizerFlagsProperty::BorderBottom);
    m_SizeExpand->SetValue(Extra.Flags & wxsSizerFlagsProperty::Expand);
    m_SizeShaped->SetValue(Extra.Flags & wxsSizerFlagsProperty::Shaped);

    if ( Extra.Flags & wxsSizerFlagsProperty::AlignBottom )
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            m_Placement->SetSelection(8);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            m_Placement->SetSelection(7);
        }
        else
        {
            m_Placement->SetSelection(6);
        }
    }
    else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterVertical )
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            m_Placement->SetSelection(5);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            m_Placement->SetSelection(4);
        }
        else
        {
            m_Placement->SetSelection(3);
        }
    }
    else
    {
        if ( Extra.Flags & wxsSizerFlagsProperty::AlignRight )
        {
            m_Placement->SetSelection(2);
        }
        else if ( Extra.Flags & wxsSizerFlagsProperty::AlignCenterHorizontal )
        {
            m_Placement->SetSelection(1);
        }
        else
        {
            m_Placement->SetSelection(0);
        }
    }

    m_Border->SetValue(Extra.Border.Value);
    m_BorderDU->SetValue(Extra.Border.DialogUnits);
}

wxsSettings::~wxsSettings()
{
    //(*Destroy(wxsSettings)
    //*)
}

void wxsSettings::OnDragTargetColClick(cb_unused wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,m_DragTargetCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        m_DragTargetCol->SetBackgroundColour(Col);
    }
}

void wxsSettings::OnDragParentColClick(cb_unused wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,m_DragParentCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        m_DragParentCol->SetBackgroundColour(Col);
    }
}

void wxsSettings::OnApply()
{
    wxColour ColTarget = m_DragTargetCol->GetBackgroundColour();
    wxColour ColParent = m_DragParentCol->GetBackgroundColour();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    cfg->Write(_T("/uniqueids"),(bool)m_UniqueIDsOnly->GetValue());
    cfg->Write(_T("/browserplacements"),(int)(m_BrowserPlacements->GetSelection()));
    cfg->Write(_T("/dragtargetcol"),(int)((((int)ColTarget.Red())<<16) + (((long)ColTarget.Green())<<8) + (long)ColTarget.Blue()));
    cfg->Write(_T("/dragparentcol"),(int)((((int)ColParent.Red())<<16) + (((long)ColParent.Green())<<8) + (long)ColParent.Blue()));
    cfg->Write(_T("/dragassisttype"),(int)m_DragAssistType->GetSelection());
    cfg->Write(_T("/paletteiconsize"),(int)(m_Icons16->GetValue()?16:32));
    cfg->Write(_T("/tooliconsize"),(int)(m_TIcons16->GetValue()?16:32));

    int GridSize = m_GridSize->GetValue();
    if ( GridSize<2 ) GridSize = 2;

    if ( m_UseGrid->GetValue() )
    {
        cfg->Write(_T("/gridsize"),GridSize);
    }
    else
    {
        cfg->Write(_T("/gridsize"),-GridSize);
    }

    cfg->Write(_T("/continousinsert"),m_Continous->GetValue());
    cfg->Write(_T("/removeprefix"),m_RemovePrefix->GetValue());

    cfg->Write(_T("/useI18N"),m_UseI18N->GetValue());
    if      (m_NoneI18N_T->GetValue())  cfg->Write(_T("/noneI18N"),0);
    else if (m_NoneI18NwxT->GetValue()) cfg->Write(_T("/noneI18N"),1);
    else if (m_NoneI18N->GetValue())    cfg->Write(_T("/noneI18N"),2);

    int Flags =  (m_BorderLeft->IsChecked()   ? wxsSizerFlagsProperty::BorderLeft   : 0) |
                 (m_BorderRight->IsChecked()  ? wxsSizerFlagsProperty::BorderRight  : 0) |
                 (m_BorderTop->IsChecked()    ? wxsSizerFlagsProperty::BorderTop    : 0) |
                 (m_BorderBottom->IsChecked() ? wxsSizerFlagsProperty::BorderBottom : 0) |
                 (m_SizeExpand->IsChecked()   ? wxsSizerFlagsProperty::Expand       : 0) |
                 (m_SizeShaped->IsChecked()   ? wxsSizerFlagsProperty::Shaped       : 0);

    switch ( m_Placement->GetSelection() )
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
        default: break;
    }

    cfg->Write(_T("/defsizer/proportion"), (int)m_Proportion->GetValue());
    cfg->Write(_T("/defsizer/flags"),      Flags);
    cfg->Write(_T("/defsizer/border"),     (int)m_Border->GetValue());
    cfg->Write(_T("/defsizer/borderdu"),   (bool)m_BorderDU->GetValue());

    wxsItemEditor::ConfigChanged();
}

void wxsSettings::OnUseGridClick(cb_unused wxCommandEvent& event)
{
    m_GridSize->Enable(m_UseGrid->GetValue());
}
