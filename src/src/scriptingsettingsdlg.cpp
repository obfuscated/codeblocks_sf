#include <sdk.h>
#include "scriptingsettingsdlg.h"

#include "manager.h"
#include "configmanager.h"
#include "filefilters.h"

#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/xrc/xmlres.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/intl.h>

BEGIN_EVENT_TABLE(ScriptingSettingsDlg, wxDialog)
    EVT_LIST_ITEM_SELECTED(XRCID("chkStartupScripts"), ScriptingSettingsDlg::OnListSelection)
    EVT_LIST_ITEM_DESELECTED(XRCID("chkStartupScripts"), ScriptingSettingsDlg::OnListDeselection)
    EVT_TEXT(XRCID("txtScript"), ScriptingSettingsDlg::OnScriptChanged)
    EVT_TEXT(XRCID("txtScriptMenu"), ScriptingSettingsDlg::OnScriptMenuChanged)
    EVT_BUTTON(XRCID("btnAdd"), ScriptingSettingsDlg::OnAddScript)
    EVT_BUTTON(XRCID("btnDelete"), ScriptingSettingsDlg::OnRemoveScript)
    EVT_BUTTON(XRCID("btnBrowseScript"), ScriptingSettingsDlg::OnBrowse)
    EVT_CHECKBOX(XRCID("chkEnableScript"), ScriptingSettingsDlg::OnEnable)
    EVT_CHECKBOX(XRCID("chkRegisterScript"), ScriptingSettingsDlg::OnRegister)

    EVT_LIST_ITEM_SELECTED(XRCID("lstTrustedScripts"), ScriptingSettingsDlg::OnTrustSelection)
    EVT_LIST_ITEM_DESELECTED(XRCID("lstTrustedScripts"), ScriptingSettingsDlg::OnTrustSelection)
    EVT_BUTTON(XRCID("btnValidateTrusts"), ScriptingSettingsDlg::OnValidateTrusts)
    EVT_BUTTON(XRCID("btnDeleteTrust"), ScriptingSettingsDlg::OnDeleteTrust)
END_EVENT_TABLE()

ScriptingSettingsDlg::ScriptingSettingsDlg(wxWindow* parent)
    : m_IgnoreTextEvents(false)
{
    //ctor
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgScriptingSettings"));

    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    list->InsertColumn(0, _("Script"), wxLIST_FORMAT_LEFT, 160);
    list->InsertColumn(1, _("Enabled"), wxLIST_FORMAT_LEFT, 64);
    list->InsertColumn(2, _("Menu"), wxLIST_FORMAT_LEFT, 160);

    FillScripts();

    list = XRCCTRL(*this, "lstTrustedScripts", wxListCtrl);
    list->InsertColumn(0, _("Script"), wxLIST_FORMAT_LEFT, 160);
    list->InsertColumn(1, _("Signature"));
    list->InsertColumn(2, _("Temp?"));

    FillTrusts();
}

ScriptingSettingsDlg::~ScriptingSettingsDlg()
{
    //dtor
}

void ScriptingSettingsDlg::FillScripts()
{
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    list->DeleteAllItems();

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("scripting"));
    wxArrayString keys = mgr->EnumerateKeys(_T("/startup_scripts"));

    for (size_t i = 0; i < keys.GetCount(); ++i)
    {
        ScriptEntry se;
        wxString ser;
        if (mgr->Read(_T("/startup_scripts/") + keys[i], &ser))
        {
            se.SerializeIn(ser);
            m_ScriptsVector.push_back(se);

            long item = list->InsertItem(list->GetItemCount(), se.script);
            list->SetItem(item, 1, se.enabled ? _("Yes") : _("No"));
            list->SetItem(item, 2, se.registered && !se.menu.IsEmpty() ? se.menu : wxString(wxEmptyString));
        }
    }

    UpdateState();
}

void ScriptingSettingsDlg::UpdateState()
{
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    bool en = sel != -1;

    const ScriptEntry& se = m_ScriptsVector[sel];

    XRCCTRL(*this, "btnDelete", wxButton)->Enable(en);
    XRCCTRL(*this, "chkEnableScript", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "txtScript", wxTextCtrl)->Enable(en && se.enabled);
    XRCCTRL(*this, "btnBrowseScript", wxButton)->Enable(en && se.enabled);
    XRCCTRL(*this, "chkRegisterScript", wxCheckBox)->Enable(en && se.enabled);
    XRCCTRL(*this, "txtScriptMenu", wxTextCtrl)->Enable(en && se.enabled && se.registered);
}

