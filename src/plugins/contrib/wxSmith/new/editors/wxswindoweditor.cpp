#include "wxswindoweditor.h"
#include "wxsdrawingwindow.h"
#include "wxswindowresdataobject.h"
#include "../wxsitem.h"
#include "../wxsparent.h"
#include "../wxsproject.h"
#include "../wxsresourcetree.h"
#include "../wxsitemfactory.h"
#include "../wxsbaseproperties.h"

#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <manager.h>
#include <configmanager.h>

// TODO (SpOoN#1#): Move this class to separate file
class wxsWindowEditor::ContentManager: public wxsDrawingWindow
{
    public:

        /** \brief Ctor */
        ContentManager(wxsWindowEditor* _Parent): wxsDrawingWindow(_Parent,-1), Parent(_Parent), MouseState(msIdle) {}

        /** \brief Dctor */
        virtual ~ContentManager() {}

        /** \brief Function refreshing current selection */
        void RefreshSelection();

    protected:

        /** \brief painting additional stuff */
        virtual void PaintExtra(wxDC* DC);

    private:

        /** \brief Enum type describing placement of drag box */
        enum DragBoxType
        {
            LeftTop = 0,
            Top,
            RightTop,
            Left,
            Right,
            LeftBtm,
            Btm,
            RightBtm,
            /*************/
            DragBoxTypeCnt
        };

        /** \brief enum for available mouse states */
        enum MouseStatesT
        {
            msIdle,
            msDraggingPointInit,
            msDraggingPoint,
            msDraggingItemInit,
            msDraggingItem,
        };

        /** \brief Structure describing one dragging point */
        struct DragPointData
        {
        	wxsItem* Item;                                  ///< \brief Associated item
        	DragBoxType Type;                               ///< \brief Type of this drag box
        	bool Grey;                                      ///< \brief If true, this drag point will be drawn grey
        	int PosX;                                       ///< \brief X position of this drag point
        	int PosY;                                       ///< \brief Y position of this drag point
        	int DragInitPosX;                               ///< \brief X position before dragging
        	int DragInitPosY;                               ///< \brief Y position before dragging
        	DragPointData* ItemPoints[DragBoxTypeCnt];      ///< \brief Pointers to all drag points for this item
        };

        /** \brief Declaration of vector containing all drag points */
        WX_DEFINE_ARRAY(DragPointData*,DragPointsT);


        wxsWindowEditor* Parent;                            ///< \brief Current window editor
        DragPointsT DragPoints;                             ///< \brief Array of visible drag points
        MouseStatesT MouseState;                            ///< \brief Current mouse state
        static const int DragBoxSize = 6;                   ///< \brief Size of boxes used to drag borders of widgets
        static const int MinDragDistance = 8;               ///< \brief Minimal distace which must be done to apply dragging

        DragPointData* CurDragPoint;                        ///< \brief Dragged drag point
        wxsItem*       CurDragItem;                         ///< \brief Dragged item
        int            DragInitPosX;
        int            DragInitPosY;


        /** \brief Processing mouse events */
        void OnMouse(wxMouseEvent& event);

        void OnMouseIdle(wxMouseEvent& event);
        void OnMouseDraggingPoint(wxMouseEvent& event);
        void OnMouseDraggingPointInit(wxMouseEvent& event);
        void OnMouseDraggingItem(wxMouseEvent& event);
        void OnMouseDraggingItemInit(wxMouseEvent& event);

        inline wxsWindowRes* GetWinRes() { return (wxsWindowRes*)Parent->GetResource(); }
        inline wxsItem* RootItem()       { return ((wxsWindowRes*)Parent->GetResource())->GetRootItem();      }
        inline wxsItem* RootSelection()  { return ((wxsWindowRes*)Parent->GetResource())->GetRootSelection(); }
        inline void SetCur(int Cur)      { SetCursor(wxCursor(Cur)); }
        inline void ResourceLock()       { Parent->ResourceLock(); }
        inline void ResourceUnlock()     { Parent->ResourceUnlock(); }

        void RebuildDragPoints();
        void ClearDragPoints();
        void GreyDragPoints();
        void AddDragPoints(wxsItem* Item,wxsItem* RootSelection);
        void UpdateDragPoints(DragPointData* anyPoint);

