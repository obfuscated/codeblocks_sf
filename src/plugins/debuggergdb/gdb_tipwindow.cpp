///////////////////////////////////////////////////////////////////////////////
// Name:        gdb_tipwindow.cpp
// Purpose:     implementation of GDBTipWindow
// Author:      Vadim Zeitlin (wxTipWindow)
// Modified by: Aug 3, 2006 Yiannis Mandravellos:
//                  improved for use in GDB tooltip evaluation (Code::Blocks IDE)
// Created:     10.09.00
// RCS-ID:      Id: tipwin.cpp,v 1.28.2.1 2005/10/18 14:33:33 MW Exp
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <sdk.h> // C::B sdk PCH

#include <algorithm> // std::max

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "tipwin.h"
#endif

// For compilers that support precompilatixon, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif // WX_PRECOMP
#ifdef __WXGTK__
    #include <gtk/gtk.h>
#endif
#include "gdb_tipwindow.h"

#if wxUSE_TIPWINDOW

#include "wx/timer.h"
#include "wx/settings.h"
#include "wx/intl.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const wxCoord TEXT_MARGIN_X = 3;
static const wxCoord TEXT_MARGIN_Y = 3;

// ----------------------------------------------------------------------------
// GDBTipWindowView
// ----------------------------------------------------------------------------

// Viewer window to put in the frame
class GDBTipWindowView : public wxWindow
{
public:
    GDBTipWindowView(wxWindow *parent);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

#if !wxUSE_POPUPWIN
    void OnKillFocus(wxFocusEvent& event);
#endif // wxUSE_POPUPWIN

    // calculate the client rect we need to display the text
    void Adjust(const wxString& symbol, const wxString& typ, const wxString& addr, const wxString& contents, wxCoord maxLength);

private:
    wxSize GetTextSize(wxArrayString& array, const wxString& text, wxCoord maxLength, int indentationAfterFirstLine = 0);
    void PrintArray(wxDC& dc, wxPoint& pt, const wxArrayString& array);
    wxString AdjustContents(const wxString& contents);

    GDBTipWindow* m_parent;
    wxCoord m_headerHeight;

    wxString m_symbol;
    wxString m_type;
    wxString m_address;
    wxString m_contents;

#if !wxUSE_POPUPWIN
    long m_creationTime;
#endif // !wxUSE_POPUPWIN

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(GDBTipWindowView)
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GDBTipWindow, wxTipWindowBase)
    EVT_LEFT_DOWN(GDBTipWindow::OnMouseClick)
    EVT_RIGHT_DOWN(GDBTipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN(GDBTipWindow::OnMouseClick)

#if !wxUSE_POPUPWIN
    EVT_KILL_FOCUS(GDBTipWindow::OnKillFocus)
    EVT_ACTIVATE(GDBTipWindow::OnActivate)
#endif // !wxUSE_POPUPWIN
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GDBTipWindowView, wxWindow)
    EVT_PAINT(GDBTipWindowView::OnPaint)

    EVT_LEFT_DOWN(GDBTipWindowView::OnMouseClick)
    EVT_RIGHT_DOWN(GDBTipWindowView::OnMouseClick)
    EVT_MIDDLE_DOWN(GDBTipWindowView::OnMouseClick)

    EVT_MOTION(GDBTipWindowView::OnMouseMove)

#if !wxUSE_POPUPWIN
    EVT_KILL_FOCUS(GDBTipWindowView::OnKillFocus)
#endif // !wxUSE_POPUPWIN
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// GDBTipWindow
// ----------------------------------------------------------------------------

GDBTipWindow::GDBTipWindow(wxWindow *parent,
                         const wxString& symbol,
                         const wxString& typ,
                         const wxString& addr,
                         const wxString& contents,
                         wxCoord maxLength,
                         GDBTipWindow** windowPtr,
                         wxRect *rectBounds)
#if wxUSE_POPUPWIN
           : wxPopupTransientWindow(parent),
#else
           : wxFrame(parent, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER | wxFRAME_NO_TASKBAR ),
#endif
            m_symbol(symbol),
            m_type(typ),
            m_address(addr),
            m_contents(contents)
{
//    int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), 8);
    SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
//    SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT));

    SetTipWindowPtr(windowPtr);
    if ( rectBounds )
    {
        SetBoundingRect(*rectBounds);
    }

    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    // set size, position and show it
    m_view = new GDBTipWindowView(this);
    m_view->Adjust(symbol, typ, addr, contents, maxLength);
    m_view->SetFocus();

    int x, y;
    wxGetMousePosition(&x, &y);

    // we want to show the tip below the mouse, not over it
    //
    // NB: the reason we use "/ 2" here is that we don't know where the current
    //     cursors hot spot is... it would be nice if we could find this out
    //     though
    y += wxSystemSettings::GetMetric(wxSYS_CURSOR_Y) / 2;

#if wxUSE_POPUPWIN
    Position(wxPoint(x, y), wxSize(0,0));
    Popup(m_view);
    #ifdef __WXGTK__
        if (!GTK_WIDGET_HAS_GRAB(m_widget))
            gtk_grab_add( m_widget );
    #endif
#else
    Move(x, y);
    Show(true);
#endif
}