void ScriptingSettingsDlg::FillTrusts()
{
    wxListCtrl* list = XRCCTRL(*this, "lstTrustedScripts", wxListCtrl);
    list->DeleteAllItems();

    const ScriptingManager::TrustedScripts& trusts = Manager::Get()->GetScriptingManager()->GetTrustedScripts();
    ScriptingManager::TrustedScripts::const_iterator it;
    for (it = trusts.begin(); it != trusts.end(); ++it)
    {
        const ScriptingManager::TrustedScriptProps& props = it->second;

        long item = list->InsertItem(list->GetItemCount(), it->first);
        list->SetItem(item, 1, wxString::Format(_T("%x"), props.crc));
        list->SetItem(item, 2, !props.permanent ? _("Yes") : _(""));
    }

    UpdateTrustsState();

    // fill main switches
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("security"));
    XRCCTRL(*this, "chkMkDir", wxCheckBox)->SetValue(mgr->ReadBool(_T("CreateDir"), false));
    XRCCTRL(*this, "chkRmDir", wxCheckBox)->SetValue(mgr->ReadBool(_T("RemoveDir"), false));
    XRCCTRL(*this, "chkCp", wxCheckBox)->SetValue(mgr->ReadBool(_T("CopyFile"), false));
    XRCCTRL(*this, "chkMv", wxCheckBox)->SetValue(mgr->ReadBool(_T("RenameFile"), false));
    XRCCTRL(*this, "chkRm", wxCheckBox)->SetValue(mgr->ReadBool(_T("RemoveFile"), false));
    XRCCTRL(*this, "chkTouch", wxCheckBox)->SetValue(mgr->ReadBool(_T("CreateFile"), false));
    XRCCTRL(*this, "chkExec", wxCheckBox)->SetValue(mgr->ReadBool(_T("Execute"), false));
}

void ScriptingSettingsDlg::UpdateTrustsState()
{
    wxListCtrl* list = XRCCTRL(*this, "lstTrustedScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    bool en = sel != -1;

    XRCCTRL(*this, "btnDeleteTrust", wxButton)->Enable(en);
    XRCCTRL(*this, "btnValidateTrusts", wxButton)->Enable(en);
}

void ScriptingSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("scripting"));
        mgr->DeleteSubPath(_T("/startup_scripts"));

        ScriptsVector::iterator it;
        int i = 0;
        for (it = m_ScriptsVector.begin(); it != m_ScriptsVector.end(); ++it, ++i)
        {
            ScriptEntry& se = *it;
            wxString key = wxString::Format(_T("/startup_scripts/script%d"), i);
            mgr->Write(key, se.SerializeOut());
        }

        mgr = Manager::Get()->GetConfigManager(_T("security"));
        mgr->Write(_T("CreateDirRecursively"), XRCCTRL(*this, "chkMkDir", wxCheckBox)->GetValue());
        mgr->Write(_T("RemoveDir"), XRCCTRL(*this, "chkRmDir", wxCheckBox)->GetValue());
        mgr->Write(_T("CopyFile"), XRCCTRL(*this, "chkCp", wxCheckBox)->GetValue());
        mgr->Write(_T("RenameFile"), XRCCTRL(*this, "chkMv", wxCheckBox)->GetValue());
        mgr->Write(_T("RemoveFile"), XRCCTRL(*this, "chkRm", wxCheckBox)->GetValue());
        mgr->Write(_T("CreateFile"), XRCCTRL(*this, "chkTouch", wxCheckBox)->GetValue());
        mgr->Write(_T("Execute"), XRCCTRL(*this, "chkExec", wxCheckBox)->GetValue());
    }

    wxDialog::EndModal(retCode);
}

void ScriptingSettingsDlg::LoadItem(long item)
{
    m_IgnoreTextEvents = true;

    // load
    ScriptEntry& se = m_ScriptsVector[item];

    XRCCTRL(*this, "chkEnableScript", wxCheckBox)->SetValue(se.enabled);
    XRCCTRL(*this, "txtScript", wxTextCtrl)->SetValue(se.script);
    XRCCTRL(*this, "chkRegisterScript", wxCheckBox)->SetValue(se.registered);
    XRCCTRL(*this, "txtScriptMenu", wxTextCtrl)->SetValue(se.menu);

    m_IgnoreTextEvents = false;
}

