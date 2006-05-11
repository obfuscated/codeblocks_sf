#include "wxswindoweditorcontent.h"
#include "wxswindoweditordragassist.h"
#include "../wxsbaseproperties.h"

BEGIN_EVENT_TABLE(wxsWindowEditorContent,wxsDrawingWindow)
    EVT_MOUSE_EVENTS(wxsWindowEditorContent::OnMouse)
END_EVENT_TABLE()

wxsWindowEditorContent::wxsWindowEditorContent(wxsWindowEditor* _Parent):
    wxsDrawingWindow(_Parent,-1),
    Parent(_Parent),
    MouseState(msIdle),
    AssistTarget(NULL),
    AssistParent(NULL),
    AssistAddAfter(false)
{
    Assist = new wxsWindowEditorDragAssist(this);
}

wxsWindowEditorContent::~wxsWindowEditorContent()
{
    if ( Assist )
    {
        delete Assist;
    }
}

void wxsWindowEditorContent::PaintExtra(wxDC* DC)
{
    Assist->DrawExtra(AssistTarget,AssistParent,AssistAddAfter,DC);

    for ( size_t i = DragPoints.Count(); i-- > 0; )
    {
        DragPointData* DPD = DragPoints[i];
        if ( DPD->Grey )
        {
            DC->SetPen(*wxGREY_PEN);
            DC->SetBrush(*wxGREY_BRUSH);
        }
        else
        {
            DC->SetPen(*wxBLACK_PEN);
            DC->SetBrush(*wxBLACK_BRUSH);
        }
        int PosX = DPD->PosX - DragBoxSize/2;
        int PosY = DPD->PosY - DragBoxSize/2;
    	DC->DrawRectangle(PosX , PosY, DragBoxSize, DragBoxSize );
    }

}

void wxsWindowEditorContent::RefreshSelection()
{
    RebuildDragPoints();
    FullRepaint();
}

void wxsWindowEditorContent::ClearDragPoints()
{
    for ( size_t i = DragPoints.Count(); i-- > 0; )
    {
        delete DragPoints[i];
    }
    DragPoints.Clear();
}

void wxsWindowEditorContent::GreyDragPoints()
{
    for ( size_t i = DragPoints.Count(); i-->0; )
    {
        DragPoints[i]->Grey = true;
    }
}

void wxsWindowEditorContent::RebuildDragPoints()
{
    ClearDragPoints();
    AddDragPoints(RootItem(),RootSelection());
}

void wxsWindowEditorContent::AddDragPoints(wxsItem* Item,wxsItem* RootSelection)
{
    if ( Item->GetIsSelected() )
    {
        int PosX, PosY;
        int SizeX, SizeY;
        if ( FindAbsoluteRect(Item,PosX,PosY,SizeX,SizeY) )
        {
            bool Grey = Item!=RootSelection;
            DragPointData* ItemPoints[DragBoxTypeCnt];

            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                ItemPoints[i] = new DragPointData;
                ItemPoints[i]->Grey = Grey;
                ItemPoints[i]->PosX = PosX;
                ItemPoints[i]->PosY = PosY;
                ItemPoints[i]->Item = Item;
                ItemPoints[i]->Type = (DragBoxType)i;

                if ( i == Top || i == Btm )
                {
                    ItemPoints[i]->PosX += SizeX / 2;
                }
                else if ( i == RightTop || i == Right || i == RightBtm )
                {
                    ItemPoints[i]->PosX += SizeX;
                }

                if ( i==Left || i == Right )
                {
                    ItemPoints[i]->PosY += SizeY / 2;
                }
                else if ( i == LeftBtm || i == Btm || i == RightBtm )
                {
                    ItemPoints[i]->PosY += SizeY;
                }

                ItemPoints[i]->DragInitPosX = ItemPoints[i]->PosX;
                ItemPoints[i]->DragInitPosY = ItemPoints[i]->PosY;
            }

            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                memcpy(ItemPoints[i]->ItemPoints,ItemPoints,sizeof(ItemPoints[0]->ItemPoints));
                DragPoints.Add(ItemPoints[i]);
            }
        }
    }

    wxsParent* parent = Item->ToParent();
    if ( parent )
    {
        for ( int i = parent->GetChildCount(); i-->0; )
        {
            AddDragPoints(parent->GetChild(i),RootSelection);
        }
    }
}

