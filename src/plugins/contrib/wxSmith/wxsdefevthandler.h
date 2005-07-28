#ifndef WXSDEFEVTHANDLER_H
#define WXSDEFEVTHANDLER_H

#include <wx/event.h>

class wxsWidget;
class wxsDragWindow;

class wxsDefEvtHandler: public wxEvtHandler
{
	public:
		wxsDefEvtHandler(wxsWidget* Widget);
		virtual ~wxsDefEvtHandler();
		
	private:
	
        wxsWidget* Widget;
        wxsDragWindow* DragWindow;
        int DragBoxSize;
        bool DragBoxVisible;
        bool DragBoxGray;
        
        void OnLClick(wxMouseEvent& event);
        void OnLDClick(wxMouseEvent& event);
        void OnActivate(wxActivateEvent& event);
        void OnSetFocus(wxFocusEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnKeyUp(wxKeyEvent& event);
        void OnChar(wxKeyEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnMove(wxMoveEvent& event);
        
        DECLARE_EVENT_TABLE()
};

#endif // WXSDEFEVTHANDLER_H
