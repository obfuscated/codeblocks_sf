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
#include "msvc10loader.h"
#include "multiselectdlg.h"
#include "importers_globals.h"

/** Remarks:
    This class is as close as possible to the MSVC6 and MSVC7 loaders.
    The similarity will help maintenance. However, "similar" does not mean "the same as"

    The results configuration parsing is stored in the member m_pc
    This hash map contains all the target information: Platform, Debug/Release, DLL/Lib/Executable, ...
    A CodeBlock target is created for each element in this hash map.
    Please note that some members are unused by CodeBlocks, such as Charset or UseDebugLibs
**/

MSVC10Loader::MSVC10Loader(cbProject* project) :
    m_pProject(project),
    m_ConvertSwitches(false),
    m_ProjectGUID(_T("")),
    m_ProjectType(_T("")),
    m_ProjectName(_T(""))
{
    //ctor
    if (platform::windows)
        m_PlatformName = _T("Win32");
    else if (platform::linux)
        m_PlatformName = _T("Linux");
    else if (platform::macosx)
        m_PlatformName = _T("MacOSX");
    else
        m_PlatformName = _T("Unknown");

    m_pc.clear();
}

MSVC10Loader::~MSVC10Loader()
{
    //dtor
}

bool MSVC10Loader::Open(const wxString& filename)
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    m_ConvertSwitches = m_pProject->GetCompilerID().IsSameAs(_T("gcc"));
    m_ProjectName = wxFileName(filename).GetName();

    pMsg->DebugLog(F(_("Importing MSVC 10.xx project: %s"), filename.wx_str()));

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

    bool bResult = GetProjectGlobals(root) // get project name & type
                   // get the project list of configuration => 1 configuration = 1 build target in CodeBlocks
                && GetProjectConfigurations(root);

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

              // get the project list of files and add them to the targets
    bResult = GetProjectConfigurationFiles(root)
              // get the project/target list of includes and add them to the targets
           && GetProjectIncludes(root)
              // get the project/target specific settings
           && GetTargetSpecific(root);

    return bResult;
}

/** get project name, type and GUID
  * \param root : the root node of the XML project file (<Project >
  **/
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
    while (prop)
    {
        const char* attr = prop->Attribute("Label");
        if (!attr) { prop = prop->NextSiblingElement(); continue; }

        wxString label = cbC2U(attr);
        if (label.MakeUpper().IsSameAs(_T("GLOBALS")))
        {
            const TiXmlElement* pGUID = prop->FirstChildElement("ProjectGuid");
            if (pGUID) m_ProjectGUID = GetText(pGUID);

            const TiXmlElement* pProjectType = prop->FirstChildElement("Keyword");
            if (pProjectType) m_ProjectType = GetText(pProjectType);

            const TiXmlElement* pProjectName = prop->FirstChildElement("RootNamespace");
            if (pProjectName) m_ProjectName = GetText(pProjectName);

            // logging
            #if wxCHECK_VERSION(2, 9, 0)
            pMsg->DebugLog(wxString::Format(_("Project global properties: GUID=%s, Type=%s, Name=%s"),
                                             m_ProjectGUID.wx_str(), m_ProjectType.wx_str(), m_ProjectName.wx_str()));
            #else
            pMsg->DebugLog(wxString::Format(_("Project global properties: GUID=%s, Type=%s, Name=%s"),
                                             m_ProjectGUID.c_str(), m_ProjectType.c_str(), m_ProjectName.c_str()));
            #endif

            bResult = true; // got everything we need
            break; // exit loop
        }

        prop = prop->NextSiblingElement();
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find global project properties, using default one."));

    m_pProject->SetTitle(m_ProjectName);
    return bResult;
}

/** get the list of configurations in the project
  * For each configuration found, a target will be created
  * \param root : the root node of the XML project file (<Project >
  **/
