#ifndef CLASSBROWSER_H
#define CLASSBROWSER_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <manager.h>
#include <wx/panel.h> // inheriting class's header file
#include "parser/parser.h"
#include "parser/token.h"

/*
 * No description
 */
class ClassBrowser : public wxPanel
{
	public:
		// class constructor
		ClassBrowser(wxNotebook* parent);
		// class destructor
		~ClassBrowser();
		void SetParser(Parser* parser);
		const Parser& GetParser(){ return *m_pParser; }
		void Update();
    private:
    	void OnTreeItemRightClick(wxTreeEvent& event);
        void OnJumpTo(wxCommandEvent& event);
        void OnRefreshTree(wxCommandEvent& event);
		void OnCBViewMode(wxCommandEvent& event);
        void ShowMenu(wxTreeItemId id, const wxPoint& pt);
        wxNotebook* m_Parent;
        int m_PageIndex;
        wxTreeCtrl* m_Tree;
		Parser* m_pParser;
		wxTreeItemId m_RootNode;

        DECLARE_EVENT_TABLE()
};

#endif // CLASSBROWSER_H

