#ifndef WXSDRAGWINDOW_H
#define WXSDRAGWINDOW_H

#include <wx/control.h>
#include <vector>

class wxsWidget;

/** This class is used as an additional layer between window's preview
 *  and user input. 
 *
 * Main task for this class is to process all mouse and keyboard events
 * processed by preview window and adding additional features like
 * mouse-dragging boxes.
 */
class wxsDragWindow : public wxControl
{
	public:
	
        /** Ctor */
		wxsDragWindow(wxWindow* Parent,wxsWidget* RootWidget,const wxSize& Size);
		
		/** Dctor */
		virtual ~wxsDragWindow();
		
		/** Function changing root widget */
		void SetWidget(wxsWidget* RootWidget);

        /** Used to notify that this widget is transparent */
		virtual bool HasTransparentBackground() const { return true; }
		
	private:
	
        /** Painting event
         *
         * During painting process, all additional graphic items are drawn.
         */
        void OnPaint(wxPaintEvent& evt);
        
        /** Erasing background will do nothing */
        void OnEraseBack(wxEraseEvent& event);

        /** Event handler for all mouse events */
        void OnMouse(wxMouseEvent& event);
        
        /** Size of boxes used to drag borders of widgets */
        static const int DragBoxSize = 5;

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
            /*******/
            DragBoxTypeCnt
        };
        
        /** Structure describing one dragging point */
        struct DragPointData
        {
        	wxsWidget* Widget;                              ///< Widget associated with this box
        	DragBoxType Type;                               ///< Type of this drag box
        	bool Invisible;                                 ///< IF true, this point is hidden
        	bool Inactive;                                  ///< If true, this drag point will be drawn gray
        	int PosX;                                       ///< X position of this drag point
        	int PosY;                                       ///< Y position of this drag point
        	DragPointData* WidgetPoints[DragBoxTypeCnt];    ///< Pointers to all drag points for this widget
        };
        
        /** Declaration of vector containing all drag points */
        typedef std::vector<DragPointData*> DragPointsT;
        typedef DragPointsT::iterator DragPointsI;
        
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
        
        /** Position of dragged item at the beginning of dragging */
        int DragItemBegX, DragItemBegY;
        
        // Mics functions
        
        void ClearDragPoints();
        void RebuildDragPoints(wxsWidget* Widget);
        wxsWidget* FindWidgetAtPos(int PosX,int PosY,wxsWidget* Widget);
        
        DECLARE_EVENT_TABLE()
};

#endif
