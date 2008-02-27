/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGERTREE_H
#define DEBUGGERTREE_H

#include <vector>

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
        WatchTreeData(Watch* w) : m_pWatch(w) {}
        Watch* m_pWatch;
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
        // recursive struct to keep all watch tree entries
        struct WatchTreeEntry
        {
            wxString name; // entry's name
            std::vector<WatchTreeEntry> entries; // child entries
            Watch* watch; // the associated watch

            WatchTreeEntry() : watch(0) {}

            void Clear()
            {
                name.Clear();
                watch = 0;
                entries.clear();
            }
            WatchTreeEntry& AddChild(const wxString& childname, Watch* childwatch)
            {
                WatchTreeEntry wet;
                wet.name = childname;
                wet.watch = childwatch;
                entries.push_back(wet);
                return *(--entries.end());
            }
        };
        WatchTreeEntry m_RootEntry;

        void BuildTree(WatchTreeEntry& entry, wxTreeItemId parent);

        void ParseEntry(WatchTreeEntry& entry, Watch* watch, wxString& text, long array_index = -1);
        int FindCharOutsideQuotes(const wxString& str, wxChar ch); // returns position of ch in str
        int FindCommaPos(const wxString& str); // ignores commas in function signatures
        void FixupVarNameForChange(wxString& str);

        bool m_InUpdateBlock;
        DECLARE_EVENT_TABLE()
};

#endif // DEBUGGERTREE_H

