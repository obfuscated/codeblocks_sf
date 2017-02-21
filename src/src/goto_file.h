#ifndef GOTO_FILE_H
#define GOTO_FILE_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(GotoFile)
    #include <wx/dialog.h>
    class wxBoxSizer;
    class wxListCtrl;
    class wxStaticText;
    class wxTextCtrl;
    //*)
#endif
//(*Headers(GotoFile)
//*)

#include "incremental_select_helper.h"


class GotoFile: public wxDialog
{
    public:
        GotoFile(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title, const wxString &message);
        ~GotoFile() override;

        int GetSelection();

        void AddControlBelowList(wxControl *control);

    private:
        void FilterItems();
    private:
        IncrementalSelectHandler m_handler;
    private:

        //(*Declarations(GotoFile)
        IncrementalListCtrl* m_ResultList;
        wxBoxSizer* m_sizer;
        wxTextCtrl* m_Text;
        //*)

        //(*Identifiers(GotoFile)
        static const long ID_TEXTCTRL1;
        static const long ID_RESULT_LIST;
        //*)

        //(*Handlers(GotoFile)
        //*)

    protected:

        void BuildContent(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                          const wxString &message);

        DECLARE_EVENT_TABLE()
};

#endif
