#include "wxsdragwindow.h"

#include <wx/dcclient.h>

#include "widget.h"

wxsDragWindow::wxsDragWindow(wxWindow* Cover,wxsWidget* Wdg,const wxSize& Size):
    wxControl(Cover,-1,wxDefaultPosition,Size,wxNO_BORDER|wxTRANSPARENT_WINDOW),
    RootWidget(Wdg)
{
}

wxsDragWindow::~wxsDragWindow()
{
	ClearDragPoints();
}

void wxsDragWindow::OnPaint(wxPaintEvent& event)
{
	wxWindow* Wnd = this;
	wxPaintDC DC(Wnd);
	DC.SetDeviceOrigin(0,0);
    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
    	DragPointData* DPD = *i;
        wxColor DrawColor( DPD->Inactive ? wxColor(0x80,0x80,0x80) : wxColor(0,0,0) );
        DC.SetPen( wxPen(DrawColor,1) );
        DC.SetBrush( wxBrush(DrawColor) );
    	DC.DrawRectangle( DPD->PosX - DragBoxSize/2, DPD->PosY - DragBoxSize/2, DragBoxSize, DragBoxSize );
    }
}

void wxsDragWindow::OnEraseBack(wxEraseEvent& event)
{
}

void wxsDragWindow::OnMouse(wxMouseEvent& event)
{
}

void wxsDragWindow::ClearDragPoints()
{
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
    	delete *i;
    }
    DragPoints.clear();
}

void wxsDragWindow::SetWidget(wxsWidget* _RootWidget)
{
	ClearDragPoints();
	RootWidget = _RootWidget;
}

BEGIN_EVENT_TABLE(wxsDragWindow,wxControl)
    EVT_PAINT(wxsDragWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxsDragWindow::OnMouse)
END_EVENT_TABLE()
