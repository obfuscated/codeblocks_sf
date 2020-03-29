#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DropTarget_H__
#define __DropTarget_H__

#include <wx/dnd.h>


class NassiView;
class NassiDiagramWindow;

class NassiDropTarget : public wxDropTarget
{
public:
    //NassiDropTarget(NassiView *view);
    NassiDropTarget(NassiDiagramWindow *window, NassiView *view);
    ~NassiDropTarget(){}
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    virtual void OnLeave();
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
private:
    NassiDropTarget(const NassiDropTarget &p);
    NassiDropTarget &operator=(const NassiDropTarget &rhs);
private:
    //NassiView *m_view;
    NassiDiagramWindow *m_window;
};

#endif