GDBTipWindow::~GDBTipWindow()
{
    if ( m_windowPtr )
    {
        *m_windowPtr = NULL;
    }
    #ifdef wxUSE_POPUPWIN
        #ifdef __WXGTK__
            if (GTK_WIDGET_HAS_GRAB(m_widget))
                gtk_grab_remove( m_widget );
        #endif
    #endif
}

void GDBTipWindow::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    Close();
}

#if wxUSE_POPUPWIN

void GDBTipWindow::OnDismiss()
{
    Close();
}

#else // !wxUSE_POPUPWIN

void GDBTipWindow::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
        Close();
}

void GDBTipWindow::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // Under Windows at least, we will get this immediately
    // because when the view window is focussed, the
    // tip window goes out of focus.
#ifdef __WXGTK__
    Close();
#endif
}

#endif // wxUSE_POPUPWIN // !wxUSE_POPUPWIN

void GDBTipWindow::SetBoundingRect(const wxRect& rectBound)
{
    m_rectBound = rectBound;
}

void GDBTipWindow::Close()
{
    if ( m_windowPtr )
    {
        *m_windowPtr = NULL;
        m_windowPtr = NULL;
    }

#if wxUSE_POPUPWIN
    Show(false);
    #ifdef __WXGTK__
        if (GTK_WIDGET_HAS_GRAB(m_widget))
            gtk_grab_remove( m_widget );
    #endif
    Destroy();
#else
    wxFrame::Close();
#endif
}

// ----------------------------------------------------------------------------
// GDBTipWindowView
// ----------------------------------------------------------------------------

GDBTipWindowView::GDBTipWindowView(wxWindow *parent)
               : wxWindow(parent, wxID_ANY,
                          wxDefaultPosition, wxDefaultSize,
                          wxNO_BORDER)
{
    // set colours
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

#if !wxUSE_POPUPWIN
    m_creationTime = wxGetLocalTime();
#endif // !wxUSE_POPUPWIN

    m_parent = (GDBTipWindow*)parent;
    m_headerHeight = 0;
}

wxSize GDBTipWindowView::GetTextSize(wxArrayString& array, const wxString& text, wxCoord maxLength, int indentationAfterFirstLine)
{
    wxClientDC dc(this);
    dc.SetFont(m_parent->GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width,
            widthMax = 0;
    m_parent->m_heightLine = 0;

    // indentation string
    wxString indent = wxString(_T(' '), indentationAfterFirstLine);

    bool breakLine = false;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == _T('\n') || *p == _T('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_parent->m_heightLine )
                m_parent->m_heightLine = height;

            array.Add(current);

            if ( !*p )
            {
                // end of text
                break;
            }

            current = indent;
            breakLine = false;
        }
        else if ( breakLine && (*p == _T(' ') || *p == _T('\t')) )
        {
            // word boundary - break the line here
            array.Add(current);
            current = indent;
            breakLine = false;
        }
        else // line goes on
        {
            current += *p;
            dc.GetTextExtent(current, &width, &height);
            if ( width > maxLength )
                breakLine = true;

            if ( width > widthMax )
                widthMax = width;

            if ( height > m_parent->m_heightLine )
                m_parent->m_heightLine = height;
        }
    }

    // this can happen if no newline and small enough not to break in lines
    if (array.GetCount() == 0)
        array.Add(text);

    // this can happen if the text was empty; set a default height
    if (m_parent->m_heightLine == 0)
    {
        dc.GetTextExtent(_T("Wq"), &width, &height);
        m_parent->m_heightLine = height;
    }

    wxSize ret;
    ret.x = widthMax;
    ret.y = array.GetCount() * m_parent->m_heightLine;
    //DBGLOG(_T("text='%s', y=%d, m_parent->m_heightLine=%d"), text.c_str(), ret.y, m_parent->m_heightLine);
    return ret;
}

