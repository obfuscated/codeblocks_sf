#include "wxsdragwindow.h"

#include <wx/dcclient.h>

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"
#include "resources/wxswindowres.h"

static const int wxsDWRefreshInterval = 50;

wxsDragWindow::wxsDragWindow(wxWindow* Cover,wxsWidget* Wdg,const wxSize& Size):
    wxControl(Cover,-1,wxDefaultPosition,Size,wxNO_BORDER|wxSTAY_ON_TOP),
    RootWidget(Wdg), CurDragPoint(NULL), CurDragWidget(NULL), RefreshTimer(this,1),
    BackFetchTimer(this,2), Background(NULL), BackFetchMode(true), PaintAfterFetch(false),
    BlockTimerRefresh(false), BlockWidgetSelect(false),
    DragParent(NULL), DragParentBitmap(NULL),
    DragTarget(NULL), DragTargetBitmap(NULL)
{
	RefreshTimer.Start(50);
	Background = new wxBitmap(GetSize().GetWidth(),GetSize().GetHeight());
}

wxsDragWindow::~wxsDragWindow()
{
	ClearDragPoints();
	delete Background;
}

void wxsDragWindow::OnPaint(wxPaintEvent& event)
{
    wxWindow* Wnd = this;
    wxPaintDC DC(Wnd);
	if ( !BackFetchMode || PaintAfterFetch )
	{
        AddGraphics(DC);
        PaintAfterFetch = false;
        BlockTimerRefresh = false;
	}
	else
	{
	    BlockTimerRefresh = true;
	    // When in background fetch mode, this widget is hidden in order
	    // to fetch background image
	    Hide();
	    FetchArea = GetUpdateRegion();
		BackFetchTimer.Start(wxsDWRefreshInterval,true);
	}
}

void wxsDragWindow::TimerRefresh(wxTimerEvent& event)
{
    if ( BlockTimerRefresh ) return;
    wxClientDC DC(this);
	AddGraphics(DC);
}

void wxsDragWindow::OnEraseBack(wxEraseEvent& event)
{
    if ( !BackFetchMode || PaintAfterFetch )
    {
        wxDC& DC = *event.GetDC();
        DC.DrawBitmap(*Background,0,0,false);
    }
}

void wxsDragWindow::OnMouse(wxMouseEvent& event)
{
    DragPointData* NewDragPoint = NULL;
    wxsWidget* NewDragWidget = NULL;
    int MouseX = event.GetX();
    int MouseY = event.GetY();
    wxsWidget* UnderCursor = FindWidgetAtPos(MouseX,MouseY,RootWidget);

    // Posting this event to previews
    ForwardMouseEventToPreview(event,UnderCursor);

    // Disabling background fetch mode when dragging
    BackFetchMode = !event.Dragging();

    // Searching for items covered by mouse
    NewDragPoint = FindCoveredPoint(MouseX,MouseY);
    if ( !NewDragPoint ) NewDragPoint = FindCoveredEdge(MouseX,MouseY);
    if ( !NewDragPoint ) NewDragWidget = UnderCursor;

	// Processing events
         if ( event.LeftUp()   ) DragFinish(UnderCursor);
    else if ( event.Dragging() ) DragProcess(MouseX,MouseY,UnderCursor);
    else if ( event.LeftDown() ) DragInit(NewDragPoint,NewDragWidget,event.ControlDown(),MouseX,MouseY);

    // Changing cursor
    UpdateCursor(event.Dragging(),NewDragPoint,NewDragWidget);
}

void wxsDragWindow::ForwardMouseEventToPreview(wxMouseEvent& event,wxsWidget* Widget)
{
// TODO (SpOoN#1#): Make this work
//    if ( Widget )
//    {
//    	int WidgetRelativeX = MouseX;
//    	int WidgetRelativeY = MouseY;
//    	ClientToScreen(&WidgetRelativeX,&WidgetRelativeY);
//    	Widget->GetPreview()->ScreenToClient(&WidgetRelativeX,&WidgetRelativeY);
//    	event.m_x = WidgetRelativeX;
//    	event.m_y = WidgetRelativeY;
//    	Widget->PreviewMouseEvent(event);
//    }
}

