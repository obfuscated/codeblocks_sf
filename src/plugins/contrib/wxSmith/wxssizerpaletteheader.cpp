#include "wxsheaders.h"
#include "wxssizerpaletteheader.h"

#include "wxsdefsizer.h"

BEGIN_EVENT_TABLE(wxsSizerPaletteHeader,wxPanel)
//(*EventTable(wxsSizerPaletteHeader)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsSizerPaletteHeader::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX2,wxsSizerPaletteHeader::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX3,wxsSizerPaletteHeader::OnBrdChange)
    EVT_CHECKBOX(ID_CHECKBOX4,wxsSizerPaletteHeader::OnBrdChange)
    EVT_SPINCTRL(ID_SPINCTRL1,wxsSizerPaletteHeader::OnBrdSizeChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON4,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON5,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON6,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON7,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON8,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON9,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON10,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON11,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_RADIOBUTTON(ID_RADIOBUTTON12,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_CHECKBOX(ID_CHECKBOX6,wxsSizerPaletteHeader::OnPlaceChange)
    EVT_CHECKBOX(ID_CHECKBOX5,wxsSizerPaletteHeader::OnPlaceChange)
//*)
    EVT_TIMER(-1,wxsSizerPaletteHeader::OnTimer)
END_EVENT_TABLE()

wxsSizerPaletteHeader::wxsSizerPaletteHeader(wxWindow* parent,wxsWidget* Modified,wxsSizerExtraParams* _Params,wxWindowID id):
    Widget(Modified),
    Params(_Params),
    Timer(this)
{
	//(*Initialize(wxsSizerPaletteHeader)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    FlexGridSizer1 = new wxFlexGridSizer(0,0,0,0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Border"));
    FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
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
    FlexGridSizer2->Add(BrdSize,1,wxALL|wxALIGN_CENTER,5);
    StaticBoxSizer1->Add(FlexGridSizer2,1,wxALL|wxALIGN_CENTER,0);
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
    StaticBoxSizer2->Add(FlexGridSizer3,1,wxALL|wxALIGN_CENTER,0);
    FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
    FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
    this->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    //*)

    ReadData();
    Timer.Start(250);
}

wxsSizerPaletteHeader::~wxsSizerPaletteHeader()
{
}


void wxsSizerPaletteHeader::OnBrdChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerPaletteHeader::OnBrdSizeChange(wxSpinEvent& event)
{
    SaveData();
}

void wxsSizerPaletteHeader::OnPlaceChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsSizerPaletteHeader::ReadData()
{
    if ( !Widget || !Params ) return;

    BrdLeft  ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Left)   != 0 );
    BrdRight ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Right)  != 0 );
    BrdTop   ->SetValue((Params->BorderFlags & wxsSizerExtraParams::Top)    != 0 );
    BrdBottom->SetValue((Params->BorderFlags & wxsSizerExtraParams::Bottom) != 0 );

    BrdSize->SetValue(Params->Border);

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

    PlaceExp->SetValue(Params->Expand);
    PlaceShp->SetValue(Params->Shaped);
}

void wxsSizerPaletteHeader::SaveData()
{
    if ( !Widget || !Params ) return;

    bool Updated = false;

    int OldFlags = Params->BorderFlags;
    Params->BorderFlags = 0;
    if ( BrdLeft  ->GetValue() ) Params->BorderFlags |= wxsSizerExtraParams::Left;
    if ( BrdRight ->GetValue() ) Params->BorderFlags |= wxsSizerExtraParams::Right;
    if ( BrdTop   ->GetValue() ) Params->BorderFlags |= wxsSizerExtraParams::Top;
    if ( BrdBottom->GetValue() ) Params->BorderFlags |= wxsSizerExtraParams::Bottom;
    if ( OldFlags != Params->BorderFlags ) Updated = true;

    if ( Params->Border != BrdSize->GetValue() )
    {
        Updated = true;
        Params->Border = BrdSize->GetValue();
    }

    int OldPlacement = Params->Placement;
    if ( PlaceLT->GetValue() ) Params->Placement = wxsSizerExtraParams::LeftTop;
    if ( PlaceCT->GetValue() ) Params->Placement = wxsSizerExtraParams::CenterTop;
    if ( PlaceRT->GetValue() ) Params->Placement = wxsSizerExtraParams::RightTop;
    if ( PlaceLC->GetValue() ) Params->Placement = wxsSizerExtraParams::LeftCenter;
    if ( PlaceCC->GetValue() ) Params->Placement = wxsSizerExtraParams::Center;
    if ( PlaceRC->GetValue() ) Params->Placement = wxsSizerExtraParams::RightCenter;
    if ( PlaceLB->GetValue() ) Params->Placement = wxsSizerExtraParams::LeftBottom;
    if ( PlaceCB->GetValue() ) Params->Placement = wxsSizerExtraParams::CenterBottom;
    if ( PlaceRB->GetValue() ) Params->Placement = wxsSizerExtraParams::RightBottom;
    if ( OldPlacement != Params->Placement ) Updated = true;

    if ( Params->Expand != PlaceExp->GetValue() )
    {
        Updated = true;
        Params->Expand = PlaceExp->GetValue();
    }
    if ( Params->Shaped != PlaceShp->GetValue() )
    {
        Updated = true;
        Params->Shaped = PlaceShp->GetValue();
    }

    if ( Updated )
    {
        Widget->UpdateProperties();
        Widget->PropertiesUpdated(false,false);
    }

    Timer.Start();
}

void wxsSizerPaletteHeader::OnTimer(wxTimerEvent& event)
{
    ReadData();
}
