/***************************************************************
 * Name:      cbprofilerconfig.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord
 * Created:   07/20/05 15:36:50
 * Copyright: (c) Dark Lord
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#include "cbprofilerconfig.h"
#include <configmanager.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/font.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>

BEGIN_EVENT_TABLE(CBProfilerConfigDlg, wxDialog)
EVT_CHECKBOX(XRCID("chkAnnSource"), CBProfilerConfigDlg::CheckBoxEvent)
EVT_CHECKBOX(XRCID("chkMinCount"), CBProfilerConfigDlg::CheckBoxEvent)
END_EVENT_TABLE()

CBProfilerConfigDlg::CBProfilerConfigDlg(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgCBProfilerConfig"));
    //wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
    //XRCCTRL(*this, "txtSample", wxTextCtrl)->SetFont(font);

    LoadSettings();
}

CBProfilerConfigDlg::~CBProfilerConfigDlg()
{
    //dtor
}

void CBProfilerConfigDlg::CheckBoxEvent(wxCommandEvent& event)
{
    if(event.GetId() == XRCID("chkAnnSource"))
        XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->Enable(XRCCTRL(*this, "chkAnnSource", wxCheckBox)->GetValue());
    if(event.GetId() == XRCID("chkMinCount"))
        XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->Enable(XRCCTRL(*this, "chkMinCount", wxCheckBox)->GetValue());
}

void CBProfilerConfigDlg::LoadSettings()
{
    // Values to be used many times along
    bool annSource=ConfigManager::Get()->Read(_T("/cbprofiler/ann_source_chk"), 0L);
    bool minCount=ConfigManager::Get()->Read(_T("/cbprofiler/min_count_chk"), 0L);

    // Output Options
    XRCCTRL(*this, "chkAnnSource", wxCheckBox)->SetValue(annSource);
    XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/ann_source_txt"), _T("")));
    XRCCTRL(*this, "chkBrief", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/brief"), 0L));
    XRCCTRL(*this, "chkFileInfo", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/file_info"), 0L));

    // Analysis Options
    XRCCTRL(*this, "chkNoStatic", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/no_static"), 0L));
    XRCCTRL(*this, "chkMinCount", wxCheckBox)->SetValue(minCount);
    XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/min_count_spn"), 0L));

    // Miscellaneous Options
    XRCCTRL(*this, "chkSum", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/sum"), 0L));

    // Extra Options
    XRCCTRL(*this, "txtExtra", wxTextCtrl)->SetValue(ConfigManager::Get()->Read(_T("/cbprofiler/extra_txt"), _T("")));

    // Enable/Disable the TextCtrl(s) as well as SpinCtrl(s)
    XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->Enable(annSource);
    XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->Enable(minCount);
}

void CBProfilerConfigDlg::SaveSettings()
{
    // Output Options
    ConfigManager::Get()->Write(_T("/cbprofiler/ann_source_chk"), XRCCTRL(*this, "chkAnnSource", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/cbprofiler/ann_source_txt"), XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->GetValue());
    ConfigManager::Get()->Write(_T("/cbprofiler/brief"), XRCCTRL(*this, "chkBrief", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/cbprofiler/file_info"), XRCCTRL(*this, "chkFileInfo", wxCheckBox)->GetValue());

    // Analysis Options
    ConfigManager::Get()->Write(_T("/cbprofiler/no_static"), XRCCTRL(*this, "chkNoStatic", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/cbprofiler/min_count_chk"), XRCCTRL(*this, "chkMinCount", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/cbprofiler/min_count_spn"), XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->GetValue());

    // Miscellaneous Options
    ConfigManager::Get()->Write(_T("/cbprofiler/sum"), XRCCTRL(*this, "chkSum", wxCheckBox)->GetValue());

    // Extra Options
    ConfigManager::Get()->Write(_T("/cbprofiler/extra_txt"), XRCCTRL(*this, "txtExtra", wxTextCtrl)->GetValue());
}

void CBProfilerConfigDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        // user pressed OK; save settings
        SaveSettings();
    }

    wxDialog::EndModal(retCode);
}
