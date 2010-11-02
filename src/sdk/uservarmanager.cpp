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
#endif

#include "annoyingdialog.h"
#include <wx/choice.h>
#include <wx/textdlg.h> //wxTextEntryDialog

#if wxCHECK_VERSION(2, 9, 0)
#include <wx/unichar.h>
#endif

#include <ctype.h>

template<> UserVariableManager* Mgr<UserVariableManager>::instance   = 0;
template<> bool                 Mgr<UserVariableManager>::isShutdown = false;

const wxString cBase     (_T("base"));
const wxString cDir      (_T("dir"));
const wxChar   cSlash(_T ('/'));
const wxString cSlashBase(_T("/base"));
const wxString cInclude  (_T("include"));
const wxString cLib      (_T("lib"));
const wxString cObj      (_T("obj"));
const wxString cCflags   (_T("cflags"));
const wxString cLflags   (_T("lflags"));
const wxString cSets     (_T("/sets/"));

const wxChar *bim[] =
{
    _T("base"),
    _T("include"),
    _T("lib"),
    _T("obj"),
    _T("cflags"),
    _T("lflags")
};
const wxArrayString builtinMembers((size_t) 6, bim);

class UsrGlblMgrEditDialog : public wxScrollingDialog
{
    wxString currentSet;
    wxString currentVar;

    wxChoice *selSet;
    wxChoice *selVar;
    wxButton *deleteSet;

    wxTextCtrl *base;
    wxTextCtrl *include;
    wxTextCtrl *lib;
    wxTextCtrl *obj;

    wxTextCtrl *name[7];
    wxTextCtrl *value[7];

    ConfigManager *cfg;

    void Help(wxCommandEvent& event);
    void DoClose();
    void OnOK(wxCommandEvent& /*event*/){DoClose();};
    void OnCancel(wxCommandEvent& /*event*/){DoClose();};
    void CloseHandler(wxCloseEvent& /*event*/){DoClose();};

    void CloneVar(wxCommandEvent& event);
    void CloneSet(wxCommandEvent& event);
    void NewVar(wxCommandEvent& event);
    void NewSet(wxCommandEvent& event);
    void DeleteVar(wxCommandEvent& event);
    void DeleteSet(wxCommandEvent& event);

    void OnFS(wxCommandEvent& event);

    void SelectSet(wxCommandEvent& event);
    void SelectVar(wxCommandEvent& s);

    void Load();
    void Save();
    void UpdateChoices();
    void AddVar(const wxString& var);

    void Sanitise(wxString& s)
    {
    s.Trim().Trim(true);

    if(s.IsEmpty())
        {
        s = _T("[?empty?]");
        return;
        }

    for(unsigned int i = 0; i < s.length(); ++i)
    #if wxCHECK_VERSION(2, 9, 0)
        s[i] = isalnum(s.GetChar(i)) ? s.GetChar(i) : wxUniChar('_');
    #else
        s[i] = isalnum(s.GetChar(i)) ? s.GetChar(i) : _T('_');
    #endif

    if(s.GetChar(0) == _T('_'))
            s.Prepend(_T("set"));

    if(s.GetChar(0) >= _T('0') && s.GetChar(0) <= _T('9'))
            s.Prepend(_T("set_"));

    };

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
    activeSet = Manager::Get()->GetConfigManager(_T("gcv"))->Read(_T("/active"));
}


wxString UserVariableManager::Replace(const wxString& variable)
{
    wxString package = variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).MakeLower();
    wxString member  = variable.AfterFirst(wxT('.')).MakeLower();

    wxString path(cSets + activeSet + _T('/') + package + _T('/'));

    wxString base = cfg->Read(path + cBase);

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
            msg.Printf(_("In the currently active Set, Code::Blocks does not know\nthe global compiler variable \"%s\".\n\nPlease define it."), package.c_str());
            InfoWindow::Display(_("Global Compiler Variables"), msg , 8000, 1000);
            UsrGlblMgrEditDialog d;
            d.AddVar(package);
            PlaceWindow(&d);
            d.ShowModal();
        }
    }

    if (member.IsEmpty() || member.IsSameAs(cBase))
        return base;

    if (member.IsSameAs(cInclude) || member.IsSameAs(cLib) || member.IsSameAs(cObj))
    {
        wxString ret = cfg->Read(path + member);
        if (ret.IsEmpty())
            ret = base + _T('/') + member;
        return ret;
    }
    else
    {
        return cfg->Read(path + member);
    }
}


