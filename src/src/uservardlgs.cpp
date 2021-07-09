#include "uservardlgs.h"
#include "annoyingdialog.h"

void UserVarManagerGUI::DisplayInfoWindow(const wxString &title, const wxString &msg)
{
    InfoWindow::Display(title, msg , 8000, 1000);
}

void UserVarManagerGUI::OpenEditWindow(const wxArrayString &var)
{
    UsrGlblMgrEditDialog d;
    for (unsigned int i = 0; i < var.GetCount(); ++i)
    {
        d.AddVar(var[i]);
    }
    PlaceWindow(&d);
    d.ShowModal();
}

wxString UserVarManagerGUI::GetVariable(wxWindow* parent, const wxString &old)
{
    GetUserVariableDialog dlg(parent, old);
    PlaceWindow(&dlg);
    dlg.ShowModal();
    return dlg.GetVariable();
}


BEGIN_EVENT_TABLE(GetUserVariableDialog, wxScrollingDialog)
    EVT_BUTTON(XRCID("ID_CONFIG"), GetUserVariableDialog::OnConfig)
    EVT_BUTTON(XRCID("wxID_OK"), GetUserVariableDialog::OnOK)
    EVT_BUTTON(XRCID("wxID_CANCEL"), GetUserVariableDialog::OnCancel)
    EVT_TREE_ITEM_ACTIVATED(XRCID("ID_GET_USER_VAR_TREE"), GetUserVariableDialog::OnActivated)
END_EVENT_TABLE()

GetUserVariableDialog::GetUserVariableDialog(wxWindow *parent, const wxString &old) :
    m_old(old)
{
    wxXmlResource::Get()->LoadObject(this, parent, wxT("dlgGetGlobalUsrVar"), wxT("wxScrollingDialog"));
    m_treectrl = XRCCTRL(*this, "ID_GET_USER_VAR_TREE", wxTreeCtrl);

    if (m_treectrl == nullptr)
        Manager::Get()->GetLogManager()->LogError(_("Failed to load dlgGetGlobalUsrVar"));

    Load();

    // Try to open the old variable
    if (m_old != wxEmptyString && m_old.StartsWith(wxT("$(#")))
    {
        // Remove "$(#"
        wxString tmp = m_old.AfterFirst('#');
        // Remove the last ")"
        tmp = tmp.BeforeFirst(')');
        // In tmp is now "var.subVar". subVar is optional
        wxString var[2];
        var[0] = tmp.Before('.');
        var[1] = tmp.After('.');
        wxTreeItemId root = m_treectrl->GetRootItem();
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treectrl->GetFirstChild(root, cookie);
        unsigned int i = 0;
        while (child.IsOk())
        {
            if (m_treectrl->GetItemText(child) == var[i])
            {
                m_treectrl->EnsureVisible(child);
                m_treectrl->SelectItem(child);
                i++;
                if (i >= 2 || var[i] == wxEmptyString)
                    break;

                root = child;
                child = m_treectrl->GetFirstChild(root, cookie);
            }
            else
                child = m_treectrl->GetNextChild(root, cookie);
        }
    }

    Fit();
    SetMinSize(GetSize());
}

void GetUserVariableDialog::Load()
{
    if (m_treectrl == nullptr)
        return;

    m_treectrl->DeleteAllItems();

    ConfigManager* CfgMan = Manager::Get()->GetConfigManager(wxT("gcv"));
    const wxString &ActiveSet = Manager::Get()->GetConfigManager(wxT("gcv"))->Read(wxT("/active"));
    wxArrayString vars = CfgMan->EnumerateSubPaths(UserVariableManager::cSets + ActiveSet + wxT("/"));
    vars.Sort();

    wxTreeItemId root = m_treectrl->AddRoot(ActiveSet);

    for (wxArrayString::iterator var_itr = vars.begin(); var_itr != vars.end() ; ++var_itr)
    {
        wxTreeItemId varId = m_treectrl->AppendItem(root, (*var_itr));
        wxArrayString subItems = CfgMan->EnumerateKeys(UserVariableManager::cSets + ActiveSet + wxT("/") + (*var_itr) + wxT("/"));

        for (wxArrayString::iterator subItr = subItems.begin(); subItr != subItems.end() ; ++subItr)
            m_treectrl->AppendItem(varId, (*subItr));
    }
    m_treectrl->Expand(root);
}

