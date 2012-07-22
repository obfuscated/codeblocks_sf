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

#include "prep.h"
#include "msvc10loader.h"
#include "multiselectdlg.h"
#include "importers_globals.h"

/** NOTES:
    This class is as close as possible to the MSVC6 and MSVC7 loaders.
    The similarity will help maintenance. However, "similar" does not mean "the same as"

    The results configuration parsing is stored in the member m_pc
    This hash map contains all the target information: Platform, Debug/Release, DLL/Lib/Executable, ...
    A CodeBlock target is created for each element in this hash map.
    Please note that some members are unused by CodeBlocks, such as mm_Charset or mm_UseDebugLibs
**/

MSVC10Loader::MSVC10Loader(cbProject* project) :
    m_pProject(project),
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

bool MSVC10Loader::Open(const wxString& filename)
{
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    /* NOTE (mandrav#1#): not necessary to ask for switches conversion... */
    //m_ConvertSwitches = m_pProject->GetCompilerID().IsSameAs(_T("gcc"));
    m_ProjectName = wxFileName(filename).GetName();

    #if wxCHECK_VERSION(2, 9, 0)
    pMsg->DebugLog(F(_("Importing MSVC 10.xx project: %s"), filename.wx_str()));
    #else
    pMsg->DebugLog(F(_("Importing MSVC 10.xx project: %s"), filename.c_str()));
    #endif

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

    // get project name & type
    bool bResult;
    bResult  = GetProjectGlobalsInfo(root);

    // get the project list of configuration => 1 configuration = 1 build target in CodeBlocks
    bResult |= GetProjectConfigurations(root);

    // get the project list of files and add them to the targets
    bResult |= GetProjectConfigurationFiles(root);

    return bResult;
}

/** get project name, type and GUID
  * \param root : the root node  of the XML project file (<Project >
  **/
bool MSVC10Loader::GetProjectGlobalsInfo(const TiXmlElement* root)
{
    if (!root) return false;

    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bool bResult = false;

    const char* title = root->Attribute("NoName");
    if (title) m_pProject->SetTitle(cbC2U(title));

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
  * \param root : the root node  of the XML project file (<Project >
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

        wxString label = cbC2U(attr); label = label.MakeUpper();
        if (label.MakeUpper().IsSameAs(_T("PROJECTCONFIGURATIONS")))
        {
            const TiXmlElement* conf = prop->FirstChildElement("ProjectConfiguration");
            while(conf)
            {
                // loop over all the configurations
                const char*         name = conf->Attribute("Include");
                const TiXmlElement* cfg  = prop->FirstChildElement("Configuration");
                const TiXmlElement* plat = prop->FirstChildElement("Platform");
                if (name && cfg && plat)
                {
                    wxString sName = cbC2U(name); sName.Replace(_T("|"), _T(" "));
                    wxString sType = GetText(cfg);
                    wxString sPlat = GetText(plat);

                    if (m_pcNames.Index(sName)==wxNOT_FOUND) m_pcNames.Add(sName);
                    m_pc[sName] = SProjectConfigurations(sName, sType, sPlat);
                }
                conf = conf->NextSiblingElement();
            }

            bResult = true;
        }
        prop = prop->NextSiblingElement();
    }

    // now that we have (in theory) the list of configurations, we will parse each configuration
    prop = root->FirstChildElement("PropertyGroup");
    while (prop)
    {
        const  char* attr = prop->Attribute("Label");
        if (!attr) { prop = prop->NextSiblingElement(); continue; }

        wxString lbl = cbC2U(attr);
        if (lbl.MakeUpper().IsSameAs(_T("CONFIGURATION")))
        {
            const char*         name = prop->Attribute("Condition");
            const TiXmlElement* type = prop->FirstChildElement("ConfigurationType");
            const TiXmlElement* dbg  = prop->FirstChildElement("UseDebugLibraries");
            const TiXmlElement* cset = prop->FirstChildElement("CharacterSet");
            if (name && type && dbg && cset)
            {
                wxString sName = cbC2U(name); sName = FormatMSCVString(sName);
                if (m_pcNames.Index(sName)==wxNOT_FOUND) m_pcNames.Add(sName);
                m_pc[sName].mm_sName        = sName; // OK, probably not so useful, just for completeness sake
                m_pc[sName].mm_TargetType   = GetText(type);
                m_pc[sName].mm_UseDebugLibs = GetText(dbg);
                m_pc[sName].mm_Charset      = GetText(cset);

                const TiXmlElement* e = NULL;

                // By default, OutDir is $(SolutionDir)$(Configuration)
                e = prop->FirstChildElement("OutDir");
                if (e) m_pc[sName].mm_sOutDir = GetText(e);

                // By default, IntDir is $(Configuration)
                e = prop->FirstChildElement("IntDir");
                if (e) m_pc[sName].mm_sIntDir = GetText(e);

                // By default, TargetName is $(ProjectName)
                e = prop->FirstChildElement("TargetName");
                if (e) m_pc[sName].mm_sTargetName = GetText(e);

                // By default, TargetExt is .exe
                e = prop->FirstChildElement("TargetExt");
                if (e) m_pc[sName].mm_sTargetExt = GetText(e);

                // $(VCInstallDir)include , $(VCInstallDir)atlmfc\include , $(WindowsSdkDir)include , $(FrameworkSDKDir)\include
                e = prop->FirstChildElement("IncludePath");
                if (e) m_pc[sName].mm_sIncludePath = GetText(e);

                // $(VCInstallDir)lib , $(VCInstallDir)atlmfc\lib , $(WindowsSdkDir)lib , $(FrameworkSDKDir)\lib
                e = prop->FirstChildElement("LibraryPath");
                if (e) m_pc[sName].mm_sLibPath = GetText(e);

                // $(VCInstallDir)bin , $(WindowsSdkDir)bin\NETFX 4.0 Tools , $(WindowsSdkDir)bin , $(VSInstallDir)Common7\Tools\bin ,
                // $(VSInstallDir)Common7\tools , $(VSInstallDir)Common7\ide , $(ProgramFiles)\HTML Help Workshop , $(FrameworkSDKDir)\bin ,
                // $(MSBuildToolsPath32) , $(VSInstallDir) , $(SystemRoot)\SysWow64 , $(FxCopDir) , $(PATH)
                e = prop->FirstChildElement("ExecutablePath");
                if (e) m_pc[sName].mm_sExePath = GetText(e);

                // $(VCInstallDir)atlmfc\src\mfc , $(VCInstallDir)atlmfc\src\mfcm , $(VCInstallDir)atlmfc\src\atl , $(VCInstallDir)crt\src
                e = prop->FirstChildElement("SourcePath");
                if (e) m_pc[sName].mm_sSourcePath = GetText(e);

                bResult = true;
            }
        }

        prop = prop->NextSiblingElement();
    }

    if (!bResult)
    {
        pMsg->DebugLog(_("Failed to find configurations in the project, create a default one: debug executable."));
        ProjectBuildTarget* bt;
        bt = m_pProject->AddBuildTarget(_T("Debug"));
        if (bt)
        {
            bt->SetCompilerID(m_pProject->GetCompilerID());
            bt->AddPlatform(spAll); // target all platforms, SupportedPlatforms enum in "globals.h"
            bt->SetTargetType(ttExecutable); // executable by default, TargetType enum in "globals.h"
        }
        bResult = true;
    }
    else
    {
        pMsg->DebugLog(_("The following configurations were found :"));
        for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
            pMsg->DebugLog(it->second.mm_sName);
    }

    m_pProject->SetTitle(m_ProjectName);

    return ( DoSelectConfiguration() );
}

