#include "wxsheaders.h"
#include "wxsdragwindow.h"

#include <wx/dcclient.h>
#include <wx/dcbuffer.h>

#include <configmanager.h>

#include "widget.h"
#include "wxsevent.h"
#include "wxsmith.h"
#include "resources/wxswindowres.h"

wxsDragWindow::wxsDragWindow(wxWindow* Cover,wxsWidget* Wdg,const wxSize& Size):
    wxControl(Cover,-1,wxDefaultPosition,Size,wxNO_BORDER|wxSTAY_ON_TOP),
    RootWidget(Wdg), CurDragPoint(NULL), CurDragWidget(NULL), RefreshTimer(this,1),
    BackFetchTimer(this,2), Background(NULL), BackFetchMode(true), PaintAfterFetch(false),
    BlockTimerRefresh(false), BlockWidgetSelect(false),
    ContentDuringRecreate(false),
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
	if ( DragTargetBitmap ) delete DragTargetBitmap;
	if ( DragParentBitmap ) delete DragParentBitmap;
}

void wxsDragWindow::OnPaint(wxPaintEvent& event)
{
    wxWindow* Wnd = this;
    wxPaintDC DC(Wnd);

    if ( ContentDuringRecreate ) return;

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
	    FetchArea.Union(GetUpdateRegion());
		BackFetchTimer.Start(wxsDWFetchDelay,true);
	}
}

void wxsDragWindow::TimerRefresh(wxTimerEvent& event)
{
    if ( ContentDuringRecreate ) return;
    if ( BlockTimerRefresh ) return;
    wxClientDC DC(this);
	AddGraphics(DC);
}

void wxsDragWindow::OnEraseBack(wxEraseEvent& event)
{
    if ( ContentDuringRecreate ) return;
    if ( !BackFetchMode || PaintAfterFetch )
    {
        wxDC& DC = *event.GetDC();
        DC.DrawBitmap(*Background,0,0,false);
    }
}

void wxsDragWindow::OnMouse(wxMouseEvent& event)
{
    if ( ContentDuringRecreate ) return;
    DragPointData* NewDragPoint = NULL;
    wxsWidget* NewDragWidget = NULL;
    int MouseX = event.GetX();
    int MouseY = event.GetY();
    bool LeftDown = event.LeftDown();
    LeftDown = ! !LeftDown;
    wxsWidget* UnderCursor = FindWidgetAtPos(MouseX,MouseY,RootWidget);

    // If we're out of window
    if ( !UnderCursor )
    {
        UnderCursor = RootWidget;
        // Small trick - changing to probably best container
        while ( UnderCursor->GetChildCount()==1 &&
                UnderCursor->GetChild(0)->IsContainer() )
        {
            UnderCursor = UnderCursor->GetChild(0);
        }
    }

    // Posting this event to previews
    ForwardMouseEventToPreview(event,UnderCursor);

    // Disabling background fetch mode when dragging
    BackFetchMode = !event.Dragging();

    BlockTimerRefresh = event.Dragging();

    // Searching for items covered by mouse
    NewDragPoint = FindCoveredPoint(MouseX,MouseY);
    if ( !NewDragPoint ) NewDragPoint = FindCoveredEdge(MouseX,MouseY);
    if ( !NewDragPoint ) NewDragWidget = UnderCursor;

    // Updating drag assist
    UpdateAssist(event.Dragging(),UnderCursor);

	// Processing events
         if ( event.LeftUp()   ) DragFinish(UnderCursor);
    else if ( event.Dragging() ) DragProcess(MouseX,MouseY,UnderCursor);
    else if ( event.LeftDown() ) DragInit(NewDragPoint,NewDragWidget,event.ControlDown(),MouseX,MouseY);

    // Changing cursor
    UpdateCursor(event.Dragging(),NewDragPoint,NewDragWidget);
}

