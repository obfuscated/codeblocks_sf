#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include "settings.h"
#include "newfromtemplatedlg.h"
#include <wx/event.h>

//forward decls
class wxMenuBar;
class wxMenu;

class DLLIMPORT TemplateManager : public wxEvtHandler
{
	public:
        static TemplateManager* Get();
		static void Free();
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		void BuildToolsMenu(wxMenu* menu);
		void NewProject();
	protected:
		void LoadTemplates();
		void OnNew(wxCommandEvent& event);
		ProjectTemplateArray m_Templates;
	private:
		TemplateManager();
		virtual ~TemplateManager();
		
		DECLARE_EVENT_TABLE();
};

#endif // TEMPLATEMANAGER_H

