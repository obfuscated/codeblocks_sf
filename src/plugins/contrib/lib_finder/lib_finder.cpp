/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/


#include <wx/arrstr.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/string.h>

#include <configmanager.h>
#include <globals.h>
#include <manager.h>
#include <projectloader_hooks.h>

#include "resultmap.h"
#include "dirlistdlg.h"
#include "processingdlg.h"
#include "libraryconfigmanager.h"
#include "libraryresult.h"
#include "lib_finder.h"
#include "projectconfigurationpanel.h"
#include "libselectdlg.h"

// Register the plugin
namespace
{
    PluginRegistrant<lib_finder> reg(_T("lib_finder"));
};

lib_finder::lib_finder()
{
}

lib_finder::~lib_finder()
{
}

void lib_finder::OnAttach()
{
    ReadStoredResults();
    ProjectLoaderHooks::HookFunctorBase* Hook = new ProjectLoaderHooks::HookFunctor<lib_finder>(this, &lib_finder::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(Hook);

    m_PkgConfig.RefreshData();

    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnProjectClose));
    #ifdef EVT_COMPILER_SET_BUILD_OPTIONS
        Manager::Get()->RegisterEventSink(cbEVT_COMPILER_SET_BUILD_OPTIONS, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnCompilerSetBuildOptions));
    #endif
}

void lib_finder::OnRelease(bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(m_HookId,true);

    for ( ProjectMapT::iterator i=m_Projects.begin(); i!=m_Projects.end(); ++i )
    {
        delete i->second;
    }
    m_Projects.clear();

    ClearStoredResults();

    m_PkgConfig.Clear();
}

int lib_finder::Execute()
{
    m_Manager.Clear();
    ResultMap m_Results;

    wxString resPath = ConfigManager::GetDataFolder();
    m_Manager.LoadXmlConfig(resPath + wxFileName::GetPathSeparator() + _T("lib_finder"));

    if ( m_Manager.GetLibraryCount() == 0 )
    {
        cbMessageBox(_("Didn't found any library :("));
        return -1;
    }

    DirListDlg Dlg(
        Manager::Get()->GetAppWindow(),
        Manager::Get()->GetConfigManager(_T("lib_finder"))->ReadArrayString(_T("search_dirs")));
    if ( Dlg.ShowModal() == wxID_CANCEL ) return 0;
    Manager::Get()->GetConfigManager(_T("lib_finder"))->Write(_T("search_dirs"),Dlg.Dirs);

    FileNamesMap FNMap;
    ProcessingDlg PDlg(0L,m_Manager,m_PkgConfig,m_Results);
    PDlg.Show();
    PDlg.MakeModal(true);
    if ( PDlg.ReadDirs(Dlg.Dirs) && PDlg.ProcessLibs() )
    {
        PDlg.Hide();
        ResultArray Results;
        m_Results.GetAllResults(Results);
        if ( Results.Count() == 0 )
        {
            cbMessageBox(_("Didn't found any library"));
        }
        else
        {
            wxArrayString Names;
            wxArrayInt Selected;
            wxString PreviousVar;
            for ( size_t i=0; i<Results.Count(); ++i )
            {
                wxString& Name =
                    Results[i]->Description.IsEmpty() ?
                    Results[i]->LibraryName :
                    Results[i]->Description;

                Names.Add(
                    wxString::Format(_T("%s : %s"),
                        Results[i]->GlobalVar.c_str(),
                        Name.c_str()));
                if ( PreviousVar != Results[i]->GlobalVar )
                {
                    Selected.Add((int)i);
                    PreviousVar = Results[i]->GlobalVar;
                }
            }

            LibSelectDlg Dlg(Manager::Get()->GetAppWindow(),Names);
            Dlg.SetSelections(Selected);

            if ( Dlg.ShowModal() == wxID_OK )
            {
                // Fetch selected libraries
                Selected = Dlg.GetSelections();

                // Clear all results if requested
                if ( Dlg.GetClearAllPrevious() )
                {
                    m_StoredResults.Clear();
                }

                // Here we will store names of libraries set-up so far
                // by checking entries we will be able to find out whether
                // we have to clear previous settings
                wxArrayString AddedLibraries;

                for ( size_t i = 0; i<Selected.Count(); i++ )
                {
                    wxString Library = Results[Selected[i]]->GlobalVar;

                    if ( true )
                    {
                        // Here we set-up internal libraries configuration
                        if ( Dlg.GetClearSelectedPrevious() )
                        {
                            if ( AddedLibraries.Index(Library)==wxNOT_FOUND )
                            {
                                // Ok, have to delete previosu results since this is the first
                                // occurence of this library in new set
                                ResultArray& Previous = m_StoredResults.GetGlobalVar(Library);
                                for ( size_t j=0; j<Previous.Count(); j++ )
                                {
                                    delete Previous[j];
                                }
                                Previous.Clear();
                            }
                            AddedLibraries.Add(Library);
                        }
                        else if ( Dlg.GetDontClearPrevious() )
                        {
                            // Find and remove duplicates
                            ResultArray& Previous = m_StoredResults.GetGlobalVar(Library);
                            for ( size_t j=0; j<Previous.Count(); j++ )
                            {
                                if ( SameResults(Previous[j],Results[Selected[i]]) )
                                {
                                    delete Previous[j];
                                    Previous.RemoveAt(j--);
                                }
                            }
                        }

                        // Add the result
                        m_StoredResults.GetGlobalVar(Library).Add(new LibraryResult(*Results[Selected[i]]));
                    }

                    if ( Dlg.GetSetupGlobalVars() )
                    {
                        // Here we set-up global variables
                        SetGlobalVar(Results[Selected[i]]);
                    }
                }
            }
        }
    }
    PDlg.MakeModal(false);
    WriteStoredResults();

	return -1;
}

