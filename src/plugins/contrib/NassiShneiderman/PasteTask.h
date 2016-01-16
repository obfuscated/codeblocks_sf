
// Interface Dependencies ---------------------------------------------
#ifndef PasteTask_h
#define PasteTask_h

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

class PasteTask : public Task
{
  public:
		PasteTask(NassiView *view, NassiFileContent *nfc, NassiBrick *brick, wxString strc, wxString strs);
		virtual ~PasteTask();

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
        PasteTask(const PasteTask &p);
        PasteTask &operator=(const PasteTask &rhs);
	private:
        NassiView *m_view;
        NassiFileContent *m_nfc;
        bool m_done;
        NassiBrick *m_brick;
        wxString m_strc, m_strs;
};

#endif //InsertBrickTask_h



