#include "wxsevent.h"

#include "wxsmith.h"

const wxEventType wxEVT_SELECT_RES = wxNewEventType();
const wxEventType wxEVT_UNSELECT_RES = wxNewEventType();
const wxEventType wxEVT_SELECT_WIDGET = wxNewEventType();
const wxEventType wxEVT_UNSELECT_WIDGET = wxNewEventType();

void wxsSelectWidget(wxsWidget* Widget)
{
    wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,Widget);
    wxSmith::Get()->ProcessEvent(SelectEvent);
}

void wxsUnselectWidget(wxsWidget* Widget)
{
    wxsEvent UnselectEvent(wxEVT_UNSELECT_WIDGET,0,NULL,Widget);
    wxSmith::Get()->ProcessEvent(UnselectEvent);
}

void wxsSelectRes(wxsResource* Res)
{
    wxsEvent SelectEvent(wxEVT_SELECT_RES,0,Res);
    wxSmith::Get()->ProcessEvent(SelectEvent);
}

void wxsUnselectRes(wxsResource* Res)
{
    wxsEvent UnselectEvent(wxEVT_UNSELECT_RES,0,Res);
    wxSmith::Get()->ProcessEvent(UnselectEvent);
}