void lib_finder::SetGlobalVar(const LibraryResult* Res)
{
    ConfigManager * cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    wxString activeSet = cfg->Read(_T("/active"));
    wxString curr = _T("/sets/") + activeSet + _T("/") + Res->GlobalVar;

    wxString IncludePath = Res->IncludePath.IsEmpty() ? _T("") : Res->IncludePath[0];
    wxString LibPath     = Res->LibPath.IsEmpty()     ? _T("") : Res->LibPath[0];
    wxString ObjPath     = Res->ObjPath.IsEmpty()     ? _T("") : Res->ObjPath[0];

    wxString CFlags;
    for ( size_t i=0; i<Res->CFlags.Count(); i++ )
    {
        CFlags.Append(L" ");
        CFlags.Append(Res->CFlags[i]);
    }
    for ( size_t i=1; i<Res->IncludePath.Count(); i++ )
    {
        CFlags.Append(L" -I");
        CFlags.Append(Res->IncludePath[i]);
    }
    CFlags.Remove(0,1);

    wxString LFlags;
    for ( size_t i=0; i<Res->LFlags.Count(); i++ )
    {
        LFlags.Append(L" ");
        LFlags.Append(Res->LFlags[i]);
    }
    for ( size_t i=1; i<Res->LibPath.Count(); i++ )
    {
        LFlags.Append(L" -L");
        LFlags.Append(Res->LibPath[i]);
    }
    for ( size_t i=1; i<Res->ObjPath.Count(); i++ )
    {
        LFlags.Append(L" -L");
        LFlags.Append(Res->ObjPath[i]);
    }
    LFlags.Remove(0,1);

    cfg->Write(curr + _T("/base"),    Res->BasePath);
    cfg->Write(curr + _T("/include"), IncludePath);
    cfg->Write(curr + _T("/lib"),     LibPath);
    cfg->Write(curr + _T("/obj"),     ObjPath);
    cfg->Write(curr + _T("/cflags"),  CFlags);
    cfg->Write(curr + _T("/lflags"),  LFlags);
}

void lib_finder::ClearStoredResults()
{
    m_StoredResults.Clear();
}

