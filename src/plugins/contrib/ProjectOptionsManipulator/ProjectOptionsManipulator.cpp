/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "ProjectOptionsManipulator.h"

#include <cbproject.h>
#include <globals.h> // cbMessageBox
#include <manager.h>
#include <projectmanager.h>

#include "ProjectOptionsManipulatorDlg.h"
#include "ProjectOptionsManipulatorResultDlg.h"

// Register the plugin
namespace
{
  PluginRegistrant<ProjectOptionsManipulator> reg(_T("ProjectOptionsManipulator"));
  long ID_PROJECT_OPTIONS_DLG        = wxNewId();
  long ID_PROJECT_OPTIONS_RESULT_DLG = wxNewId();
};

// Identifiers for settings dialog
BEGIN_EVENT_TABLE(ProjectOptionsManipulator, cbPlugin)
END_EVENT_TABLE()

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

ProjectOptionsManipulator::ProjectOptionsManipulator() :
  m_Dlg(NULL)
{
  //ctor
}// ProjectOptionsManipulator

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

ProjectOptionsManipulator::~ProjectOptionsManipulator()
{
  //dtor
}//~ProjectOptionsManipulator

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::OnAttach()
{
  // do whatever initialization you need for your plugin
  // NOTE: after this function, the inherited member variable
  // m_IsAttached will be TRUE...
  // You should check for it in other functions, because if it
  // is FALSE, it means that the application did *not* "load"
  // (see: does not need) this plugin...

  m_Dlg = new ProjectOptionsManipulatorDlg( Manager::Get()->GetAppWindow(), ID_PROJECT_OPTIONS_DLG );
}// OnAttach

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::OnRelease(bool WXUNUSED(appShutDown))
{
  // do de-initialization for your plugin
  // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
  // which means you must not use any of the SDK Managers
  // NOTE: after this function, the inherited member variable
  // m_IsAttached will be FALSE...

  if (m_Dlg)
    m_Dlg->Destroy();
}// OnRelease

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int ProjectOptionsManipulator::Execute()
{
  if ( !IsAttached() || !m_Dlg )
    return -1;

  wxArrayString result;
  if ( m_Dlg->ShowModal()==wxID_OK )
  {
    if      ( m_Dlg->GetScanForWorkspace() )
    {
      if ( !OperateWorkspace(result) )
      {
        cbMessageBox(_("Processing options for workspace failed!"), _("Error"),
                     wxICON_ERROR, Manager::Get()->GetAppWindow());
        return -1;
      }
    }
    else if ( m_Dlg->GetScanForProject()   )
    {
      if ( !OperateProject( m_Dlg->GetProjectIdx(), result ) )
      {
        cbMessageBox(_("Processing options for project failed!"), _("Error"),
                     wxICON_ERROR, Manager::Get()->GetAppWindow());
        return -1;
      }
    }
  }

  if ( !result.IsEmpty() )
  {
    ProjectOptionsManipulatorResultDlg dlg( Manager::Get()->GetAppWindow(),ID_PROJECT_OPTIONS_RESULT_DLG );
    dlg.ApplyResult(result);
    dlg.ShowModal();
  }

  return 0;
}// Execute

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

