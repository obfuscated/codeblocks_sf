#include "wxsframepreviewfrm.h"
#include "wxsframeres.h"

#include <wx/accel.h>


BEGIN_EVENT_TABLE(wxsFramePreviewFrm,wxFrame)
    EVT_MENU(wxID_EXIT,wxsFramePreviewFrm::OnEscape)
END_EVENT_TABLE()

wxsFramePreviewFrm::wxsFramePreviewFrm(wxsFrameRes* Resource):
    Res(Resource)
{
    wxAcceleratorEntry Acc[1];
    Acc[0].Set(wxACCEL_NORMAL,WXK_ESCAPE,wxID_EXIT);
    wxAcceleratorTable Table(1,Acc);
    SetAcceleratorTable(Table);

}

wxsFramePreviewFrm::~wxsFramePreviewFrm()
{
    Res->NotifyPreviewClosed();
}

void wxsFramePreviewFrm::OnEscape(wxCommandEvent& event)
{
    Close();
}
