/////////////////////////////////////////////////////////////////////////////
// Name:        wxLayoutManager.cpp
// Purpose:     wxLayoutManager implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:
// Copyright:
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/layoutmanager.h>
#include <wx/dockhost.h>
#include <wx/dockpanel.h>
#include <wx/dockwindow.h>
#include <wx/slidebar.h>
#include <wx/util.h>
#include <wx/mdi.h>

#include <wx/gdicmn.h>

#include <wx/list.h>
#include <wx/listimpl.cpp>

using namespace wxUtil;

// ----------------------------------------------------------------------------
// wxOwnerEventHandler implementation
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxOwnerEventHandler, wxEvtHandler )

BEGIN_EVENT_TABLE( wxOwnerEventHandler, wxEvtHandler )
    EVT_SIZE(wxOwnerEventHandler::OnSize)
    EVT_MOVE(wxOwnerEventHandler::OnMove)
	EVT_SLIDEBAR_UPDATE_LAYOUT(wxOwnerEventHandler::OnUpdateLayout)
END_EVENT_TABLE()

void wxOwnerEventHandler::OnSize( wxSizeEvent &event ) {
    if( pOwner_ ) {
        pOwner_->OnSize();
    }
    event.Skip();
}

void wxOwnerEventHandler::OnMove( wxMoveEvent &event ) {
    if( pOwner_ ) {
        pOwner_->OnMove();
    }
    event.Skip();
}

void wxOwnerEventHandler::OnUpdateLayout( wxCommandEvent& WXUNUSED(event) ) {
    if( pOwner_ ) {
        pOwner_->OnUpdateLayout();
    }
}

// ----------------------------------------------------------------------------
// wxLayoutManager constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxLayoutManager, wxObject )

DEFINE_LOCAL_EVENT_TYPE( wxEVT_LAYOUT_CHANGED )

WX_DECLARE_LIST( wxDockPanel, DockPanelList );
WX_DEFINE_LIST( DockPanelList );

WX_DEFINE_LIST( DockHostList );

#define STREAM_VERSION "wxDocking-Stream-v1.0"

// ----------------------------------------------------------------------------
// wxLayoutManager implementation
// ----------------------------------------------------------------------------
wxLayoutManager::wxLayoutManager( wxWindow * pOwnerFrame )
    : pOwnerWindow_( pOwnerFrame ) {
    Init();

    // install event handler
    frameEventHandler_.SetOwner( this );
    pOwnerWindow_->PushEventHandler( &frameEventHandler_ );
}

wxLayoutManager::~wxLayoutManager() {
    // uninstall event handler
    pOwnerWindow_->RemoveEventHandler( &frameEventHandler_ );

    // make sure all dockwindows extended Show() handling is turned off
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        wxASSERT(pDockWindow);
        pDockWindow->DisableShowOverride();
    }
}

void wxLayoutManager::Init() {
    dockHosts_.Clear();
    dockWindows_.Clear();
    flags_ = wxDWF_SPLITTER_BORDERS;
    pAutoLayoutClientWindow_ = NULL;
}

void wxLayoutManager::SetLayout( unsigned int flags, wxWindow * pAutoLayoutClientWindow ) {
    flags_ = flags;
    pAutoLayoutClientWindow_ = pAutoLayoutClientWindow;

    // generate internal event
    settingsChanged();
}

void wxLayoutManager::AddDefaultHosts() {
    // adds the standard four hosts - in the standard priority order
    AddDockHost( wxTOP );
    AddDockHost( wxBOTTOM );
    AddDockHost( wxLEFT );
    AddDockHost( wxRIGHT );
}

