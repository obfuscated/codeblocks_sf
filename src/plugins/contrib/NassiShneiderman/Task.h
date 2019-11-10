
// Interface Dependencies ---------------------------------------------
#ifndef Task_h
#define Task_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// END Interface Dependencies -----------------------------------------

// --------------------------------------------------------------------
//
//  Name
//    Task
//
//  Description
//    Abstract base class for all tasks that interact
//    with the user using mouse events.
//
//  Notes:
//
// --------------------------------------------------------------------

class HooverDrawlet;

class Task
{
  public:
		Task();
		virtual ~Task();

		virtual wxCursor Start() = 0;

        // events from window:
		virtual void OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position) = 0;
        virtual void OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position) = 0;
        virtual void OnMouseRightDown(wxMouseEvent &event, const wxPoint &position) = 0;
        virtual void OnMouseRightUp(wxMouseEvent& event, const wxPoint &position) = 0;
        virtual HooverDrawlet *OnMouseMove(wxMouseEvent &event, const wxPoint &position) = 0;
        virtual void OnKeyDown(wxKeyEvent &event) = 0;
        virtual void OnChar(wxKeyEvent &event) = 0;

        virtual bool Done()const{return true;}

        // events from frame(s)
        virtual bool CanEdit()const{ return false; }
        virtual bool HasSelection()const = 0;
        //virtual bool CanCopy()const = 0;
        //virtual bool CanCut()const = 0;
        virtual bool CanPaste()const = 0;
        virtual void Copy() = 0;
        virtual void Cut() = 0;
        virtual void Paste() = 0;
        virtual void DeleteSelection() = 0;

        virtual void UpdateSize(){};

	private:
};

#endif


