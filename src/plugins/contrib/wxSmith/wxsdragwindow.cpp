#include "wxsdragwindow.h"

#include <wx/dcclient.h>

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"

wxsDragWindow::wxsDragWindow(wxWindow* Cover,wxsWidget* Wdg,const wxSize& Size):
    wxControl(Cover,-1,wxDefaultPosition,Size,wxNO_BORDER|wxTRANSPARENT_WINDOW),
    RootWidget(Wdg), CurDragPoint(NULL), CurDragWidget(NULL)
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
    	if ( (*i)->Invisible ) continue;
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
    if ( event.LeftDown() )
    {
        CurDragPoint = NULL;
        CurDragWidget = NULL;
        bool FoundDragging = false;
        int MouseX = event.GetX();
        int MouseY = event.GetY();
        ClientToScreen(&MouseX,&MouseY);
        
    	// Searching for drag point
    	
    	for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    	{
    		int PosX = (*i)->PosX - DragBoxSize/2;
    		int PosY = (*i)->PosY - DragBoxSize/2;
    		
    		if ( MouseX >= PosX &&
    		     MouseY >= PosY &&
    		     MouseX <= PosX + DragBoxSize &&
    		     MouseY <= PosY + DragBoxSize )
            {
                DragItemBegX = (*i)->PosX;
                DragItemBegY = (*i)->PosY;
                CurDragPoint = *i;
                FoundDragging = true;
                break;
            }
    	}
    	
    	if ( !FoundDragging )
    	{
    		// Additional checking of widget edges
    		
            for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
            {
            	DragPointData* DPD = *i;
            	
            	switch ( DPD->Type )
            	{
                    case Top:
                    case Btm:
                        {
                        	int PosX1 = 0, PosX2 = 0;
                        	DPD->Widget->GetPreview()->ClientToScreen(&PosX1,&PosX2);
                        	PosX2 = PosX1 + DPD->Widget->GetPreview()->GetSize().GetWidth();
                        	int PosY = DPD->PosY - DragBoxSize / 2;
                        	
                        	if ( MouseX > PosX1 &&
                        	     MouseX < PosX2 &&
                        	     MouseY >= PosY &&
                                 MouseY <= PosY + DragBoxSize )
                            {
                                CurDragPoint = DPD;
                                DragItemBegX = DPD->PosX;
                                DragItemBegY = DPD->PosY;
                                FoundDragging = true;
                            }
                        }
                        break;
                    
                    case Left:
                    case Right:
                        {
                        	int PosY1 = 0, PosY2 = 0;
                        	DPD->Widget->GetPreview()->ClientToScreen(&PosY1,&PosY2);
                        	PosY2 = PosY1 + DPD->Widget->GetPreview()->GetSize().GetHeight();
                        	int PosX = DPD->PosX - DragBoxSize / 2;
                        	
                        	if ( MouseY > PosY1 &&
                        	     MouseY < PosY2 &&
                        	     MouseX >= PosX &&
                                 MouseX <= PosX + DragBoxSize )
                            {
                                CurDragPoint = DPD;
                                DragItemBegX = DPD->PosX;
                                DragItemBegY = DPD->PosY;
                                FoundDragging = true;
                            }
                        }
                        break;
                    
                    default:
                        break;
            	}
            	
            	if ( FoundDragging ) break;
            }
            
            if ( !FoundDragging )
            {
            	// Checking if we started to drag widget
            	
            	CurDragWidget = FindWidgetAtPos(MouseX,MouseY,RootWidget);
            	FoundDragging = CurDragWidget != NULL;
            	DragItemBegX = 0;
            	DragItemBegY = 0;
            	CurDragWidget->GetPreview()->ClientToScreen(&DragItemBegX,&DragItemBegY);
            }
    	}
    	
    	if ( FoundDragging )
    	{
    		DragMouseBegX = MouseX;
    		DragMouseBegY = MouseY;
    		
    		if ( CurDragWidget )
    		{
                wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,CurDragWidget);
                wxPostEvent(wxSmith::Get(),SelectEvent);
    		}
    		else
    		{
                wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,CurDragPoint->Widget);
                wxPostEvent(wxSmith::Get(),SelectEvent);
    		}
    	}
    }
    
    
}

void wxsDragWindow::ClearDragPoints()
{
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
    	delete *i;
    }
    DragPoints.clear();
}

void wxsDragWindow::RebuildDragPoints(wxsWidget* Widget)
{
	if ( !Widget ) return;
	
    for ( int i = 0; i<Widget->GetChildCount(); i++ )
    {
    	RebuildDragPoints(Widget->GetChild(i));
    }
    
	if ( Widget->GetBPType() & ( wxsWidget::bptSize | wxsWidget::bptPosition ) )
	{
        if ( Widget->GetPreview() )
        {
            DragPointData* WidgetPoints[DragBoxTypeCnt];
            
            int PosX=0, PosY=0;
            int SizeX=0, SizeY=0;
            Widget->GetPreview()->ClientToScreen(&PosX,&PosY);
            Widget->GetPreview()->GetSize(&SizeX,&SizeY);
            
            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                WidgetPoints[i] = new DragPointData;
                WidgetPoints[i]->Widget = Widget;
                WidgetPoints[i]->Type = (DragBoxType)i;
                WidgetPoints[i]->Invisible = true;
                WidgetPoints[i]->Inactive = false;
                WidgetPoints[i]->PosX = PosX;
                WidgetPoints[i]->PosY = PosY;
                
                if ( i == Top || i == Btm )
                {
                	WidgetPoints[i]->PosX += SizeX / 2;
                }
                else if ( i == RightTop || i == Right || i == RightBtm )
                {
                	WidgetPoints[i]->PosX += SizeX;
                }
                
                if ( i==Left || i == Right )
                {
                	WidgetPoints[i]->PosY += SizeY / 2;
                }
                else if ( i == LeftBtm || i == Btm || i == RightBtm )
                {
                	WidgetPoints[i]->PosY += SizeY;
                }
            }
            
            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                memcpy(WidgetPoints[i]->WidgetPoints,WidgetPoints,sizeof(WidgetPoints));
                DragPoints.push_back(WidgetPoints[i]);
            }
            
            
        }
	}

}

void wxsDragWindow::SetWidget(wxsWidget* _RootWidget)
{
	ClearDragPoints();
	RootWidget = _RootWidget;
	RebuildDragPoints(RootWidget);
}

wxsWidget* wxsDragWindow::FindWidgetAtPos(int PosX,int PosY,wxsWidget* Widget)
{
    if ( !Widget ) return NULL;
    
    for ( int i=0; i<Widget->GetChildCount(); ++i )
    {
    	wxsWidget* Wdg = FindWidgetAtPos(PosX,PosY,Widget->GetChild(i));
    	if ( Wdg ) return Wdg;
    }
    
    int WdgX = 0, WdgY = 0;
    int WdgSX, WdgSY;
    Widget->GetPreview()->ClientToScreen(&WdgX,&WdgY);
    Widget->GetPreview()->GetSize(&WdgSX,&WdgSY);
    
    if ( PosX >= WdgX && PosY >= WdgY && PosX < WdgX + WdgSX && PosY < WdgY + WdgSY )
    {
    	return Widget;
    }
    
    return NULL;
}

BEGIN_EVENT_TABLE(wxsDragWindow,wxControl)
    EVT_PAINT(wxsDragWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxsDragWindow::OnMouse)
END_EVENT_TABLE()
