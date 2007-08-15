#ifndef MLDOOORDE
#define MLDOOORDE


#include <logmanager.h>

#include <wx/aui/auibook.h>
typedef wxAuiNotebook InfoPaneNotebook;   // or wxFlatNotebook, or whatever...

//#include <wx/wxFlatNotebook/wxFlatNotebook.h>
//typedef wxFlatNotebook InfoPaneNotebook;

class wxWindow;
class wxCommandEvent;
class wxMouseEvent;

class InfoPane : public InfoPaneNotebook
{
    DECLARE_EVENT_TABLE()

    struct Page
    {
        Page() : window(0), visible(0), logger(0){};
        wxString title;
        wxWindow* window;
        bool visible;
        bool logger;
    };

    static const int num_pages = ::max_logs + 8;

    Page page[num_pages];
    const int baseID;

public:
    InfoPane(wxWindow* parent);

    void Toggle(size_t index);

    bool AddLogger(wxWindow* p, const wxString& title);
    bool DeleteLogger(wxWindow* p, Logger* l);

    bool AddNonLogger(wxWindow* p, const wxString& title);
    bool DeleteNonLogger(wxWindow* p);

    void OnMenu(wxCommandEvent& event);
    void ContextMenu(wxContextMenuEvent& event);
};


#endif