wxsDragWindow::DragPointData* wxsDragWindow::FindCoveredPoint(int MouseX,int MouseY)
{
    DragPointData* Found = NULL;
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
    	if ( !IsVisible((*i)->Widget) ) continue;
        int PosX = (*i)->PosX - DragBoxSize/2;
        int PosY = (*i)->PosY - DragBoxSize/2;

        if ( MouseX >= PosX &&
             MouseY >= PosY &&
             MouseX <= PosX + DragBoxSize &&
             MouseY <= PosY + DragBoxSize )
        {
            Found = *i;
            if ( !Found->NoAction ) break;
        }
    }
    return Found;
}

wxsDragWindow::DragPointData* wxsDragWindow::FindCoveredEdge(int MouseX,int MouseY)
{
    DragPointData* Found = NULL;

    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
        DragPointData* DPD = *i;

        if ( !IsVisible(DPD->Widget) ) continue;

        switch ( DPD->Type )
        {
            case Top:
            case Btm:
                {
                    int PosX1, PosX2;
                    FindAbsolutePosition(DPD->Widget,&PosX1,&PosX2);
                    ScreenToClient(&PosX1,&PosX2);
                    PosX2 = PosX1 + DPD->Widget->GetPreview()->GetSize().GetWidth();
                    int PosY = DPD->PosY - DragBoxSize / 2;

                    if ( MouseX >= PosX1 &&
                         MouseX <= PosX2 &&
                         MouseY >= PosY &&
                         MouseY <= PosY + DragBoxSize )
                    {
                        Found = DPD;
                    }
                }
                break;

            case Left:
            case Right:
                {
                    int PosY1, PosY2;
                    FindAbsolutePosition(DPD->Widget,&PosY1,&PosY2);
                    ScreenToClient(&PosY1,&PosY2);

                    PosY1 = PosY2;
                    PosY2 = PosY1 + DPD->Widget->GetPreview()->GetSize().GetHeight();
                    int PosX = DPD->PosX - DragBoxSize / 2;

                    if ( MouseY >= PosY1 &&
                         MouseY <= PosY2 &&
                         MouseX >= PosX &&
                         MouseX <= PosX + DragBoxSize )
                    {
                        Found = DPD;
                    }
                }
                break;

            default:
                break;
        }

        if ( Found && !Found->NoAction ) break;
    }

    return Found;
}

wxsDragWindow::DragPointData* wxsDragWindow::FindLeftTop(wxsWidget* Widget)
{
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
        if ( (*i)->Widget == Widget )
        {
            return (*i)->WidgetPoints[LeftTop];
        }
    }
    return NULL;
}

void wxsDragWindow::DragInit(wxsDragWindow::DragPointData* NewDragPoint,wxsWidget* NewDragWidget,bool MultipleSel,int MouseX,int MouseY)
{
    if ( NewDragPoint || NewDragWidget )
    {
        DragMouseBegX = MouseX;
        DragMouseBegY = MouseY;
        DragDistanceSmall = true;
        CaptureMouse();

        if ( NewDragWidget )
        {
            if ( MultipleSel ) { GrayDragPoints (); }
            else               { ClearDragPoints(); }

            CurDragWidget = NewDragWidget;
            CurDragPoint = FindLeftTop(CurDragWidget);

            if ( !CurDragPoint )
            {
                // Haven't found drag point for this widget - new points will be added
                CurDragPoint = BuildDragPoints(CurDragWidget);
                BlackDragPoints(CurDragWidget);
            }
            else
            {
                // This widget is already selected - only main selected widget will be changed
                GrayDragPoints();
                BlackDragPoints(CurDragWidget);
            }
            SelectWidget(CurDragWidget);
            UpdateGraphics();
        }
        else
        {
            // CurDragWidget == NULL means we're moving drag point only, not whole widget
            CurDragWidget = NULL;
            CurDragPoint = NewDragPoint;
        }

        for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
        {
            // Copying initial position data
            DragPointData* DPD = *i;
            DPD->DragInitPosX = DPD->PosX;
            DPD->DragInitPosY = DPD->PosY;
        }

    }
    else
    {
        // Nothing selected
        CurDragPoint = NULL;
        CurDragWidget = NULL;
    }
}

