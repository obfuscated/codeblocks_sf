#include "wxsdefevthandler.h"

#include "wxspropertiesman.h"

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"
#include "wxsdragwindow.h"


wxsDefEvtHandler::wxsDefEvtHandler(wxsWidget* Wdg):
    Widget(Wdg), DragWindow(NULL), DragBoxSize(5), DragBoxVisible(true), DragBoxGray(false)
{
    if ( Wdg->GetPreview() )
    {
//        DragWindow = new wxsDragWindow(Wdg->GetPreview(),Wdg);
    }
}

wxsDefEvtHandler::~wxsDefEvtHandler()
{
}

void wxsDefEvtHandler::OnLClick(wxMouseEvent& event)
{
    wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,Widget);
    wxPostEvent(wxSmith::Get(),SelectEvent);
	event.Skip();
}

void wxsDefEvtHandler::OnLDClick(wxMouseEvent& event)
{
	event.Skip();
}

void wxsDefEvtHandler::OnActivate(wxActivateEvent& event)
{

}

void wxsDefEvtHandler::OnSetFocus(wxFocusEvent& event)
{

}

void wxsDefEvtHandler::OnKillFocus(wxFocusEvent& event)
{

}

void wxsDefEvtHandler::OnKeyDown(wxKeyEvent& event)
{

}

void wxsDefEvtHandler::OnKeyUp(wxKeyEvent& event)
{

}

void wxsDefEvtHandler::OnChar(wxKeyEvent& event)
{

}

void wxsDefEvtHandler::OnPaint(wxPaintEvent& event)
{
	event.Skip();
}

void wxsDefEvtHandler::OnSize(wxSizeEvent& event)
{
    if ( DragWindow )
    {
    	DragWindow->SetSize(event.GetSize());
    }
    event.Skip();
}

void wxsDefEvtHandler::OnMove(wxMoveEvent& event)
{
    if ( DragWindow )
    {
    	DragWindow->Move(event.GetPosition());
    }
    event.Skip();
}



BEGIN_EVENT_TABLE(wxsDefEvtHandler,wxEvtHandler)
    EVT_LEFT_DOWN(wxsDefEvtHandler::OnLClick)
    EVT_LEFT_DCLICK(wxsDefEvtHandler::OnLDClick)
    EVT_ACTIVATE(wxsDefEvtHandler::OnActivate)
    EVT_SET_FOCUS(wxsDefEvtHandler::OnSetFocus)
    EVT_KILL_FOCUS(wxsDefEvtHandler::OnKillFocus)
    EVT_KEY_DOWN(wxsDefEvtHandler::OnKeyDown)
    EVT_KEY_UP(wxsDefEvtHandler::OnKeyUp)
    EVT_CHAR(wxsDefEvtHandler::OnChar)
    EVT_PAINT(wxsDefEvtHandler::OnPaint)
    EVT_MOUSE_EVENTS(wxsDefEvtHandler::OnLDClick)
    EVT_MOVE(wxsDefEvtHandler::OnMove)
    EVT_SIZE(wxsDefEvtHandler::OnSize)
END_EVENT_TABLE()
