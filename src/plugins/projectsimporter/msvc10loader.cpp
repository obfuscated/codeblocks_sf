/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/msgdlg.h>

    #include "manager.h"
    #include "logmanager.h"
    #include "cbproject.h"
    #include "globals.h"
    #include "compilerfactory.h"
    #include "compiler.h"
#endif

#include <wx/choicdlg.h>

#include <tinyxml/tinyxml.h>

#include "prep.h"
#include "msvc7workspaceloader.h"
#include "msvc10loader.h"
#include "multiselectdlg.h"
#include "importers_globals.h"

/** Remarks:
    This class is as close as possible to the MSVC6 and MSVC7 loaders.
    The similarity will help maintenance. However, "similar" does not mean "the same as"

    The results configuration parsing is stored in the member m_pc
    This hash map contains all the target information: Platform, Debug/Release, DLL/Lib/Executable, ...
    A CodeBlock target is created for each element in this hash map.
**/

const wxString MSVC10Loader::g_AdditionalDependencies = _T("winmm.lib;comctl32.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib");

MSVC10Loader::MSVC10Loader(cbProject* project) :
    m_pProject(project),
    m_ConvertSwitches(false),
    m_NoImportLib(false)
{
    if (platform::windows)
        m_PlatformName = _T("Win32");
    else if (platform::Linux)
        m_PlatformName = _T("Linux");
    else if (platform::macosx)
        m_PlatformName = _T("MacOSX");
    else
        m_PlatformName = _T("Unknown");
}

MSVC10Loader::~MSVC10Loader()
{
}

bool MSVC10Loader::Open(const wxString& filename)
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    m_ConvertSwitches = m_pProject->GetCompilerID().IsSameAs(_T("gcc"));
    m_ProjectName = wxFileName(filename).GetName();
    if (!MSVC7WorkspaceLoader::g_WorkspacePath.IsEmpty())
    {
        wxFileName tmp(MSVC7WorkspaceLoader::g_WorkspacePath); tmp.MakeRelativeTo(m_pProject->GetBasePath());
        m_WorkspacePath = tmp.GetPathWithSep();
    }

    pMsg->DebugLog(F(_("Importing MSVC 10+ project: %s"), filename.wx_str()));

    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
        return false;

    pMsg->DebugLog(_("Parsing project file..."));
    const TiXmlElement* root = doc.FirstChildElement("Project");
    if (!root)
    {
        pMsg->DebugLog(_("Not a valid MS Visual Studio project file..."));
        return false;
    }

    // initialisation of the project
    m_pProject->ClearAllProperties();
    m_pProject->SetModified(true);
    if (!m_ConvertSwitches)
    {
//        m_pProject->AddCompilerOption(_T("/EHsc")); // default, "/EHs /EHc" works as well
        m_pProject->AddLinkerOption(_T("/pdb:$(TARGET_OUTPUT_DIR)$(TARGET_OUTPUT_BASENAME).pdb"));
        m_pProject->AddIncludeDir(_T(".")); // some projects require it. Implicit with Visual Studio
        m_pProject->AddResourceIncludeDir(_T("."));
    }

    bool bResult = GetProjectGlobals(root)         // get project name & type
                && GetProjectConfigurations(root); // get the project list of configuration => 1 configuration = 1 build target in CodeBlocks

    if (!bResult)
    {
        pMsg->DebugLog(_("Could not obtain project configurations."));
        return false;
    }

    if ( !DoSelectConfigurations() )
        return true; // user cancelled

    if ( !DoCreateConfigurations() )
    {
        pMsg->DebugLog(_("Failed to create configurations in the project."));
        return false;
    }

    bResult = GetProjectConfigurationFiles(root) // get the project list of files and add them to the targets
           && GetProjectIncludes(root)           // get the project/target list of includes and add them to the targets
           && GetTargetSpecific(root);           // get the project/target specific settings

    return bResult;
}

