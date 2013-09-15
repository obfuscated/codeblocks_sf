/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiManagerParentQP.h"

#include <prep.h>

//(*InternalHeaders(wxsAuiManagerParentQP)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(wxsAuiManagerParentQP)
const long wxsAuiManagerParentQP::ID_STATICTEXT4 = wxNewId();
const long wxsAuiManagerParentQP::ID_TEXTCTRL1 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX18 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX15 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX20 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX21 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX19 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX22 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHOICE1 = wxNewId();
const long wxsAuiManagerParentQP::ID_TEXTCTRL2 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX7 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX9 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX11 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX10 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX12 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX6 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX8 = wxNewId();
const long wxsAuiManagerParentQP::ID_STATICTEXT1 = wxNewId();
const long wxsAuiManagerParentQP::ID_SPINCTRL1 = wxNewId();
const long wxsAuiManagerParentQP::ID_STATICLINE1 = wxNewId();
const long wxsAuiManagerParentQP::ID_STATICTEXT2 = wxNewId();
const long wxsAuiManagerParentQP::ID_SPINCTRL2 = wxNewId();
const long wxsAuiManagerParentQP::ID_STATICLINE2 = wxNewId();
const long wxsAuiManagerParentQP::ID_STATICTEXT3 = wxNewId();
const long wxsAuiManagerParentQP::ID_SPINCTRL3 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX1 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX2 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX5 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX3 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX4 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX13 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX14 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX16 = wxNewId();
const long wxsAuiManagerParentQP::ID_CHECKBOX17 = wxNewId();
const long wxsAuiManagerParentQP::ID_RADIOBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsAuiManagerParentQP,wxsAdvQPPChild)
    //(*EventTable(wxsAuiManagerParentQP)
    //*)
END_EVENT_TABLE()

