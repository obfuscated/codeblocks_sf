#ifndef WXSWINDOWEDITORCONTENT_H
#define WXSWINDOWEDITORCONTENT_H

#include "wxsdrawingwindow.h"
#include "wxswindoweditor.h"
#include "../wxsparent.h"

class wxsWindowEditorDragAssist;

/** \brief Class with window editor content */
class wxsWindowEditorContent: public wxsDrawingWindow
{
    public:

        /** \brief Ctor */
        wxsWindowEditorContent(wxsWindowEditor* _Parent);

        /** \brief Dctor */
        virtual ~wxsWindowEditorContent();

        /** \brief Function which must be called when preview change */
        void NewPreview();

        /** \brief Function refreshing current selection (calculating new positions) */
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

        /** \brief Structure containing rect for each item */
        WX_DECLARE_VOIDPTR_HASH_MAP(wxRect,ItemToRectT);

        /** \brief Structure containing wxWindow for each item */
        WX_DECLARE_VOIDPTR_HASH_MAP(wxWindow*,ItemToWindowT);

        wxsWindowEditor* Parent;                            ///< \brief Current window editor
        DragPointsT DragPoints;                             ///< \brief Array of visible drag points
        ItemToRectT ItemToRect;                             ///< \brief Coordinates of each item stored here
        ItemToWindowT ItemToWindow;                         ///< \brief Window in editor area for each item is stored here
        MouseStatesT MouseState;                            ///< \brief Current mouse state
        static const int DragBoxSize = 6;                   ///< \brief Size of boxes used to drag borders of widgets
        static const int MinDragDistance = 8;               ///< \brief Minimal distace which must be done to apply dragging

        DragPointData* CurDragPoint;                        ///< \brief Dragged drag point
        wxsItem*       CurDragItem;                         ///< \brief Dragged item
        int            DragInitPosX;                        ///< \brief Initial mouse x position when dragging
        int            DragInitPosY;                        ///< \brief Initial mouse y position when dragging

        wxsWindowEditorDragAssist* Assist;                  ///< \brief Assisting class
        wxsItem*   AssistTarget;
        wxsParent* AssistParent;
        bool       AssistAddAfter;


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
        inline void BeginChange()        { Parent->BeginChange(); }
        inline void EndChange()          { Parent->EndChange(); }

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

        /** \brief Getting bounding rect for given item */
        bool FindAbsoluteRect(wxsItem* Item,int& PosX,int& PosY,int& SizeX,int& SizeY);

        /** \brief Getting preview window for given item */
        wxWindow* GetPreviewWindow(wxsItem* Item);

        /** \brief Function updating ItemToRect and ItemToWindow maps */
        void RecalculateMaps();
        void RecalculateMapsReq(wxsItem* Item);

        friend class wxsWindowEditorDragAssist;

        DECLARE_EVENT_TABLE()
};

#endif
