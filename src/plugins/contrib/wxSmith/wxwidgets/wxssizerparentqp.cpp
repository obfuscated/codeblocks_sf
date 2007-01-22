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

#include "wxssizerparentqp.h"

//(*InternalHeaders(wxsSizerParentQP)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(wxsSizerParentQP)
const long wxsSizerParentQP::ID_CHECKBOX1 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX2 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX3 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX4 = wxNewId();
const long wxsSizerParentQP::ID_SPINCTRL1 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX7 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON4 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON5 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON6 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON7 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON8 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON9 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON10 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON11 = wxNewId();
const long wxsSizerParentQP::ID_RADIOBUTTON12 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX6 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX5 = wxNewId();
const long wxsSizerParentQP::ID_SPINCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsSizerParentQP,wxPanel)
    //(*EventTable(wxsSizerParentQP)
    //*)
END_EVENT_TABLE()

wxsSizerParentQP::wxsSizerParentQP(wxsAdvQPP* parent,wxsSizerExtra* Extra,wxWindowID id):
    wxsAdvQPPChild(parent,_("Sizer")),
    m_Extra(Extra)
{
	//(*Initialize(wxsSizerParentQP)
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxGridSizer* GridSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	FlexGridSizer1->AddGrowableCol(1);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Border"));
	FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
	FlexGridSizer2->AddGrowableCol(1);
	GridSizer1 = new wxGridSizer(0,3,0,0);
	GridSizer1->Add(0,0,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BrdTop = new wxCheckBox(this,ID_CHECKBOX1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
	GridSizer1->Add(BrdTop,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	GridSizer1->Add(0,0,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BrdLeft = new wxCheckBox(this,ID_CHECKBOX2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX2"));
	GridSizer1->Add(BrdLeft,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	GridSizer1->Add(0,0,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BrdRight = new wxCheckBox(this,ID_CHECKBOX3,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX3"));
	GridSizer1->Add(BrdRight,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	GridSizer1->Add(0,0,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BrdBottom = new wxCheckBox(this,ID_CHECKBOX4,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX4"));
	GridSizer1->Add(BrdBottom,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(GridSizer1,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BrdSize = new wxSpinCtrl(this,ID_SPINCTRL1,_("0"),wxDefaultPosition,wxSize(51,-1),0,0,100,0,_T("ID_SPINCTRL1"));
	BrdSize->SetValue(_("0"));
	FlexGridSizer2->Add(BrdSize,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,0);
	BrdDlg = new wxCheckBox(this,ID_CHECKBOX7,_("Dialog Units"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX7"));
	StaticBoxSizer1->Add(BrdDlg,0,wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Placement"));
	FlexGridSizer3 = new wxFlexGridSizer(0,0,0,0);
	GridSizer2 = new wxGridSizer(3,3,0,0);
	PlaceLT = new wxRadioButton(this,ID_RADIOBUTTON4,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON4"));
	GridSizer2->Add(PlaceLT,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceCT = new wxRadioButton(this,ID_RADIOBUTTON5,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON5"));
	GridSizer2->Add(PlaceCT,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceRT = new wxRadioButton(this,ID_RADIOBUTTON6,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON6"));
	GridSizer2->Add(PlaceRT,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceLC = new wxRadioButton(this,ID_RADIOBUTTON7,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON7"));
	GridSizer2->Add(PlaceLC,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceCC = new wxRadioButton(this,ID_RADIOBUTTON8,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON8"));
	GridSizer2->Add(PlaceCC,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceRC = new wxRadioButton(this,ID_RADIOBUTTON9,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON9"));
	GridSizer2->Add(PlaceRC,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceLB = new wxRadioButton(this,ID_RADIOBUTTON10,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON10"));
	GridSizer2->Add(PlaceLB,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceCB = new wxRadioButton(this,ID_RADIOBUTTON11,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON11"));
	GridSizer2->Add(PlaceCB,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	PlaceRB = new wxRadioButton(this,ID_RADIOBUTTON12,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON12"));
	GridSizer2->Add(PlaceRB,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer3->Add(GridSizer2,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	PlaceShp = new wxCheckBox(this,ID_CHECKBOX6,_("Shaped"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX6"));
	BoxSizer1->Add(PlaceShp,1,wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,2);
	PlaceExp = new wxCheckBox(this,ID_CHECKBOX5,_("Expand"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX5"));
	BoxSizer1->Add(PlaceExp,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer3->Add(BoxSizer1,1,wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,10);
	StaticBoxSizer2->Add(FlexGridSizer3,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL,this,_("Proportion"));
	Proportion = new wxSpinCtrl(this,ID_SPINCTRL2,_("0"),wxDefaultPosition,wxSize(65,21),0,0,100,0,_T("ID_SPINCTRL2"));
	Proportion->SetValue(_("0"));
	StaticBoxSizer3->Add(Proportion,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(StaticBoxSizer3,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
	Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
	Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdSizeChange);
	Connect(ID_CHECKBOX7,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdDlgChange);
	Connect(ID_RADIOBUTTON4,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON5,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON6,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON7,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON8,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON9,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON10,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON11,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_RADIOBUTTON12,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnPlaceChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxsSizerParentQP::OnProportionChange);
	//*)

    ReadData();
}

void wxsSizerParentQP::OnBrdChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::OnBrdSizeChange(wxSpinEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::OnPlaceChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::OnProportionChange(wxSpinEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::ReadData()
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    BrdLeft   ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::BorderLeft)   != 0 );
    BrdRight  ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::BorderRight)  != 0 );
    BrdTop    ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::BorderTop)    != 0 );
    BrdBottom ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::BorderBottom) != 0 );
    PlaceExp  ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::Expand) != 0);
    PlaceShp  ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::Shaped) != 0);
    BrdSize   ->SetValue(m_Extra->Border.Value);
    BrdDlg    ->SetValue(m_Extra->Border.DialogUnits);
    Proportion->SetValue(m_Extra->Proportion);

    if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignBottom )
    {
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRB->SetValue(true); else
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCB->SetValue(true); else
                                                                           PlaceLB->SetValue(true);
    }
    else if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignCenterVertical )
    {
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRC->SetValue(true); else
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCC->SetValue(true); else
                                                                           PlaceLC->SetValue(true);
    }
    else
    {
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRT->SetValue(true); else
        if ( m_Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCT->SetValue(true); else
                                                                           PlaceLT->SetValue(true);
    }
}

void wxsSizerParentQP::SaveData()
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    m_Extra->Flags = 0;
    if ( BrdLeft  ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::BorderLeft;
    if ( BrdRight ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::BorderRight;
    if ( BrdTop   ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::BorderTop;
    if ( BrdBottom->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::BorderBottom;
    if ( PlaceExp ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::Expand;
    if ( PlaceShp ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::Shaped;
    m_Extra->Border.Value       = BrdSize->GetValue();
    m_Extra->Border.DialogUnits = BrdDlg->GetValue();
    m_Extra->Proportion         = Proportion->GetValue();
    if ( PlaceLT->GetValue() || PlaceCT->GetValue() || PlaceRT->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignTop;
    if ( PlaceLC->GetValue() || PlaceCC->GetValue() || PlaceRC->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignCenterVertical;
    if ( PlaceLB->GetValue() || PlaceCB->GetValue() || PlaceRB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignBottom;
    if ( PlaceLT->GetValue() || PlaceLC->GetValue() || PlaceLB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignLeft;
    if ( PlaceCT->GetValue() || PlaceCC->GetValue() || PlaceCB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal;
    if ( PlaceRT->GetValue() || PlaceRC->GetValue() || PlaceRB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignRight;

    NotifyChange();
}

void wxsSizerParentQP::OnBrdDlgChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::Update()
{
    ReadData();
}
