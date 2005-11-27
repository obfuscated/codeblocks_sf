#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include "settings.h"
#include "newfromtemplatedlg.h"
#include <wx/event.h>

//forward decls
class wxMenuBar;
class wxMenu;
class cbProject;

class DLLIMPORT TemplateManager : public wxEvtHandler
{
	public:
        static TemplateManager* Get();
		static void Free();
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		void BuildToolsMenu(wxMenu* menu);
		cbProject* NewProject();
		void SaveUserTemplate(cbProject* prj);
	protected:
		void LoadTemplates();
		void LoadUserTemplates();
		cbProject* NewProjectFromTemplate(NewFromTemplateDlg& dlg);
		cbProject* NewProjectFromUserTemplate(NewFromTemplateDlg& dlg);
		void OnNew(wxCommandEvent& event);
		ProjectTemplateArray m_Templates;
		wxArrayString m_UserTemplates;
	private:
		TemplateManager();
		virtual ~TemplateManager();

		DECLARE_EVENT_TABLE();
};

#endif // TEMPLATEMANAGER_H

