/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxbarholder.h
// Purpose:     wxBarHolder class
// Author:      Mark McCormack
// Modified by:
// Created:     25/05/05
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BARHOLDER_H_
#define _WX_BARHOLDER_H_

// wxWindows
#include <wx/dockit_defs.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/laywin.h>

class wxGripWindow;
class wxSlideBar;

#define wxBF_EXPAND_X   0x01
#define wxBF_EXPAND_Y   0x02
#define wxBF_DEFAULT    0x00

// ----------------------------------------------------------------------------
// wxBarHolder control
// ----------------------------------------------------------------------------

class WXDOCKIT_DECLSPEC wxBarHolder : public wxWindow
{
public:
    // Default constructor
    wxBarHolder() {
        Init();
    }
    
    // Normal constructor
    wxBarHolder::wxBarHolder( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("barholder") ) {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    void Init();

    bool Create( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("barholder") );

    virtual ~wxBarHolder();

    // interface
    void AddWindow( wxWindow * pWindow, const wxString & label = "", unsigned int flags = wxBF_DEFAULT );
    void SetSlideBar( wxSlideBar * pSlideBar );
    void ShowGripper( bool state );
    void SetHeightOverride( int override );
    void SetWidthOverride( int override );
    void SetFlags( unsigned int flags );
    unsigned int GetFlags();
	wxString GetLabel();

    virtual wxSize DoGetBestSize() const;
    void UpdateSize();

    // event handlers
	void OnErase( wxEraseEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnGripBegin( wxMouseEvent &event );
    void OnGripEnd( wxMouseEvent &event );
    void OnGripMotion( wxMouseEvent &event );
	
private:
    wxSize getNonClientSize() const;
    wxSize getClientBestSize( wxWindow * pClient ) const;

private:
    wxSlideBar * pSlideBar_;
    wxWindow * pOurBar_;
    wxGripWindow * pGripWindow_;
	wxString label_;

    int margin_;
    int heightOverride_;
    int widthOverride_;
    bool showGripper_;
    unsigned int flags_;
    wxSize originalSize_;
    bool moving_;

private:
    DECLARE_DYNAMIC_CLASS( wxBarHolder )
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_BARHOLDER_H_