void wxsDragWindow::ForwardMouseEventToPreview(wxMouseEvent& event,wxsWidget* Widget)
{
    if ( Widget )
    {
    	int WidgetRelativeX = event.GetX();
    	int WidgetRelativeY = event.GetY();
    	ClientToScreen(&WidgetRelativeX,&WidgetRelativeY);
    	Widget->GetPreview()->ScreenToClient(&WidgetRelativeX,&WidgetRelativeY);
    	event.m_x = WidgetRelativeX;
    	event.m_y = WidgetRelativeY;
    	Widget->PreviewMouseEvent(event);
    }
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
                    int SizeW, SizeH;
                    DPD->Widget->GetPreview()->GetSize(&SizeW,&SizeH);
                    if ( SizeH < DragBoxSize ) break; // There must be place to drag this widget
                    FindAbsolutePosition(DPD->Widget,&PosX1,&PosX2);
                    ScreenToClient(&PosX1,&PosX2);
                    PosX2 = PosX1 + SizeW;
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
                    int SizeW, SizeH;
                    DPD->Widget->GetPreview()->GetSize(&SizeW,&SizeH);
                    if ( SizeW < DragBoxSize ) break; // There must be place to drag this widget
                    FindAbsolutePosition(DPD->Widget,&PosY1,&PosY2);
                    ScreenToClient(&PosY1,&PosY2);

                    PosY1 = PosY2;
                    PosY2 = PosY1 + SizeH;
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
        if ( UnderCursor && !UnderCursor->IsContainer() &&

             (wxsDWAssistType == wxsDTNone) )
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

        RebuildEdgePoints(WidgetPoints);
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

        RebuildEdgePoints(WidgetPoints);
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

        // Calculating new widget's position and size
        DragPointData* LeftTopPoint = FindLeftTop(CurDragPoint);
        PosX += LeftTopPoint->PosX - LeftTopPoint->DragInitPosX;
        PosY += LeftTopPoint->PosY - LeftTopPoint->DragInitPosY;
        SizeX = LeftTopPoint->WidgetPoints[Right]->PosX - LeftTopPoint->PosX;
        SizeY = LeftTopPoint->WidgetPoints[Btm]->PosY - LeftTopPoint->PosY;

        // Correcting negative size
        if ( SizeX < 0 )
        {
            PosX += SizeX;
            LeftTopPoint->PosX += SizeX;
            SizeX = -SizeX;
        }
        if ( SizeY < 0 )
        {
            PosY += SizeY;
            LeftTopPoint->PosY += SizeY;
            SizeY = -SizeY;
        }

        // Applying changes
        wxsBaseProperties& Params = Widget->BaseProperties;
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
        Widget->UpdatePropertiesWindow();
        Widget->PropertiesChanged(false,false);     // This will recreate preview
    }
    else
    {
        wxsWindowEditor* Editor = (wxsWindowEditor*)RootWidget->GetResource()->GetEditor();

        RootWidget->StoreCollapsed();

        std::vector<wxsWidget*> AllToMove;
        GetSelectionNoChildren(AllToMove);

        // Finding out what new parent widget will be
        wxsWidget* NewParent = UnderCursor;

        bool NewParentIsSizer = NewParent->GetInfo().Sizer;
        int NewInSizerPos = -1;
        int Cnt = (int)AllToMove.size();

        for(;;)
        {
            bool ForceMoreParent = false;
            for ( int i=0; i<Cnt; i++ )
            {
                if ( AllToMove[i] == NewParent ) ForceMoreParent = true;
            }

            if ( NewParent->IsContainer() && !ForceMoreParent ) break;
            NewParent = NewParent->GetParent();
            if ( !NewParent )
            {
                CurDragPoint = NULL;
                CurDragWidget = NULL;
                return;
            }

            NewParentIsSizer = NewParent->GetInfo().Sizer;
            if ( NewParentIsSizer )
            {
                NewInSizerPos = NewParent->FindChild(UnderCursor);

                // To make dragging more natural, we have to
                // change insert pos little bit

                if ( (CurDragWidget->GetParent() == NewParent) &&
                     (NewParent->FindChild(CurDragWidget) < NewInSizerPos) )
                {
                    NewInSizerPos++;
                }
            }
        }

        // First pass - checking if widget can be moved and
        //              recalculating position
        for ( int i=0; i<Cnt; i++ )
        {
            wxsWidget* Moved = AllToMove[i];
            if ( (Moved != NewParent) &&
                 (Moved->FindChild(NewParent,0) < 0) &&
                  NewParent->CanAddChild(Moved) )
            {
                wxsBaseProperties& Params = Moved->BaseProperties;
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
            else
            {
                // This widget won't be moved
                AllToMove.erase(AllToMove.begin()+i);
                i--;
                Cnt--;
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
                        // Moved->BuildTree(wxsTREE(),NewParent->GetTreeId(),NewInSizerPos);
                    }
                }
            }
            else
            {
                Moved->KillTree(wxsTREE());
                OldParent->DelChildId(OldInSizerPos);
                NewParent->AddChild(Moved,NewInSizerPos);
                //Moved->BuildTree(wxsTREE(),NewParent->GetTreeId(),NewInSizerPos);
            }
            Moved->GetResource()->RebuildTree(wxsTREE());

            wxsBaseProperties& Params = Moved->BaseProperties;
            Params.DefaultPosition = NewParentIsSizer;
            if ( NewInSizerPos >= 0 )
            {
                NewInSizerPos++;
            }
        }