void wxLayoutManager::AddDockHost( wxDirection dir, int initialSize, const wxString& name ) {
    // fill in name?
    wxString dockName = name;
    if( dockName == "guessname" ) {
        switch( dir ) {
        case wxLEFT:
            dockName = wxDEFAULT_LEFT_HOST;
        break;
        case wxRIGHT:
            dockName = wxDEFAULT_RIGHT_HOST;
        break;
        case wxTOP:
            dockName = wxDEFAULT_TOP_HOST;
        break;
        case wxBOTTOM:
            dockName = wxDEFAULT_BOTTOM_HOST;
        break;
        default:
            wxASSERT_MSG( false, "AddDockHost() - direction parameter not recognised" );
        break;
        }
    }

    // check for duplicate
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( dockName == pDockHost->GetName() || dir == pDockHost->GetDirection() ) {
            wxASSERT_MSG( false, "AddDockHost() - direction or name already used" );
        }
    }

    // create host
    wxDockHost * pDockHost = new wxDockHost( pOwnerWindow_, 0, dir, dockName );
    pDockHost->SetLayoutManager( this );
    pDockHost->SetAreaSize( initialSize );

    // add a host
    dockHosts_.Append( pDockHost );
}

wxDockHost * wxLayoutManager::GetDockHost( const wxString& name ) {
    // find dock host
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( name == pDockHost->GetName()  ) {
            return pDockHost;
        }
    }

    return NULL;
}

wxDockHost  *wxLayoutManager :: GetDockHost ( const wxDirection  &_dir )
{
    // find dock host
    wxDockHost   *pDockHost = 0;

    for ( DockHostList :: Node  *node = dockHosts_.GetFirst(); node; node = node -> GetNext () )
    {
        pDockHost = node -> GetData ();

        if( pDockHost && ( _dir == pDockHost -> GetDirection () ) )
            return ( pDockHost );
    }

    return NULL;
}

void wxLayoutManager::AddDockWindow( wxDockWindowBase * pDockWindow ) {
    // check for duplicate
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pKnownDockWindow = node->GetData();
        if( pDockWindow->GetName() == pKnownDockWindow->GetName() ) {
            wxASSERT_MSG( false, "AddDockWindow() - name already used" );
        }
    }

    // add a window
    dockWindows_.Append( pDockWindow );
    pDockWindow->SetDockingManager( this );
}

HostInfo wxLayoutManager::TestForHost( int sx, int sy ) {
    HostInfo tHostInfo;
    // test host for screen co-ordinate inside of
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( pDockHost->TestForPanel( sx, sy, tHostInfo ) ) {
            break;
        }
    }
    return tHostInfo;
}

wxRect wxLayoutManager::TrimDockArea( wxDockHost * pDockHost, wxRect &dockArea ) {
    wxRect tDockArea( dockArea );

    // test host for screen co-ordinate inside of
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pKnownDockHost = node->GetData();

        if( pKnownDockHost == pDockHost ) {
            // ignore all hosts after us
            return tDockArea;
        }
        if( pKnownDockHost->IsEmpty() ) {
            // ignore empty dock hosts
            continue;
        }

        // chew of another bit
        wxRect chewArea = pKnownDockHost->GetClientArea();
        if( pKnownDockHost->GetDirection() == wxLEFT && pDockHost->GetDirection() != wxRIGHT) {
            if( chewArea.width ) {
                tDockArea.x += chewArea.width;
                tDockArea.width -= chewArea.width;
            }
        }
        if( pKnownDockHost->GetDirection() == wxRIGHT && pDockHost->GetDirection() != wxLEFT ) {
            if( chewArea.width ) {
                tDockArea.width -= chewArea.width + 1;
            }
        }
        if( pKnownDockHost->GetDirection() == wxTOP && pDockHost->GetDirection() != wxBOTTOM ) {
            if( chewArea.height ) {
                tDockArea.y += chewArea.height;
                tDockArea.height -= chewArea.height;
            }
        }
        if( pKnownDockHost->GetDirection() == wxBOTTOM && pDockHost->GetDirection() != wxTOP ) {
            if( chewArea.height ) {
                tDockArea.height -= chewArea.height + 1;
            }
        }
    }
    return tDockArea;
}