void wxsDragWindow::DragProcess(int MouseX,int MouseY,wxsWidget* UnderCursor)
{
    if ( !CurDragPoint || CurDragPoint->NoAction ) return;
    int ShiftX = MouseX - DragMouseBegX;
    int ShiftY = MouseY - DragMouseBegY;

    if ( abs(ShiftX) + abs(ShiftY) >= MinDragDistance ) DragDistanceSmall = false;
    if ( DragDistanceSmall ) return;

    // Creating local array of pointers to all drag points
    DragPointData* WidgetPoints[DragBoxTypeCnt];
    memcpy(WidgetPoints,CurDragPoint->WidgetPoints,sizeof(WidgetPoints));

    // Shifting corner points

    #define DoShiftX(Placement) WidgetPoints[Placement]->PosX = WidgetPoints[Placement]->DragInitPosX + ShiftX
    #define DoShiftY(Placement) WidgetPoints[Placement]->PosY = WidgetPoints[Placement]->DragInitPosY + ShiftY

    if ( CurDragWidget )
    {
        // Snapping to sizer area
        if ( UnderCursor && !UnderCursor->IsContainer() )
        {
            wxsWidget* Parent = UnderCursor->GetParent();
            if ( Parent && Parent->GetInfo().Sizer )
            {
                // Changing parent to sizer - current dragged widget
                // will be placed in place of UnderCursor
                ShiftX = 0;
                ShiftY = 0;
                UnderCursor->GetPreview()->ClientToScreen(&ShiftX,&ShiftY);
                CurDragWidget->GetPreview()->ScreenToClient(&ShiftX,&ShiftY);
            }
        }

        // Standard proceedure - just shifting everything
        for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
        {
            (*i)->PosX = (*i)->DragInitPosX + ShiftX;
            (*i)->PosY = (*i)->DragInitPosY + ShiftY;
        }

    }
    else
    {
        // Shifting corners
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

        // Rebuilding edge points

        WidgetPoints[Top  ]->PosX = ( WidgetPoints[LeftTop ]->PosX + WidgetPoints[RightTop]->PosX ) / 2;
        WidgetPoints[Top  ]->PosY =   WidgetPoints[LeftTop ]->PosY;
        WidgetPoints[Left ]->PosX =   WidgetPoints[LeftTop ]->PosX;
        WidgetPoints[Left ]->PosY = ( WidgetPoints[LeftTop ]->PosY + WidgetPoints[LeftBtm ]->PosY ) / 2;
        WidgetPoints[Right]->PosX =   WidgetPoints[RightTop]->PosX;
        WidgetPoints[Right]->PosY = ( WidgetPoints[RightTop]->PosY + WidgetPoints[RightBtm]->PosY ) / 2;
        WidgetPoints[Btm  ]->PosX = ( WidgetPoints[LeftBtm ]->PosX + WidgetPoints[RightBtm]->PosX ) / 2;
        WidgetPoints[Btm  ]->PosY =   WidgetPoints[LeftBtm ]->PosY;
    }

    #undef DoShiftX
    #undef DoShiftY

    UpdateGraphics();
}