void GetUserVariableDialog::OnOK(cb_unused wxCommandEvent& evt)
{
    m_SelectedVar = GetSelectedVariable();
    EndModal(wxID_OK);
}

void GetUserVariableDialog::OnActivated(cb_unused wxTreeEvent& event)
{
    m_SelectedVar = GetSelectedVariable();
    EndModal(wxID_OK);
}

void GetUserVariableDialog::OnCancel(cb_unused wxCommandEvent& evt)
{
    m_SelectedVar = wxEmptyString;
    EndModal(wxID_CANCEL);
}

void GetUserVariableDialog::OnConfig(cb_unused wxCommandEvent& evt)
{
    Manager::Get()->GetUserVariableManager()->Configure();
    Load();
}

wxString GetUserVariableDialog::GetSelectedVariable()
{
    wxTreeItemId subVar = m_treectrl->GetSelection();
    wxTreeItemId var = m_treectrl->GetItemParent(subVar);

    if (subVar == m_treectrl->GetRootItem() || !subVar.IsOk())
        return wxEmptyString;

    wxString ret;
    ret << wxT("$(#");
    if (var == m_treectrl->GetRootItem()) // It is only a variable
        ret << m_treectrl->GetItemText(subVar) << wxT(")");
    else // var with subitem
        ret << m_treectrl->GetItemText(var) << wxT(".") <<  m_treectrl->GetItemText(subVar) << wxT(")");

    return ret;
}

BEGIN_EVENT_TABLE(UsrGlblMgrEditDialog, wxScrollingDialog)
    EVT_BUTTON(XRCID("cloneVar"), UsrGlblMgrEditDialog::CloneVar)
    EVT_BUTTON(XRCID("newVar"), UsrGlblMgrEditDialog::NewVar)
    EVT_BUTTON(XRCID("deleteVar"), UsrGlblMgrEditDialog::DeleteVar)
    EVT_BUTTON(XRCID("cloneSet"), UsrGlblMgrEditDialog::CloneSet)
    EVT_BUTTON(XRCID("newSet"), UsrGlblMgrEditDialog::NewSet)
    EVT_BUTTON(XRCID("deleteSet"), UsrGlblMgrEditDialog::DeleteSet)
    EVT_BUTTON(XRCID("help"), UsrGlblMgrEditDialog::Help)
    EVT_BUTTON(wxID_OK, UsrGlblMgrEditDialog::OnOK)
    EVT_CLOSE(UsrGlblMgrEditDialog::CloseHandler)
    EVT_BUTTON(XRCID("fs1"), UsrGlblMgrEditDialog::OnFS)
    EVT_BUTTON(XRCID("fs2"), UsrGlblMgrEditDialog::OnFS)
    EVT_BUTTON(XRCID("fs3"), UsrGlblMgrEditDialog::OnFS)
    EVT_BUTTON(XRCID("fs4"), UsrGlblMgrEditDialog::OnFS)
    EVT_BUTTON(XRCID("fs5"), UsrGlblMgrEditDialog::OnFS)

    EVT_CHOICE(XRCID("selSet"), UsrGlblMgrEditDialog::SelectSet)
    EVT_LISTBOX(XRCID("selVar"), UsrGlblMgrEditDialog::SelectVar)
END_EVENT_TABLE()

