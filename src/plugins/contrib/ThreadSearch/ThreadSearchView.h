/***************************************************************
 * Name:      ThreadSearchView
 * Purpose:   This class implements the panel that is added to
 *            Code::Blocks Message notebook or layout.
 *            It runs the search worker thread and receives
 *            ThreadSearchEvent from it to update the list log.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_VIEW_H
#define THREAD_SEARCH_VIEW_H

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/timer.h>
#include <wx/dynarray.h>
#include <wx/datetime.h>
#include <wx/panel.h>
#include <wx/arrstr.h>

#include "editormanager.h"
#include "cbproject.h"
#include "projectmanager.h"

#include "ThreadSearchLoggerBase.h"

class wxToolBar;
class wxStaticBox;
class wxComboBox;
class wxButton;
class wxStaticText;
class cbStyledTextCtrl;
class wxPanel;
class wxMouseEvent;
class wxCommandEvent;
class wxSplitterEvent;
class wxScintillaEvent;
class wxContextMenuEvent;
class wxSplitterWindow;
class wxStaticBoxSizer;
class wxBoxSizer;
class wxBitmap;
class wxBitmapButton;
class wxStaticLine;
class wxImageList;

class ThreadSearch;
class ThreadSearchEvent;
class ThreadSearchThread;
class ThreadSearchFindData;
class SearchInPanel;
class DirectoryParamsPanel;


class ThreadSearchView: public wxPanel {

    friend class ThreadSearch;
public:
    // begin wxGlade: ThreadSearchView::ids
    // end wxGlade

    /** Constructor. */
    ThreadSearchView(ThreadSearch& threadSearchPlugin);

    /** Destructor. */
    ~ThreadSearchView();

    /** Runs the worker thread search.
      * @param aFindData : structure containing all search parameters :
      *                    text, whole word, case, directory...
      */
    void ThreadedSearch(const ThreadSearchFindData& aFindData);

    /** No comment
      * @return true if a search is running, false otherwise
      */
    bool IsSearchRunning();

    /** This function is called from ThreadSearchThread::OnExit.
      * This must not be called directly.
      */
    void OnThreadExit();

    /** Makes instance update its graphical widgets.
      * Should be called by ThreadSearch instance after m_ThreadSearchPlugin modification
      */
    void Update();

    /** Loads file in code preview and makes line visible.
      * @param file : file path
      * @param line : line to display in the preview editor
      * @return true if success
      */
    bool UpdatePreview(const wxString& file, long line);

    /** Sets/gets the splitter window sash position. */
    void SetSashPosition(int position, const bool redraw = true);
    int  GetSashPosition() const;

    /** Sets/gets the search history */
    void          SetSearchHistory(const wxArrayString& searchPatterns, const wxArrayString& searchDirs,
                                   const wxArrayString& searchMasks);
    wxArrayString GetSearchHistory() const;
    wxArrayString GetSearchDirsHistory() const;
    wxArrayString GetSearchMasksHistory() const;

    /** SetLoggerType
      * Sets the logger type. If value is different from current one,
      * m_pLogger is destroyed and rebuilt on the fly.
      * @param lgrType : type of logger, can be a wxListCtrl or a wxTreeCtrl
      */
    void SetLoggerType(ThreadSearchLoggerBase::eLoggerTypes lgrType);

    /** PostThreadSearchEvent
      * @param event : event sent by the worker thread (ThreadSearchThread)
      * Thread safe mechanism. Clone the worker thread event to the mutex protected events array.
      */
    void PostThreadSearchEvent(const ThreadSearchEvent& event);

    /** SetToolBar
      * C::B plugins manager provides a toolbar instance to ThreadSearch instance
      * during init. This instance is referenced in ThreadSearchView by m_pToolBar.
      */
    void SetToolBar(wxToolBar* pToolBar) {m_pToolBar = pToolBar;}


    /** @return A string with the path to the icon images, it takes into account the size of the images.
      */
    wxString GetImagePrefix() const;

    /** Set the proper image depending if there are any enabled options in the popup menu.
      */
    void UpdateOptionsButtonImage(const ThreadSearchFindData &findData);

    /** This method shows/hide the search graphical controls.
      * @param show : show = true/hide = false toolbar
      */
    void ShowSearchControls(bool show);

    /** This method applies splitter settings
      * @param showCodePreview : show = true/hide = false preview editor (=~ splitted or not)
      * @param splitterMode : wxSPLIT_HORIZONTAL or wxSPLIT_VERTICAL
      */
    void ApplySplitterSettings(bool showCodePreview, long splitterMode);

    void FocusSearchCombo(const wxString &searchWord);
