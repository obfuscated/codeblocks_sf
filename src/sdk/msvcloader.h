#ifndef MSVCLOADER_H
#define MSVCLOADER_H

#include "ibaseloader.h"
#include <wx/dynarray.h>
#include <wx/filename.h>
#include "compiletargetbase.h" // for target type

// forward decls
class cbProject;
class ProjectBuildTarget;

class MSVCLoader : public IBaseLoader
{
	public:
		MSVCLoader(cbProject* project);
		virtual ~MSVCLoader();
		
		bool Open(const wxString& filename);
		bool Save(const wxString& filename);
	protected:
        bool ReadConfigurations();
        bool ParseConfiguration(int index);
        bool ParseSourceFiles();
        void ProcessCompilerOptions(ProjectBuildTarget* target, const wxString& opts);
        void ProcessLinkerOptions(ProjectBuildTarget* target, const wxString& opts);
        wxString RemoveQuotes(const wxString& src);

        cbProject* m_pProject;
        bool m_ConvertSwitches;
        wxArrayString m_Configurations;
        wxArrayInt m_ConfigurationsLineIndex;
        TargetType m_Type;
        wxFileName m_Filename;
        int m_BeginTargetLine;
	private:
};

#endif // MSVCLOADER_H