void UserVariableManager::Preempt(const wxString& variable)
{
    if (variable.find(_T('#')) == wxString::npos)
        return;

    wxString member(variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).BeforeFirst(wxT(')')).MakeLower());

    if (!cfg->Exists(cSets + activeSet + _T('/') + member + _T("/base")) &&
            preempted.Index(member) == wxNOT_FOUND)
    {
        preempted.Add(member);
    }
}

bool UserVariableManager::Exists(const wxString& variable) const
{
    if (variable.find(_T('#')) == wxString::npos)
        return false;

    wxString member(variable.AfterLast(wxT('#')).BeforeFirst(wxT('.')).BeforeFirst(wxT(')')).MakeLower());
    return !cfg->Exists(cSets + activeSet + _T('/') + member + _T("/base"));
}

void UserVariableManager::Arrogate()
{
    if (preempted.GetCount() == 0)
        return;

    wxString peList;

    UsrGlblMgrEditDialog d;

    for (unsigned int i = 0; i < preempted.GetCount(); ++i)
    {
        d.AddVar(preempted[i]);
        peList << preempted[i] << _T('\n');
    }
    peList = peList.BeforeLast('\n'); // remove trailing newline

    wxString msg;
    if (preempted.GetCount() == 1)
        msg.Printf(_("In the currently active Set, Code::Blocks does not know\nthe global compiler variable \"%s\".\n\nPlease define it."), peList.c_str());
    else
        msg.Printf(_("In the currently active Set, Code::Blocks does not know\nthe following global compiler variables:\n%s\n\nPlease define them."), peList.c_str());

    PlaceWindow(&d);
    preempted.Clear();
    InfoWindow::Display(_("Global Compiler Variables"), msg , 8000 + 800*preempted.GetCount(), 100);

    d.ShowModal();
}

UserVariableManager::UserVariableManager()
{
    cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    Migrate();
}

