#include "../wxsheaders.h"
#include "wxslistbookparentqp.h"

BEGIN_EVENT_TABLE(wxsListbookParentQP,wxsQPPPanel)
    //(*EventTable(wxsListbookParentQP)
    EVT_TEXT(ID_TEXTCTRL1,wxsListbookParentQP::OnLabelText)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsListbookParentQP::OnSelectionChange)
    //*)
    EVT_TIMER(0,wxsListbookParentQP::OnWriteTimer)
    EVT_TIMER(1,wxsListbookParentQP::OnReadTimer)
END_EVENT_TABLE()

wxsListbookParentQP::wxsListbookParentQP(wxWindow* parent,wxsWidget* Modified,wxsListbookExtraParams* _Params,wxWindowID id):
    wxsQPPPanel(Modified),
    WriteTimer(this,0),
    ReadTimer(this,1),
    Widget(Modified),
    Params(_Params)
{
	//(*Initialize(wxsListbookParentQP)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Listbook options"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxSUNKEN_BORDER);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Label"));
    Label = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
    if ( 0 ) Label->SetMaxLength(0);
    StaticBoxSizer1->Add(Label,0,wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Selection"));
    Selection = new wxCheckBox(this,ID_CHECKBOX1,_("Selected"),wxDefaultPosition,wxDefaultSize,0);
    Selection->SetValue(false);
    StaticBoxSizer2->Add(Selection,1,wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticText1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    this->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    //*)

    ReadData();
    ReadTimer.Start(500);
}

wxsListbookParentQP::~wxsListbookParentQP()
{
    ReadTimer.Stop();
    WriteTimer.Stop();
    SaveData();
}


void wxsListbookParentQP::OnLabelText(wxCommandEvent& event)
{
    ReadTimer.Stop();
    WriteTimer.Start(1000,true);
}

void wxsListbookParentQP::OnSelectionChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsListbookParentQP::OnReadTimer(wxTimerEvent& event)
{
    ReadData();
}

void wxsListbookParentQP::OnWriteTimer(wxTimerEvent& event)
{
    SaveData();
}

void wxsListbookParentQP::ReadData()
{
    if ( !Valid() ) return;
    if ( !Widget || !Params ) return;

    if ( Params->Label != Label->GetValue() )
    {
        Label->SetValue(Params->Label);
    }

    if ( Params->Selected != Selection->GetValue() )
    {
        Selection->SetValue(Params->Selected);
    }
}

void wxsListbookParentQP::SaveData()
{
    if ( !Valid() ) return;
    if ( !Widget || !Params ) return;

    bool Updated = false;

    #define Update(a,b) if ( (Params->a) != (b) ) { (Params->a) = (b); Updated = true; }
    Update(Label,Label->GetValue());
    Update(Selected,Selection->GetValue());
    #undef Update

    if ( Updated )
    {
        Widget->UpdatePropertiesWindow();
        Widget->PropertiesChanged(false,false);
    }

    ReadTimer.Start();
}