wxsAuiManagerParentQP::wxsAuiManagerParentQP(wxsAdvQPP* parent, wxsAuiPaneInfoExtra* Extra, cb_unused wxWindowID id):
    wxsAdvQPPChild(parent,_("AuiManager")),
    m_Extra(Extra)
{
    //(*Initialize(wxsAuiManagerParentQP)
    wxStaticBoxSizer* StaticBoxSizer2;
    wxGridSizer* GridSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxBoxSizer* BoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer7;
    wxStaticBoxSizer* StaticBoxSizer5;
    wxGridSizer* GridSizer3;
    wxFlexGridSizer* FlexGridSizer7;
    wxBoxSizer* BoxSizer2;
    wxFlexGridSizer* FlexGridSizer4;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxFlexGridSizer* FlexGridSizer6;
    wxFlexGridSizer* FlexGridSizer3;
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxBoxSizer* BoxSizer1;
    wxFlexGridSizer* FlexGridSizer5;
    wxBoxSizer* BoxSizer5;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, wxID_ANY, wxPoint(-1,-1), wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL, this, _("General"));
    FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer4->AddGrowableCol(1);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer4->Add(StaticText4, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    Name = new wxTextCtrl(this, ID_TEXTCTRL1, _("Pane name"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer4->Add(Name, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Visible = new wxCheckBox(this, ID_CHECKBOX18, _("Visible"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX18"));
    Visible->SetValue(false);
    FlexGridSizer4->Add(Visible, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Resizable = new wxCheckBox(this, ID_CHECKBOX15, _("Resizable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX15"));
    Resizable->SetValue(false);
    FlexGridSizer4->Add(Resizable, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Movable = new wxCheckBox(this, ID_CHECKBOX20, _("Movable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX20"));
    Movable->SetValue(false);
    FlexGridSizer4->Add(Movable, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Floatable = new wxCheckBox(this, ID_CHECKBOX21, _("Floatable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX21"));
    Floatable->SetValue(false);
    FlexGridSizer4->Add(Floatable, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PaneBorder = new wxCheckBox(this, ID_CHECKBOX19, _("Border"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX19"));
    PaneBorder->SetValue(false);
    FlexGridSizer4->Add(PaneBorder, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DestroyOnClose = new wxCheckBox(this, ID_CHECKBOX22, _("Destroy on close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX22"));
    DestroyOnClose->SetValue(false);
    FlexGridSizer4->Add(DestroyOnClose, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(FlexGridSizer4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer4, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer7 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Standard pane type"));
    StandardPane = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    StandardPane->SetSelection( StandardPane->Append(_("None")) );
    StandardPane->Append(_("Default pane"));
    StandardPane->Append(_("Center pane"));
    StandardPane->Append(_("Toolbar pane"));
    StaticBoxSizer7->Add(StandardPane, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer7, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Caption"));
    FlexGridSizer5 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer5->AddGrowableCol(0);
    FlexGridSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer7->AddGrowableCol(0);
    Caption = new wxTextCtrl(this, ID_TEXTCTRL2, _("Caption"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer7->Add(Caption, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CaptionVisible = new wxCheckBox(this, ID_CHECKBOX7, _("Visible"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    CaptionVisible->SetValue(false);
    FlexGridSizer7->Add(CaptionVisible, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(FlexGridSizer7, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Buttons"));
    FlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer6->AddGrowableCol(0);
    FlexGridSizer6->AddGrowableCol(1);
    MinimizeButton = new wxCheckBox(this, ID_CHECKBOX9, _("Minimize"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
    MinimizeButton->SetValue(false);
    FlexGridSizer6->Add(MinimizeButton, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PinButton = new wxCheckBox(this, ID_CHECKBOX11, _("Pin"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
    PinButton->SetValue(false);
    FlexGridSizer6->Add(PinButton, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    MaximizeButton = new wxCheckBox(this, ID_CHECKBOX10, _("Maximize"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
    MaximizeButton->SetValue(false);
    FlexGridSizer6->Add(MaximizeButton, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CloseButton = new wxCheckBox(this, ID_CHECKBOX12, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
    CloseButton->SetValue(false);
    FlexGridSizer6->Add(CloseButton, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer5->Add(FlexGridSizer6, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer5, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer5, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer3, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Dock"));
    FlexGridSizer3 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer3->AddGrowableCol(0);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Docked = new wxCheckBox(this, ID_CHECKBOX6, _("Docked"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    Docked->SetValue(false);
    BoxSizer2->Add(Docked, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DockFixed = new wxCheckBox(this, ID_CHECKBOX8, _("Fixed"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    DockFixed->SetValue(false);
    BoxSizer2->Add(DockFixed, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(BoxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2 = new wxFlexGridSizer(0, 5, 0, 0);
    FlexGridSizer2->AddGrowableCol(0);
    FlexGridSizer2->AddGrowableCol(2);
    FlexGridSizer2->AddGrowableCol(4);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Layer:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer1->Add(StaticText1, 0, wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Layer = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL1"));
    Layer->SetValue(_T("0"));
    Layer->SetMinSize(wxSize(0,-1));
    BoxSizer1->Add(Layer, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_VERTICAL, _T("ID_STATICLINE1"));
    FlexGridSizer2->Add(StaticLine1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Row:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer4->Add(StaticText2, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    Row = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL2"));
    Row->SetValue(_T("0"));
    Row->SetMinSize(wxSize(0,-1));
    BoxSizer4->Add(Row, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_VERTICAL, _T("ID_STATICLINE2"));
    FlexGridSizer2->Add(StaticLine2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Position:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer5->Add(StaticText3, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Position = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, -10000, 10000, 0, _T("ID_SPINCTRL3"));
    Position->SetValue(_T("0"));
    Position->SetMinSize(wxSize(0,-1));
    BoxSizer5->Add(Position, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer5, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(FlexGridSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Direction"));
    GridSizer1 = new wxGridSizer(0, 3, 0, 0);
    GridSizer1->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DockTop = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    DockTop->SetValue(false);
    GridSizer1->Add(DockTop, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DockLeft = new wxCheckBox(this, ID_CHECKBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    DockLeft->SetValue(false);
    GridSizer1->Add(DockLeft, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    DockCenter = new wxCheckBox(this, ID_CHECKBOX5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    DockCenter->SetValue(false);
    GridSizer1->Add(DockCenter, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    DockRight = new wxCheckBox(this, ID_CHECKBOX3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    DockRight->SetValue(false);
    GridSizer1->Add(DockRight, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DockBottom = new wxCheckBox(this, ID_CHECKBOX4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    DockBottom->SetValue(false);
    GridSizer1->Add(DockBottom, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(GridSizer1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer3->Add(StaticBoxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Dockable"));
    GridSizer3 = new wxGridSizer(0, 3, 0, 0);
    GridSizer3->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TopDockable = new wxCheckBox(this, ID_CHECKBOX13, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
    TopDockable->SetValue(false);
    GridSizer3->Add(TopDockable, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer3->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    LeftDockable = new wxCheckBox(this, ID_CHECKBOX14, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX14"));
    LeftDockable->SetValue(false);
    GridSizer3->Add(LeftDockable, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer3->Add(-1,-1,1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    RightDockable = new wxCheckBox(this, ID_CHECKBOX16, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX16"));
    RightDockable->SetValue(false);
    GridSizer3->Add(RightDockable, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer3->Add(0,0,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BottomDockable = new wxCheckBox(this, ID_CHECKBOX17, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX17"));
    BottomDockable->SetValue(false);
    GridSizer3->Add(BottomDockable, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer6->Add(GridSizer3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer3->Add(StaticBoxSizer6, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(BoxSizer3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(FlexGridSizer3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    wxString __wxRadioBoxChoices_1[3] =
    {
    	_("None"),
    	_("Default"),
    	_("Top")
    };
    Gripper = new wxRadioBox(this, ID_RADIOBOX1, _("Gripper"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
    FlexGridSizer1->Add(Gripper, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnNameChange);
    Connect(ID_CHECKBOX18,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHECKBOX15,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHECKBOX20,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHECKBOX21,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHECKBOX19,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHECKBOX22,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGeneralChange);
    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnStandardPaneChange);
    Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionChange);
    Connect(ID_CHECKBOX7,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionVisibleClick);
    Connect(ID_CHECKBOX9,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionButtonClick);
    Connect(ID_CHECKBOX11,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionButtonClick);
    Connect(ID_CHECKBOX10,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionButtonClick);
    Connect(ID_CHECKBOX12,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnCaptionButtonClick);
    Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockChange);
    Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockChange);
    Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockSiteChange);
    Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockSiteChange);
    Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockSiteChange);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockDirectionChange);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockDirectionChange);
    Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockDirectionChange);
    Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockDirectionChange);
    Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockDirectionChange);
    Connect(ID_CHECKBOX13,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockableChange);
    Connect(ID_CHECKBOX14,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockableChange);
    Connect(ID_CHECKBOX16,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockableChange);
    Connect(ID_CHECKBOX17,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnDockableChange);
    Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&wxsAuiManagerParentQP::OnGripperSelect);
    //*)

    ReadData();
}

wxsAuiManagerParentQP::~wxsAuiManagerParentQP()
{
    //(*Destroy(wxsAuiManagerParentQP)
    //*)
}

void wxsAuiManagerParentQP::ReadData()
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    //{General
    Name->          SetValue( m_Extra->m_Name );
    Visible->       SetValue( m_Extra->m_Visible );
    Resizable->     SetValue( m_Extra->m_Resizable );
    PaneBorder->    SetValue( m_Extra->m_PaneBorder );
    Movable->       SetValue( m_Extra->m_Movable );
    Floatable->     SetValue( m_Extra->m_Floatable );
    DestroyOnClose->SetValue( m_Extra->m_DestroyOnClose );
    //}General

    //Standard pane type
    StandardPane->SetSelection( m_Extra->m_StandardPane );

    //{Caption
    CaptionVisible->SetValue( m_Extra->m_CaptionVisible );
    Caption->       SetValue( m_Extra->m_Caption );
    MinimizeButton->SetValue( m_Extra->m_MinimizeButton );
    MaximizeButton->SetValue( m_Extra->m_MaximizeButton );
    PinButton->     SetValue( m_Extra->m_PinButton );
    CloseButton->   SetValue( m_Extra->m_CloseButton );
    //}Caption

    //Gripper
    switch ( m_Extra->m_Gripper )
    {
        case 0:
            Gripper->SetSelection(0);
            break;

        case wxLEFT:
            Gripper->SetSelection(1);
            break;

        case wxTOP:
            Gripper->SetSelection(2);
            break;

        default:
            break;
    }

    //{Dock
    Docked->   SetValue( m_Extra->m_Docked );
    DockFixed->SetValue( m_Extra->m_DockFixed );

    Layer->   SetValue( wxString::Format(_T("%ld"),m_Extra->m_Layer) );
    Row->     SetValue( wxString::Format(_T("%ld"),m_Extra->m_Row) );
    Position->SetValue( wxString::Format(_T("%ld"),m_Extra->m_Position) );

    switch ( m_Extra->m_DockDirection )
    {
        case wxAUI_DOCK_TOP:
            DockTop->   SetValue(true);
            DockBottom->SetValue(false);
            DockLeft->  SetValue(false);
            DockRight-> SetValue(false);
            DockCenter->SetValue(false);
            break;

        case wxAUI_DOCK_BOTTOM:
            DockTop->   SetValue(false);
            DockBottom->SetValue(true);
            DockLeft->  SetValue(false);
            DockRight-> SetValue(false);
            DockCenter->SetValue(false);
            break;

        case wxAUI_DOCK_RIGHT:
            DockTop->   SetValue(false);
            DockBottom->SetValue(false);
            DockLeft->  SetValue(false);
            DockRight-> SetValue(true);
            DockCenter->SetValue(false);
            break;

        case wxAUI_DOCK_CENTER:
            DockTop->   SetValue(false);
            DockBottom->SetValue(false);
            DockLeft->  SetValue(false);
            DockRight-> SetValue(false);
            DockCenter->SetValue(true);
            break;

        case wxAUI_DOCK_LEFT:
        default:
            DockTop->   SetValue(false);
            DockBottom->SetValue(false);
            DockLeft->  SetValue(true);
            DockRight-> SetValue(false);
            DockCenter->SetValue(false);
    }

    long Dockable = m_Extra->m_DockableFlags;

    if ( Dockable == wxsAuiDockableProperty::Dockable )
    {
        TopDockable->   SetValue(true);
        BottomDockable->SetValue(true);
        LeftDockable->  SetValue(true);
        RightDockable-> SetValue(true);
    }
    else
    {
        if ( Dockable & wxsAuiDockableProperty::TopDockable    ) TopDockable->   SetValue( true );
        if ( Dockable & wxsAuiDockableProperty::BottomDockable ) BottomDockable->SetValue( true );
        if ( Dockable & wxsAuiDockableProperty::LeftDockable   ) LeftDockable->  SetValue( true );
        if ( Dockable & wxsAuiDockableProperty::RightDockable  ) RightDockable-> SetValue( true );
    }
    //}Dock
}

void wxsAuiManagerParentQP::Update()
{
    ReadData();
}

void wxsAuiManagerParentQP::OnDockChange(cb_unused wxCommandEvent& event)
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    m_Extra->m_Docked    = Docked->GetValue();
    m_Extra->m_DockFixed = DockFixed->GetValue();

    NotifyChange();
}

void wxsAuiManagerParentQP::OnDockDirectionChange(cb_unused wxCommandEvent& event)
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    if ( DockTop->GetValue() && (m_Extra->m_DockDirection != wxAUI_DOCK_TOP) )
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_TOP;
        m_Extra->m_DockableFlags |= wxsAuiDockableProperty::TopDockable;
        DockBottom->SetValue(false);
        DockLeft->  SetValue(false);
        DockRight-> SetValue(false);
        DockCenter->SetValue(false);
    }
    else if ( DockBottom->GetValue() && (m_Extra->m_DockDirection != wxAUI_DOCK_BOTTOM) )
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_BOTTOM;
        m_Extra->m_DockableFlags |= wxsAuiDockableProperty::BottomDockable;
        DockLeft->  SetValue(false);
        DockRight-> SetValue(false);
        DockCenter->SetValue(false);
    }
    else if ( DockLeft->GetValue() && (m_Extra->m_DockDirection != wxAUI_DOCK_LEFT) )
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_LEFT;
        m_Extra->m_DockableFlags |= wxsAuiDockableProperty::LeftDockable;
        DockRight-> SetValue(false);
        DockCenter->SetValue(false);
    }
    else if ( DockRight->GetValue() && (m_Extra->m_DockDirection != wxAUI_DOCK_RIGHT) )
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_RIGHT;
        m_Extra->m_DockableFlags |= wxsAuiDockableProperty::RightDockable;
        DockCenter->SetValue(false);
    }
    else if ( DockCenter->GetValue() && (m_Extra->m_DockDirection != wxAUI_DOCK_CENTER) )
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_CENTER;
        m_Extra->m_DockableFlags = wxsAuiDockableProperty::Dockable;
    }
    else
    {
        m_Extra->m_DockDirection = wxAUI_DOCK_LEFT;
        m_Extra->m_DockableFlags |= wxsAuiDockableProperty::LeftDockable;
        m_Extra->m_Docked        = false;

        Docked->    SetValue(false);
        DockTop->   SetValue(false);
        DockBottom->SetValue(false);
        DockLeft->  SetValue(true);
        DockRight-> SetValue(false);
        DockCenter->SetValue(false);
    }

    NotifyChange();
}

void wxsAuiManagerParentQP::OnDockSiteChange(cb_unused wxSpinEvent& event)
{
    if ( !GetPropertyContainer() || !m_Extra ) return;

    m_Extra->m_Layer    = Layer->GetValue();
    m_Extra->m_Row      = Row->GetValue();
    m_Extra->m_Position = Position->GetValue();

    NotifyChange();
}

void wxsAuiManagerParentQP::OnNameChange(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_Name = Name->GetValue();
    NotifyChange();
}

void wxsAuiManagerParentQP::OnCaptionChange(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_Caption = Caption->GetValue();
    NotifyChange();
}

void wxsAuiManagerParentQP::OnCaptionButtonClick(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_MinimizeButton = MinimizeButton->GetValue();
    m_Extra->m_MaximizeButton = MaximizeButton->GetValue();
    m_Extra->m_PinButton      = PinButton->GetValue();
    m_Extra->m_CloseButton    = CloseButton->GetValue();

    NotifyChange();
}

void wxsAuiManagerParentQP::OnDockableChange(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    bool Top    = TopDockable->GetValue();
    bool Bottom = BottomDockable->GetValue();
    bool Left   = LeftDockable->GetValue();
    bool Right  = RightDockable->GetValue();

    if ( Top && Bottom && Left && Right ) m_Extra->m_DockableFlags = wxsAuiDockableProperty::Dockable;
    else
    {
        m_Extra->m_DockableFlags = 0;
        if ( Top    ) m_Extra->m_DockableFlags |= wxsAuiDockableProperty::TopDockable;
        if ( Bottom ) m_Extra->m_DockableFlags |= wxsAuiDockableProperty::BottomDockable;
        if ( Left   ) m_Extra->m_DockableFlags |= wxsAuiDockableProperty::LeftDockable;
        if ( Right  ) m_Extra->m_DockableFlags |= wxsAuiDockableProperty::RightDockable;
    }

    NotifyChange();
}

void wxsAuiManagerParentQP::OnGripperSelect(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    switch ( Gripper->GetSelection() )
    {
        case 1:
            m_Extra->m_Gripper = wxLEFT;
            break;

        case 2:
            m_Extra->m_Gripper = wxTOP;

        case 0: // fall-though
        default:
            m_Extra->m_Gripper = 0;
            break;
    }

    NotifyChange();
}

void wxsAuiManagerParentQP::OnGeneralChange(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_Visible        = Visible->GetValue();
    m_Extra->m_Resizable      = Resizable->GetValue();
    m_Extra->m_PaneBorder     = PaneBorder->GetValue();
    m_Extra->m_Movable        = Movable->GetValue();
    m_Extra->m_Floatable      = Floatable->GetValue();
    m_Extra->m_DestroyOnClose = DestroyOnClose->GetValue();

    NotifyChange();
}

void wxsAuiManagerParentQP::OnCaptionVisibleClick(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_CaptionVisible = CaptionVisible->GetValue();

    NotifyChange();
}

void wxsAuiManagerParentQP::OnStandardPaneChange(cb_unused wxCommandEvent& event)
{
    if ( !m_Extra ) return;

    m_Extra->m_StandardPane = StandardPane->GetSelection();

    switch ( m_Extra->m_StandardPane )
    {
        case wxsAuiPaneInfoExtra::DefaultPane:
            m_Extra->m_DockableFlags  = wxsAuiDockableProperty::Dockable;
            m_Extra->m_Floatable      = true;
            m_Extra->m_Movable        = true;
            m_Extra->m_Resizable      = true;
            m_Extra->m_CaptionVisible = true;
            m_Extra->m_PaneBorder     = true;
            m_Extra->m_CloseButton    = true;
            break;

        case wxsAuiPaneInfoExtra::CenterPane:
            m_Extra->m_Docked         = true;
            m_Extra->m_Visible        = true;
            m_Extra->m_DockableFlags  = wxsAuiDockableProperty::Dockable;
            m_Extra->m_Floatable      = false;
            m_Extra->m_Movable        = false;
            m_Extra->m_Resizable      = true;
            m_Extra->m_PaneBorder     = true;
            m_Extra->m_CaptionVisible = false;
            m_Extra->m_Gripper        = 0;
            m_Extra->m_DestroyOnClose = false;
            m_Extra->m_DockFixed      = false;
            m_Extra->m_CloseButton    = false;
            m_Extra->m_MaximizeButton = false;
            m_Extra->m_MinimizeButton = false;
            m_Extra->m_PinButton      = false;
            m_Extra->m_DockDirection  = wxAUI_DOCK_CENTER;
            break;

        case wxsAuiPaneInfoExtra::ToolbarPane:
            m_Extra->m_DockableFlags  = wxsAuiDockableProperty::Dockable;
            m_Extra->m_Floatable      = true;
            m_Extra->m_Movable        = true;
            m_Extra->m_Resizable      = false;
            m_Extra->m_CaptionVisible = false;
            m_Extra->m_PaneBorder     = true;
            m_Extra->m_CloseButton    = true;
            m_Extra->m_Gripper        = wxLEFT;
            if ( m_Extra->m_Layer == 0 ) m_Extra->m_Layer = 10;

        default:
            break;
    }

    NotifyChange();
}
