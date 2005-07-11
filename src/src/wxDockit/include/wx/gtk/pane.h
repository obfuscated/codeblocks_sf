/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/pane.h
// Purpose:     wxPane class
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PANE_H_
#define _WX_PANE_H_

#include <wx/dockit_defs.h>

// ----------------------------------------------------------------------------
// Pane control
// ----------------------------------------------------------------------------

class WXDOCKIT_DECLSPEC wxPane : public wxPaneBase
{
public:
    // Default constructor
    wxPane() {
        Init();
    }
    
    // Normal constructor
    wxPane( wxWindow *parent, wxWindowID id = -1, const wxString& name = wxT("pane"), const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL ) {
        Init();
        Create( parent, id, name, pos, size, style );
    }

    virtual wxWindow * SetClient( wxWindow * pClient, bool removeBorder = false );
    
private:
    DECLARE_DYNAMIC_CLASS(wxPane)
};


#endif
    // _WX_PANE_H_