wxRect wxLayoutManager::RectToScreen( wxRect &rect ) {
    // convert owner (frame) rect to screen rect
    wxASSERT(pOwnerWindow_);
    wxRect tRect( rect );
    wxPoint pos = pOwnerWindow_->ClientToScreen( tRect.GetPosition() );
    tRect.SetPosition( pos );
    return tRect;
}

wxPoint wxLayoutManager::PointFromScreen( wxPoint &point ) {
    wxASSERT(pOwnerWindow_);
    return pOwnerWindow_->ScreenToClient( point );
}

unsigned int wxLayoutManager::GetFlags() {
    return flags_;
}

bool wxLayoutManager::IsPrimaryDockHost( wxDockHost * pDockHost ) {
	// are we the first dock host in the list?
	if( dockHosts_.IndexOf( pDockHost ) == 0 ) {
        return true;
	}
	else {
		return false;
	}
}

void wxLayoutManager::SetDockArea( wxRect &rect ) {
	dockArea_ = rect;
}

wxRect wxLayoutManager::GetDockArea() {
	return dockArea_;
}

void wxLayoutManager::DockWindow( wxDockWindowBase * pDockWindow, HostInfo &hi, bool noHideOperation ) {
    wxASSERT(hi.valid);
    wxASSERT(pDockWindow);

    wxDockPanel * pClient = pDockWindow->GetDockPanel();
    wxASSERT(pClient);

    // can't dock back into the same panel
    if( hi.pPanel == pClient ) {
        return;
    }

    // undock first if in a host
    if( pClient->GetDockedHost() ) {
        UndockWindow( pDockWindow, true );
    }
    // dock a window into a host
    wxDockHost * pDockHost = hi.pHost;
    pDockHost->DockPanel( pClient, hi );
    if( !noHideOperation ) {
        pDockWindow->ActualShow( false );
        pDockWindow->SetDocked( true );
    }
    pDockWindow->SetDockingInfo( hi );
    UpdateAllHosts( true );
}

void wxLayoutManager::UndockWindow( wxDockWindowBase * pDockWindow, bool noShowOperation ) {
    wxASSERT(pDockWindow);

    // already undocked?
    if( !pDockWindow->IsDocked() ) {
        return;
    }

    // undock a window
    wxDockPanel * pClient = pDockWindow->GetDockPanel();
    wxASSERT(pClient);
    wxDockHost * pDockHost = pClient->GetDockedHost();
    wxASSERT(pDockHost);
    pDockHost->UndockPanel( pClient );
    pDockWindow->Layout();
    if( !noShowOperation ) {
        pDockWindow->ActualShow( true );
        pDockWindow->SetDocked( false );
    }
    UpdateAllHosts( true );
}

void wxLayoutManager::OnSize() {
    // callback event generated by the a host size change
    UpdateAllHosts( true );
}

void wxLayoutManager::OnMove() {
    // XXX: no longer used
}

void wxLayoutManager::OnUpdateLayout() {
    UpdateAllHosts( false );
}

void wxLayoutManager::UpdateAllHosts( bool WXUNUSED(sizeChange), wxDockHost * WXUNUSED(pIgnoreHost) ) {
// NOTE (mandrav#1#): Moved event firing at the end of this function
    wxLayoutAlgorithm layout;
    wxMDIParentFrame * pMDIFrame = wxDynamicCast( pOwnerWindow_, wxMDIParentFrame );
	if( pMDIFrame ) {
		// layout within an MDI frame
		layout.LayoutMDIFrame( pMDIFrame );
	}
    else {
		// layout within an normal frame/or standard wxWindow
		layout.LayoutWindow( pOwnerWindow_, pAutoLayoutClientWindow_ );
    }

    // generate an event
    wxCommandEvent e( wxEVT_LAYOUT_CHANGED );
    e.SetEventObject( this );
    wxEvtHandler * pFrameEventHandler = pOwnerWindow_->GetEventHandler();
    wxASSERT( pFrameEventHandler );
    pFrameEventHandler->ProcessEvent( e );
}