void wxsDragWindow::DragFinish(wxsWidget* UnderCursor)
{
    if ( HasCapture() ) ReleaseMouse();
    if ( !CurDragPoint || CurDragPoint->NoAction || DragDistanceSmall ) return;

    int PosX = 0, PosY = 0;
    int SizeX = 0, SizeY = 0;

    if ( !CurDragWidget )
    {
        wxsWidget* Widget = CurDragPoint->Widget;
        Widget->GetPreview()->GetPosition(&PosX,&PosY);

        // Updating Widget's position and size
        DragPointData* LeftTopPoint = FindLeftTop(CurDragPoint);
        PosX += LeftTopPoint->PosX - LeftTopPoint->DragInitPosX;
        PosY += LeftTopPoint->PosY - LeftTopPoint->DragInitPosY;
        SizeX = LeftTopPoint->WidgetPoints[Right]->PosX - LeftTopPoint->PosX;
        SizeY = LeftTopPoint->WidgetPoints[Btm]->PosY - LeftTopPoint->PosY;

        // Applying changes
        wxsWidgetBaseParams& Params = Widget->GetBaseParams();
        if ( LeftTopPoint->PosX != LeftTopPoint->DragInitPosX ||
             LeftTopPoint->PosY != LeftTopPoint->DragInitPosY )
        {
            Params.DefaultPosition =
                Widget->GetParent() && Widget->GetParent()->GetInfo().Sizer;
            Params.PosX = PosX;
            Params.PosY = PosY;
        }
        Params.DefaultSize = false;
        Params.SizeX = SizeX;
        Params.SizeY = SizeY;
        Widget->UpdateProperties();
        Widget->PropertiesUpdated(false,false);     // This will recreate preview
    }
    else
    {
        // Finding out what new parent widget will be

        wxsWindowEditor* Editor = (wxsWindowEditor*)RootWidget->GetResource()->GetEditor();

        wxsWidget* NewParent = UnderCursor;
        bool NewParentIsSizer = NewParent->GetInfo().Sizer;
        int NewInSizerPos = -1;

        if ( !NewParent->IsContainer() )
        {
            NewParent = NewParent->GetParent();
            NewParentIsSizer = NewParent->GetInfo().Sizer;
            if ( NewParentIsSizer )
            {
                NewInSizerPos = NewParent->FindChild(UnderCursor);
            }
        }

        std::vector<wxsWidget*> AllToMove;
        GetSelectionNoChildren(AllToMove);

        // First pass - recalculating position
        int Cnt = (int)AllToMove.size();
        for ( int i=0; i<Cnt; i++ )
        {
            wxsWidget* Moved = AllToMove[i];
            wxsWidgetBaseParams& Params = Moved->GetBaseParams();
            DragPointData* LeftTopPoint = FindLeftTop(Moved);
            if ( LeftTopPoint )
            {
                FindAbsolutePosition(Moved,&PosX,&PosY);
                PosX += LeftTopPoint->PosX - LeftTopPoint->DragInitPosX;
                PosY += LeftTopPoint->PosY - LeftTopPoint->DragInitPosY;

                NewParent->GetPreview()->ScreenToClient(&PosX,&PosY);
                Params.PosX = PosX;
                Params.PosY = PosY;
            }
        }

        // Second pass - changing resource structure
        // Must kill preview to avoid seg faults caused by
        // differences between preview structure and resource's
        // structure
        Editor->KillPreview();
        for ( int i=0; i<Cnt; i++ )
        {
            wxsWidget* Moved = AllToMove[i];
            wxsWidget* OldParent = Moved->GetParent();
            int OldInSizerPos = OldParent->FindChild(Moved);

            if ( NewParent == OldParent )
            {
                if ( NewParentIsSizer )
                {
                    if ( NewInSizerPos != OldInSizerPos )
                    {
                        Moved->KillTree(wxsTREE());
                        OldParent->ChangeChildPos(OldInSizerPos,NewInSizerPos);
                        NewInSizerPos = OldParent->FindChild(Moved);
                        Moved->BuildTree(wxsTREE(),NewParent->GetTreeId(),NewInSizerPos);
                    }
                }
            }
            else
            {
                Moved->KillTree(wxsTREE());
                OldParent->DelChildId(OldInSizerPos);
                NewParent->AddChild(Moved,NewInSizerPos);
                Moved->BuildTree(wxsTREE(),NewParent->GetTreeId(),NewInSizerPos);
            }

            wxsWidgetBaseParams& Params = Moved->GetBaseParams();
            Params.DefaultPosition = NewParentIsSizer;
            if ( NewInSizerPos >= 0 )
            {
                NewInSizerPos++;
            }
        }

        wxsTREE()->Refresh();
        RootWidget->PropertiesUpdated(false,false);

        /*
        for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
        {
            DragPointData* LeftTopPoint = *i;
            if ( LeftTopPoint->Type != LeftTop ) continue;
            wxsWidget* Widget = LeftTopPoint->Widget;

            Widget->GetPreview()->GetPosition(&PosX,&PosY);

            // Updating Widget's position

            PosX += LeftTopPoint->PosX - LeftTopPoint->DragInitPosX;
            PosY += LeftTopPoint->PosY - LeftTopPoint->DragInitPosY;

            // Applying changes

            wxsWidgetBaseParams& Params = Widget->GetBaseParams();
            Params.PosX = PosX;
            Params.PosY = PosY;
            Params.DefaultPosition =
                Widget->GetParent()!=NULL &&
                Widget->GetParent()->GetInfo().Sizer;

            Widget->UpdateProperties();
            Widget->PropertiesUpdated(false,false);
        }
        */
    }

    CurDragPoint = NULL;
    CurDragWidget = NULL;
}