bool MSVC10Loader::GetProjectGlobals(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    const char* title = root->Attribute("NoName");
    if (title) m_pProject->SetTitle(cbC2U(title));

    // parse all global parameters
    const TiXmlElement* prop = root->FirstChildElement("PropertyGroup");
    for (; prop; prop=prop->NextSiblingElement("PropertyGroup"))
    {
        const char* attr = prop->Attribute("Label");
        if (!attr) continue;

        wxString label = cbC2U(attr);
        if (label.IsSameAs(_T("Globals"),false))
        {
            const TiXmlElement* pProjectName = prop->FirstChildElement("ProjectName"); // new
            if (!pProjectName)  pProjectName = prop->FirstChildElement("RootNamespace"); // old
            if (pProjectName) m_ProjectName = GetText(pProjectName);

            const TiXmlElement* pGUID = prop->FirstChildElement("ProjectGuid");
            if (pGUID) m_ProjectGUID = GetText(pGUID);

            const TiXmlElement* pProjectType = prop->FirstChildElement("Keyword");
            if (pProjectType) m_ProjectType = GetText(pProjectType);

            // logging
            pMsg->DebugLog(wxString::Format(_("Project global properties: GUID=%s, Type=%s, Name=%s"),
                                             m_ProjectGUID.wx_str(), m_ProjectType.wx_str(), m_ProjectName.wx_str()));

            bResult = true; // got everything we need
            break; // exit loop
        }
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find global project properties, using default one."));

    m_pProject->SetTitle(m_ProjectName);
    return bResult;
}

bool MSVC10Loader::GetProjectConfigurations(const TiXmlElement* root)
{
    // delete all targets of the project (we'll create new ones from the imported configurations)
    while (m_pProject && m_pProject->GetBuildTargetsCount())
        m_pProject->RemoveBuildTarget(0);

    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // first we try to get the list of configurations : there is normally a specific chapter for that
    // this is not truly necessary, but it is cleaner like that:
    // the plugin will be easier to understand, and easier to extend if necessary
    const TiXmlElement* prop = root->FirstChildElement("ItemGroup");
    for (; prop; prop=prop->NextSiblingElement("ItemGroup"))
    {
        const char* attr = prop->Attribute("Label");
        if (!attr) continue;

        wxString label = cbC2U(attr);
        if (label.IsSameAs(_T("ProjectConfigurations"),false))
        {
            const TiXmlElement* conf = prop->FirstChildElement("ProjectConfiguration");
            for (; conf; conf=conf->NextSiblingElement("ProjectConfiguration"))
            {
                // loop over all the configurations
                const char*         name = conf->Attribute("Include");
                const TiXmlElement* cfg  = conf->FirstChildElement("Configuration");
                const TiXmlElement* plat = conf->FirstChildElement("Platform");
                if (name && cfg && plat)
                {
                    SProjectConfiguration pc;
                    pc.bt           = NULL;
                    // ProjectConfiguration
                    pc.sName        = cbC2U(name); pc.sName.Replace(_T("|"), _T(" "));
                    pc.sConf        = GetText(cfg);
                    pc.sPlatform    = GetText(plat);
                    // PropertyGroup
                    pc.TargetType   = _T("Application");
//                    pc.UseDebugLibs = _T("true");
                    pc.Charset      = _T("NotSet");
                    pc.bIsDefault   = false;
                    pc.bNoImportLib = -1; // unset, use global
                    m_pc[pc.sName]  = pc;

                    pMsg->DebugLog(_("Found project configuration: ") + pc.sName);

                    bResult = true;
                }
            }
        }
    }

    if (!bResult)
    {
        pMsg->DebugLog(_("Failed to find project configurations."));
        return false;
    }

    // now that we have (in theory) the list of configurations, we will parse each configuration
    bResult = GetConfiguration(root);
    // set empty values to default value and replace macros
    for (HashProjectsConfs::iterator it=m_pc.begin(); it!=m_pc.end(); ++it)
    {
        SProjectConfiguration& pc = it->second;
        if (pc.sOutDir.IsEmpty())     pc.sOutDir = m_OutDir.IsEmpty() ? wxString(_T("$(SolutionDir)$(Configuration)"))+wxFILE_SEP_PATH : m_OutDir;
        if (pc.sIntDir.IsEmpty())     pc.sIntDir = m_IntDir.IsEmpty() ? wxString(_T("$(Configuration)"))+wxFILE_SEP_PATH : m_IntDir;
        if (pc.sTargetName.IsEmpty()) pc.sTargetName = _T("$(ProjectName)");

        if (pc.sTargetExt.IsEmpty())
        {
            if (pc.TargetType.IsSameAs(_T("DynamicLibrary"), false))
                pc.sTargetExt =! m_ConvertSwitches ? _T(".dll") : _T(".so");
            else if (pc.TargetType.IsSameAs(_T("StaticLibrary"),  false))
                pc.sTargetExt =! m_ConvertSwitches ? _T(".lib") : _T(".a");
            else
                pc.sTargetExt =! m_ConvertSwitches ? _T(".exe") : _T("");
        }
        if (pc.bNoImportLib==-1)
            pc.bNoImportLib=m_NoImportLib;
//        if (pc.sExePath.IsEmpty())
//            pc.sExePath=_T("");
//        if (pc.sSourcePath.IsEmpty())
//            pc.sSourcePath=_T("");
        ReplaceConfigMacros(pc, pc.sOutDir);
        ReplaceConfigMacros(pc, pc.sIntDir);
        ReplaceConfigMacros(pc, pc.sTargetName);
        // crap (currently useless, probably never needed)
        ReplaceConfigMacros(pc, pc.sExePath);
        ReplaceConfigMacros(pc, pc.sSourcePath);
    }

    m_pProject->SetTitle(m_ProjectName);

    return true;
}

bool MSVC10Loader::GetConfigurationName(const TiXmlElement* e, wxString& config, const wxString& defconfig)
{
    if (!defconfig.IsEmpty())
        config = defconfig;
    else
    {
        const char* name = e->Attribute("Condition");
        if (name)
        {
            config = SubstituteConfigMacros(cbC2U(name));
            if (!m_pc.count(config))
                return false;
        }
    }
    return true;
}

void MSVC10Loader::SetConfigurationValues(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, wxString* globaltarget)
{
    wxString config;
    for (const TiXmlElement* e=root->FirstChildElement(key); e; e=e->NextSiblingElement(key))
    {
        if (!GetConfigurationName(e,config,defconfig))
            continue;

        wxString* value;
        if (config.IsEmpty())
        {
            if (!globaltarget) continue;
            value = globaltarget;
        }
        else
            value = (wxString*)((char*)&m_pc[config]+target);

        *value = GetText(e);
    }
}

void MSVC10Loader::SetConfigurationValuesPath(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, wxString* globaltarget)
{
    wxString config;
    for (const TiXmlElement* e=root->FirstChildElement(key); e; e=e->NextSiblingElement(key))
    {
        if (!GetConfigurationName(e,config,defconfig))
          continue;

        wxString* value;
        if (config.IsEmpty())
        {
            if (!globaltarget) continue;
            value = globaltarget;
        }
        else
            value = (wxString*)((char*)&m_pc[config]+target);

        *value = UnixFilename(GetText(e));
        if ((*value).Last()!=wxFILE_SEP_PATH)
            *value += wxFILE_SEP_PATH;
    }
}

void MSVC10Loader::SetConfigurationValuesBool(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, char* globaltarget)
{
    wxString config;
    for (const TiXmlElement* e=root->FirstChildElement(key); e; e=e->NextSiblingElement(key))
    {
        if (!GetConfigurationName(e,config,defconfig))
            continue;

        char* value;
        if (config.IsEmpty())
        {
            if (!globaltarget) continue;
            value = globaltarget;
        }
        else
            value = (char*)((char*)&m_pc[config]+target);

        wxString val = GetText(e);
        if (!val.IsEmpty() && ((val.IsSameAs(_T("true"),false)) || val.IsSameAs(_T("1"))))
            *value = true;
        else
            *value = false;
    }
}

bool MSVC10Loader::GetConfiguration(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    // now that we have (in theory) the list of configurations, we will parse each configuration
    const TiXmlElement* prop = root->FirstChildElement("PropertyGroup");
    if (!prop) return false;
    for (; prop; prop=prop->NextSiblingElement("PropertyGroup"))
    {
        const char* attr = prop->Attribute("Label");
        if (attr && !cbC2U(attr).IsSameAs(_T("Configuration"),false))
            continue;

        wxString config;
        if (!GetConfigurationName(prop,config,wxEmptyString))
            continue;
        if (!config.IsEmpty())
        {
            const TiXmlElement* e;
            e = prop->FirstChildElement("ConfigurationType");
            if (e) m_pc[config].TargetType = GetText(e);
            e = prop->FirstChildElement("CharacterSet");
            if (e) m_pc[config].Charset = GetText(e);
//            e = prop->FirstChildElement("UseDebugLibraries");
//            if(e) m_pc[config].UseDebugLibs=GetText(e);
        }

        // By default, OutDir is $(SolutionDir)$(Configuration)
        SetConfigurationValuesPath(prop,"OutDir",offsetof(SProjectConfiguration,sOutDir),config,&m_OutDir);

        // By default, IntDir is $(Configuration)
        SetConfigurationValuesPath(prop,"IntDir",offsetof(SProjectConfiguration,sIntDir),config,&m_IntDir);

        // By default, IgnoreImportLibrary is false
        SetConfigurationValuesBool(prop,"IgnoreImportLibrary",offsetof(SProjectConfiguration,bNoImportLib),config,&m_NoImportLib);

        // By default, TargetName is $(ProjectName)
        SetConfigurationValues(prop,"TargetName",offsetof(SProjectConfiguration,sTargetName),config);

        // By default, TargetExt is .exe
        SetConfigurationValues(prop,"TargetExt",offsetof(SProjectConfiguration,sTargetExt),config);

        // $(VCInstallDir)bin , $(WindowsSdkDir)bin\NETFX 4.0 Tools , $(WindowsSdkDir)bin , $(VSInstallDir)Common7\Tools\bin ,
        // $(VSInstallDir)Common7\tools , $(VSInstallDir)Common7\ide , $(ProgramFiles)\HTML Help Workshop , $(FrameworkSDKDir)\bin ,
        // $(MSBuildToolsPath32) , $(VSInstallDir) , $(SystemRoot)\SysWow64 , $(FxCopDir) , $(PATH)
        SetConfigurationValuesPath(prop,"ExecutablePath",offsetof(SProjectConfiguration,sExePath),config);

        // $(VCInstallDir)atlmfc\src\mfc , $(VCInstallDir)atlmfc\src\mfcm , $(VCInstallDir)atlmfc\src\atl , $(VCInstallDir)crt\src
        SetConfigurationValuesPath(prop,"SourcePath",offsetof(SProjectConfiguration,sSourcePath),config);
    }
    return true;
}

bool MSVC10Loader::GetProjectConfigurationFiles(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse each ItemGroup
    const TiXmlElement* prop = root->FirstChildElement("ItemGroup");
    for (; prop; prop=prop->NextSiblingElement("ItemGroup"))
    {
        const TiXmlElement* none = prop->FirstChildElement("None");
        for (; none; none=none->NextSiblingElement("None"))
        {
            const char* attr = none->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), false, false);
                HandleFilesAndExcludes(none, pf);
            }
            bResult = true; // at least one file imported
        }

        const TiXmlElement* incl = prop->FirstChildElement("ClInclude");
        for (; incl; incl=incl->NextSiblingElement("ClInclude"))
        {
            const char* attr = incl->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), false, false);
                HandleFilesAndExcludes(incl, pf);
            }
            bResult = true; // at least one file imported
        }

        const TiXmlElement* comp = prop->FirstChildElement("ClCompile");
        for (; comp; comp=comp->NextSiblingElement("ClCompile"))
        {
            const char* attr = comp->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), true, true);
                HandleFilesAndExcludes(comp, pf);
            }
            bResult = true; // at least one file imported
        }

        const TiXmlElement* res = prop->FirstChildElement("ResourceCompile");
        for (; res; res=res->NextSiblingElement("ResourceCompile"))
        {
            const char* attr = res->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), true, true);
                HandleFilesAndExcludes(res, pf);
            }
            bResult = true; // at least one file imported
        }
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any files in the project...?!"));

    return bResult;
}