wxDockHost * wxLayoutManager::findDockHost( const wxString& name ) {
    // look for a particular host
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        if( pDockHost->GetName() == name ) {
            // found
            return pDockHost;
        }
    }
    // not found
    return NULL;
}

wxDockWindowBase * wxLayoutManager::findDockWindow( const wxString& name ) {
    // look for a particular window
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();
        if( pDockWindow->GetName() == name ) {
            // found
            return pDockWindow;
        }
    }
    // not found
    return NULL;
}

void wxLayoutManager::settingsChanged() {
    // update size of all hosts
    for( DockHostList::Node *node = dockHosts_.GetFirst(); node; node = node->GetNext() ) {
        wxDockHost * pDockHost = node->GetData();
        pDockHost->SettingsChanged();
    }
}

bool wxLayoutManager::SaveToStream( wxOutputStream &stream ) {

    // version
    WriteString( stream, STREAM_VERSION );

    // windows
    WriteString( stream, "<layout>" );

    int winCount = dockWindows_.GetCount();
    stream.Write( &winCount, sizeof( winCount ) );
    for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
        wxDockWindowBase * pDockWindow = node->GetData();

        // name
        WriteString( stream, pDockWindow->GetName() );

        // undocked size
        wxRect size = pDockWindow->GetRect();
        stream.Write( &size.x, sizeof( size.x ) );
        stream.Write( &size.y, sizeof( size.y ) );
        stream.Write( &size.width, sizeof( size.width ) );
        stream.Write( &size.height, sizeof( size.height ) );

        // attributes
        bool isShown = pDockWindow->IsShown();
        stream.Write( &isShown, sizeof( isShown ) );
        bool isDocked = pDockWindow->IsDocked();
        stream.Write( &isDocked, sizeof( isDocked ) );

        // area taken
        wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
        wxASSERT(pDockPanel);
        int area = pDockPanel->GetArea();
        stream.Write( &area, sizeof( area ) );

        // docking info
        HostInfo &hi = pDockWindow->GetDockingInfo();
        if( hi.pHost ) {
            WriteString( stream, hi.pHost->GetName() );
        }
        else {
            WriteString( stream, "<nohost>" );
        }
    }

    // write out hosts
    int hostCount = dockHosts_.GetCount();
    stream.Write( &hostCount, sizeof( hostCount ) );
    for( DockHostList::Node *hnode = dockHosts_.GetFirst(); hnode; hnode = hnode->GetNext() ) {
        wxDockHost * pDockHost = hnode->GetData();

        // name
        WriteString( stream, pDockHost->GetName() );

        // area size
        int areaSize = pDockHost->GetAreaSize();
        stream.Write( &areaSize, sizeof( areaSize ) );

        // panel size
        int panelArea = pDockHost->GetPanelArea();
        stream.Write( &panelArea, sizeof( panelArea ) );

        // docked windows
        const DockWindowList & dwl = pDockHost->GetDockWindowList();
        int winCount = dwl.GetCount();
        stream.Write( &winCount, sizeof( winCount ) );
        for( DockWindowList::Node *lnode = dwl.GetFirst(); lnode; lnode = lnode->GetNext() ) {
            wxDockWindowBase * pDockWindow = lnode->GetData();

            // name
            WriteString( stream, pDockWindow->GetName() );
        }
    }

    return true;
}