void lib_finder::ReadStoredResults()
{
    ClearStoredResults();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    wxArrayString Results = cfg->EnumerateSubPaths(_T("/stored_results"));
    for ( size_t i=0; i<Results.Count(); i++ )
    {
        wxString Path = _T("/stored_results/") + Results[i] + _T("/");
        LibraryResult* Result = new LibraryResult();

        Result->LibraryName  = cfg->Read(Path+_T("name"),wxEmptyString);
        Result->GlobalVar    = cfg->Read(Path+_T("global_var"),wxEmptyString);
        Result->BasePath     = cfg->Read(Path+_T("base_path"),wxEmptyString);
        Result->Description  = cfg->Read(Path+_T("description"),wxEmptyString);
        Result->PkgConfigVar = cfg->Read(Path+_T("pkg_config_var"),wxEmptyString);

        Result->Categories   = cfg->ReadArrayString(Path+_T("categories"));
        Result->IncludePath  = cfg->ReadArrayString(Path+_T("include_paths"));
        Result->LibPath      = cfg->ReadArrayString(Path+_T("lib_paths"));
        Result->ObjPath      = cfg->ReadArrayString(Path+_T("obj_paths"));
        Result->CFlags       = cfg->ReadArrayString(Path+_T("cflags"));
        Result->LFlags       = cfg->ReadArrayString(Path+_T("lflags"));
        Result->Compilers    = cfg->ReadArrayString(Path+_T("compilers"));

        m_StoredResults.GetGlobalVar(Result->GlobalVar).Add(Result);
    }
}

void lib_finder::WriteStoredResults()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    ResultArray Results;
    m_StoredResults.GetAllResults(Results);

    for ( size_t i=0; i<Results.Count(); i++ )
    {
        LibraryResult* Result = Results[i];
        wxString Path = wxString::Format(_T("/stored_results/res%06d/"),i);

        cfg->Write(Path+_T("name"),Result->LibraryName);
        cfg->Write(Path+_T("global_var"),Result->GlobalVar);
        cfg->Write(Path+_T("base_path"),Result->BasePath);
        cfg->Write(Path+_T("description"),Result->Description);
        cfg->Write(Path+_T("pkg_config_var"),Result->PkgConfigVar);

        cfg->Write(Path+_T("categories"),Result->Categories);
        cfg->Write(Path+_T("include_paths"),Result->IncludePath);
        cfg->Write(Path+_T("lib_paths"),Result->LibPath);
        cfg->Write(Path+_T("obj_paths"),Result->ObjPath);
        cfg->Write(Path+_T("cflags"),Result->CFlags);
        cfg->Write(Path+_T("lflags"),Result->LFlags);
        cfg->Write(Path+_T("compilers"),Result->Compilers);
    }
}

void lib_finder::OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading)
{
    ProjectConfiguration* Proj = GetProject(project);
    if ( loading ) Proj->XmlLoad(elem,project);
    else           Proj->XmlWrite(elem,project);
}

void lib_finder::OnProjectClose(CodeBlocksEvent& event)
{
    event.Skip();
    cbProject* Proj = event.GetProject();
    ProjectMapT::iterator i = m_Projects.find(Proj);
    if ( i == m_Projects.end() ) return;
    delete i->second;
    m_Projects.erase(i);
}

void lib_finder::OnCompilerSetBuildOptions(CodeBlocksEvent& event)
{
    event.Skip();
    cbProject* Proj = event.GetProject();
    ProjectConfiguration* Conf = GetProject(Proj);
    wxString Target = event.GetBuildTargetName();
    if ( Target.IsEmpty() )
    {
        // Setting up options for project
        SetupTarget(Proj,Conf->m_GlobalUsedLibs);
    }
    else
    {
        // SEtting up compile target
        SetupTarget(Proj->GetBuildTarget(Target),Conf->m_TargetsUsedLibs[Target]);
    }
}

ProjectConfiguration* lib_finder::GetProject(cbProject* Project)
{
    ProjectConfiguration* Conf = m_Projects[Project];
    if ( !Conf )
    {
        Conf = m_Projects[Project] = new ProjectConfiguration();
    }
    return Conf;
}

