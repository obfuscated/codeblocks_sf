#ifndef WXSDIALOGPREVIEWDLG_H
#define WXSDIALOGPREVIEWDLG_H

#include <wx/dialog.h>

class wxsWindowRes;
class wxsDialogPreviewDlg: public wxDialog
{
    public:

        wxsDialogPreviewDlg(wxsWindowRes* Resource);
        ~wxsDialogPreviewDlg();

    private:

        wxsWindowRes* Res;
        void OnEscape(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif
