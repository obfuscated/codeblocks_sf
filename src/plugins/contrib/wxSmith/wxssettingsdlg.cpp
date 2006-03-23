#include "wxsheaders.h"
#include "wxssettingsdlg.h"

#include <configmanager.h>
#include <wx/colordlg.h>
#include "wxsglobals.h"
#include "wxswindoweditor.h"
#include "wxsdefsizer.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxPanel)
//(*EventTable(wxsSettingsDlg)
EVT_COMBOBOX(XRCID("ID_COMBOBOX1"),wxsSettingsDlg::OnDragAssistTypeSelect)
EVT_BUTTON(XRCID("ID_BUTTON1"),wxsSettingsDlg::OnDragTargetColClick)
EVT_BUTTON(XRCID("ID_BUTTON2"),wxsSettingsDlg::OnDragParentColClick)
//*)
END_EVENT_TABLE()

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxsSettingsDlg)
    wxXmlResource::Get()->LoadPanel(this,parent,_T("wxsSettingsDlg"));
    DragAssistType = XRCCTRL(*this,"ID_COMBOBOX1",wxComboBox);
    DragTargetCol = XRCCTRL(*this,"ID_BUTTON1",wxButton);
    DragParentCol = XRCCTRL(*this,"ID_BUTTON2",wxButton);
    Icons16 = XRCCTRL(*this,"ID_RADIOBUTTON1",wxRadioButton);
    Icons32 = XRCCTRL(*this,"ID_RADIOBUTTON2",wxRadioButton);
    StaticText6 = XRCCTRL(*this,"ID_STATICTEXT6",wxStaticText);
    TIcons16 = XRCCTRL(*this,"ID_RADIOBUTTON3",wxRadioButton);
    TIcons32 = XRCCTRL(*this,"ID_RADIOBUTTON4",wxRadioButton);
    Panel3 = XRCCTRL(*this,"ID_PANEL3",wxPanel);
    StaticText7 = XRCCTRL(*this,"ID_STATICTEXT7",wxStaticText);
    spinProportion = XRCCTRL(*this,"ID_SPINCTRL2",wxSpinCtrl);
    StaticText8 = XRCCTRL(*this,"ID_STATICTEXT8",wxStaticText);
    chkTop = XRCCTRL(*this,"ID_CHECKBOX1",wxCheckBox);
    chkBottom = XRCCTRL(*this,"ID_CHECKBOX2",wxCheckBox);
    chkLeft = XRCCTRL(*this,"ID_CHECKBOX3",wxCheckBox);
    chkRight = XRCCTRL(*this,"ID_CHECKBOX4",wxCheckBox);
    StaticText9 = XRCCTRL(*this,"ID_STATICTEXT9",wxStaticText);
    chkExpand = XRCCTRL(*this,"ID_CHECKBOX5",wxCheckBox);
    StaticText10 = XRCCTRL(*this,"ID_STATICTEXT10",wxStaticText);
    chkShaped = XRCCTRL(*this,"ID_CHECKBOX6",wxCheckBox);
    StaticText12 = XRCCTRL(*this,"ID_STATICTEXT12",wxStaticText);
    choicePlacement = XRCCTRL(*this,"ID_CHOICE1",wxChoice);
    StaticText13 = XRCCTRL(*this,"ID_STATICTEXT13",wxStaticText);
    spinBorder = XRCCTRL(*this,"ID_SPINCTRL3",wxSpinCtrl);
    chkAutoSelect = XRCCTRL(*this,"ID_CHECKBOX7",wxCheckBox);
    PrevFetchDelay = XRCCTRL(*this,"ID_SPINCTRL1",wxSpinCtrl);
    //*)

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

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    spinProportion->SetValue(cfg->ReadInt(_T("/defsizer/proportion"), 0));
    int borderFlags = cfg->ReadInt(_T("/defsizer/borderflags"), wxsSizerExtraParams::All);
    chkLeft->SetValue(borderFlags & wxsSizerExtraParams::Left);
    chkRight->SetValue(borderFlags & wxsSizerExtraParams::Right);
    chkTop->SetValue(borderFlags & wxsSizerExtraParams::Top);
    chkBottom->SetValue(borderFlags & wxsSizerExtraParams::Bottom);
    chkExpand->SetValue(cfg->ReadBool(_T("/defsizer/expand"), false));
    chkShaped->SetValue(cfg->ReadBool(_T("/defsizer/shaped"), false));
    choicePlacement->SetSelection(cfg->ReadInt(_T("/defsizer/placement"), wxsSizerExtraParams::LeftTop) - 1);
    spinBorder->SetValue(cfg->ReadInt(_T("/defsizer/border"), 0));
    chkAutoSelect->SetValue(cfg->ReadBool(_T("/autoselectwidgets"), true));
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

    int borderFlags =   (chkLeft->IsChecked()   ? wxsSizerExtraParams::Left   : 0) |
                        (chkRight->IsChecked()  ? wxsSizerExtraParams::Right  : 0) |
                        (chkTop->IsChecked()    ? wxsSizerExtraParams::Top    : 0) |
                        (chkBottom->IsChecked() ? wxsSizerExtraParams::Bottom : 0);

    cfg->Write(_T("/defsizer/proportion"), (int)spinProportion->GetValue());
    cfg->Write(_T("/defsizer/borderflags"), (int)borderFlags);
    cfg->Write(_T("/defsizer/expand"), (bool)chkExpand->IsChecked());
    cfg->Write(_T("/defsizer/shaped"), (bool)chkShaped->IsChecked());
    cfg->Write(_T("/defsizer/placement"), (int)choicePlacement->GetSelection() + 1);
    cfg->Write(_T("/defsizer/border"), (int)spinBorder->GetValue());
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