UsrGlblMgrEditDialog::UsrGlblMgrEditDialog(const wxString& var) :
    m_CurrentSet(Manager::Get()->GetConfigManager(_T("gcv"))->Read(_T("/active"))),
    m_CurrentVar(var)
{
    wxXmlResource::Get()->LoadObject(this, Manager::Get()->GetAppWindow(), _T("dlgGlobalUservars"),_T("wxScrollingDialog"));
    m_SelSet    = XRCCTRL(*this, "selSet",   wxChoice);
    m_SelVar    = XRCCTRL(*this, "selVar",   wxListBox);
    m_DeleteSet = XRCCTRL(*this, "deleteSet",wxButton);

    m_Base    = XRCCTRL(*this, "base",    wxTextCtrl);
    m_Include = XRCCTRL(*this, "include", wxTextCtrl);
    m_Lib     = XRCCTRL(*this, "lib",     wxTextCtrl);
    m_Obj     = XRCCTRL(*this, "obj",     wxTextCtrl);
    m_Bin     = XRCCTRL(*this, "bin",     wxTextCtrl);

    wxSplitterWindow *splitter = XRCCTRL(*this, "splitter", wxSplitterWindow);
    if (splitter)
        splitter->SetSashGravity(0.7);

    wxString n;
    for (unsigned int i = 0; i < 7; ++i)
    {
        n.Printf(_T("n%d"), i);
        m_Name[i]  = (wxTextCtrl*) FindWindow(n);

        n.Printf(_T("v%d"), i);
        m_Value[i] = (wxTextCtrl*) FindWindow(n);
    }

    m_CfgMan = Manager::Get()->GetConfigManager(_T("gcv"));
    m_CfgMan->Exists(_T("/sets/default/foo"));

    UpdateChoices();
    Load();
    PlaceWindow(this);
}

void UsrGlblMgrEditDialog::DoClose()
{
    Save();
    EndModal(wxID_OK);
}