void wxsDragWindow::UpdateCursor(bool Dragging,DragPointData* NewDragPoint,wxsWidget* NewDragWidget)
{
    if ( !Dragging )
    {
        // We're not dragging - checkign what's under cursor
    	if ( !NewDragWidget && NewDragPoint )
    	{
    		switch ( NewDragPoint->Type )
    		{
                case LeftTop:
                case RightBtm:
                    SetCur(wxCURSOR_SIZENWSE);
                    break;

                case Top:
                case Btm:
                    SetCur(wxCURSOR_SIZENS);
                    break;

                case RightTop:
                case LeftBtm:
                    SetCur(wxCURSOR_SIZENESW);
                    break;

                case Left:
                case Right:
                    SetCur(wxCURSOR_SIZEWE);
                    break;

                default:
                    SetCur(wxCURSOR_ARROW);
    		}
    	}
    	else
    	{
    		SetCur(wxCURSOR_ARROW);
    	}
    }
    else
    {
        if ( CurDragWidget )
        {
            SetCur( CurDragPoint->NoAction ? wxCURSOR_NO_ENTRY : wxCURSOR_SIZING );
        }
        else if ( CurDragPoint )
        {
            if ( CurDragPoint->NoAction ) SetCur(wxCURSOR_NO_ENTRY);
        }
        else
        {
            SetCur(wxCURSOR_ARROW);
        }
    }
}

void wxsDragWindow::OnSelectWidget(wxsEvent& event)
{
    if ( BlockWidgetSelect ) return;
    if ( !IsInside(event.GetWidget(),RootWidget) )
    {
        ClearDragPoints();
    }
    else
    {
        wxsWidget* Wdg = event.GetWidget();
        if ( Wdg->GetParent() ) Wdg->GetParent()->EnsurePreviewVisible(Wdg);
        if ( ::wxGetKeyState(WXK_CONTROL) )
        {
            GrayDragPoints();
            BlackDragPoints(Wdg);
        }
        else
        {
            ClearDragPoints();
            BuildDragPoints(Wdg);
            BlackDragPoints(Wdg);
        }
    }
    UpdateGraphics();
}

void wxsDragWindow::OnUnselectWidget(wxsEvent& event)
{
    for ( size_t i = 0; i < DragPoints.size(); )
    {
    	DragPointData* DPD = DragPoints[i];
    	if ( DPD->Widget == event.GetWidget() )
    	{
    		DragPoints.erase(DragPoints.begin()+i);
    	}
    	else
    	{
    		i++;
    	}
    }
    UpdateGraphics();
}

void wxsDragWindow::ClearDragPoints()
{
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
    	delete *i;
    }
    DragPoints.clear();
}

wxsDragWindow::DragPointData* wxsDragWindow::BuildDragPoints(wxsWidget* Widget)
{
	if ( !Widget ) return NULL;

    if ( Widget->GetPreview() )
    {
        DragPointData* WidgetPoints[DragBoxTypeCnt];

        for ( int i=0; i<DragBoxTypeCnt; ++i )
        {
            WidgetPoints[i] = new DragPointData;
            WidgetPoints[i]->Inactive = false;
        }

        UpdateDragPointData(Widget,WidgetPoints);

        for ( int i=0; i<DragBoxTypeCnt; ++i )
        {
            DragPoints.push_back(WidgetPoints[i]);
        }

        return WidgetPoints[0];
    }

    return NULL;
}

