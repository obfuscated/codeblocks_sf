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
    #include "cbworkspace.h"
    #include "globals.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "workspaceloader.h"

    #include <wx/intl.h>
#endif

#include <wx/filedlg.h>
#include "filefilters.h"

cbWorkspace::cbWorkspace(const wxString& filename) :
    m_IsOK(true),
    m_IsDefault(true),
    m_Modified(false),
    m_Filename(DEFAULT_WORKSPACE),
    m_Title(_("Default workspace")),
    m_PreferredTargetName()
{
    //ctor
    if ( filename.Matches(DEFAULT_WORKSPACE) || filename.IsEmpty() )
    {
        // if no filename given, use the default workspace
        wxString tmp = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH;

        if (!wxDirExists(tmp))
            wxMkdir(tmp, 0755);

        tmp << wxFILE_SEP_PATH << DEFAULT_WORKSPACE;
        m_Filename = tmp;
    }
    else
    {
        m_Filename = filename;
        m_IsDefault = false;
    }

    if ( !filename.IsEmpty() )
    {
        Load();
    }
}

cbWorkspace::~cbWorkspace()
{
}

void cbWorkspace::Load()
{
    wxString fname = m_Filename.GetFullPath();
    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Loading workspace \"%s\""), fname.wx_str()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Loading workspace \"%s\""), fname.c_str()));
    #endif

    if (!m_Filename.FileExists())
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("File does not exist."));
        if (!m_IsDefault)
        {
            wxString msg;
            msg.Printf(_("Workspace '%s' does not exist..."), fname.c_str());
            cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
            // workspace wasn't loaded successfully
            m_IsOK = false;
            return;
        }
    }

    if (FileTypeOf(fname) == ftCodeBlocksWorkspace)
    {
        IBaseWorkspaceLoader* pWsp = new WorkspaceLoader;

        wxString Title;
        m_IsOK = pWsp && (pWsp->Open(fname, Title) || m_IsDefault);
        if(!Title.IsEmpty())
        {
            m_Title = Title;
        }

        delete pWsp;
    }

    m_Filename.SetExt(FileFilters::WORKSPACE_EXT);
    SetModified(false);
}

bool cbWorkspace::Save(bool force)
{
    if (m_Filename.GetFullPath().IsEmpty())
        return SaveAs(_T(""));

    // always save the layout file
    SaveLayout();

    if (!force && !m_Modified)
        return true;

    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Saving workspace \"%s\""), m_Filename.GetFullPath().wx_str()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Saving workspace \"%s\""), m_Filename.GetFullPath().c_str()));
    #endif
    WorkspaceLoader wsp;
    bool ret = wsp.Save(m_Title, m_Filename.GetFullPath());
    SetModified(!ret);
    if(!ret)
        cbMessageBox(_("Couldn't save workspace ") + m_Filename.GetFullPath() + _("\n(Maybe the file is write-protected?)"), _("Warning"), wxICON_WARNING);
    return ret;
}

bool cbWorkspace::SaveAs(const wxString& /*filename*/)
{
    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                     _("Save workspace"),
                     m_Filename.GetPath(),
                     m_Filename.GetFullName(),
                     FileFilters::GetFilterString(_T('.') + FileFilters::WORKSPACE_EXT),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return false;

    m_Filename = dlg.GetPath();
    if (m_Filename.GetExt() == wxEmptyString)
        m_Filename.SetExt(_T("workspace"));

    if (m_Filename.GetFullName().Matches(DEFAULT_WORKSPACE))
        m_IsDefault = true;
    else
        m_IsDefault = false;

    return Save(true);
}

void cbWorkspace::SetTitle(const wxString& title)
{
    m_Title = title;
    SetModified(true);
}

void cbWorkspace::SetModified(bool modified)
{
    m_Modified = modified;
    // Manager::Get()->GetLogManager()->DebugLog(F(_T("Setting workspace to modified = \"%s\""), modified ? _T("true") : _T("false")));
}

void cbWorkspace::PreferredTarget(const wxString &target)
{
    if ( ! target.IsEmpty() )
    {
        m_PreferredTargetName = target;
    }
}

wxString cbWorkspace::PreferredTarget() const
{
    return m_PreferredTargetName;
}

bool cbWorkspace::SaveLayout()
{
    LogManager *log = Manager::Get()->GetLogManager();
    WorkspaceLoader wsl;
    wxFileName fn(m_Filename);
    fn.SetExt( _T("workspace.layout") );
#if wxCHECK_VERSION(2, 9, 0)
    log->DebugLog(F(_T("Saving workspace layout \"%s\""), fn.GetFullPath().wx_str()));
#else
    log->DebugLog(F(_T("Saving workspace layout \"%s\""), fn.GetFullPath().c_str()));
#endif
    const bool rc = wsl.SaveLayout( fn.GetFullPath() );
    if (!rc)
    {
#if wxCHECK_VERSION(2, 9, 0)
        log->DebugLog(F(_T("Couldn't save workspace layout \"%s\""), fn.GetFullPath().wx_str()));
#else
        log->DebugLog(F(_T("Couldn't save workspace layout \"%s\""), fn.GetFullPath().c_str()));
#endif
    }
    return rc;
}

bool cbWorkspace::LoadLayout()
{
    LogManager *log = Manager::Get()->GetLogManager();
    WorkspaceLoader wsl;
    wxFileName fn(m_Filename);
    fn.SetExt( _T("workspace.layout") );
    bool rc = false;
    if ( fn.FileExists() )
    {
#if wxCHECK_VERSION(2, 9, 0)
        log->DebugLog(F(_T("Loading workspace layout \"%s\""), fn.GetFullPath().wx_str()));
#else
        log->DebugLog(F(_T("Loading workspace layout \"%s\""), fn.GetFullPath().c_str()));
#endif
        rc = wsl.LoadLayout( fn.GetFullPath() );
        if (!rc)
        {
#if wxCHECK_VERSION(2, 9, 0)
            log->DebugLog(F(_T("Couldn't load workspace layout \"%s\""), fn.GetFullPath().wx_str()));
#else
            log->DebugLog(F(_T("Couldn't load workspace layout \"%s\""), fn.GetFullPath().c_str()));
#endif
        }
    }
    else
    {
#if wxCHECK_VERSION(2, 9, 0)
        log->DebugLog(F(_T("Workspace layout file doesn't exist \"%s\""), fn.GetFullPath().wx_str()));
#else
        log->DebugLog(F(_T("Workspace layout file doesn't exist \"%s\""), fn.GetFullPath().c_str()));
#endif
    }
    return rc;
}
