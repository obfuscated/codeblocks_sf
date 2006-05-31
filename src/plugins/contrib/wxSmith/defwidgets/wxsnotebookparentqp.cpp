#include "../wxsheaders.h"
#include "wxsnotebookparentqp.h"

BEGIN_EVENT_TABLE(wxsNotebookParentQP,wxsQPPPanel)
    //(*EventTable(wxsNotebookParentQP)
    EVT_TEXT(ID_TEXTCTRL1,wxsNotebookParentQP::OnLabelText)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsNotebookParentQP::OnSelectionChange)
    //*)
    EVT_TIMER(0,wxsNotebookParentQP::OnWriteTimer)
    EVT_TIMER(1,wxsNotebookParentQP::OnReadTimer)
END_EVENT_TABLE()

wxsNotebookParentQP::wxsNotebookParentQP(wxWindow* parent,wxsWidget* Modified,wxsNotebookExtraParams* _Params,wxWindowID id):
    wxsQPPPanel(Modified),
    WriteTimer(this,0),
    ReadTimer(this,1),
    Widget(Modified),
    Params(_Params)
{
	//(*Initialize(wxsNotebookParentQP)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Notebook options"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxSUNKEN_BORDER);
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

wxsNotebookParentQP::~wxsNotebookParentQP()
{
    ReadTimer.Stop();
    WriteTimer.Stop();
    SaveData();
}


void wxsNotebookParentQP::OnLabelText(wxCommandEvent& event)
{
    ReadTimer.Stop();
    WriteTimer.Start(1000,true);
}

void wxsNotebookParentQP::OnSelectionChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsNotebookParentQP::OnReadTimer(wxTimerEvent& event)
{
    ReadData();
}

void wxsNotebookParentQP::OnWriteTimer(wxTimerEvent& event)
{
    SaveData();
}

void wxsNotebookParentQP::ReadData()
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

void wxsNotebookParentQP::SaveData()
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
