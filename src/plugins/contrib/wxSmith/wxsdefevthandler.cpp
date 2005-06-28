#include "wxsdefevthandler.h"

#include "wxspropertiesman.h"

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"

wxsDefEvtHandler::wxsDefEvtHandler(wxsWidget* Wdg):
    Widget(Wdg)
{
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

BEGIN_EVENT_TABLE(wxsDefEvtHandler,wxEvtHandler)
    EVT_LEFT_DOWN(wxsDefEvtHandler::OnLClick)
    EVT_LEFT_DCLICK(wxsDefEvtHandler::OnLDClick)
END_EVENT_TABLE()