bool MSVC10Loader::GetProjectIncludes(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse all global parameters
    const TiXmlElement* prop = root->FirstChildElement("PropertyGroup");
    for (; prop; prop=prop->NextSiblingElement("PropertyGroup"))
    {
        const char* attr = prop->Attribute("Condition");
        if (!attr) continue;

        wxString conf = cbC2U(attr);
        for (HashProjectsConfs::iterator it=m_pc.begin(); it!=m_pc.end(); ++it)
        {
            wxString sName = it->second.sName;
            wxString sConf = SubstituteConfigMacros(conf);
            if (sConf.IsSameAs(sName))
            {
                // $(VCInstallDir)include , $(VCInstallDir)atlmfc\include , $(WindowsSdkDir)include , $(FrameworkSDKDir)\include
                const TiXmlElement* cinc = prop->FirstChildElement("IncludePath");
                wxArrayString cdirs = GetArrayPaths(cinc, m_pc[sName]);
                for (size_t j=0; j<cdirs.Count(); ++j)
                {
                    ProjectBuildTarget* bt = m_pc[sName].bt;
                    if (bt) bt->AddIncludeDir(cdirs.Item(j));
                }
                // $(VCInstallDir)lib , $(VCInstallDir)atlmfc\lib , $(WindowsSdkDir)lib , $(FrameworkSDKDir)\lib
                const TiXmlElement* linc = prop->FirstChildElement("LibraryPath");
                wxArrayString ldirs = GetArrayPaths(linc, m_pc[sName]);
                for (size_t j=0; j<ldirs.Count(); ++j)
                {
                    ProjectBuildTarget* bt = m_pc[sName].bt;
                    if (bt) bt->AddLibDir(ldirs.Item(j));
                }
                bResult = true; // got something
            }
        }
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any includes in the project...?!"));

    return bResult;
}

bool MSVC10Loader::GetTargetSpecific(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse all global parameters
    const TiXmlElement* idef = root->FirstChildElement("ItemDefinitionGroup");
    for (; idef; idef=idef->NextSiblingElement("ItemDefinitionGroup"))
    {
        const char* attr = idef->Attribute("Condition");
        if (!attr) continue;

        wxString conf = cbC2U(attr);
        for (HashProjectsConfs::iterator it=m_pc.begin(); it!=m_pc.end(); ++it)
        {
            wxString sName = it->second.sName;
            wxString sConf = SubstituteConfigMacros(conf);
            if (sConf.IsSameAs(sName))
            {
                assert(m_pc[sName].bt);
                if (!m_pc[sName].bt)
                    continue;

                ProjectBuildTarget* bt = m_pc[sName].bt;
                if (!m_ConvertSwitches && !m_pc[sName].Charset.IsEmpty())
                {
                    if (m_pc[sName].Charset.IsSameAs(_T("NotSet"),false))
                        ; // nop
                    else if (m_pc[sName].Charset.IsSameAs(_T("Unicode"),false))
                        bt->AddCompilerOption(_T("/D_UNICODE /DUNICODE"));
                    else if (m_pc[sName].Charset.IsSameAs(_T("MultiByte"),false))
                        bt->AddCompilerOption(_T("/D_MBCS"));
                    else
                        pMsg->DebugLog(_("Import; Unsupported CharacterSet: ") + m_pc[sName].Charset);
                }
                if (!m_pc[sName].sIntDir.IsEmpty())
                    bt->SetObjectOutput(m_pc[sName].sIntDir);

                if (!m_pc[sName].sOutDir.IsEmpty())
                {
                    bt->SetOutputFilename(m_pc[sName].sOutDir+m_pc[sName].sTargetName+m_pc[sName].sTargetExt);
                    bt->SetDepsOutput(m_pc[sName].sOutDir);
                }

                if (!m_pc[sName].sConf.IsEmpty())
                {
                    if (m_pc[sName].sConf.IsSameAs(_T("Release"), false))
                    {
                        // nop
                    }
                    else if (m_pc[sName].sConf.IsSameAs(_T("Debug"),false))
                        bt->AddCompilerOption(!m_ConvertSwitches ? _T("/Zi") : _T("-g"));
                    else
                        pMsg->DebugLog(_("Import; Unsupported Configuration: ") + m_pc[sName].sConf);
                }

                if (m_pc[sName].bNoImportLib)
                {
                    bt->SetCreateDefFile(false);
                    bt->SetCreateStaticLib(false);
                }

                const TiXmlElement* comp = idef->FirstChildElement("ClCompile");
                if (comp)
                {
                    const TiXmlElement* pp = comp->FirstChildElement("PreprocessorDefinitions");
                    wxArrayString pps = GetArray(pp);
                    for (size_t j=0; j<pps.Count(); ++j)
                        bt->AddCompilerOption((m_ConvertSwitches ? _T("-D") : _T("/D")) + pps.Item(j));

                    const TiXmlElement* cinc = comp->FirstChildElement("AdditionalIncludeDirectories");
                    wxArrayString cdirs = GetArrayPaths(cinc, m_pc[sName]);
                    for (size_t j=0; j<cdirs.Count(); ++j)
                        bt->AddIncludeDir(cdirs.Item(j));

                    const TiXmlElement* copt = comp->FirstChildElement("AdditionalOptions");
                    wxArrayString copts = GetArray(copt,_T(" "));
                    if (!m_ConvertSwitches)
                    {
                        for (size_t j=0; j<copts.Count(); ++j)
                            bt->AddCompilerOption(copts.Item(j));
                    }

                    if ((copt=comp->FirstChildElement("Optimization")))
                    {
                        wxString val = GetText(copt);
                        if (val.IsSameAs(_T("Disabled"),false))
                            bt->AddCompilerOption(!m_ConvertSwitches ? _T("/Od") : _T("-O0"));
                        else if (val.IsSameAs(_T("MinSpace"), false))
                        {
                            if (!m_ConvertSwitches) bt->AddCompilerOption(_T("/O1"));
                            else
                            {
                                bt->AddLinkerOption(_T("-s"));
                                bt->AddCompilerOption(_T("-Os"));
                            }
                        }
                        else if (val.IsSameAs(_T("MaxSpeed"),false))
                        {
                            if (!m_ConvertSwitches) bt->AddCompilerOption(_T("/O2"));
                            else
                            {
                                bt->AddLinkerOption(_T("-s"));
                                bt->AddCompilerOption(_T("-O1"));
                            }
                        }
                        else if (val.IsSameAs(_T("Full"),false))
                        {
                            if (!m_ConvertSwitches) bt->AddCompilerOption(_T("/Ox"));
                            else
                            {
                                bt->AddLinkerOption(_T("-s"));
                                bt->AddCompilerOption(_T("-O2"));
                            }
                        }
                        else
                            pMsg->DebugLog(_("Import; Unsupported Optimization: ") + val+_T("\n"));
                    }
                    if (!m_ConvertSwitches && (copt=comp->FirstChildElement("RuntimeLibrary")))
                    {
                        wxString val = GetText(copt);
                        if (val.IsSameAs(_T("MultiThreaded"),false))
                            bt->AddCompilerOption(_T("/MT"));
                        else if (val.IsSameAs(_T("MultiThreadedDebug"),false))
                            bt->AddCompilerOption(_T("/MTd"));
                        else if (val.IsSameAs(_T("MultiThreadedDll"),false))
                            bt->AddCompilerOption(_T("/MD"));
                        else if(val.IsSameAs(_T("MultiThreadedDebugDll"),false))
                            bt->AddCompilerOption(_T("/MDd"));
                        else
                            pMsg->DebugLog(_("Import; Unsupported RuntimeLibrary: ")+val);
                    }
                    if ((copt=comp->FirstChildElement("WarningLevel")))
                    {
                        wxString val = GetText(copt);
                        if (val.IsSameAs(_T("Level1"),false))
                        {   if (!m_ConvertSwitches) bt->AddCompilerOption(_T("/W1")); }
                        else if (val.IsSameAs(_T("Level2"),false))
                            bt->AddCompilerOption(!m_ConvertSwitches ? _T("/W2") : _T("-Wall"));
                        else if (val.IsSameAs(_T("Level3"),false))
                            bt->AddCompilerOption(!m_ConvertSwitches ? _T("/W3") : _T("-Wall"));
                        else if (val.IsSameAs(_T("Level4"),false))
                        {
                            if (!m_ConvertSwitches) bt->AddCompilerOption(_T("/W4"));
                            else
                            {
                                bt->AddCompilerOption(_T("-Wall"));
                                bt->AddCompilerOption(_T("-Wextra"));
                            }
                        }
                        else
                            pMsg->DebugLog(_("Import; Unsupported WarningLevel: ") + val);
                    }
                    if (!m_ConvertSwitches && (copt=comp->FirstChildElement("DisableSpecificWarnings")))
                    {
                        wxArrayString warns = GetArray(copt);
                        for (size_t j=0; j<warns.Count(); ++j)
                            bt->AddCompilerOption(_T("/wd") + warns.Item(j));
                    }
                }

                const TiXmlElement* res = idef->FirstChildElement("ResourceCompile");
                if (res)
                {
                    const TiXmlElement* pp = res->FirstChildElement("PreprocessorDefinitions");
                    wxArrayString pps = GetArray(pp);
                    for (size_t j=0; j<pps.Count(); ++j)
                        bt->AddCompilerOption((m_ConvertSwitches ? _T("-D") : _T("/D")) + pps.Item(j));

                    const TiXmlElement* cinc = res->FirstChildElement("AdditionalIncludeDirectories");
                    wxArrayString cdirs = GetArrayPaths(cinc,m_pc[sName]);
                    for (size_t j=0; j<cdirs.Count(); ++j)
                        bt->AddResourceIncludeDir(cdirs.Item(j));

                    const TiXmlElement* copt = res->FirstChildElement("AdditionalOptions");
                    wxArrayString copts = GetArray(copt,_T(" "));
                    if (!m_ConvertSwitches)
                    {
                        for (size_t j=0; j<copts.Count(); ++j)
                            bt->AddCompilerOption(copts.Item(j));
                    }
                }

                const TiXmlElement* link = idef->FirstChildElement("Link");
                if (link)
                {
                    const TiXmlElement* copt;
                    if ((copt=link->FirstChildElement("OutputFile")))
                    {
                        wxString val = GetText(copt);
                        ReplaceConfigMacros(m_pc[sName],val);
                        if (!val.IsEmpty())
                            bt->SetOutputFilename(val);
                    }
                    if ((copt=link->FirstChildElement("ModuleDefinitionFile")))
                    {
                        wxString val = GetText(copt);
                        ReplaceConfigMacros(m_pc[sName],val);
                        if (!val.IsEmpty())
                            bt->SetDefinitionFileFilename(val);
                    }
                    if ((copt=link->FirstChildElement("ImportLibrary")))
                    {
                        wxString val=GetText(copt);
                        ReplaceConfigMacros(m_pc[sName],val);
                        if (!val.IsEmpty())
                            bt->SetImportLibraryFilename(val);
                    }

                    copt = link->FirstChildElement("AdditionalDependencies");
                    wxArrayString libs = GetLibs(copt);
                    for (size_t j=0; j<libs.Count(); ++j)
                        bt->AddLinkLib(libs.Item(j));

                    copt = link->FirstChildElement("AdditionalLibraryDirectories"); /// @note : maybe use loops on all elements
                    for (; copt; copt=copt->NextSiblingElement("AdditionalLibraryDirectories"))
                    {
                        wxArrayString ldirs = GetArrayPaths(copt,m_pc[sName]);
                        for (size_t j=0; j<ldirs.Count(); ++j)
                            bt->AddLibDir(ldirs.Item(j));
                    }

                    if (!m_ConvertSwitches)
                    {
                        copt = link->FirstChildElement("AdditionalOptions");
                        wxArrayString lopts = GetArray(copt,_T(" "));
                        for (size_t j=0; j<lopts.Count(); ++j)
                            bt->AddLinkerOption(lopts.Item(j));

                        copt = link->FirstChildElement("GenerateDebugInformation");
                        wxString sDebug = GetText(copt);
                        if (sDebug.IsSameAs(_T("true"),false))
                            bt->AddLinkerOption(_T("/debug"));
                    }
                }

                const TiXmlElement* event;
                if ((event=idef->FirstChildElement("PreBuildEvent")))
                {
                    const TiXmlElement* copt=event->FirstChildElement("Command");
                    for (; copt; copt=copt->NextSiblingElement("Command"))
                    {
                        wxString cmd = UnixFilename(GetText(copt));
                        ReplaceConfigMacros(m_pc[sName],cmd);
                        if (!cmd.IsEmpty()) bt->AddCommandsBeforeBuild(cmd);
                    }
                }
                if ((event=idef->FirstChildElement("PostBuildEvent")))
                {
                    const TiXmlElement* copt = event->FirstChildElement("Command");
                    for (; copt; copt=copt->NextSiblingElement("Command"))
                    {
                        wxString cmd=UnixFilename(GetText(copt));
                        ReplaceConfigMacros(m_pc[sName],cmd);
                        if (!cmd.IsEmpty()) bt->AddCommandsAfterBuild(cmd);
                    }
                }

                bResult = true; // got something
            }
        }
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any includes in the project...?!"));

    return bResult;
}

bool MSVC10Loader::DoSelectConfigurations()
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    if ( ImportersGlobals::ImportAllTargets ) // by default, all targets are imported
        return true;

    // ask the user to select a configuration - multiple choice ;)
    wxArrayString configurations;
    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
        configurations.Add(it->second.sName);

    MultiSelectDlg dlg(0, configurations, true, _("Select configurations to import:"), m_pProject->GetTitle());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_CANCEL)
    {
        pMsg->DebugLog(_("Cancelled.."));
        return false;
    }

    wxArrayString asSelectedStrings = dlg.GetSelectedStrings();
    if (asSelectedStrings.GetCount() < 1)
    {
        pMsg->DebugLog(_("No selection -> cancelled."));
        return false;
    }

    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); )
    {
        if (asSelectedStrings.Index(it->second.sName)==wxNOT_FOUND)
            m_pc.erase(it++); // remove deselected
        else
            ++it;
    }

    return true;
}

