#include "../wxsheaders.h"
#include "wxsframepreviewfrm.h"

#include "../wxsresource.h"

#include <wx/accel.h>


BEGIN_EVENT_TABLE(wxsFramePreviewFrm,wxFrame)
//    EVT_MOVE(wxsFramePreviewFrm::OnMove)
//    EVT_SIZE(wxsFramePreviewFrm::OnSize)
    EVT_CLOSE(wxsFramePreviewFrm::OnClose)
    EVT_MENU(wxID_EXIT,wxsFramePreviewFrm::OnEscape)
END_EVENT_TABLE()

wxsFramePreviewFrm::wxsFramePreviewFrm()//: CPD(NULL)
{
}

wxsFramePreviewFrm::wxsFramePreviewFrm(
    wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style):
        wxFrame(parent,id,title,pos,size,style)//, CPD(NULL)
{
}

wxsFramePreviewFrm::~wxsFramePreviewFrm()
{
}

void wxsFramePreviewFrm::Initialize(wxsResource* _Res)
{
    wxAcceleratorEntry Acc[1];
    Acc[0].Set(wxACCEL_NORMAL,WXK_ESCAPE,wxID_EXIT);
    wxAcceleratorTable Table(1,Acc);
    SetAcceleratorTable(Table);

//    if ( IsBorderLess() )
//    {
//        CPD = new ClosePreviewDlg(this,this);
//        CPD->Show();
//    }

    Res = _Res;
    Layout();
}

bool wxsFramePreviewFrm::IsBorderLess()
{
    //if ( !( GetWindowStyle() & wxCLOSE_BOX ) ) return true;
    return false;
}

void wxsFramePreviewFrm::OnMove(wxMoveEvent& event)
{
//    if ( CPD ) CPD->AdjustPosition();
}

void wxsFramePreviewFrm::OnSize(wxSizeEvent& event)
{
//    if ( CPD ) CPD->AdjustPosition();
}

void wxsFramePreviewFrm::OnClose(wxCloseEvent& event)
{
    if ( Res ) Res->HidePreview();
    else event.Skip();
}

void wxsFramePreviewFrm::OnEscape(wxCommandEvent& event)
{
    Close();
}