private:
    // begin wxGlade: ThreadSearchView::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    /** ClearThreadSearchEventsArray
      * Removes all events sent by the worker thread and stored in the array.
      * Thread safe method.
      @return true if clear successful. Not successful if Mutex is not caught.
      */
    bool ClearThreadSearchEventsArray();

    /** StopThread
      * Called to stop the thread and manage all required operations.
      * @return true if successful.
      */
    bool StopThread();

    // BEGIN Duplicated from cbeditor.cpp to apply folding options
    void SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back);
    void UnderlineFoldedLines(bool underline);
    void SetFoldingIndicator(int id);
    // END Duplicated from cbeditor.cpp to apply folding options

    ThreadSearchThread* m_pFindThread;             // Worker thread pointer. Must be allocated on the heap.
    ThreadSearch&       m_ThreadSearchPlugin;      // Thread search plugin reference. 'Subject' in the observer pattern.
    wxString            m_PreviewFilePath;         // File currently previewed path. Used to avoid reloading files.
    wxDateTime          m_PreviewFileDate;         // File currently previewed modification time. Used to avoid reloading files.

    wxMutex             m_MutexSearchEventsArray;  // Mutex used for multi thread access to m_ThreadSearchEventsArray
    wxArrayPtrVoid      m_ThreadSearchEventsArray; // Used to store events sent by the worker thread.
    wxTimer             m_Timer;                   // Used for events update
    long                m_StoppingThread;          // Used not to do more than one thread termination.

protected:
    // Graphical widgets managed by wxGlade
    // begin wxGlade: ThreadSearchView::attributes
    wxStaticBox* m_pSizerSearchDirItems_staticbox;
    wxComboBox* m_pCboSearchExpr;
    wxBitmapButton* m_pBtnSearch;
    wxBitmapButton* m_pBtnOptions;
    wxStaticLine* m_pStaticLine1;
    wxStaticText* m_pStaTxtSearchIn;
    SearchInPanel* m_pPnlSearchIn;
    wxStaticLine* m_pStaticLine2;
    wxBitmapButton* m_pBtnShowDirItems;
    DirectoryParamsPanel* m_pPnlDirParams;
    cbStyledTextCtrl* m_pSearchPreview;
    wxPanel* m_pPnlPreview;
    ThreadSearchLoggerBase* m_pLogger;
    wxPanel* m_pPnlListLog;
    wxSplitterWindow* m_pSplitter;
    // end wxGlade
    wxStaticBoxSizer* m_pSizerSearchDirItems;
    wxBoxSizer*       m_pSizerSearchItems;
    wxToolBar*        m_pToolBar;

    DECLARE_EVENT_TABLE()

    /** AddExpressionToSearchCombos
      * Inserts expression to search combo in first position.
      * If expression is already listed, it is removed before insertion.
      * Used to keep the search history in the combo box.
      * @param expression : searched text.
      * @param path : searched path.
      * @param mask : searched mask.
      */
    void AddExpressionToSearchCombos(const wxString& expression, const wxString& path, const wxString& mask);

    /// enum defining the possible labels to update Search buttons (view and toolbars)
    /// Undefined can be used to skip label update
    enum eSearchButtonLabel {
        search,
        cancel,
        skip
    };

    /** UpdateSearchButtons
      * Updates wxButtons and wxBitmap buttons according to new label.
      * @param enable : true : enable; false : disable
      * @param label  : see eSearchButtonLabel declaration
      */
    void UpdateSearchButtons(bool enable, eSearchButtonLabel label = skip);

    /** Method used to enable/disable the graphical widgets related
      * to search parameters. Disabled when thread is running.
      * @param enable : true : enable; false : disable
      */
    void EnableControls(bool enable);

public:
    void OnThreadSearchErrorEvent(const ThreadSearchEvent& event);
    void OnCboSearchExprEnter(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnBtnSearchClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnBtnOptionsClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnBtnShowDirItemsClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnSplitterDoubleClick(wxSplitterEvent &event); // wxGlade: <event_handler>

    void OnShowOptionsDialog(wxCommandEvent &event);
    void OnQuickOptions(wxCommandEvent &event);
    void OnQuickOptionsUpdateUI(wxUpdateUIEvent &event);

    void OnMarginClick(wxScintillaEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnLoggerClick      (const wxString& file, long line);  // Called by ThreadSearchLoggerBase derived instance
                                                                // when user clicks on a search result
    void OnLoggerDoubleClick(const wxString& file, long line);  // Called by ThreadSearchLoggerBase derived instance
                                                                // when user double clicks on a search result

    void OnBtnSearchOpenFiles(wxCommandEvent &event);
    void OnBtnSearchTargetFiles(wxCommandEvent &event);
    void OnBtnSearchProjectFiles(wxCommandEvent &event);
    void OnBtnSearchWorkspaceFiles(wxCommandEvent &event);
    void OnBtnSearchDirectoryFiles(wxCommandEvent &event);

    void OnTmrListCtrlUpdate(wxTimerEvent& event);
}; // wxGlade: end class


#endif // THREAD_SEARCH_VIEW_H