bool MSVC10Loader::DoCreateConfigurations()
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // create the project targets
    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
    {
        ProjectBuildTarget* bt = m_pProject->AddBuildTarget(it->second.sName);
        if (bt)
        {
            bt->SetCompilerID(m_pProject->GetCompilerID());
            bt->AddPlatform(spAll); // target all platforms, SupportedPlatforms enum in "globals.h"

            TargetType tt = ttExecutable;
            if      (it->second.TargetType == _T("Application"))    tt = ttExecutable;
            else if (it->second.TargetType == _T("Console"))        tt = ttConsoleOnly;
            else if (it->second.TargetType == _T("StaticLibrary"))  tt = ttStaticLib;
            else if (it->second.TargetType == _T("DynamicLibrary")) tt = ttDynamicLib;
            else
                pMsg->DebugLog(_("Import; Unsupported target type: ") + it->second.TargetType);

            bt->SetTargetType(tt); // executable by default, TargetType enum in "globals.h"
            it->second.bt = bt; // apply

            pMsg->DebugLog(_("Created project build target: ") + it->second.sName);

            bResult = true; // at least one config imported
        }
    }

    return bResult;
}

wxString MSVC10Loader::GetText(const TiXmlElement* e)
{
    // convenience function for getting XML text
    wxString sResult = _T("");
    if (e)
    {
        const TiXmlNode* child = e->FirstChild();
        if (child)
        {
            const TiXmlText* childText = child->ToText();
            if (childText && childText->Value())
                sResult = cbC2U(childText->Value());
        }
    }

    return sResult;
}

