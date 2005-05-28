#ifndef WXSDEFEVTHANDLER_H
#define WXSDEFEVTHANDLER_H

#include <wx/event.h>

class wxsWidget;

class wxsDefEvtHandler: public wxEvtHandler
{
	public:
		wxsDefEvtHandler(wxsWidget* Widget);
		virtual ~wxsDefEvtHandler();
		
	private:
	
        wxsWidget* Widget;
        
        void OnLClick(wxMouseEvent& event);
        void OnLDClick(wxMouseEvent& event);
        
        DECLARE_EVENT_TABLE()
};

#endif // WXSDEFEVTHANDLER_H
