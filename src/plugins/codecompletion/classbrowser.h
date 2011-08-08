/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSER_H
#define CLASSBROWSER_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <manager.h>
#include <wx/panel.h>
#include "parser/parser.h"
#include "parser/token.h"
#include "classbrowserbuilderthread.h"

class NativeParser;
class wxComboBox;
class wxTreeCtrl;
class wxTextCtrl;
class cbProject;

/*
 * No description
 */
class ClassBrowser : public wxPanel
{
    friend class myTextCtrl;
public:
    // class constructor
    ClassBrowser(wxWindow* parent, NativeParser* np);
    // class destructor
    virtual ~ClassBrowser();
    const wxTreeCtrl* GetTree() { return m_Tree; }
    void SetParser(ParserBase* parser);
    const ParserBase& GetParser(){ return *m_Parser; }
    const ParserBase* GetParserPtr() { return m_Parser; }
    void UpdateView(bool checkHeaderSwap = false);
    void UpdateSash();

private:
    void OnTreeItemDoubleClick(wxTreeEvent& event);
    void OnTreeItemRightClick(wxTreeEvent& event);
    void OnJumpTo(wxCommandEvent& event);
    void OnRefreshTree(wxCommandEvent& event);
    void OnForceReparse(wxCommandEvent& event);
    void OnCBViewMode(wxCommandEvent& event);
    void OnCBExpandNS(wxCommandEvent& event);
    void OnViewScope(wxCommandEvent& event);
    void OnDebugSmartSense(wxCommandEvent& event);
    void OnSetSortType(wxCommandEvent& event);

    void OnSearch(wxCommandEvent& event);
    bool FoundMatch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& item);
    wxTreeItemId FindNext(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start);
    wxTreeItemId FindChild(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start, bool recurse = false, bool partialMatch = false);
    bool RecursiveSearch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& parent, wxTreeItemId& result);

    void ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, const wxPoint& pt);

    void BuildTree();

    void OnTreeItemSelected(wxTreeEvent& event);
    void OnTreeItemExpanding(wxTreeEvent& event);
#ifndef CC_NO_COLLAPSE_ITEM
    void OnTreeItemCollapsing(wxTreeEvent& event);
#endif // CC_NO_COLLAPSE_ITEM

private:
    NativeParser*              m_NativeParser;
    CBTreeCtrl*                m_Tree;
    CBTreeCtrl*                m_TreeBottom;
    wxComboBox*                m_Search;
    wxTreeCtrl*                m_TreeForPopupMenu;
    ParserBase*                m_Parser;

    // filtering
    wxString                   m_ActiveFilename;
    cbProject*                 m_ActiveProject;

    wxSemaphore                m_Semaphore;
    ClassBrowserBuilderThread* m_BuilderThread;

    DECLARE_EVENT_TABLE()
};

#endif // CLASSBROWSER_H
