#ifndef PROJECTLOADER_H
#define PROJECTLOADER_H

#include "ibaseloader.h"

#define PROJECT_FILE_VERSION_MAJOR 1
#define PROJECT_FILE_VERSION_MINOR 2

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
        void DoMakeCommands(TiXmlElement* parentNode, CompileTargetBase* target);

        void DoBuild(TiXmlElement* parentNode);
        void DoBuildTarget(TiXmlElement* parentNode);
        void DoBuildTargetOptions(TiXmlElement* parentNode, ProjectBuildTarget* target);

        void DoEnvironment(TiXmlElement* parentNode, CompileOptionsBase* base);

        void DoUnits(TiXmlElement* parentNode);
        void DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file);
	private:
        void ConvertVersion_Pre_1_1();
        void ConvertLibraries(CompileTargetBase* object);

        // convenience functions, used in Save()
        TiXmlElement* AddElement(TiXmlElement* parent, const char* name, const char* attr, const wxString& attribute);
        TiXmlElement* AddElement(TiXmlElement* parent, const char* name, const char* attr, int attribute);
        void AddArrayOfElements(TiXmlElement* parent, const char* name, const char* attr, const wxArrayString& array);
        void SaveEnvironment(TiXmlElement* parent, CustomVars* vars);

        // accepts a questionable compiler index and returns a valid compiler index
        // (popping up a selection dialog if needed)
        int GetValidCompilerIndex(int proposal, const wxString& scope);

        ProjectLoader(){} // no default ctor

        cbProject* m_pProject;
        bool m_Upgraded;
        bool m_OpenDirty; // set this to true if the project is loaded but modified (like the case when setting another compiler, if invalid)
        bool m_IsPre_1_2;
};

#endif // PROJECTLOADER_H

