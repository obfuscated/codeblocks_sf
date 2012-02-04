/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSER_H
#define CLASSBROWSER_H

#include <wx/panel.h>

#include <settings.h> // SDK
#include <cbplugin.h>
#include <manager.h>

#include "cctreectrl.h"
#include "classbrowserbuilderthread.h"
#include "parser/parser.h"
#include "parser/token.h"

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
public:
    // class constructor
    ClassBrowser(wxWindow* parent, NativeParser* np);
    // class destructor
    virtual ~ClassBrowser();

    const wxTreeCtrl* GetCCTreeCtrl() { return m_CCTreeCtrl; }
    void  SetParser(ParserBase* parser);
    const ParserBase& GetParser()     { return *m_Parser; }
    const ParserBase* GetParserPtr()  { return  m_Parser; }
    void  UpdateClassBrowserView(bool checkHeaderSwap = false);
    void  UpdateSash();

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

    void ThreadedBuildTree();

    void OnTreeItemExpanding(wxTreeEvent& event);
#ifndef CC_NO_COLLAPSE_ITEM
    void OnTreeItemCollapsing(wxTreeEvent& event);
#endif // CC_NO_COLLAPSE_ITEM
    void OnTreeSelChanged(wxTreeEvent& event);

    void OnMakeSelectItem(wxCommandEvent& event);

private:
    NativeParser*              m_NativeParser;
    CCTreeCtrl*                m_CCTreeCtrl;
    CCTreeCtrl*                m_CCTreeCtrlBottom;
    wxTreeCtrl*                m_TreeForPopupMenu;

    wxComboBox*                m_Search;
    ParserBase*                m_Parser;

    // filtering
    wxString                   m_ActiveFilename;
    cbProject*                 m_ActiveProject;

    wxSemaphore                m_ClassBrowserSemaphore;
    ClassBrowserBuilderThread* m_ClassBrowserBuilderThread;

    DECLARE_EVENT_TABLE()
};

#endif // CLASSBROWSER_H
