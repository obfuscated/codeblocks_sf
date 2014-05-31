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

/** symbol browser panel is shown in the Management panel besides projects browser panel.
 *
 *
 *  It mainly have four parts, see the simple diagram below.
 *
 *     +----------------+
 *     | view option    |
 *     +----------------+
 *     | search box     |
 *     +----------------+
 *     |  top tree      |
 *     |                |
 *     +----------------+
 *     |  bottom tree   |
 *     |                |
 *     +----------------+
 *
 *  1, the view option is a list box to let user choose which tokens should be shown in the trees
 *     it can either show the tokens for the whole workspace, or only the active C::B project, or
 *     the active editor (source files)
 *  2, search box, a combo-box lets user enter texts for searching a specific symbol
 *  3, top tree, a wxTreeCtrl show the mostly top level symbols, such as the global namespaces,
 *     macro definitions, class definition in global namesapces.
 *  4, bottom tree, show members of the select item in the top tree, e.g. if a user select a class,
 *     then, this tree will show all the member variables of the class. Note, this tree can be hidden
 *     by the user, so only the top tree is shown on the panel.
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

    /** update or refresh the symbol browser trees, there are many cases the tree need to be updated.
     *  E.g. if the View option of the tree is the "Current file's symbols", the user switch the editor
     *  to a new source file, then the tree should be updated(rebuild).
     *  @param checkHeaderSwap if true, we should check if the new editor opened has the same base file
     *  as the old file. E.g. If you are currently viewing a file named A.cpp, and you switch to A.h
     *  in this case, the tree should not be updated, because tokens(symbols) in both files were already
     *  shown. False if you need to update the tree without such optimization.
     */
    void  UpdateClassBrowserView(bool checkHeaderSwap = false);

    /** update the position sash bar between top tree and the bottom tree, the position (percentage)
     *  of the two trees are saved in the configuration files.
     */
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

    void ThreadedBuildTree(cbProject* activeProject);

    void OnTreeItemExpanding(wxTreeEvent& event);
#ifndef CC_NO_COLLAPSE_ITEM
    void OnTreeItemCollapsing(wxTreeEvent& event);
#endif // CC_NO_COLLAPSE_ITEM
    void OnTreeSelChanged(wxTreeEvent& event);

    /** class browser builder thread will send notification event to the parent, this is the event
     *  handler function
     */
    void OnThreadEvent(wxCommandEvent& event);

private:
    NativeParser*              m_NativeParser;

    /** the top(main) level tree control, see above diagram for details*/
    CCTreeCtrl*                m_CCTreeCtrl;

    /** the bottom tree control, mainly used to show the member variable and member functions*/
    CCTreeCtrl*                m_CCTreeCtrlBottom;

    wxTreeCtrl*                m_TreeForPopupMenu;

    wxComboBox*                m_Search;
    ParserBase*                m_Parser;

    /** source file name of active editor, used for filtering(if view option is Current file's symbols)*/
    wxString                   m_ActiveFilename;

    /** semaphore to synchronize the GUI(class browser) and the tree builder thread, when the GUI
     *  post the semaphore, the waiting thread start doing the job
     */
    wxSemaphore                m_ClassBrowserSemaphore;

    /** a wxThread used to build the wxTreeCtrl for the top and bottom in the class(symbol) browser
     *  because it always take many seconds to build the trees, so those work were delegated to a
     *  worker thread.
     */
    ClassBrowserBuilderThread* m_ClassBrowserBuilderThread;

    DECLARE_EVENT_TABLE()
};

#endif // CLASSBROWSER_H
