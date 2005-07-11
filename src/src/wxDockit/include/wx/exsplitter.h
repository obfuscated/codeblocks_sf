///////////////////////////////////////////////////////////////////////////////
// Name:        wx/exsplitter.h
// Purpose:     wxExSplitter class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EXSPLITTER_H_
#define _WX_EXSPLITTER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/window.h>

#define SPLITTER_WINDOWS     2
#define SPLITTER_SIZE        6
#define MINIMUM_WINDOW_SIZE  32

// ----------------------------------------------------------------------------
// wxExSplitter
// ----------------------------------------------------------------------------

#define wxESF_LIVE_UPDATE	0x01
#define	wxESF_DRAW_GRIPPER	0x02

class wxExSplitter : public wxWindow {
    DECLARE_CLASS( wxExSplitter )
    DECLARE_EVENT_TABLE()

public:
    wxExSplitter() {
        Init();
    }
    void Init();
    
    wxExSplitter( wxWindow * parent, wxOrientation orientation, wxWindow * win1, wxWindow * win2, unsigned int flags ) {
        Init();
        Create( parent, orientation, win1, win2, flags );
    }
    
    // basic interface
    bool Create( wxWindow * parent, wxOrientation orientation, wxWindow * win1, wxWindow * win2, unsigned int flags );
    
    // basic interface
    void SetWindows( wxWindow * pWin1, wxWindow * pWin2 );
    void SetFlags( unsigned int flags );
    
    // event handlers
    void OnLeftDown( wxMouseEvent& event );
    void OnMouseMove( wxMouseEvent& event );
    void OnErase( wxEraseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnLeftUp( wxMouseEvent& event );

    void SetBoundaries ();

private:
    void draw( wxRect r );

private:
    wxWindow * pOwner_;
    wxWindow * pWindow_[SPLITTER_WINDOWS];
    wxOrientation orientation_;
    wxPoint startPosition_;
    wxRect startRect_[SPLITTER_WINDOWS];
    wxRect startRectUs_;

    // movement boundary top-left / bottom-left points
    wxPoint boundary[2];

    bool dragging_;
    bool bounded_;
    int minSize_;
    wxRect endRect_[SPLITTER_WINDOWS];
    wxRect endRectUs_;
    unsigned int flags_;
    wxRect lastDrawnRect_;
};

// ----------------------------------------------------------------------------
// wxExSplitter events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE( wxEVT_SPLITTER_MOVED, wxEVT_FIRST + 1211 )  // TODO: must change id
END_DECLARE_EVENT_TYPES()

#define EVT_SPLITTER_MOVED( fn ) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SPLITTER_MOVED, -1, -1, (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),

#endif
    // _WX_EXSPLITTER_H_