bool wxLayoutManager::LoadFromStream( wxInputStream &stream ) {
	int i;
    // version
    wxString version = ReadString( stream );
    if( version != STREAM_VERSION ) {
        return false;
    }

    wxString layoutTag = ReadString( stream );
    if( layoutTag == "<layout>" ) {
        DockPanelList lockedPanels;

        // undock all windows
        for( DockWindowList::Node *node = dockWindows_.GetFirst(); node; node = node->GetNext() ) {
            wxDockWindowBase * pDockWindow = node->GetData();
            wxASSERT( pDockWindow );
            pDockWindow->Remove();
        }

        // read in windows
        int winCount = 0;
        stream.Read( &winCount, sizeof( winCount ) );
        for( i=0; i<winCount; i++ ) {
            // name
            wxString name = ReadString( stream );

            // undocked size
            wxRect size;
            stream.Read( &size.x, sizeof( size.x ) );
            stream.Read( &size.y, sizeof( size.y ) );
            stream.Read( &size.width, sizeof( size.width ) );
            stream.Read( &size.height, sizeof( size.height ) );

            // attributes
            bool isShown = false;
            stream.Read( &isShown, sizeof( isShown ) );
            bool isDocked = false;
            stream.Read( &isDocked, sizeof( isDocked ) );

            // area taken
            int area = 0;
            stream.Read( &area, sizeof( area ) );

            // docking info
            wxString host = ReadString( stream );

            // find window and apply
            wxDockWindowBase * pDockWindow = findDockWindow( name );
            if( pDockWindow ) {
                pDockWindow->SetSize( size );
                pDockWindow->SetDocked( isDocked );

                wxDockPanel * pDockPanel = pDockWindow->GetDockPanel();
                wxASSERT( pDockPanel );

                // panel area
                pDockPanel->SetArea( area );
                pDockPanel->LockAreaValue( true );
                lockedPanels.Append( pDockPanel );

                // docking info
                wxDockHost * pDockHost = findDockHost( host );
                if( pDockHost && isDocked ) {
                    // was docked
                    HostInfo hi;
                    hi = pDockHost;
                    pDockWindow->SetDockingInfo( hi );
                }
                else {
                    // was not docked
                    if( isShown ) {
                        pDockWindow->Appear();
                    }
                    pDockWindow->ClearDockingInfo();
                }
            }
            else {
                // could not find window
            }
        }

        DockHostList lockedHosts;

        // read in hosts
        int hostCount = 0;
        stream.Read( &hostCount, sizeof( hostCount ) );
        for( i=0; i<hostCount; i++ ) {
            // name
            wxString name = ReadString( stream );

            // area size
            int areaSize = 0;
            stream.Read( &areaSize, sizeof( areaSize ) );

            // panel area
            int panelArea = 0;
            stream.Read( &panelArea, sizeof( panelArea ) );

            // find host and apply
            wxDockHost * pDockHost = findDockHost( name );
            if( pDockHost ) {
                // areas
                pDockHost->SetAreaSize( areaSize );
                pDockHost->SetPanelArea( panelArea );
                pDockHost->LockPanelValue( true );
                lockedHosts.Append( pDockHost );

                // docked windows
                int winCount = 0;
                stream.Read( &winCount, sizeof( winCount ) );
                for( int i=0; i<winCount; i++ ) {
                    // name
                    wxString name = ReadString( stream );

                    // find window and dock
                    wxDockWindowBase * pDockWindow = findDockWindow( name );
                    if( pDockWindow ) {
                        HostInfo hi;
                        hi = pDockHost;
                        DockWindow( pDockWindow, hi );
                    }
                    else {
                        // could not find window
                    }
                }
            }
            else {
                // could not find host
            }
        }
        UpdateAllHosts( true );

        // unlock all panels
        for( DockPanelList::Node *pnode = lockedPanels.GetFirst(); pnode; pnode = pnode->GetNext() ) {
            wxDockPanel * pDockPanel = pnode->GetData();
            wxASSERT( pDockPanel );
            pDockPanel->LockAreaValue( false );
        }

        // unlock all hosts
        for( DockHostList::Node *hnode = lockedHosts.GetFirst(); hnode; hnode = hnode->GetNext() ) {
            wxDockHost * pDockHost = hnode->GetData();
            wxASSERT( pDockHost );
            pDockHost->LockPanelValue( false );
        }

    }   // end <layout>

    return true;
}
