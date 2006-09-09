/*
#include "wxssettingsdlg.h"

#include <configmanager.h>
#include <wx/colordlg.h>
//#include "wxsglobals.h"
//#include "wxssizer.h"
//#include "editors/wxswindoweditor.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxPanel)
    //(*EventTable(wxsSettingsDlg)
    EVT_COMBOBOX(ID_COMBOBOX1,wxsSettingsDlg::OnDragAssistTypeSelect)
    EVT_BUTTON(ID_BUTTON1,wxsSettingsDlg::OnDragTargetColClick)
    EVT_BUTTON(ID_BUTTON2,wxsSettingsDlg::OnDragParentColClick)
    //*)
END_EVENT_TABLE()

static long placementConversionArray[] =
{
    wxsSizerFlagsProperty::AlignLeft             | wxsSizerFlagsProperty::AlignTop,
    wxsSizerFlagsProperty::AlignCenterHorizontal | wxsSizerFlagsProperty::AlignTop,
    wxsSizerFlagsProperty::AlignRight            | wxsSizerFlagsProperty::AlignTop,
    wxsSizerFlagsProperty::AlignLeft             | wxsSizerFlagsProperty::AlignCenterVertical,
    wxsSizerFlagsProperty::AlignCenterHorizontal | wxsSizerFlagsProperty::AlignCenterVertical,
    wxsSizerFlagsProperty::AlignRight            | wxsSizerFlagsProperty::AlignCenterVertical,
    wxsSizerFlagsProperty::AlignLeft             | wxsSizerFlagsProperty::AlignBottom,
    wxsSizerFlagsProperty::AlignCenterHorizontal | wxsSizerFlagsProperty::AlignBottom,
    wxsSizerFlagsProperty::AlignRight            | wxsSizerFlagsProperty::AlignBottom,
    0
};

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxsSettingsDlg)
    wxStaticText* StaticText2;
    wxStaticText* StaticText3;
    wxStaticText* StaticText4;
    wxStaticText* StaticText5;

    Create(parent,id,wxDefaultPosition,wxDefaultSize,0);
    FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer6 = new wxFlexGridSizer(0,1,0,0);
    FlexGridSizer6->AddGrowableCol(0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Editor"));
    FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
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
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Default sizer settings"));
    FlexGridSizer5 = new wxFlexGridSizer(0,2,0,0);
    StaticText7 = new wxStaticText(this,ID_STATICTEXT7,_("Proportion:"),wxDefaultPosition,wxDefaultSize,0);
    spinProportion = new wxSpinCtrl(this,ID_SPINCTRL2,_("0"),wxDefaultPosition,wxDefaultSize,0,0,100);
    StaticText8 = new wxStaticText(this,ID_STATICTEXT8,_("Border flags:"),wxDefaultPosition,wxDefaultSize,0);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    chkTop = new wxCheckBox(this,ID_CHECKBOX1,_("Top"),wxDefaultPosition,wxDefaultSize,0);
    chkTop->SetValue(true);
    chkBottom = new wxCheckBox(this,ID_CHECKBOX2,_("Bottom"),wxDefaultPosition,wxDefaultSize,0);
    chkBottom->SetValue(true);
    chkLeft = new wxCheckBox(this,ID_CHECKBOX3,_("Left"),wxDefaultPosition,wxDefaultSize,0);
    chkLeft->SetValue(true);
    chkRight = new wxCheckBox(this,ID_CHECKBOX4,_("Right"),wxDefaultPosition,wxDefaultSize,0);
    chkRight->SetValue(true);
    BoxSizer2->Add(chkTop,0,wxALIGN_CENTER,5);
    BoxSizer2->Add(chkBottom,0,wxLEFT|wxALIGN_CENTER,5);
    BoxSizer2->Add(chkLeft,0,wxLEFT|wxALIGN_CENTER,5);
    BoxSizer2->Add(chkRight,0,wxLEFT|wxALIGN_CENTER,5);
    StaticText9 = new wxStaticText(this,ID_STATICTEXT9,_("Expand:"),wxDefaultPosition,wxDefaultSize,0);
    chkExpand = new wxCheckBox(this,ID_CHECKBOX5,_T(""),wxDefaultPosition,wxDefaultSize,0);
    chkExpand->SetValue(false);
    StaticText10 = new wxStaticText(this,ID_STATICTEXT10,_("Shaped:"),wxDefaultPosition,wxDefaultSize,0);
    chkShaped = new wxCheckBox(this,ID_CHECKBOX6,_T(""),wxDefaultPosition,wxDefaultSize,0);
    chkShaped->SetValue(false);
    StaticText12 = new wxStaticText(this,ID_STATICTEXT12,_("Placement:"),wxDefaultPosition,wxDefaultSize,0);
    choicePlacement = new wxChoice(this,ID_CHOICE1,wxDefaultPosition,wxDefaultSize,0,NULL,0);
    choicePlacement->Append(_("Left-Top"));
    choicePlacement->Append(_("Top"));
    choicePlacement->Append(_("Right-Top"));
    choicePlacement->Append(_("Left"));
    choicePlacement->Append(_("Center"));
    choicePlacement->Append(_("Right"));
    choicePlacement->Append(_("Left-Bottom"));
    choicePlacement->Append(_("Bottom"));
    choicePlacement->Append(_("Right-Bottom"));
    choicePlacement->SetSelection(0);
    StaticText13 = new wxStaticText(this,ID_STATICTEXT13,_("Border:"),wxDefaultPosition,wxDefaultSize,0);
    spinBorder = new wxSpinCtrl(this,ID_SPINCTRL3,_("0"),wxDefaultPosition,wxDefaultSize,0,0,100);
    chkBorderDU = new wxCheckBox(this,ID_CHECKBOX8,_("Use dialog units"),wxDefaultPosition,wxDefaultSize,0);
    chkBorderDU->SetValue(false);
    FlexGridSizer5->Add(StaticText7,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(spinProportion,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
    FlexGridSizer5->Add(StaticText8,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(BoxSizer2,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
    FlexGridSizer5->Add(StaticText9,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(chkExpand,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
    FlexGridSizer5->Add(StaticText10,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(chkShaped,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
    FlexGridSizer5->Add(StaticText12,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(choicePlacement,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
    FlexGridSizer5->Add(StaticText13,1,wxALL|wxALIGN_CENTER,5);
    FlexGridSizer5->Add(spinBorder,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
    FlexGridSizer5->Add(16,8,1);
    FlexGridSizer5->Add(chkBorderDU,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
    StaticBoxSizer2->Add(FlexGridSizer5,1,wxALIGN_CENTER,5);
    FlexGridSizer6->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
    FlexGridSizer6->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    chkAutoSelect = new wxCheckBox(this,ID_CHECKBOX7,_("Automatically select new widgets, when created"),wxDefaultPosition,wxDefaultSize,0);
    chkAutoSelect->SetValue(false);
    BoxSizer1->Add(chkAutoSelect,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
    FlexGridSizer1->Add(FlexGridSizer6,1,wxALIGN_CENTER|wxEXPAND,4);
    FlexGridSizer1->Add(BoxSizer1,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxEXPAND,4);
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

    if ( wxsDWPalIconSize == 16 ) Icons16->SetValue(true);
    else                          Icons32->SetValue(true);
    if ( wxsDWToolIconSize == 16 ) TIcons16->SetValue(true);
    else                           TIcons32->SetValue(true);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    spinProportion->SetValue(cfg->ReadInt(_T("/defsizer/proportion"), 0));

    int flags = cfg->ReadInt(_T("/defsizer/flags"),
        wxsSizerFlagsProperty::AlignCenterHorizontal |
        wxsSizerFlagsProperty::AlignCenterVertical |
        wxsSizerFlagsProperty::BorderTop |
        wxsSizerFlagsProperty::BorderBottom |
        wxsSizerFlagsProperty::BorderLeft |
        wxsSizerFlagsProperty::BorderRight);

    chkLeft->SetValue(flags & wxsSizerFlagsProperty::BorderLeft);
    chkRight->SetValue(flags & wxsSizerFlagsProperty::BorderRight);
    chkTop->SetValue(flags & wxsSizerFlagsProperty::BorderTop);
    chkBottom->SetValue(flags & wxsSizerFlagsProperty::BorderBottom);
    chkExpand->SetValue(flags & wxsSizerFlagsProperty::Expand);
    chkShaped->SetValue(flags & wxsSizerFlagsProperty::Shaped);

    choicePlacement->SetSelection(0);
    for ( int i=0; placementConversionArray[i]; i++ )
    {
        if ( (flags & placementConversionArray[i]) == placementConversionArray[i] )
        {
            choicePlacement->SetSelection(i);
            break;
        }
    }

    spinBorder->SetValue(cfg->ReadInt(_T("/defsizer/border"),0));
    chkBorderDU->SetValue(cfg->ReadBool(_T("/defsizer/borderdu"),false));
    chkAutoSelect->SetValue(cfg->ReadBool(_T("/autoselectwidgets"),true));
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
    cfg->Write(_T("/paletteiconsize"),(int)(Icons16->GetValue()?16:32));
    cfg->Write(_T("/tooliconsize"),(int)(TIcons16->GetValue()?16:32));

    int flags =   (chkLeft  ->IsChecked() ? wxsSizerFlagsProperty::BorderLeft   : 0) |
                  (chkRight ->IsChecked() ? wxsSizerFlagsProperty::BorderRight  : 0) |
                  (chkTop   ->IsChecked() ? wxsSizerFlagsProperty::BorderTop    : 0) |
                  (chkBottom->IsChecked() ? wxsSizerFlagsProperty::BorderBottom : 0) |
                  (chkExpand->IsChecked() ? wxsSizerFlagsProperty::Expand       : 0) |
                  (chkShaped->IsChecked() ? wxsSizerFlagsProperty::Shaped       : 0) |
                  (placementConversionArray[choicePlacement->GetSelection()]);

    cfg->Write(_T("/defsizer/proportion"), (int)spinProportion->GetValue());
    cfg->Write(_T("/defsizer/flags"), (int)flags);
    cfg->Write(_T("/defsizer/border"), (int)spinBorder->GetValue());
    cfg->Write(_T("/defsizer/borderdu"), (bool)chkBorderDU->GetValue());
    cfg->Write(_T("/autoselectwidgets"), (bool)chkAutoSelect->GetValue());

    wxsWindowEditor::ReloadImages();
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
*/
