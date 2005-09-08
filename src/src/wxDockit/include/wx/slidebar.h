/////////////////////////////////////////////////////////////////////////////
// Name:        wx/slidebar.h
// Purpose:     wxSlideBar class
// Author:      Mark McCormack
// Modified by:
// Created:     25/05/05
// RCS-ID:
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SLIDEBAR_H_
#define _WX_SLIDEBAR_H_

#include <wx/defs.h>
#include <wx/dockit_defs.h>
#include <wx/barholder.h>
#include <wx/wfstream.h>

enum eSlideBarMode {
    wxSLIDE_MODE_SIMPLE,
    wxSLIDE_MODE_COMPACT
};

struct strBarPlacement {
	wxBarHolder * pBarHolder;
	wxRect placement;
};

class wxMenu;

// ----------------------------------------------------------------------------
// wxSlideBar control
// ----------------------------------------------------------------------------

WX_DECLARE_LIST( wxBarHolder, BarHolderList );
WX_DECLARE_OBJARRAY( strBarPlacement, BarPlacementArray );

class WXDOCKIT_DECLSPEC wxSlideBar : public wxWindow
{
public:
    // Default constructor
    wxSlideBar()
        : barList_( wxKEY_STRING ) {
        Init();
    }

    // Normal constructor
    wxSlideBar( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("slidebar") )
        : barList_( wxKEY_STRING ) {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    void Init();

    bool Create( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("slidebar") );

    virtual ~wxSlideBar();

    // standard interface
    wxBarHolder * AddWindow( wxWindow * pWindow, const wxString & label = wxT(""), unsigned int flags = wxBF_DEFAULT );
    void UpdateLayout();

	// extended interface
    void SetMode( eSlideBarMode mode );
    eSlideBarMode GetMode();
	void SetBarLock( bool enable );
	bool GetBarLock();

    // event handlers
    void OnQueryLayoutInfo( wxQueryLayoutInfoEvent& event );
    void OnCalculateLayout( wxCalculateLayoutEvent& event );
    void OnSize( wxSizeEvent &event );
	void OnContextMenu( wxContextMenuEvent &event );
	void OnLockBars( wxCommandEvent &event );
	void OnContextItem( wxCommandEvent &event );

    // overrides
    virtual void DoGetSize( int * x, int * y ) const;
    virtual wxSize DoGetBestSize() const;

    // load/save
    bool SaveToStream( wxOutputStream &stream );
    bool LoadFromStream( wxInputStream &stream );

    // access
    wxBarHolder * GetBarHolderAt( wxPoint pt );
	wxBarHolder * GetBarHolderAt( BarPlacementArray &barPlacementArray, wxPoint pt );
	void SwapBarHolders( wxBarHolder * p1, wxBarHolder * p2 );
	BarPlacementArray & CalcBarPlacement( int width, int * pAreaHeight = NULL );

private:
	void applyBarPlacement( BarPlacementArray & bpl );
	void createContextMenu();
	void deleteContextMenu();
    void refreshBars();

    DECLARE_DYNAMIC_CLASS( wxSlideBar )
    DECLARE_EVENT_TABLE()

private:
	BarPlacementArray barPlacementArray_;
    int areaHeight_;
    int areaWidth_;
    eSlideBarMode mode_;
    BarHolderList barList_;
	bool barLock_;
	wxMenu * pContextMenu_;

	int contextIdStart_;
	int contextIdEnd_;
};

// ----------------------------------------------------------------------------
// wxSlideBar events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE( wxEVT_SLIDEBAR_SIZE_CHANGED, wxEVT_FIRST + 1251 )  // TODO: must change id
    DECLARE_LOCAL_EVENT_TYPE( wxEVT_SLIDEBAR_UPDATE_LAYOUT, wxEVT_FIRST + 1252 )  // TODO: must change id
END_DECLARE_EVENT_TYPES()

#define EVT_SLIDEBAR_SIZE_CHANGED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_SIZE_CHANGED, -1, -1, (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#define EVT_SLIDEBAR_UPDATE_LAYOUT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SLIDEBAR_UPDATE_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),

#endif
    // _WX_SLIDEBAR_H_
