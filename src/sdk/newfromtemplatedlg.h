#ifndef NEWFROMTEMPLATEDLG_H
#define NEWFROMTEMPLATEDLG_H

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include "projecttemplateloader.h"
#include "settings.h"

WX_DEFINE_ARRAY(ProjectTemplateLoader*, ProjectTemplateArray);

class NewFromTemplateDlg : public wxDialog
{
	public:
		NewFromTemplateDlg(const ProjectTemplateArray& templates, const wxArrayString& user_templates);
		virtual ~NewFromTemplateDlg();
		
		ProjectTemplateLoader* GetTemplate(){ return m_Template; }
        bool DoNotCreateFiles();
		bool SelectedUserTemplate();
		wxString GetSelectedUserTemplate();
		int GetOptionIndex();
		int GetFileSetIndex();
	protected:
		void BuildCategories();
		void BuildList();
		void FillTemplate(ProjectTemplateLoader* pt);
		void OnListSelection(wxListEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
	private:
		ProjectTemplateLoader* m_Template;
		wxImageList m_ImageList;
		const ProjectTemplateArray m_Templates;
		DECLARE_EVENT_TABLE()
};

#endif // NEWFROMTEMPLATEDLG_H