/** get the list of files in the project
  * For each configuration found, the files will be added
  * \param root : the root node  of the XML project file (<Project >
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

    return bResult;
}

/** Ask the users to select which configurations to import
  * \return true on success, false on failure
  */
bool MSVC10Loader::DoSelectConfiguration()
{
    wxArrayString asSelectedStrings;
    if ( !ImportersGlobals::ImportAllTargets ) // by default, all targets are imported
    {
        // ask the user to select a configuration - multiple choice ;)
        wxArrayString configurations;
        for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
            configurations.Add(it->second.mm_sName);

        MultiSelectDlg dlg(0, configurations, true, _("Select configurations to import:"), m_pProject->GetTitle());
        PlaceWindow(&dlg);
        if (dlg.ShowModal() == wxID_CANCEL)
        {
            Manager::Get()->GetLogManager()->DebugLog(_("Cancelled.."));
            return false;
        }
        asSelectedStrings = dlg.GetSelectedStrings();
        if (asSelectedStrings.GetCount() < 1) return(false);

        for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
            it->second.mm_bImport = false;

        for (size_t i = 0; i < asSelectedStrings.GetCount(); ++i)
            m_pc[asSelectedStrings[i]].mm_bImport = true;
    }

    // create the project targets
    for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
    {
        ProjectBuildTarget* bt;
        bt = m_pProject->AddBuildTarget(it->second.mm_sName);
        if (bt)
        {
            bt->SetCompilerID(m_pProject->GetCompilerID());
            bt->AddPlatform(spAll); // target all platforms, SupportedPlatforms enum in "globals.h"

            TargetType tt;
            tt = ttExecutable;
            if (it->second.mm_sType == _T("Application"))    tt = ttExecutable;
            if (it->second.mm_sType == _T("DynamicLibrary")) tt = ttDynamicLib;
            if (it->second.mm_sType == _T("StaticLibrary"))  tt = ttStaticLib;

            bt->SetTargetType(tt); //executable by default, TargetType enum in "globals.h"
            it->second.mm_bt = bt;
        }
        else
            return false;
    }

    return true;
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

    sResult = ReplaceMSVCMacros(sResult);
    return sResult;
}

void MSVC10Loader::HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf)
{
    if (!pf)
        return;

    // add it to all configurations, not just the first
    for (size_t i=0; i<m_pcNames.Count(); ++i)
        pf->AddBuildTarget(m_pcNames[i]);

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
            if (s_value.IsSameAs(_T("TRUE")))
            {
                const char* cond = excl->Attribute("Condition");
                if (cond)
                {
                    wxString sName = cbC2U(cond); sName = FormatMSCVString(sName);
                    pf->RemoveBuildTarget(sName);
                }
            }
        }

        excl = excl->NextSiblingElement();
    }
}

/** Format a string by performing substitution
  * It is mainly intended for formatting Configuration names such as "'$(Configuration)|$(Platform)'=='Debug|Win32'"
  * \param sString : the string to format
  * \return the formatted string
  */
wxString MSVC10Loader::FormatMSCVString(const wxString& sString)
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