void UsrGlblMgrEditDialog::CloneVar(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new clone:"), _("Clone Variable"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString clone = d.GetValue();

        if (clone.IsEmpty())
            return;

        Sanitise(clone);

        wxString srcPath(_T("/sets/") + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));
        wxString dstPath(_T("/sets/") + m_CurrentSet + _T('/') + clone + _T('/'));

        wxArrayString existing = m_CfgMan->EnumerateSubPaths(_T("/sets/" + m_CurrentSet));

        if (existing.Index(clone) != wxNOT_FOUND)
        {
            wxString msg;
            msg.Printf(_("Cowardly refusing to overwrite existing variable \"%s\"."), clone.wx_str());
            InfoWindow::Display(_("Clone Set"), msg);
            return;
        }

        wxArrayString members = m_CfgMan->EnumerateKeys(srcPath);

        for (unsigned j = 0; j < members.GetCount(); ++j)
            m_CfgMan->Write(dstPath + members[j], m_CfgMan->Read(srcPath + members[j]));

        m_CurrentVar = clone;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::CloneSet(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new clone:"), _("Clone Set"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString clone = d.GetValue();
        Sanitise(clone);

        if (clone.IsEmpty())
            return;
        wxArrayString existing = m_CfgMan->EnumerateSubPaths(_T("/sets"));

        if (existing.Index(clone) != wxNOT_FOUND)
        {
            wxString msg;
            msg.Printf(_("Cowardly refusing overwrite existing set \"%s\"."), clone.wx_str());
            InfoWindow::Display(_("Clone Set"), msg);
            return;
        }

        wxString srcPath(UserVariableManager::cSets + m_CurrentSet + _T("/"));
        wxString dstPath(UserVariableManager::cSets + clone + _T("/"));
        wxString oldpath, newpath;

        wxArrayString vars = m_CfgMan->EnumerateSubPaths(srcPath);

        for (unsigned int i = 0; i < vars.GetCount(); ++i)
        {
            wxArrayString members = m_CfgMan->EnumerateKeys(srcPath + vars[i]);

            for (unsigned j = 0; j < members.GetCount(); ++j)
            {
                wxString item = vars[i] + _T("/") + members[j];
                m_CfgMan->Write(dstPath + item, m_CfgMan->Read(srcPath + item));
            }
        }
        m_CurrentSet = clone;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::DeleteVar(cb_unused wxCommandEvent& event)
{
    wxString msg;
    msg.Printf(_("Delete the global compiler variable \"%s\" from this set?"), m_CurrentVar.wx_str());
    AnnoyingDialog d(_("Delete Global Variable"), msg, wxART_QUESTION);
    PlaceWindow(&d);
    if (d.ShowModal() == AnnoyingDialog::rtYES)
    {
        m_CfgMan->DeleteSubPath(UserVariableManager::cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));
        m_CurrentVar = wxEmptyString;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::DeleteSet(cb_unused wxCommandEvent& event)
{
    wxString msg;
    msg.Printf(_("Do you really want to delete the entire\n"
                 "global compiler variable set \"%s\"?\n\n"
                 "This cannot be undone."), m_CurrentSet.wx_str());
    AnnoyingDialog d(_("Delete Global Variable Set"), msg, wxART_QUESTION);
    PlaceWindow(&d);
    if (d.ShowModal() == AnnoyingDialog::rtYES)
    {
        m_CfgMan->DeleteSubPath(UserVariableManager::cSets + m_CurrentSet + _T('/'));
        m_CurrentSet = wxEmptyString;
        m_CurrentVar = wxEmptyString;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::AddVar(const wxString& name)
{
    if (name.IsEmpty())
        return;
    m_CurrentVar = name;
    m_CfgMan->Exists(_T("/sets/") + m_CurrentSet + _T('/') + name + _T('/'));

    m_CurrentVar = name;
    UpdateChoices();
    Load();
}

void UsrGlblMgrEditDialog::Sanitise(wxString& s)
{
    s.Trim().Trim(true);

    if (s.IsEmpty())
    {
        s = _T("[?empty?]");
        return;
    }

    for (unsigned int i = 0; i < s.length(); ++i)
#if wxCHECK_VERSION(3, 0, 0)
        s[i] = wxIsalnum(s.GetChar(i)) ? s.GetChar(i) : wxUniChar('_');
#else
        s[i] = wxIsalnum(s.GetChar(i)) ? s.GetChar(i) : _T('_');
#endif

    if (s.GetChar(0) == _T('_'))
        s.Prepend(_T("set"));

    if (s.GetChar(0) >= _T('0') && s.GetChar(0) <= _T('9'))
        s.Prepend(_T("set_"));
}

void UsrGlblMgrEditDialog::NewVar(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new variable:"), _("New Variable"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString name = d.GetValue();
        Save();
        Sanitise(name);
        AddVar(name);
    }
}

void UsrGlblMgrEditDialog::NewSet(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new set:"), _("New Set"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString name = d.GetValue();
        Sanitise(name);

        if (name.IsEmpty())
            return;
        m_CurrentSet = name;
        m_CfgMan->Exists(_T("/sets/") + name + _T('/'));

        m_CurrentSet = name;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::SelectVar(cb_unused wxCommandEvent& event)
{
    Save();
    m_CurrentVar = m_SelVar->GetStringSelection();
    Load();
}

void UsrGlblMgrEditDialog::SelectSet(cb_unused wxCommandEvent& event)
{
    Save();
    m_CurrentSet = m_SelSet->GetStringSelection();
    m_CfgMan->Write(_T("/active"), m_CurrentSet);
    UpdateChoices();
    Load();
}


void UsrGlblMgrEditDialog::Load()
{
    m_DeleteSet->Enable(!m_CurrentSet.IsSameAs(_T("default")));

    wxString path(UserVariableManager::cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));

    wxArrayString knownMembers = m_CfgMan->EnumerateKeys(path);

    for (unsigned int i = 0; i < UserVariableManager::builtinMembers.GetCount(); ++i)
    {
        ((wxTextCtrl*) FindWindow(UserVariableManager::builtinMembers[i]))->SetValue(m_CfgMan->Read(path + UserVariableManager::builtinMembers[i]));

        int index = knownMembers.Index(UserVariableManager::builtinMembers[i], false);
        if (index != wxNOT_FOUND)
            knownMembers.RemoveAt(index);
    }

    for (unsigned int i = 0; i < 7; ++i)
    {
        m_Name[i]->SetValue(wxEmptyString);
        m_Value[i]->SetValue(wxEmptyString);
    }

    for (unsigned int i = 0; i < knownMembers.GetCount(); ++i)
    {
        m_Name[i]->SetValue(knownMembers[i].Lower());
        m_Value[i]->SetValue(m_CfgMan->Read(path + knownMembers[i]));
    }
}

void UsrGlblMgrEditDialog::Save()
{
    wxString path(UserVariableManager::cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));
    wxString mbr(_T('#') + m_CurrentVar + _T('.'));

    m_CfgMan->DeleteSubPath(path);

    wxString s, t;
    for (unsigned int i = 0; i < UserVariableManager::builtinMembers.GetCount(); ++i)
    {
        t = ((wxTextCtrl*) FindWindow(UserVariableManager::builtinMembers[i]))->GetValue();

        if (   i == 0
            && (   (   !m_CurrentVar.IsEmpty()
                    && t.IsEmpty() )
                || t.Contains(_T('#') + m_CurrentVar) ) )
        {
            if (cbMessageBox(_("Are you sure you want to save an invalid global variable?"), _("Global variables"),
                             wxYES_NO | wxICON_QUESTION, this) == wxID_YES)
            {
                t.assign(_T("(invalid)"));
            }
        }

        if (t.Contains(mbr + UserVariableManager::builtinMembers[i]))
            t.assign(_T("(invalid)"));

        if (!t.IsEmpty())
            m_CfgMan->Write(path + UserVariableManager::builtinMembers[i], t);
    }

    for (unsigned int i = 0; i < 7; ++i)
    {
        s = m_Name[i]->GetValue();
        t = m_Value[i]->GetValue();

        if (t.Contains(mbr + s))
            t.assign(_T("(invalid)"));

        if (!s.IsEmpty() && !t.IsEmpty())
            m_CfgMan->Write(path + s, t);
    }
}

void UsrGlblMgrEditDialog::UpdateChoices()
{
    if (m_CurrentSet.IsEmpty())
        m_CurrentSet = _T("default");

    wxArrayString sets = m_CfgMan->EnumerateSubPaths(UserVariableManager::cSets);
    wxArrayString vars = m_CfgMan->EnumerateSubPaths(UserVariableManager::cSets + m_CurrentSet + _T("/"));
    sets.Sort();
    vars.Sort();

    m_SelSet->Clear();
    m_SelSet->Append(sets);
    m_SelVar->Clear();
    m_SelVar->Append(vars);

    if (m_CurrentVar.IsEmpty() && m_SelVar->GetCount() > 0)
        m_CurrentVar = m_SelVar->GetString(0);

    m_SelSet->SetStringSelection(m_CurrentSet);
    m_SelVar->SetStringSelection(m_CurrentVar);
}


void UsrGlblMgrEditDialog::OnFS(wxCommandEvent& event)
{
    wxTextCtrl* c = nullptr;
    int id = event.GetId();

    if      (id == XRCID("fs1"))
        c = m_Base;
    else if (id == XRCID("fs2"))
        c = m_Include;
    else if (id == XRCID("fs3"))
        c = m_Lib;
    else if (id == XRCID("fs4"))
        c = m_Obj;
    else if (id == XRCID("fs5"))
        c = m_Bin;
    else
        cbThrow(_T("Encountered invalid button ID"));

    wxString path = ChooseDirectory(this, _("Choose a location"), c->GetValue());
    if (!path.IsEmpty())
        c->SetValue(path);
}

void UsrGlblMgrEditDialog::Help(cb_unused wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Global_compiler_variables"));
}

