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
* $Revision$
* $Id$
* $HeadURL$
*/


#include <wx/arrstr.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/dir.h>
#include <wx/url.h>
#include <wx/wfstream.h>


#include <configmanager.h>
#include <globals.h>
#include <manager.h>
#include <projectloader_hooks.h>
#include <compiler.h>
#include <compilerfactory.h>
#include <scripting/bindings/sc_utils.h>
#include <scripting/bindings/sc_typeinfo_all.h>
#include <scriptingmanager.h>
#include <logmanager.h>

#include "resultmap.h"
#include "libraryresult.h"
#include "lib_finder.h"
#include "projectconfigurationpanel.h"
#include "librariesdlg.h"
#include "projectmissinglibs.h"


namespace
{
    // Register the plugin
    PluginRegistrant<lib_finder> reg(_T("lib_finder"));

    static const bool ExtraEventPresent = true;
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
    m_KnownLibraries[rtDetected].ReadDetectedResults();
    m_KnownLibraries[rtPkgConfig].ReadPkgConfigResults( &m_PkgConfig );
    m_KnownLibraries[rtPredefined].ReadPredefinedResults();

    // Add project extensions
    ProjectLoaderHooks::HookFunctorBase* Hook = new ProjectLoaderHooks::HookFunctor<lib_finder>(this, &lib_finder::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(Hook);

    // Register events
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnProjectClose));
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_STARTED, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnCompilerStarted));
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnCompilerFinished));
    if ( ExtraEventPresent )
    {
        Manager::Get()->RegisterEventSink(cbEVT_COMPILER_SET_BUILD_OPTIONS, new cbEventFunctor<lib_finder, CodeBlocksEvent>(this, &lib_finder::OnCompilerSetBuildOptions));
    }

    // Register scripting extensions
    RegisterScripting();
}

void lib_finder::OnRelease(bool /*appShutDown*/)
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
    LibrariesDlg m_Dlg(Manager::Get()->GetAppWindow(),m_KnownLibraries);
    PlaceWindow(&m_Dlg);
    m_Dlg.ShowModal();
	return -1;
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
    if ( Conf->m_DisableAuto ) return;
    wxString Target = event.GetBuildTargetName();
    if ( Target.IsEmpty() )
    {
        // Setting up options for project
        SetupTarget(Proj,Conf->m_GlobalUsedLibs);
    }
    else
    {
        // Setting up compile target
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
    return new ProjectConfigurationPanel(parent,GetProject(project),project,m_KnownLibraries);
}

void lib_finder::SetupTarget(CompileTargetBase* Target,const wxArrayString& Libs)
{
    if ( !Target ) return;
    wxArrayString NotFound;
    wxArrayString NoCompiler;
    wxArrayString NoVersion;

    std::set< wxString > included;
    std::set< wxString > toInclude;

    for ( size_t i=0; i<Libs.Count(); i++ )
    {
        toInclude.insert( Libs[i] );
    }

    while ( !toInclude.empty() )
    {
        wxString Lib = *toInclude.begin();
        toInclude.erase( toInclude.begin() );

        if ( included.find( Lib ) != included.end() )
        {
            // Library already processed
            continue;
        }
        included.insert( Lib );

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
                        for ( size_t k=0; k<Config[j]->Require.GetCount(); k++ )
                        {
                            toInclude.insert( Config[j]->Require[k] );
                        }
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
        Message += _T("\n");
        Message += _("Would you like to go to detection dialog ?");

        if ( cbMessageBox( Message, _("LibFinder - error"), wxYES_NO|wxICON_EXCLAMATION ) == wxID_YES )
        {
            ProjectMissingLibs dlg(Manager::Get()->GetAppWindow(), NotFound, m_KnownLibraries);
            PlaceWindow(&dlg);
            dlg.ShowModal();
        }
    }
}

