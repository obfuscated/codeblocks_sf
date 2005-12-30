#ifndef __WXSEVENT_H
#define __WXSEVENT_H

#include <wx/event.h>
#include "wxsglobals.h"

class wxsResource;
class wxsWidget;

class wxsEvent: public wxCommandEvent
{
    public:

		wxsEvent(wxEventType Type = wxEVT_NULL, int id = 0, wxsResource* _Res = NULL, wxsWidget* _Widget=NULL)
			: wxCommandEvent(Type, id),
			Res(_Res),
            Widget(_Widget)
        {}

		wxsEvent(const wxsEvent& event)
			: wxCommandEvent(event),
			Res(event.Res),
			Widget(event.Widget)
        {}

		virtual wxEvent *Clone() const { return new wxsEvent(*this); }

		inline wxsResource* GetResource() { return Res;}
		inline void SetResource(wxsResource* _Res) { Res = Res; }
		inline wxsWidget* GetWidget() { return Widget; }
		inline void SetWidget(wxsWidget* _Widget) { Widget = _Widget; }

    private:

        wxsResource* Res;
        wxsWidget* Widget;
};

typedef void (wxEvtHandler::*wxsEventFunction)(wxsEvent&);

extern const wxEventType wxEVT_SELECT_RES;      // Sent when resource is selected
extern const wxEventType wxEVT_UNSELECT_RES;    // Sent when given resourcee is no longer valid
extern const wxEventType wxEVT_SELECT_WIDGET;   // Sent when selecting given widget
extern const wxEventType wxEVT_UNSELECT_WIDGET; // Send when given widget is no longer valid

#define EVT_SELECT_RES(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SELECT_RES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(wxsEventFunction)&fn, (wxObject *) NULL ),
#define EVT_UNSELECT_RES(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_UNSELECT_RES, -1, -1, (wxObjectEventFunction)(wxEventFunction)(wxsEventFunction)&fn, (wxObject *) NULL ),
#define EVT_SELECT_WIDGET(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SELECT_WIDGET, -1, -1, (wxObjectEventFunction)(wxEventFunction)(wxsEventFunction)&fn, (wxObject *) NULL ),
#define EVT_UNSELECT_WIDGET(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_UNSELECT_WIDGET, -1, -1, (wxObjectEventFunction)(wxEventFunction)(wxsEventFunction)&fn, (wxObject *) NULL ),

/** Sending global widget selection event */
void wxsSelectWidget(wxsWidget* Widget);

/** Sending globak widget unselection dialog */
void wxsUnselectWidget(wxsWidget* Widget);

/** Sending global resource selection event */
void wxsSelectRes(wxsResource* Res);

/** Sending global resource unselection event */
void wxsUnselectRes(wxsResource* Res);

/** Changing blocking of selection events 
 *
 * If selection events are blocked, wxsSelectWidget
 * and wxsSelectRes do nothing */
void wxsBlockSelectEvents(bool Block = true);

#endif
