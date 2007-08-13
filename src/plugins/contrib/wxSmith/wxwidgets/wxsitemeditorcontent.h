#ifndef WXSITEMEDITORCONTENT_H
#define WXSITEMEDITORCONTENT_H

#include "wxsdrawingwindow.h"

class wxsItemEditorDragAssist;
class wxsItemResData;
class wxsItemEditor;
class wxsItemInfo;
class wxsParent;
class wxsItem;

/** \brief Class with window editor content (graphical area for editing resource) */
class wxsItemEditorContent: public wxsDrawingWindow
{
    public:

        /** \brief Ctor */
        wxsItemEditorContent(wxWindow* Parent,wxsItemResData* Data,wxsItemEditor* Editor);

        /** \brief Dctor */
        virtual ~wxsItemEditorContent();

        /** \brief Function shiwch must be called before changing preview */
        void BeforePreviewChanged();

        /** \brief Function which must be called after changing preview */
        void AfterPreviewChanged();

        /** \brief Function refreshing current selection (calculating new positions) */
        void RefreshSelection();

        /** \brief Starting sequence of adding new item by pointing with mouse */
        void InsertByPointing(const wxsItemInfo* Info);

    protected:

        /** \brief painting additional stuff */
        virtual void PaintExtra(wxDC* DC);

        /** \brief Using post-screenshoot procedure to get item's placement */
        virtual void ScreenShootTaken();

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
            msTargetSearch,
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

        wxsItemResData* m_Data;                             ///< \brief Data of handled resource
        wxsItemEditor* m_Editor;                            ///< \brief Main editor class
        DragPointsT m_DragPoints;                           ///< \brief Array of visible drag points
        ItemToRectT m_ItemToRect;                           ///< \brief Coordinates of each item stored here
        ItemToWindowT m_ItemToWindow;                       ///< \brief Window in editor area for each item is stored here
        MouseStatesT m_MouseState;                          ///< \brief Current mouse state
        static const int m_DragBoxSize = 6;                 ///< \brief Size of boxes used to drag borders of widgets
        static const int m_MinDragDistance = 8;             ///< \brief Minimal distace which must be done to apply dragging

        DragPointData* m_CurDragPoint;                      ///< \brief Dragged drag point
        wxsItem*       m_CurDragItem;                       ///< \brief Dragged item
        int            m_DragInitPosX;                      ///< \brief Initial mouse x position when dragging
        int            m_DragInitPosY;                      ///< \brief Initial mouse y position when dragging

        wxsItemEditorDragAssist* m_Assist;                  ///< \brief Assisting class
        wxsItem*   m_AssistTarget;
        wxsParent* m_AssistParent;
        bool       m_AssistAddAfter;

        const wxsItemInfo* m_TargetInfo;                    ///< \brief Info of item added by pointing with mouse
        int m_TargetX;
        int m_TargetY;


        /** \brief Processing mouse events */
        void OnMouse(wxMouseEvent& event);

        void OnMouseIdle(wxMouseEvent& event);
        void OnMouseDraggingPoint(wxMouseEvent& event);
        void OnMouseDraggingPointInit(wxMouseEvent& event);
        void OnMouseDraggingItem(wxMouseEvent& event);
        void OnMouseDraggingItemInit(wxMouseEvent& event);
        void OnMouseTargetSearch(wxMouseEvent& event);

        inline void SetCur(int Cur) { SetCursor(wxCursor(Cur)); }

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

        /** \brief Adding item in point-by-mouse mode */
        void AddItemAtTarget(wxsParent* AssistParent,int Position,const wxsItemInfo* Info,int PosX,int PosY);

        /** \brief Snappign coordinates to grid if necessary for coordinates relative to parent */
        void GridFixup(wxWindow* PreviewWindow,int& PosX,int& PosY);

        /** \brief Snapping coordinates to grid if necessary for global coordinates */
        void GridFixupForGlobalCoordinates(int& PosX,int& PosY,wxsItem* Owner);

        /** \brief Checking if we're using continous insert mode */
        static bool IsContinousInsert();

        friend class wxsItemEditorDragAssist;

        DECLARE_EVENT_TABLE()
};

#endif
