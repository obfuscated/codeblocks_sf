#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include "settings.h"
#include "manager.h"
#include "cbplugin.h"
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

		cbProject* New(TemplateOutputType initial = totProject, wxString* pFilename = 0);
		wxString GetLastCreatedFilename() const;
		void SaveUserTemplate(cbProject* prj);
	protected:
		void LoadTemplates();
		void LoadUserTemplates();
		cbProject* NewFromTemplate(NewFromTemplateDlg& dlg, wxString* pFilename = 0);
		cbProject* NewProjectFromUserTemplate(NewFromTemplateDlg& dlg, wxString* pFilename = 0);
		wxArrayString m_UserTemplates;
	private:
		TemplateManager();
		virtual ~TemplateManager();
};

#endif // TEMPLATEMANAGER_H
