
// Interface Dependencies ---------------------------------------------
#ifndef TaskTest_h
#define TaskTest_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "Task.h"

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

class NassiView;

class TaskTest: public Task
{
  public:
		TaskTest(NassiView *view);
		virtual ~TaskTest();

		virtual void Start();

		virtual HooverDrawlet *OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseRightDown(wxMouseEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseRightUp(wxMouseEvent& event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseMove(wxMouseEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnKeyDown(wxKeyEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnChar(wxKeyEvent &event, const wxPoint &position);

	private:
        NassiView *m_view;
};

#endif




