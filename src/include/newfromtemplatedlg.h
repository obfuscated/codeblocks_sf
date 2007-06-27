#ifndef NEWFROMTEMPLATEDLG_H
#define NEWFROMTEMPLATEDLG_H

#include <wx/dialog.h>
#include "globals.h"
#include "pluginmanager.h"
#include "cbplugin.h"

class wxChoice;
class wxListCtrl;
class wxListEvent;
class ProjectTemplateLoader;

class NewFromTemplateDlg : public wxDialog
{
	public:
		NewFromTemplateDlg(TemplateOutputType initial, const wxArrayString& user_templates);
		virtual ~NewFromTemplateDlg();

		ProjectTemplateLoader* GetTemplate(){ return m_Template; }
		cbWizardPlugin* GetWizard(){ return m_pWizard; }
		int GetWizardIndex() const { return m_WizardIndex; }
		cbWizardPlugin* GetSelectedTemplate();
		bool SelectedUserTemplate() const;
		wxString GetSelectedUserTemplate() const;

		void EndModal(int retCode);
	protected:
		void FillTemplate(ProjectTemplateLoader* pt);
		void BuildCategories();
		void BuildCategoriesFor(TemplateOutputType otype, wxChoice* cat);
		void BuildList();
		void BuildListFor(TemplateOutputType otype, wxListCtrl* list, const wxChoice* cat);
		void ClearList();
		void ClearListFor(wxListCtrl* list);
		void OnListRightClick(wxListEvent& event);
		void OnListActivate(wxListEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
		void OnEditScript(wxCommandEvent& event);
		void OnDiscardScript(wxCommandEvent& event);
		void OnEditGlobalScript(wxCommandEvent& event);
		void OnViewChange(wxCommandEvent& event);
		void OnHelp(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);

		void ChangeView();
		void EditScript(const wxString& filename);
		wxListCtrl* GetVisibleListCtrl();
		wxChoice* GetVisibleCategory();
		TemplateOutputType GetVisibleOutputType() const;
	private:
		ProjectTemplateLoader* m_Template;
		cbWizardPlugin* m_pWizard;
		int m_WizardIndex;
		PluginsArray m_Wizards;
		DECLARE_EVENT_TABLE()
};

#endif // NEWFROMTEMPLATEDLG_H