bool lib_finder::TryAddLibrary(CompileTargetBase* Target,LibraryResult* Result)
{
    if ( !Result->Compilers.IsEmpty() )
    {
        wxString compilerId = Target->GetCompilerID();
        bool found = false;
        for ( size_t i=0; i<Result->Compilers.Count(); i++ )
        {
            if ( Result->Compilers[i].Matches( compilerId ) )
            {
                found = true;
                break;
            }
        }

        if ( !found )
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

namespace ScriptBindings
{

// Dummy type
struct LibFinder{};

template<>
struct TypeInfo<LibFinder> {
    static uint32_t typetag;
    static constexpr const SQChar *className = _SC("LibFinder");
    using baseClass = void;
};

uint32_t TypeInfo<LibFinder>::typetag = uint32_t(TypeTag::Unassigned);

template<bool (*func)(const wxString &, cbProject *, const wxString &)>
SQInteger LibFinder_LibraryToProject(HSQUIRRELVM v)
{
    // env table, LibName, Project, TargetName
    ExtractParams4<SkipParam, const wxString *, cbProject *, const wxString *> extractor(v);
    if (!extractor.Process("LibFinder::LibraryToProject"))
        return extractor.ErrorMessage();
    const bool result = func(*extractor.p1, extractor.p2, *extractor.p3);
    sq_pushbool(v, result);
    return 1;
}

SQInteger LibFinder_SetupTargetManually(HSQUIRRELVM v)
{
    // env table, Target
    ExtractParams2<SkipParam, CompileTargetBase *> extractor(v);
    if (!extractor.Process("LibFinder::SetupTargetManually"))
        return extractor.ErrorMessage();
    const bool result = lib_finder::SetupTargetManually(extractor.p1);
    sq_pushbool(v, result);
    return 1;
}

SQInteger LibFinder_EnsureIsDefined(HSQUIRRELVM v)
{
    // env table, ShortCode
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("LibFinder::EnsureIsDefined"))
        return extractor.ErrorMessage();
    const bool result = lib_finder::EnsureIsDefined(*extractor.p1);
    sq_pushbool(v, result);
    return 1;
}

} // namespace ScriptBindings

void lib_finder::RegisterScripting()
{
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = scriptMgr->GetVM();
    if (v)
    {
        using namespace ScriptBindings;

        TypeInfo<LibFinder>::typetag = scriptMgr->RequestClassTypeTag();

        PreserveTop preserveTop(v);

        sq_pushroottable(v);
        const SQInteger classDecl = CreateClassDecl<LibFinder>(v);
        BindDisabledCtor(v);
        BindStaticMethod(v, _SC("AddLibraryToProject"),
                         LibFinder_LibraryToProject<lib_finder::AddLibraryToProject>,
                         _SC("LibFinder::AddLibraryToProject"));
        BindStaticMethod(v, _SC("IsLibraryInProject"),
                         LibFinder_LibraryToProject<lib_finder::IsLibraryInProject>,
                         _SC("LibFinder::IsLibraryInProject"));
        BindStaticMethod(v, _SC("RemoveLibraryFromProject"),
                         LibFinder_LibraryToProject<lib_finder::RemoveLibraryFromProject>,
                         _SC("LibFinder::RemoveLibraryFromProject"));
        BindStaticMethod(v, _SC("SetupTargetManually"), LibFinder_SetupTargetManually,
                         _SC("LibFinder::SetupTargetManually"));
        BindStaticMethod(v, _SC("EnsureIsDefined"), LibFinder_EnsureIsDefined,
                         _SC("LibFinder::EnsureIsDefined"));

        // Put the class in the root table. This must be last!
        sq_newslot(v, classDecl, SQFalse);

        sq_poptop(v); // Pop root table.
    }
}

void lib_finder::UnregisterScripting()
{
    HSQUIRRELVM v = Manager::Get()->GetScriptingManager()->GetVM();
    if (v)
    {
        using namespace ScriptBindings;
        PreserveTop preserveTop(v);
        sq_pushroottable(v);
        sq_pushstring(v, _SC("LibFinder"), -1);
        sq_deleteslot(v, -2, false);
        sq_poptop(v);

        TypeInfo<LibFinder>::typetag = uint32_t(TypeTag::Unassigned);
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

bool lib_finder::SetupTargetManually(CompileTargetBase* Target)
{
    if ( !m_Singleton ) return false;

    if ( m_Singleton->m_Targets.find(Target) == m_Singleton->m_Targets.end() ) return false;
    m_Singleton->SetupTarget(Target,m_Singleton->m_Targets[Target]);
    return true;
}

bool lib_finder::EnsureIsDefined(const wxString& ShortCode)
{
    if ( !m_Singleton ) return false;

    for ( int i=0; i<rtCount; i++ )
    {
        if ( m_Singleton->m_KnownLibraries[i].IsShortCode(ShortCode) )
        {
            // Found this library
            return true;
        }
    }

    // Did not foud library
    return false;
}

bool lib_finder::TryDownload(const wxString& ShortCode,const wxString& FileName)
{
    wxArrayString Urls = Manager::Get()->GetConfigManager(_T("lib_finder"))->ReadArrayString(_T("download_urls"));
    for ( size_t i=0; i<Urls.Count(); i++ )
    {
        wxString Url = Urls[i];
        if ( Url.IsEmpty() ) continue;

        if ( Url.Last() != _T('/') ) Url.Append(_T('/'));
        Url << ShortCode << _T(".xml");

        wxURL UrlData(Url);
        if ( !UrlData.IsOk() )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Invalid url '%s'"),Url.wx_str()));
            continue;
        }
        UrlData.SetProxy( ConfigManager::GetProxy() );

        wxInputStream* is = UrlData.GetInputStream();
        if ( !is || !is->IsOk() )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Couldn't open stream for '%s'"),Url.wx_str()));
            delete is;
            continue;
        }

        wxFileOutputStream Output(FileName);
        if ( !Output.IsOk() )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Couldn't write to file '%s'"),FileName.wx_str()));
            delete is;
            return false;
        }

        is->Read( Output );
        bool ret = is->IsOk() && Output.IsOk();
        delete is;
        return ret;
    }

    LogManager::Get()->LogWarning(F(_T("lib_finder: Couldn't find suitable download url for '%s'"),ShortCode.wx_str()));
    return false;
}

void lib_finder::OnCompilerStarted(CodeBlocksEvent& event)
{
    // When compilation starts, we generate map of build targets
    // for each target in project - this may be used later to
    // allow invoking lib_finder setup from scripts
    event.Skip();
    m_Targets.clear();

    cbProject* cbp = ProjectManager::Get()->GetActiveProject();
    if ( !cbp ) return;

    ProjectConfiguration* Conf = GetProject(cbp);

    // Copy libraries used in whole project
    m_Targets[(CompileTargetBase*)cbp] = Conf->m_GlobalUsedLibs;

    // Copy libraries used in build targets
    for ( int i=0; i<cbp->GetBuildTargetsCount(); i++ )
    {
        m_Targets[(CompileTargetBase*)(cbp->GetBuildTarget(i))] = Conf->m_TargetsUsedLibs[cbp->GetBuildTarget(i)->GetTitle()];
    }
}

void lib_finder::OnCompilerFinished(CodeBlocksEvent& event)
{
    event.Skip();

    // We don't need target bindings anymore
    m_Targets.clear();
}

bool lib_finder::IsExtraEvent()
{
    return ExtraEventPresent;
}