void MSVC10Loader::HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf)
{
    if (!e || !pf)
        return;

    // add it to all configurations, not just the first
    for (HashProjectsConfs::iterator it=m_pc.begin(); it!=m_pc.end(); ++it)
        pf->AddBuildTarget(it->second.sName);

    // handle explicit exclusions like:
    // <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">true</ExcludedFromBuild>
    // <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">true</ExcludedFromBuild>
    const TiXmlElement* excl = e->FirstChildElement("ExcludedFromBuild");
    for (; excl; excl=excl->NextSiblingElement("ExcludedFromBuild"))
    {
        const TiXmlText* do_excl = excl->ToText();
        if (do_excl)
        {
            const char* value = do_excl->Value();
            wxString  s_value = cbC2U(value);
            if (s_value.IsSameAs(_T("true"), false))
            {
                const char* cond = excl->Attribute("Condition");
                if (cond)
                {
                    wxString sName = cbC2U(cond); sName = SubstituteConfigMacros(sName);
                    pf->RemoveBuildTarget(sName);
                }
            }
        }
    }
}

wxArrayString MSVC10Loader::GetArrayPaths(const TiXmlElement* e, const SProjectConfiguration &pc)
{
    wxArrayString sResult;
    if (e)
    {
        wxString val = GetText(e);
        // Specific: for ItemGroups (not Dollar, but percentage)
        ReplaceConfigMacros(pc,val);
        val.Replace(_T("%(AdditionalIncludeDirectories)"), wxEmptyString); // not supported
        val.Replace(_T("%(AdditionalLibraryDirectories)"), wxEmptyString); // not supported
        if (!val.IsEmpty())
        {
            wxArrayString aVal = GetArrayFromString(val, _T(";"));
            for (size_t i=0; i<aVal.Count(); ++i)
            {
                val = aVal.Item(i);
                if (!val.Trim().IsEmpty())
                {
                    val = UnixFilename(val);
                    if (val.Last()!=wxFILE_SEP_PATH)
                        val += wxFILE_SEP_PATH;
                    sResult.Add(val);
                }
            }
        }
    }
    return sResult;
}

