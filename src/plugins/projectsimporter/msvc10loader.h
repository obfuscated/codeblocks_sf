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
class TiXmlElement;

class MSVC10Loader : public IBaseLoader
{
    public:
        MSVC10Loader(cbProject* project);
        virtual ~MSVC10Loader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
    protected:
        cbProject* m_pProject;
        bool m_ConvertSwitches;

        // macros used in Visual Studio projects
        wxString m_ConfigurationName;
        wxString m_TargetFilename;
        wxString m_TargetPath;
        wxString m_OutDir;
        wxString m_IntDir;

    private:
        wxString m_PlatformName;

        // user interaction
        bool DoSelectConfigurations();

        // creation of targets
        bool DoCreateConfigurations();

        // these methods parse specific parts of the project file
        bool GetProjectGlobals(const TiXmlElement* root); ///< \brief get project name, type and GUID
        bool GetProjectConfigurations(const TiXmlElement* root); ///< \brief get the list of configurations in the project, and create targets in CB Project
        bool GetConfiguration(const TiXmlElement* root); ///< \brief get the configuration in the project (used by GetProjectConfigurations)
        bool GetProjectConfigurationFiles(const TiXmlElement* root); ///< \brief get the list of files in the project, and add them to targets in CB Project
        bool GetProjectIncludes(const TiXmlElement* root); ///< \brief get the list of includes in the project, and add them to targets in CB Project
        bool GetTargetSpecific(const TiXmlElement* root); ///< \brief get the list of target specific defines, libs, etc. in the project, and add them to targets in CB Project

        // these private members store the results of parsing the vcxproj file
        wxString m_ProjectGUID;
        wxString m_ProjectType;
        wxString m_ProjectName;

        // project configuration hash map
        struct SProjectConfiguration
        {
            ProjectBuildTarget* bt; // the CodeBlocks target linked to this configuration

            wxString sName;         // name of the configuration "Win32 Debug", "Win32 Release"
            wxString sPlatform;     // Win32, Win64
            wxString sConf;         // Release or Debug
            wxString TargetType;    // application, dll, static lib, console
            wxString UseDebugLibs;  // indicates if debug libraries or release libraries must be used
            wxString Charset;       // The only value I have seen so far is "Unicode"
            bool     bIsDefault;    // if true, this is the default target
            bool     bImport;       // if true, this target will be imported (default)
            wxString sOutDir;
            wxString sIntDir;
            wxString sTargetName;
            wxString sTargetExt;
            wxString sExePath;
            wxString sIncludePath;
            wxString sLibPath;
            wxString sSourcePath;
        };
        WX_DECLARE_STRING_HASH_MAP(SProjectConfiguration, HashProjectsConfs);
        HashProjectsConfs m_pc;
        wxArrayString     m_pcNames;

        // tinyXML helper
        wxString GetText(const TiXmlElement* e); ///< \brief convenience function for getting XML text
        void HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf); ///< \brief convenience function for getting exclusion state of files
        wxArrayString GetDirectories(const TiXmlElement* e); ///< \brief convenience function for getting separated directories
        wxArrayString GetPreprocessors(const TiXmlElement* e); ///< \brief convenience function for getting separated preprocessors
        wxArrayString GetLibs(const TiXmlElement* e); ///< \brief convenience function for getting separated link libraries
        wxArrayString GetOptions(const TiXmlElement* e); ///< \brief convenience function for getting separated compiler/linker options

        // misc helpers
        wxString ReplaceMSVCMacros(const wxString& str);
        wxString SubstituteConfigMacros(const wxString& sString); ///< \brief Format a string by performing substitution
};

#endif // MSVC10LOADER_H
