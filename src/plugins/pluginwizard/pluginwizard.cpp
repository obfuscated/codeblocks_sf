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
* $Id$
* $Date$
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
#include <customvars.h>
#include "pluginwizarddlg.h"

CB_IMPLEMENT_PLUGIN(PluginWizard);


PluginWizard::PluginWizard()
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString);
    wxXmlResource::Get()->Load(resPath + _T("/plugin_wizard.zip#zip:*.xrc"));

    m_PluginInfo.name = _T("PluginWizard");
    m_PluginInfo.title = _("Code::Blocks Plugin wizard");
    m_PluginInfo.version = _T("0.1");
    m_PluginInfo.description = _("This is the Code::Blocks plugin wizard...");
    m_PluginInfo.author = _T("Yiannis An. Mandravellos");
    m_PluginInfo.authorEmail = _T("info@codeblocks.org");
    m_PluginInfo.authorWebsite = _T("www.codeblocks.org");
    m_PluginInfo.thanksTo = _T("");
	m_PluginInfo.hasConfigure = false;
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
	if (dlg.ShowModal() != wxID_OK)
        return -1;

    cbProject* project = Manager::Get()->GetProjectManager()->NewProject();
    if (!project)
        return -1;

    // add compiler options
#ifdef __WXMSW__
    project->AddCompilerOption(_T("-D__GNUWIN32__"));
    project->AddCompilerOption(_T("-DWXUSINGDLL"));
    project->AddCompilerOption(_T("-DBUILDING_PLUGIN"));
    // wx & cb dirs
    project->AddIncludeDir(_T("$(WX_DIR)\\include"));
    project->AddIncludeDir(_T("$(WX_DIR)\\lib\\gcc_dll\\msw"));
    project->AddIncludeDir(_T("$(WX_DIR)\\lib\\gcc_dll$(WX_CFG)\\msw"));
    project->AddIncludeDir(_T("$(WX_DIR)\\contrib\\include"));
    project->AddIncludeDir(_T("$(CB_SDK)\\include")); // SDK installation
    project->AddIncludeDir(_T("$(CB_SDK)\\sdk")); // source tree
    project->AddIncludeDir(_T("$(CB_SDK)\\sdk\\tinyxml")); // source tree
    project->AddIncludeDir(_T("$(CB_SDK)\\sdk\\wxscintilla\\include")); // source tree
#else
    project->AddCompilerOption(_T("`wx-config --cflags`"));
#endif

#ifdef __WXMSW__
// NOTE (rickg22#1#): How not to hardwire wxmsw242 into the plugin?
// NOTE (mandrav#1#): By making the version an environment variable...
    project->AddLinkLib(_T("wxmsw$(WX_VER)"));
    // wx & cb dirs
    project->AddLibDir(_T("$(WX_DIR)\\lib\\gcc_dll\\msw"));
    project->AddLibDir(_T("$(WX_DIR)\\lib\\gcc_dll$(WX_CFG)"));
    project->AddLibDir(_T("$(CB_SDK)\\lib")); // SDK installation
    project->AddLibDir(_T("$(CB_SDK)\\devel")); // source tree
    project->AddLibDir(_T("$(CB_SDK)\\sdk\\tinyxml")); // source tree
    project->AddLibDir(_T("$(CB_SDK)\\sdk\\wxscintilla\\include")); // source tree
#else
    project->AddLinkerOption(_T("`wx-config --libs`"));
#endif

#ifdef __WXMSW__
    // now create the necessary env. vars
// TODO (mandrav#1#): Make these read from LibManager
    wxString wxver = _T("26");
    wxString wxdir = _T("C:\\wxWidgets-2.6.1");
    wxString wxcfg = _T("NonUnicode");
    wxString cbsdk = _T("C:\\codeblocks-1.0rc2");
    CustomVars vars;
    vars.Add(_T("WX_VER"), wxver);
    vars.Add(_T("WX_DIR"), wxdir);
    vars.Add(_T("WX_CFG"), wxcfg);
    vars.Add(_T("CB_SDK"), cbsdk);
    project->SetCustomVars(vars);
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
    msg.ShowModal();
#endif
    return 0;
}
