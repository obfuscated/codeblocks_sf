/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "uservarmanager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "cbexception.h"
    #include "infowindow.h"

    #include <wx/button.h>
    #include "scrollingdialog.h"
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
    #include <wx/splitter.h>
#endif

#include "annoyingdialog.h"
#include <wx/choice.h>
#include <wx/textdlg.h> //wxTextEntryDialog

#if wxCHECK_VERSION(2, 9, 0)
#include <wx/unichar.h>
#endif

#include <ctype.h>

template<> UserVariableManager* Mgr<UserVariableManager>::instance   = nullptr;
template<> bool                 Mgr<UserVariableManager>::isShutdown = false;

const wxString cBase     (_T("base"));
const wxString cDir      (_T("dir"));
const wxChar   cSlash(_T ('/'));
const wxString cSlashBase(_T("/base"));
const wxString cInclude  (_T("include"));
const wxString cLib      (_T("lib"));
const wxString cObj      (_T("obj"));
const wxString cBin      (_T("bin"));
const wxString cCflags   (_T("cflags"));
const wxString cLflags   (_T("lflags"));
const wxString cSets     (_T("/sets/"));

const wxChar *bim[] =
{
    _T("base"),
    _T("include"),
    _T("lib"),
    _T("obj"),
    _T("bin"),
    _T("cflags"),
    _T("lflags")
};
const wxArrayString builtinMembers((size_t) 7, bim);

class UsrGlblMgrEditDialog : public wxScrollingDialog
{
    wxString m_CurrentSet;
    wxString m_CurrentVar;

    wxChoice *m_SelSet;
    wxChoice *m_SelVar;
    wxButton *m_DeleteSet;

    wxTextCtrl *m_Base;
    wxTextCtrl *m_Include;
    wxTextCtrl *m_Lib;
    wxTextCtrl *m_Obj;
    wxTextCtrl *m_Bin;

    wxTextCtrl *m_Name[7];
    wxTextCtrl *m_Value[7];

    ConfigManager *m_CfgMan;

    void Help(wxCommandEvent& event);
    void DoClose();
    void OnOK(cb_unused wxCommandEvent& event)       { DoClose(); };
    void OnCancel(cb_unused wxCommandEvent& event)   { DoClose(); };
    void CloseHandler(cb_unused wxCloseEvent& event) { DoClose(); };

    void CloneVar(wxCommandEvent&  event);
    void CloneSet(wxCommandEvent&  event);
    void NewVar(wxCommandEvent&    event);
    void NewSet(wxCommandEvent&    event);
    void DeleteVar(wxCommandEvent& event);
    void DeleteSet(wxCommandEvent& event);
    // handler for the folder selection button
    void OnFS(wxCommandEvent& event);

    void SelectSet(wxCommandEvent& event);
    void SelectVar(wxCommandEvent& event);

    void Load();
    void Save();
    void UpdateChoices();
    void AddVar(const wxString& var);
    void Sanitise(wxString& s);

    DECLARE_EVENT_TABLE()

public:
    UsrGlblMgrEditDialog(const wxString& var = wxEmptyString);
    friend class UserVariableManager;
};

void UserVariableManager::Configure()
{
    UsrGlblMgrEditDialog d;
    PlaceWindow(&d);
    d.ShowModal();
    m_ActiveSet = Manager::Get()->GetConfigManager(_T("gcv"))->Read(_T("/active"));
}


