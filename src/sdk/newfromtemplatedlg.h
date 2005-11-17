#ifndef NEWFROMTEMPLATEDLG_H
#define NEWFROMTEMPLATEDLG_H

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include "projecttemplateloader.h"
#include "settings.h"
#include "pluginmanager.h"

WX_DEFINE_ARRAY(ProjectTemplateLoader*, ProjectTemplateArray);

class NewFromTemplateDlg : public wxDialog
{
	public:
		NewFromTemplateDlg(const ProjectTemplateArray& templates, const wxArrayString& user_templatesz);
		virtual ~NewFromTemplateDlg();

		ProjectTemplateLoader* GetTemplate(){ return m_Template; }
		cbProjectWizardPlugin* GetWizard(){ return m_pWizard; }
		int GetWizardIndex(){ return m_WizardIndex; }
        bool DoNotCreateFiles();
		bool SelectedTemplate();
		bool SelectedUserTemplate();
		wxString GetSelectedUserTemplate();
		int GetOptionIndex();
		int GetFileSetIndex();
		wxString GetProjectPath();
		wxString GetProjectName();
	protected:
		void BuildCategories();
		void BuildList();
		void FillTemplate(ProjectTemplateLoader* pt);
		void ClearList();
		void OnListSelection(wxListEvent& event);
		void OnBrowsePath(wxCommandEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
		void OnOptionChanged(wxCommandEvent& event);
		void OnFilesetChanged(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
		ProjectTemplateLoader* m_Template;
		cbProjectWizardPlugin* m_pWizard;
		int m_WizardIndex;
		wxImageList m_ImageList;
		const ProjectTemplateArray m_Templates;
		PluginsArray m_Wizards;
		DECLARE_EVENT_TABLE()
};

#endif // NEWFROMTEMPLATEDLG_H

