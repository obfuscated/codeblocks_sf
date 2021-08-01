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
    #include <wx/textdlg.h>
    #include <wx/splitter.h>
    #include <wx/choice.h>
    #include <wx/listbox.h>
#endif

#include "annoyingdialog.h"

#if wxCHECK_VERSION(3, 0, 0)
#include <wx/unichar.h>
#endif

#include <ctype.h>

template<> UserVariableManager* Mgr<UserVariableManager>::instance   = nullptr;
template<> bool                 Mgr<UserVariableManager>::isShutdown = false;

const wxString UserVariableManager::cBase     (_T("base"));
const wxString UserVariableManager::cDir      (_T("dir"));
const wxChar   UserVariableManager::cSlash(_T ('/'));
const wxString UserVariableManager::cSlashBase(_T("/base"));
const wxString UserVariableManager::cInclude  (_T("include"));
const wxString UserVariableManager::cLib      (_T("lib"));
const wxString UserVariableManager::cObj      (_T("obj"));
const wxString UserVariableManager::cBin      (_T("bin"));
const wxString UserVariableManager::cCflags   (_T("cflags"));
const wxString UserVariableManager::cLflags   (_T("lflags"));
const wxString UserVariableManager::cSets     (_T("/sets/"));

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
const wxArrayString UserVariableManager::builtinMembers((size_t) 7, bim);

class UserVarManagerNoGuiUI : public UserVarManagerUI
{
public:
    ~UserVarManagerNoGuiUI()    {};

    void DisplayInfoWindow(const wxString &title,const wxString &msg) override
    {
        Manager::Get()->GetLogManager()->LogWarning(msg);
    }

    void OpenEditWindow(const wxArrayString &var) override { };
    wxString GetVariable(wxWindow* parent, const wxString &old) override { return ""; };
};

void UserVariableManager::SetUI(std::unique_ptr<UserVarManagerUI> ui)
{
    m_ui = std::move(ui);
}


void UserVariableManager::Configure()
{
    m_ui->OpenEditWindow();
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

            m_ui->DisplayInfoWindow(_("Global Compiler Variables"), msg);
            wxArrayString ar;
            ar.Add(package);
            m_ui->OpenEditWindow(ar);

            base = m_CfgMan->Read(path + cBase);
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
        m_ui->DisplayInfoWindow(_("Global Compiler Variables"), msg);
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
    return m_CfgMan->Exists(cSets + m_ActiveSet + _T('/') + member + _T("/base"));
}

void UserVariableManager::Arrogate()
{
    if (m_Preempted.GetCount() == 0)
        return;

    wxString peList;
    for (unsigned int i = 0; i < m_Preempted.GetCount(); ++i)
    {
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

    m_ui->DisplayInfoWindow(_("Global Compiler Variables"), msg);
    m_ui->OpenEditWindow(m_Preempted);
    m_Preempted.Clear();
}

UserVariableManager::UserVariableManager()
{
    m_CfgMan = Manager::Get()->GetConfigManager(_T("gcv"));
    m_ui = std::unique_ptr<UserVarManagerUI>(new UserVarManagerNoGuiUI());
    Migrate();
}

UserVariableManager::~UserVariableManager()
{
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

wxString UserVariableManager::GetVariable(wxWindow *parent, const wxString &old)
{
    return m_ui->GetVariable(parent, old);
}
