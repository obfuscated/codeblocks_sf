#ifndef WXSDRAGWINDOW_H
#define WXSDRAGWINDOW_H

#include <wx/control.h>
#include <wx/timer.h>
#include <wx/bitmap.h>
#include <vector>
#include "wxsevent.h"
#include "wxsdrawingwindow.h"

class wxsWidget;
/** This class is used as an additional layer between window's preview
 *  and user input.
 *
 * Main task for this class is to process all mouse and keyboard events
 * processed by preview window and adding additional features like
 * mouse-dragging boxes.
 */
class wxsDragWindow: public wxsDrawingWindow
{
	public:

        /** Ctor */
		wxsDragWindow(wxWindow* Parent,wxsWidget* RootWidget);

		/** Dctor */
		virtual ~wxsDragWindow();

		/** Function changing root widget */
		void SetWidget(wxsWidget* RootWidget);

		/** Getting currently selected widget or NULL if there's no such widget inside this resource */
		wxsWidget* GetSelection();

		/** Getting count of widgets in multiple selection */
		int GetMultipleSelCount();

		/** Getting widget frrom multiple selection */
		wxsWidget* GetMultipleSelWidget(int Index);

        /** Getting vector of selected widges. If any-level parent of widget
         *  is also selected, widget is skipped.
         */
		void GetSelectionNoChildren(std::vector<wxsWidget*>& Vector);

		/** Checking if given widget is inside current selection */
		bool IsSelected(wxsWidget* Widget);

	private:

        WX_DECLARE_HASH_MAP(wxsWidget*,wxRect,wxPointerHash,wxPointerEqual,WidgetToRectMapT);

        /** Size of boxes used to drag borders of widgets */
        static const int DragBoxSize = 6;

        /** Minimal distace which must be done to apply dragging */
        static const int MinDragDistance = 8;

        /** Enum type describing placement of drag box */
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

        /** Structure describing one dragging point */
        struct DragPointData
        {
        	wxsWidget* Widget;                              ///< Widget associated with this box
        	DragBoxType Type;                               ///< Type of this drag box
        	//bool Invisible;                                 ///< If true, this point is hidden
        	bool Inactive;                                  ///< If true, this drag point will be drawn gray
        	bool NoAction;                                  ///< If true, thiss drag point is used to show placement of object only, not for moving or sizing
        	int PosX;                                       ///< X position of this drag point
        	int PosY;                                       ///< Y position of this drag point
        	int DragInitPosX;                               ///< X position before dragging
        	int DragInitPosY;                               ///< Y position before dragging
        	bool KillMe;                                    ///< Used while refreshing drag points list to find invalid points
        	DragPointData* WidgetPoints[DragBoxTypeCnt];    ///< Pointers to all drag points for this widget
        };

        /** Declaration of vector containing all drag points */
        typedef std::vector<DragPointData*> DragPointsT;
        typedef DragPointsT::iterator DragPointsI;

        /** Function drawing all additional graphic items */
        virtual void PaintExtra(wxDC* DC);

        /** Event handler for all mouse events */
        void OnMouse(wxMouseEvent& event);

        /** Forwarding mouse event to preview widget */
        void ForwardMouseEventToPreview(wxMouseEvent& event,wxsWidget* Widget);

        /** Searching for drag point under cursor */
        DragPointData* FindCoveredPoint(int PosX,int PosY);

        /** Searching for drag point attached to widget edge under cursor */
        DragPointData* FindCoveredEdge(int PosX,int PosY);

        /** Searching for left-top drag point for given widget */
        DragPointData* FindLeftTop(wxsWidget* Widget);

        /** Returning left-top drag point from other one */
        inline DragPointData* FindLeftTop(DragPointData* Point) { return Point->WidgetPoints[LeftTop]; }

        /** Initializing drag sequence */
        void DragInit(DragPointData* NewDragPoint,wxsWidget* NewDragWidget,bool MultipleSel,int MouseX,int MouseY);