//        wxsSelectWidget(GetSelection());
//        wxsTREE()->Expand(GetSelection()->GetTreeId());
        RootWidget->RestoreCollapsed();
        wxsTREE()->Refresh();
        RootWidget->PropertiesChanged(false,false);
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
    if ( ContentDuringRecreate ) return;
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
    Refresh();
}

void wxsDragWindow::OnUnselectWidget(wxsEvent& event)
{
    if ( ContentDuringRecreate ) return;
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
	if ( ContentDuringRecreate ) return NULL;

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
    if ( ContentDuringRecreate ) return;
    int PosX, PosY;
    int SizeX, SizeY;
    bool NoAction = false;// ! ( Widget->GetBPType() & ( wxsWidget::bptSize | wxsWidget::bptPosition ) );

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
    }

    for ( int i=0; i<DragBoxTypeCnt; ++i )
    {
        memcpy(WidgetPoints[i]->WidgetPoints,WidgetPoints,sizeof(WidgetPoints[0]->WidgetPoints));
    }
}

void wxsDragWindow::RecalculateDragPoints()
{
    if ( ContentDuringRecreate ) return;
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
    if ( ContentDuringRecreate ) return NULL;
    if ( !Widget || !Widget->GetPreview() || !Widget->GetPreview()->IsShown() ) return NULL;

    int WdgX, WdgY;
    int WdgSX, WdgSY;
    FindAbsolutePosition(Widget,&WdgX,&WdgY);
    ScreenToClient(&WdgX,&WdgY);
    Widget->GetPreview()->GetSize(&WdgSX,&WdgSY);

    for ( int i=0; i<Widget->GetChildCount(); ++i )
    {
        wxsWidget* Wdg = FindWidgetAtPos(PosX,PosY,Widget->GetChild(i));
        if ( Wdg ) return Wdg;
    }

    if ( PosX >= WdgX && PosY >= WdgY && PosX < WdgX + WdgSX && PosY < WdgY + WdgSY )
    {
    	return Widget;
    }

    return NULL;
}

void wxsDragWindow::AddGraphics(wxDC& DC)
{
    int DragAssistType = wxsDWAssistType;

    if ( DragAssistType )
    {
        if ( DragParent && DragParent->GetPreview() )
        {
            int PosX, PosY;
            int SizeX, SizeY;
            FindAbsolutePosition(DragParent,&PosX,&PosY);
            ScreenToClient(&PosX,&PosY);
            DragParent->GetPreview()->GetSize(&SizeX,&SizeY);
            long Col = wxsDWParentCol;
            int R = (Col>>16)&0xFF;
            int G = (Col>> 8)&0xFF;
            int B = (Col    )&0xFF;

            if ( DragAssistType == 1 )
            {
                DC.SetPen(wxPen(wxColour(R,G,B),2));
                DC.SetBrush(*wxTRANSPARENT_BRUSH);
                DC.DrawRectangle(PosX,PosY,SizeX,SizeY);
            }
            else
            {
                if ( !DragParentBitmap )
                {
                    wxImage Covered = Background->GetSubBitmap(wxRect(PosX,PosY,SizeX,SizeY)).ConvertToImage();
                    for ( int y=0; y<SizeY; y++ )
                    {
                        for ( int x=0; x<SizeX; x++ )
                        {
                            Covered.SetRGB(x,y,
                                ( Covered.GetRed(x,y)   + R ) / 2,
                                ( Covered.GetGreen(x,y) + G ) / 2,
                                ( Covered.GetBlue(x,y)  + B ) / 2 );
                        }
                    }
                    DragParentBitmap = new wxBitmap(Covered);
                }

                if ( DragParentBitmap )
                {
                    DC.DrawBitmap(*DragParentBitmap,PosX,PosY);
                }
            }
        }

        if ( DragTarget && (DragTarget!=DragParent) && DragTarget->GetPreview() )
        {
            int PosX, PosY;
            int SizeX, SizeY;
            FindAbsolutePosition(DragTarget,&PosX,&PosY);
            ScreenToClient(&PosX,&PosY);
            DragTarget->GetPreview()->GetSize(&SizeX,&SizeY);
            long Col = wxsDWTargetCol;
            int R = (Col>>16)&0xFF;
            int G = (Col>> 8)&0xFF;
            int B = (Col    )&0xFF;

            if ( DragAssistType == 1 )
            {
                DC.SetPen(wxPen(wxColour(R,G,B),2));
                DC.SetBrush(*wxTRANSPARENT_BRUSH);
                DC.DrawRectangle(PosX,PosY,SizeX,SizeY);
            }
            else
            {
                if ( !DragTargetBitmap )
                {
                    wxImage Covered = Background->GetSubBitmap(wxRect(PosX,PosY,SizeX,SizeY)).ConvertToImage();
                    for ( int y=0; y<SizeY; y++ )
                    {
                        for ( int x=0; x<SizeX; x++ )
                        {
                            Covered.SetRGB(x,y,
                                ( Covered.GetRed(x,y)   + R ) / 2,
                                ( Covered.GetGreen(x,y) + G ) / 2,
                                ( Covered.GetBlue(x,y)  + B ) / 2 );
                        }
                    }
                    DragTargetBitmap = new wxBitmap(Covered);
                }

                if ( DragTargetBitmap )
                {
                    DC.DrawBitmap(*DragTargetBitmap,PosX,PosY);
                }
            }
        }
    }

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
    if ( ContentDuringRecreate ) return NULL;
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
    if ( ContentDuringRecreate ) return 0;
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
    if ( DragTargetBitmap )
    {
        delete DragTargetBitmap;
        DragTargetBitmap = NULL;
    }
    if ( DragParentBitmap )
    {
        delete DragParentBitmap;
        DragParentBitmap = NULL;
    }
	wxScreenDC DC;
	wxMemoryDC DestDC;
    int X = 0, Y = 0;
    ClientToScreen(&X,&Y);
    DestDC.SelectObject(*Background);

    DestDC.Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight(),&DC,X,Y);

    /*
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
    */

    FetchArea.Clear();

    Manager::ProcessPendingEvents();
	PaintAfterFetch = true;
	Show();
	Update();
	Manager::ProcessPendingEvents();
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
    if ( !Where ) return false;
	return Where->FindChild(What,0) >= 0;
}

