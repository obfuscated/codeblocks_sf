#ifndef DEBUGGERTREE_H
#define DEBUGGERTREE_H

#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/dynarray.h>

#include "debugger_defs.h"

extern int cbCustom_WATCHES_CHANGED;

class WatchTreeData : public wxTreeItemData
{
    public:
        WatchTreeData(Watch* w) : m_pWatch(w), m_UpToDate(true) {}
        Watch* m_pWatch;
        bool m_UpToDate;
};

class DebuggerGDB;

class DebuggerTree : public wxPanel
{
    public:
        DebuggerTree(wxWindow* parent, DebuggerGDB* debugger);
        virtual ~DebuggerTree();
        wxTreeCtrl* GetTree(){ return m_pTree; }

        void BeginUpdateTree(); ///< Clears and freezes the tree for massive updates
        void BuildTree(Watch* watch, const wxString& infoText, WatchStringFormat fmt); ///< adds a new node to the tree, parsing @c infoText
        void EndUpdateTree(); ///< Un-freezes the tree

        void ClearWatches();
        void SetWatches(const WatchesArray& watches);
        const WatchesArray& GetWatches();
        void AddWatch(const wxString& watch, WatchFormat format = Undefined, bool notify = true);
        void DeleteWatch(Watch* watch, bool notify = true);
        void DeleteWatch(const wxString& watch, WatchFormat format = Any, bool notify = true);
        void DeleteAllWatches();
        Watch* FindWatch(const wxString& watch, WatchFormat format = Any);
        int FindWatchIndex(const wxString& watch, WatchFormat format = Any);
    protected:
        void BuildTreeGDB(Watch* watch, const wxString& infoText);
        void BuildTreeCDB(Watch* watch, const wxString& infoText);
        void NotifyForChangedWatches();
        void ShowMenu(wxTreeItemId id, const wxPoint& pt);
        void OnTreeRightClick(wxTreeEvent& event);
        void OnRightClick(wxCommandEvent& event);
        void OnAddWatch(wxCommandEvent& event);
        void OnLoadWatchFile(wxCommandEvent& event);
        void OnSaveWatchFile(wxCommandEvent& event);
        void OnEditWatch(wxCommandEvent& event);
        void OnDeleteWatch(wxCommandEvent& event);
        void OnDeleteAllWatches(wxCommandEvent& event);
        void OnDereferencePointer(wxCommandEvent& event);
        void OnWatchThis(wxCommandEvent& event);
        void OnChangeValue(wxCommandEvent& event);

        wxTreeCtrl* m_pTree;
        DebuggerGDB* m_pDebugger;
        WatchesArray m_Watches;
        wxArrayString m_TreeState;
        int m_NumUpdates;
        int m_CurrNumUpdates;
    private:
        void MarkAllNodes(const wxTreeItemId& parent, bool uptodate);
        void ClearAllMarkedNodes(const wxTreeItemId& parent, bool uptodate);
        wxTreeItemId AddItem(wxTreeItemId& parent, const wxString& text, Watch* watch, bool* newlyAdded = 0);
        bool FindChildItem(const wxString& item, const wxTreeItemId& parent, wxTreeItemId& result);
        void ParseEntry(Watch* watch, wxTreeItemId& parent, wxString& text);
        int FindCharOutsideQuotes(const wxString& str, wxChar ch); // returns position of ch in str
        int FindCommaPos(const wxString& str); // ignores commas in function signatures
        void FixupVarNameForChange(wxString& str);

        bool m_InUpdateBlock;
        DECLARE_EVENT_TABLE()
};

#endif // DEBUGGERTREE_H