bool MSVC10Loader::GetProjectConfigurations(const TiXmlElement* root)
{
    // delete all targets of the project (we 'll create new ones from the imported configurations)
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
    while (prop)
    {
        const char* attr = prop->Attribute("Label");
        if (!attr) { prop = prop->NextSiblingElement(); continue; }

        wxString label = cbC2U(attr);
        if (label.MakeUpper().IsSameAs(_T("PROJECTCONFIGURATIONS")))
        {
            const TiXmlElement* conf = prop->FirstChildElement("ProjectConfiguration");
            while (conf)
            {
                // loop over all the configurations
                const char*         name = conf->Attribute("Include");
                const TiXmlElement* cfg  = conf->FirstChildElement("Configuration");
                const TiXmlElement* plat = conf->FirstChildElement("Platform");
                if (name && cfg && plat)
                {
                    wxString sName = cbC2U(name); sName.Replace(_T("|"), _T(" "));
                    wxString sConf = GetText(cfg);
                    wxString sPlat = GetText(plat);

                    if (m_pcNames.Index(sName)==wxNOT_FOUND) m_pcNames.Add(sName);

                    SProjectConfiguration pc;
                    pc.bt           = NULL;
                    pc.sName        = sName;
                    pc.sPlatform    = sPlat;
                    pc.sConf        = sConf;
                    pc.TargetType   = _T("Application");
                    pc.UseDebugLibs = _T("true");
                    pc.bIsDefault   = false;
                    pc.bImport      = true;
                    m_pc[sName]     = pc;

                    pMsg->DebugLog(_("Found project configuration: ") + sName);

                    bResult = true;
                }
                conf = conf->NextSiblingElement();
            }
        }
        prop = prop->NextSiblingElement();
    }

    if (!bResult)
    {
        pMsg->DebugLog(_("Failed to find project configurations."));
        return false;
    }

    // now that we have (in theory) the list of configurations, we will parse each configuration
    bResult &= GetConfiguration(root);

    m_pProject->SetTitle(m_ProjectName);

    return true;
}

/** get the configuration in the project
  * \param root : the root node of the XML project file (<Project >
  **/
