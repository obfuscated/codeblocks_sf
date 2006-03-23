#include "wxsdialogpreviewdlg.h"
#include "wxsdialogres.h"
#include <wx/accel.h>

BEGIN_EVENT_TABLE(wxsDialogPreviewDlg,wxDialog)
    EVT_MENU(wxID_EXIT,wxsDialogPreviewDlg::OnEscape)
END_EVENT_TABLE()

wxsDialogPreviewDlg::wxsDialogPreviewDlg(wxsWindowRes* Resource):
    Res(Resource)
{
    wxAcceleratorEntry Acc[1];
    Acc[0].Set(wxACCEL_NORMAL,WXK_ESCAPE,wxID_EXIT);
    wxAcceleratorTable Table(1,Acc);
    SetAcceleratorTable(Table);

}

wxsDialogPreviewDlg::~wxsDialogPreviewDlg()
{
    Res->NotifyPreviewClosed();
}

void wxsDialogPreviewDlg::OnEscape(wxCommandEvent& event)
{
    Close();
}
