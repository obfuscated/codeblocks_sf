#ifndef MLDOOORDE
#define MLDOOORDE


#include <sdk.h>
#include <wx/aui/auibook.h>

typedef wxAuiNotebook InfoPaneNotebook;   // or wxFlatNotebook, or whatever...


//#include <wx/wxFlatNotebook/wxFlatNotebook.h>
//typedef wxAuiNotebook InfoPaneNotebook;



class InfoPane : public InfoPaneNotebook
{
    DECLARE_EVENT_TABLE()

    int baseID;

public:
    InfoPane(wxWindow* parent) : InfoPaneNotebook(parent), baseID(0) { };

    void Show(size_t index) { };
    void Hide(size_t index) { };

    void OnMenu(wxCommandEvent& event);
    void OnRightClick(wxMouseEvent& event);
};


#endif
