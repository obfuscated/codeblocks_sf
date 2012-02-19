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
        bool GetProjectGlobalsInfo(TiXmlElement* root); ///< \brief get project name, type and GUID
        bool GetProjectConfigurations(TiXmlElement* root); ///< \brief get the list of configurations in the project, and create targets in CB Project

        // these private members store the results of parsing the vcxproj file
        wxString m_ProjectGUID;
        wxString m_ProjectType;
        wxString m_ProjectName;

        // project configuration hash map
        struct _ProjectConfigurations_
        {
            wxString mm_sName;         // name of the configuration "Win32 Debug", "Win32 Release"
            wxString mm_sPlatform;     // Win32, Win64
            wxString mm_sType;         // Release or Debug
            wxString mm_TargetType;    // application, dll, static lib, console
            wxString mm_UseDebugLibs;  // indicates if debug libraries or release libraries must be used
            wxString mm_Charset;       // The only value I have seen so far is "Unicode"
            bool mm_bImport;           // if true, this target will be imported (default)
            ProjectBuildTarget* mm_bt; // the CodeBlocks target linked to this configuration
            wxString mm_sOutDir;
            wxString mm_sIntDir;
            wxString mm_sTargetName;
            wxString mm_sTargetExt;
            wxString mm_sIncludePath;
            wxString mm_sLibPath;

            // default constructor
            _ProjectConfigurations_() :
                mm_sName(_T("")),
                mm_sPlatform(_T("Win32")),
                mm_sType(_T("Debug")),
                mm_TargetType(_T("Application")),
                mm_UseDebugLibs(_T("true")),
                mm_Charset(_T("Unicode")),
                mm_bImport(true),
                mm_bt(NULL)
            { ; };

             // constructor
            _ProjectConfigurations_(wxString sName, wxString sPlatform = _T("Win32"), wxString sType = _T("Debug")) :
                mm_sName(sName),
                mm_sPlatform(sPlatform),
                mm_sType(sType),
                mm_TargetType(_T("Application")),
                mm_UseDebugLibs(_T("true")),
                mm_Charset(_T("Unicode")),
                mm_bImport(true),
                mm_bt(NULL)
             { ; };
        };
        WX_DECLARE_STRING_HASH_MAP(_ProjectConfigurations_, HashProjectsConfs);
        HashProjectsConfs m_pc;

        // tinyXML helper
        wxString GetText(TiXmlElement *e); ///< \brief convenience function for getting XML text

        // misc helpers
        wxString FormatMSCVString(wxString sString); ///< \brief Format a string by performing substitution
};

#endif // MSVC10LOADER_H
