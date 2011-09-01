#ifndef TEXTCTRLTASK_H
#define TEXTCTRLTASK_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "Task.h"
//#include "NassiView.h"

class NassiView;
class NassiFileContent;
class TextGraph;
class TextCtrl;

class TextCtrlTask : public Task
{
    public:
        TextCtrlTask(NassiView *view, NassiFileContent *nfc, TextCtrl *textctrl, TextGraph *textgraph, const wxPoint &pos);
        virtual ~TextCtrlTask();

        virtual wxCursor Start();

        // events from window:
		virtual void OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position);
        virtual void OnMouseRightDown(wxMouseEvent &event, const wxPoint &position);

        virtual void OnMouseRightUp(wxMouseEvent& event, const wxPoint &position);
        virtual void OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position);
        virtual HooverDrawlet *OnMouseMove(wxMouseEvent &event, const wxPoint &position);
        virtual void OnKeyDown(wxKeyEvent &event);
        virtual void OnChar(wxKeyEvent &event);

        virtual bool Done()const{return m_done;}

        // events from frame(s)
        virtual bool CanEdit()const{ return true; }
        //virtual bool CanCopy()const;
        //virtual bool CanCut()const;
        virtual bool CanPaste()const;
        virtual bool HasSelection()const;
        virtual void DeleteSelection();
        virtual void Copy();
        virtual void Cut();
        virtual void Paste();

        virtual void UpdateSize();
    protected:
        bool m_done;
        TextCtrl *m_textctrl;
        NassiView *m_view;
        NassiFileContent *m_nfc;
        TextGraph *m_textgraph;
    private:
        TextCtrlTask(const TextCtrlTask &p);
        TextCtrlTask &operator=(const TextCtrlTask &rhs);
    private:
        class EditPosition
        {
            public:
                wxUint32 line;
                wxUint32 column;
                bool operator==(const EditPosition &a)const {return (a.line == this->line) && (a.column == this->column);}
                bool operator!=(const EditPosition &a)const {return !(a == (*this));}
        };
        void CloseTask();
        EditPosition GetEditPosition(const wxPoint &pos);
    public:
        void UnlinkTextGraph();
};

#endif // TEXTCTRLTASK_H
