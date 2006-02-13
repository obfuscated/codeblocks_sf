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

#include <sdk.h>
#include "pluginwizard.h"
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mdi.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <cbproject.h>
#include "pluginwizarddlg.h"
#include <globals.h>

CB_IMPLEMENT_PLUGIN(PluginWizard, "Code::Blocks Plugin wizard");


PluginWizard::PluginWizard()
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::GetDataFolder();
    wxXmlResource::Get()->Load(resPath + _T("/plugin_wizard.zip#zip:*.xrc"));

    m_PluginInfo.name = _T("PluginWizard");
    m_PluginInfo.title = _("Code::Blocks Plugin wizard");
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
#ifdef __WXMSW__
    project->AddCompilerOption(_T("-pipe"));
    project->AddCompilerOption(_T("-mthreads"));
    project->AddCompilerOption(_T("-fmessage-length=0"));
    project->AddCompilerOption(_T("-fexceptions"));
    project->AddCompilerOption(_T("-Winvalid-pch"));
    project->AddCompilerOption(_T("-D__GNUWIN32__"));
    project->AddCompilerOption(_T("-DWXUSINGDLL"));
    project->AddCompilerOption(_T("-DBUILDING_PLUGIN"));
    project->AddCompilerOption(_T("-D__WXMSW__"));
    project->AddCompilerOption(_T("-DHAVE_W32API_H"));
    // wx & cb dirs
    project->AddIncludeDir(_T("$(#WX.include)"));
    project->AddIncludeDir(_T("$(#WX.lib)\\gcc_dll\\msw"));
    project->AddIncludeDir(_T("$(#WX.lib)\\gcc_dll\\msw$(WX_CFG)\\msw"));
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
    project->AddCompilerOption(_T("`wx-config --cflags`"));
#endif

#ifdef __WXMSW__
// NOTE (rickg22#1#): How not to hardwire wxmsw242 into the plugin?
// NOTE (mandrav#1#): By making the version an environment variable...
    project->AddLinkLib(_T("wxmsw$(WX_VER)"));
    // wx & cb dirs
    project->AddLibDir(_T("$(#WX.lib)\\gcc_dll"));
    project->AddLibDir(_T("$(#WX.lib)\\gcc_dll$(WX_CFG)"));
    project->AddLibDir(_T("$(#CB.lib)")); // SDK installation
    project->AddLibDir(_T("$(#CB)\\devel")); // source tree
    project->AddLibDir(_T("$(#CB)\\sdk\\tinyxml")); // source tree
#else
    project->AddLinkerOption(_T("`wx-config --libs`"));
#endif

#ifdef __WXMSW__
    // now create the necessary env. vars
// TODO (mandrav#1#): Make these read from LibManager
    wxString wxver = _T("26");
    wxString wxcfg = _T("");
    project->SetVar(_T("WX_CFG"), wxcfg);
    project->SetVar(_T("WX_VER"), wxver);
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
    target->SetTargetType(ttDynamicLib);
    target->SetCreateDefFile(false);
    target->SetCreateStaticLib(false);
    target->SetOutputFilename(name + _T(".") + DYNAMICLIB_EXT);

    Manager::Get()->GetProjectManager()->RebuildTree();

#ifdef __WXMSW__
    wxMessageDialog msg(Manager::Get()->GetAppWindow(),
                    _("The new plugin project has been created.\n"
                    "You should now update the project's custom variables, to adjust "
                    "for your environment..."),
                    _("Information"),
                    wxOK | wxICON_INFORMATION);
    PlaceWindow(&msg);
    msg.ShowModal();
#endif
    return 0;
}