bool ProjectOptionsManipulator::OperateWorkspace(wxArrayString& result)
{
  ProjectsArray* pa = Manager::Get()->GetProjectManager()->GetProjects();
  bool success = true;
  if (pa)
  {
    for (size_t i=0; i<pa->GetCount(); ++i)
      success &= OperateProject( pa->Item(i), result );
  }

  return success;
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

bool ProjectOptionsManipulator::OperateProject(size_t prj_idx, wxArrayString& result)
{
  ProjectsArray* pa = Manager::Get()->GetProjectManager()->GetProjects();
  bool success = true;
  if (pa)
    success &= OperateProject( pa->Item(prj_idx), result );

  return success;
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

bool ProjectOptionsManipulator::OperateProject(cbProject* prj, wxArrayString& result)
{
  if (!prj) return false;

  const wxString opt = m_Dlg->GetOption();
  const wxString val = m_Dlg->GetValue();

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eCompiler) )
    ProcessCompilerOptions(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eLinker) )
    ProcessLinkerOptions(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eCompilerPaths) )
    ProcessCompilerPaths(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eLinkerPaths) )
    ProcessLinkerPaths(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eResCompPaths) )
    ProcessResCompPaths(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eLinkerLibs) )
    ProcessLinkerLibs(prj, opt, result);

  if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eCustomVars) )
    ProcessCustomVars(prj, opt, val, result);

  return true;
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessCompilerOptions(cbProject* prj, const wxString& opt, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetCompilerOptions(), opt);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains compiler option '%s'."),
                                      prj->GetTitle().wx_str(), opt.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain compiler option '%s'."),
                                      prj->GetTitle().wx_str(), opt.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetCompilerOptions(), opt);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains compiler option '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), opt.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain compiler option '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), opt.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveCompilerOption(opt);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveCompilerOption(opt);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddCompilerOption(opt);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddCompilerOption(opt);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessLinkerOptions(cbProject* prj, const wxString& opt, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetLinkerOptions(), opt);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains linker option '%s'."),
                                      prj->GetTitle().wx_str(), opt.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain linker option '%s'."),
                                      prj->GetTitle().wx_str(), opt.wx_str()));
        }

      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetLinkerOptions(), opt);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains linker option '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), opt.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain linker option '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), opt.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveLinkerOption(opt);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveLinkerOption(opt);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddLinkerOption(opt);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddLinkerOption(opt);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessCompilerPaths(cbProject* prj, const wxString& path, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetIncludeDirs(), path);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains compiler path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain compiler path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetIncludeDirs(), path);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains compiler path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain compiler path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveIncludeDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveIncludeDir(path);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddIncludeDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddIncludeDir(path);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessLinkerPaths(cbProject* prj, const wxString& path, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetLibDirs(), path);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains linker path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain linker path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetLibDirs(), path);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains linker path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain linker path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveLibDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveLibDir(path);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddLibDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddLibDir(path);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessResCompPaths(cbProject* prj, const wxString& path, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetResourceIncludeDirs(), path);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains resource compiler path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain resource compiler path '%s'."),
                                      prj->GetTitle().wx_str(), path.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetResourceIncludeDirs(), path);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains resource compiler path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain resource compiler path '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), path.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveResourceIncludeDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveResourceIncludeDir(path);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddResourceIncludeDir(path);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddResourceIncludeDir(path);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessLinkerLibs(cbProject* prj, const wxString& lib, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_opt = SearchOption(prj->GetLinkLibs(), lib);
        if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Contains linker lib '%s'."),
                                      prj->GetTitle().wx_str(), lib.wx_str()));
        }
        else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not contain linker lib '%s'."),
                                      prj->GetTitle().wx_str(), lib.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_opt = SearchOption(tgt->GetLinkLibs(), lib);
            if (has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Contains linker lib '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), lib.wx_str()));
            }
            else if (!has_opt && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not contain linker lib '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), lib.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->RemoveLinkLib(lib);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->RemoveLinkLib(lib);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->AddLinkLib(lib);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->AddLinkLib(lib);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void ProjectOptionsManipulator::ProcessCustomVars(cbProject* prj, const wxString& var, const wxString& value, wxArrayString& result)
{
  ProjectOptionsManipulatorDlg::EProjectScanOption scan_opt = m_Dlg->GetScanOption();
  switch (scan_opt)
  {
    case ProjectOptionsManipulatorDlg::eSearch:
    case ProjectOptionsManipulatorDlg::eSearchNot:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
      {
        bool has_var = prj->HasVar(var);
        if (has_var && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
        {
          result.Add(wxString::Format(_("Project '%s': Does not define custom var '%s'."),
                                      prj->GetTitle().wx_str(), var.wx_str()));
        }
        else if (has_var && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
        {
          result.Add(wxString::Format(_("Project '%s': Defines custom var '%s'."),
                                      prj->GetTitle().wx_str(), var.wx_str()));
        }
      }

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt)
          {
            bool has_var = prj->HasVar(var);
            if (has_var && scan_opt==ProjectOptionsManipulatorDlg::eSearchNot)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Does not define custom var '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), var.wx_str()));
            }
            else if (has_var && scan_opt==ProjectOptionsManipulatorDlg::eSearch)
            {
              result.Add(wxString::Format(_("Project '%s', target '%s': Defines custom var '%s'."),
                                          prj->GetTitle().wx_str(), tgt->GetTitle().wx_str(), var.wx_str()));
            }
          }
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eRemove:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->UnsetVar(var);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->UnsetVar(var);
        }
      }
    }
    break;

    case ProjectOptionsManipulatorDlg::eAdd:
    {
      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eProject) )
        prj->SetVar(var, value);

      if ( m_Dlg->GetOptionActive(ProjectOptionsManipulatorDlg::eTarget) )
      {
        for (int i=0; i<prj->GetBuildTargetsCount(); ++i)
        {
          ProjectBuildTarget* tgt = prj->GetBuildTarget(i);
          if (tgt) tgt->SetVar(var, value);
        }
      }
    }
    break;

    default:
    break;
  }
}

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

bool ProjectOptionsManipulator::SearchOption(const wxArrayString& opt_array, const wxString& opt)
{
  switch ( m_Dlg->GetSearchOption() )
  {
    case (ProjectOptionsManipulatorDlg::eContains):
    {
      for (size_t i=0; i<opt_array.Count(); ++i)
      {
        if ( opt_array.Item(i).Contains(opt) )
          return true;
      }
    }
    break;

    case (ProjectOptionsManipulatorDlg::eEquals): // fall through
    default:
    {
      if ( opt_array.Index(opt)!=wxNOT_FOUND)
        return true;
    }
    break;
  }

  return false;
}
