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
 *  It mainly has four parts, see the simple diagram below.
 * @code
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
 * @endcode
 *  1, the view option is a list box to let user choose which tokens should be shown in the trees
 *     it can either show the tokens for the whole workspace, or only the active C::B project, or
 *     the active editor (source files)
 *  2, search box, a combo-box lets user enter texts for searching a specific symbol
 *  3, top tree, a wxTreeCtrl shows the mostly top level symbols, such as the global namespaces,
 *     macro definitions, class definition in global namesapces.
 *  4, bottom tree, shows members of the select item in the top tree, e.g. if a user select a class,
 *     then, this tree will show all the member variables of the class. Note, this tree can be hidden
 *     by the user in CC's setting dialog, in this case only the top tree is shown on the panel.
 */
class ClassBrowser : public wxPanel
{
public:
    /** class constructor
     * @param parent the parent window
     * @param np the NativeParser instance, which contains all the images for the wxTreeCtrl
     */
    ClassBrowser(wxWindow* parent, NativeParser* np);

    /** class destructor */
    virtual ~ClassBrowser();

    /** Set the Parser object associated with the class browser
     *
     *  the browser tree must access to the TokenTree, so it needs a ParserBase pointer
     *  @param parser the Parser instance
     */
    void  SetParser(ParserBase* parser);

    /** update or refresh the symbol browser trees
     *
     *  There are many cases the tree need to be updated.
     *  E.g. if the View option of the tree is the "Current file's symbols", the user switch the editor
     *  to a new source file, then the tree should be updated(rebuild).
     *  @param checkHeaderSwap if true, we should check if the new editor opened has the same base file
     *  as the old file. E.g. If you are currently viewing a file named A.cpp, and you switch to A.h
     *  in this case, the tree should not be updated, because tokens(symbols) in both files were already
     *  shown. False if you need to update the tree without such optimization.
     */
    void  UpdateClassBrowserView(bool checkHeaderSwap = false);

    /** update the position sash bar between top tree and the bottom tree, the position (percentage)
     *  of the two trees are saved in the C::B's configuration file.
     */
    void  UpdateSash();

private:
    /** handler for the mouse double click on a tree item, we usually make a jump to the
     *  associated token's position.
     */
    void OnTreeItemDoubleClick(wxTreeEvent& event);

    /** show a context menu */
    void OnTreeItemRightClick(wxTreeEvent& event);

    // those are the handler for context menus
    /** the handler for jump to declaration or jump to definition menu item */
    void OnJumpTo(wxCommandEvent& event);

    /** force rebuilding the tree */
    void OnRefreshTree(wxCommandEvent& event);

    /** reparse the current project */
    void OnForceReparse(wxCommandEvent& event);

    /** user change the view mode (View Inheritance or View the bottom tree) */
    void OnCBViewMode(wxCommandEvent& event);

    /** whether automatically expand the namespace option switch */
    void OnCBExpandNS(wxCommandEvent& event);

    /** the view scope choice has changed, user can switch between view everything, view a single
     *  file, or view the project
     */
    void OnViewScope(wxCommandEvent& event);

    /** whether print the debug log message */
    void OnDebugSmartSense(wxCommandEvent& event);

    /** sort type changed */
    void OnSetSortType(wxCommandEvent& event);

    /** when user try to search a word in the symbols tree */
    void OnSearch(wxCommandEvent& event);

    /** string compare between the search and the token's name associated with item
     *  @param search the search key
     *  @param tree tree control
     *  @param item the item id of the tree
     *  @return true if their name matches
     */
    bool FoundMatch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& item);

    /** get the next item of the "start" item, if no next item in the current level, go up one level
     *  @note it looks like the "search" parameter is not used in this function
     *  @code
     *  --node1
     *    |--start item
     *    |--returned item(a)
     *  --node2
     *    |--returned item(b)
     *  @endcode
     *  return the item(a) in the same level, if there is no such item, then return item(b)
     */
    wxTreeItemId FindNext(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start);

    /** get a child of the parent item, which matches the "search" key
     *  @param recurse whether recursive search to the children of the children
     *  @partialMatch "search" key is the prefix of the item's text
     */
    wxTreeItemId FindChild(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start, bool recurse = false, bool partialMatch = false);

    /** find an item whose item name matches the "search" key string in the sub-tree
     *  @param parent search will start from this parent item, and recursively to the sub-tree
     */
    bool RecursiveSearch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& parent, wxTreeItemId& result);

    /** build and show a context menu when user right click on the tree item "id" */
    void ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, const wxPoint& pt);

    /** create a thread to update the symbol tree, if the thread is already created, just pause and
     *  resume the thread.
     */
    void ThreadedBuildTree(cbProject* activeProject);

    /** expanding one node of top tree
     *  @note that the bottom tree do not actually show a tree structure, it just list the members
     *  of the selected node in the top tree
     */
    void OnTreeItemExpanding(wxTreeEvent& event);

#ifndef CC_NO_COLLAPSE_ITEM
    /** collapse one node of the top tree */
    void OnTreeItemCollapsing(wxTreeEvent& event);
#endif // CC_NO_COLLAPSE_ITEM

    /** item selection changed in the top tree */
    void OnTreeSelChanged(wxTreeEvent& event);

    /** class browser builder thread will send notification event to the parent, this is the event
     *  handler function
     *  currently, there are three kinds of events send from the builder thread
     *  @see EThreadEvent for details.
     */
    void OnThreadEvent(wxCommandEvent& event);

private:

    /** the pointer to parser manager object */
    NativeParser*              m_NativeParser;

    /** the top(main) level tree control, see above diagram for details */
    CCTreeCtrl*                m_CCTreeCtrl;

    /** the bottom tree control, mainly used to show the member variable and member functions */
    CCTreeCtrl*                m_CCTreeCtrlBottom;

    /** remember the context menu is created from which tree control, the upper or the bottom */
    wxTreeCtrl*                m_TreeForPopupMenu;

    /** the search combobox in the header of the window */
    wxComboBox*                m_Search;

    /** a pointer to the associated parser object */
    ParserBase*                m_Parser;

    /** source file name of active editor, used for filtering(if view option is Current file's symbols) */
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