        wxsItem* FindItemAtPos(int PosX,int PosY,wxsItem* SearchIn);
        DragPointData* FindDragPointAtPos(int PosX,int PosY);
        DragPointData* FindDragPointFromItem(wxsItem* Item);

        /** \brief Searching for new parent item during item dragging mode */
        bool FindDraggingItemTarget(int PosX,int PosY,wxsItem* Dragging,wxsParent*& NewParent,wxsItem*& AtCursor,bool& AddAfter);

        bool FindAbsoluteRect(wxsItem* Item,int& PosX,int& PosY,int& SizeX,int& SizeY);

        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsWindowEditor::ContentManager,wxsDrawingWindow)
    EVT_MOUSE_EVENTS(wxsWindowEditor::ContentManager::OnMouse)
END_EVENT_TABLE()

void wxsWindowEditor::ContentManager::PaintExtra(wxDC* DC)
{
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

void wxsWindowEditor::ContentManager::RefreshSelection()
{
    RebuildDragPoints();
    FullRepaint();
}

void wxsWindowEditor::ContentManager::ClearDragPoints()
{
    for ( size_t i = DragPoints.Count(); i-- > 0; )
    {
        delete DragPoints[i];
    }
    DragPoints.Clear();
}

void wxsWindowEditor::ContentManager::GreyDragPoints()
{
    for ( size_t i = DragPoints.Count(); i-->0; )
    {
        DragPoints[i]->Grey = true;
    }
}

void wxsWindowEditor::ContentManager::RebuildDragPoints()
{
    ClearDragPoints();
    AddDragPoints(RootItem(),RootSelection());
}

void wxsWindowEditor::ContentManager::AddDragPoints(wxsItem* Item,wxsItem* RootSelection)
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

void wxsWindowEditor::ContentManager::UpdateDragPoints(DragPointData* anyPoint)
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

bool wxsWindowEditor::ContentManager::FindAbsoluteRect(wxsItem* Item,int& PosX,int& PosY,int& SizeX,int& SizeY)
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

wxsItem* wxsWindowEditor::ContentManager::FindItemAtPos(int PosX,int PosY,wxsItem* SearchIn)
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

wxsWindowEditor::ContentManager::DragPointData* wxsWindowEditor::ContentManager::FindDragPointAtPos(int PosX,int PosY)
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

wxsWindowEditor::ContentManager::DragPointData* wxsWindowEditor::ContentManager::FindDragPointFromItem(wxsItem* Item)
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

void wxsWindowEditor::ContentManager::OnMouse(wxMouseEvent& event)
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

void wxsWindowEditor::ContentManager::OnMouseIdle(wxMouseEvent& event)
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

void wxsWindowEditor::ContentManager::OnMouseDraggingPointInit(wxMouseEvent& event)
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

void wxsWindowEditor::ContentManager::OnMouseDraggingPoint(wxMouseEvent& event)
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
        ResourceLock();
        ResourceUnlock();
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

void wxsWindowEditor::ContentManager::OnMouseDraggingItemInit(wxMouseEvent& event)
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
    }
}

void wxsWindowEditor::ContentManager::OnMouseDraggingItem(wxMouseEvent& event)
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
                        // TODO (SpOoN#1#): Store and restore additional parent properties like sizer flags
                        // TODO (SpOoN#1#): Update resource tree after update
                        // TODO (SpOoN#1#): Set new position
                        // TODO (SpOoN#1#): When parent did not change and parent is not sizer, do not unbind, only change position

                        CurDragItem->GetParent()->UnbindChild(CurDragItem);

                        // Adding to new one
                        int NewIndex = -1;
                        if ( AtCursor )
                        {
                            NewIndex = NewParent->GetChildIndex(AtCursor);
                            if ( AddAfter ) NewIndex++;
                        }

                        NewParent->AddChild(CurDragItem,NewIndex);
                    }
                }
            }
        }
        UpdateDragPoints(CurDragPoint);
        MouseState = msIdle;
        ResourceLock();
        ResourceUnlock();
        return;
    }

    int DeltaX = event.GetX() - DragInitPosX;
    int DeltaY = event.GetY() - DragInitPosY;

    for ( size_t i=0; i<DragPoints.Count(); i++ )
    {
        DragPoints[i]->PosX = DragPoints[i]->DragInitPosX + DeltaX;
        DragPoints[i]->PosY = DragPoints[i]->DragInitPosY + DeltaY;
    }

    FullRepaint();
}