void wxsWindowEditorContent::UpdateDragPoints(DragPointData* anyPoint)
{
    DragPointData** ItemPoints = anyPoint->ItemPoints;
    wxsItem* Item = anyPoint->Item;

    int PosX, PosY;
    int SizeX, SizeY;
    if ( FindAbsoluteRect(Item,PosX,PosY,SizeX,SizeY) )
    {
        for ( int i=0; i<DragBoxTypeCnt; ++i )
        {
            ItemPoints[i]->PosX = PosX;
            ItemPoints[i]->PosY = PosY;
            ItemPoints[i]->Item = Item;

            if ( i == Top || i == Btm )
            {
                ItemPoints[i]->PosX += SizeX / 2;
            }
            else if ( i == RightTop || i == Right || i == RightBtm )
            {
                ItemPoints[i]->PosX += SizeX;
            }

            if ( i==Left || i == Right )
            {
                ItemPoints[i]->PosY += SizeY / 2;
            }
            else if ( i == LeftBtm || i == Btm || i == RightBtm )
            {
                ItemPoints[i]->PosY += SizeY;
            }

            ItemPoints[i]->DragInitPosX = ItemPoints[i]->PosX;
            ItemPoints[i]->DragInitPosY = ItemPoints[i]->PosY;
        }
    }
}

bool wxsWindowEditorContent::FindAbsoluteRect(wxsItem* Item,int& PosX,int& PosY,int& SizeX,int& SizeY)
{
    if ( !Item ) return false;
    if ( !Item->GetPreview() ) return false;
    wxWindow* win = wxDynamicCast(Item->GetPreview(),wxWindow);
    if ( !win ) return false;
// TODO (SpOoN#1#): Add additional visibility check (query item's parent)
    if ( !win->IsShown() ) return false;
    PosX = 0;
    PosY = 0;
    win->GetPosition(&PosX,&PosY);
    win->GetParent()->ClientToScreen(&PosX,&PosY);
    ScreenToClient(&PosX,&PosY);
    win->GetSize(&SizeX,&SizeY);
    return true;
}

wxsItem* wxsWindowEditorContent::FindItemAtPos(int PosX,int PosY,wxsItem* SearchIn)
{
    int itemPosX;
    int itemPosY;
    int itemSizeX;
    int itemSizeY;

    if ( !FindAbsoluteRect(SearchIn,itemPosX,itemPosY,itemSizeX,itemSizeY) ) return NULL;

    if ( PosX < itemPosX ) return NULL;
    if ( PosX >= (itemPosX+itemSizeX) ) return NULL;
    if ( PosY < itemPosY ) return NULL;
    if ( PosY >= (itemPosY+itemSizeY) ) return NULL;

    wxsParent* parent = SearchIn->ToParent();
    if ( parent )
    {
        for ( int i = parent->GetChildCount(); i-->0; )
        {
            wxsItem* f = FindItemAtPos(PosX,PosY,parent->GetChild(i));
            if ( f )
            {
                return f;
            }
        }
    }

    return SearchIn;
}

wxsWindowEditorContent::DragPointData* wxsWindowEditorContent::FindDragPointAtPos(int PosX,int PosY)
{
    for ( size_t i=DragPoints.Count(); i-->0; )
    {
        DragPointData* DPD = DragPoints[i];
        int dpx = DPD->PosX - (DragBoxSize/2);
        int dpy = DPD->PosY - (DragBoxSize/2);

        if ( (PosX >= dpx) && (PosX < dpx+DragBoxSize) &&
             (PosY >= dpy) && (PosY < dpy+DragBoxSize) )
        {
            return DPD;
        }
    }

// TODO (SpOoN#1#): Search for edges

    return NULL;
}

wxsWindowEditorContent::DragPointData* wxsWindowEditorContent::FindDragPointFromItem(wxsItem* Item)
{
    for ( size_t i = 0; i<DragPoints.Count(); i+= 8 )
    {
        if ( DragPoints[i]->Item == Item )
        {
            return DragPoints[i];
        }
    }
    return NULL;
}

