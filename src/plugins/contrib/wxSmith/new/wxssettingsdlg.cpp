#include "wxssettingsdlg.h"

#include <configmanager.h>
#include <wx/colordlg.h>
#include "wxsglobals.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxPanel)
    //(*EventTable(wxsSettingsDlg)
    EVT_COMBOBOX(ID_COMBOBOX1,wxsSettingsDlg::OnDragAssistTypeSelect)
    EVT_BUTTON(ID_BUTTON1,wxsSettingsDlg::OnDragTargetColClick)
    EVT_BUTTON(ID_BUTTON2,wxsSettingsDlg::OnDragParentColClick)
    //*)
END_EVENT_TABLE()

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxsSettingsDlg)
    wxStaticText* StaticText2;
    wxStaticText* StaticText3;
    wxStaticText* StaticText4;
    wxStaticText* StaticText5;
    wxStaticText* StaticText1;
    
    Create(parent,id,wxDefaultPosition,wxDefaultSize,0);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Editor"));
    FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
    FlexGridSizer2->AddGrowableCol(1);
    StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Drag Assistance Type"),wxDefaultPosition,wxDefaultSize,0);
    DragAssistType = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY);
    DragAssistType->Append(_("None"));
    DragAssistType->Append(_("Simple"));
    DragAssistType->Append(_("Colour Mix"));
    DragAssistType->SetSelection(2);
    StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Drag target colour"),wxDefaultPosition,wxDefaultSize,0);
    DragTargetCol = new wxButton(this,ID_BUTTON1,_("..."),wxDefaultPosition,wxDefaultSize,0);
    if (false) DragTargetCol->SetDefault();
    StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Drag Parent Colour"),wxDefaultPosition,wxDefaultSize,0);
    DragParentCol = new wxButton(this,ID_BUTTON2,_("..."),wxDefaultPosition,wxDefaultSize,0);
    if (false) DragParentCol->SetDefault();
    StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Palette icon size"),wxDefaultPosition,wxDefaultSize,0);
    FlexGridSizer3 = new wxFlexGridSizer(0,0,0,0);
    Icons16 = new wxRadioButton(this,ID_RADIOBUTTON1,_("16x16"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
    Icons16->SetValue(false);
    Icons32 = new wxRadioButton(this,ID_RADIOBUTTON2,_("32x32"),wxDefaultPosition,wxDefaultSize,0);
    Icons32->SetValue(false);
    FlexGridSizer3->Add(Icons16,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer3->Add(Icons32,1,wxALL|wxALIGN_CENTER,5);
    StaticText6 = new wxStaticText(this,ID_STATICTEXT6,_("Tool icon size"),wxDefaultPosition,wxDefaultSize,0);
    FlexGridSizer4 = new wxFlexGridSizer(0,0,0,0);
    TIcons16 = new wxRadioButton(this,ID_RADIOBUTTON3,_("16x16"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
    TIcons16->SetValue(false);
    TIcons32 = new wxRadioButton(this,ID_RADIOBUTTON4,_("32x32"),wxDefaultPosition,wxDefaultSize,0);
    TIcons32->SetValue(false);
    FlexGridSizer4->Add(TIcons16,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer4->Add(TIcons32,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(StaticText2,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(DragAssistType,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer2->Add(StaticText3,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(DragTargetCol,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer2->Add(StaticText4,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(DragParentCol,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    FlexGridSizer2->Add(StaticText5,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(FlexGridSizer3,1,wxALIGN_CENTER,5);
    FlexGridSizer2->Add(StaticText6,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer2->Add(FlexGridSizer4,1,wxALIGN_CENTER,5);
    StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER,5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Advnaced"));
    FlexGridSizer5 = new wxFlexGridSizer(0,0,0,0);
    FlexGridSizer5->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Preview fetch delay"),wxDefaultPosition,wxDefaultSize,0);
    PrevFetchDelay = new wxSpinCtrl(this,ID_SPINCTRL1,_("50"),wxDefaultPosition,wxDefaultSize,0,0,500);
    FlexGridSizer5->Add(StaticText1,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(PrevFetchDelay,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
    StaticBoxSizer2->Add(FlexGridSizer5,1,wxALIGN_CENTER,5);
    FlexGridSizer1->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
    this->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    //*)
    
    SetSize(500,500);

    long ColTarget = wxsDWTargetCol;
    long ColParent = wxsDWParentCol;
    DragAssistType->SetSelection(wxsDWAssistType);
    DragTargetCol->SetBackgroundColour(wxColour((ColTarget>>16)&0xFF,(ColTarget>>8)&0xFF,ColTarget&0xFF));
    DragParentCol->SetBackgroundColour(wxColour((ColParent>>16)&0xFF,(ColParent>>8)&0xFF,ColParent&0xFF));
    PrevFetchDelay->SetValue(wxsDWFetchDelay);

    if ( wxsDWPalIconSize == 16 ) Icons16->SetValue(true);
    else                          Icons32->SetValue(true);
    if ( wxsDWToolIconSize == 16 ) TIcons16->SetValue(true);
    else                           TIcons32->SetValue(true);
}

wxsSettingsDlg::~wxsSettingsDlg()
{
}

void wxsSettingsDlg::OnApply()
{
    wxColour ColTarget = DragTargetCol->GetBackgroundColour();
    wxColour ColParent = DragParentCol->GetBackgroundColour();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    cfg->Write(_T("/dragtargetcol"),(int)((((int)ColTarget.Red())<<16) + (((long)ColTarget.Green())<<8) + (long)ColTarget.Blue()));
    cfg->Write(_T("/dragparentcol"),(int)((((int)ColParent.Red())<<16) + (((long)ColParent.Green())<<8) + (long)ColParent.Blue()));
    cfg->Write(_T("/dragassisttype"),(int)DragAssistType->GetSelection());
    cfg->Write(_T("/backfetchdelay"),(int)PrevFetchDelay->GetValue());
    cfg->Write(_T("/paletteiconsize"),(int)(Icons16->GetValue()?16:32));
    cfg->Write(_T("/tooliconsize"),(int)(TIcons16->GetValue()?16:32));
//    wxsWindowEditor::ReloadImages();
}

void wxsSettingsDlg::OnDragTargetColClick(wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,DragTargetCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        DragTargetCol->SetBackgroundColour(Col);
    }
}

void wxsSettingsDlg::OnDragParentColClick(wxCommandEvent& event)
{
    wxColour Col = ::wxGetColourFromUser(this,DragParentCol->GetBackgroundColour());
    if ( Col.Ok() )
    {
        DragParentCol->SetBackgroundColour(Col);
    }
}

void wxsSettingsDlg::OnDragAssistTypeSelect(wxCommandEvent& event)
{
    bool Enable = DragAssistType->GetSelection() != wxsDTNone;
    DragTargetCol->Enable(Enable);
    DragParentCol->Enable(Enable);
}