bool MSVC10Loader::GetConfiguration(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // now that we have (in theory) the list of configurations, we will parse each configuration
    const TiXmlElement* prop = root->FirstChildElement("PropertyGroup");
    while (prop)
    {
        const char* attr = prop->Attribute("Label");
        if (!attr) { prop = prop->NextSiblingElement(); continue; }

        wxString label = cbC2U(attr);
        if (label.MakeUpper().IsSameAs(_T("CONFIGURATION")))
        {
            const char*         name = prop->Attribute("Condition");
            const TiXmlElement* type = prop->FirstChildElement("ConfigurationType");
            const TiXmlElement* dbg  = prop->FirstChildElement("UseDebugLibraries");
            const TiXmlElement* cset = prop->FirstChildElement("CharacterSet");
            if (name && type && dbg && cset)
            {
                wxString sName = cbC2U(name); sName = SubstituteConfigMacros(sName);
                if (m_pcNames.Index(sName)==wxNOT_FOUND) m_pcNames.Add(sName);
                m_pc[sName].sName        = sName; // OK, probably not so useful, just for completeness sake
                m_pc[sName].TargetType   = GetText(type);
                m_pc[sName].UseDebugLibs = GetText(dbg);
                m_pc[sName].Charset      = GetText(cset);

                const TiXmlElement* e = NULL;

                // By default, OutDir is $(SolutionDir)$(Configuration)
                e = prop->FirstChildElement("OutDir");
                if (e) m_pc[sName].sOutDir = GetText(e);

                // By default, IntDir is $(Configuration)
                e = prop->FirstChildElement("IntDir");
                if (e) m_pc[sName].sIntDir = GetText(e);

                // By default, TargetName is $(ProjectName)
                e = prop->FirstChildElement("TargetName");
                if (e) m_pc[sName].sTargetName = GetText(e);

                // By default, TargetExt is .exe
                e = prop->FirstChildElement("TargetExt");
                if (e) m_pc[sName].sTargetExt = GetText(e);

                // $(VCInstallDir)include , $(VCInstallDir)atlmfc\include , $(WindowsSdkDir)include , $(FrameworkSDKDir)\include
                e = prop->FirstChildElement("IncludePath");
                if (e) m_pc[sName].sIncludePath = GetText(e);

                // $(VCInstallDir)lib , $(VCInstallDir)atlmfc\lib , $(WindowsSdkDir)lib , $(FrameworkSDKDir)\lib
                e = prop->FirstChildElement("LibraryPath");
                if (e) m_pc[sName].sLibPath = GetText(e);

                // $(VCInstallDir)bin , $(WindowsSdkDir)bin\NETFX 4.0 Tools , $(WindowsSdkDir)bin , $(VSInstallDir)Common7\Tools\bin ,
                // $(VSInstallDir)Common7\tools , $(VSInstallDir)Common7\ide , $(ProgramFiles)\HTML Help Workshop , $(FrameworkSDKDir)\bin ,
                // $(MSBuildToolsPath32) , $(VSInstallDir) , $(SystemRoot)\SysWow64 , $(FxCopDir) , $(PATH)
                e = prop->FirstChildElement("ExecutablePath");
                if (e) m_pc[sName].sExePath = GetText(e);

                // $(VCInstallDir)atlmfc\src\mfc , $(VCInstallDir)atlmfc\src\mfcm , $(VCInstallDir)atlmfc\src\atl , $(VCInstallDir)crt\src
                e = prop->FirstChildElement("SourcePath");
                if (e) m_pc[sName].sSourcePath = GetText(e);

                bResult = true;
            }
        }

        prop = prop->NextSiblingElement();
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find configuration, using default one."));

    return bResult;
}

/** get the list of files in the project
  * For each configuration found, the files will be added
  * \param root : the root node of the XML project file (<Project >
  **/
bool MSVC10Loader::GetProjectConfigurationFiles(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse each ItemGroup
    const TiXmlElement* prop = root->FirstChildElement("ItemGroup");
    while (prop)
    {
        const TiXmlElement* none = prop->FirstChildElement("None");
        while (none)
        {
            const char* attr = none->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), false, false);
                HandleFilesAndExcludes(none, pf);
            }

            none = none->NextSiblingElement();
            bResult = true; // at least one file imported
        }

        const TiXmlElement* incl = prop->FirstChildElement("ClInclude");
        while (incl)
        {
            const char* attr = incl->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), false, false);
                HandleFilesAndExcludes(incl, pf);
            }

            incl = incl->NextSiblingElement();
            bResult = true; // at least one file imported
        }

        const TiXmlElement* comp = prop->FirstChildElement("ClCompile");
        while (comp)
        {
            const char* attr = comp->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), true, true);
                HandleFilesAndExcludes(comp, pf);
            }

            comp = comp->NextSiblingElement();
            bResult = true; // at least one file imported
        }

        const TiXmlElement* res = prop->FirstChildElement("ResourceCompile");
        while (res)
        {
            const char* attr = res->Attribute("Include");
            if (attr)
            {
                ProjectFile* pf = m_pProject->AddFile(0, cbC2U(attr), true, true);
                HandleFilesAndExcludes(res, pf);
            }

            res = res->NextSiblingElement();
            bResult = true; // at least one file imported
        }

        prop = prop->NextSiblingElement();
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any files in the project...?!"));

    return bResult;
}

/** get project includes
  * \param root : the root node of the XML project file (<Project >
  **/