void wxsWindowEditorContent::OnMouse(wxMouseEvent& event)
{
    switch ( MouseState )
    {
        case msDraggingPointInit: OnMouseDraggingPointInit (event); break;
        case msDraggingPoint:     OnMouseDraggingPoint     (event); break;
        case msDraggingItemInit:  OnMouseDraggingItemInit  (event); break;
        case msDraggingItem:      OnMouseDraggingItem      (event); break;
        default:                  OnMouseIdle              (event); break;
    }
}

void wxsWindowEditorContent::OnMouseIdle(wxMouseEvent& event)
{
    BlockFetch(false);
    DragInitPosX = event.GetX();
    DragInitPosY = event.GetY();
    if ( event.LeftDown() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        // Selecting / drag init event
        int MouseX = event.GetX();
        int MouseY = event.GetY();

        wxsItem* OnCursor = FindItemAtPos(MouseX,MouseY,RootItem());
        if ( !OnCursor ) OnCursor = RootItem();

        // TODO (SpOoN#1#): Uncomment when done
        // ForwardClickToParent(OnCursor,MouseX,MouseY)

        DragPointData* DPD = FindDragPointAtPos(MouseX,MouseY);

        if ( DPD )
        {
            // If there's drag point, starting point-dragging sequence
            CurDragPoint = DPD;
            MouseState = msDraggingPointInit;
        }
        else
        {
            if ( !OnCursor->GetIsSelected() )
            {
                if ( !event.ControlDown() ) RootItem()->ClearSelection();
                OnCursor->SetIsSelected(true);
                GetWinRes()->SelectionChanged(OnCursor);
            }
            else
            {
                GetWinRes()->SelectionChanged(OnCursor);
            }

            CurDragPoint = FindDragPointFromItem(OnCursor);
            CurDragItem = OnCursor;
            MouseState = msDraggingItemInit;
        }
    }

    if ( !event.LeftIsDown() && event.RightDown() && !event.MiddleIsDown() )
    {
        // Menu invoking event
    }

    if ( !event.LeftIsDown() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        // Updating cursor

        DragPointData* DPD = FindDragPointAtPos(event.GetX(),event.GetY());

        if ( DPD )
        {
    		switch ( DPD->Type )
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
}

void wxsWindowEditorContent::OnMouseDraggingPointInit(wxMouseEvent& event)
{
    BlockFetch(true);

    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > MinDragDistance )
    {
        MouseState = msDraggingPoint;
    }
}

void wxsWindowEditorContent::OnMouseDraggingPoint(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<DragPoints.Count(); i++ )
        {
            DragPoints[i]->PosX = DragPoints[i]->DragInitPosX;
            DragPoints[i]->PosY = DragPoints[i]->DragInitPosY;
        }
        FullRepaint();
        MouseState = msIdle;
        return;
    }

    if ( !event.LeftIsDown() )
    {
        wxsBaseProperties* Props = CurDragPoint->Item->GetBaseProps();
        if ( Props )
        {
            DragPointData* leftTop = CurDragPoint->ItemPoints[LeftTop];
            DragPointData* rightBtm = CurDragPoint->ItemPoints[RightBtm];
            int OldPosX = leftTop->DragInitPosX;
            int OldPosY = leftTop->DragInitPosY;
            int OldSizeX = rightBtm->DragInitPosX - OldPosX;
            int OldSizeY = rightBtm->DragInitPosY - OldPosY;
            int NewPosX = leftTop->PosX;
            int NewPosY = leftTop->PosY;
            int NewSizeX = rightBtm->PosX - NewPosX;
            int NewSizeY = rightBtm->PosY - NewPosY;

            if ( NewSizeX < 0 )
            {
                NewPosX += NewSizeX;
                NewSizeX = -NewSizeX;
            }

            if ( NewSizeY < 0 )
            {
                NewPosY += NewSizeY;
                NewSizeY = -NewSizeY;
            }

            wxWindow* Preview = wxDynamicCast(CurDragPoint->Item->GetPreview(),wxWindow);

            if ( Preview )
            {
                if ( NewPosX!=OldPosX || NewPosY!=OldPosY )
                {
                    if ( CurDragItem->GetParent() && (CurDragItem->GetParent()->GetType() == wxsTSizer) )
                    {
                        Props->Position.SetPosition(wxDefaultPosition,Preview->GetParent());
                    }
                    else
                    {
                        Props->Position.SetPosition(wxPoint(NewPosX,NewPosY),Preview->GetParent());
                    }
                }

                if ( NewSizeX!=OldSizeX || NewSizeY!=OldSizeY )
                {
                    Props->Size.SetSize(wxSize(NewSizeX,NewSizeY),Preview->GetParent());
                }
            }
        }

        UpdateDragPoints(CurDragPoint);
        MouseState = msIdle;
        CurDragItem->NotifyPropertyChange(true);
        return;
    }

    int DeltaX = event.GetX() - DragInitPosX;
    int DeltaY = event.GetY() - DragInitPosY;

    DragPointData* leftTop = CurDragPoint->ItemPoints[LeftTop];
    DragPointData* rightBtm = CurDragPoint->ItemPoints[RightBtm];

    switch ( CurDragPoint->Type )
    {
        case LeftTop:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Top:
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case RightTop:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Left:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            break;

        case Right:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            break;

        case LeftBtm:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case Btm:
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case RightBtm:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        default:;
    }

    int LX = leftTop->PosX;
    int LY = leftTop->PosY;
    int RX = rightBtm->PosX;
    int RY = rightBtm->PosY;

    DragPointData** ItemPoints = leftTop->ItemPoints;

    ItemPoints[Top]->PosX = (LX+RX)/2;
    ItemPoints[Top]->PosY = LY;
    ItemPoints[RightTop]->PosX = RX;
    ItemPoints[RightTop]->PosY = LY;
    ItemPoints[Left]->PosX = LX;
    ItemPoints[Left]->PosY = (LY+RY) / 2;
    ItemPoints[Right]->PosX = RX;
    ItemPoints[Right]->PosY = (LY+RY) / 2;
    ItemPoints[LeftBtm]->PosX = LX;
    ItemPoints[LeftBtm]->PosY = RY;
    ItemPoints[Btm]->PosX = (LX+RX)/2;
    ItemPoints[Btm]->PosY = RY;
    FullRepaint();
}

