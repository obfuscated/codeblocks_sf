#ifndef WXSDIALOGPREVIEWDLG_H
#define WXSDIALOGPREVIEWDLG_H

//#include "closepreview.h"
#include <wx/dialog.h>

class wxsResource;

class wxsDialogPreviewDlg: public wxDialog
{
    public:

        wxsDialogPreviewDlg();

        wxsDialogPreviewDlg(
            wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE);

        ~wxsDialogPreviewDlg();

        /** Additional initialization
         *
         * This function initializes accelerator table and adds close button
         * when this is borderless dialog. This function must be called after
         * window content is initialized
         */
        void Initialize(wxsResource* Res);

    private:

        bool IsBorderLess();
//        ClosePreviewDlg* CPD;
        wxsResource* Res;

        void OnMove(wxMoveEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnEscape(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif // WXSDIALOGPREVIEWDLG_H
