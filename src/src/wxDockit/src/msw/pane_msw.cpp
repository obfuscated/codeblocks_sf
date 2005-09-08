/////////////////////////////////////////////////////////////////////////////
// Name:        msw/pane_msw.cpp
// Purpose:     wxPane implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     28/12/03
// RCS-ID:
// Copyright:
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/pane.h>

// ----------------------------------------------------------------------------
// wxPane constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPane, wxPaneBase)

// ----------------------------------------------------------------------------
// wxPane implementation
// ----------------------------------------------------------------------------

wxWindow * wxPane::SetClient( wxWindow * pClient, bool removeBorder ) {
    // set the pane's client window
    if( pClient ) {
        HWND wndClient = (HWND)pClient->GetHandle();

        if( removeBorder ) {
            // remove the standard edge from the control
            DWORD exStyle = ::GetWindowLong( wndClient, GWL_EXSTYLE );
            ::SetWindowLong( wndClient, GWL_EXSTYLE, (exStyle & ~WS_EX_CLIENTEDGE) );
        }
    }

    return wxPaneBase::SetClient( pClient, removeBorder );
}