void wxsWindowEditorContent::OnMouseDraggingItemInit(wxMouseEvent& event)
{
    BlockFetch(true);

    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > MinDragDistance )
    {
        MouseState = msDraggingItem;
        Assist->NewDragging();
    }
}

void wxsWindowEditorContent::OnMouseDraggingItem(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<DragPoints.Count(); i++ )
        {
            DragPoints[i]->PosX = DragPoints[i]->DragInitPosX;
            DragPoints[i]->PosY = DragPoints[i]->DragInitPosY;
        }
        MouseState = msIdle;
        AssistParent = NULL;
        AssistTarget = NULL;
        AssistAddAfter = false;
        Assist->NewDragging();
        FullRepaint();
        return;
    }

    if ( !event.LeftIsDown() )
    {
        // Applying change
        wxsBaseProperties* Props = CurDragItem->GetBaseProps();
        if ( Props )
        {
            if ( CurDragPoint->PosX != CurDragPoint->DragInitPosX ||
                 CurDragPoint->PosY != CurDragPoint->DragInitPosY )
            {
                wxsParent* NewParent = NULL;
                wxsItem* AtCursor = NULL;
                bool AddAfter = true;
                if ( FindDraggingItemTarget(event.GetX(),event.GetY(),CurDragItem,NewParent,AtCursor,AddAfter) )
                {
                    if ( (CurDragItem->GetParent() == NewParent) || NewParent->CanAddChild(CurDragItem,false) )
                    {
                        // TODO (SpOoN#1#): Update resource tree after update

                        wxsParent* CurParent = CurDragItem->GetParent();

                        if ( CurParent != NewParent ||
                             NewParent->GetType() == wxsTSizer )
                        {
                            if ( AtCursor != CurDragItem )
                            {
                                // Storing extra data
                                int CurIndex = CurParent->GetChildIndex(CurDragItem);
                                TiXmlElement ExtraData("extra");
                                CurParent->StoreExtraData(CurIndex,&ExtraData);

                                // Unbinding from old parent
                                CurDragItem->GetParent()->UnbindChild(CurDragItem);

                                // Adding to new one
                                int NewIndex = -1;
                                if ( AtCursor )
                                {
                                    NewIndex = NewParent->GetChildIndex(AtCursor);
                                    if ( AddAfter ) NewIndex++;
                                }

                                NewParent->AddChild(CurDragItem,NewIndex);

                                // Restoring extra data
                                NewIndex = NewParent->GetChildIndex(CurDragItem);
                                NewParent->RestoreExtraData(NewIndex,&ExtraData);
                            }
                        }

                        wxsBaseProperties* Props = CurDragItem->GetBaseProps();
                        if ( Props )
                        {
                            if ( NewParent->GetType() == wxsTSizer )
                            {
                                Props->Position.SetPosition(wxDefaultPosition,NULL);
                            }
                            else
                            {
                                // Calculating new position
                                int PosX;
                                int PosY;
                                int SizeX;
                                int SizeY;
                                if ( FindAbsoluteRect(CurDragItem,PosX,PosY,SizeX,SizeY) )
                                {
                                    PosX += CurDragPoint->PosX - CurDragPoint->DragInitPosX;
                                    PosY += CurDragPoint->PosY - CurDragPoint->DragInitPosY;
                                    ClientToScreen(&PosX,&PosY);
                                    wxWindow* PreviewParent = wxDynamicCast(CurDragItem->GetPreview(),wxWindow)->GetParent();
                                    if ( PreviewParent )
                                    {
                                        PreviewParent->ScreenToClient(&PosX,&PosY);
                                        Props->Position.SetPosition(wxPoint(PosX,PosY),PreviewParent);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        UpdateDragPoints(CurDragPoint);
        MouseState = msIdle;
        CurDragItem->NotifyPropertyChange(true);
        AssistTarget = NULL;
        AssistParent = NULL;
        AssistAddAfter = false;
        Assist->NewDragging();
        return;
    }

    int DeltaX = event.GetX() - DragInitPosX;
    int DeltaY = event.GetY() - DragInitPosY;

    for ( size_t i=0; i<DragPoints.Count(); i++ )
    {
        DragPoints[i]->PosX = DragPoints[i]->DragInitPosX + DeltaX;
        DragPoints[i]->PosY = DragPoints[i]->DragInitPosY + DeltaY;
    }

    if ( !FindDraggingItemTarget(event.GetX(),event.GetY(),CurDragItem,AssistParent,AssistTarget,AssistAddAfter) )
    {
        AssistTarget = NULL;
        AssistParent = NULL;
        AssistAddAfter = false;
    }
    FullRepaint();
}

bool wxsWindowEditorContent::FindDraggingItemTarget(int PosX,int PosY,wxsItem* Dragging,wxsParent*& NewParent,wxsItem*& AtCursor,bool& AddAfter)
{
    // Searching for item at cursor position
    wxsItem* Cursor = FindItemAtPos(PosX,PosY,RootItem());
    if ( !Cursor ) Cursor = RootItem();

    // Avoiding shifting into dragged item
    wxsParent* DraggedAsParent = Dragging->ToParent();
    if ( DraggedAsParent && DraggedAsParent->IsGrandChild(Cursor) )
    {
        // Can not drag into own child
        return false;
    }

    NewParent = Cursor->ToParent();

    if ( NewParent && !::wxGetKeyState(WXK_ALT) )
    {
        AtCursor = NULL;
        AddAfter = true;
        return true;
    }

    NewParent = Cursor->GetParent();
    if ( !NewParent )
    {
        // Should never be here, just in case
        return false;
    }

    if ( NewParent->GetType() == wxsTSizer )
    {
        AtCursor = Cursor;
        AddAfter = true;

        int ItemPosX;
        int ItemPosY;
        int ItemSizeX;
        int ItemSizeY;
        if ( FindAbsoluteRect(Cursor,ItemPosX,ItemPosY,ItemSizeX,ItemSizeY) )
        {
            // If cursor is on the left side, changing AddAfter flag to false
            if ( PosX < ItemPosX+(ItemSizeX/2) )
            {
                AddAfter = false;
            }
        }
    }
    else
    {
        AtCursor = NULL;
        AddAfter = true;
    }
    return true;
}