void UserVariableManager::Migrate()
{
    ConfigManager *current = Manager::Get()->GetConfigManager(_T("gcv"));

    activeSet = current->Read(_T("/active"));

    if (!activeSet.IsEmpty())
        return;

    activeSet = _T("default");
    current->Exists(_T("/sets/default/foo")); // assert /sets/default
    current->Write(_T("/active"), activeSet);
    wxString oldpath;
    wxString newpath;

    ConfigManager *old = Manager::Get()->GetConfigManager(_T("global_uservars"));
    wxArrayString vars = old->EnumerateSubPaths(_T("/"));

    for (unsigned int i = 0; i < vars.GetCount(); ++i)
    {
        vars[i].Prepend(_T('/'));
        wxArrayString members = old->EnumerateKeys(vars[i]);

        for (unsigned j = 0; j < members.GetCount(); ++j)
        {
            oldpath.assign(vars[i] + _T("/") + members[j]);
            newpath.assign(_T("/sets/default") + vars[i] + _T("/") + members[j]);

            current->Write(newpath, old->Read(oldpath));
        }
    }
    old->Delete();
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

    EVT_CHOICE(XRCID("selSet"), UsrGlblMgrEditDialog::SelectSet)
    EVT_CHOICE(XRCID("selVar"), UsrGlblMgrEditDialog::SelectVar)
END_EVENT_TABLE()

UsrGlblMgrEditDialog::UsrGlblMgrEditDialog(const wxString& var) : currentSet(Manager::Get()->GetConfigManager(_T("gcv"))->Read(_T("/active"))), currentVar(var)
{
    wxXmlResource::Get()->LoadObject(this, Manager::Get()->GetAppWindow(), _T("dlgGloabalUservars"),_T("wxScrollingDialog"));
    selSet = XRCCTRL(*this, "selSet", wxChoice);
    selVar = XRCCTRL(*this, "selVar", wxChoice);
    deleteSet = XRCCTRL(*this, "deleteSet", wxButton);

    base = XRCCTRL(*this, "base", wxTextCtrl);
    include = XRCCTRL(*this, "include", wxTextCtrl);
    lib = XRCCTRL(*this, "lib", wxTextCtrl);
    obj = XRCCTRL(*this, "obj", wxTextCtrl);

    wxString n;
    for (unsigned int i = 0; i < 7; ++i)
    {
        n.Printf(_T("n%d"), i);
        name[i] = (wxTextCtrl*) FindWindow(n);

        n.Printf(_T("v%d"), i);
        value[i] = (wxTextCtrl*) FindWindow(n);
    }

    cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    cfg->Exists(_T("/sets/default/foo"));
#ifdef cbDEBUG_EXTRA
    assert(selSet);
    assert(selVar);
    assert(deleteSet);

    assert(base);
    assert(include);
    assert(lib);
    assert(obj);

    for (unsigned int i = 0; i < 7; ++i)
    {
        assert(name[i]);
        assert(value[i]);
    }
#endif
    UpdateChoices();
    Load();
    PlaceWindow(this);
}

void UsrGlblMgrEditDialog::DoClose()
{
    Save();
    EndModal(wxID_OK);
}


void UsrGlblMgrEditDialog::CloneVar(wxCommandEvent& /*event*/)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new clone:"), _("Clone Variable"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString clone = d.GetValue();

        if (clone.IsEmpty())
            return;

        Sanitise(clone);

        wxString srcPath(_T("/sets/") + currentSet + _T('/') + currentVar + _T('/'));
        wxString dstPath(_T("/sets/") + currentSet + _T('/') + clone + _T('/'));

        wxArrayString existing = cfg->EnumerateSubPaths(_T("/sets/" + currentSet));

        if (existing.Index(clone) != wxNOT_FOUND)
        {
            wxString msg;
            msg.Printf(_("Cowardly refusing to overwrite existing variable \"%s\"."), clone.c_str());
            InfoWindow::Display(_("Clone Set"), msg);
            return;
        }

        wxArrayString members = cfg->EnumerateKeys(srcPath);

        for (unsigned j = 0; j < members.GetCount(); ++j)
            cfg->Write(dstPath + members[j], cfg->Read(srcPath + members[j]));

        currentVar = clone;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::CloneSet(wxCommandEvent& /*event*/)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new clone:"), _("Clone Set"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString clone = d.GetValue();
        Sanitise(clone);

        if (clone.IsEmpty())
            return;
        wxArrayString existing = cfg->EnumerateSubPaths(_T("/sets"));

        if (existing.Index(clone) != wxNOT_FOUND)
        {
            wxString msg;
            msg.Printf(_("Cowardly refusing overwrite existing set \"%s\"."), clone.c_str());
            InfoWindow::Display(_("Clone Set"), msg);
            return;
        }

        wxString srcPath(cSets + currentSet + _T("/"));
        wxString dstPath(cSets + clone + _T("/"));
        wxString oldpath, newpath;

        wxArrayString vars = cfg->EnumerateSubPaths(srcPath);

        for (unsigned int i = 0; i < vars.GetCount(); ++i)
        {
            wxArrayString members = cfg->EnumerateKeys(srcPath + vars[i]);

            for (unsigned j = 0; j < members.GetCount(); ++j)
            {
                wxString item = vars[i] + _T("/") + members[j];
                cfg->Write(dstPath + item, cfg->Read(srcPath + item));
            }
        }
        currentSet = clone;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::DeleteVar(wxCommandEvent& /*event*/)
{
    wxString msg;
    msg.Printf(_("Delete the global compiler variable \"%s\" from this set?"), currentVar.c_str());
    AnnoyingDialog d(_("Delete Global Variable"), msg, wxART_QUESTION);
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_YES)
    {
        cfg->DeleteSubPath(cSets + currentSet + _T('/') + currentVar + _T('/'));
        currentVar = wxEmptyString;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::DeleteSet(wxCommandEvent& /*event*/)
{
    wxString msg;
    msg.Printf(_("Do you really want to delete\nthe entire global compiler variable set \"%s\"?\n\nThis cannot be undone."), currentSet.c_str());
    AnnoyingDialog d(_("Delete Global Variable Set"), msg, wxART_QUESTION);
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_YES)
    {
        cfg->DeleteSubPath(cSets + currentSet + _T('/'));
        currentSet = wxEmptyString;
        currentVar = wxEmptyString;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::AddVar(const wxString& name)
{
    if (name.IsEmpty())
        return;
    currentVar = name;
    cfg->Exists(_T("/sets/") + currentSet + _T('/') + name + _T('/'));

    currentVar = name;
    UpdateChoices();
    Load();
}

void UsrGlblMgrEditDialog::NewVar(wxCommandEvent& /*event*/)
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

void UsrGlblMgrEditDialog::NewSet(wxCommandEvent& /*event*/)
{
    wxTextEntryDialog d(this, _("Please specify a name for the new set:"), _("New Set"));
    PlaceWindow(&d);
    if (d.ShowModal() == wxID_OK)
    {
        wxString name = d.GetValue();
        Sanitise(name);

        if (name.IsEmpty())
            return;
        currentSet = name;
        cfg->Exists(_T("/sets/") + name + _T('/'));

        currentSet = name;
        UpdateChoices();
        Load();
    }
}

void UsrGlblMgrEditDialog::SelectVar(wxCommandEvent& /*s*/)
{
    Save();
    currentVar = selVar->GetStringSelection();
    Load();
}

void UsrGlblMgrEditDialog::SelectSet(wxCommandEvent& /*event*/)
{
    Save();
    currentSet = selSet->GetStringSelection();
    cfg->Write(_T("/active"), currentSet);
    UpdateChoices();
    Load();
}


void UsrGlblMgrEditDialog::Load()
{
    deleteSet->Enable(!currentSet.IsSameAs(_T("default")));

    wxString path(cSets + currentSet + _T('/') + currentVar + _T('/'));

    wxArrayString knownMembers = cfg->EnumerateKeys(path);

    for (unsigned int i = 0; i < builtinMembers.GetCount(); ++i)
    {
        ((wxTextCtrl*) FindWindow(builtinMembers[i]))->SetValue(cfg->Read(path + builtinMembers[i]));

        int index = knownMembers.Index(builtinMembers[i], false);
        if (index != wxNOT_FOUND)
            knownMembers.RemoveAt(index);
    }

    for (unsigned int i = 0; i < 7; ++i)
    {
        name[i]->SetValue(wxEmptyString);
        value[i]->SetValue(wxEmptyString);
    }

    for (unsigned int i = 0; i < knownMembers.GetCount(); ++i)
    {
        name[i]->SetValue(knownMembers[i].Lower());
        value[i]->SetValue(cfg->Read(path + knownMembers[i]));
    }
}

void UsrGlblMgrEditDialog::Save()
{
    wxString path(cSets + currentSet + _T('/') + currentVar + _T('/'));

    wxString mbr(_T('#') + currentVar + _T('.'));

    cfg->DeleteSubPath(path);

    wxString s, t;
    for (unsigned int i = 0; i < builtinMembers.GetCount(); ++i)
    {
        t = ((wxTextCtrl*) FindWindow(builtinMembers[i]))->GetValue();

        if (   i == 0
            && (   (   !currentVar.IsEmpty()
                    && t.IsEmpty() )
                || t.Contains(_T('#') + currentVar) ) )
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
            cfg->Write(path + builtinMembers[i], t);
    }

    for (unsigned int i = 0; i < 7; ++i)
    {
        s = name[i]->GetValue();
        t = value[i]->GetValue();

        if (t.Contains(mbr + s))
            t.assign(_T("(invalid)"));

        if (!s.IsEmpty() && !t.IsEmpty())
            cfg->Write(path + s, t);
    }
}

void UsrGlblMgrEditDialog::UpdateChoices()
{
    if (currentSet.IsEmpty())
        currentSet = _T("default");

    wxArrayString sets = cfg->EnumerateSubPaths(cSets);
    wxArrayString vars = cfg->EnumerateSubPaths(cSets + currentSet + _T("/"));

    selSet->Clear();
    selSet->Append(sets);
    selVar->Clear();
    selVar->Append(vars);

    if (currentVar.IsEmpty() && selVar->GetCount() > 0)
        currentVar = selVar->GetString(0);

    selSet->SetStringSelection(currentSet);
    selVar->SetStringSelection(currentVar);
}


void UsrGlblMgrEditDialog::OnFS(wxCommandEvent& event)
{
    wxTextCtrl* c = 0;
    int id = event.GetId();

    if      (id == XRCID("fs1"))
        c = base;
    else if (id == XRCID("fs2"))
        c = include;
    else if (id == XRCID("fs3"))
        c = lib;
    else if (id == XRCID("fs4"))
        c = obj;
    else
        cbThrow(_T("Encountered invalid button ID"));

    wxString path = ChooseDirectory(this, _("Choose a location"), c->GetValue());
    if (!path.IsEmpty())
        c->SetValue(path);
}

void UsrGlblMgrEditDialog::Help(wxCommandEvent& /*event*/)
{
    wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Global_compiler_variables"));
}
