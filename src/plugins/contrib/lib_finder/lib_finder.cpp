/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2008  Bartlomiej Swiecki
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
#include <wx/dir.h>

#include <configmanager.h>
#include <globals.h>
#include <manager.h>
#include <projectloader_hooks.h>
#include <compiler.h>
#include <compilerfactory.h>
#include <sqplus.h>
#include <sc_base_types.h>

#include "resultmap.h"
#include "dirlistdlg.h"
#include "processingdlg.h"
#include "libraryconfigmanager.h"
#include "libraryresult.h"
#include "lib_finder.h"
#include "projectconfigurationpanel.h"
#include "libselectdlg.h"

namespace
{
    // Register the plugin
    PluginRegistrant<lib_finder> reg(_T("lib_finder"));

    // Some class required for scripting
    class LibFinder
    {
    };
};

lib_finder* lib_finder::m_Singleton = 0;

lib_finder::lib_finder()
{
    m_Singleton = this;
}

lib_finder::~lib_finder()
{
    m_Singleton = 0;
}

void lib_finder::OnAttach()
{
    // Initialize components
    m_PkgConfig.RefreshData();

    // Read all known libraries
    ReadDetectedResults();
    ReadPkgConfigResults();
    ReadPredefinedResults();

    // Add project extensions
    ProjectLoaderHooks::HookFunctorBase* Hook = new ProjectLoaderHooks::HookFunctor<lib_finder>(this, &lib_finder::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(Hook);

    // Register events
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnProjectClose));
    #ifdef EVT_COMPILER_SET_BUILD_OPTIONS
        Manager::Get()->RegisterEventSink(cbEVT_COMPILER_SET_BUILD_OPTIONS, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnCompilerSetBuildOptions));
    #endif

    // Register scripting extensions
    RegisterScripting();
}

void lib_finder::OnRelease(bool appShutDown)
{
    // unregister cripting extensions
    UnregisterScripting();

    // Unregister project extensions
    ProjectLoaderHooks::UnregisterHook(m_HookId,true);
    for ( ProjectMapT::iterator i=m_Projects.begin(); i!=m_Projects.end(); ++i )
    {
        delete i->second;
    }
    m_Projects.clear();

    // Clear detected libraries
    for ( int i=0; i<rtCount; i++ )
    {
        m_KnownLibraries[i].Clear();
    }

    // Uninitialize components
    m_PkgConfig.Clear();
}