wxArrayString MSVC10Loader::GetArray(const TiXmlElement* e, wxString delim)
{
    wxArrayString sResult;
    if (e)
    {
        wxString val = GetText(e);
        // Specific: for ItemGroups (not Dollar, but percentage)
        val.Replace(_T("%(PreprocessorDefinitions)"), wxEmptyString); // not supported
        val.Replace(_T("%(AdditionalOptions)"), wxEmptyString); // not supported
        val.Replace(_T("%(DisableSpecificWarnings)"), wxEmptyString); // not supported
        if (!val.IsEmpty()){
            wxArrayString aVal = GetArrayFromString(val, delim);
            for (size_t i=0; i<aVal.Count(); ++i)
            {
                val = aVal.Item(i);
                if (!val.Trim().IsEmpty())
                    sResult.Add(val);
            }
        }
    }
    return sResult;
}

wxArrayString MSVC10Loader::GetLibs(const TiXmlElement* e)
{
    wxArrayString sResult;
    wxString val;
    if (e)
        val = GetText(e);
    else
        val=_T("%(AdditionalDependencies)");
    // Specific: for ItemGroups (not Dollar, but percentage)
    val.Replace(_T("%(AdditionalDependencies)"), g_AdditionalDependencies);
    if (!val.IsEmpty())
    {
        wxArrayString aVal = GetArrayFromString(val, _T(";"));
        for (size_t i=0; i<aVal.Count(); ++i)
        {
            val = aVal.Item(i);
            if (!val.Trim().IsEmpty())
                sResult.Add(val);
        }
    }
    return sResult;
}