void wxsDragWindow::UpdateDragPointData(wxsWidget* Widget,DragPointData** WidgetPoints)
{
    int PosX, PosY;
    int SizeX, SizeY;
    bool NoAction = ! ( Widget->GetBPType() & ( wxsWidget::bptSize | wxsWidget::bptPosition ) );

    FindAbsolutePosition(Widget,&PosX,&PosY);

    ScreenToClient(&PosX,&PosY);
    Widget->GetPreview()->GetSize(&SizeX,&SizeY);

    for ( int i=0; i<DragBoxTypeCnt; ++i )
    {
        WidgetPoints[i]->Widget = Widget;
        WidgetPoints[i]->Type = (DragBoxType)i;
        WidgetPoints[i]->PosX = PosX;
        WidgetPoints[i]->PosY = PosY;
        WidgetPoints[i]->NoAction = NoAction;

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
	if ( DragPoints.empty() ) return;

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

        if ( Index != -1 )
        {
            UpdateDragPointData(Widget,DragPoints[Index]->WidgetPoints);
            HintIndex = ( Index + 1 ) % DragPoints.size();
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
	Refresh();
}

wxsWidget* wxsDragWindow::FindWidgetAtPos(int PosX,int PosY,wxsWidget* Widget)
{
    if ( !Widget || !Widget->GetPreview() || !Widget->GetPreview()->IsShown() ) return NULL;

    int WdgX, WdgY;
    int WdgSX, WdgSY;
    FindAbsolutePosition(Widget,&WdgX,&WdgY);
    ScreenToClient(&WdgX,&WdgY);
    Widget->GetPreview()->GetSize(&WdgSX,&WdgSY);

    if ( PosX >= WdgX && PosY >= WdgY && PosX < WdgX + WdgSX && PosY < WdgY + WdgSY )
    {
        for ( int i=0; i<Widget->GetChildCount(); ++i )
        {
            wxsWidget* Wdg = FindWidgetAtPos(PosX,PosY,Widget->GetChild(i));
            if ( Wdg ) return Wdg;
        }

    	return Widget;
    }

    return NULL;
}

void wxsDragWindow::AddGraphics(wxDC& DC)
{
    /*
    if ( DragParent && DragParent->GetPreview() )
    {
        int PosX, PosY;
        FindAbsolutePosition(DragParent,&PosX,&PosY);
        ScreenToClient(&PosX,&PosY);

        if ( !DragParentBitmap )
        {
            int SizeX, SizeY;
            DragParent->GetPreview()->GetSize(&SizeX,&SizeY);
            wxImage Covered = Background->GetSubBitmap(wxRect(PosX,PosY,SizeX,SizeY));
            for ( int y=0; y<SizeX; y++ )
            {
                for ( int x=0; x<SizeX; x++ )
                {
                    Covered.SetRGB(
                }
            }
    }

    if ( DragTarget && DragTarget!=DragParent && DragTarget->GetPreview() )
    {

    }
    */

    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
    	DragPointData* DPD = *i;
        wxColor DrawColor( DPD->Inactive ? wxColor(0x80,0x80,0x80) : wxColor(0,0,0) );
        DC.SetPen( wxPen(DrawColor,1) );
        int Style = IsVisible(DPD->Widget) ? wxSOLID : wxTRANSPARENT;
        DC.SetBrush( wxBrush(DrawColor,Style) );

        int PosX = DPD->PosX - DragBoxSize/2;
        int PosY = DPD->PosY - DragBoxSize/2;

    	DC.DrawRectangle(PosX , PosY, DragBoxSize, DragBoxSize );
    }
}

void wxsDragWindow::SetCur(int Cur)
{
	SetCursor(wxCursor(Cur));
	if ( RootWidget && RootWidget->GetPreview() )
	{
        RootWidget->GetPreview()->SetCursor(wxCursor(Cur));
	}
}

void wxsDragWindow::OnSize(wxSizeEvent& event)
{
	NotifySizeChange(event.GetSize());
	event.Skip();
}

void wxsDragWindow::NotifySizeChange(const wxSize& Size)
{
    delete Background;
    Background = new wxBitmap(Size.GetWidth(),Size.GetHeight());
}

wxsWidget* wxsDragWindow::GetSelection()
{
	for ( DragPointsI i =  DragPoints.begin(); i!=DragPoints.end(); ++i )
	{
		if ( !(*i)->Inactive )
		{
			// Here's active drag point - it's at the edge of current selection
			return (*i)->Widget;
		}
	}
    return NULL;
}

int wxsDragWindow::GetMultipleSelCount()
{
	return DragPoints.size() / DragBoxTypeCnt;
}

wxsWidget* wxsDragWindow::GetMultipleSelWidget(int Index)
{
	Index *= DragBoxTypeCnt;
	if ( Index < 0 || Index >= (int)DragPoints.size() ) return NULL;
	return DragPoints[Index]->Widget;
}

void wxsDragWindow::OnFetchBackground(wxTimerEvent& event)
{
	wxScreenDC DC;
	wxMemoryDC DestDC;
    int X = 0, Y = 0;
    ClientToScreen(&X,&Y);
    DestDC.SelectObject(*Background);
    wxRegionIterator upd(FetchArea);
    while ( upd )
    {
        int x = upd.GetX();
        int y = upd.GetY();
        int W = upd.GetW();
        int H = upd.GetH();
        DestDC.Blit(x,y,W,H,&DC,X+x,Y+y);
        upd++;
    }

    ProcessPendingEvents();
	PaintAfterFetch = true;
	Show();
	Update();
	ProcessPendingEvents();
}

void wxsDragWindow::FindAbsolutePosition(wxsWidget* Widget,int* X,int* Y)
{
    *X = 0;
    *Y = 0;
    wxWindow* Wnd = Widget->GetPreview();

    Wnd->GetPosition(X,Y);
    Wnd->GetParent()->ClientToScreen(X,Y);
}

void wxsDragWindow::GrayDragPoints()
{
    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
    	(*i)->Inactive = true;
    }
}

