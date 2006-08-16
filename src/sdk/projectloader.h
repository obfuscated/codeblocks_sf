#ifndef PROJECTLOADER_H
#define PROJECTLOADER_H

#include <wx/hashmap.h>
#include "ibaseloader.h"

#define PROJECT_FILE_VERSION_MAJOR 1
#define PROJECT_FILE_VERSION_MINOR 5

class cbProject;
class ProjectBuildTarget;
class ProjectFile;

WX_DECLARE_STRING_HASH_MAP(wxString, CompilerSubstitutes);

/** Code::Blocks project file loader. */
class DLLIMPORT ProjectLoader : public IBaseLoader
{
	public:
        /** Constructor.
          * @param project The project to handle (load/save). */
		ProjectLoader(cbProject* project);
		/// Destructor.
		virtual ~ProjectLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
        /** Export a target as a new project.
          * In other words, save a copy of the project containing only the specified target.
          * @param filename The new project filename.
          * @param onlyTarget The target name. If empty, it's like saving the project under a different name
          * (i.e. all targets are exported to the new project). */
        bool ExportTargetAsProject(const wxString& filename, const wxString& onlyTarget);
        /** @return True if the file was upgraded after load, false if not. */
        bool FileUpgraded(){ return m_Upgraded; }
        /** @return True if the file was modified while loading, false if not. This is usually true if FileUpgraded() returned true. */
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
        void DoVirtualTargets(TiXmlElement* parentNode);

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
        void SaveEnvironment(TiXmlElement* parent, CompileOptionsBase* base);

        // accepts a questionable compiler index and returns a valid compiler index
        // (popping up a selection dialog if needed)
        wxString GetValidCompilerID(const wxString& proposal, const wxString& scope);

        ProjectLoader(){} // no default ctor

        cbProject* m_pProject;
        bool m_Upgraded;
        bool m_OpenDirty; // set this to true if the project is loaded but modified (like the case when setting another compiler, if invalid)
        bool m_IsPre_1_2;
        int m_1_4_to_1_5_deftarget;
        CompilerSubstitutes m_CompilerSubstitutes;
};

#endif // PROJECTLOADER_H

