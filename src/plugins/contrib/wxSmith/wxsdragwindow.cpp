#include "wxsdragwindow.h"

#include <wx/dcclient.h>

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"

wxsDragWindow::wxsDragWindow(wxWindow* Cover,wxsWidget* Wdg,const wxSize& Size):
    wxControl(Cover,-1,wxDefaultPosition,Size,wxNO_BORDER|wxTRANSPARENT_WINDOW|wxSTAY_ON_TOP),
    RootWidget(Wdg), CurDragPoint(NULL), CurDragWidget(NULL), RefreshTimer(this,1)
{
	BuildDragPoints(Wdg);
	RefreshTimer.Start(50);
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
	AddGraphics(DC);
}

void wxsDragWindow::TimerRefresh(wxTimerEvent& event)
{
	wxClientDC DC(this);
	AddGraphics(DC);
}

void wxsDragWindow::OnEraseBack(wxEraseEvent& event)
{
}

void wxsDragWindow::OnMouse(wxMouseEvent& event)
{
    DragPointData* NewDragPoint = NULL;
    wxsWidget* NewDragWidget = NULL;
    bool FoundDragging = false;
    int MouseX = event.GetX();
    int MouseY = event.GetY();
    ClientToScreen(&MouseX,&MouseY);
        
    // Searching for item covered by mouse
    
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
        int PosX = (*i)->PosX - DragBoxSize/2;
        int PosY = (*i)->PosY - DragBoxSize/2;
        
        if ( MouseX >= PosX &&
             MouseY >= PosY &&
             MouseX <= PosX + DragBoxSize &&
             MouseY <= PosY + DragBoxSize )
        {
            NewDragPoint = *i;
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
                            NewDragPoint = DPD;
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
                            NewDragPoint = DPD;
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
            
            NewDragWidget = FindWidgetAtPos(MouseX,MouseY,RootWidget);
            FoundDragging = NewDragWidget != NULL;
        }
    }
    
	
	// Processing Left Down event
	
    if ( event.LeftDown() )
    {
    	if ( FoundDragging )
    	{
    		DragMouseBegX = MouseX;
    		DragMouseBegY = MouseY;
    		DragDistanceSmall = true;
    		CaptureMouse();
    		
    		if ( NewDragWidget )
    		{
    			CurDragWidget = NewDragWidget;
                wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,CurDragWidget);
                wxPostEvent(wxSmith::Get(),SelectEvent);
                ActivateWidget(CurDragWidget);
                
                // Searchign for any drag point for this widget - it will be used
                // to shift all drag points for it
                for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
                {
                	if ( (*i)->Widget == CurDragWidget )
                	{
                		CurDragPoint = *i;
                		break;
                	}
                }
    		}
    		else
    		{
    			CurDragPoint = NewDragPoint;
                wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,CurDragPoint->Widget);
                wxPostEvent(wxSmith::Get(),SelectEvent);
                ActivateWidget(CurDragPoint->Widget);
    		}
    		
            for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
            {
                DragPointData* DPD = *i;
                DPD->DragInitPosX = DPD->PosX;
                DPD->DragInitPosY = DPD->PosY;
            }
    	}
    }

    // Processing Dragging event
    
    if ( event.Dragging() && CurDragPoint )
    {
        int ShiftX = MouseX - DragMouseBegX;
        int ShiftY = MouseY - DragMouseBegY;
        
        if ( abs(ShiftX) + abs(ShiftY) >= MinDragDistance ) DragDistanceSmall = false;
        
        if ( !DragDistanceSmall )
        {

            DragPointData* WidgetPoints[DragBoxTypeCnt];
            memcpy(WidgetPoints,CurDragPoint->WidgetPoints,sizeof(WidgetPoints));
            
            // Shifting corner points
            
            #define DoShiftX(Placement) WidgetPoints[Placement]->PosX = WidgetPoints[Placement]->DragInitPosX + ShiftX
            #define DoShiftY(Placement) WidgetPoints[Placement]->PosY = WidgetPoints[Placement]->DragInitPosY + ShiftY
            
            if ( CurDragWidget )
            {
                DoShiftX(LeftTop);
                DoShiftY(LeftTop);
                DoShiftX(RightTop);
                DoShiftY(RightTop);
                DoShiftX(LeftBtm);
                DoShiftY(LeftBtm);
                DoShiftX(RightBtm);
                DoShiftY(RightBtm);
            }
            else
            {
                switch ( CurDragPoint->Type )
                {
                    case LeftTop:
                        DoShiftX(LeftTop);
                        DoShiftY(LeftTop);
                        DoShiftY(RightTop);
                        DoShiftX(LeftBtm);
                        break;
                        
                    case Top:
                        DoShiftY(LeftTop);
                        DoShiftY(RightTop);
                        break;
                        
                    case RightTop:
                        DoShiftY(LeftTop);
                        DoShiftX(RightTop);
                        DoShiftY(RightTop);
                        DoShiftX(RightBtm);
                        break;
                    
                    case Left:
                        DoShiftX(LeftTop);
                        DoShiftX(LeftBtm);
                        break;
                        
                    case Right:
                        DoShiftX(RightTop);
                        DoShiftX(RightBtm);
                        break;
                        
                    case LeftBtm:
                        DoShiftX(LeftTop);
                        DoShiftX(LeftBtm);
                        DoShiftY(LeftBtm);
                        DoShiftY(RightBtm);
                        break;
                    
                    case Btm:
                        DoShiftY(LeftBtm);
                        DoShiftY(RightBtm);
                        break;
                        
                    case RightBtm:
                        DoShiftX(RightTop);
                        DoShiftY(LeftBtm);
                        DoShiftX(RightBtm);
                        DoShiftY(RightBtm);
                        break;
                        
                    default:;
                }
            }
            
            #undef DoShiftX
            #undef DoShiftY
            
            // Rebuilding edge points
            
            WidgetPoints[Top]->PosX = ( WidgetPoints[LeftTop]->PosX +  WidgetPoints[RightTop]->PosX ) / 2;
            WidgetPoints[Top]->PosY = WidgetPoints[LeftTop]->PosY;
            WidgetPoints[Left]->PosX = WidgetPoints[LeftTop]->PosX;
            WidgetPoints[Left]->PosY = ( WidgetPoints[LeftTop]->PosY + WidgetPoints[LeftBtm]->PosY ) / 2; 
            WidgetPoints[Right]->PosX = WidgetPoints[RightTop]->PosX;
            WidgetPoints[Right]->PosY = ( WidgetPoints[RightTop]->PosY + WidgetPoints[RightBtm]->PosY ) / 2; 
            WidgetPoints[Btm]->PosX = ( WidgetPoints[LeftBtm]->PosX +  WidgetPoints[RightBtm]->PosX ) / 2;
            WidgetPoints[Btm]->PosY = WidgetPoints[LeftBtm]->PosY;
            
            // Refreshing
            
            if ( RootWidget->GetPreview() )
            {
                RootWidget->GetPreview()->Refresh();
                RootWidget->GetPreview()->Update();
            }
            
            wxClientDC DC(this);
            AddGraphics(DC);
        }
    }
    
    // Updating widget's properties
    if ( event.LeftUp() || event.Dragging() )
    {
        int PosX = 0, PosY = 0;
        int SizeX = 0, SizeY = 0;
        
        if ( CurDragPoint && !DragDistanceSmall )
        {
            wxsWidget* Widget = CurDragPoint->Widget;
            Widget->GetPreview()->GetPosition(&PosX,&PosY);
    
            // Updating Widget's position and size
            
            DragPointData* LeftTopPoint = CurDragPoint->WidgetPoints[LeftTop];
            PosX += LeftTopPoint->PosX - LeftTopPoint->DragInitPosX;
            PosY += LeftTopPoint->PosY - LeftTopPoint->DragInitPosY;
            SizeX = LeftTopPoint->WidgetPoints[Right]->PosX - LeftTopPoint->PosX;
            SizeY = LeftTopPoint->WidgetPoints[Btm]->PosY - LeftTopPoint->PosY;

            // Applying changes
    
            wxsWidgetBaseParams& Params = Widget->GetBaseParams();
            Params.DefaultPosition = false;
            Params.DefaultSize = false;
            Params.PosX = PosX;
            Params.PosY = PosY;
            Params.SizeX = SizeX;
            Params.SizeY = SizeY;
            Widget->UpdateProperties();
    
            if ( event.LeftUp() )
            {
                CurDragPoint = NULL;
                CurDragWidget = NULL;
                
                Widget->UpdatePreview();
            }
        }
        
        if ( event.LeftUp() && HasCapture() )
        {
        	ReleaseMouse();
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

void wxsDragWindow::BuildDragPoints(wxsWidget* Widget)
{
	if ( !Widget ) return;
	
    for ( int i = 0; i<Widget->GetChildCount(); i++ )
    {
    	BuildDragPoints(Widget->GetChild(i));
    }
    
	//if ( Widget->GetBPType() & ( wxsWidget::bptSize | wxsWidget::bptPosition ) )
	{
        if ( Widget->GetPreview() )
        {
            DragPointData* WidgetPoints[DragBoxTypeCnt];
            
            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                WidgetPoints[i] = new DragPointData;
                WidgetPoints[i]->Invisible = true;
                WidgetPoints[i]->Inactive = false;
            }
            
            UpdateDragPointData(Widget,WidgetPoints);
                
            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                DragPoints.push_back(WidgetPoints[i]);
            }
        }
	}
}

