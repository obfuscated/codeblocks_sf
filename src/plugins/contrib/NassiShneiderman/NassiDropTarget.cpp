
#ifdef __GNUG__
// #pragma interface
#endif

//#include <wx/dcmemory.h>
//#include <wx/msgdlg.h>
//#include <wx/mstream.h>
//#include <wx/txtstrm.h>

#include "NassiDropTarget.h"
#include "DataObject.h"
#include "NassiView.h"
#include "NassiDiagramWindow.h"


NassiDropTarget::NassiDropTarget(NassiDiagramWindow *window, NassiView *view)
    : wxDropTarget(new NassiDataObject((NassiBrick *)0, (NassiView *)view) ),
    m_window(window)
{}

wxDragResult NassiDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    m_window->OnDragEnter();
    return OnDragOver(x, y, def);
}
void NassiDropTarget::OnLeave()
{
    m_window->OnDragLeave();
}

wxDragResult NassiDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    if ( !GetData() )
    {
        wxMessageBox(_("Failed to get drag and drop data") );
        return wxDragNone;
    }

    return m_window->OnDrop(wxPoint(x,y),
        ((NassiDataObject *)GetDataObject())->GetBrick(),
        ((NassiDataObject *)GetDataObject())->GetText(0),
        ((NassiDataObject *)GetDataObject())->GetText(1),
         def );
}
wxDragResult NassiDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    bool HasNoBricks = !(((NassiDataObject *)GetDataObject())->HasBrick());
    // GetData() has not yet been called. so we assume here that it will get some bricks.
    // TODO (daniel#1#): Check for a nicer solution
    HasNoBricks = false;
    return m_window->OnDragOver(wxPoint(x, y), def, HasNoBricks);
}