wxString UserVariableManager::Replace(const wxString& variable)
{
    wxString package = variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).MakeLower();
    wxString member  = variable.AfterFirst(wxT('.')).MakeLower();

    wxString path(cSets + m_ActiveSet + _T('/') + package + _T('/'));

    wxString base = m_CfgMan->Read(path + cBase);

    if (base.IsEmpty())
    {
        if (Manager::Get()->GetProjectManager()->IsLoading())
        {
            // a project/workspace is being loaded.
            // no need to bug the user now about global vars.
            // just preempt it; ProjectManager will call Arrogate() when it's done.
            Preempt(variable);
            return variable;
        }
        else
        {
            wxString msg;
            msg.Printf(_("In the currently active set, Code::Blocks does not know\n"
                         "the global compiler variable \"%s\".\n\n"
                         "Please define it."), package.wx_str());
            InfoWindow::Display(_("Global Compiler Variables"), msg , 8000, 1000);
            UsrGlblMgrEditDialog d;
            d.AddVar(package);
            PlaceWindow(&d);
            d.ShowModal();
        }
    }

    if (member.IsEmpty() || member.IsSameAs(cBase))
        return base;

    if (member.IsSameAs(cInclude) || member.IsSameAs(cLib) || member.IsSameAs(cObj) || member.IsSameAs(cBin))
    {
        wxString ret = m_CfgMan->Read(path + member);
        if (ret.IsEmpty())
            ret = base + _T('/') + member;
        return ret;
    }

    const wxString wtf(wxT("#$%&???WTF???&%$#"));
    wxString ret = m_CfgMan->Read(path + member, wtf);
    if ( ret.IsSameAs(wtf) )
    {
        wxString msg;
        msg.Printf(_("In the currently active set, Code::Blocks does not know\n"
                     "the member \"%s\" of the global compiler variable \"%s\".\n\n"
                     "Please define it."), member.wx_str(), package.wx_str());
        InfoWindow::Display(_("Global Compiler Variables"), msg , 8000, 1000);
    }

    return ret;
}


void UserVariableManager::Preempt(const wxString& variable)
{
    if (variable.find(_T('#')) == wxString::npos)
        return;

    wxString member(variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).BeforeFirst(wxT(')')).MakeLower());

    if (!m_CfgMan->Exists(cSets + m_ActiveSet + _T('/') + member + _T("/base")) &&
            m_Preempted.Index(member) == wxNOT_FOUND)
    {
        m_Preempted.Add(member);
    }
}

bool UserVariableManager::Exists(const wxString& variable) const
{
    if (variable.find(_T('#')) == wxString::npos)
        return false;

    wxString member(variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).BeforeFirst(wxT(')')).MakeLower());
    return !m_CfgMan->Exists(cSets + m_ActiveSet + _T('/') + member + _T("/base"));
}

void UserVariableManager::Arrogate()
{
    if (m_Preempted.GetCount() == 0)
        return;

    wxString peList;

    UsrGlblMgrEditDialog d;

    for (unsigned int i = 0; i < m_Preempted.GetCount(); ++i)
    {
        d.AddVar(m_Preempted[i]);
        peList << m_Preempted[i] << _T('\n');
    }
    peList = peList.BeforeLast('\n'); // remove trailing newline

    wxString msg;
    if (m_Preempted.GetCount() == 1)
        msg.Printf(_("In the currently active set, Code::Blocks does not know\n"
                     "the global compiler variable \"%s\".\n\n"
                     "Please define it."), peList.wx_str());
    else
        msg.Printf(_("In the currently active set, Code::Blocks does not know\n"
                     "the following global compiler variables:\n"
                     "%s\n\n"
                     "Please define them."), peList.wx_str());

    PlaceWindow(&d);
    m_Preempted.Clear();
    InfoWindow::Display(_("Global Compiler Variables"), msg , 8000 + 800*m_Preempted.GetCount(), 100);

    d.ShowModal();
}

UserVariableManager::UserVariableManager()
{
    m_CfgMan = Manager::Get()->GetConfigManager(_T("gcv"));
    Migrate();
}