bool wxsWindowEditor::ContentManager::FindDraggingItemTarget(int PosX,int PosY,wxsItem* Dragging,wxsParent*& NewParent,wxsItem*& AtCursor,bool& AddAfter)
{
    // Searching for item at cursor position
    wxsItem* Cursor = FindItemAtPos(PosX,PosY,RootItem());
    if ( !Cursor ) Cursor = RootItem();

    // Avoiding shifting into dragged item
    wxsParent* DraggedAsParent = Dragging->ToParent();
    if ( DraggedAsParent && DraggedAsParent->IsGrandChild(Cursor) )
    {
        return false;       // no move needed
    }

    NewParent = Cursor->ToParent();

    if ( NewParent )
    {
        // TODO (SpOoN#1#): Some key (alt ?) should forbid jumping here

        // Dragging over
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

static const long wxsInsIntoId    = wxNewId();
static const long wxsInsBeforeId  = wxNewId();
static const long wxsInsAfterId   = wxNewId();
static const long wxsDelId        = wxNewId();
static const long wxsPreviewId    = wxNewId();
static const long wxsQuickPropsId = wxNewId();

wxsWindowEditor::wxsWindowEditor(wxWindow* parent,wxsResource* Resource):
    wxsEditor(parent,wxEmptyString,Resource),
    TopPreview(NULL),
    QuickPropsOpen(false),
    ResourceLockCnt(0)
{
    wxASSERT(Resource!=NULL);

    InitializeImages();

    VertSizer = new wxBoxSizer(wxVERTICAL);
    WidgetsSet = new wxNotebook(this,-1);
    BuildPalette(WidgetsSet);
    HorizSizer = new wxBoxSizer(wxHORIZONTAL);
    VertSizer->Add(HorizSizer,1,wxEXPAND);
    VertSizer->Add(WidgetsSet,0,wxEXPAND);

    Content = new ContentManager(this);
    HorizSizer->Add(Content,1,wxEXPAND);

    QPArea = new wxScrolledWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxVSCROLL|wxSUNKEN_BORDER|wxALWAYS_SHOW_SB);
    QPArea->SetScrollbars(0,5,0,0);
    HorizSizer->Add(QPArea,0,wxEXPAND);
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);

    OpsSizer = new wxBoxSizer(wxVERTICAL);
    HorizSizer->Add(OpsSizer,0,wxEXPAND);

    OpsSizer->Add(InsIntoBtn   = new wxBitmapButton(this,wxsInsIntoId,InsIntoImg));
    OpsSizer->Add(InsBeforeBtn = new wxBitmapButton(this,wxsInsBeforeId,InsBeforeImg));
    OpsSizer->Add(InsAfterBtn  = new wxBitmapButton(this,wxsInsAfterId,InsAfterImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(DelBtn       = new wxBitmapButton(this,wxsDelId,DelImg));
    OpsSizer->Add(PreviewBtn   = new wxBitmapButton(this,wxsPreviewId,PreviewImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(QuickPanelBtn = new wxBitmapButton(this,wxsQuickPropsId,QuickPropsImgOpen));
    InsIntoBtn   ->SetToolTip(_("Insert new widgets into current selection"));
    InsBeforeBtn ->SetToolTip(_("Insert new widgets before current selection"));
    InsAfterBtn  ->SetToolTip(_("Insert new widgets after current selection"));
    DelBtn       ->SetToolTip(_("Delete current selection"));
    PreviewBtn   ->SetToolTip(_("Show preview"));
    QuickPanelBtn->SetToolTip(_("Open / Close Quick Properties panel"));

    SetSizer(VertSizer);

    SetInsertionTypeMask(0);

    if ( GetWinRes()->GetBasePropsFilter() == wxsFLFile )
    {
        InitFilename(GetWinRes()->GetXrcFile());
        SetTitle(m_Shortname);
    }
    else
    {
        wxASSERT_MSG( GetProject() != NULL, _T("Only wxsFLFile mode may not have project associated") );
        wxString FileName = GetProject()->GetProjectFileName(GetWinRes()->GetWxsFile());
        InitFilename(FileName);
        SetTitle(m_Shortname);
    }

    UndoBuff = new wxsWinUndoBuffer(GetWinRes());
    Corrector = new wxsCorrector(GetWinRes());
    ToggleQuickPropsPanel(false);
    AllEditors.insert(this);
    BuildPreview();

    // Changing selection to root item
    RootItem()->ClearSelection();
    GetWinRes()->SelectionChanged(NULL);
}

wxsWindowEditor::~wxsWindowEditor()
{
    // Destroying also Quick Props panel which usually triggers it's
    // Save() method when being destroyed
    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();

	KillPreview();

    // First we need to discard all changes,
    // this operation will recreate unmodified code
    // in source files
    if ( GetModified() )
    {
        // Loading resource from disk - this will recreate source code
        GetWinRes()->LoadResource();
        GetWinRes()->RebuildCode();
    }

    // Now doing the rest
	delete UndoBuff;
	delete Corrector;
	GetWinRes()->HidePreview();

	AllEditors.erase(this);
}

void wxsWindowEditor::ReloadImages()
{
    ImagesLoaded = false;
    InitializeImages();
    for ( WindowSet::iterator i=AllEditors.begin(); i!=AllEditors.end(); ++i )
    {
        (*i)->RebuildIcons();
    }
}

void wxsWindowEditor::BuildPreview()
{
    Content->SetSizer(NULL);
    Freeze();

    // Generating preview

    wxsItem* TopItem = GetWinRes()->GetRootItem();
    wxObject* TopPreviewObject = TopItem ? TopItem->BuildPreview(Content,false) : NULL;
    TopPreview = wxDynamicCast(TopPreviewObject,wxWindow);
    if ( !TopPreview )
    {
        delete TopPreviewObject;
        Content->RefreshSelection();
    }
    else
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(TopPreview,0,wxALL,10);
        Content->SetVirtualSizeHints(1,1);
        Content->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(Content);
        HorizSizer->Layout();
        VertSizer->Layout();
        Content->ContentChanged();
        Content->RefreshSelection();
    }

    // TODO: Check if these are needed
    Layout();
    Thaw();
    Refresh();
}

void wxsWindowEditor::KillPreview()
{
    if ( TopPreview )
    {
        Content->SetSizer(NULL);
        GetWinRes()->GetRootItem()->InvalidatePreview();
        delete TopPreview;
        TopPreview = NULL;
    }
}

bool wxsWindowEditor::Save()
{
    GetWinRes()->SaveResource();
    UndoBuff->Saved();
	return true;
}

bool wxsWindowEditor::GetModified()
{
	return GetWinRes()->GetModified();
}

void wxsWindowEditor::SetModified(bool modified)
{
    GetWinRes()->SetModified(modified);
    if ( GetWinRes()->GetModified() )
    {
        SetTitle(_T("*") + GetShortName());
    }
    else
    {
        SetTitle(GetShortName());
    }
}

bool wxsWindowEditor::CanUndo()
{
	return UndoBuff->CanUndo();
}

bool wxsWindowEditor::CanRedo()
{
	return UndoBuff->CanRedo();
}

void wxsWindowEditor::Undo()
{
    ResourceLock();
    UndoBuff->Undo();
    Corrector->ClearCache();
    ResourceUnlock();
	SetModified(UndoBuff->IsModified());
	// TODO: Restore selection
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::Redo()
{
    ResourceLock();
    UndoBuff->Redo();
    Corrector->ClearCache();
    ResourceUnlock();
	SetModified(UndoBuff->IsModified());
	// TODO: Restore selection
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

bool wxsWindowEditor::HasSelection()
{
    return HasSelection(RootItem());
}

bool wxsWindowEditor::HasSelection(wxsItem* Item)
{
    if ( Item->GetIsSelected() ) return true;
    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return false;
    for ( int i=0; i<Parent->GetChildCount(); ++i )
    {
        if ( HasSelection(Parent->GetChild(i)) ) return true;
    }
    return false;
}

bool wxsWindowEditor::CanPaste()
{
    if ( !wxTheClipboard->Open() ) return false;
    bool Res = wxTheClipboard->IsSupported(wxsDF_WIDGET);
    // FIXME (SpOoN#1#): Add support for text (XRC) data
    wxTheClipboard->Close();
    return Res;
}

void wxsWindowEditor::Cut()
{
    Copy();

    // Removing items copied into clipboard
    ResourceLock();
    KillSelection(RootItem());
    Corrector->ClearCache();
    BuildPreview();

    // TODO: Select previous item / parent item etc
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::KillSelection(wxsItem* Item)
{
    wxsParent* P = Item->ToParent();
    if ( P )
    {
        for ( int i = P->GetChildCount(); i-->0; )
        {
            wxsItem* Child = P->GetChild(i);
            if ( Child->GetIsSelected() )
            {
                 wxsTREE()->Delete(Child->GetLastTreeItemId());
                 P->UnbindChild(Child);
                 wxsKILL(Child);
            }
            else
            {
                KillSelection(Child);
            }
        }
    }
}

void wxsWindowEditor::Copy()
{
	// Almost all selected widgets will be added into clipboard
	// but with one exception - widget won't be added if parent of this
	// widget at any level is also selected

	ItemArray Items;
	GetSelectionNoChildren(Items,RootItem());

    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject* Data = new wxsWindowResDataObject;
    size_t Cnt = Items.Count();
    for ( size_t i=0; i<Cnt; i++ )
    {
    	Data->AddItem(Items[i]);
    }
    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();

}

void wxsWindowEditor::Paste()
{
    if ( !wxTheClipboard->Open() ) return;

    wxsWindowResDataObject Data;
    if ( wxTheClipboard->GetData(Data) )
    {
        wxsItem* RelativeTo = GetCurrentSelection();
        int InsertionType = InsType;
        if ( !RelativeTo )
        {
            InsertionType = itInto;
            RelativeTo = GetWinRes()->GetRootItem();
            wxsParent* Parent = RelativeTo->ToParent();
            if ( Parent &&
                 Parent->GetChildCount() == 1 &&
                 Parent->GetChild(0)->GetType() == wxsTSizer )
            {
                RelativeTo = Parent->GetChild(0);
            }
        }

        int Cnt = Data.GetItemCount();
        if ( Cnt )
        {
            ResourceLock();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsItem* Insert = Data.BuildItem(GetWinRes(),i);
                if ( Insert )
                {
                    switch ( InsertionType )
                    {
                        case itAfter:
                            InsertAfter(Insert,RelativeTo);
                            RelativeTo = Insert;
                            break;

                        case itBefore:
                            InsertBefore(Insert,RelativeTo);
                            break;

                        case itInto:
                            InsertInto(Insert,RelativeTo);
                            break;
                    }
                }
            }
            ResourceUnlock();
            GetWinRes()->RebuildCode();
        }
    }
    wxTheClipboard->Close();

	// TODO: Select added items
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

bool wxsWindowEditor::InsertBefore(wxsItem* New,wxsItem* Ref)
{
	if ( !Ref ) Ref = GetCurrentSelection();

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index) )
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),Parent->GetLastTreeItemId(),Index);

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

bool wxsWindowEditor::InsertAfter(wxsItem* New,wxsItem* Ref)
{
	if ( !Ref ) Ref = GetCurrentSelection();

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index+1))
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),Parent->GetLastTreeItemId(),Index+1);

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

