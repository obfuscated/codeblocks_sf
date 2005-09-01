#ifndef PROJECTLOADER_H
#define PROJECTLOADER_H

#include "ibaseloader.h"

#define PROJECT_FILE_VERSION_MAJOR _T("1")
#define PROJECT_FILE_VERSION_MINOR _T("1")

class cbProject;
class ProjectBuildTarget;
class ProjectFile;

class DLLIMPORT ProjectLoader : public IBaseLoader
{
	public:
		ProjectLoader(cbProject* project);
		virtual ~ProjectLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
        bool FileUpgraded(){ return m_Upgraded; }
        bool FileModified(){ return m_OpenDirty; }
	protected:
        void DoProjectOptions(TiXmlElement* parentNode);
        void DoCompilerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoResourceCompilerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoLinkerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoIncludesOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoLibsOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoExtraCommands(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);

        void DoBuild(TiXmlElement* parentNode);
        void DoBuildTarget(TiXmlElement* parentNode);
        void DoBuildTargetOptions(TiXmlElement* parentNode, ProjectBuildTarget* target);

        void DoEnvironment(TiXmlElement* parentNode, CompileOptionsBase* base);

        void DoUnits(TiXmlElement* parentNode);
        void DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file);

        void BeginOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs);
        bool DoOptionSection(wxString& buffer, const wxArrayString& array, int nrOfTabs, const wxString& optionName = _("option"));
        void EndOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs);

        // shortcut that calls BeginOptionSection(), DoOptionSection() and EndOptionSection()
        void SaveOptions(wxString& buffer, const wxArrayString& array, const wxString& sectionName, int nrOfTabs, const wxString& optionName = _("option"), const wxString& extra = wxEmptyString);
	private:
        void SaveCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs);
        void SaveResourceCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs);
        void SaveLinkerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs);
        void SaveEnvironment(wxString& buffer, CustomVars* vars, int nrOfTabs);

        void ConvertVersion_Pre_1_1();
        void ConvertLibraries(CompileTargetBase* object);

        // accepts a questionable compiler index and returns a valid compiler index
        // (popping up a selection dialog if needed)
        int GetValidCompilerIndex(int proposal, const wxString& scope);

        ProjectLoader(){} // no default ctor

        cbProject* m_pProject;
        bool m_Upgraded;
        bool m_OpenDirty; // set this to true if the project is loaded but modified (like the case when setting another compiler, if invalid)
};

#endif // PROJECTLOADER_H