void UserVariableManager::Migrate()
{
    ConfigManager *cfgman_gcv = Manager::Get()->GetConfigManager(_T("gcv"));

    m_ActiveSet = cfgman_gcv->Read(_T("/active"));

    if (!m_ActiveSet.IsEmpty())
        return;

    m_ActiveSet = _T("default");
    cfgman_gcv->Exists(_T("/sets/default/foo")); // assert /sets/default
    cfgman_gcv->Write(_T("/active"), m_ActiveSet);
    wxString oldpath;
    wxString newpath;

    ConfigManager *cfgman_old = Manager::Get()->GetConfigManager(_T("global_uservars"));
    wxArrayString vars = cfgman_old->EnumerateSubPaths(_T("/"));

    for (unsigned int i = 0; i < vars.GetCount(); ++i)
    {
        vars[i].Prepend(_T('/'));
        wxArrayString members = cfgman_old->EnumerateKeys(vars[i]);

        for (unsigned j = 0; j < members.GetCount(); ++j)
        {
            oldpath.assign(vars[i] + _T("/") + members[j]);
            newpath.assign(_T("/sets/default") + vars[i] + _T("/") + members[j]);

            cfgman_gcv->Write(newpath, cfgman_old->Read(oldpath));
        }
    }
    cfgman_old->Delete();
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
    EVT_CHOICE(XRCID("selVar"), UsrGlblMgrEditDialog::SelectVar)
END_EVENT_TABLE()

UsrGlblMgrEditDialog::UsrGlblMgrEditDialog(const wxString& var) :
    m_CurrentSet(Manager::Get()->GetConfigManager(_T("gcv"))->Read(_T("/active"))),
    m_CurrentVar(var)
{
    wxXmlResource::Get()->LoadObject(this, Manager::Get()->GetAppWindow(), _T("dlgGlobalUservars"),_T("wxScrollingDialog"));
    m_SelSet    = XRCCTRL(*this, "selSet",    wxChoice);
    m_SelVar    = XRCCTRL(*this, "selVar",    wxChoice);
    m_DeleteSet = XRCCTRL(*this, "deleteSet", wxButton);

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

        wxString srcPath(cSets + m_CurrentSet + _T("/"));
        wxString dstPath(cSets + clone + _T("/"));
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
        m_CfgMan->DeleteSubPath(cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));
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
        m_CfgMan->DeleteSubPath(cSets + m_CurrentSet + _T('/'));
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
#if wxCHECK_VERSION(2, 9, 0)
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

    wxString path(cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));

    wxArrayString knownMembers = m_CfgMan->EnumerateKeys(path);

    for (unsigned int i = 0; i < builtinMembers.GetCount(); ++i)
    {
        ((wxTextCtrl*) FindWindow(builtinMembers[i]))->SetValue(m_CfgMan->Read(path + builtinMembers[i]));

        int index = knownMembers.Index(builtinMembers[i], false);
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
    wxString path(cSets + m_CurrentSet + _T('/') + m_CurrentVar + _T('/'));
    wxString mbr(_T('#') + m_CurrentVar + _T('.'));

    m_CfgMan->DeleteSubPath(path);

    wxString s, t;
    for (unsigned int i = 0; i < builtinMembers.GetCount(); ++i)
    {
        t = ((wxTextCtrl*) FindWindow(builtinMembers[i]))->GetValue();

        if (   i == 0
            && (   (   !m_CurrentVar.IsEmpty()
                    && t.IsEmpty() )
                || t.Contains(_T('#') + m_CurrentVar) ) )
        {
            if (cbMessageBox(_("Are you sure you want to save an invalid global variable?"), _("Global variables"),
                             wxYES_NO | wxICON_QUESTION) == wxID_YES)
            {
                t.assign(_T("(invalid)"));
            }
        }

        if (t.Contains(mbr + builtinMembers[i]))
            t.assign(_T("(invalid)"));

        if (!t.IsEmpty())
            m_CfgMan->Write(path + builtinMembers[i], t);
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

    wxArrayString sets = m_CfgMan->EnumerateSubPaths(cSets);
    wxArrayString vars = m_CfgMan->EnumerateSubPaths(cSets + m_CurrentSet + _T("/"));
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
