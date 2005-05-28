#include "wxsdefevthandler.h"

#include "wxspropertiesman.h"

#include "widget.h"

wxsDefEvtHandler::wxsDefEvtHandler(wxsWidget* Wdg):
    Widget(Wdg)
{
}

wxsDefEvtHandler::~wxsDefEvtHandler()
{
}

void wxsDefEvtHandler::OnLClick(wxMouseEvent& event)
{
    wxsPropertiesMan::Get()->SetActiveWidget(Widget);
}

void wxsDefEvtHandler::OnLDClick(wxMouseEvent& event)
{
}

BEGIN_EVENT_TABLE(wxsDefEvtHandler,wxEvtHandler)
    EVT_LEFT_DOWN(wxsDefEvtHandler::OnLClick)
    EVT_LEFT_DCLICK(wxsDefEvtHandler::OnLDClick)
END_EVENT_TABLE()
