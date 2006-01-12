#include "wxsheaders.h"
#include "wxsevent.h"

#include "wxsmith.h"
#include "wxsglobals.h"

const wxEventType wxEVT_SELECT_RES = wxNewEventType();
const wxEventType wxEVT_UNSELECT_RES = wxNewEventType();
const wxEventType wxEVT_SELECT_WIDGET = wxNewEventType();
const wxEventType wxEVT_UNSELECT_WIDGET = wxNewEventType();
static int wxsBlockSelectEventsCnt = 0;

void wxsBlockSelectEvents(bool Block)
{
    if ( Block ) wxsBlockSelectEventsCnt++;
    else         wxsBlockSelectEventsCnt--;

    if ( wxsBlockSelectEventsCnt < 0 ) DBGLOG(_T("wxSmith: called unmatched wxsBlockSelectEvents(false)"));
//    if ( wxsBlockSelectEventsCnt > 1 ) DBGLOG(_T("wxSmith: recursive wxsBlockSelectEvents(true) call"));
}

void wxsSelectWidget(wxsWidget* Widget)
{
    if ( wxsBlockSelectEventsCnt > 0 ) return;
    wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,Widget);
    wxsPLUGIN()->ProcessEvent(SelectEvent);
}

void wxsUnselectWidget(wxsWidget* Widget)
{
    wxsEvent UnselectEvent(wxEVT_UNSELECT_WIDGET,0,NULL,Widget);
    wxsPLUGIN()->ProcessEvent(UnselectEvent);
}

void wxsSelectRes(wxsResource* Res)
{
    if ( wxsBlockSelectEventsCnt > 0 ) return;
    wxsEvent SelectEvent(wxEVT_SELECT_RES,0,Res);
    wxsPLUGIN()->ProcessEvent(SelectEvent);
}

void wxsUnselectRes(wxsResource* Res)
{
    wxsEvent UnselectEvent(wxEVT_UNSELECT_RES,0,Res);
    wxsPLUGIN()->ProcessEvent(UnselectEvent);
}