bool wxsDragWindow::IsVisible(wxsWidget* Widget)
{
	if ( !Widget ) return true;
	if ( !Widget->GetPreview() ) return false;
	if ( !Widget->GetPreview()->IsShown() ) return false;
	return Widget->ReallyVisible();
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
    wxClientDC ClientDC(this);

    wxBufferedDC DC(&ClientDC,GetSize());
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

void wxsDragWindow::UpdateAssist(bool Dragging,wxsWidget* UnderCursor)
{
    if ( !Dragging || !UnderCursor || DragDistanceSmall || !CurDragWidget )
    {
        DragTarget = NULL;
        DragParent = NULL;
        if ( DragTargetBitmap )
        {
            delete DragTargetBitmap;
            DragTargetBitmap = NULL;
        }
        if ( DragParentBitmap )
        {
            delete DragParentBitmap;
            DragParentBitmap = NULL;
        }
        return;
    }

    wxsWidget* Parent = UnderCursor;
    if ( !Parent->IsContainer() )
    {
        Parent = Parent->GetParent();
    }

    if ( DragTarget != UnderCursor )
    {
        DragTarget = UnderCursor;
        if ( DragTargetBitmap )
        {
            delete DragTargetBitmap;
            DragTargetBitmap = NULL;
        }
    }

    if ( DragParent != Parent )
    {
        DragParent = Parent;
        if ( DragParentBitmap )
        {
            delete DragParentBitmap;
            DragParentBitmap = NULL;
        }
    }
}

void wxsDragWindow::RebuildEdgePoints(wxsDragWindow::DragPointData** WidgetPoints)
{
    WidgetPoints[Top  ]->PosX = ( WidgetPoints[LeftTop ]->PosX + WidgetPoints[RightTop]->PosX ) / 2;
    WidgetPoints[Top  ]->PosY =   WidgetPoints[LeftTop ]->PosY;
    WidgetPoints[Left ]->PosX =   WidgetPoints[LeftTop ]->PosX;
    WidgetPoints[Left ]->PosY = ( WidgetPoints[LeftTop ]->PosY + WidgetPoints[LeftBtm ]->PosY ) / 2;
    WidgetPoints[Right]->PosX =   WidgetPoints[RightTop]->PosX;
    WidgetPoints[Right]->PosY = ( WidgetPoints[RightTop]->PosY + WidgetPoints[RightBtm]->PosY ) / 2;
    WidgetPoints[Btm  ]->PosX = ( WidgetPoints[LeftBtm ]->PosX + WidgetPoints[RightBtm]->PosX ) / 2;
    WidgetPoints[Btm  ]->PosY =   WidgetPoints[LeftBtm ]->PosY;
}

void wxsDragWindow::SetUpdateMode(bool Update)
{
    ContentDuringRecreate = Update;
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