void ScriptingSettingsDlg::SaveItem(long item)
{
    m_IgnoreTextEvents = true;

    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    ScriptEntry& se = m_ScriptsVector[item];

    se.enabled = XRCCTRL(*this, "chkEnableScript", wxCheckBox)->GetValue();
    se.script = XRCCTRL(*this, "txtScript", wxTextCtrl)->GetValue();
    se.registered = XRCCTRL(*this, "chkRegisterScript", wxCheckBox)->GetValue();
    se.menu = XRCCTRL(*this, "txtScriptMenu", wxTextCtrl)->GetValue();

    // update view
    list->SetItem(item, 0, se.script);
    list->SetItem(item, 1, se.enabled ? _("Yes") : _("No"));
    list->SetItem(item, 2, se.registered && !se.menu.IsEmpty() ? se.menu : wxString(wxEmptyString));

    m_IgnoreTextEvents = false;
}

void ScriptingSettingsDlg::OnListSelection(wxListEvent& event)
{
//    DBGLOG(_T("Selected %d"), event.GetIndex());

    // load
    long sel = event.GetIndex();
    LoadItem(sel);

    UpdateState();
}

void ScriptingSettingsDlg::OnListDeselection(wxListEvent& event)
{
//    DBGLOG(_T("Deselected %d"), event.GetIndex());

    // save
    long sel = event.GetIndex();
    SaveItem(sel);

    UpdateState();
}

void ScriptingSettingsDlg::OnScriptChanged(wxCommandEvent& event)
{
    if (m_IgnoreTextEvents)
        return;
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    SaveItem(sel);
    UpdateState();
}

void ScriptingSettingsDlg::OnScriptMenuChanged(wxCommandEvent& event)
{
    if (m_IgnoreTextEvents)
        return;
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    SaveItem(sel);
    UpdateState();
}

void ScriptingSettingsDlg::OnEnable(wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    SaveItem(sel);
    UpdateState();
}

void ScriptingSettingsDlg::OnRegister(wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    SaveItem(sel);
    UpdateState();
}

void ScriptingSettingsDlg::OnAddScript(wxCommandEvent& event)
{
    ScriptEntry se;
    se.script = _T("new.script");
    se.enabled = true;
    se.registered = false;
    m_ScriptsVector.push_back(se);

    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);

    // update view
    long item = list->InsertItem(list->GetItemCount(), se.script);
    list->SetItem(item, 1, _("No"));
    list->SetItem(item, 2, wxString(_("No")));

    list->SetItemState(item, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);

    OnBrowse(event);
}

void ScriptingSettingsDlg::OnRemoveScript(wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "chkStartupScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    list->DeleteItem(sel);
    m_ScriptsVector.erase(m_ScriptsVector.begin() + sel);

    if (sel > list->GetItemCount())
        --sel;
    list->SetItemState(sel, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    if (sel >= 0)
        LoadItem(sel);
    UpdateState();
}

void ScriptingSettingsDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Select script file"),
                            XRCCTRL(*this, "txtScript", wxTextCtrl)->GetValue(),
                            XRCCTRL(*this, "txtScript", wxTextCtrl)->GetValue(),
                            FileFilters::GetFilterString(_T(".script")),
                            wxOPEN | wxHIDE_READONLY);
    PlaceWindow(dlg);
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString sel = UnixFilename(dlg->GetPath());
        wxString userdir = UnixFilename(ConfigManager::GetFolder(sdScriptsUser));
        wxString globaldir = UnixFilename(ConfigManager::GetFolder(sdScriptsGlobal));
        wxFileName f(sel);
        if (sel.StartsWith(userdir))
        {
            f.MakeRelativeTo(userdir);
        }
        else if (sel.StartsWith(globaldir))
        {
            f.MakeRelativeTo(globaldir);
        }
        XRCCTRL(*this, "txtScript", wxTextCtrl)->SetValue(f.GetFullPath());
    }
    dlg->Destroy();
}

void ScriptingSettingsDlg::OnTrustSelection(wxListEvent& event)
{
    UpdateTrustsState();
}

void ScriptingSettingsDlg::OnDeleteTrust(wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lstTrustedScripts", wxListCtrl);
    long sel = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    wxString script = list->GetItemText(sel);
    Manager::Get()->GetScriptingManager()->RemoveTrust(script);
    list->DeleteItem(sel);

    UpdateTrustsState();
}

void ScriptingSettingsDlg::OnValidateTrusts(wxCommandEvent& event)
{
    bool check = true;
    wxListCtrl* list = XRCCTRL(*this, "lstTrustedScripts", wxListCtrl);
    for (int i = 0; i < list->GetItemCount(); ++i)
    {
        wxString script = list->GetItemText(i);
        if (!Manager::Get()->GetScriptingManager()->IsScriptTrusted(script))
            check = false;
    }

    if (check)
        cbMessageBox(_("All script trusts are valid!"), _("Information"), wxICON_INFORMATION);
}