wxString GDBTipWindowView::AdjustContents(const wxString& contents)
{
    // format contents string
    //
    // it comes in the form:
    // {i=1, f=5.22, b=true, st={u="o"}}
    //
    // we 'll convert it to:
    // {
    //     i=1,
    //     f=5.22,
    //     b=true,
    //     st={
    //         u="o"
    //     }
    // }

    wxString tmp = contents;
    if (tmp.GetChar(0) == _T('{') && tmp.Last() == _T('}'))
    {
        tmp.Remove(0, 1);
        tmp.RemoveLast();
    }
    wxString ret;
    wxString indent;
    int template_depth = 0;
    for (const wxChar* p = tmp.c_str(); p && *p; ++p)
    {
        // indent
        if (*p == _T('{'))
        {
            ret << *p;
            indent << _T(' ') << _T(' ');
            ret << _T('\n') << indent;
        }

        // unindent
        else if (*p == _T('}'))
        {
            ret << _T('\n');
            indent.RemoveLast();
            indent.RemoveLast();
            ret << indent << *p;
        }

        // template/function depth add
        else if (*p == _T('<') || *p == _T('('))
        {
            ret << *p;
            ++template_depth;
        }

        // template/function depth remove
        else if (*p == _T('>') || *p == _T(')'))
        {
            ret << *p;
            --template_depth;
        }

        // commas = newlines (except for templates)
        else if (*p == _T(',') && template_depth <= 0)
        {
            ret << *p << _T('\n') << indent;
            // skip whitespace so indentation doesn't break
            ++p;
            while (*p == _T(' ') || *p == _T('\t'))
                ++p;
            // don't ruin the for-loop's book-keeping ;)
            --p;
        }

        // append
        else
        {
            ret << *p;
        }
    }
    return ret;
}

void GDBTipWindowView::Adjust(const wxString& symbol, const wxString& typ, const wxString& addr, const wxString& contents, wxCoord maxLength)
{
    wxString text;
    wxString tmp;

    tmp << _("Symbol  : ") << symbol << _T(" (") << typ << _T(')');
    text = tmp;
    wxSize size0 = GetTextSize(m_parent->m_symbolLines, text, maxLength, (_("Symbol  :") + symbol + _T(" ")).Length());

    text.Clear();
    text << _("Address : ") << addr;
    wxSize size1 = GetTextSize(m_parent->m_symbolLines, text, maxLength, (_("Address : ") + addr).Length());

    size1.x = std::max(size0.x, size1.x);

    text = AdjustContents(contents);
    wxSize size2 = GetTextSize(m_parent->m_textLines, text, std::max(maxLength, size1.x));

    m_headerHeight = TEXT_MARGIN_Y + (m_parent->m_heightLine * m_parent->m_symbolLines.GetCount());

    // take into account the border size and the margins
    wxSize size(std::max(size1.x, size2.x), m_headerHeight + size2.y);
    size.x += 2*(TEXT_MARGIN_X + 1);
    size.y += 2*(TEXT_MARGIN_Y + 1);

    m_parent->SetClientSize(size);
    SetSize(0, 0, size.x, size.y);

    m_symbol = symbol;
    m_type = typ;
    m_address = addr;
    m_contents = contents;
}

void GDBTipWindowView::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();

    // first fill the header's background
    dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), wxSOLID));
    dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID) );
    rect.width = size.x;
    rect.height = m_headerHeight;
    dc.DrawRectangle(rect);

    // now fill the main's background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));
    dc.SetPen( wxPen(GetForegroundColour(), 1, wxSOLID) );
    rect.y = m_headerHeight - 1;
    rect.height = size.y - (m_headerHeight - 1);
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    dc.SetTextBackground(GetBackgroundColour());
    dc.SetTextForeground(GetForegroundColour());
    dc.SetFont(m_parent->GetFont());

    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;

    PrintArray(dc, pt, m_parent->m_symbolLines);
    pt.y += 2;
    PrintArray(dc, pt, m_parent->m_textLines);
}

void GDBTipWindowView::PrintArray(wxDC& dc, wxPoint& pt, const wxArrayString& array)
{
    size_t count = array.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(array[n], pt);

        pt.y += m_parent->m_heightLine;
    }
}

void GDBTipWindowView::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    m_parent->Close();
}

void GDBTipWindowView::OnMouseMove(wxMouseEvent& event)
{
    const wxRect& rectBound = m_parent->m_rectBound;

    if ( rectBound.width &&
#if wxCHECK_VERSION(2, 8, 0)
		!rectBound.Contains(ClientToScreen(event.GetPosition())) )
#else
		 !rectBound.Inside(ClientToScreen(event.GetPosition())) )
#endif
    {
        // mouse left the bounding rect, disappear
        m_parent->Close();
    }
    else
    {
        event.Skip();
    }
}

#if !wxUSE_POPUPWIN
void GDBTipWindowView::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // Workaround the kill focus event happening just after creation in wxGTK
    if (wxGetLocalTime() > m_creationTime + 1)
        m_parent->Close();
}
#endif // !wxUSE_POPUPWIN

#endif // wxUSE_TIPWINDOW