bool wxsWindowEditor::InsertInto(wxsItem* New,wxsItem* Ref)
{
	if ( !Ref ) Ref = GetCurrentSelection();
	if ( !Ref || !Ref->ToParent() )
	{
		wxsKILL(New);
		return false;
	}

    Corrector->BeforePaste(New);
    wxsParent* P = Ref->ToParent();
    if ( !P->AddChild(New) )
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),P->GetLastTreeItemId());

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

void wxsWindowEditor::InitializeImages()
{
    if ( ImagesLoaded ) return;
    wxString basePath = ConfigManager::GetDataFolder() + _T("/images/wxsmith/");

    static const wxString NormalNames[] =
    {
        _T("insertinto.png"),
        _T("insertafter.png"),
        _T("insertbefore.png"),
        _T("deletewidget.png"),
        _T("showpreview.png"),
        _T("quickpropsopen.png"),
        _T("quickpropsclose.png"),
        _T("selected.png")
    };

    static const wxString SmallNames[] =
    {
        _T("insertinto16.png"),
        _T("insertafter16.png"),
        _T("insertbefore16.png"),
        _T("deletewidget16.png"),
        _T("showpreview16.png"),
        _T("quickpropsopen16.png"),
        _T("quickpropsclose16.png"),
        _T("selected16.png")
    };

    const wxString* Array = ( wxsDWToolIconSize == 16L ) ? SmallNames : NormalNames;

    InsIntoImg.LoadFile(basePath + Array[0]);
    InsAfterImg.LoadFile(basePath + Array[1]);
    InsBeforeImg.LoadFile(basePath + Array[2]);
    DelImg.LoadFile(basePath + Array[3]);
    PreviewImg.LoadFile(basePath + Array[4]);
    QuickPropsImgOpen.LoadFile(basePath + Array[5]);
    QuickPropsImgClose.LoadFile(basePath + Array[6]);
    SelectedImg.LoadFile(basePath + Array[7]);

    ImagesLoaded = true;
}