        /** Processing mouse while dragging */
        void DragProcess(int MouseX,int MouseY,wxsWidget* UnderCursor);

        /** Finalizing dragging sequence */
        void DragFinish(wxsWidget* UnderCursor);

        /** Updating cursor */
        void UpdateCursor(bool Dragging,DragPointData* NewDragPoint,wxsWidget* NewDragWidget);

        /** Updating drag assist */
        void UpdateAssist(bool Dragging,wxsWidget* UnderCursor);

        /** Event handler for EVT_SELECT_WIDGET event */
        void OnSelectWidget(wxsEvent& event);

        /** Event handler for EVT_UNSELECT_WIDGET event */
        void OnUnselectWidget(wxsEvent& event);

        /** Removing all drag points */
        void ClearDragPoints();

        /** Adding drag points for given widget
         *  \return first drag point for this widget
         */
        DragPointData* BuildDragPoints(wxsWidget* Widget);

        /** Updating drag points for given widget */
        void UpdateDragPointData(wxsWidget* Widget,DragPointData** WidgetPoints);

        /** Updating content of current drag points
         *
         * This functions updates coordinates for drag points and removes
         * points for widgets which were removed.
         */
        void RecalculateDragPoints();

        /** Additional function used inside RecalculateDragPoints()  */
        void RecalculateDragPointsReq(wxsWidget* Widget,int& HintIndex);

        /** Changing cursor to one of defaults */
        inline void SetCur(int Cur);

        /** Finding absolute (screen-related) position of given widget */
        bool FindAbsoluteRect(wxsWidget* Widget,int* X,int* Y,int* Width,int* Height);

        /** Rebuilding map containing rects for widgets */
        void UpdateRectMapReq(wxsWidget* Widget);

        /** Searching for widget at given position */
        wxsWidget* FindWidgetAtPos(int PosX,int PosY,wxsWidget* Widget);

        /** Setting all current drag points to inactive (gray) */
        void GrayDragPoints();

        /** Setting drag points for given widget to active (black) */
        void BlackDragPoints(wxsWidget* Widget);

        /** Checking if given widget is inside widget tree */
        bool IsInside(wxsWidget* What,wxsWidget* Where);

        /** Checking if widget is truthly visible (it and all of it's parents must be shown) */
        bool IsVisible(wxsWidget* Wdg);

        /** Notifying outside about widget change */
        void SelectWidget(wxsWidget* Widget);

        /** Function used inside GetSelectionNoChildren to iterate through
            all widgets */
        void GetSelectionNoChildrenReq(wxsWidget* Widget,std::vector<wxsWidget*>& Vector);

        /** Function rebuilding drag points on edges of widgets from other points */
        void RebuildEdgePoints(DragPointData** WidgetPoints);

    /*******************************************/
    /* Variables                               */
    /*******************************************/

        /** All drag points used inside this drag window */
        DragPointsT DragPoints;

        /** Root widget for this resource */
        wxsWidget* RootWidget;

        /** Drag Point which is currently dragged */
        DragPointData* CurDragPoint;

        /** Widget which is currently dragged */
        wxsWidget* CurDragWidget;

        /** Mouse position at the beginning of dragging */
        int DragMouseBegX, DragMouseBegY;

        /** Set to true if distance while dragging was too small and will be discarded */
        bool DragDistanceSmall;

        /** Flag blocking incomming widget select events */
        bool BlockWidgetSelect;

        /** Target container where dragged widgets will be thrown (used while painting) */
        wxsWidget* DragParent;

        /** Changed background bitmap for DragParent */
        wxBitmap* DragParentBitmap;

        /** Highlighted widget which is under cursor when dragging (used while painting) */
        wxsWidget* DragTarget;

        /** Changed background bitmap for DragTarget */
        wxBitmap* DragTargetBitmap;

        /** Map assigning screen rectangle into one widget */
        WidgetToRectMapT WidgetToRectMap;

        /** Flag indicating whether we're dragging item or not */
        bool IsDragging;

        DECLARE_EVENT_TABLE()
};

#endif
