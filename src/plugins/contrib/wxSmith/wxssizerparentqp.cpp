#include "wxsheaders.h"
#include "wxssizerparentqp.h"
#include "wxsdefsizer.h"

BEGIN_EVENT_TABLE(wxsSizerParentQP,wxsQPPPanel)
//(*EventTable(wxsSizerParentQP)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX2,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX3,wxsSizerParentQP::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX4,wxsSizerParentQP::OnBrdChange)
    EVT_SPINCTRL(ID_SPINCTRL1,wxsSizerParentQP::OnBrdSizeChange)
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
    EVT_TIMER(-1,wxsSizerParentQP::OnTimer)
END_EVENT_TABLE()

wxsSizerParentQP::wxsSizerParentQP(wxWindow* parent,wxsWidget* Modified,wxsSizerExtraParams* _Params,wxWindowID id):
    wxsQPPPanel(Modified),
    Widget(Modified),
    Params(_Params),
    Timer(this)
{
	//(*Initialize(wxsSizerParentQP)
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxGridSizer* GridSizer1;
    wxStaticBoxSizer* StaticBoxSizer2;

    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Sizer options"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxSUNKEN_BORDER);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Border"));
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
    BrdSize = new wxSpinCtrl(this,ID_SPINCTRL1,_("0"),wxDefaultPosition,wxSize(51,-1),0, 0 ,100);
    FlexGridSizer2->Add(GridSizer1,1,wxALIGN_CENTER,5);
    FlexGridSizer2->Add(BrdSize,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
    StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER,0);
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
    Proportion = new wxSpinCtrl(this,ID_SPINCTRL2,_("0"),wxDefaultPosition,wxSize(65,21),0, 0 ,100);
    StaticBoxSizer3->Add(Proportion,1,wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticText1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticBoxSizer3,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    this->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    //*)

    ReadData();
    Timer.Start(500);
}

wxsSizerParentQP::~wxsSizerParentQP()
{
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
    if ( !Valid() ) return;
    if ( !Widget || !Params ) return;

    if ( GetBorderFlags() != Params->BorderFlags )
    {
        BrdLeft  ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Left)   != 0 );
        BrdRight ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Right)  != 0 );
        BrdTop   ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Top)    != 0 );
        BrdBottom->SetValue((Params->BorderFlags & wxsSizerExtraParams::Bottom) != 0 );
    }

    if ( GetBorderSize() != Params->Border )
    {
        BrdSize->SetValue(Params->Border);
    }

    if ( GetPlacement() != Params->Placement )
    {
        switch ( Params->Placement )
        {
            case wxsSizerExtraParams::LeftTop:      PlaceLT->SetValue(true); break;
            case wxsSizerExtraParams::CenterTop:    PlaceCT->SetValue(true); break;
            case wxsSizerExtraParams::RightTop:     PlaceRT->SetValue(true); break;
            case wxsSizerExtraParams::LeftCenter:   PlaceLC->SetValue(true); break;
            case wxsSizerExtraParams::Center:       PlaceCC->SetValue(true); break;
            case wxsSizerExtraParams::RightCenter:  PlaceRC->SetValue(true); break;
            case wxsSizerExtraParams::LeftBottom:   PlaceLB->SetValue(true); break;
            case wxsSizerExtraParams::CenterBottom: PlaceCB->SetValue(true); break;
            case wxsSizerExtraParams::RightBottom:  PlaceRB->SetValue(true); break;
            default:                                PlaceCC->SetValue(true); break;
        }
    }

    if ( GetExpand() != Params->Expand )
    {
        PlaceExp->SetValue(Params->Expand);
    }

    if ( GetShaped() != Params->Shaped )
    {
        PlaceShp->SetValue(Params->Shaped);
    }

    if ( GetProportion() != Params->Proportion )
    {
        Proportion->SetValue(Params->Proportion);
    }
}

void wxsSizerParentQP::SaveData()
{
    if ( !Valid() ) return;
    if ( !Widget || !Params ) return;

    bool Updated = false;

    #define Update(a,b) if ( (Params->a) != (b) ) { (Params->a) = (b); Updated = true; }

    Update(BorderFlags,GetBorderFlags());
    Update(Border,GetBorderSize());
    Update(Placement,GetPlacement());
    Update(Expand,GetExpand());
    Update(Shaped,GetShaped());
    Update(Proportion,GetProportion());

    #undef Update

    if ( Updated )
    {
        Widget->UpdatePropertiesWindow();
        Widget->PropertiesChanged(false,false);
    }

    Timer.Start();
}

void wxsSizerParentQP::OnTimer(wxTimerEvent& event)
{
    ReadData();
}

inline int wxsSizerParentQP::GetBorderFlags()
{
    int BorderFlags = 0;
    if ( BrdLeft  ->GetValue() ) BorderFlags |= wxsSizerExtraParams::Left;
    if ( BrdRight ->GetValue() ) BorderFlags |= wxsSizerExtraParams::Right;
    if ( BrdTop   ->GetValue() ) BorderFlags |= wxsSizerExtraParams::Top;
    if ( BrdBottom->GetValue() ) BorderFlags |= wxsSizerExtraParams::Bottom;
    return BorderFlags;
}

inline int wxsSizerParentQP::GetBorderSize()
{
    return BrdSize->GetValue();
}

inline int wxsSizerParentQP::GetPlacement()
{
    int Placement = 0;
    if ( PlaceLT->GetValue() ) Placement = wxsSizerExtraParams::LeftTop;
    if ( PlaceCT->GetValue() ) Placement = wxsSizerExtraParams::CenterTop;
    if ( PlaceRT->GetValue() ) Placement = wxsSizerExtraParams::RightTop;
    if ( PlaceLC->GetValue() ) Placement = wxsSizerExtraParams::LeftCenter;
    if ( PlaceCC->GetValue() ) Placement = wxsSizerExtraParams::Center;
    if ( PlaceRC->GetValue() ) Placement = wxsSizerExtraParams::RightCenter;
    if ( PlaceLB->GetValue() ) Placement = wxsSizerExtraParams::LeftBottom;
    if ( PlaceCB->GetValue() ) Placement = wxsSizerExtraParams::CenterBottom;
    if ( PlaceRB->GetValue() ) Placement = wxsSizerExtraParams::RightBottom;
    return Placement;
}

inline bool wxsSizerParentQP::GetExpand()
{
    return PlaceExp->GetValue();
}

inline bool wxsSizerParentQP::GetShaped()
{
    return PlaceShp->GetValue();
}

inline int wxsSizerParentQP::GetProportion()
{
    return Proportion->GetValue();
}
