#ifndef PROJECTLOADER_H
#define PROJECTLOADER_H

#include "ibaseloader.h"

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
	protected:
        void DoProjectOptions(TiXmlElement* parentNode);
        void DoCompilerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoLinkerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoIncludesOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoLibsOptions(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        void DoExtraCommands(TiXmlElement* parentNode, ProjectBuildTarget* target = 0L);
        
        void DoBuild(TiXmlElement* parentNode);
        void DoBuildTarget(TiXmlElement* parentNode);
        void DoBuildTargetOptions(TiXmlElement* parentNode, ProjectBuildTarget* target);

        void DoUnits(TiXmlElement* parentNode);
        void DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file);
        
        void SaveOptions(wxString& buffer, const wxArrayString& array, const wxString& sectionName, int nrOfTabs, const wxString& optionName = "option");
	private:
        ProjectLoader(){} // no default ctor
        cbProject* m_pProject;
};

#endif // PROJECTLOADER_H