cbConfigurationPanel* lib_finder::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    return new ProjectConfigurationPanel(parent,GetProject(project),m_StoredResults,m_PkgConfig.GetLibraries());
}

void lib_finder::SetupTarget(CompileTargetBase* Target,const wxArrayString& Libs)
{
    if ( !Target ) return;
    wxArrayString NotFound;
    wxArrayString NoCompiler;

    for ( size_t i=0; i<Libs.Count(); i++ )
    {
        wxString& Lib = Libs[i];
        ResultArray* Config = 0;

        if ( m_StoredResults.IsGlobalVar(Lib) )
        {
            Config = &m_StoredResults.GetGlobalVar(Lib);
        }
        else if ( m_PkgConfig.GetLibraries().IsGlobalVar(Lib) )
        {
            Config = &m_PkgConfig.GetLibraries().GetGlobalVar(Lib);
        }

        if ( !Config )
        {
            NotFound.Add(Lib);
            continue;
        }

        // Ok, we got set of configurations for this library
        // now we only have to find the one that's needed
        // Currently we only filter compiler type
        bool Ok = false;
        for ( size_t j=0; j<Config->GetCount(); j++ )
        {
            if ( TryAddLibrary(Target,(*Config)[j]) )
            {
                Ok = true;
                break;
            }
        }
        if ( !Ok )
        {
            NoCompiler.Add(Lib);
        }
    }

    if ( !NotFound.IsEmpty() || !NoCompiler.IsEmpty() )
    {
        wxString Message = _("Found following issues with libraries:\n\n");
        if ( !NotFound.IsEmpty() )
        {
            Message += _("Didn't found configuration for libraries:\n");
            for ( size_t i=0; i<NotFound.Count(); i++ )
            {
                Message += _T("  * ") + NotFound[i];
            }
            if ( !NoCompiler.IsEmpty() )
            {
                Message += _T("\n");
            }
        }
        if ( !NoCompiler.IsEmpty() )
        {
            Message += _("These libraries were not configured for used compiler:\n");
            for ( size_t i=0; i<NoCompiler.Count(); i++ )
            {
                Message += _T("  * ") + NoCompiler[i];
            }
        }

        wxMessageBox(Message,_("LibFinder - error"));
    }
}

bool lib_finder::TryAddLibrary(CompileTargetBase* Target,LibraryResult* Result)
{
    if ( !Result->Compilers.IsEmpty() )
    {
        if ( Result->Compilers.Index(Target->GetCompilerID()) == wxNOT_FOUND )
        {
            // Target's compiler does not match list of supported compilers for this result
            return false;
        }
    }

    // Ok, this target match the result, let's update compiler options
    if ( !Result->PkgConfigVar.IsEmpty() )
    {
        if ( !m_PkgConfig.UpdateTarget(Result->PkgConfigVar,Target) )
        {
            return false;
        }
    }

    for ( size_t i=0; i<Result->IncludePath.Count(); i++ )
    {
        Target->AddIncludeDir(Result->IncludePath[i]);
    }

    for ( size_t i=0; i<Result->LibPath.Count(); i++ )
    {
        Target->AddLibDir(Result->LibPath[i]);
    }

    for ( size_t i=0; i<Result->ObjPath.Count(); i++ )
    {
        Target->AddResourceIncludeDir(Result->ObjPath[i]);
    }

    for ( size_t i=0; i<Result->CFlags.Count(); i++ )
    {
        Target->AddCompilerOption(Result->CFlags[i]);
    }

    for ( size_t i=0; i<Result->LFlags.Count(); i++ )
    {
        Target->AddLinkerOption(Result->LFlags[i]);
    }

    return true;
}

bool lib_finder::SameResults(LibraryResult* First, LibraryResult* Second)
{
    if ( First->GlobalVar   != Second->GlobalVar   ) return false;
    if ( First->LibraryName != Second->LibraryName ) return false;
    if ( First->BasePath    != Second->BasePath    ) return false;
    if ( First->Description != Second->Description ) return false;
    return true;
}
