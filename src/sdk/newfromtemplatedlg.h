#ifndef NEWFROMTEMPLATEDLG_H
#define NEWFROMTEMPLATEDLG_H

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <wx/imaglist.h>
#include "projecttemplateloader.h"
#include "settings.h"
#include "pluginmanager.h"

WX_DEFINE_ARRAY(ProjectTemplateLoader*, ProjectTemplateArray);

class NewFromTemplateDlg : public wxDialog
{
	public:
		NewFromTemplateDlg(const wxArrayString& user_templates);
		virtual ~NewFromTemplateDlg();

		ProjectTemplateLoader* GetTemplate(){ return m_Template; }
		cbWizardPlugin* GetWizard(){ return m_pWizard; }
		int GetWizardIndex(){ return m_WizardIndex; }
		cbWizardPlugin* NewFromTemplateDlg::GetSelectedTemplate();
		bool SelectedUserTemplate();
		wxString GetSelectedUserTemplate();

		void EndModal(int retCode);
	protected:
		void FillTemplate(ProjectTemplateLoader* pt);
		void BuildCategories();
		void BuildCategoriesFor(cbWizardPlugin::OutputType otype, wxComboBox* cat);
		void BuildList();
		void BuildListFor(cbWizardPlugin::OutputType otype, wxListCtrl* list, wxComboBox* cat);
		void ClearList();
		void ClearListFor(wxListCtrl* list);
		void OnListRightClick(wxListEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
		void OnEditScript(wxCommandEvent& event);
		void OnEditGlobalScript(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);

		void EditScript(const wxString& relativeFilename);
		wxListCtrl* GetVisibleListCtrl();
		wxComboBox* GetVisibleCategory();
		cbWizardPlugin::OutputType GetVisibleOutputType();
	private:
		ProjectTemplateLoader* m_Template;
		cbWizardPlugin* m_pWizard;
		int m_WizardIndex;
		PluginsArray m_Wizards;
		DECLARE_EVENT_TABLE()
};

#endif // NEWFROMTEMPLATEDLG_H

