///////////////////////////////////////////////////////////////////////////////
// Name:        gdb_tipwindow.h
// Purpose:     GDBTipWindow is a window like the one typically used for
//              showing the tooltips
// Author:      Vadim Zeitlin (wxTipWindow)
// Modified by: Aug 3, 2006 Yiannis Mandravellos:
//                  improved for use in GDB tooltip evaluation (Code::Blocks IDE)
// Created:     10.09.00
// RCS-ID:      Id: tipwin.h,v 1.14 2004/05/23 20:50:25 JS Exp
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDB_TIPWINDOW_H_
#define _WX_GDB_TIPWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "tipwin.h"
#endif

#if wxUSE_TIPWINDOW

#if wxUSE_POPUPWIN
    #include "wx/popupwin.h"

    #define wxTipWindowBase wxPopupTransientWindow
#else
    #include "wx/frame.h"

    #define wxTipWindowBase wxFrame
#endif
#include "wx/arrstr.h"

class GDBTipWindowView;

// ----------------------------------------------------------------------------
// GDBTipWindow
// ----------------------------------------------------------------------------

class GDBTipWindow : public wxTipWindowBase
{
public:
    // the mandatory ctor parameters are: the parent window and the text to
    // show
    //
    // optionally you may also specify the length at which the lines are going
    // to be broken in rows (100 pixels by default)
    //
    // windowPtr and rectBound are just passed to SetTipWindowPtr() and
    // SetBoundingRect() - see below
    GDBTipWindow(wxWindow *parent,
                const wxString& symbol,
                const wxString& typ,
                const wxString& addr,
                const wxString& contents,
                wxCoord maxLength = 240,
                GDBTipWindow** windowPtr = NULL,
                wxRect *rectBound = NULL);

    virtual ~GDBTipWindow();

    // If windowPtr is not NULL the given address will be NULLed when the
    // window has closed
    void SetTipWindowPtr(GDBTipWindow** windowPtr) { m_windowPtr = windowPtr; }

    // If rectBound is not NULL, the window will disappear automatically when
    // the mouse leave the specified rect: note that rectBound should be in the
    // screen coordinates!
    void SetBoundingRect(const wxRect& rectBound);

    // Hide and destroy the window
    void Close();

protected:
    // called by GDBTipWindowView only
    bool CheckMouseInBounds(const wxPoint& pos);

    // event handlers
    void OnMouseClick(wxMouseEvent& event);

#if !wxUSE_POPUPWIN
    void OnActivate(wxActivateEvent& event);
    void OnKillFocus(wxFocusEvent& event);
#else // wxUSE_POPUPWIN
    virtual void OnDismiss();
#endif // wxUSE_POPUPWIN/!wxUSE_POPUPWIN

private:
    wxArrayString m_textLines;
    wxArrayString m_symbolLines;
    wxCoord m_heightLine;

    wxString m_symbol;
    wxString m_type;
    wxString m_address;
    wxString m_contents;

    GDBTipWindowView *m_view;

    GDBTipWindow** m_windowPtr;
    wxRect m_rectBound;

    DECLARE_EVENT_TABLE()

    friend class GDBTipWindowView;

    DECLARE_NO_COPY_CLASS(GDBTipWindow)
};

#endif // wxUSE_TIPWINDOW

#endif // _WX_GDB_TIPWINDOW_H_
