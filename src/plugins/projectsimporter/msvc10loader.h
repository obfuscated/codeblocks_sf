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
        // default %(AdditionalDependencies), not sure if "winmm" and "comctl32" are part of them
        static const wxString g_AdditionalDependencies;
        MSVC10Loader(cbProject* project);
        virtual ~MSVC10Loader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
    protected:
        cbProject* m_pProject;
        char m_ConvertSwitches;
        char m_NoImportLib;

        wxString m_WorkspacePath;
        // macros used in Visual Studio projects
        wxString m_ConfigurationName; // unused
        wxString m_TargetFilename; // unused
        wxString m_TargetPath; // unused
        wxString m_OutDir;
        wxString m_IntDir;

    private:
        wxString m_PlatformName;

        // user interaction
        /** Ask the users to select which configurations to import
          * \return true on success, false on failure
          */
        bool DoSelectConfigurations();

        /** Creates the configuration selected for import
          * \return true on success, false on failure
          */
        bool DoCreateConfigurations();

        // helper functions for GetConfiguration
        bool GetConfigurationName(const TiXmlElement* e, wxString& config, const wxString& defconfig);
        void SetConfigurationValues(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, wxString* globaltarget=NULL);
        void SetConfigurationValuesPath(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, wxString* globaltarget=NULL);
        void SetConfigurationValuesBool(const TiXmlElement* root, const char* key, size_t target, const wxString& defconfig, char* globaltarget=NULL);

        // these methods parse specific parts of the project file
        /** get project name, type and GUID
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetProjectGlobals(const TiXmlElement* root);
        /** get the list of configurations in the project, and create targets in CB Project
          * For each configuration found, a target will be created
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetProjectConfigurations(const TiXmlElement* root);
        /** get the configuration in the project (used by GetProjectConfigurations)
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetConfiguration(const TiXmlElement* root);
        /** get the list of files in the project, and add them to targets in CB Project
          * For each configuration found, the files will be added
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetProjectConfigurationFiles(const TiXmlElement* root);
        /** get the list of includes in the project, and add them to targets in CB Project
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetProjectIncludes(const TiXmlElement* root);
        /** get the list of target specific defines, libs, etc. in the project, and add them to targets in CB Project
          * \param root : the root node of the XML project file (<Project >
          **/
        bool GetTargetSpecific(const TiXmlElement* root);

        // these private members store the results of parsing the vcxproj file
        wxString m_ProjectGUID;
        wxString m_ProjectType;
        wxString m_ProjectName;

        // project configuration hash map
        struct SProjectConfiguration
        {
            ProjectBuildTarget* bt; // the CodeBlocks target linked to this configuration

            // ProjectConfiguration
            wxString sName;         // name of the configuration "Win32 Debug", "Win32 Release"
            wxString sConf;         // Configuration: Release, Debug
            wxString sPlatform;     // Platform: Win32, x64
            //PropertyGroup
            wxString TargetType;    // ConfigurationType: Console, Application, DynamicLibrary, StaticLibrary
//            wxString UseDebugLibs;  // UseDebugLibraries: indicates if debug libraries or release libraries must be used
            wxString Charset;       // CharacterSet: "NotSet", "Unicode" (/D_UNICODE, TCHAR is wchar_t), "MultiByte" (/D_MBCS, TCHAR is char)

            char bIsDefault;   // unused, if true, this is the default target
            char bNoImportLib; // if true, don't create import library
            wxString sOutDir;
            wxString sIntDir;
            wxString sTargetName;
            wxString sTargetExt;
            wxString sExePath; // unused
            wxString sSourcePath; // unused
//            wxString sIncludePath; // directly set in GetProjectIncludes
//            wxString sLibPath; // "
            // ClCompile
//            wxString sOptimization;  // directly set in GetTargetSpecific
//            wxString sRuntimeLibrary;// "
//            wxString sWarningLevel;  // "
//            wxString sDisableSpecificWarnings;  // "
            // Link
//            wxString sOutputFile; // directly set in GetTargetSpecific
//            wxString sModuleDefinitionFile; // "
//            wxString sImportLibrary; // "
        };
        WX_DECLARE_STRING_HASH_MAP(SProjectConfiguration, HashProjectsConfs);
        HashProjectsConfs m_pc;

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
        wxString GetText(const TiXmlElement* e);
        void HandleFilesAndExcludes(const TiXmlElement* e, ProjectFile* pf); ///< \brief convenience function for getting exclusion state of files
        wxArrayString GetArray(const TiXmlElement* e, const wxString delim = _T(";")); ///< \brief convenience function for getting 'delim' separated values (defines,options)
        wxArrayString GetArrayPaths(const TiXmlElement* e, const SProjectConfiguration &pc); ///< \brief convenience function for getting separated directories
        wxArrayString GetLibs(const TiXmlElement* e); ///< \brief convenience function for getting separated link libraries or default libs

        // misc helpers
        wxString ReplaceMSVCMacros(const wxString& str);
        void ReplaceConfigMacros(const SProjectConfiguration &pc, wxString &str);

        /** Format a string by performing substitution
          * It is mainly intended for formatting Configuration names such as "'$(Configuration)|$(Platform)'=='Debug|Win32'"
          * \param sString : the string to format
          * \return the formatted string
          */
        wxString SubstituteConfigMacros(const wxString& sString);
};

#endif // MSVC10LOADER_H
