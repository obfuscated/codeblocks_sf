#ifndef CLASSBROWSER_H
#define CLASSBROWSER_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <manager.h>
#include <xtra_classes.h>
#include "parser/parser.h"
#include "parser/token.h"

class NativeParser;
class wxListCtrl;
class wxTextCtrl;

/*
 * No description
 */
class ClassBrowser : public wxSplitPanel
{
	public:
		// class constructor
		ClassBrowser(wxNotebook* parent, NativeParser* np);
		// class destructor
		~ClassBrowser();
		void SetParser(Parser* parser);
		const Parser& GetParser(){ return *m_pParser; }
		void Update();
    private:
        friend class myTextCtrl;
		void OnTreeItemClick(wxTreeEvent& event);
		void OnTreeItemDoubleClick(wxTreeEvent& event);
    	void OnTreeItemRightClick(wxTreeEvent& event);
    	void OnListItemRightClick(wxTreeEvent& event);
        void OnJumpTo(wxCommandEvent& event);
        void OnRefreshTree(wxCommandEvent& event);
        void OnForceReparse(wxCommandEvent& event);
		void OnCBViewMode(wxCommandEvent& event);
		void OnViewScope(wxCommandEvent& event);
		void OnSearch(wxCommandEvent& event);
		bool RecursiveSearch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& parent, wxTreeItemId& result);
        void ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, const wxPoint& pt);
        wxNotebook* m_Parent;
        NativeParser* m_NativeParser;
        int m_PageIndex;
        wxTreeCtrl* m_Tree;
        wxTreeCtrl* m_List;
        wxTextCtrl* m_Search;
        wxTreeCtrl* m_TreeForPopupMenu;
		Parser* m_pParser;
		wxTreeItemId m_RootNode;

        DECLARE_EVENT_TABLE()
};

#endif // CLASSBROWSER_H

