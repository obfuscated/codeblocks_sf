#ifndef PROJECTTEMPLATELOADER_H
#define PROJECTTEMPLATELOADER_H

#include "tinyxml/tinyxml.h"
#include "settings.h"

struct FileSetFile
{
	wxString source;
	wxString destination;
};
WX_DECLARE_OBJARRAY(FileSetFile, FileSetFileArray);

struct FileSet
{
	wxString name;
	wxString title;
	FileSetFileArray files;
};
WX_DECLARE_OBJARRAY(FileSet, FileSetArray);

struct TemplateOption
{
	wxString name;
	wxString file;
	wxArrayString extraCFlags;
	wxArrayString extraLDFlags;
};
WX_DECLARE_OBJARRAY(TemplateOption, TemplateOptionArray);

class DLLIMPORT ProjectTemplateLoader
{
	public:
		ProjectTemplateLoader();
		virtual ~ProjectTemplateLoader();

        bool Open(const wxString& filename);

		wxString m_Name;
		wxString m_Title;
		wxString m_Category;
		wxString m_Bitmap;
		FileSetArray m_FileSets;
		TemplateOptionArray m_TemplateOptions;
	protected:
        void DoTemplate(TiXmlElement* parentNode);
        void DoFileSet(TiXmlElement* parentNode);
        void DoFileSetFile(TiXmlElement* parentNode, FileSet& fs);
        void DoOption(TiXmlElement* parentNode);
        void DoOptionProject(TiXmlElement* parentNode, TemplateOption& to);
        void DoOptionCompiler(TiXmlElement* parentNode, TemplateOption& to);
        void DoOptionLinker(TiXmlElement* parentNode, TemplateOption& to);
	private:
};

#endif // PROJECTTEMPLATELOADER_H

