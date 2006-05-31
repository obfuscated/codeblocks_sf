#include "wxsheaders.h"
#include "wxsstandardqp.h"
#include "widget.h"

BEGIN_EVENT_TABLE(wxsStandardQP,wxsQPPPanel)
    //(*EventTable(wxsStandardQP)
    EVT_TEXT(ID_TEXTCTRL1,wxsStandardQP::OnTextChanged)
    EVT_CHECKBOX(ID_CHECKBOX1,wxsStandardQP::OnChange)
    EVT_TEXT(ID_TEXTCTRL2,wxsStandardQP::OnTextChanged)
    EVT_CHECKBOX(ID_CHECKBOX3,wxsStandardQP::OnChange)
    EVT_CHECKBOX(ID_CHECKBOX2,wxsStandardQP::OnChange)
    EVT_CHECKBOX(ID_CHECKBOX4,wxsStandardQP::OnChange)
    //*)
    EVT_TIMER(0,wxsStandardQP::OnReadTimer)
    EVT_TIMER(1,wxsStandardQP::OnWriteTimer)
END_EVENT_TABLE()

wxsStandardQP::wxsStandardQP(wxWindow* parent,wxsWidget* _Widget,wxWindowID id):
    wxsQPPPanel(_Widget),
    ReadTimer(this,0),
    WriteTimer(this,1),
    Widget(_Widget)
{
	//(*Initialize(wxsStandardQP)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    MainSizer = new wxFlexGridSizer(0,1,0,0);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Standard options"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxSUNKEN_BORDER);
    VariableSizer = new wxStaticBoxSizer(wxVERTICAL,this,_("Variable"));
    VarName = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
    if ( 0 ) VarName->SetMaxLength(0);
    IsMember = new wxCheckBox(this,ID_CHECKBOX1,_("Is class member"),wxDefaultPosition,wxDefaultSize,0);
    IsMember->SetValue(false);
    VariableSizer->Add(VarName,1,wxALIGN_CENTER|wxEXPAND,5);
    VariableSizer->Add(IsMember,1,wxALIGN_CENTER|wxEXPAND,5);
    IdentifierSizer = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Identifier"));
    Ident = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0);
    if ( 0 ) Ident->SetMaxLength(0);
    IdentifierSizer->Add(Ident,1,wxALIGN_CENTER|wxEXPAND,5);
    FlagsSizer = new wxStaticBoxSizer(wxVERTICAL,this,_("Flags"));
    Enabled = new wxCheckBox(this,ID_CHECKBOX3,_("Enabled"),wxDefaultPosition,wxDefaultSize,0);
    Enabled->SetValue(false);
    Focused = new wxCheckBox(this,ID_CHECKBOX2,_("Focused"),wxDefaultPosition,wxDefaultSize,0);
    Focused->SetValue(false);
    Hdden = new wxCheckBox(this,ID_CHECKBOX4,_("Hidden"),wxDefaultPosition,wxDefaultSize,0);
    Hdden->SetValue(false);
    FlagsSizer->Add(Enabled,1,wxALIGN_CENTER|wxEXPAND,5);
    FlagsSizer->Add(Focused,1,wxALIGN_CENTER|wxEXPAND,5);
    FlagsSizer->Add(Hdden,1,wxALIGN_CENTER|wxEXPAND,5);
    MainSizer->Add(StaticText1,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    MainSizer->Add(VariableSizer,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    MainSizer->Add(IdentifierSizer,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    MainSizer->Add(FlagsSizer,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
    this->SetSizer(MainSizer);
    MainSizer->Fit(this);
    MainSizer->SetSizeHints(this);
    //*)

    if ( ! (Widget->GetBPType() & bptVariable) )
    {
        MainSizer->Hide(VariableSizer);
    }

    if ( ! (Widget->GetBPType() & bptId) )
    {
        MainSizer->Hide(IdentifierSizer);
    }

    if ( ! (Widget->GetBPType() & (bptEnabled|bptHidden|bptFocused)) )
    {
        MainSizer->Hide(FlagsSizer);
    }
    else
    {
        Enabled->Enable(Widget->GetBPType()&bptEnabled);
        Focused->Enable(Widget->GetBPType()&bptFocused);
        Hdden->Enable(Widget->GetBPType()&bptHidden);
    }

    Layout();
    MainSizer->Fit(this);
    MainSizer->SetSizeHints(this);

    ReadData();
    ReadTimer.Start(500);
}

wxsStandardQP::~wxsStandardQP()
{
    SaveData();
}


void wxsStandardQP::OnTextChanged(wxCommandEvent& event)
{
    ReadTimer.Stop();
    WriteTimer.Start(1000,true);
}

void wxsStandardQP::OnChange(wxCommandEvent& event)
{
    SaveData();
}

void wxsStandardQP::OnReadTimer(wxTimerEvent& event)
{
    ReadData();
}

void wxsStandardQP::OnWriteTimer(wxTimerEvent& event)
{
    SaveData();
}

void wxsStandardQP::ReadData()
{
    if ( !Valid() ) return;
    wxsBaseProperties& Params = Widget->BaseProperties;

    if ( Widget->GetBPType() & bptVariable )
    {
        if ( Params.VarName != VarName->GetValue() )
        {
            VarName->SetValue(Params.VarName);
        }

        if ( Params.IsMember != IsMember->GetValue() )
        {
            IsMember->SetValue(Params.IsMember);
        }
    }

    if ( Widget->GetBPType() & bptId )
    {
        if ( Params.IdName != Ident->GetValue() )
        {
            Ident->SetValue(Params.IdName);
        }
    }

    if ( Widget->GetBPType() & bptEnabled )
    {
        if ( Params.Enabled != Enabled->GetValue() )
        {
            Enabled->SetValue(Params.Enabled);
        }
    }

    if ( Widget->GetBPType() & bptFocused )
    {
        if ( Params.Focused != Focused->GetValue() )
        {
            Focused->SetValue(Params.Focused);
        }
    }

    if ( Widget->GetBPType() & bptHidden )
    {
        if ( Params.Hidden != Hdden->GetValue() )
        {
            Hdden->SetValue(Params.Hidden);
        }
    }
}

void wxsStandardQP::SaveData()
{
    if ( !Valid() ) return;

    bool Updated = false;
    wxsBaseProperties& Params = Widget->BaseProperties;

    #define Update(f,a,b) if ( Widget->GetBPType() & bpt##f ) if ( (Params.a) != (b) ) { (Params.a) = (b); Updated = true; }
    Update(Variable,VarName,VarName->GetValue());
    Update(Variable,IsMember,IsMember->GetValue());
    Update(Id,IdName,Ident->GetValue());
    Update(Enabled,Enabled,Enabled->GetValue());
    Update(Focused,Focused,Focused->GetValue());
    Update(Hidden,Hidden,Hdden->GetValue());
    #undef Update

    if ( Updated )
    {
        Widget->UpdatePropertiesWindow();
        Widget->PropertiesChanged(false,false);
    }

    ReadTimer.Start();
}