void wxsDragWindow::UpdateDragPointData(wxsWidget* Widget,DragPointData** WidgetPoints)
{
    int PosX=0, PosY=0;
    int SizeX=0, SizeY=0;
    Widget->GetPreview()->ClientToScreen(&PosX,&PosY);
    Widget->GetPreview()->GetSize(&SizeX,&SizeY);
    
    for ( int i=0; i<DragBoxTypeCnt; ++i )
    {
        WidgetPoints[i]->Widget = Widget;
        WidgetPoints[i]->Type = (DragBoxType)i;
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

        WidgetPoints[i]->KillMe = false;
        memcpy(WidgetPoints[i]->WidgetPoints,WidgetPoints,sizeof(WidgetPoints[0]->WidgetPoints));
    }
}

void wxsDragWindow::RecalculateDragPoints()
{
    // If there are no dragpoints we jujst build new array
	if ( DragPoints.empty() )
	{
		BuildDragPoints(RootWidget);
		return;
	}
	
	// Setting KillMe flag for all points
	for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
	{
		(*i)->KillMe = true;
	}
	
	// Processing all widget in this window
	int HintIndex = 0;
	RecalculateDragPointsReq(RootWidget,HintIndex);

    // Deleting invalid drag points
    for ( size_t i=0; i<DragPoints.size(); )
    {
    	if ( DragPoints[i]->KillMe )
    	{
    		delete DragPoints[i];
    		DragPoints.erase(DragPoints.begin()+i);
    	}
    	else
    	{
    		i++;
    	}
    }
}

