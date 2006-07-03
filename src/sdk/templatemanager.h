#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include "settings.h"
#include "manager.h"
#include <wx/event.h>

//forward decls
class wxMenuBar;
class wxMenu;
class cbProject;
class NewFromTemplateDlg;

class DLLIMPORT TemplateManager : public Mgr<TemplateManager>, public wxEvtHandler
{
        friend class Mgr<TemplateManager>;
	public:
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
		wxArrayString m_UserTemplates;
	private:
		TemplateManager();
		virtual ~TemplateManager();
};

#endif // TEMPLATEMANAGER_H