wxString MSVC10Loader::ReplaceMSVCMacros(const wxString& str)
{
    wxString ret = str;
    ret.Replace(_T("$(SolutionDir)"),m_WorkspacePath);
    ret.Replace(_T("$(ProjectDir)"),_T("")); // use relative and not m_pProject->GetBasePath()
    ret.Replace(_T("$(ProfileDir)"),_T("")); // same as above, seen at least once in MSVC 10-12
    ret.Replace(_T("$(ProjectName)"),m_ProjectName);
    ret.Replace(_T("$(ConfigurationName)"),m_ConfigurationName);
    ret.Replace(_T("$(PlatformName)"),m_PlatformName);
    ret.Replace(_T("$(TargetPath)"),m_TargetPath);
    ret.Replace(_T("$(TargetFileName)"),m_TargetFilename);
//    ret.Replace(_T("\""),_T(""));
    return ret;
}

void MSVC10Loader::ReplaceConfigMacros(const SProjectConfiguration &pc, wxString &str)
{
    str.Replace(_T("$(Configuration)"),pc.sConf);
    str.Replace(_T("$(Platform)"),pc.sPlatform);

    str.Replace(_T("$(OutDir)"),pc.sOutDir);
    str.Replace(_T("$(IntDir)"),pc.sIntDir);
    str.Replace(_T("$(TargetName)"),pc.sTargetName);
    str.Replace(_T("$(TargetExt)"),pc.sTargetExt);
    str = ReplaceMSVCMacros(str);
}

wxString MSVC10Loader::SubstituteConfigMacros(const wxString& sString)
{
    wxString sResult(sString);

    sResult.Replace(_T("$(Configuration)"), _T(""));
    sResult.Replace(_T("$(Platform)"),      _T(""));
    sResult.Replace(_T("=="),               _T(""));
    sResult.Replace(_T("\'"),               _T(""));
    sResult.Replace(_T("|"),                _T(" "));
    sResult.Trim(false);

    return sResult;
}

bool MSVC10Loader::Save(cb_unused const wxString& filename)
{
    // no support to save MSVC10 projects
    return false;
}
