#include "wxsheaders.h"
#include "wxssettingsdlg.h"

#include <configmanager.h>
#include <wx/colordlg.h>
#include "wxsglobals.h"
#include "wxswindoweditor.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxDialog)
//(*EventTable(wxsSettingsDlg)
    EVT_COMBOBOX(XRCID("ID_COMBOBOX1"),wxsSettingsDlg::OnDragAssistTypeSelect)
    EVT_BUTTON(XRCID("ID_BUTTON1"),wxsSettingsDlg::OnDragTargetColClick)
    EVT_BUTTON(XRCID("ID_BUTTON2"),wxsSettingsDlg::OnDragParentColClick)
    EVT_BUTTON(XRCID("ID_BUTTON3"),wxsSettingsDlg::OnBtnOkClick)
    EVT_BUTTON(XRCID("ID_BUTTON4"),wxsSettingsDlg::OnBtnCancelClick)
//*)
END_EVENT_TABLE()

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxsSettingsDlg)
    wxXmlResource::Get()->LoadDialog(this,parent,_T("wxsSettingsDlg"));
    DragAssistType = XRCCTRL(*this,"ID_COMBOBOX1",wxComboBox);
    DragTargetCol = XRCCTRL(*this,"ID_BUTTON1",wxButton);
    DragParentCol = XRCCTRL(*this,"ID_BUTTON2",wxButton);
    Icons16 = XRCCTRL(*this,"ID_RADIOBUTTON1",wxRadioButton);
    Icons32 = XRCCTRL(*this,"ID_RADIOBUTTON2",wxRadioButton);
    StaticText6 = XRCCTRL(*this,"ID_STATICTEXT6",wxStaticText);
    TIcons16 = XRCCTRL(*this,"ID_RADIOBUTTON3",wxRadioButton);
    TIcons32 = XRCCTRL(*this,"ID_RADIOBUTTON4",wxRadioButton);
    PrevFetchDelay = XRCCTRL(*this,"ID_SPINCTRL1",wxSpinCtrl);
    BtnCancel = XRCCTRL(*this,"ID_BUTTON4",wxButton);
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
}

wxsSettingsDlg::~wxsSettingsDlg()
{
}

void wxsSettingsDlg::OnBtnOkClick(wxCommandEvent& event)
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
    wxsWindowEditor::ReloadImages();
    EndModal(wxID_OK);
}

void wxsSettingsDlg::OnBtnCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
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
