#include "../wxsheaders.h"
#include "wxschoicebookparentqp.h"

BEGIN_EVENT_TABLE(wxsChoicebookParentQP,wxsQPPPanel)
    //(*EventTable(wxsChoicebookParentQP)
    EVT_TEXT(ID_TEXTCTRL1,wxsChoicebookParentQP::OnLabelText)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsChoicebookParentQP::OnSelectionChange)
    //*)
    EVT_TIMER(0,wxsChoicebookParentQP::OnWriteTimer)
    EVT_TIMER(1,wxsChoicebookParentQP::OnReadTimer)
END_EVENT_TABLE()

wxsChoicebookParentQP::wxsChoicebookParentQP(wxWindow* parent,wxsWidget* Modified,wxsChoicebookExtraParams* _Params,wxWindowID id):
    wxsQPPPanel(Modified),
    WriteTimer(this,0),
    ReadTimer(this,1),
    Widget(Modified),
    Params(_Params)
{
	//(*Initialize(wxsChoicebookParentQP)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Choicebook options"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxSUNKEN_BORDER);
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

wxsChoicebookParentQP::~wxsChoicebookParentQP()
{
    ReadTimer.Stop();
    WriteTimer.Stop();
    SaveData();
}


void wxsChoicebookParentQP::OnLabelText(wxCommandEvent& event)
{
    ReadTimer.Stop();
    WriteTimer.Start(1000,true);
}

void wxsChoicebookParentQP::OnSelectionChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsChoicebookParentQP::OnReadTimer(wxTimerEvent& event)
{
    ReadData();
}

void wxsChoicebookParentQP::OnWriteTimer(wxTimerEvent& event)
{
    SaveData();
}

void wxsChoicebookParentQP::ReadData()
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

void wxsChoicebookParentQP::SaveData()
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
