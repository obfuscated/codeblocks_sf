// Interface Dependencies ---------------------------------------------
#ifndef EditTextTask_h
#define EditTextTask_h

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
// -------------------------------------------------------------------

class HooverDrawlet;

class EditTextTask : public Task
{
  public:
		EditTextTask(NassiView *view, NassiFileContent *nfc, TextGraph *textgraph, const wxPoint &pos);
		virtual ~EditTextTask();

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
        virtual bool CanEdit(){return true;}
        virtual bool CanCopy();
        virtual bool CanCut();
        virtual bool CanPaste();
        virtual bool HasSelection();
        virtual void DeleteSelection();
        virtual void Copy();
        virtual void Paste();
        virtual void Cut();

        virtual void UpdateSize();

    private:
        void InsertText(const wxString &str);

        virtual bool Done();
        void CloseTask();
        wxCommand *GetDeleteCommand();
        wxString GetSelectedText();

    private:
        class EditPosition
        {
            public:
                wxUint32 line;
                wxUint32 column;
                bool operator==(const EditPosition &a)const {return (a.line == this->line) && (a.column == this->column);}
                bool operator!=(const EditPosition &a)const {return !(a == (*this));}
        };
        EditPosition m_posa, m_posb;
        void DecrementPosition(EditPosition &pos);
        void IncrementPosition(EditPosition &pos);
    private:
        EditPosition GetEditPosition(const wxPoint &pos);
        void HideCaret();
        void ShowCaret();
        void MoveCaret();

        void DeleteBack();
        void Delete();



    public:
        void UnlinkTextGraph();

	private:
        NassiView *m_view;
        NassiFileContent *m_nfc;
        bool m_done;
        TextGraph *m_textgraph;
};

#endif //EditTextTask_h