namespace
{
    int PrioritySort(const wxsItemInfo** it1,const wxsItemInfo** it2)
    {
        return (*it1)->Priority - (*it2)->Priority;
    }

    WX_DEFINE_ARRAY(const wxsItemInfo*,ItemsT);
    WX_DECLARE_STRING_HASH_MAP(ItemsT,MapT);
}

void wxsWindowEditor::BuildPalette(wxNotebook* Palette)
{
    Palette->DeleteAllPages();

    // First we need to split all widgets into groups
    // it will be done using multimap (map of arrays)

    MapT Map;

    for ( const wxsItemInfo* Info = wxsFACTORY()->GetFirstInfo(); Info; Info = wxsFACTORY()->GetNextInfo() )
    {
        if ( !Info->Category.empty() )
        {
            Map[Info->Category].Add(Info);
        }
    }

    for ( MapT::iterator i = Map.begin(); i!=Map.end(); ++i )
    {
        wxScrolledWindow* CurrentPanel = new wxScrolledWindow(Palette,-1,wxDefaultPosition,wxDefaultSize,0/*wxALWAYS_SHOW_SB|wxHSCROLL*/);
        CurrentPanel->SetScrollRate(1,0);
        Palette->AddPage(CurrentPanel,i->first);
        wxSizer* RowSizer = new wxBoxSizer(wxHORIZONTAL);

        ItemsT& Items = i->second;
        Items.Sort(PrioritySort);

        for ( size_t j=Items.Count(); j-->0; )
        {
            const wxsItemInfo* Info = Items[j];
            wxBitmap* Icon;
            if ( wxsDWPalIconSize == 16L )
            {
                Icon = Info->Icon16;
            }
            else
            {
                Icon = Info->Icon32;
            }

            if ( Icon )
            {
                wxBitmapButton* Btn =
                    new wxBitmapButton(CurrentPanel,-1,*Icon,
                        wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,
                        wxDefaultValidator, Info->Name);
                RowSizer->Add(Btn,0,wxALIGN_CENTER);
                Btn->SetToolTip(Info->Name);
            }
            else
            {
                wxButton* Btn = new wxButton(CurrentPanel,-1,Info->Name,
                    wxDefaultPosition,wxDefaultSize,0,
                    wxDefaultValidator,Info->Name);
                RowSizer->Add(Btn,0,wxGROW);
                Btn->SetToolTip(Info->Name);
            }
        }
        CurrentPanel->SetSizer(RowSizer);
        RowSizer->SetVirtualSizeHints(CurrentPanel);
    }
}

