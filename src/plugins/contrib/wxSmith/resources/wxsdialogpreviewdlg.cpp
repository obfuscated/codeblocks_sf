#include "../wxsheaders.h"
#include "wxsdialogpreviewdlg.h"

#include "../wxsresource.h"

#include <wx/accel.h>


BEGIN_EVENT_TABLE(wxsDialogPreviewDlg,wxDialog)
//    EVT_MOVE(wxsDialogPreviewDlg::OnMove)
//    EVT_SIZE(wxsDialogPreviewDlg::OnSize)
    EVT_CLOSE(wxsDialogPreviewDlg::OnClose)
    EVT_MENU(wxID_EXIT,wxsDialogPreviewDlg::OnEscape)
END_EVENT_TABLE()

wxsDialogPreviewDlg::wxsDialogPreviewDlg()//: CPD(NULL)
{
}

wxsDialogPreviewDlg::wxsDialogPreviewDlg(
    wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style):
        wxDialog(parent,id,title,pos,size,style)//, CPD(NULL)
{
}

wxsDialogPreviewDlg::~wxsDialogPreviewDlg()
{
}

void wxsDialogPreviewDlg::Initialize(wxsResource* _Res)
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

bool wxsDialogPreviewDlg::IsBorderLess()
{
    //if ( !( GetWindowStyle() & wxCLOSE_BOX ) ) return true;
    return false;
}

void wxsDialogPreviewDlg::OnMove(wxMoveEvent& event)
{
//    if ( CPD ) CPD->AdjustPosition();
}

void wxsDialogPreviewDlg::OnSize(wxSizeEvent& event)
{
//    if ( CPD ) CPD->AdjustPosition();
}

void wxsDialogPreviewDlg::OnClose(wxCloseEvent& event)
{
    if ( Res ) Res->HidePreview();
    else event.Skip();
}

void wxsDialogPreviewDlg::OnEscape(wxCommandEvent& event)
{
    Close();
}
