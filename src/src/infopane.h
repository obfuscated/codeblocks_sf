#ifndef MLDOOORDE
#define MLDOOORDE


#include <logmanager.h>

#ifdef CB_USE_AUI_NOTEBOOK
    #include <wx/aui/auibook.h>
    typedef wxAuiNotebook InfoPaneNotebook;
    typedef wxAuiNotebook PieceOfShitBaseClass;
    static const int infopane_flags = wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT;
#else
    #include <wx/wxFlatNotebook/wxFlatNotebook.h>
    typedef wxFlatNotebook InfoPaneNotebook;
    typedef wxPanel PieceOfShitBaseClass;
    static const int infopane_flags = wxFNB_NO_X_BUTTON | wxFNB_FF2;
#endif

class wxWindow;
class wxCommandEvent;
class wxMouseEvent;

class InfoPane : public InfoPaneNotebook
{
    DECLARE_EVENT_TABLE()

    struct Page
    {
        Page() : icon(0), window(0), logger(0), indexInNB(-1), islogger(0) {};
        wxString title;
        wxBitmap* icon;
        wxWindow* window;
        Logger* logger;
        int indexInNB; // used to be "visible" flag: invisible is <0, any other value means visible
        bool islogger;
    };

    static const int num_pages = ::max_logs + 8;
    wxBitmap defaultBitmap;

    Page page[num_pages];
    const int baseID;

    void Toggle(size_t index);
    void Hide(size_t i);

    void OnMenu(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void ContextMenu(wxContextMenuEvent& event);

	int AddPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon = 0);
public:

    InfoPane(wxWindow* parent);
    ~InfoPane();

    /*
    *  Show a tab, or bring it to the foreground if already shown.
    *  Only use this method if crucial information is shown which the user must not miss, or if
    *  information is shown that requires input from the user (for example search results).
    *  In all other cases, it is the user's decision which tab he wants to look at, not yours.
    *  Note that you cannot hide a tab programatically.
    */
    void Show(size_t index);
    void Show(Logger* logger);
    void ShowNonLogger(wxWindow* p);


    /*
    *  You should not need to call these functions under normal conditions. The application initialises
    *  everything at startup, after the main window has been created and the plugins have been loaded.
    *  AddLogger:
    *  Add the GUI control obtained by a logger's CreateControl() function.
    *  DeleteLogger:
    *  Remove (delete) a tab and its logger. All log output directed to the corresponging log index
    *  will be redirected to the null log thereafter.
    *  To prove that you are serious, you must know the logger belonging to the tab to delete.
    */
    int AddLogger(Logger* logger, wxWindow* p, const wxString& title, wxBitmap* icon = 0);
    bool DeleteLogger(Logger* l);

    /*
    *  If something that is not a logger should be placed in the info pane (think twice about this before doing it),
    *  use AddNonLogger()/DeleteNonLogger() for that purpose.
    *  An example of something that is not a logger but might still show up in the info pane is the list of search results.
    */
    int AddNonLogger(wxWindow* p, const wxString& title, wxBitmap* icon = 0);
    bool RemoveNonLogger(wxWindow* p);
    bool DeleteNonLogger(wxWindow* p);
};


#endif