void wxsWindowEditor::InsertRequest(const wxString& Name)
{
    wxsItem* CurrentSelection = GetCurrentSelection();
    if ( !CurrentSelection )
    {
        DBGLOG(_("wxSmith: No item selected - couldn't create new item"));
        return;
    }

    if ( !InsType ) return;

    wxsItem* New = wxsGEN(Name,GetWinRes());
    if ( !New )
    {
        DBGLOG(_("wxSmith: Culdn't generate item inside factory"));
        return;
    }

    ResourceLock();
    switch ( InsType )
    {
        case itBefore:
            InsertBefore(New,CurrentSelection);
            break;

        case itAfter:
            InsertAfter(New,CurrentSelection);
            break;

        case itInto:
            InsertInto(New,CurrentSelection);
            break;

        default:
            wxsKILL(New);
            DBGLOG(_T("wxSmith: Internal error"));
            break;
    }
    ResourceUnlock();
    GetWinRes()->RebuildCode();
}

void wxsWindowEditor::OnButton(wxCommandEvent& event)
{
    wxWindow* Btn = (wxWindow*)event.GetEventObject();
    if ( Btn )
    {
        InsertRequest(Btn->GetName());
    }
}

void wxsWindowEditor::SetInsertionTypeMask(int Mask)
{
    InsTypeMask = Mask;
    SetInsertionType(InsType);
}

