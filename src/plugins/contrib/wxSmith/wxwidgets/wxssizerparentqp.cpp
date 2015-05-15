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

#include "wxssizerparentqp.h"

//(*InternalHeaders(wxsSizerParentQP)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(wxsSizerParentQP)
const long wxsSizerParentQP::ID_CHECKBOX1 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX2 = wxNewId();
const long wxsSizerParentQP::ID_CHECKBOX8 = wxNewId();
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
const long wxsSizerParentQP::ID_STATICLINE1 = wxNewId();
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
    wxStaticBoxSizer* StaticBoxSizer2;
    wxGridSizer* GridSizer1;
    wxGridSizer* GridSizer2;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxBoxSizer* BoxSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Border"));
    GridSizer1 = new wxGridSizer(3, 3, 0, 0);
    GridSizer1->Add(-1,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    BrdTop = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    BrdTop->SetValue(false);
    GridSizer1->Add(BrdTop, 0, wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(-1,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    BrdLeft = new wxCheckBox(this, ID_CHECKBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    BrdLeft->SetValue(false);
    GridSizer1->Add(BrdLeft, 0, wxALIGN_CENTER_VERTICAL, 5);
    BrdAll = new wxCheckBox(this, ID_CHECKBOX8, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    BrdAll->SetValue(false);
    GridSizer1->Add(BrdAll, 0, wxALIGN_CENTER_VERTICAL, 5);
    BrdRight = new wxCheckBox(this, ID_CHECKBOX3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    BrdRight->SetValue(false);
    GridSizer1->Add(BrdRight, 0, wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(-1,-1,0, wxALIGN_CENTER_VERTICAL, 5);
    BrdBottom = new wxCheckBox(this, ID_CHECKBOX4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    BrdBottom->SetValue(false);
    GridSizer1->Add(BrdBottom, 0, wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(GridSizer1, 0, wxEXPAND, 5);
    BrdSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(38,-1)), 0, 0, 100, 0, _T("ID_SPINCTRL1"));
    BrdSize->SetValue(_T("0"));
    StaticBoxSizer1->Add(BrdSize, 0, wxTOP|wxEXPAND, 5);
    BrdDlg = new wxCheckBox(this, ID_CHECKBOX7, _("Dialog Units"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    BrdDlg->SetValue(false);
    StaticBoxSizer1->Add(BrdDlg, 0, wxTOP|wxEXPAND, 5);
    BoxSizer1->Add(StaticBoxSizer1, 0, wxEXPAND, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Placement"));
    GridSizer2 = new wxGridSizer(3, 3, 0, 0);
    PlaceLT = new wxRadioButton(this, ID_RADIOBUTTON4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON4"));
    GridSizer2->Add(PlaceLT, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceCT = new wxRadioButton(this, ID_RADIOBUTTON5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON5"));
    GridSizer2->Add(PlaceCT, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceRT = new wxRadioButton(this, ID_RADIOBUTTON6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON6"));
    GridSizer2->Add(PlaceRT, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceLC = new wxRadioButton(this, ID_RADIOBUTTON7, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON7"));
    GridSizer2->Add(PlaceLC, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceCC = new wxRadioButton(this, ID_RADIOBUTTON8, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON8"));
    GridSizer2->Add(PlaceCC, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceRC = new wxRadioButton(this, ID_RADIOBUTTON9, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON9"));
    GridSizer2->Add(PlaceRC, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceLB = new wxRadioButton(this, ID_RADIOBUTTON10, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON10"));
    GridSizer2->Add(PlaceLB, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceCB = new wxRadioButton(this, ID_RADIOBUTTON11, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON11"));
    GridSizer2->Add(PlaceCB, 0, wxALIGN_CENTER_VERTICAL, 5);
    PlaceRB = new wxRadioButton(this, ID_RADIOBUTTON12, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON12"));
    GridSizer2->Add(PlaceRB, 0, wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(GridSizer2, 0, wxEXPAND, 5);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    StaticBoxSizer2->Add(StaticLine1, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    PlaceShp = new wxCheckBox(this, ID_CHECKBOX6, _("Shaped"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    PlaceShp->SetValue(false);
    StaticBoxSizer2->Add(PlaceShp, 0, wxEXPAND, 5);
    PlaceExp = new wxCheckBox(this, ID_CHECKBOX5, _("Expand"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    PlaceExp->SetValue(false);
    StaticBoxSizer2->Add(PlaceExp, 0, wxTOP|wxEXPAND, 5);
    BoxSizer1->Add(StaticBoxSizer2, 0, wxEXPAND, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Proportion"));
    Proportion = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(38,-1)), 0, 0, 100, 0, _T("ID_SPINCTRL2"));
    Proportion->SetValue(_T("0"));
    StaticBoxSizer3->Add(Proportion, 0, wxEXPAND, 5);
    BoxSizer1->Add(StaticBoxSizer3, 0, wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdChange);
    Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsSizerParentQP::OnBrdAll);
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
    BrdAll    ->SetValue(   (m_Extra->Flags & wxsSizerFlagsProperty::BorderAll) != 0
                         || (   BrdLeft->IsChecked() && BrdRight->IsChecked()
                             && BrdTop->IsChecked()  && BrdBottom->IsChecked()) );
    BrdSize   ->SetValue(m_Extra->Border.Value);
    BrdDlg    ->SetValue(m_Extra->Border.DialogUnits);

    PlaceExp  ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::Expand) != 0);
    PlaceShp  ->SetValue((m_Extra->Flags & wxsSizerFlagsProperty::Shaped) != 0);

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
    m_Extra->Border.Value       = BrdSize->GetValue();
    m_Extra->Border.DialogUnits = BrdDlg->GetValue();

    if ( PlaceExp ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::Expand;
    if ( PlaceShp ->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::Shaped;

    if ( PlaceLT->GetValue() || PlaceCT->GetValue() || PlaceRT->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignTop;
    if ( PlaceLC->GetValue() || PlaceCC->GetValue() || PlaceRC->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignCenterVertical;
    if ( PlaceLB->GetValue() || PlaceCB->GetValue() || PlaceRB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignBottom;
    if ( PlaceLT->GetValue() || PlaceLC->GetValue() || PlaceLB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignLeft;
    if ( PlaceCT->GetValue() || PlaceCC->GetValue() || PlaceCB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal;
    if ( PlaceRT->GetValue() || PlaceRC->GetValue() || PlaceRB->GetValue() ) m_Extra->Flags |= wxsSizerFlagsProperty::AlignRight;

    m_Extra->Proportion         = Proportion->GetValue();

    NotifyChange();
}

wxsSizerParentQP::~wxsSizerParentQP()
{
    //(*Destroy(wxsSizerParentQP)
    //*)
}

void wxsSizerParentQP::OnBrdDlgChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerParentQP::Update()
{
    ReadData();
}

void wxsSizerParentQP::OnBrdAll(wxCommandEvent& event)
{
    const bool ALLCHECKED(event.IsChecked());
    BrdLeft->SetValue(ALLCHECKED);
    BrdRight->SetValue(ALLCHECKED);
    BrdTop->SetValue(ALLCHECKED);
    BrdBottom->SetValue(ALLCHECKED);
    SaveData();
}
