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
    TiXmlElement* root;

    root = doc.FirstChildElement("Project");
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
    bResult = GetProjectGlobalsInfo(root);

    // get the project list of configuration => 1 configuration = 1 build target in  CodeBlocks
    bResult = GetProjectConfigurations(root);

    return bResult;
}

/** get project name, type and GUID
  * \param root : the root node  of the XML project file (<Project >
  **/
bool MSVC10Loader::GetProjectGlobalsInfo(TiXmlElement* root)
{
    TiXmlElement* prop = NULL;
    bool bResult;
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bResult = false;
    if (root) prop = root->FirstChildElement("PropertyGroup");
    m_pProject->SetTitle(cbC2U(root->Attribute("NoName")));

    while (prop)
    {
        wxString sLabelName = cbC2U(prop->Attribute("Label"));
        if (sLabelName.MakeUpper().IsSameAs(_T("GLOBALS")))
        {
            TiXmlElement* pGUID = prop->FirstChildElement("ProjectGuid");
            m_ProjectGUID = GetText(pGUID);

            TiXmlElement* pProjectType = prop->FirstChildElement("Keyword");
            m_ProjectType = GetText(pProjectType);

            TiXmlElement* pProjectName = prop->FirstChildElement("RootNamespace");
            m_ProjectName = GetText(pProjectName);

            bResult = true;
            prop = NULL; //exit loop

            // logging
            #if wxCHECK_VERSION(2, 9, 0)
            pMsg->DebugLog(wxString::Format(_("Project global properties: GUID=%s, Type=%s, Name=%s"),
                                             m_ProjectGUID.wx_str(), m_ProjectType.wx_str(), m_ProjectName.wx_str()));
            #else
            pMsg->DebugLog(wxString::Format(_("Project global properties: GUID=%s, Type=%s, Name=%s"),
                                             m_ProjectGUID.c_str(), m_ProjectType.c_str(), m_ProjectName.c_str()));
            #endif
        }
        else
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
bool MSVC10Loader::GetProjectConfigurations(TiXmlElement* root)
{
    // delete all targets of the project (we 'll create new ones from the imported configurations)
    while (m_pProject->GetBuildTargetsCount())
        m_pProject->RemoveBuildTarget(0);

    TiXmlElement* prop = NULL;
    bool bResult;
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg) return false;

    bResult = false;

    // first we try to get the list of configurations : there is normally a specific chapter for that
    // this is not truly necessary, but it is cleaner like that:
    // the plugin will be easier to understand, and easier to extend if necessary
    if (root) prop = root->FirstChildElement("ItemGroup");

    while (prop)
    {
        wxString sLabelName = cbC2U(prop->Attribute("Label"));
        sLabelName = sLabelName.MakeUpper();
        if (sLabelName == _T("PROJECTCONFIGURATIONS"))
        {
            TiXmlElement* conf = NULL;
            conf = prop->FirstChildElement("ProjectConfiguration");
            while(conf)
            {
                // loop over all the configurations
                wxString sConfName = cbC2U(conf->Attribute("Include"));
                sConfName.Replace(_T("|"), _T(" "));
                wxString sConfType;
                wxString sConfPlatform;
                TiXmlElement* pConfType = prop->FirstChildElement("Configuration");
                sConfType = GetText(pConfType);

                TiXmlElement* pConfPlatform = prop->FirstChildElement("Platform");
                sConfPlatform = GetText(pConfPlatform);

                m_pc[sConfName] = _ProjectConfigurations_(sConfName, sConfPlatform, sConfType);

                conf = conf->NextSiblingElement();
            }

            bResult = true;
        }
        prop = prop->NextSiblingElement();
    }

    // now that we have (in theory) the list of configurations, we will parse each configuration
    if (root) prop = root->FirstChildElement("PropertyGroup");
    while (prop)
    {
        wxString sLabelName = cbC2U(prop->Attribute("Label"));
        sLabelName = sLabelName.MakeUpper();
        if (sLabelName == _T("CONFIGURATION"))
        {
            wxString sConfName;

            sConfName = cbC2U(prop->Attribute("Condition"));
            sConfName = FormatMSCVString(sConfName);

            TiXmlElement* e;

            m_pc[sConfName].mm_sName      = sConfName; // OK, probably not so useful, just for completeness sake

            e = prop->FirstChildElement("ConfigurationType");
            m_pc[sConfName].mm_TargetType = GetText(e);

            e = prop->FirstChildElement("UseDebugLibraries");
            m_pc[sConfName].mm_UseDebugLibs = GetText(e);

            e = prop->FirstChildElement("CharacterSet");
            m_pc[sConfName].mm_Charset = GetText(e);

            ////

            e = prop->FirstChildElement("OutDir");
            m_pc[sConfName].mm_sOutDir = GetText(e);

            e = prop->FirstChildElement("IntDir");
            m_pc[sConfName].mm_sIntDir = GetText(e);

            e = prop->FirstChildElement("TargetName");
            m_pc[sConfName].mm_sTargetName = GetText(e);

            e = prop->FirstChildElement("TargetExt");
            m_pc[sConfName].mm_sTargetExt = GetText(e);

            e = prop->FirstChildElement("IncludePath");
            m_pc[sConfName].mm_sIncludePath = GetText(e);

            e = prop->FirstChildElement("LibraryPath");
            m_pc[sConfName].mm_sLibPath = GetText(e);

            bResult = true;
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
    }
    else
    {
        pMsg->DebugLog(_("The following configurations were found :"));
        for (HashProjectsConfs::iterator it = m_pc.begin(); it != m_pc.end(); ++it)
            pMsg->DebugLog(it->second.mm_sName);
    }

    m_pProject->SetTitle(m_ProjectName);

    return(DoSelectConfiguration());
}

/** Ask the users to select which configurations to import
  * \return true on success, false on failure
  */
bool MSVC10Loader::DoSelectConfiguration()
{
    wxArrayString asSelectedStrings;
    if (!ImportersGlobals::ImportAllTargets) // by default, all targets are imported
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
wxString MSVC10Loader::GetText(TiXmlElement *e)
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

/** Format a string by performing substitution
  * It is mainly intended for formatting Configuration names such as "'$(Configuration)|$(Platform)'=='Debug|Win32'"
  * \param sString : the string to format
  * \return the formatted string
  */
wxString MSVC10Loader::FormatMSCVString(wxString sString)
{
    wxString sResult = sString;

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

