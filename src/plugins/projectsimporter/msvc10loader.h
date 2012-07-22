/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef MSVC10LOADER_H
#define MSVC10LOADER_H

#include "ibaseloader.h"
#include <wx/hashmap.h>

// forward decls
class cbProject;
class ProjectFile;

class MSVC10Loader : public IBaseLoader
{
    public:
        MSVC10Loader(cbProject* project);
        virtual ~MSVC10Loader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
    protected:
        cbProject* m_pProject;

        // macros used in Visual Studio projects
        wxString m_ConfigurationName;
        wxString m_TargetFilename;
        wxString m_TargetPath;
        wxString m_OutDir;
        wxString m_IntDir;

    private:
        wxString m_PlatformName;

        wxString ReplaceMSVCMacros(const wxString& str);

        // user interaction
        bool DoSelectConfiguration();

        // these methods parse specific parts of the project file
        bool GetProjectGlobalsInfo(const TiXmlElement* root); ///< \brief get project name, type and GUID
        bool GetProjectConfigurations(const TiXmlElement* root); ///< \brief get the list of configurations in the project, and create targets in CB Project
        bool GetProjectConfigurationFiles(const TiXmlElement* root); ///< \brief get the list of files in the project, and add them to targets in CB Project

        // these private members store the results of parsing the vcxproj file
        wxString m_ProjectGUID;
        wxString m_ProjectType;
        wxString m_ProjectName;

        // project configuration hash map
        struct SProjectConfigurations
        {
            ProjectBuildTarget* mm_bt; // the CodeBlocks target linked to this configuration

            wxString mm_sName;         // name of the configuration "Win32 Debug", "Win32 Release"
            wxString mm_sPlatform;     // Win32, Win64
            wxString mm_sType;         // Release or Debug
            wxString mm_TargetType;    // application, dll, static lib, console
            wxString mm_UseDebugLibs;  // indicates if debug libraries or release libraries must be used
            wxString mm_Charset;       // The only value I have seen so far is "Unicode"
            bool     mm_bImport;       // if true, this target will be imported (default)
            wxString mm_sOutDir;
            wxString mm_sIntDir;
            wxString mm_sTargetName;
            wxString mm_sTargetExt;
            wxString mm_sExePath;
            wxString mm_sIncludePath;
            wxString mm_sLibPath;
            wxString mm_sSourcePath;

            // default constructor
            SProjectConfigurations() :
                mm_bt(NULL),
                mm_sName(_T("")),
                mm_sPlatform(_T("Win32")),
                mm_sType(_T("Debug")),
                mm_TargetType(_T("Application")),
                mm_UseDebugLibs(_T("true")),
                mm_Charset(_T("Unicode")),
                mm_bImport(true)
            { ; };

             // constructor
            SProjectConfigurations(wxString sName, wxString sPlatform = _T("Win32"), wxString sType = _T("Debug")) :
                mm_bt(NULL),
                mm_sName(sName),
                mm_sPlatform(sPlatform),
                mm_sType(sType),
                mm_TargetType(_T("Application")),
                mm_UseDebugLibs(_T("true")),
                mm_Charset(_T("Unicode")),
                mm_bImport(true)
             { ; };
        };
        WX_DECLARE_STRING_HASH_MAP(SProjectConfigurations, HashProjectsConfs);
        HashProjectsConfs m_pc;
        wxArrayString     m_pcNames;

        // tinyXML helper
        wxString GetText(const TiXmlElement* e); ///< \brief convenience function for getting XML text
        void HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf); ///< \brief convenience function for getting exclusion state of files

        // misc helpers
        wxString FormatMSCVString(const wxString& sString); ///< \brief Format a string by performing substitution
};

#endif // MSVC10LOADER_H