void wxsWindowEditor::SetInsertionType(int Type)
{
    Type &= InsTypeMask;

    if ( !Type ) Type = InsTypeMask;

    if ( Type & itInto )
    {
        InsType = itInto;
    }
    else if ( Type & itAfter )
    {
        InsType = itAfter;
    }
    else if ( Type & itBefore )
    {
        InsType = itBefore;
    }
    else
    {
        InsType = 0;
    }

    RebuildInsTypeIcons();
}

void wxsWindowEditor::RebuildInsTypeIcons()
{
    BuildInsTypeIcon(InsIntoBtn,InsIntoImg,(InsType&itInto)!=0,(InsTypeMask&itInto)!=0);
    BuildInsTypeIcon(InsBeforeBtn,InsBeforeImg,(InsType&itBefore)!=0,(InsTypeMask&itBefore)!=0);
    BuildInsTypeIcon(InsAfterBtn,InsAfterImg,(InsType&itAfter)!=0,(InsTypeMask&itAfter)!=0);
}

void wxsWindowEditor::BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,bool Selected,bool Enabled)
{
    if ( !Enabled || !Selected )
    {
        Btn->SetLabel(Original);
    }
    else
    {
        wxBitmap Copy = Original;
        wxMemoryDC DC;
        DC.SelectObject(Copy);
        DC.DrawBitmap(SelectedImg,0,0);
        Btn->SetLabel(Copy);
    }

    Btn->Enable(Enabled);
    Btn->Refresh();
}

void wxsWindowEditor::RebuildQuickPropsIcon()
{
    QuickPanelBtn->SetLabel( QuickPropsOpen ? QuickPropsImgClose : QuickPropsImgOpen );
}

void wxsWindowEditor::RebuildIcons()
{
    RebuildInsTypeIcons();
    RebuildQuickPropsIcon();
    DelBtn->SetLabel(DelImg);
    PreviewBtn->SetLabel(PreviewImg);
    BuildPalette(WidgetsSet);
    Layout();
}

void wxsWindowEditor::OnInsInto(wxCommandEvent& event)
{
    SetInsertionType(itInto);
}

void wxsWindowEditor::OnInsAfter(wxCommandEvent& event)
{
    SetInsertionType(itAfter);
}

void wxsWindowEditor::OnInsBefore(wxCommandEvent& event)
{
    SetInsertionType(itBefore);
}

void wxsWindowEditor::OnDelete(wxCommandEvent& event)
{
    ResourceLock();
    KillSelection(RootItem());
    ResourceUnlock();

	// TODO: Select previous item / parent etc.
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::OnPreview(wxCommandEvent& event)
{
    if ( GetWinRes()->IsPreview() )
    {
        GetWinRes()->HidePreview();
    }
    else
    {
        GetWinRes()->ShowPreview();
    }
}

void wxsWindowEditor::OnQuickProps(wxCommandEvent& event)
{
    QuickPropsOpen = !QuickPropsOpen;
    RebuildQuickPropsIcon();
    ToggleQuickPropsPanel(QuickPropsOpen);
}

void wxsWindowEditor::ToggleQuickPropsPanel(bool Open)
{
    HorizSizer->Show(QPArea,Open,true);
    Layout();
}

void wxsWindowEditor::RebuildQuickProps(wxsItem* Selection)
{
    Freeze();

    int QPx, QPy;

    QPArea->GetViewStart(&QPx,&QPy);
    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);

    if ( Selection )
    {
        wxWindow* QPPanel = Selection->BuildQuickPropertiesPanel(QPArea);
        if ( QPPanel )
        {
            QPSizer->Add(QPPanel,0,wxEXPAND);
        }
    }
    QPSizer->Layout();
    QPSizer->Fit(QPArea);
    Layout();
    QPArea->Scroll(QPx,QPy);
    Thaw();
}