void wxsDragWindow::BlackDragPoints(wxsWidget* Widget)
{
    for ( DragPointsI i = DragPoints.begin(); i != DragPoints.end(); ++i )
    {
        DragPointData* DPD = *i;
        if ( DPD->Widget == Widget )
        {
            (*i)->Inactive = false;
        }
    }
}

bool wxsDragWindow::IsInside(wxsWidget* What,wxsWidget* Where )
{
	return Where->FindChild(What,0) >= 0;
}

bool wxsDragWindow::IsVisible(wxsWidget* Widget)
{
	if ( !Widget ) return true;
	if ( !Widget->GetPreview() ) return false;
	if ( !Widget->GetPreview()->IsShown() ) return false;
	return IsVisible(Widget->GetParent());
}

void wxsDragWindow::GetSelectionNoChildren(std::vector<wxsWidget*>& Vector)
{
	Vector.clear();
	GetSelectionNoChildrenReq(RootWidget,Vector);
}

void wxsDragWindow::GetSelectionNoChildrenReq(wxsWidget* Widget,std::vector<wxsWidget*>& Vector)
{
    if ( !Widget )
    {
        return;
    }

	if ( IsSelected(Widget) )
	{
	    Vector.push_back(Widget);
	    return;
	}

    int Cnt = Widget->GetChildCount();
    for ( int i=0; i<Cnt; i++ )
    {
        GetSelectionNoChildrenReq(Widget->GetChild(i),Vector);
    }
}

void wxsDragWindow::SelectWidget(wxsWidget* Widget)
{
    BlockWidgetSelect = true;
    wxsSelectWidget(Widget);
    BlockWidgetSelect = false;
}

void wxsDragWindow::UpdateGraphics()
{
    wxClientDC DC(this);
    DC.DrawBitmap(*Background,0,0,false);
    AddGraphics(DC);
}

bool wxsDragWindow::IsSelected(wxsWidget* Widget)
{
    for ( DragPointsI i = DragPoints.begin(); i!=DragPoints.end(); ++i )
    {
        if ( (*i)->Widget == Widget ) return true;
    }
    return false;
}

BEGIN_EVENT_TABLE(wxsDragWindow,wxControl)
    EVT_PAINT(wxsDragWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxsDragWindow::OnMouse)
    EVT_ERASE_BACKGROUND(wxsDragWindow::OnEraseBack)
    EVT_TIMER(1,wxsDragWindow::TimerRefresh)
    EVT_TIMER(2,wxsDragWindow::OnFetchBackground)
    EVT_SELECT_WIDGET(wxsDragWindow::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsDragWindow::OnUnselectWidget)
    EVT_SIZE(wxsDragWindow::OnSize)
END_EVENT_TABLE()
