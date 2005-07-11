/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/pane_gtk.cpp
// Purpose:     wxPane implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
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
    // XXX: do we need to do this?
    return wxPaneBase::SetClient( pClient, removeBorder );
}
