#include "wxsheaders.h"
#include "wxssettingsdlg.h"

#include <configmanager.h>
#include <wx/colordlg.h>
#include "wxsglobals.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxDialog)
//(*EventTable(wxsSettingsDlg)
    EVT_COMBOBOX(XRCID("ID_COMBOBOX1"),wxsSettingsDlg::OnDragAssistTypeSelect)
    EVT_BUTTON(XRCID("ID_BUTTON1"),wxsSettingsDlg::OnDragTargetColClick)
    EVT_BUTTON(XRCID("ID_BUTTON2"),wxsSettingsDlg::OnDragParentColClick)
    EVT_BUTTON(XRCID("ID_BUTTON3"),wxsSettingsDlg::OnBtnOkClick)
    EVT_BUTTON(XRCID("ID_BUTTON4"),wxsSettingsDlg::OnBtnCancelClick)
//*)
END_EVENT_TABLE()

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id):
    wxDialog(parent,id,_T(""),wxDefaultPosition,wxDefaultSize)
{
    //(*Initialize(wxsSettingsDlg)
    wxXmlResource::Get()->LoadDialog(this,parent,_T("wxsSettingsDlg"));
    DragAssistType = XRCCTRL(*this,"ID_COMBOBOX1",wxComboBox);
    DragTargetCol = XRCCTRL(*this,"ID_BUTTON1",wxButton);
    DragParentCol = XRCCTRL(*this,"ID_BUTTON2",wxButton);
    PrevFetchDelay = XRCCTRL(*this,"ID_SPINCTRL1",wxSpinCtrl);
    BtnCancel = XRCCTRL(*this,"ID_BUTTON4",wxButton);
    //*)

    long ColTarget = wxsDWTargetCol;
    long ColParent = wxsDWParentCol;
    DragAssistType->SetSelection(wxsDWAssistType);
    DragTargetCol->SetBackgroundColour(wxColour((ColTarget>>16)&0xFF,(ColTarget>>8)&0xFF,ColTarget&0xFF));
    DragParentCol->SetBackgroundColour(wxColour((ColParent>>16)&0xFF,(ColParent>>8)&0xFF,ColParent&0xFF));
    PrevFetchDelay->SetValue(wxsDWFetchDelay);
}

wxsSettingsDlg::~wxsSettingsDlg()
{
}



void wxsSettingsDlg::OnBtnOkClick(wxCommandEvent& event)
{
    wxColour ColTarget = DragTargetCol->GetBackgroundColour();
    wxColour ColParent = DragParentCol->GetBackgroundColour();

    ConfigManager::Get()->Write(_T("/wxsmith/dragtargetcol"),(((long)ColTarget.Red())<<16) + (((long)ColTarget.Green())<<8) + (long)ColTarget.Blue());
    ConfigManager::Get()->Write(_T("/wxsmith/dragparentcol"),(((long)ColParent.Red())<<16) + (((long)ColParent.Green())<<8) + (long)ColParent.Blue());
    ConfigManager::Get()->Write(_T("/wxsmith/dragassisttype"),(long)DragAssistType->GetSelection());
    ConfigManager::Get()->Write(_T("/wxsmith/backfetchdelay"),(long)PrevFetchDelay->GetValue());
    Close();
}

void wxsSettingsDlg::OnBtnCancelClick(wxCommandEvent& event)
{
    Close();
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