int lib_finder::Execute()
{
    LibraryConfigManager m_Manager(m_KnownLibraries);
    ResultMap m_Results;

    // Loading library search filters
    if ( !LoadSearchFilters(&m_Manager) )
    {
        cbMessageBox(
            _("Didn't found any search filters used to detect libraries.\n"
              "Please check if lib_finder plugin is installed properly."));
        return -1;
    }

    // Getting list of directories to process
    DirListDlg Dlg(
        Manager::Get()->GetAppWindow(),
        Manager::Get()->GetConfigManager(_T("lib_finder"))->ReadArrayString(_T("search_dirs")));
    if ( Dlg.ShowModal() == wxID_CANCEL ) return 0;
    Manager::Get()->GetConfigManager(_T("lib_finder"))->Write(_T("search_dirs"),Dlg.Dirs);

    // Do the processing
    FileNamesMap FNMap;
    ProcessingDlg PDlg(Manager::Get()->GetAppWindow(),m_Manager,m_KnownLibraries,m_Results);
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
                        Results[i]->ShortCode.c_str(),
                        Name.c_str()));
                if ( PreviousVar != Results[i]->ShortCode )
                {
                    Selected.Add((int)i);
                    PreviousVar = Results[i]->ShortCode;
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
                    m_KnownLibraries[rtDetected].Clear();
                }

                // Here we will store names of libraries set-up so far
                // by checking entries we will be able to find out whether
                // we have to clear previous settings
                wxArrayString AddedLibraries;

                for ( size_t i = 0; i<Selected.Count(); i++ )
                {
                    wxString Library = Results[Selected[i]]->ShortCode;

                    if ( true )
                    {
                        // Here we set-up internal libraries configuration
                        if ( Dlg.GetClearSelectedPrevious() )
                        {
                            if ( AddedLibraries.Index(Library)==wxNOT_FOUND )
                            {
                                // Ok, have to delete previosu results since this is the first
                                // occurence of this library in new set
                                ResultArray& Previous = m_KnownLibraries[rtDetected].GetShortCode(Library);
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
                            ResultArray& Previous = m_KnownLibraries[rtDetected].GetShortCode(Library);
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
                        m_KnownLibraries[rtDetected].GetShortCode(Library).Add(new LibraryResult(*Results[Selected[i]]));
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
    WriteDetectedResults();

	return -1;
}

bool lib_finder::LoadSearchFilters(LibraryConfigManager* CfgManager)
{
    wxString Sep = wxFileName::GetPathSeparator();

    CfgManager->LoadXmlConfig(ConfigManager::GetFolder(sdDataGlobal) + Sep + _T("lib_finder"));
    CfgManager->LoadXmlConfig(ConfigManager::GetFolder(sdDataUser)   + Sep + _T("lib_finder"));

    return CfgManager->GetLibraryCount()>0;
}


void lib_finder::SetGlobalVar(const LibraryResult* Res)
{
    ConfigManager * cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    wxString activeSet = cfg->Read(_T("/active"));
    wxString curr = _T("/sets/") + activeSet + _T("/") + Res->ShortCode;

    wxString IncludePath = Res->IncludePath.IsEmpty() ? _T("") : Res->IncludePath[0];
    wxString LibPath     = Res->LibPath.IsEmpty()     ? _T("") : Res->LibPath[0];
    wxString ObjPath     = Res->ObjPath.IsEmpty()     ? _T("") : Res->ObjPath[0];

    wxString CFlags;
    if ( !Res->PkgConfigVar.IsEmpty() )
    {
        CFlags.Append(_T(" `pkg-config "));
        CFlags.Append(Res->PkgConfigVar);
        CFlags.Append(_T(" --cflags`"));
    }
    for ( size_t i=0; i<Res->CFlags.Count(); i++ )
    {
        CFlags.Append(_T(" "));
        CFlags.Append(Res->CFlags[i]);
    }
    for ( size_t i=1; i<Res->IncludePath.Count(); i++ )
    {
        CFlags.Append(_T(" -I"));
        CFlags.Append(Res->IncludePath[i]);
    }
    for ( size_t i=0; i<Res->Defines.Count(); i++ )
    {
        CFlags.Append(_T(" -D"));
        CFlags.Append(Res->Defines[i]);
    }
    CFlags.Remove(0,1);

    wxString LFlags;
    if ( !Res->PkgConfigVar.IsEmpty() )
    {
        LFlags.Append(_T(" `pkg-config "));
        LFlags.Append(Res->PkgConfigVar);
        LFlags.Append(_T(" --libs`"));
    }
    for ( size_t i=0; i<Res->LFlags.Count(); i++ )
    {
        LFlags.Append(_T(" "));
        LFlags.Append(Res->LFlags[i]);
    }
    for ( size_t i=1; i<Res->LibPath.Count(); i++ )
    {
        LFlags.Append(_T(" -L"));
        LFlags.Append(Res->LibPath[i]);
    }
    for ( size_t i=1; i<Res->ObjPath.Count(); i++ )
    {
        LFlags.Append(_T(" -L"));
        LFlags.Append(Res->ObjPath[i]);
    }
    for ( size_t i=0; i<Res->Libs.Count(); i++ )
    {
        LFlags.Append(_T(" -l"));
        LFlags.Append(Res->Libs[i]);
    }
    LFlags.Remove(0,1);

    wxString BasePath = Res->BasePath;
    if ( BasePath.IsEmpty() )
    {
        // BasePath is mandatory so let's set it anyway
        if ( !Res->PkgConfigVar.IsEmpty() )
        {
            BasePath = _T("`pkg-config ")+ Res->PkgConfigVar + _T(" --variable=prefix`");
        }
        else
        {
            BasePath = _T("---");
        }
    }

    cfg->Write(curr + _T("/base"),    BasePath);
    cfg->Write(curr + _T("/include"), IncludePath);
    cfg->Write(curr + _T("/lib"),     LibPath);
    cfg->Write(curr + _T("/obj"),     ObjPath);
    cfg->Write(curr + _T("/cflags"),  CFlags);
    cfg->Write(curr + _T("/lflags"),  LFlags);
}

void lib_finder::ReadDetectedResults()
{
    m_KnownLibraries[rtDetected].Clear();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    wxArrayString Results = cfg->EnumerateSubPaths(_T("/stored_results"));
    for ( size_t i=0; i<Results.Count(); i++ )
    {
        wxString Path = _T("/stored_results/") + Results[i] + _T("/");
        LibraryResult* Result = new LibraryResult();

        Result->Type         = rtDetected;

        Result->LibraryName  = cfg->Read(Path+_T("name"),wxEmptyString);
        Result->ShortCode    = cfg->Read(Path+_T("short_code"),wxEmptyString);
        Result->BasePath     = cfg->Read(Path+_T("base_path"),wxEmptyString);
        Result->Description  = cfg->Read(Path+_T("description"),wxEmptyString);
        Result->PkgConfigVar = cfg->Read(Path+_T("pkg_config_var"),wxEmptyString);

        Result->Categories   = cfg->ReadArrayString(Path+_T("categories"));
        Result->IncludePath  = cfg->ReadArrayString(Path+_T("include_paths"));
        Result->LibPath      = cfg->ReadArrayString(Path+_T("lib_paths"));
        Result->ObjPath      = cfg->ReadArrayString(Path+_T("obj_paths"));
        Result->Libs         = cfg->ReadArrayString(Path+_T("libs"));
        Result->Defines      = cfg->ReadArrayString(Path+_T("defines"));
        Result->CFlags       = cfg->ReadArrayString(Path+_T("cflags"));
        Result->LFlags       = cfg->ReadArrayString(Path+_T("lflags"));
        Result->Compilers    = cfg->ReadArrayString(Path+_T("compilers"));

        if ( Result->ShortCode.IsEmpty() )
        {
            delete Result;
            continue;
        }

        m_KnownLibraries[rtDetected].GetShortCode(Result->ShortCode).Add(Result);
    }
}

void lib_finder::ReadPkgConfigResults()
{
    m_PkgConfig.DetectLibraries(m_KnownLibraries[rtPkgConfig]);
}

void lib_finder::ReadPredefinedResults()
{
    SearchDirs Dirs[] = { sdDataGlobal, sdDataUser };
    size_t DirsCnt = sizeof(Dirs) / sizeof(Dirs[0]);

    for ( size_t i=0; i<DirsCnt; i++ )
    {
        wxString Path = ConfigManager::GetFolder(Dirs[i]) + wxFileName::GetPathSeparator() + _T("lib_finder/predefined");
        wxDir Dir(Path);
        wxString Name;
        if ( !Dir.IsOpened() ) continue;
        if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_FILES|wxDIR_HIDDEN) )
        {
            do
            {
                LoadPredefinedResultFromFile(Path+wxFileName::GetPathSeparator()+Name);
            }
            while ( Dir.GetNext(&Name) );
        }
    }
}

void lib_finder::LoadPredefinedResultFromFile(const wxString& FileName)
{
    TiXmlDocument Doc;

    if ( !Doc.LoadFile(FileName.mb_str(wxConvFile)) ) return;

    wxString CBBase = ConfigManager::GetFolder(sdBase) + wxFileName::GetPathSeparator();

    for ( TiXmlElement* RootElem = Doc.FirstChildElement("predefined_library");
          RootElem;
          RootElem = RootElem->NextSiblingElement("predefined_library") )
    {
        for ( TiXmlElement* Elem = RootElem->FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            LibraryResult* Result = new LibraryResult();
            Result->Type         = rtPredefined;
            Result->LibraryName  = wxString(Elem->Attribute("name")      ,wxConvUTF8);
            Result->ShortCode    = wxString(Elem->Attribute("short_code"),wxConvUTF8);
            Result->BasePath     = wxString(Elem->Attribute("base_path") ,wxConvUTF8);
            Result->PkgConfigVar = wxString(Elem->Attribute("pkg_config"),wxConvUTF8);
            if ( TiXmlElement* Sub = Elem->FirstChildElement("description") )
            {
                Result->Description  = wxString(Sub->GetText(),wxConvUTF8);
            }

            for ( TiXmlAttribute* Attr = Elem->FirstAttribute(); Attr; Attr=Attr->Next() )
            {
//                if ( !strncasecmp(Attr->Name(),"category",8) )
                if ( !strncmp(Attr->Name(),"category",8) )
                {
                    Result->Categories.Add(wxString(Attr->Value(),wxConvUTF8));
                }
            }

            for ( TiXmlElement* Sub = Elem->FirstChildElement(); Sub; Sub=Sub->NextSiblingElement() )
            {
                wxString Name = wxString(Sub->Value(),wxConvUTF8).Lower();

                if ( Name == _T("path") )
                {
                    wxString Include = wxString(Sub->Attribute("include"),wxConvUTF8);
                    wxString Lib     = wxString(Sub->Attribute("lib"),wxConvUTF8);
                    wxString Obj     = wxString(Sub->Attribute("obj"),wxConvUTF8);

                    if ( !Include.IsEmpty() )
                    {
                        Result->IncludePath.Add(wxFileName(Include).IsRelative() ? CBBase + Include : Include);
                    }

                    if ( !Lib.IsEmpty() )
                    {
                        Result->LibPath.Add(wxFileName(Lib).IsRelative() ? CBBase + Lib : Lib);
                    }

                    if ( !Obj.IsEmpty() )
                    {
                        Result->ObjPath.Add(wxFileName(Obj).IsRelative() ? CBBase + Obj : Obj);
                    }
                }

                if ( Name == _T("add") )
                {
                    wxString Lib = wxString(Sub->Attribute("lib"),wxConvUTF8);
                    wxString Define = wxString(Sub->Attribute("define"),wxConvUTF8);
                    wxString CFlags = wxString(Sub->Attribute("cflags"),wxConvUTF8);
                    wxString LFlags = wxString(Sub->Attribute("lflags"),wxConvUTF8);

                    if ( !Lib.IsEmpty() ) Result->Libs.Add(Lib);
                    if ( !Define.IsEmpty() ) Result->Defines.Add(Define);
                    if ( !CFlags.IsEmpty() ) Result->CFlags.Add(CFlags);
                    if ( !LFlags.IsEmpty() ) Result->LFlags.Add(LFlags);
                }

                if ( Name == _T("compiler") )
                {
                    Result->Compilers.Add(wxString(Sub->Attribute("name"),wxConvUTF8));
                }
            }

            if ( Result->LibraryName.IsEmpty() ||
                 Result->ShortCode.IsEmpty() )
            {
                delete Result;
                continue;
            }

            m_KnownLibraries[rtPredefined].GetShortCode(Result->ShortCode).Add(Result);
        }
    }
}

void lib_finder::WriteDetectedResults()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    ResultArray Results;
    m_KnownLibraries[rtDetected].GetAllResults(Results);

    for ( size_t i=0; i<Results.Count(); i++ )
    {
        LibraryResult* Result = Results[i];
        wxString Path = wxString::Format(_T("/stored_results/res%06d/"),i);

        cfg->Write(Path+_T("name"),Result->LibraryName);
        cfg->Write(Path+_T("short_code"),Result->ShortCode);
        cfg->Write(Path+_T("base_path"),Result->BasePath);
        cfg->Write(Path+_T("description"),Result->Description);
        cfg->Write(Path+_T("pkg_config_var"),Result->PkgConfigVar);

        cfg->Write(Path+_T("categories"),Result->Categories);
        cfg->Write(Path+_T("include_paths"),Result->IncludePath);
        cfg->Write(Path+_T("lib_paths"),Result->LibPath);
        cfg->Write(Path+_T("obj_paths"),Result->ObjPath);
        cfg->Write(Path+_T("libs"),Result->Libs);
        cfg->Write(Path+_T("defines"),Result->Defines);
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
    return new ProjectConfigurationPanel(parent,GetProject(project),m_KnownLibraries);
}

void lib_finder::SetupTarget(CompileTargetBase* Target,const wxArrayString& Libs)
{
    if ( !Target ) return;
    wxArrayString NotFound;
    wxArrayString NoCompiler;
    wxArrayString NoVersion;

    for ( size_t i=0; i<Libs.Count(); i++ )
    {
        wxString& Lib = Libs[i];

        bool AnyFound   = false;
        bool Added      = false;
        bool IsCompiler = false;
        bool IsVersion  = false;

        for ( int i=0; i<rtCount && !Added; i++ )
        {
            if ( m_KnownLibraries[i].IsShortCode(Lib) )
            {
                AnyFound = true;

                // Ok, we got set of configurations for this library
                // now we only have to find the one that's needed
                // Currently we only filter compiler type

                ResultArray& Config = m_KnownLibraries[i].GetShortCode(Lib);
                for ( size_t j=0; j<Config.GetCount(); j++ )
                {
                    if ( TryAddLibrary(Target,Config[j]) )
                    {
                        Added = true;
                        break;
                    }
                }
            }
        }

        if ( !AnyFound )
        {
            NotFound.Add(Lib);
            continue;
        }
        else if ( !Added )
        {
            if ( !IsCompiler )
            {
                NoCompiler.Add(Lib);
            }
            else if ( !IsVersion )
            {
                NoVersion.Add(Lib);
            }
            else
            {
                NoCompiler.Add(Lib);
            }
        }
    }

    if ( !NotFound.IsEmpty() || !NoCompiler.IsEmpty() || !NoVersion.IsEmpty() )
    {
        wxString Message = _("Found following issues with libraries:\n");
        if ( !NotFound.IsEmpty() )
        {
            Message += _T("\n");
            Message += _("Didn't found configuration for libraries:\n");
            for ( size_t i=0; i<NotFound.Count(); i++ )
            {
                Message += _T("  * ") + NotFound[i];
            }
        }
        if ( !NoCompiler.IsEmpty() )
        {
            Message += _T("\n");
            Message += _("These libraries were not configured for used compiler:\n");
            for ( size_t i=0; i<NoCompiler.Count(); i++ )
            {
                Message += _T("  * ") + NoCompiler[i];
            }
        }
        if ( !NoVersion.IsEmpty() )
        {
            Message += _T("\n");
            Message += _("These libraries did not meet version requirements:\n");
            for ( size_t i=0; i<NoVersion.Count(); i++ )
            {
                Message += _T("  * ") + NoVersion[i];
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

    // Read used compiler to detect command line option for defines
    Compiler* comp = CompilerFactory::GetCompiler(Target->GetCompilerID());
    wxString DefinePrefix = _T("-D");
    if ( comp )
    {
        DefinePrefix = comp->GetSwitches().defines;
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

    for ( size_t i=0; i<Result->Libs.Count(); i++ )
    {
        Target->AddLinkLib(Result->Libs[i]);
    }

    for ( size_t i=0; i<Result->Defines.Count(); i++ )
    {
        Target->AddCompilerOption(DefinePrefix+Result->Defines[i]);
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
    if ( First->ShortCode   != Second->ShortCode   ) return false;
    if ( First->LibraryName != Second->LibraryName ) return false;
    if ( First->BasePath    != Second->BasePath    ) return false;
    if ( First->Description != Second->Description ) return false;
    return true;
}

void lib_finder::RegisterScripting()
{
    SqPlus::SQClassDef<LibFinder>("LibFinder")
        .staticFunc(&lib_finder::AddLibraryToProject,"AddLibraryToProject")
        .staticFunc(&lib_finder::IsLibraryInProject,"IsLibraryInProject")
        .staticFunc(&lib_finder::RemoveLibraryFromProject,"RemoveLibraryFromProject")
    ;
}

void lib_finder::UnregisterScripting()
{
    Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    if ( v )
    {
        sq_pushroottable(v);
        sq_pushstring(v,"LibFinder",-1);
        sq_deleteslot(v,-2,false);
        sq_poptop(v);
    }
}

bool lib_finder::AddLibraryToProject(const wxString& LibName,cbProject* Project,const wxString& TargetName)
{
    if ( !m_Singleton ) return false;

    ProjectConfiguration* Config = m_Singleton->GetProject(Project);
    wxArrayString* Libs;
    if ( TargetName.IsEmpty() )
    {
        Libs = &Config->m_GlobalUsedLibs;
    }
    else
    {
        if ( !Project->GetBuildTarget(TargetName) ) return false;
        Libs = &Config->m_TargetsUsedLibs[TargetName];
    }

    if ( Libs->Index(LibName) == wxNOT_FOUND )
    {
        Libs->Add(LibName);
        Project->SetModified(true);
    }
    return true;
}

bool lib_finder::IsLibraryInProject(const wxString& LibName,cbProject* Project,const wxString& TargetName)
{
    if ( !m_Singleton ) return false;

    ProjectConfiguration* Config = m_Singleton->GetProject(Project);
    wxArrayString* Libs;
    if ( TargetName.IsEmpty() )
    {
        Libs = &Config->m_GlobalUsedLibs;
    }
    else
    {
        if ( !Project->GetBuildTarget(TargetName) ) return false;
        Libs = &Config->m_TargetsUsedLibs[TargetName];
    }

    return Libs->Index(LibName) != wxNOT_FOUND;
}

bool lib_finder::RemoveLibraryFromProject(const wxString& LibName,cbProject* Project,const wxString& TargetName)
{
    if ( !m_Singleton ) return false;

    ProjectConfiguration* Config = m_Singleton->GetProject(Project);
    wxArrayString* Libs;
    if ( TargetName.IsEmpty() )
    {
        Libs = &Config->m_GlobalUsedLibs;
    }
    else
    {
        if ( !Project->GetBuildTarget(TargetName) ) return false;
        Libs = &Config->m_TargetsUsedLibs[TargetName];
    }

    int Index = Libs->Index(LibName);
    if ( Index == wxNOT_FOUND )
    {
        return false;
    }

    Libs->RemoveAt(Index);
    Project->SetModified(true);
    return true;
}
