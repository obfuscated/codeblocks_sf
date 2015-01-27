#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/combobox.h>
#include <wx/dynarray.h>

#include <vector>
#include "FileExplorerSettings.h"
#include "FileExplorerUpdater.h"
#include "directorymonitor.h"

class UpdateQueue;

class Expansion;

typedef std::vector<Expansion*> ExpList;

class VCSstate
{
public:
    int state;
    wxString path;
};

WX_DECLARE_OBJARRAY(VCSstate, VCSstatearray);

class Expansion
{
public:
    Expansion() { name = _T("");}
    ~Expansion() {for(size_t i=0;i<children.size();i++) delete children[i];}
    wxString name;
    ExpList children;
};

class wxFEDropTarget;


class FileTreeCtrl: public wxTreeCtrl
{
public: //wxTR_HIDE_ROOT|
    FileTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS|wxTR_MULTIPLE|wxTR_NO_LINES,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = _T("treeCtrl"));
    FileTreeCtrl();
    FileTreeCtrl(wxWindow *parent);
    void OnKeyDown(wxKeyEvent &e);
//    void OnActivate(wxTreeEvent &event);
    virtual ~FileTreeCtrl();
//    void SortChildren(const wxTreeItemId& ti);
protected:
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
    DECLARE_DYNAMIC_CLASS(FileTreeCtrl)
    DECLARE_EVENT_TABLE()
};

class FileExplorer: public wxPanel
{
    friend class FileExplorerUpdater;
    friend class wxFEDropTarget;
public:
    FileExplorer(wxWindow *parent,wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL|wxTE_PROCESS_ENTER, const wxString& name = _T("Files"));
    ~FileExplorer();
    bool SetRootFolder(wxString root);
    wxString GetRootFolder() {return m_root;}
    void FindFile(const wxString &/*file*/) {}
    void MoveFiles(const wxString &destination, const wxArrayString &selectedfiles);
    void CopyFiles(const wxString &destination, const wxArrayString &selectedfiles);

private:
    // User initiated events
    void OnRightClick(wxTreeEvent &event);
    void OnActivate(wxTreeEvent &event);
    void OnExpand(wxTreeEvent &event);
    void OnEnterLoc(wxCommandEvent &event);
    void OnEnterWild(wxCommandEvent &event);
    void OnChooseLoc(wxCommandEvent &event);
    void OnChooseWild(wxCommandEvent &event);
    void OnSetLoc(wxCommandEvent &event);
    void OnNewFile(wxCommandEvent &event);
    void OnOpenInEditor(wxCommandEvent &event);
    void OnNewFolder(wxCommandEvent &event);
    void OnAddFavorite(wxCommandEvent &event);
    void OnCopy(wxCommandEvent &event);
    void OnDuplicate(wxCommandEvent &event);
    void OnMove(wxCommandEvent &event);
    void OnDelete(wxCommandEvent &event);
    void OnRename(wxCommandEvent &event);
    void OnExpandAll(wxCommandEvent &event);
    void OnCollapseAll(wxCommandEvent &event);
    void OnSettings(wxCommandEvent &event);
    void OnShowHidden(wxCommandEvent &event);
    void OnParseCVS(wxCommandEvent &event);
    void OnParseSVN(wxCommandEvent &event);
    void OnParseHG(wxCommandEvent &event);
    void OnParseBZR(wxCommandEvent &event);
    void OnParseGIT(wxCommandEvent &event);
    void OnUpButton(wxCommandEvent &event);
    void OnRefresh(wxCommandEvent &event);
    void OnBeginDragTreeItem(wxTreeEvent &event);
    void OnEndDragTreeItem(wxTreeEvent &event);
    void OnKeyDown(wxKeyEvent &event);

    void OnAddToProject(wxCommandEvent &event);

    // Events related to updating the Tree
    void OnDirMonitor(wxDirectoryMonitorEvent &e);
    void OnUpdateTreeItems(wxCommandEvent &event);
    void OnTimerCheckUpdates(wxTimerEvent &event);
    void OnExecRequest(wxCommandEvent &event);

    void UpdateAbort();
    void ResetDirMonitor();

    void WriteConfig();
    void ReadConfig();

    wxArrayString GetSelectedPaths();
    bool IsFilesOnly(wxArrayTreeItemIds tis);
    void FindFile(const wxString &findfilename, const wxTreeItemId &ti);
    void FocusFile(const wxTreeItemId &ti);
    bool IsInSelection(const wxTreeItemId &ti);
    wxString GetFullPath(const wxTreeItemId &ti);
    bool GetItemFromPath(const wxString &path, wxTreeItemId &ti);
    void GetExpandedNodes(wxTreeItemId ti, Expansion *exp);
    void GetExpandedPaths(wxTreeItemId ti, wxArrayString &paths);
    wxTreeItemId GetNextExpandedNode(wxTreeItemId ti);
    bool ValidateRoot();
    void Refresh(wxTreeItemId ti);
    void RefreshExpanded(wxTreeItemId ti);
    void SetImages();
    wxString m_root;
    FileTreeCtrl *m_Tree; //the widget display the file tree from root defined by m_Loc
    wxComboBox *m_Loc; // the combo box maintaining a list of useful locations and the current location
    wxComboBox *m_WildCards; // the combo box maintaining a list of wildcard filters for files
    wxButton *m_UpButton;
    bool m_show_hidden;
    wxArrayTreeItemIds m_selectti; //contains selections after context menu is called up
    FavoriteDirs m_favdirs;

    //State information required for updating the Tree in a background thread
    wxTimer *m_updatetimer;
    FileExplorerUpdater *m_updater;
    bool m_updater_cancel;
    bool m_update_expand;
    wxTreeItemId m_updating_node;
    wxTreeItemId m_updated_node;
    bool m_update_active;
    UpdateQueue *m_update_queue;
    wxDirectoryMonitor *m_dir_monitor;
    wxFEDropTarget *m_droptarget;

    int m_ticount; //number of selections
    wxString m_dragtest;
    size_t m_findmatchcount;
    wxArrayString m_findmatch;

    bool m_parse_cvs;
    bool m_parse_svn;
    bool m_parse_hg;
    bool m_parse_bzr;
    bool m_parse_git;
    bool m_kill;
    DECLARE_EVENT_TABLE()
};

//wxPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel")

//wxTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "treeCtrl")

#endif // FILEEXPLORER_H

