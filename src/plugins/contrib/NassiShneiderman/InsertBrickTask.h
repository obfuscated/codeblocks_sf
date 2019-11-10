
// Interface Dependencies ---------------------------------------------
#ifndef InsertBrickTask_h
#define InsertBrickTask_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "Task.h"
#include "NassiView.h"

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

class InsertBrickTask : public Task
{
  public:
		InsertBrickTask(NassiView *view, NassiFileContent *nfc, NassiView::NassiTools tool);
		virtual ~InsertBrickTask();

		virtual wxCursor Start();

        // events from window:
		virtual void OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position);
        virtual void OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position);
        virtual void OnMouseRightDown(wxMouseEvent &event, const wxPoint &position);
        virtual void OnMouseRightUp(wxMouseEvent& event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseMove(wxMouseEvent &event, const wxPoint &position);
        virtual void OnKeyDown(wxKeyEvent &event);
        virtual void OnChar(wxKeyEvent &event);

        // events from frame(s)
        virtual bool CanEdit()const;
        //virtual bool CanCopy()const;
        //virtual bool CanCut()const;
        virtual bool CanPaste()const;
        virtual bool HasSelection()const;
        virtual void DeleteSelection();
        virtual void Copy();
        virtual void Cut();
        virtual void Paste();


        virtual bool Done()const;

	private:
        InsertBrickTask(const InsertBrickTask &p);
        InsertBrickTask &operator=(InsertBrickTask &rhs);
    private:
        NassiView *m_view;
        NassiFileContent *m_nfc;
        bool m_done;
        NassiView::NassiTools m_tool;
};

#endif //InsertBrickTask_h



