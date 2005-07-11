///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/toolbutton.h
// Purpose:     wxToolButton class
// Author:      Mark McCormack
// Modified by:
// Created:     19/05/04
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBUTTON_H_
#define _WX_TOOLBUTTON_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/toolbutton.h>

// ----------------------------------------------------------------------------
// wxToolButton
// ----------------------------------------------------------------------------

class wxToolButton : public wxToolButtonBase {
    DECLARE_EVENT_TABLE()

public:
    // Default constructor
    wxToolButton() { 
        Init();
    }

    void Init();

    // Normal constructor
    wxToolButton( wxWindow *parent,
                  wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxBORDER_NONE,
                  const wxString& name = wxButtonNameStr ) {
        Init();
        Create( parent, id, pos, size, style, name );
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxBORDER_NONE,
                 const wxString& name = wxButtonNameStr );

    virtual ~wxToolButton();

    // event handlers
    void OnErase( wxEraseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnLeftDown( wxMouseEvent& event );
    void OnLeftUp( wxMouseEvent& event );
    void OnEnter( wxMouseEvent& event );
    void OnLeave( wxMouseEvent& event );

private:
    bool hovered_;
    bool pressed_;

private:
    DECLARE_DYNAMIC_CLASS( wxToolButton )
};

#endif
    // _WX_TOOLBUTTON_H_