void wxsDragWindow::RecalculateDragPointsReq(wxsWidget* Widget,int& HintIndex)
{
    for ( int i = 0; i<Widget->GetChildCount(); i++ )
    {
    	RecalculateDragPointsReq(Widget->GetChild(i),HintIndex);
    }
	
	//if ( Widget->GetBPType() & ( wxsWidget::bptSize | wxsWidget::bptPosition ) )
	{
        if ( Widget->GetPreview() )
        {
        	int Index = HintIndex;
            while ( DragPoints[Index]->Widget != Widget )
            {
            	Index = (Index+1) % DragPoints.size();
            	if ( Index == HintIndex )
            	{
            		Index = -1;
            		break;
            	}
            }
            
            if ( Index == -1 )
            {
                // There's new widget in this window - we create new
                // drag points for it
                DragPointData* WidgetPoints[DragBoxTypeCnt];
                
                for ( int i=0; i<DragBoxTypeCnt; ++i )
                {
                    WidgetPoints[i] = new DragPointData;
                    WidgetPoints[i]->Invisible = true;
                    WidgetPoints[i]->Inactive = false;
                }
                
                UpdateDragPointData(Widget,WidgetPoints);
                    
                for ( int i=0; i<DragBoxTypeCnt; ++i )
                {
                    DragPoints.push_back(WidgetPoints[i]);
                }
            }
            else
            {
            	UpdateDragPointData(Widget,DragPoints[Index]->WidgetPoints);
            	HintIndex = ( Index + 1 ) % DragPoints.size();
            }
        }
	}
	
}

void wxsDragWindow::SetWidget(wxsWidget* _RootWidget)
{
	if ( RootWidget == _RootWidget )
	{
		RecalculateDragPoints();
	}
	else
	{
        ClearDragPoints();
        RootWidget = _RootWidget;
        BuildDragPoints(RootWidget);
	}
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

void wxsDragWindow::AddGraphics(wxDC& DC)
{
    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
    	DragPointData* DPD = *i;
    	if ( (*i)->Invisible ) continue;
        wxColor DrawColor( DPD->Inactive ? wxColor(0x80,0x80,0x80) : wxColor(0,0,0) );
        DC.SetPen( wxPen(DrawColor,1) );
        DC.SetBrush( wxBrush(DrawColor) );
        
        int PosX = DPD->PosX - DragBoxSize/2;
        int PosY = DPD->PosY - DragBoxSize/2;
        
        ScreenToClient(&PosX,&PosY);        
    	DC.DrawRectangle(PosX , PosY, DragBoxSize, DragBoxSize );
    }
}

void wxsDragWindow::ActivateWidget(wxsWidget* Widget,bool GrayTheRest)
{
    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
    	DragPointData* DPD = *i;
        if ( DPD->Widget == Widget )
        {
        	DPD->Invisible = false;
        	DPD->Inactive = false;
        }
        else if ( GrayTheRest && !DPD->Invisible )
        {
        	DPD->Inactive = true;
        }
        else
        {
        	DPD->Invisible = true;
        }
    }

    assert ( RootWidget->GetPreview() );
    
    RootWidget->GetPreview()->Refresh();
    RootWidget->GetPreview()->Update();
    
    wxClientDC DC(this);
    AddGraphics(DC);
}

BEGIN_EVENT_TABLE(wxsDragWindow,wxControl)
//    EVT_PAINT(wxsDragWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxsDragWindow::OnMouse)
    EVT_TIMER(1,wxsDragWindow::TimerRefresh)
END_EVENT_TABLE()