bool MSVC10Loader::GetProjectIncludes(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse all global parameters
    const TiXmlElement* prop = root->FirstChildElement("PropertyGroup");
    while (prop)
    {
        const char* attr = prop->Attribute("Condition");
        if (!attr) { prop = prop->NextSiblingElement(); continue; }

        wxString conf = cbC2U(attr);
        for (size_t i=0; i<m_pcNames.Count(); ++i)
        {
            wxString sName = m_pcNames.Item(i);
            wxString sConf = SubstituteConfigMacros(conf);
            if (sConf.IsSameAs(sName))
            {
                const TiXmlElement* cinc = prop->FirstChildElement("IncludePath");
                wxArrayString cdirs = GetDirectories(cinc);
                for (size_t j=0; j<cdirs.Count(); ++j)
                {
                    ProjectBuildTarget* bt = m_pc[sName].bt;
                    if (bt) bt->AddIncludeDir(cdirs.Item(j));
                }

                const TiXmlElement* linc = prop->FirstChildElement("LibraryPath");
                wxArrayString ldirs = GetDirectories(linc);
                for (size_t j=0; j<ldirs.Count(); ++j)
                {
                    ProjectBuildTarget* bt = m_pc[sName].bt;
                    if (bt) bt->AddLibDir(ldirs.Item(j));
                }
                bResult = true; // got something
            }
        }

        prop = prop->NextSiblingElement();
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any includes in the project...?!"));

    return bResult;
}

/** get target specific stuff
  * \param root : the root node of the XML project file (<Project >
  **/
bool MSVC10Loader::GetTargetSpecific(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // parse all global parameters
    const TiXmlElement* idef = root->FirstChildElement("ItemDefinitionGroup");
    while (idef)
    {
        const char* attr = idef->Attribute("Condition");
        if (!attr) { idef = idef->NextSiblingElement(); continue; }

        wxString conf = cbC2U(attr);
        for (size_t i=0; i<m_pcNames.Count(); ++i)
        {
            wxString sName = m_pcNames.Item(i);
            wxString sConf = SubstituteConfigMacros(conf);
            if (sConf.IsSameAs(sName))
            {
                const TiXmlElement* comp = idef->FirstChildElement("ClCompile");
                if (comp)
                {
                    const TiXmlElement* pp = comp->FirstChildElement("PreprocessorDefinitions");
                    wxArrayString pps = GetPreprocessors(pp);
                    for (size_t j=0; j<pps.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt) bt->AddCompilerOption((m_ConvertSwitches ? _T("-D") : _T("/D")) + pps.Item(j));
                    }

                    const TiXmlElement* cinc = comp->FirstChildElement("AdditionalIncludeDirectories");
                    wxArrayString cdirs = GetDirectories(cinc);
                    for (size_t j=0; j<cdirs.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt) bt->AddIncludeDir(cdirs.Item(j));
                    }

                    const TiXmlElement* copt = comp->FirstChildElement("AdditionalOptions");
                    wxArrayString copts = GetOptions(copt);
                    for (size_t j=0; j<copts.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt && !m_ConvertSwitches) bt->AddCompilerOption(copts.Item(j));
                    }
                }

                const TiXmlElement* link = idef->FirstChildElement("Link");
                if (link)
                {
                    const TiXmlElement* llib = link->FirstChildElement("AdditionalDependencies");
                    wxArrayString libs = GetLibs(llib);
                    for (size_t j=0; j<libs.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt) bt->AddLinkLib(libs.Item(j));
                    }

                    const TiXmlElement* linc = link->FirstChildElement("AdditionalLibraryDirectories");
                    wxArrayString ldirs = GetDirectories(linc);
                    for (size_t j=0; j<ldirs.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt) bt->AddLibDir(ldirs.Item(j));
                    }

                    const TiXmlElement* lopt = comp->FirstChildElement("AdditionalOptions");
                    wxArrayString lopts = GetOptions(lopt);
                    for (size_t j=0; j<lopts.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt && !m_ConvertSwitches) bt->AddLinkerOption(lopts.Item(j));
                    }

                    const TiXmlElement* debug = link->FirstChildElement("GenerateDebugInformation");
                    wxString sDebug = GetText(debug);
                    if (sDebug.MakeUpper().IsSameAs(_T("TRUE")))
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt && !m_ConvertSwitches) bt->AddLinkerOption(_T("/debug"));
                    }
                }

                const TiXmlElement* res = idef->FirstChildElement("ResourceCompile");
                if (res)
                {
                    const TiXmlElement* pp = res->FirstChildElement("PreprocessorDefinitions");
                    wxArrayString pps = GetPreprocessors(pp);
                    for (size_t j=0; j<pps.Count(); ++j)
                    {
                        ProjectBuildTarget* bt = m_pc[sName].bt;
                        if (bt) bt->AddCompilerOption((m_ConvertSwitches ? _T("-D") : _T("/D")) + pps.Item(j));
                    }
                }

                bResult = true; // got something
            }
        }

        idef = idef->NextSiblingElement();
    }

    if (!bResult)
        pMsg->DebugLog(_("Failed to find any includes in the project...?!"));

    return bResult;
}

