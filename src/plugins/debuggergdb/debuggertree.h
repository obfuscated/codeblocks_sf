#ifndef DEBUGGERTREE_H
#define DEBUGGERTREE_H

#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>

extern int cbCustom_WATCHES_CHANGED;

class DebuggerTree : public wxPanel
{
	public:
		DebuggerTree(wxEvtHandler* debugger, wxNotebook* parent);
		virtual ~DebuggerTree();
		wxTreeCtrl* GetTree(){ return m_pTree; }
		void BuildTree(const wxString& infoText);
		
		void ClearWatches();
		void SetWatches(const wxArrayString& watches);
		const wxArrayString& GetWatches();
		void AddWatch(const wxString& watch);
		void DeleteWatch(const wxString& watch);
	protected:
		void ShowMenu(wxTreeItemId id, const wxPoint& pt);
		void OnTreeRightClick(wxTreeEvent& event);
		void OnAddWatch(wxCommandEvent& event);
		void OnEditWatch(wxCommandEvent& event);
		void OnDeleteWatch(wxCommandEvent& event);
		
		void ParseEntry(const wxTreeItemId& parent, wxString& text);
		wxTreeCtrl* m_pTree;
		wxNotebook* m_pParent;
		wxEvtHandler* m_pDebugger;
		int m_PageIndex;
		wxArrayString m_Watches;
	private:
		DECLARE_EVENT_TABLE()
};

#endif // DEBUGGERTREE_H

