#include "wxssizerparentqp.h"

BEGIN_EVENT_TABLE(wxsSizerParentQP,wxPanel)
    //(*EventTable(wxsSizerParentQP)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX2,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX3,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX4,wxsSizerParentQP::OnBrdChange)
    EVT_SPINCTRL(ID_SPINCTRL1,wxsSizerParentQP::OnBrdSizeChange)
    EVT_CHECKBOX(ID_CHECKBOX7,wxsSizerParentQP::OnBrdDlgChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON4,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON5,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON6,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON7,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON8,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON9,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON10,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON11,wxsSizerParentQP::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON12,wxsSizerParentQP::OnPlaceChange)
    EVT_CHECKBOX(ID_CHECKBOX6,wxsSizerParentQP::OnPlaceChange)
    EVT_CHECKBOX(ID_CHECKBOX5,wxsSizerParentQP::OnPlaceChange)
    EVT_SPINCTRL(ID_SPINCTRL2,wxsSizerParentQP::OnProportionChange)
    //*)
END_EVENT_TABLE()

wxsSizerParentQP::wxsSizerParentQP(wxsAdvQPP* parent,wxsSizerExtra* _Extra,wxWindowID id):
    wxsAdvQPPChild(parent,_("Sizer")),
    Extra(_Extra)
{
	//(*Initialize(wxsSizerParentQP)
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxGridSizer* GridSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Border"));
	FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
	FlexGridSizer2->AddGrowableCol(1);
	GridSizer1 = new wxGridSizer(0,3,0,0);
	BrdTop = new wxCheckBox(this,ID_CHECKBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	BrdTop->SetValue(false);
	BrdLeft = new wxCheckBox(this,ID_CHECKBOX2,_T(""),wxDefaultPosition,wxDefaultSize,0);
	BrdLeft->SetValue(false);
	BrdRight = new wxCheckBox(this,ID_CHECKBOX3,_T(""),wxDefaultPosition,wxDefaultSize,0);
	BrdRight->SetValue(false);
	BrdBottom = new wxCheckBox(this,ID_CHECKBOX4,_T(""),wxDefaultPosition,wxDefaultSize,0);
	BrdBottom->SetValue(false);
	GridSizer1->Add(0,0,1);
	GridSizer1->Add(BrdTop,1,wxALIGN_CENTER,5);
	GridSizer1->Add(0,0,1);
	GridSizer1->Add(BrdLeft,1,wxALIGN_CENTER,5);
	GridSizer1->Add(0,0,1);
	GridSizer1->Add(BrdRight,1,wxALIGN_CENTER,5);
	GridSizer1->Add(0,0,1);
	GridSizer1->Add(BrdBottom,1,wxALIGN_CENTER,5);
	BrdSize = new wxSpinCtrl(this,ID_SPINCTRL1,_("0"),wxDefaultPosition,wxSize(51,-1),0,0,100);
	FlexGridSizer2->Add(GridSizer1,1,wxALIGN_CENTER,5);
	FlexGridSizer2->Add(BrdSize,1,wxALL|wxALIGN_CENTER,5);
	BrdDlg = new wxCheckBox(this,ID_CHECKBOX7,_("Dialog Units"),wxDefaultPosition,wxDefaultSize,0);
	BrdDlg->SetValue(false);
	StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER,0);
	StaticBoxSizer1->Add(BrdDlg,0,wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Placement"));
	FlexGridSizer3 = new wxFlexGridSizer(0,0,0,0);
	GridSizer2 = new wxGridSizer(3,3,0,0);
	PlaceLT = new wxRadioButton(this,ID_RADIOBUTTON4,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceLT->SetValue(false);
	PlaceCT = new wxRadioButton(this,ID_RADIOBUTTON5,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceCT->SetValue(false);
	PlaceRT = new wxRadioButton(this,ID_RADIOBUTTON6,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceRT->SetValue(false);
	PlaceLC = new wxRadioButton(this,ID_RADIOBUTTON7,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceLC->SetValue(false);
	PlaceCC = new wxRadioButton(this,ID_RADIOBUTTON8,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceCC->SetValue(false);
	PlaceRC = new wxRadioButton(this,ID_RADIOBUTTON9,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceRC->SetValue(false);
	PlaceLB = new wxRadioButton(this,ID_RADIOBUTTON10,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceLB->SetValue(false);
	PlaceCB = new wxRadioButton(this,ID_RADIOBUTTON11,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceCB->SetValue(false);
	PlaceRB = new wxRadioButton(this,ID_RADIOBUTTON12,_T(""),wxDefaultPosition,wxDefaultSize,0);
	PlaceRB->SetValue(false);
	GridSizer2->Add(PlaceLT,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceCT,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceRT,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceLC,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceCC,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceRC,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceLB,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceCB,1,wxALIGN_CENTER,5);
	GridSizer2->Add(PlaceRB,1,wxALIGN_CENTER,5);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	PlaceShp = new wxCheckBox(this,ID_CHECKBOX6,_("Shaped"),wxDefaultPosition,wxDefaultSize,0);
	PlaceShp->SetValue(false);
	PlaceExp = new wxCheckBox(this,ID_CHECKBOX5,_("Expand"),wxDefaultPosition,wxDefaultSize,0);
	PlaceExp->SetValue(false);
	BoxSizer1->Add(PlaceShp,1,wxTOP|wxALIGN_CENTER|wxEXPAND,2);
	BoxSizer1->Add(PlaceExp,1,wxALIGN_CENTER,0);
	FlexGridSizer3->Add(GridSizer2,1,wxALIGN_CENTER,5);
	FlexGridSizer3->Add(BoxSizer1,1,wxLEFT|wxALIGN_CENTER,10);
	StaticBoxSizer2->Add(FlexGridSizer3,1,wxALIGN_CENTER,0);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL,this,_("Proportion"));
	Proportion = new wxSpinCtrl(this,ID_SPINCTRL2,_("0"),wxDefaultPosition,wxSize(65,21),0,0,100);
	StaticBoxSizer3->Add(Proportion,1,wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(StaticBoxSizer3,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	this->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
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
    if ( !GetPropertyContainer() || !Extra ) return;
    
    BrdLeft   ->SetValue((Extra->Flags & wxsSizerFlagsProperty::BorderLeft)   != 0 );
    BrdRight  ->SetValue((Extra->Flags & wxsSizerFlagsProperty::BorderRight)  != 0 );
    BrdTop    ->SetValue((Extra->Flags & wxsSizerFlagsProperty::BorderTop)    != 0 );
    BrdBottom ->SetValue((Extra->Flags & wxsSizerFlagsProperty::BorderBottom) != 0 );
    PlaceExp  ->SetValue((Extra->Flags & wxsSizerFlagsProperty::Expand) != 0);
    PlaceShp  ->SetValue((Extra->Flags & wxsSizerFlagsProperty::Shaped) != 0);
    BrdSize   ->SetValue(Extra->Border);
    BrdDlg    ->SetValue(Extra->BorderInDU);
    Proportion->SetValue(Extra->Proportion);

    if ( Extra->Flags & wxsSizerFlagsProperty::AlignBottom )
    {
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRB->SetValue(true); else
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCB->SetValue(true); else
                                                                           PlaceLB->SetValue(true);
    }
    else if ( Extra->Flags & wxsSizerFlagsProperty::AlignCenterVertical )
    {
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRC->SetValue(true); else
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCC->SetValue(true); else
                                                                           PlaceLC->SetValue(true);
    }
    else
    {
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignRight )            PlaceRT->SetValue(true); else
        if ( Extra->Flags & wxsSizerFlagsProperty::AlignCenterHorizontal ) PlaceCT->SetValue(true); else
                                                                           PlaceLT->SetValue(true);
    }
}

void wxsSizerParentQP::SaveData()
{
    if ( !GetPropertyContainer() || !Extra ) return;
    
    Extra->Flags = 0;
    if ( BrdLeft  ->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::BorderLeft;
    if ( BrdRight ->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::BorderRight;
    if ( BrdTop   ->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::BorderTop;
    if ( BrdBottom->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::BorderBottom;
    if ( PlaceExp ->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::Expand;
    if ( PlaceShp ->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::Shaped;
    Extra->Border     = BrdSize->GetValue();
    Extra->BorderInDU = BrdDlg->GetValue();
    Extra->Proportion = Proportion->GetValue();
    if ( PlaceLT->GetValue() || PlaceCT->GetValue() || PlaceRT->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignTop;
    if ( PlaceLC->GetValue() || PlaceCC->GetValue() || PlaceRC->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignCenterVertical;
    if ( PlaceLB->GetValue() || PlaceCB->GetValue() || PlaceRB->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignBottom;
    if ( PlaceLT->GetValue() || PlaceLC->GetValue() || PlaceLB->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignLeft;
    if ( PlaceCT->GetValue() || PlaceCC->GetValue() || PlaceCB->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignCenterHorizontal;
    if ( PlaceRT->GetValue() || PlaceRC->GetValue() || PlaceRB->GetValue() ) Extra->Flags |= wxsSizerFlagsProperty::AlignRight;
    
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