/** Ask the users to select which configurations to import
  * \return true on success, false on failure
  */
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

    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
        it->second.bImport = false; // Disable all

    for (size_t i = 0; i < asSelectedStrings.GetCount(); ++i)
        m_pc[asSelectedStrings[i]].bImport = true;  // Enable selected

    return true;
}

/** Creates the configuration selected for import
  * \return true on success, false on failure
  */
bool MSVC10Loader::DoCreateConfigurations()
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    // create the project targets
    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
    {
        if (!it->second.bImport) continue;

        ProjectBuildTarget* bt = m_pProject->AddBuildTarget(it->second.sName);
        if (bt)
        {
            bt->SetCompilerID(m_pProject->GetCompilerID());
            bt->AddPlatform(spAll); // target all platforms, SupportedPlatforms enum in "globals.h"

            TargetType tt;
            tt = ttExecutable;
            if      (it->second.TargetType == _T("Application"))    tt = ttExecutable;
            else if (it->second.TargetType == _T("DynamicLibrary")) tt = ttDynamicLib;
            else if (it->second.TargetType == _T("StaticLibrary"))  tt = ttStaticLib;
            else
                pMsg->DebugLog(_("Warning: Unsupported target type: ") + it->second.TargetType);

            bt->SetTargetType(tt); // executable by default, TargetType enum in "globals.h"
            it->second.bt = bt; // apply

            pMsg->DebugLog(_("Created project build target: ") + it->second.sName);

            bResult = true; // at least one config imported
        }
    }

    return bResult;
}

/** convenience function for getting XML text
  * On failure, an empty string (and not NULL) is returned
  * \param e: the element from which the text must be extracted
  * \return a STL string, representing the value. The string is empty (=="") if the element is not valid
  *         or if nothing could be retrieved
  * Example: <tag>this is a text</tag>
  *         the method will return "this is a text"
  *         <tag></tag>
  *         the method will return ""
  */
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

    return ReplaceMSVCMacros(sResult);
}

void MSVC10Loader::HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf)
{
    if (!e || !pf)
        return;

    // add it to all configurations, not just the first
    for (size_t i=0; i<m_pcNames.Count(); ++i)
        pf->AddBuildTarget(m_pcNames.Item(i));

    // handle explicit exclusions like:
    // <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">true</ExcludedFromBuild>
    // <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">true</ExcludedFromBuild>
    const TiXmlElement* excl = e->FirstChildElement("ExcludedFromBuild");
    // Operate existing excludes
    while (excl)
    {
        const TiXmlText* do_excl = excl->ToText();
        if (do_excl)
        {
            const char* value = do_excl->Value();
            wxString  s_value = cbC2U(value);
            if (s_value.MakeUpper().IsSameAs(_T("TRUE")))
            {
                const char* cond = excl->Attribute("Condition");
                if (cond)
                {
                    wxString sName = cbC2U(cond); sName = SubstituteConfigMacros(sName);
                    pf->RemoveBuildTarget(sName);
                }
            }
        }

        excl = excl->NextSiblingElement();
    }
}

wxArrayString MSVC10Loader::GetDirectories(const TiXmlElement* e)
{
    wxArrayString sResult;
    if (e)
    {
        wxString dirs = GetText(e);
        wxArrayString dirsWithMacros;
        if (!dirs.IsEmpty())
            dirsWithMacros = GetArrayFromString(dirs, _T(";"));

        for (size_t i=0; i<dirsWithMacros.Count(); ++i)
        {
            wxString sDir = dirsWithMacros.Item(i);
            // Specific: for ItemGroups (not Dollar, but percentage)
            sDir.Replace(wxT("%(AdditionalIncludeDirectories)"), wxEmptyString); // not supported
            sDir.Replace(wxT("%(AdditionalLibraryDirectories)"), wxEmptyString); // not supported

            if (!sDir.Trim().IsEmpty())
                sResult.Add(sDir);
        }
    }

    return sResult;
}