void wxsWindowEditor::ResourceLock()
{
    if ( !ResourceLockCnt++ )
    {
        GetWinRes()->GetRootItem()->InvalidatePreview();
    }
}

void wxsWindowEditor::ResourceUnlock()
{
    if ( ! --ResourceLockCnt )
    {
        UndoBuff->StoreChange();
        Freeze();
        KillPreview();
        BuildPreview();
        Thaw();
        Content->ContentChanged();
        Content->RefreshSelection();
        SetModified(true);
// TODO (SpOoN#1#): Notify about data change
    }

    wxASSERT_MSG(ResourceLockCnt>=0,
        _T("wxsWindowEditor::ResourceUnlock() called without\n")
        _T("corresponding wxsWindowEditor::ResourceLock()"));
}

void wxsWindowEditor::SelectionChanged()
{
    wxsItem* Item = GetCurrentSelection();
    // Updating insertion type mask

    int itMask = 0;
    if ( Item )
    {
        if ( Item->GetParent() )
        {
            // When sizer is added into non-sizer parent, no other items can be added to
            // this parent
            if ( Item->GetType() != wxsTSizer ||
                 Item->GetParent()->GetType() == wxsTSizer )
            {
                itMask |= itBefore | itAfter;
            }
        }

        if ( Item->ToParent() )
        {
            itMask |= itInto;
        }
    }

    SetInsertionTypeMask(itMask);
    RebuildQuickProps(Item);

    // Refreshing selection items inside content window
    Content->RefreshSelection();

    // TODO: Refresh set of available items inside palette
}

void wxsWindowEditor::NotifyChange(wxsItem* Changed)
{
    ResourceLock();
    Corrector->AfterChange(Changed);
    ResourceUnlock();
}

void wxsWindowEditor::GetSelectionNoChildren(wxsWindowEditor::ItemArray& Array,wxsItem* Item)
{
    if ( Item->GetIsSelected() )
    {
        Array.Add(Item);
    }
    else
    {
        wxsParent* P = Item->ToParent();
        if ( P )
        {
            for ( int i=0; i<P->GetChildCount(); i++ )
            {
                GetSelectionNoChildren(Array,P->GetChild(i));
            }
        }
    }
}

wxImage wxsWindowEditor::InsIntoImg;
wxImage wxsWindowEditor::InsBeforeImg;
wxImage wxsWindowEditor::InsAfterImg;
wxImage wxsWindowEditor::DelImg;
wxImage wxsWindowEditor::PreviewImg;
wxImage wxsWindowEditor::QuickPropsImgOpen;
wxImage wxsWindowEditor::QuickPropsImgClose;
wxImage wxsWindowEditor::SelectedImg;
wxsWindowEditor::WindowSet wxsWindowEditor::AllEditors;
bool wxsWindowEditor::ImagesLoaded = false;

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_BUTTON(wxsInsIntoId,wxsWindowEditor::OnInsInto)
    EVT_BUTTON(wxsInsBeforeId,wxsWindowEditor::OnInsBefore)
    EVT_BUTTON(wxsInsAfterId,wxsWindowEditor::OnInsAfter)
    EVT_BUTTON(wxsDelId,wxsWindowEditor::OnDelete)
    EVT_BUTTON(wxsPreviewId,wxsWindowEditor::OnPreview)
    EVT_BUTTON(wxsQuickPropsId,wxsWindowEditor::OnQuickProps)
    EVT_BUTTON(-1,wxsWindowEditor::OnButton)
END_EVENT_TABLE()
