

#ifndef _NASSI_DIAGRAM_H_INCLUDED
#define _NASSI_DIAGRAM_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include <wx/scrolwin.h>
#include <wx/caret.h>
#include <wx/dnd.h>

#include "bricks.h"


class NassiView;
class HooverDrawlet;


class NassiDiagramWindow: public wxScrolledWindow
{
    friend class TextCtrl;
public:

    //NassiDiagramWindow(){}
    NassiDiagramWindow(wxWindow *parent, NassiView *view);
    ~NassiDiagramWindow();
private:
    NassiDiagramWindow(const NassiDiagramWindow &p);
    NassiDiagramWindow &operator=(const NassiDiagramWindow &rhs);
private:
    void OnDraw(wxDC& /*dc*/){} //virtual
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent &event);
    void OnMouseLeftDown(wxMouseEvent &event);
    void OnMouseRightDown(wxMouseEvent &event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent &event);
    void OnEnter(wxMouseEvent &event);
    void OnLeave(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnChar(wxKeyEvent &event);

    void RemoveDrawlet(wxDC &dc);

    void OnErase(wxEraseEvent &event);
    void OnPaint(wxPaintEvent &event);
    void PaintBackground(wxDC &dc);
    void Draw(wxDC &dc);

    bool HasFocus(){return this == this->FindFocus();}
    void OnKillFocus(wxFocusEvent &event);
    void OnSetFocus(wxFocusEvent &event);


//    wxPoint dndpt;
//    bool dndsource;
//    bool focus;
//    bool pasting;
//    bool drop;
//    bool dropcopy;

//    bool OnDrop(wxPoint pt, NassiBrick *brick, wxString strc = _T(""), wxString strs = _T("case :") );
//    wxDragResult OnDragOver(wxPoint pt, wxDragResult def);
//    void OnDragLeave(void);
//    void OnDragEnter(void);

private:
    NassiView *m_view;
    HooverDrawlet *m_hd;

public:
    wxDragResult OnDrop(const wxPoint &pt, NassiBrick *brick, wxString strc, wxString strs, wxDragResult def);
    wxDragResult OnDragOver(const wxPoint &pt, wxDragResult def, bool HasNoBricks);
    void OnDragLeave(void);
    void OnDragEnter(void);

protected:
    //DECLARE_DYNAMIC_CLASS(NassiDiagramWindow)
    DECLARE_EVENT_TABLE()
};



#endif