wxArrayString MSVC10Loader::GetPreprocessors(const TiXmlElement* e)
{
    wxArrayString sResult;
    if (e)
    {
        wxString preproc = GetText(e);
        wxArrayString preprocWithMacros;
        if (!preproc.IsEmpty())
            preprocWithMacros = GetArrayFromString(preproc, _T(";"));

        for (size_t i=0; i<preprocWithMacros.Count(); ++i)
        {
            wxString sPreproc = preprocWithMacros.Item(i);
            // Specific: for ItemGroups (not Dollar, but percentage)
            sPreproc.Replace(wxT("%(PreprocessorDefinitions)"), wxEmptyString); // not supported
            if (!sPreproc.Trim().IsEmpty())
                sResult.Add(sPreproc);
        }
    }

    return sResult;
}

wxArrayString MSVC10Loader::GetLibs(const TiXmlElement* e)
{
    wxArrayString sResult;
    if (e)
    {
        wxString libs = GetText(e);
        wxArrayString libsWithMacros;
        if (!libs.IsEmpty())
            libsWithMacros = GetArrayFromString(libs, _T(";"));

        for (size_t i=0; i<libsWithMacros.Count(); ++i)
        {
            wxString sLib = libsWithMacros.Item(i);
            // Specific: for ItemGroups (not Dollar, but percentage)
            sLib.Replace(wxT("%(AdditionalDependencies)"), wxEmptyString); // not supported
            if (!sLib.Trim().IsEmpty())
                sResult.Add(sLib);
        }
    }

    return sResult;
}

wxArrayString MSVC10Loader::GetOptions(const TiXmlElement* e)
{
    wxArrayString sResult;
    if (e)
    {
        wxString opts = GetText(e);
        wxArrayString optsWithMacros;
        if (!opts.IsEmpty())
            optsWithMacros = GetArrayFromString(opts, _T(" "));

        for (size_t i=0; i<optsWithMacros.Count(); ++i)
        {
            wxString sOpt = optsWithMacros.Item(i);
            // Specific: for ItemGroups (not Dollar, but percentage)
            sOpt.Replace(wxT("%(AdditionalOptions)"), wxEmptyString); // not supported
            if (!sOpt.Trim().IsEmpty())
                sResult.Add(sOpt);
        }
    }

    return sResult;
}

wxString MSVC10Loader::ReplaceMSVCMacros(const wxString& str)
{
    wxString ret = str;
    ret.Replace(_T("$(OutDir)"),            m_OutDir);
    ret.Replace(_T("$(IntDir)"),            m_IntDir);
    ret.Replace(_T("$(INTDIR)"),            m_IntDir);
    ret.Replace(_T("$(ConfigurationName)"), m_ConfigurationName);
    ret.Replace(_T("$(PlatformName)"),      m_PlatformName);
    ret.Replace(_T("$(ProjectName)"),       m_ProjectName);
    ret.Replace(_T("$(ProjectDir)"),        m_pProject->GetBasePath());
    ret.Replace(_T("$(TargetPath)"),        m_TargetPath);
    ret.Replace(_T("$(TargetFileName)"),    m_TargetFilename);
    ret.Replace(_T("\""),                   _T(""));
    //ret.Replace(_T("&quot;"), _T("\""));

    // env. vars substitution removed because C::B recognizes them
    // during use ;)

    return ret;
}

/** Format a string by performing substitution
  * It is mainly intended for formatting Configuration names such as "'$(Configuration)|$(Platform)'=='Debug|Win32'"
  * \param sString : the string to format
  * \return the formatted string
  */
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

bool MSVC10Loader::Save(const wxString& filename)
{
    // no support to save MSVC10 projects
    return false;
}
