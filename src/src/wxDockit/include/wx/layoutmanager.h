///////////////////////////////////////////////////////////////////////////////
// Name:        wx/layoutmanager.h
// Purpose:     wxLayoutManager class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LAYOUTMANAGER_H_
#define _WX_LAYOUTMANAGER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/dockit_defs.h>
#include <wx/object.h>
#include <wx/gdicmn.h>
#include <wx/event.h>
#include <wx/wfstream.h>

class wxFrame;
class wxDockHost;
class wxDockPanel;
class wxDockWindowBase;

#define wxDEFAULT_LEFT_HOST   "LeftHost"
#define wxDEFAULT_RIGHT_HOST  "RightHost"
#define wxDEFAULT_TOP_HOST    "TopHost"
#define wxDEFAULT_BOTTOM_HOST "BottomHost"

#define INITIAL_HOST_SIZE 64

// ----------------------------------------------------------------------------
// wxOwnerEventHandler
// ----------------------------------------------------------------------------

class wxOwnerEventHandler : public wxEvtHandler 
{
    DECLARE_CLASS( wxOwnerEventHandler )
    DECLARE_EVENT_TABLE()
public:
    wxOwnerEventHandler() {
        pOwner_ = NULL;
    }
    void SetOwner( class wxLayoutManager * pOwner ) {
        pOwner_ = pOwner;
    }

    void OnSize( wxSizeEvent &WXUNUSED(event) );
    void OnMove( wxMoveEvent &WXUNUSED(event) );
	void OnUpdateLayout( wxCommandEvent &WXUNUSED(event) );
	
private:
    class wxLayoutManager * pOwner_;
};

// ----------------------------------------------------------------------------
// HostInfo
// ----------------------------------------------------------------------------

enum ePlacement {
    HIP_NONE,
    HIP_FRONT,
    HIP_BACK
};

struct HostInfo {

    HostInfo() {
        Reset();
    }
    wxDockHost * operator =(wxDockHost * pDockHost) {
        pHost = pDockHost;
        valid = true;
        return pDockHost;
    }
    void Reset() {
        valid = false;
        pHost = NULL;
        pPanel = NULL;
        placement = HIP_NONE;
    }
    bool valid;
    wxDockHost * pHost;
    wxDockPanel * pPanel;
    ePlacement placement;
};

// ----------------------------------------------------------------------------
// wxLayoutManager
// ----------------------------------------------------------------------------

WX_DECLARE_LIST( wxDockHost, DockHostList );
WX_DECLARE_LIST( wxDockWindowBase, DockWindowList );

#define wxDWF_LIVE_UPDATE       0x01
#define	wxDWF_SPLITTER_BORDERS  0x02

class WXDOCKIT_DECLSPEC wxLayoutManager : public wxObject
{
    DECLARE_CLASS( wxLayoutManager )

public:
    wxLayoutManager( wxWindow * pOwnerWindow );
    ~wxLayoutManager();
    
    void Init();
    void SetLayout( unsigned int flags, wxWindow * pAutoLayoutClientWindow = NULL );

    // dockhost
    void AddDefaultHosts();
    void AddDockHost( wxDirection dir, int initialSize = INITIAL_HOST_SIZE, const wxString& name = "guessname" );
    wxDockHost * GetDockHost( const wxString& name );
    wxDockHost * GetDockHost( const wxDirection  &_dir );

    // dockwindow
    void AddDockWindow( wxDockWindowBase * pDockWindow );
    void DockWindow( wxDockWindowBase * pDockWindow, HostInfo &hi, bool noHideOperation = false );
    void UndockWindow( wxDockWindowBase * pDockWindow, bool noShowOperation = false );

    // load/save
    bool SaveToStream( wxOutputStream &stream );
    bool LoadFromStream( wxInputStream &stream );
    
    // access
    HostInfo TestForHost( int sx, int sy );
    wxRect TrimDockArea( wxDockHost * pDockHost, wxRect &dockArea );
	bool IsPrimaryDockHost( wxDockHost * pDockHost );
	void SetDockArea( wxRect &rect );
	wxRect GetDockArea();
    wxRect RectToScreen( wxRect &rect );
    wxPoint PointFromScreen( wxPoint &point );
    unsigned int GetFlags();

    void UpdateAllHosts( bool sizeChange, wxDockHost * pIgnoreHost = NULL );
    
    // event handers
    void OnSize();
    void OnMove();
	void OnUpdateLayout();
	
private:
    wxDockHost * findDockHost( const wxString& name );
    wxDockWindowBase * findDockWindow( const wxString& name );

    void settingsChanged();

private:
    DockHostList dockHosts_;
    DockWindowList dockWindows_;
    
    wxWindow * pOwnerWindow_;
    wxOwnerEventHandler frameEventHandler_;
    
    unsigned int flags_;
    wxWindow * pAutoLayoutClientWindow_;
	wxRect dockArea_;
};

// ----------------------------------------------------------------------------
// wxLayoutManager events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE( wxEVT_LAYOUT_CHANGED, wxEVT_FIRST + 1211 )   // TODO: must change ids
END_DECLARE_EVENT_TYPES()

#define EVT_LAYOUT_CHANGED( fn ) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_LAYOUT_CHANGED, -1, -1, (wxObjectEventFunction) (wxEventFunction) & fn, NULL ),

#endif
    // _WX_LAYOUTMANAGER_H_
