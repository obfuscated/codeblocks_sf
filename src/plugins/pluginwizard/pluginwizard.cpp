/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/
#ifdef CB_PRECOMP
#include "sdk.h"
#else
#include <wx/intl.h>
#include <wx/fs_zip.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "configmanager.h"
#include "globals.h"
#include "manager.h"
#include "projectbuildtarget.h"
#include "projectmanager.h"
#endif
#include "infowindow.h"

#include <wx/filefn.h>
#include <wx/filesys.h>
#include "filefilters.h"
#include "pluginwizard.h"
#include "pluginwizarddlg.h"

CB_IMPLEMENT_PLUGIN(PluginWizard, "Plugin wizard");


PluginWizard::PluginWizard()
{
    if(!Manager::LoadResource(_T("plugin_wizard.zip")))
    {
        NotifyMissingFile(_T("plugin_wizard.zip"));
    }

    m_PluginInfo.name = _T("PluginWizard");
    m_PluginInfo.title = _("Plugin wizard");
    m_PluginInfo.version = _T("0.1");
    m_PluginInfo.description = _("This is the Code::Blocks plugin wizard...");
    m_PluginInfo.author = _T("Yiannis An. Mandravellos");
    m_PluginInfo.authorEmail = _T("info@codeblocks.org");
    m_PluginInfo.authorWebsite = _T("www.codeblocks.org");
    m_PluginInfo.thanksTo = _T("");
}

PluginWizard::~PluginWizard()
{
}

void PluginWizard::OnAttach()
{
}

void PluginWizard::OnRelease(bool appShutDown)
{
}

int PluginWizard::Execute()
{
    PluginWizardDlg dlg;
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return -1;

    cbProject* project = Manager::Get()->GetProjectManager()->NewProject();
    if (!project)
        return -1;

    // add compiler options
    project->AddCompilerOption(_T("-pipe"));
    project->AddCompilerOption(_T("-fmessage-length=0"));
    project->AddCompilerOption(_T("-fexceptions"));
    project->AddCompilerOption(_T("-Winvalid-pch"));
    project->AddCompilerOption(_T("-DcbDEBUG"));
    project->AddCompilerOption(_T("-DCB_PRECOMP"));
#ifdef __WXMSW__
    project->AddCompilerOption(_T("-mthreads"));
    project->AddCompilerOption(_T("-DHAVE_W32API_H"));
    project->AddCompilerOption(_T("-D__WXMSW__"));
    project->AddCompilerOption(_T("-DWXUSINGDLL"));
    project->AddCompilerOption(_T("-DTIXML_USE_STL"));
    project->AddCompilerOption(_T("-DWX_PRECOMP"));
    project->AddCompilerOption(_T("-DwxUSE_UNICODE"));
    project->AddCompilerOption(_T("-DBUILDING_PLUGIN"));
    // wx & cb dirs
    project->AddIncludeDir(_T("$(#WX.include)"));
    project->AddIncludeDir(_T("$(#WX.lib)\\gcc_dll\\msw$(WX_SUFFIX)"));
    project->AddIncludeDir(_T("$(#WX)\\contrib\\include"));
    project->AddIncludeDir(_T("$(#CB.include)")); // SDK installation
    project->AddIncludeDir(_T("$(#CB.include)\\tinyxml")); // SDK installation
    project->AddIncludeDir(_T("$(#CB.include)\\wxscintilla\\include")); // SDK installation
    project->AddIncludeDir(_T("$(#CB)\\sdk")); // source tree
    project->AddIncludeDir(_T("$(#CB)\\sdk\\tinyxml")); // source tree
    project->AddIncludeDir(_T("$(#CB)\\sdk\\wxscintilla\\include")); // source tree
    // resource dirs
    project->AddResourceIncludeDir(_T("$(#WX.include)"));
#else
    project->AddCompilerOption(_T("-fPIC"));
    project->AddCompilerOption(_T("`wx-config --cflags`"));
#endif

#ifdef __WXMSW__
    project->AddLinkLib(_T("wxmsw26$(WX_SUFFIX)"));
    // wx & cb dirs
    project->AddLibDir(_T("$(#WX.lib)\\gcc_dll"));
    project->AddLibDir(_T("$(#CB.lib)")); // SDK installation
    project->AddLibDir(_T("$(#CB)\\devel")); // source tree
    project->AddLibDir(_T("$(#CB)\\sdk\\tinyxml")); // source tree
#else
    project->AddLinkerOption(_T("`wx-config --libs`"));
#endif

#ifdef __WXMSW__
    // now create the necessary env. vars
    project->SetVar(_T("WX_SUFFIX"), _T("u"));
#endif

    // cross-platform options
    project->AddCompilerOption(_T("-Wall")); // all warnings on
    project->AddCompilerOption(_T("-g")); // debugging symbols
    project->AddLinkLib(_T("codeblocks"));

    wxSetWorkingDirectory(project->GetBasePath());
    dlg.CreateFiles();

    wxString name = !dlg.GetInfo().name.IsEmpty() ? dlg.GetInfo().name : _T("CustomPlugin");
    wxString title = !dlg.GetInfo().title.IsEmpty() ? dlg.GetInfo().title : name;
    project->SetTitle(title);
    project->AddFile(0, dlg.GetHeaderFilename());
    project->AddFile(0, dlg.GetImplementationFilename());

    ProjectBuildTarget* target = project->GetBuildTarget(0);
    target->SetCompilerID(_T("gcc")); // force GCC; it's our supported compiler
    target->SetIncludeInTargetAll(true);
    target->SetTargetType(ttDynamicLib);
    target->SetCreateDefFile(false);
    target->SetCreateStaticLib(false);
    target->SetOutputFilename(name + _T(".") + FileFilters::DYNAMICLIB_EXT);

    Manager::Get()->GetProjectManager()->RebuildTree();

#ifdef __WXMSW__
        InfoWindow::Display(_("Plugin Wizard"), _("The new plugin project has been created.\n"
                    "You should now update the project's custom variables\nto adapt "
                    "to your environment.") , 10000);
#endif
    return 0;
}
