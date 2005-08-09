/////////////////////////////////////////////////////////////////////////////
// Name:        custctrl.cpp
// Purpose:     wxCustomControls (non-wxWindow based)
// Author:      Jaakko Salli
// Modified by:
// Created:     Oct-24-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "custctrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/dynarray.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/frame.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/choice.h"
    #include "wx/textctrl.h"
    #include "wx/scrolwin.h"
    #include "wx/combobox.h"
#endif

// This define is necessary to prevent macro clearing
#define __wxCCM_SOURCE_FILE__

#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
# undef wxUSE_UXTHEME
#endif

//#include <wx/propertygrid/custctrl.h>
#define __wxPG_SOURCE_FILE__
#include <wx/propgrid/propgrid.h>

#include "wx/renderer.h"

#include "wx/clipbrd.h"

#define __INTENSE_DEBUGGING__       0
#define __PAINT_DEBUGGING__         0
#define __MOUSE_DEBUGGING__         0

#define DRAW_CTRL_IN_DATA_FUNC(MANAGER) \
            ctrl->Draw();


//
// Here are some extra platform dependent defines.
//

#if defined(__WXMSW__)
    // tested

    #define wxPG_TEXTCTRL_DOUBLE_CLICK_MODE     2 // 0 = as left click, 1 = select word (GTK), 2 = select word and following spaces (MSW)

    #define wxPG_DROPDOWN_BUTTON_MAX_WIDTH      17

#elif defined(__WXGTK__)
    // tested

    #define wxPG_TEXTCTRL_DOUBLE_CLICK_MODE     1 // 0 = as left click, 1 = select word (GTK), 2 = select word and following spaces (MSW)

    #define wxPG_DROPDOWN_BUTTON_MAX_WIDTH      19

#elif defined(__WXMAC__)
    // *not* tested

    #define wxPG_TEXTCTRL_DOUBLE_CLICK_MODE     1 // 0 = as left click, 1 = select word (GTK), 2 = select word and following spaces (MSW)

    #define wxPG_DROPDOWN_BUTTON_MAX_WIDTH      17

#else
    // defaults

    #define wxPG_TEXTCTRL_DOUBLE_CLICK_MODE     1 // 0 = as left click, 1 = select word (GTK), 2 = select word and following spaces (MSW)

    #define wxPG_DROPDOWN_BUTTON_MAX_WIDTH      21

#endif

// -----------------------------------------------------------------------
// Extra Native Rendering
// -----------------------------------------------------------------------

// Only for wxWidgets 2.5.4 and earlier
#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 5 ) || wxPG_USE_CUSTOM_CONTROLS

#define wxCC_DROPDOWN_BUTTON_WIDTH  17 // for font with height of one below
#define wxCC_DROPDOWN_BUTTON_WIDTH_WITH_FONT_HEIGHT 13

#ifndef __WX_CUSTCTRL_H__
# define wxCONTROL_POPUP_ARROW       wxCONTROL_CHECKED
# if defined(__WXMSW__) && wxUSE_UXTHEME
#  include "wx/msw/uxtheme.h"
# endif
#endif

//
// TODO: After its sure that both MSW and GTK have native dropdown button
//   rendering in wx lib, change code here to support it (retain old code
//   for < 2.5.4 support).
//

#if defined(__WXGTK__)
//#if defined(__WXMSW__)

// wxGTK native button.
//# if defined(__WXGTK20__)

#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

void wxRendererNative_DrawButton ( wxWindow* win, wxDC& dc, const wxRect& rect,
                                   int flags )
{

    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;
    if (s_button == NULL)
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_button_new();
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    // Device context must inherit from wxWindowDC
    // (so it must be wxClientDC, wxMemoryDC or wxPaintDC).
    wxWindowDC* wdc = wxDynamicCast(&dc,wxWindowDC);
    wxASSERT ( wdc );

    GtkStateType state = GTK_STATE_NORMAL;
    GtkShadowType shadow = GTK_SHADOW_OUT;

    if ( flags & wxCONTROL_PRESSED )
        shadow = GTK_SHADOW_IN;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    
    gtk_paint_box
    (
        s_button->style,
        //GTK_PIZZA(wdc->m_window)->bin_window,
        wdc->m_window,
        state,
        shadow,
        NULL,
        s_button,
        "button",
        //dc.XLOG2DEV(rect.x) -1, rect.y -1, rect.width +2, rect.height +2
        //rect.x -1, rect.y -1, rect.width +2, rect.height +2
        rect.x, rect.y, rect.width, rect.height
    );

    if ( flags & wxCONTROL_POPUP_ARROW )
    {
        // Arrow on button.

        int arr_wid = rect.width/2;
        int arr_hei = rect.height/2;
        if ( arr_wid & 1 ) arr_wid++;
        if ( arr_hei & 1 ) arr_hei++;

        gtk_paint_arrow
        (
            s_button->style,
            //GTK_PIZZA(wdc->m_window)->bin_window,
            wdc->m_window,
            state,
            shadow,
            NULL,
            s_button,
            "arrow",
            GTK_ARROW_DOWN,
            TRUE,
            rect.x + (rect.width/2-arr_wid/2) + 1,
            rect.y + (rect.height/2-arr_hei/2) + 1,
            arr_wid,
            arr_hei
        );
    }

    /*
    GtkWidget* widget = wnd->GetHandle();

    GtkStateType state = GTK_STATE_NORMAL;

    if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;

    GdkRectangle clip_rect;

    wxRect r2 = wnd->GetClientRect();

    clip_rect.x = r2.x;
    clip_rect.y = r2.y;
    clip_rect.width = r2.width;
    clip_rect.height = r2.height;

    gtk_paint_box (widget->style, widget->window,
			 state, GTK_SHADOW_IN,
			 &clip_rect, widget, "buttondefault",
			 r1.x, r1.y, r1.width, r1.height);
    */
}

/*# else // defined(__WXGTK20__)

void wxRendererNative_DrawButton ( wxWindow*, wxDC& dc, const wxRect& r1,
                                   int flags )
{
    wxColour face_colour = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE );

    if ( flags & wxCONTROL_PRESSED )
    {
#  define __LIGHTER__ (-18)
            face_colour.Set ( face_colour.Red()>(-__LIGHTER__)?face_colour.Red()+__LIGHTER__:0,
                          face_colour.Green()>(-__LIGHTER__)?face_colour.Green()+__LIGHTER__:0,
                          face_colour.Blue()>(-__LIGHTER__)?face_colour.Blue()+__LIGHTER__:0
                        );
#  undef __LIGHTER__
    }
    else if ( flags & wxCONTROL_CURRENT )
    {
#  define __LIGHTER__ 18
            face_colour.Set ( face_colour.Red()<(255-__LIGHTER__)?face_colour.Red()+__LIGHTER__:255,
                          face_colour.Green()<(255-__LIGHTER__)?face_colour.Green()+__LIGHTER__:255,
                          face_colour.Blue()<(255-__LIGHTER__)?face_colour.Blue()+__LIGHTER__:255
                        );
#  undef __LIGHTER__
    }
   //     face_colour = wxSystemSettings::GetColour ( wxSYS_COLOUR_3DLIGHT );

    dc.SetBrush ( face_colour );

    dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_3DDKSHADOW ) );

    dc.DrawRectangle ( r1 );
    
    wxRect r2(r1);
    r2.Deflate ( 1 );

    dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNSHADOW ) );

    if ( !(flags & wxCONTROL_PRESSED) )
    {
        dc.DrawRectangle ( r2 );

        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNHIGHLIGHT ) );
        dc.DrawLine ( r2.x, r2.y, r2.x, r2.y + r2.height - 1 );
        dc.DrawLine ( r2.x, r2.y, r2.x + r2.width, r2.y );
    }
    else
    {
        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNSHADOW ) );
        dc.DrawLine ( r2.x, r2.y, r2.x, r2.y + r2.height );
        dc.DrawLine ( r2.x, r2.y, r2.x + r2.width, r2.y );
    }

    if ( flags & wxCONTROL_POPUP_ARROW )
    {
        // Arrow on button.

        wxBitmap* bmp = gs_bmp_gtk_arrow.m_bmp;
        if ( !bmp )
            bmp = gs_bmp_gtk_arrow.SetXpm((const char**)gs_xpm_gtk_arrow);

        wxASSERT ( bmp );
        dc.DrawBitmap(*bmp,
            r1.x + (r1.width/2-bmp->GetWidth()/2),
            r1.y + (r1.height/2-bmp->GetHeight()/2) + 1,
            TRUE);

    }
}

# endif // !defined(__WXGTK20__)*/

#else

#if wxUSE_UXTHEME
    #include <commctrl.h>
    //#include <tmschema.h>

    // Since tmschema.h is in Win32 Platform SDK,
    // we need to define some values from it here.
    #ifndef BP_PUSHBUTTON

    # define BP_PUSHBUTTON      1

    # define PBS_NORMAL         1
    # define PBS_HOT            2
    # define PBS_PRESSED        3
    # define PBS_DISABLED       4
    # define PBS_DEFAULTED      5

    # define CP_DROPDOWNBUTTON  1

    # define CBXS_NORMAL        1
    # define CBXS_HOT           2
    # define CBXS_PRESSED       3
    # define CBXS_DISABLED      4

    #endif

#endif

static wxPoint dropdown_arrow_points[3];

// wxMSW native button (default for platforms without their own)
#if wxUSE_UXTHEME
void wxRendererNative_DrawButton (wxWindow* win, wxDC& dc, const wxRect& r1,
                                  int flags )
#else
void wxRendererNative_DrawButton (wxWindow*, wxDC& dc, const wxRect& r1,
                                  int flags )
#endif
{

#if wxUSE_UXTHEME

    const wchar_t *classes = L"ComboBox";

    if ( !(flags & wxCONTROL_POPUP_ARROW) )
        classes = L"Button";

    wxUxThemeHandle themeHandle(win, classes);
#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 4 )
    WXHTHEME hTheme = themeHandle;
#else
    HTHEME hTheme = themeHandle;
#endif

    //wxLogDebug(wxT("%i,%i,%i,%i,%i"),PBS_PRESSED,PBS_HOT,PBS_DEFAULTED,
    //    PBS_DISABLED,BP_PUSHBUTTON);

    if ( hTheme )
    {
        int iState;

        int col_ind;

        int component_ind;

        tagRECT trect;
        trect.left = r1.x;
        trect.top = r1.y;
        trect.right = r1.x + r1.width - 1;
        trect.bottom = r1.y + r1.height - 1;

        if ( !(flags & wxCONTROL_POPUP_ARROW) )
        {
            // Normal XP button.

            //col_ind = 1606; // TMT_WINDOW
            col_ind = wxSYS_COLOUR_BTNFACE;

            component_ind = BP_PUSHBUTTON;

            trect.left -= 1;
            trect.top -= 1;
            trect.right += 2;
            trect.bottom += 2;

            iState = PBS_NORMAL;

            if ( flags & wxCONTROL_PRESSED )
                iState = PBS_PRESSED;
            else if ( flags & wxCONTROL_CURRENT )
                iState = PBS_HOT;
            else if ( flags & wxCONTROL_FOCUSED )
                iState = PBS_DEFAULTED;
            else if ( flags & wxCONTROL_DISABLED )
                iState = PBS_DISABLED;

        }
        else
        {
            // Dropdown XP button.

            //col_ind = 1623; // TMT_LIGHT3D
            //col_ind = TMT_LIGHT3D;
            //col_ind = TMT_BUTTONALTERNATEFACE;
            trect.bottom += 1;
            col_ind = wxSYS_COLOUR_WINDOW;

            component_ind = CP_DROPDOWNBUTTON;

            iState = CBXS_NORMAL;

            if ( flags & wxCONTROL_PRESSED )
                iState = CBXS_PRESSED;
            else if ( flags & wxCONTROL_CURRENT )
                iState = CBXS_HOT;
            else if ( flags & wxCONTROL_DISABLED )
                iState = CBXS_DISABLED;

        }

        //wxColour bg_col(wxUxThemeEngine::Get()->GetThemeSysColor(hTheme,col_ind));
        //wxColour bg_col(*wxWHITE);
        wxColour bg_col(::wxSystemSettings::GetColour((wxSystemColour)col_ind));

        dc.SetBrush ( bg_col );
        dc.SetPen ( bg_col );
        dc.DrawRectangle ( r1 );

        wxUxThemeEngine::Get()->DrawThemeBackground( hTheme, dc.GetHDC(),
                                                     component_ind, iState,
                                                     &trect, NULL);

        return;
    }

#endif

    //
    // Draw button in traditional style.
    //

    dc.SetBrush ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE ) );

    if ( !(flags & wxCONTROL_PRESSED) )
    {

        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_3DDKSHADOW ) );
        dc.DrawRectangle ( r1 );
    
        dc.SetBrush ( *wxTRANSPARENT_BRUSH );

        wxRect r2(r1);
        r2.Deflate ( 1 );

        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNSHADOW ) );
        dc.DrawRectangle ( r2 );

        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNHIGHLIGHT ) );
        dc.DrawLine ( r2.x, r2.y, r2.x, r2.y + r2.height-1 );
        dc.DrawLine ( r2.x, r2.y, r2.x + r2.width - 1, r2.y );

        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_3DLIGHT ) );
        dc.DrawLine ( r1.x, r1.y, r1.x, r1.y + r1.height-1 );
        dc.DrawLine ( r1.x, r1.y, r1.x + r1.width - 1, r1.y );
    }
    else
    {
        dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNSHADOW ) );
        dc.DrawRectangle ( r1 );
    }

    if ( flags & wxCONTROL_POPUP_ARROW )
    {
        wxColour buttextcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNTEXT );

        // arrow on button
        dc.SetBrush ( buttextcol );
        dc.SetPen ( buttextcol );

        int rect_mid = r1.width / 2;
        int arw_wid = r1.width/5;
        int arw_top_y = (r1.height/2) - (arw_wid/2);

        dropdown_arrow_points[0].x = rect_mid - arw_wid;
        dropdown_arrow_points[0].y = arw_top_y;
        dropdown_arrow_points[1].x = rect_mid + arw_wid; //rect.width-(rect.width/3);
        dropdown_arrow_points[1].y = arw_top_y;
        dropdown_arrow_points[2].x = rect_mid;
        dropdown_arrow_points[2].y = arw_top_y + arw_wid;

        dc.DrawPolygon ( 3, dropdown_arrow_points, r1.x, r1.y );
    }

}

#endif

#endif // if wxWidgets < 2.5.5 or wxPG_USE_CUSTOM_CONTROLS


#if wxPG_USE_GENERIC_TEXTCTRL

BEGIN_EVENT_TABLE(wxGenericTextCtrl, wxGTextCtrlBase)
  EVT_MOTION(wxGenericTextCtrl::OnMouseEvent)
  EVT_LEFT_DOWN(wxGenericTextCtrl::OnMouseEvent)
  EVT_LEFT_UP(wxGenericTextCtrl::OnMouseEvent)
  //EVT_RIGHT_UP(wxPropertyGrid::OnMouseEvent)
  EVT_LEFT_DCLICK(wxGenericTextCtrl::OnMouseEvent)
  EVT_PAINT(wxGenericTextCtrl::OnPaint)
  //EVT_SIZE(wxPropertyGrid::OnResize)
  EVT_KEY_DOWN(wxGenericTextCtrl::OnKeyEvent)
  EVT_CHAR(wxGenericTextCtrl::OnKeyEvent)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxGenericTextCtrl, wxGTextCtrlBase)

void wxGenericTextCtrl::Init()
{
    m_scrollPosition = 0;
    m_position = 0;
    m_selStart = -1;
    m_selEnd = -1;
    m_itemButDown = -1;
    m_pCaret = (wxCaret*) NULL;
    m_fontHeight = 0;

    m_isModified = false;
    m_isEditable = true;
}

bool wxGenericTextCtrl::Create(wxWindow *parent, wxWindowID id,
                               const wxString& value,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    bool res = wxGTextCtrlBase::Create(parent,id,pos,size,
        style|wxWANTS_CHARS,validator,name);

    if ( res )
    {
        wxFont default_font = wxGTextCtrlBase::GetFont();
        wxGTextCtrlBase::SetOwnFont ( default_font );
    
        SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) );
        m_text = value;

        RecalculateMetrics();

        SetCursor( wxCURSOR_IBEAM );

        if ( !m_pCaret )
        {
            m_pCaret = new wxCaret(this,1,m_fontHeight);
        }

        SetBackgroundStyle( wxBG_STYLE_SYSTEM );

        SetInsertionPoint(0);

        return true;
    }

    return false;
}

wxGenericTextCtrl::~wxGenericTextCtrl()
{
    delete m_pCaret;
}

void wxGenericTextCtrl::RecalculateMetrics()
{
    // Call this on Create and SetFont (atleast)
    int x;
	GetTextExtent(wxT("jG"), &x, &m_fontHeight, 0, 0, &m_font);
}

void wxGenericTextCtrl::DispatchEvent( int evtId )
{
    wxCommandEvent evt(evtId,GetId());
    evt.SetEventObject (this);
    GetEventHandler()->AddPendingEvent(evt);
}

#define wxTE_HT_NO_TEXT 0
#define wxTE_HT_BEFORE  1
#define wxTE_HT_ON_TEXT 2
#define wxTE_HT_BEYOND  3

void wxGenericTextCtrl::OnKeyEvent ( wxKeyEvent& event )
{
    int keycode = event.GetKeyCode();
    
    if ( keycode == WXK_LEFT || keycode == WXK_UP )
    {
        int pos = (int)m_position;

        if ( pos > 0 )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selStart )
                        m_selStart = pos - 1;
                    else
                    {
                        m_selEnd = pos - 1;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos - 1;
                    m_selEnd = pos;
                }
                Refresh (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                Refresh ();
            }

            SetInsertionPoint ( pos - 1 );
                
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            Refresh ();
        }

    }
    else if ( keycode == WXK_RIGHT || keycode == WXK_DOWN )
    {
        int pos = (int)m_position;

        if ( pos < (int)m_text.length() )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selEnd )
                        m_selEnd = pos + 1;
                    else
                    {
                        m_selStart = pos + 1;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos;
                    m_selEnd = pos + 1;
                }
                Refresh (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                Refresh ();
            }

            SetInsertionPoint ( pos + 1 );
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            Refresh ();
        }
    }
    else if ( keycode == WXK_BACK )
    {
        if ( m_selStart != -1 )
        {
            // With selection, backspace works just like delete.
            DeleteSelection();

            SetInsertionPoint ( m_position );

            Refresh();

            DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
        }
        else if ( m_position > 0 )
        {
            m_text.erase ( m_position - 1, 1 );

            SetInsertionPoint ( m_position - 1 );

            Refresh();

            DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
        }
    }
    else if ( keycode == WXK_DELETE && !event.ShiftDown() )
    {
        if ( m_selStart == -1 )
        {
            // Delete character at cursor

            if ( m_position < m_text.length() )
            {
                m_text.erase ( m_position, 1 );

                SetInsertionPoint ( m_position );

                Refresh();

                DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
            }
        }
        else
        {
            // Delete selection

            DeleteSelection();

            SetInsertionPoint ( m_position );

            Refresh();

            DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
        }
    }
    else if ( keycode == WXK_RETURN || keycode == WXK_NUMPAD_ENTER )
    {
        if ( GetWindowStyle() & wxTE_PROCESS_ENTER )
        {
            DispatchEvent (wxEVT_COMMAND_TEXT_ENTER);
        }
    }
    else if ( keycode == WXK_HOME )
    {
        int pos = (int)m_position;

        if ( pos > 0 )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selStart )
                        m_selStart = 0;
                    else
                    {
                        m_selEnd = m_selStart;
                        m_selStart = 0;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = 0;
                    m_selEnd = pos;
                }
                Refresh (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                Refresh ();
            }

            SetInsertionPoint ( 0 );
                
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            Refresh ();
        }

    }
    else if ( keycode == WXK_END )
    {
        int pos = (int)m_position;

        if ( pos < (int)m_text.length() )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selEnd )
                        m_selEnd = m_text.length();
                    else
                    {
                        m_selStart = m_selEnd;
                        m_selEnd = m_text.length();
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos;
                    m_selEnd = m_text.length();
                }
                Refresh (); // need to refresh after selection change
            }
            else if ( m_selStart != -1 )
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                Refresh ();
            }
            SetInsertionPoint ( m_text.length() );
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            Refresh ();
        }
    }
    else if ( event.GetEventType() == wxEVT_CHAR &&
              ( (keycode >= WXK_SPACE && keycode < WXK_START )
                ||
                keycode > WXK_WINDOWS_MENU
              )
            )
    {
    // character?
    #if wxUSE_UNICODE
    # if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
        // The old way with unicode.
        //wxChar c = event.m_uniChar didn't work.
        wxChar c = (wxChar) keycode;
    # else
        // The new way (2.5.3+).
        wxChar c = event.GetUnicodeKey();
    # endif
    #else
        wxChar c = (wxChar) keycode;
    #endif

        DeleteSelection();

        m_text.insert ( m_position, 1, c );

        SetInsertionPoint ( m_position + 1, m_position );

        Refresh();

        DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
    }
    else
    {
        // Clipboard operations
        int cb_op = 100; // 100 = invalid

        // Cut:   Ctrl-X and Shift-Del
        // Copy:  Ctrl-C and ???
        // Paste: Ctrl-V and Shift-Ins

        if ( event.ControlDown() )
        {
            if ( keycode == wxT('X') ) cb_op = 1;
            else if ( keycode == wxT('C') ) cb_op = 2;
            else if ( keycode == wxT('V') ) cb_op = 3;
        }
        else if ( event.ShiftDown() )
        {
            if ( keycode == WXK_DELETE ) cb_op = 1;
            //else if ( keycode == wxT('C') ) cb_op = 2;
            else if ( keycode == WXK_INSERT ) cb_op = 3;
        }

        // Clipboard operations
        if ( cb_op < 3 )
        {
        // Cut and Copy
            if ( m_selStart >= 0 && wxTheClipboard->Open() )
            {
                wxTextDataObject* obj = new wxTextDataObject(
                    m_text.Mid(m_selStart,m_selEnd-m_selStart)
                    );
                //wxLogDebug ( wxT("data_object = 0x%X"), (size_t)obj );
                wxTheClipboard->AddData( obj );

                // Cut-only Portion
                if ( cb_op == 1 )
                {
                    DeleteSelection();

                    SetInsertionPoint ( m_position );

                    Refresh();

                    DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
                }

                wxTheClipboard->Close();

                wxTheClipboard->Flush();

                //delete obj;
            }
        }
        else if ( cb_op == 3 )
        {
        // Paste

            if ( wxTheClipboard->Open() )
            {
                if (wxTheClipboard->IsSupported( wxDF_TEXT ))
                {
                    wxTextDataObject data;
                    wxTheClipboard->GetData( data );
                    wxString text = data.GetText();

                    DeleteSelection();

                    m_text.insert ( m_position, text );

                    SetInsertionPoint ( m_position + text.length(), m_position + text.length() - 1 );

                    Refresh();

                    DispatchEvent (wxEVT_COMMAND_TEXT_UPDATED);
                }  
                wxTheClipboard->Close();
            }
        }
        else
            event.Skip();
    }
}

void wxGenericTextCtrl::OnMouseEvent ( wxMouseEvent& event )
{
    int x = event.GetPosition().x;

    if ( event.GetEventType() == wxEVT_MOTION )
    {
        if ( event.Dragging() && m_itemButDown >= 0 )
        {
            wxSize sz = GetClientSize();

            // Fix position;
            if ( x < wxCC_TEXTCTRL_XSPACING )
                x = wxCC_TEXTCTRL_XSPACING;
            else if ( x >= (sz.x-wxCC_TEXTCTRL_XSPACING) )
                x = (sz.x-1-wxCC_TEXTCTRL_XSPACING);

            int hitpos;

            if ( HitTest(x,&hitpos) >= wxTE_HT_ON_TEXT )
            {
                int ns1, ns2;
                if ( hitpos < m_itemButDown )
                {
                    ns1 = hitpos;
                    ns2 = m_itemButDown;
                }
                else if ( hitpos > m_itemButDown )
                {
                    ns1 = m_itemButDown;
                    ns2 = hitpos;
                }
                else
                {
                    ns1 = -1;
                    ns2 = m_selEnd;
                }

                if ( m_selStart != ns1 || m_selEnd != ns2 )
                {
                    m_selStart = ns1;
                    m_selEnd = ns2;
                    Refresh();
                    SetInsertionPoint ( hitpos );
                }
            }
        }
    }
    else if ( event.GetEventType() == wxEVT_LEFT_DOWN 
#if !wxPG_TEXTCTRL_DOUBLE_CLICK_MODE
              || event.GetEventType() == wxEVT_LEFT_DCLICK
#endif
            )
    {
        // Focus now!
        SetFocus();

        //wxLogDebug ( wxT("wxGenericTextCtrl::OnMouseEvent(wxEVT_LEFT_DOWN)") );
        int hitpos;
        int res = HitTest(x,&hitpos);
        if ( res < wxTE_HT_ON_TEXT )
            hitpos = m_text.length();

        //if ( res >= wxTE_HT_ON_TEXT )
        {

            //wxLogDebug(wxT("hitpos:%i"),hitpos);

            SetInsertionPoint ( hitpos );

            m_itemButDown = hitpos;

            // Clear selection?
            if ( m_selStart != -1 )
            {
                m_selStart = -1;
                Refresh ();
            }
        }

    }
    else if ( event.GetEventType() == wxEVT_LEFT_UP )
    {
        m_itemButDown = -1;
    }
#if wxPG_TEXTCTRL_DOUBLE_CLICK_MODE
    else if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
    {
        // Select double-clicked word.
        SetFocus();

        int hitpos;

        if ( HitTest(x,&hitpos) >= wxTE_HT_ON_TEXT )
        {
            // hitpos = first character that is not counted.

            int textlen = m_text.length();

            // Find start pos
            int startpos = hitpos-1;
            if ( startpos >= 0 )
            {
                const wxChar* p = &m_text.c_str()[startpos];
                while ( startpos >= 0 && *p > wxT(' ') )
                {
                    p--;
                    startpos--;
                }
                startpos++;
            }

            // Find end pos
            int endpos = hitpos-1;
            if ( endpos >= 0 )
            {
                const wxChar* p = &m_text.c_str()[endpos];
                while ( endpos < textlen && *p > wxT(' ') )
                {
                    p++;
                    endpos++;
                }
            #if wxPG_TEXTCTRL_DOUBLE_CLICK_MODE == 2
                while ( endpos < textlen && *p <= wxT(' ') )
                {
                    p++;
                    endpos++;
                }
            #endif
            }

            if ( endpos > startpos )
            {
                m_selStart = startpos;
                m_selEnd = endpos;
            }
            else
            {
                m_selStart = -1;
                endpos = hitpos;
            }

            Refresh();
            SetInsertionPoint ( endpos );
        }
    }
#endif
}

// -----------------------------------------------------------------------
// Pure virtuals
// -----------------------------------------------------------------------

wxString wxGenericTextCtrl::GetValue() const
{
    return m_text;
}

void wxGenericTextCtrl::SetValue ( const wxString& value )
{
    m_text = value;
    m_position = m_scrollPosition = 0;
    m_selStart = m_selEnd = -1;
    SetInsertionPoint(m_position);
    Refresh();
}

wxString wxGenericTextCtrl::GetRange(long from, long to) const
{
    return m_text.SubString(from,to-from);
}

int wxGenericTextCtrl::GetLineLength(long lineNo) const
{
    if ( lineNo == 0 )
        return m_text.length();
    return 0;
}

wxString wxGenericTextCtrl::GetLineText(long lineNo) const
{
    if ( lineNo == 0 )
        return m_text;
    return wxEmptyString;
}

int wxGenericTextCtrl::GetNumberOfLines() const
{
    return 1;
}

bool wxGenericTextCtrl::IsModified() const
{
    return m_isModified;
}

bool wxGenericTextCtrl::IsEditable() const
{
    return m_isEditable;
}

void wxGenericTextCtrl::GetSelection(long* from, long* to) const
{
    if (from)
        *from = m_selStart;
    if (to)
        *to = m_selEnd;
}

void wxGenericTextCtrl::Remove(long from, long to)
{
    m_text.Remove(from,to-from);
    Refresh();
}

void wxGenericTextCtrl::SetFocus()
{
    //wxLogDebug(wxT("wxGenericTextCtrl::SetFocus"));
    wxGTextCtrlBase::SetFocus();
    SetInsertionPoint( m_position, -1 );
}

// -----------------------------------------------------------------------
// Drawing
// -----------------------------------------------------------------------

void wxGenericTextCtrl::OnPaint ( wxPaintEvent& /*event*/ )
{
    wxPaintDC dc(this);

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxGenericTextCtrl::Draw()") );
#endif

    wxString* str = &m_text;
    wxString tempstr;

    wxRect rect(wxPoint(0,0),GetClientSize());

    wxRect write_rect(rect.x+wxCC_TEXTCTRL_XSPACING,
                      rect.y,
                      rect.width-(wxCC_TEXTCTRL_XSPACING*2),
                      rect.height);

    //wxLogDebug ( wxT("CC: DRAW ( x = %i, y = %i, text = %s )"), (int)rect.x, (int)rect.y,
    //    str->c_str() );

    HideCaretBalanced();

//    wxColour colBg = GetBackgroundColour();
    wxColour colTx = GetForegroundColour();

/*    dc.SetBrush ( colBg );
    dc.SetPen ( colBg );
    dc.DrawRectangle ( rect );*/

    // get font metrics
    dc.SetFont ( m_font );
    dc.SetTextForeground ( colTx );

    //int fw = dc.GetCharWidth() + 1;
    int fh = dc.GetCharHeight();

    if ( fh < rect.height )
    {
        write_rect.y += (rect.height - fh) / 2;
        write_rect.height = fh;
    }

    // determine number of characters to write.
    unsigned int scrollpos = m_scrollPosition;

    int hitpos;
    if ( HitTest(rect.width-wxCC_TEXTCTRL_XSPACING/*-3+2*/,&hitpos) < wxTE_HT_ON_TEXT )
    {
        ShowCaretBalanced();
        return;
    }
    /*
    int res = HitTest ( wxPoint(rect.width-wxCC_TEXTCTRL_XSPACING-3+2,wxCC_TEXTCTRL_YSPACING+1) );
    if ( res < 1 )
    {
        manager->ShowCaretBalanced();
        return;
    }
    */

    unsigned int write_end = (unsigned int)hitpos;

#ifdef __WXDEBUG__
    if ( write_end < scrollpos)
    {
        wxLogDebug ( wxT("ERROR: write_end (%u) should be more than scrollpos (%u) "),
            write_end, scrollpos );
        wxASSERT ( write_end >= scrollpos );
        ShowCaretBalanced();
        return;
    }
#endif

    unsigned int charcount = write_end - scrollpos;

    //wxLogDebug ( wxT("    (%i, %i, %i, %i)"), rect.x, rect.y, rect.width, rect.height );

    if ( m_selStart < 0 ||
         FindFocus() != this ||
         (unsigned int)m_selStart >= write_end ||
         (unsigned int)m_selEnd <= scrollpos )
    {

        // No selection to be drawn
        tempstr = str->Mid(scrollpos,charcount);
        dc.DrawText ( tempstr, write_rect.x, write_rect.y );
    }
    else
    {
        // Selection has to be drawn

        int w1 = 0, w2 = 0;
        unsigned int selstart = (unsigned int) m_selStart;
        if ( selstart < scrollpos ) selstart = scrollpos;
        unsigned int selend = (unsigned int) m_selEnd;
        if ( selend > write_end ) selend = write_end;

        // unselected portion 1

        if ( selstart > scrollpos )
        {
            tempstr = str->Mid(scrollpos, selstart-scrollpos);
            GetTextExtent (tempstr,&w1,&fh,NULL,NULL,&m_font);
            dc.DrawText ( tempstr, write_rect.x, write_rect.y );
        }

        // selected portion

        if ( selend > scrollpos )
        {
            tempstr = str->Mid(selstart,selend-selstart);
            GetTextExtent (tempstr,&w2,&fh,NULL,NULL,&m_font);

            wxColour colSelBg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            wxColour colSelTx = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
            dc.SetBrush ( colSelBg );
            dc.SetPen ( colSelBg );
            dc.DrawRectangle ( write_rect.x+w1, write_rect.y, w2, write_rect.height );
            dc.SetTextForeground ( colSelTx );

            dc.DrawText ( tempstr, write_rect.x+w1, write_rect.y );
        }

        // unselected portion 2

        if ( selend < write_end )
        {
            dc.SetTextForeground ( colTx );
            tempstr = str->Mid(selend, write_end-selend);
            dc.DrawText ( tempstr, write_rect.x+w1+w2, write_rect.y );
        }
    }

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("  ends...") );
#endif

    ShowCaretBalanced();
}

// -----------------------------------------------------------------------

// should be faster than creating new string for a sub-string (as with Mid)
inline void wxstring_copy ( wxString& dst, wxString& src, unsigned int pos, unsigned int len )
{
    wxChar* ptr = (wxChar*)&src.GetWritableChar(pos);
    wxChar cb = ptr[len];
    ptr[len] = 0; // terminate
    dst = ptr;
    ptr[len] = cb; // revert to original
}

// -----------------------------------------------------------------------

#define wxCTC_HITTEST_GETEXTENT(INDEX) \
    wxstring_copy ( tempstr, m_text, m_scrollPosition, INDEX ); \
    dc.GetTextExtent ( tempstr, &tw, NULL );

//parent->GetTextExtent ( tempstr, &tw, &th, NULL, NULL, &m_control->GetFont() );

// -----------------------------------------------------------------------

// Returns index of character at position (-1 if none). Result is absolute,
// not relative to the scroll position.

int wxGenericTextCtrl::HitTest ( wxCoord x, int* pCol )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxGenericTextCtrl::HitTest(%i,%i)"), (int)pos.x, (int)pos.y );
#endif

    wxASSERT ( pCol );

    int retval = wxTE_HT_ON_TEXT;

    if ( m_text.length() < 1 )
        return wxTE_HT_NO_TEXT;

    wxClientDC dc(this);
    dc.SetFont(GetFont());

    size_t startpos = m_scrollPosition;
    size_t hi = m_text.length() - startpos;

    if ( x <= wxCC_TEXTCTRL_XSPACING )
    {
        if ( x < 0 )
            return wxTE_HT_BEFORE;
        *pCol = m_scrollPosition;
        return wxTE_HT_ON_TEXT;
    }

    x -= wxCC_TEXTCTRL_XSPACING;

    int tw;

    wxString tempstr;

    wxCTC_HITTEST_GETEXTENT(hi)

    if ( x >= tw )
    {
#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("  returns: %i"), (int)( hi + startpos ) );
#endif
        *pCol = ( hi + startpos );
        return retval;
    }

    // Start from average width based estimate.
    int current_pos = x / dc.GetCharWidth();

    wxCTC_HITTEST_GETEXTENT(current_pos)

    //int retries = 0;
    int prev_tw;

    if ( tw > x )
    {
        // Go lower.
        do
        {
            prev_tw = tw;
            current_pos--;
            wxCTC_HITTEST_GETEXTENT(current_pos)
            //retries++;
        } while ( tw > x && current_pos > 0 );

        // Get nearest.
        if ( (prev_tw-x) < (x-tw) )
            current_pos++;

        //wxLogDebug(wxT("Going lower, %i retries (x=%i,result=%i)"),retries,x,current_pos);
    }
    else if ( tw < x )
    {
        // Go higher.
        int text_len = m_text.length();
        do
        {
            prev_tw = tw;
            current_pos++;
            wxCTC_HITTEST_GETEXTENT(current_pos)
            //retries++;
        } while ( tw < x && current_pos < text_len );
        
        // Get nearest.
        if ( (x-prev_tw) < (tw-x) )
            current_pos--;

        //wxLogDebug(wxT("Going higher, %i retries (x=%i,result=%i)"),retries,x,current_pos);
    }
    

#if __INTENSE_DEBUGGING__
    //wxLogDebug (wxT("  returns: %i (%i calls)"), (int)(startpos + lo), (int)calls );
#endif

    *pCol = (startpos + current_pos);
    return retval;
}

// -----------------------------------------------------------------------

bool wxGenericTextCtrl::SetInsertionPoint ( long pos, long first_visible )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxGenericTextCtrl::SetInsertionPoint(%i)"), (int)pos );
#endif

    wxRect rect = GetClientRect();

    bool need_draw = FALSE;

    if ( pos >= 0 )
    {

        m_position = pos;

        int tw, th;

        if ( first_visible < 0 )
            first_visible = pos;

        // Need to scroll it?
        if ( first_visible < (int)m_scrollPosition )
        {
            m_scrollPosition = first_visible;
            //if ( pos > 0 ) m_scrollPosition = pos - 1;
            need_draw = TRUE;
        }

        wxString s = m_text.Mid(m_scrollPosition,pos-m_scrollPosition);
        GetTextExtent (s,&tw,&th,NULL,NULL,&m_font);
        //wxLogDebug(wxT("text_extent: %i"),tw);

        int area_width = (rect.width-(wxCC_TEXTCTRL_XSPACING*2));
        //wxWindow* parent = GetParent();

        // Increment scroll position until end becomes visible.
        while ( tw >= area_width )
        {
            m_scrollPosition += 1;
            s = m_text.Mid(m_scrollPosition,pos-m_scrollPosition);
            GetTextExtent (s,&tw,&th,NULL,NULL,&m_font);
            need_draw = TRUE;
        }

        // Adjust to the space between characters.
        //tw -= 1; // Sometime back this was needed. Don't know why not anymore.

        if ( FindFocus() == this )
        {

            wxASSERT ( m_pCaret );

            m_pCaret->Move(wxCC_TEXTCTRL_XSPACING + tw,
                           (rect.height-m_fontHeight)/2);

            if ( !m_pCaret->IsVisible() )
                m_pCaret->Show();

            //wxLogDebug(wxT("CaretVisible: %i"),(int)(m_pCaret->IsVisible()));
        }

        if ( need_draw )
            Refresh();

    }
    else
    {
        if ( FindFocus() == this )
        {
            if ( m_pCaret->IsVisible() )
                m_pCaret->Show(false);
        }
    }

    return need_draw;
}

// -----------------------------------------------------------------------

bool wxGenericTextCtrl::SetSelection ( long from, long to )
{
    if ( from < 0 )
        from = 0;
    if ( to < 0 )
        to = m_text.length();

    if ( m_selStart == from && m_selEnd == to )
        return false;

    m_selStart = from;
    m_selEnd = to;

    return true;
}

// -----------------------------------------------------------------------

// like DEL key was pressed
void wxGenericTextCtrl::DeleteSelection ()
{
    // remove selected portion?

    if ( m_selStart != -1 )
    {
        wxASSERT( m_selStart < (int)m_text.length() );
        wxASSERT( m_selEnd >= m_selStart );

        // need to move insertion point to the beginning
        //if ( m_position == (unsigned int)m_selEnd )
        //    SetInsertionPoint ( m_selStart, -1, rect, ctrl );

        //wxLogDebug(wxT("%i,%i"),m_selStart,m_selEnd);

        m_text.erase ( m_selStart, m_selEnd - m_selStart );
        m_position = m_selStart;
        m_selStart = m_selEnd = -1;
    }
}

#endif

#if wxPG_USE_CUSTOM_CONTROLS

// -----------------------------------------------------------------------
// wxCustomControlHandler
// -----------------------------------------------------------------------

void wxCustomControlHandler::Create ( wxCustomControl* pctrl,
                                   const wxPoint& pos,
                                   const wxSize& sz )
{
    m_control = pctrl;
    SetSize ( pos, sz );
}

// -----------------------------------------------------------------------
// wxCustomTextCtrlHandler
// -----------------------------------------------------------------------

#define wxTE_HT_NO_TEXT 0
#define wxTE_HT_BEFORE  1
#define wxTE_HT_ON_TEXT 2
#define wxTE_HT_BEYOND  3

bool wxCustomTextCtrlHandler::OnKeyEvent ( wxKeyEvent& event )
{
    wxCCustomTextCtrl* ctrl = (wxCCustomTextCtrl*)m_control;
    wxCustomControlManager* manager = ctrl->GetManager();
    int keycode = event.GetKeyCode();
    
    if ( keycode == WXK_LEFT || keycode == WXK_UP )
    {
        int pos = (int)m_position;

        if ( pos > 0 )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selStart )
                        m_selStart = pos - 1;
                    else
                    {
                        m_selEnd = pos - 1;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos - 1;
                    m_selEnd = pos;
                }
                ctrl->Draw (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                ctrl->Draw ();
            }

            ctrl->SetInsertionPoint ( pos - 1 );
                
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            ctrl->Draw ();
        }

    }
    else if ( keycode == WXK_RIGHT || keycode == WXK_DOWN )
    {
        int pos = (int)m_position;

        if ( pos < (int)m_text.length() )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selEnd )
                        m_selEnd = pos + 1;
                    else
                    {
                        m_selStart = pos + 1;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos;
                    m_selEnd = pos + 1;
                }
                ctrl->Draw (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                ctrl->Draw ();
            }

            ctrl->SetInsertionPoint ( pos + 1 );
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            ctrl->Draw ();
        }
    }
    else if ( keycode == WXK_BACK )
    {
        if ( m_selStart != -1 )
        {
            // With selection, backspace works just like delete.
            DeleteSelection();

            ctrl->SetInsertionPoint ( m_position );

            ctrl->Draw();

            manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
        }
        else if ( m_position > 0 )
        {
            m_text.erase ( m_position - 1, 1 );

            ctrl->SetInsertionPoint ( m_position - 1 );

            ctrl->Draw();

            manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
        }
    }
    else if ( keycode == WXK_DELETE && !event.ShiftDown() )
    {
        if ( m_selStart == -1 )
        {
            // Delete character at cursor

            if ( m_position < m_text.length() )
            {
                m_text.erase ( m_position, 1 );

                ctrl->SetInsertionPoint ( m_position );

                ctrl->Draw();

                manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
            }
        }
        else
        {
            // Delete selection

            DeleteSelection();

            ctrl->SetInsertionPoint ( m_position );

            ctrl->Draw();

            manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
        }
    }
    else if ( keycode == WXK_RETURN || keycode == WXK_NUMPAD_ENTER )
    {
        if ( ctrl->GetWindowStyle() & wxTE_PROCESS_ENTER )
        {
            manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_ENTER);
        }
    }
    else if ( keycode == WXK_HOME )
    {
        int pos = (int)m_position;

        if ( pos > 0 )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selStart )
                        m_selStart = 0;
                    else
                    {
                        m_selEnd = m_selStart;
                        m_selStart = 0;
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = 0;
                    m_selEnd = pos;
                }
                ctrl->Draw (); // need to refresh after selection change
            }
            else
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                ctrl->Draw ();
            }

            ctrl->SetInsertionPoint ( 0 );
                
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            ctrl->Draw ();
        }

    }
    else if ( keycode == WXK_END )
    {
        int pos = (int)m_position;

        if ( pos < (int)m_text.length() )
        {
            if ( event.m_shiftDown )
            {
                // shift was down - set/modify selection
                if ( m_selStart != -1 )
                {
                    // modify
                    if ( pos == m_selEnd )
                        m_selEnd = m_text.length();
                    else
                    {
                        m_selStart = m_selEnd;
                        m_selEnd = m_text.length();
                        if ( m_selStart == m_selEnd )
                            m_selStart = -1;
                    }
                }
                else
                {
                    // set
                    m_selStart = pos;
                    m_selEnd = m_text.length();
                }
                ctrl->Draw (); // need to refresh after selection change
            }
            else if ( m_selStart != -1 )
            {
                // If not shift down, clear the selection
                m_selStart = -1;
                ctrl->Draw ();
            }
            ctrl->SetInsertionPoint ( m_text.length() );
        }
        else if ( m_selStart != -1 && !event.m_shiftDown )
        {
            // Just reset the selection.
            m_selStart = -1;
            ctrl->Draw ();
        }
    }
    else if ( event.GetEventType() == wxEVT_CHAR &&
              ( (keycode >= WXK_SPACE && keycode < WXK_START )
                ||
                keycode > WXK_WINDOWS_MENU
              )
            )
    {
    // character?
    #if wxUSE_UNICODE
    # if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
        // The old way with unicode.
        //wxChar c = event.m_uniChar didn't work.
        wxChar c = (wxChar) keycode;
    # else
        // The new way (2.5.3+).
        wxChar c = event.GetUnicodeKey();
    # endif
    #else
        wxChar c = (wxChar) keycode;
    #endif

        DeleteSelection();

        m_text.insert ( m_position, 1, c );
        
        ctrl->SetInsertionPoint ( m_position + 1, m_position );
        
        ctrl->Draw();

        manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
    }
    else
    {
        // Clipboard operations
        int cb_op = 100; // 100 = invalid

        // Cut:   Ctrl-X and Shift-Del
        // Copy:  Ctrl-C and ???
        // Paste: Ctrl-V and Shift-Ins

        if ( event.ControlDown() )
        {
            if ( keycode == wxT('X') ) cb_op = 1;
            else if ( keycode == wxT('C') ) cb_op = 2;
            else if ( keycode == wxT('V') ) cb_op = 3;
        }
        else if ( event.ShiftDown() )
        {
            if ( keycode == WXK_DELETE ) cb_op = 1;
            //else if ( keycode == wxT('C') ) cb_op = 2;
            else if ( keycode == WXK_INSERT ) cb_op = 3;
        }

        // Clipboard operations
        if ( cb_op < 3 )
        {
        // Cut and Copy
            if ( m_selStart >= 0 && wxTheClipboard->Open() )
            {
                wxTextDataObject* obj = new wxTextDataObject(
                    m_text.Mid(m_selStart,m_selEnd-m_selStart)
                    );
                //wxLogDebug ( wxT("data_object = 0x%X"), (size_t)obj );
                wxTheClipboard->AddData( obj );

                // Cut-only Portion
                if ( cb_op == 1 )
                {
                    DeleteSelection();

                    ctrl->SetInsertionPoint ( m_position );

                    ctrl->Draw();

                    manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
                }

                wxTheClipboard->Close();

                wxTheClipboard->Flush();

                //delete obj;
            }
        }
        else if ( cb_op == 3 )
        {
        // Paste

            if ( wxTheClipboard->Open() )
            {
                if (wxTheClipboard->IsSupported( wxDF_TEXT ))
                {
                    wxTextDataObject data;
                    wxTheClipboard->GetData( data );
                    wxString text = data.GetText();

                    DeleteSelection();

                    m_text.insert ( m_position, text );

                    ctrl->SetInsertionPoint ( m_position + text.length(), m_position + text.length() - 1 );

                    ctrl->Draw();

                    manager->AddEvent (ctrl,wxEVT_COMMAND_TEXT_UPDATED);
                }  
                wxTheClipboard->Close();
            }
        }
        else
            return FALSE;
    }

    return TRUE;
}

// -----------------------------------------------------------------------

bool wxCustomTextCtrlHandler::OnMouseEvent ( wxMouseEvent& event )
{
    wxCCustomTextCtrl* ctrl = (wxCCustomTextCtrl*)m_control;
    int x = event.GetPosition().x;

    if ( event.GetEventType() == wxEVT_MOTION )
    {
        //wxLogDebug ( wxT("wxCustomTextCtrlHandler::OnMouseEvent(wxEVT_MOTION)") );

        if ( event.Dragging() && m_control->GetManager()->IsDragging () )
        {
            // Fix position;
            if ( x < wxCC_TEXTCTRL_XSPACING )
                x = wxCC_TEXTCTRL_XSPACING;
            else if ( x >= (m_rect.width-wxCC_TEXTCTRL_XSPACING) )
                x = (m_rect.width-1-wxCC_TEXTCTRL_XSPACING);

            int hitpos;

            if ( HitTest(x,&hitpos) >= wxTE_HT_ON_TEXT /*&& hitpos != -1*/ )
            {
                int ns1, ns2;
                if ( hitpos < m_itemButDown )
                {
                    ns1 = hitpos;
                    ns2 = m_itemButDown;
                }
                else if ( hitpos > m_itemButDown )
                {
                    ns1 = m_itemButDown;
                    ns2 = hitpos;
                }
                else
                {
                    ns1 = -1;
                    ns2 = m_selEnd;
                }

                if ( m_selStart != ns1 || m_selEnd != ns2 )
                {
                    m_selStart = ns1;
                    m_selEnd = ns2;
                    ctrl->Draw();
                    ctrl->SetInsertionPoint ( hitpos );
                }
            }
        }
    }
    else if ( event.GetEventType() == wxEVT_LEFT_DOWN 
#if !wxPG_TEXTCTRL_DOUBLE_CLICK_MODE
              || event.GetEventType() == wxEVT_LEFT_DCLICK
#endif
            )
    {
        //wxLogDebug ( wxT("wxCustomTextCtrlHandler::OnMouseEvent(wxEVT_LEFT_DOWN)") );
        int hitpos;

        if ( HitTest(x,&hitpos) >= wxTE_HT_ON_TEXT )
        {
            ctrl->SetInsertionPoint ( hitpos );

            m_itemButDown = hitpos;

            // Clear selection?
            if ( m_selStart != -1 )
            {
                m_selStart = -1;
                ctrl->Draw ();
            }
        }

    }
#if wxPG_TEXTCTRL_DOUBLE_CLICK_MODE
    else if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
    {
        // Select double-clicked word.

        int hitpos;

        if ( HitTest(x,&hitpos) >= wxTE_HT_ON_TEXT )
        {
            // hitpos = first character that is not counted.

            int textlen = m_text.length();

            // Find start pos
            int startpos = hitpos-1;
            if ( startpos >= 0 )
            {
                const wxChar* p = &m_text.c_str()[startpos];
                while ( startpos >= 0 && *p > wxT(' ') )
                {
                    p--;
                    startpos--;
                }
                startpos++;
            }

            // Find end pos
            int endpos = hitpos-1;
            if ( endpos >= 0 )
            {
                const wxChar* p = &m_text.c_str()[endpos];
                while ( endpos < textlen && *p > wxT(' ') )
                {
                    p++;
                    endpos++;
                }
            #if wxPG_TEXTCTRL_DOUBLE_CLICK_MODE == 2
                while ( endpos < textlen && *p <= wxT(' ') )
                {
                    p++;
                    endpos++;
                }
            #endif
            }

            if ( endpos > startpos )
            {
                m_selStart = startpos;
                m_selEnd = endpos;
            }
            else
            {
                m_selStart = -1;
                endpos = hitpos;
            }

            ctrl->Draw();
            ctrl->SetInsertionPoint ( endpos );
        }
    }
#endif
    return FALSE;
}

// -----------------------------------------------------------------------

void wxCustomTextCtrlHandler::SetValue ( const wxString& value )
{
    m_scrollPosition = 0;
    m_position = 0;
    m_selStart = -1;
    m_text = value;
    m_flags |= wxCC_FL_MODIFIED;
}

// -----------------------------------------------------------------------

void wxCustomTextCtrlHandler::Draw ( wxDC& dc, const wxRect& rect )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxCustomTextCtrlHandler::Draw()") );
#endif
    wxCustomControl* ctrl = m_control;
    //const wxRect& rect = m_rect;

    wxASSERT ( ctrl->m_pFont != NULL );

    wxCustomControlManager* manager = ctrl->m_manager;

    wxWindow* parent = manager->m_parent;

    //wxWindow* parent = m_parent;
    wxString* str = &m_text;
    //wxString& tempstr = manager->m_tempStr1;
    wxString tempstr;

    wxRect write_rect(rect.x+wxCC_TEXTCTRL_XSPACING,
                      rect.y,
                      rect.width-(wxCC_TEXTCTRL_XSPACING*2),
                      rect.height);

    //wxLogDebug ( wxT("CC: DRAW ( x = %i, y = %i, text = %s )"), (int)rect.x, (int)rect.y,
    //    str->c_str() );

    manager->HideCaretBalanced();

    dc.SetBrush ( manager->GetWindowColour() );
    dc.SetPen ( manager->GetWindowColour() );
    
    dc.DrawRectangle ( rect );

    // get font metrics
    dc.SetFont ( *ctrl->m_pFont );
    dc.SetTextForeground ( manager->GetWindowTextColour() );

    //int fw = dc.GetCharWidth() + 1;
    int fh = dc.GetCharHeight();

    if ( fh < rect.height )
    {
        write_rect.y += (rect.height - fh) / 2;
        write_rect.height = fh;
    }

    // determine number of characters to write.
    unsigned int scrollpos = m_scrollPosition;

    int hitpos;
    if ( HitTest(rect.width-wxCC_TEXTCTRL_XSPACING/*-3+2*/,&hitpos) < wxTE_HT_ON_TEXT )
    {
        manager->ShowCaretBalanced();
        return;
    }
    /*
    int res = HitTest ( wxPoint(rect.width-wxCC_TEXTCTRL_XSPACING-3+2,wxCC_TEXTCTRL_YSPACING+1) );
    if ( res < 1 )
    {
        manager->ShowCaretBalanced();
        return;
    }
    */

    unsigned int write_end = (unsigned int)hitpos;

#ifdef __WXDEBUG__
    if ( write_end < scrollpos)
    {
        wxLogDebug ( wxT("ERROR: write_end (%u) should be more than scrollpos (%u) "),
            write_end, scrollpos );
        wxASSERT ( write_end >= scrollpos );
        manager->ShowCaretBalanced();
        return;
    }
#endif

    unsigned int charcount = write_end - scrollpos;

    //wxLogDebug ( wxT("    (%i, %i, %i, %i)"), rect.x, rect.y, rect.width, rect.height );

    if ( m_selStart < 0 ||
         (unsigned int)m_selStart >= write_end ||
         (unsigned int)m_selEnd <= scrollpos 
       )
    {

        // No selection to be drawn
        tempstr = str->Mid(scrollpos,charcount);
        dc.DrawText ( tempstr, write_rect.x, write_rect.y );
    }
    else
    {
        // Selection has to be drawn

        int w1 = 0, w2 = 0;
        unsigned int selstart = (unsigned int) m_selStart;
        if ( selstart < scrollpos ) selstart = scrollpos;
        unsigned int selend = (unsigned int) m_selEnd;
        if ( selend > write_end ) selend = write_end;

        // unselected portion 1

        if ( selstart > scrollpos )
        {
            tempstr = str->Mid(scrollpos, selstart-scrollpos);
            parent->GetTextExtent (tempstr,&w1,&fh,NULL,NULL,ctrl->m_pFont);
            dc.DrawText ( tempstr, write_rect.x, write_rect.y );
        }

        // selected portion

        if ( selend > scrollpos )
        {
            tempstr = str->Mid(selstart,selend-selstart);
            parent->GetTextExtent (tempstr,&w2,&fh,NULL,NULL,ctrl->m_pFont);

            dc.SetBrush ( manager->GetSelectionColour() );
            dc.SetPen ( manager->GetSelectionColour() );
            dc.DrawRectangle ( write_rect.x+w1, write_rect.y, w2, write_rect.height );
            dc.SetTextForeground ( manager->GetSelectionTextColour() );

            dc.DrawText ( tempstr, write_rect.x+w1, write_rect.y );
        }

        // unselected portion 2

        if ( selend < write_end )
        {
            dc.SetTextForeground ( manager->GetWindowTextColour() );
            tempstr = str->Mid(selend, write_end-selend);
            dc.DrawText ( tempstr, write_rect.x+w1+w2, write_rect.y );
        }
    }

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("  ends...") );
#endif

    manager->ShowCaretBalanced();
}

// -----------------------------------------------------------------------

// should be faster than creating new string for a sub-string (as with Mid)
inline void wxstring_copy ( wxString& dst, wxString& src, unsigned int pos, unsigned int len )
{
    wxChar* ptr = (wxChar*)&src.GetWritableChar(pos);
    wxChar cb = ptr[len];
    ptr[len] = 0; // terminate
    dst = ptr;
    ptr[len] = cb; // revert to original
}

// -----------------------------------------------------------------------

// used by HitTest
/*
int wxCustomTextCtrlHandler::UpdateExtentCache ( wxString& tempstr, size_t index )
{
    int tw, th;
    wxArrayInt& arrext = m_arrExtents;
    unsigned int startpos = m_scrollPosition;
    if ( index >= arrext.GetCount() ) arrext.SetCount(index+1,-1);
    wxWindow* parent = m_control->GetParent();
    wxstring_copy ( tempstr, m_text, startpos, index );
    parent->GetTextExtent ( tempstr, &tw, &th, NULL, NULL, &m_control->GetFont() );
    
    //wxLogDebug (wxT("wxCustomTextCtrlHandler::UpdateExtentCache(%i)"),(int)index);

    arrext.Item(index) = tw;
    return tw;
}

#define wxCTC_HITTEST_GETEXTENT(INDEX) \
        if ( INDEX < arrext.GetCount() && arrext.Item(INDEX) >= 0 ) \
            tw = arrext.Item(INDEX); \
        else \
            tw = UpdateExtentCache(tempstr,INDEX); \

*/

#define wxCTC_HITTEST_GETEXTENT(INDEX) \
    wxstring_copy ( tempstr, m_text, m_scrollPosition, INDEX ); \
    dc.GetTextExtent ( tempstr, &tw, NULL );

//parent->GetTextExtent ( tempstr, &tw, &th, NULL, NULL, &m_control->GetFont() );

// -----------------------------------------------------------------------

    // Linear search
    /*
    i = 0;
    lo = hi;
    int prev_tw = -100;

    while ( i < hi )
    {
        //tw = m_arrExtents.Item( i );
        wxstring_copy ( tempstr, m_text, startpos, i );
        parent->GetTextExtent ( tempstr, &tw, &th, NULL, NULL, ctrl->m_pFont );
        //wxLogDebug ( tempstr );

        //parent->GetTextExtent (m_text.Mid(startpos,i),&res,&tw,NULL,NULL,ctrl->m_pFont);
        int diff = tw - x;
        if ( diff > 0 )
        {
            //wxLogDebug ( wxT("  ( i=%i, diff=%i, prev_diff=%i)"), (int)i, diff, (int)(x - prev_tw) );
            if ( diff <= (x - prev_tw) )
                return (startpos+i);
            return (startpos+i - 1);
        }
        prev_tw = tw;
        i++;
    }

    return (startpos + hi);
    */

// Returns index of character at position (-1 if none). Result is absolute,
// not relative to the scroll position.

#if 1

int wxCustomTextCtrlHandler::HitTest ( wxCoord x, int* pCol )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxCustomTextCtrlHandler::HitTest(%i,%i)"), (int)pos.x, (int)pos.y );
#endif

    wxASSERT ( pCol );

    int retval = wxTE_HT_ON_TEXT;

    if ( m_text.length() < 1 )
        return wxTE_HT_NO_TEXT;

    wxCustomControl* ctrl = m_control;
    //const wxRect& rect = m_rect;

    //wxString tempstr;
    wxWindow* parent = ctrl->GetParent();
    wxClientDC dc(parent);
    dc.SetFont(ctrl->GetFont());

    size_t startpos = m_scrollPosition;
    size_t hi = m_text.length() - startpos;

    if ( x <= wxCC_TEXTCTRL_XSPACING )
    {
        if ( x < 0 )
            return wxTE_HT_BEFORE;
        *pCol = m_scrollPosition;
        return wxTE_HT_ON_TEXT;
    }

    x -= wxCC_TEXTCTRL_XSPACING;

    int tw;

    wxString& tempstr = ctrl->GetManager()->GetTempString();

    wxCTC_HITTEST_GETEXTENT(hi)

    if ( x >= tw )
    {
#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("  returns: %i"), (int)( hi + startpos ) );
#endif
        *pCol = ( hi + startpos );
        return retval;
    }

    // Start from average width based estimate.
    int current_pos = x / dc.GetCharWidth();

    wxCTC_HITTEST_GETEXTENT(current_pos)

    //int retries = 0;
    int prev_tw;

    if ( tw > x )
    {
        // Go lower.
        do
        {
            prev_tw = tw;
            current_pos--;
            wxCTC_HITTEST_GETEXTENT(current_pos)
            //retries++;
        } while ( tw > x && current_pos > 0 );

        // Get nearest.
        if ( (prev_tw-x) < (x-tw) )
            current_pos++;

        //wxLogDebug(wxT("Going lower, %i retries (x=%i,result=%i)"),retries,x,current_pos);
    }
    else if ( tw < x )
    {
        // Go higher.
        int text_len = m_text.length();
        do
        {
            prev_tw = tw;
            current_pos++;
            wxCTC_HITTEST_GETEXTENT(current_pos)
            //retries++;
        } while ( tw < x && current_pos < text_len );
        
        // Get nearest.
        if ( (x-prev_tw) < (tw-x) )
            current_pos--;
        
        //wxLogDebug(wxT("Going higher, %i retries (x=%i,result=%i)"),retries,x,current_pos);
    }
    

#if __INTENSE_DEBUGGING__
    //wxLogDebug (wxT("  returns: %i (%i calls)"), (int)(startpos + lo), (int)calls );
#endif

    *pCol = (startpos + current_pos);
    return retval;
}

#endif


#if 0

// Returns index of character at position (-1 if none). Result is absolute,
// not relative to the scroll position.

int wxCustomTextCtrlHandler::HitTest ( wxCoord x, int* pCol )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxCustomTextCtrlHandler::HitTest(%i,%i)"), (int)pos.x, (int)pos.y );
#endif

    wxASSERT ( pCol );

    int retval = wxTE_HT_ON_TEXT;

    if ( m_text.length() < 1 )
        return wxTE_HT_NO_TEXT;

    wxCustomControl* ctrl = m_control;
    //const wxRect& rect = m_rect;

    //wxString tempstr;
    wxWindow* parent = ctrl->GetParent();
    wxClientDC dc(parent);
    dc.SetFont(ctrl->GetFont());

    size_t i;
    size_t startpos = m_scrollPosition;
    size_t lo = 0;
    size_t hi = m_text.length() - startpos;
    //int th;

    /*
    int x = pos.x;

    
    if ( x < 0 || pos.y < 0 ||
         x >= (rect.width-wxCC_TEXTCTRL_XSPACING) || pos.y >= rect.height  )
        return -1;
    */
    if ( x <= wxCC_TEXTCTRL_XSPACING )
    {
        if ( x < 0 )
            return wxTE_HT_BEFORE;
        *pCol = m_scrollPosition;
        return wxTE_HT_ON_TEXT;
    }

    x -= wxCC_TEXTCTRL_XSPACING;

    /*
         x >= (rect.width-wxCC_TEXTCTRL_XSPACING) || pos.y >= rect.height  )
        return -1;
    */

    /*
    if ( hi > 256 )
    {
        //wxLogDebug (wxT("WARNING: wxCustomTextCtrlHandler::HitTest() cache recalculation exceeds 512 limit."));
        hi = 256;
    }

    // initialize extent cache
    if ( m_flags & wxCC_FL_MODIFIED )
    {
        m_arrExtents.empty();
    }

    wxArrayInt& arrext = m_arrExtents;
    */

    int res = 0;
    int tw; //, th;

    wxString& tempstr = ctrl->GetManager()->GetTempString();

    wxCTC_HITTEST_GETEXTENT(hi)

    if ( x >= tw )
    {
#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("  returns: %i"), (int)( hi + startpos ) );
#endif
        *pCol = ( hi + startpos );
        return retval;
    }

    // Limit our search area according to the average character width
    size_t cw = tw/m_text.length();
    size_t orig_hi = hi;
    size_t x_per_cw = x/cw;
    lo = x_per_cw / 2;
    hi = x_per_cw * 2;
    if ( hi > orig_hi )
        hi = orig_hi;

    // Binary search.
    while ( lo < hi )
    {
        i = (lo + hi)/2;

        wxCTC_HITTEST_GETEXTENT(i)

        //tempstr = m_text.Mid(startpos,i);
        //parent->GetTextExtent (tempstr,&tw,&th,NULL,NULL,ctrl->m_pFont);
        //tw = m_arrExtents.Item( i );
        //res = (x - m_arrExtents.Item( i - 1 ) ); // /2; // limit accuracy in favor of speed

        res = x - tw;
        //wxLogDebug (wxT("i = %i, lo = %i (\"%s\"), hi = %i, res = %i"),(int)i,(int)lo,
        //    m_text.Mid((startpos+i),1).c_str(),(int)hi,(int)res);

        if ( res < 0 )
        {
            // x is less than extent
            hi = i;
        }
        else if ( res > 0 )
        {
            // x is more than extent
            lo = i + 1;
        }
        else
        {
            lo = i;
            break;
        }
    }

    // when not exact, make sure we get nearest
    if ( res != 0 && lo > 0 )
    {
        int res_lower;
        wxCTC_HITTEST_GETEXTENT(lo)
        res = x - tw;
        wxCTC_HITTEST_GETEXTENT(lo-1)
        res_lower = x - tw;
        //wxLogDebug ( wxT ("diff(lo-1) = %i, diff(lo) = %i"), (int)abs(res_lower), (int)abs(res) );
        if ( abs(res_lower) < abs(res) )
            lo--;
    }

#if __INTENSE_DEBUGGING__
    //wxLogDebug (wxT("  returns: %i (%i calls)"), (int)(startpos + lo), (int)calls );
#endif

    *pCol = (startpos + lo);
    return retval;
}


#endif

// Returns index of character at position (-1 if none). Result is absolute,
// not relative to the scroll position.
// NOTE: This is ripped from wxUniversal's textctrl.cpp.
# if 0
int wxCustomTextCtrlHandler::HitTest ( const wxPoint& pos )
{
    wxString line = m_text.Mid(m_scrollPosition);
    int x = pos.x - wxCC_TEXTCTRL_XSPACING;

    int res = wxTE_HT_ON_TEXT; // 0 = wxTE_HT_BEFORE
                 // 1 = wxTE_HT_ON_TEXT
                 // 2 = wxTE_HT_BEYOND
    int col;
    //wxTextCtrl *self = wxConstCast(this, wxTextCtrl);
    wxWindow* wnd = m_control->GetParent();
    wxClientDC dc(wnd);
    dc.SetFont(m_control->GetFont());
    //wnd->DoPrepareDC(dc);

    wxCoord width;
    dc.GetTextExtent(line, &width, NULL);
    wxLogDebug(wxT("%i, %s"),x,line.c_str());
    if ( x >= width )
    {
        // clicking beyond the end of line is equivalent to clicking at
        // the end of it, so return the last line column
        col = line.length();
        /*if ( col )
        {
            // unless the line is empty and so doesn't have any column at all -
            // in this case return 0, what else can we do?
            col--;
        }*/

        res = wxTE_HT_BEYOND;
    }
    else if ( x < 0 )
    {
        col = 0;

        res = wxTE_HT_BEFORE;
    }
    else // we're inside the line
    {
        // now calculate the column: first, approximate it with fixed-width
        // value and then calculate the correct value iteratively: note that
        // we use the first character of the line instead of (average)
        // GetCharWidth(): it is common to have lines of dashes, for example,
        // and this should give us much better approximation in such case
        //
        // OPT: maybe using (cache) m_widthAvg would be still faster? profile!
        dc.GetTextExtent(line[0], &width, NULL);

        col = x / width;
        if ( col < 0 )
        {
            col = 0;
        }
        else if ( (size_t)col > line.length() )
        {
            col = line.length();
        }

        // matchDir is the direction in which we should move to reach the
        // character containing the given position
        enum
        {
            Match_Left  = -1,
            Match_None  = 0,
            Match_Right = 1
        } matchDir = Match_None;
        for ( ;; )
        {
            // check that we didn't go beyond the line boundary
            if ( col < 0 )
            {
                col = 0;
                break;
            }
            if ( (size_t)col > line.length() )
            {
                col = line.length();
                break;
            }

            wxString strBefore(line, (size_t)col);
            dc.GetTextExtent(strBefore, &width, NULL);
            if ( width > x )
            {
                if ( matchDir == Match_Right )
                {
                    // we were going to the right and, finally, moved beyond
                    // the original position - stop on the previous one
                    col--;

                    break;
                }

                if ( matchDir == Match_None )
                {
                    // we just started iterating, now we know that we should
                    // move to the left
                    matchDir = Match_Left;
                }
                //else: we are still to the right of the target, continue
            }
            else // width < x
            {
                // invert the logic above
                if ( matchDir == Match_Left )
                {
                    // with the exception that we don't need to backtrack here
                    break;
                }

                if ( matchDir == Match_None )
                {
                    // go to the right
                    matchDir = Match_Right;
                }
            }

            // this is not supposed to happen
            wxASSERT_MSG( matchDir, _T("logic error in wxTextCtrl::HitTest") );

            if ( matchDir == Match_Right )
                col++;
            else
                col--;
        }
    }

    // check that we calculated it correctly
    /*
#ifdef __WXDEBUG__
    if ( res == wxTE_HT_ON_TEXT )
    {
        wxCoord width1;
        wxString text = line.Left(col);
        dc.GetTextExtent(text, &width1, NULL);
        if ( (size_t)col < line.length() )
        {
            wxCoord width2;

            text += line[col];
            dc.GetTextExtent(text, &width2, NULL);

            wxASSERT_MSG( (width1 <= x) && (x < width2),
                          _T("incorrect HitTestLine() result") );
        }
        else // we return last char
        {
            wxASSERT_MSG( x >= width1, _T("incorrect HitTestLine() result") );
        }
    }
#endif // WXDEBUG_TEXT
    */

    /*
    if ( colOut )
        *colOut = col;

    return res;
    */
    return col;
}

#endif

// -----------------------------------------------------------------------

bool wxCustomTextCtrlHandler::SetInsertionPoint ( long pos, long first_visible )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxCustomTextCtrlHandler::SetInsertionPoint(%i)"), (int)pos );
#endif

    wxCustomControl* ctrl = m_control;
    const wxRect& rect = m_rect;

    wxCustomControlManager* manager = ctrl->m_manager;
    bool need_draw = FALSE;

    if ( pos >= 0 )
    {

        m_position = pos;

        wxString tempstr;

        int tw, th;

        if ( first_visible < 0 )
            first_visible = pos;

        // Need to scroll it?
        if ( first_visible < (int)m_scrollPosition )
        {
            m_scrollPosition = first_visible;
            //if ( pos > 0 ) m_scrollPosition = pos - 1;
            need_draw = TRUE;
        }

        tempstr = m_text.Mid(m_scrollPosition,pos-m_scrollPosition);
        manager->m_parent->GetTextExtent (tempstr,&tw,&th,NULL,NULL,ctrl->m_pFont);

        int area_width = (rect.width-(wxCC_TEXTCTRL_XSPACING*2));
        wxWindow* parent = ctrl->GetParent();

        // Increment scroll position until end becomes visible.
        while ( tw >= area_width )
        {
            m_scrollPosition += 1;
            tempstr = m_text.Mid(m_scrollPosition,pos-m_scrollPosition);
            parent->GetTextExtent (tempstr,&tw,&th,NULL,NULL,ctrl->m_pFont);
            need_draw = TRUE;
        }

        // Adjust to the space between characters.
        //tw -= 1; // Sometime back this was needed. Don't know why not anymore.

        if ( need_draw )
            m_flags |= wxCC_FL_MODIFIED;

        if ( ctrl == manager->GetFocused() )
        {

            // Move position to here
            manager->MoveCaret ( rect, wxCC_TEXTCTRL_XSPACING + tw,
                (rect.height-ctrl->m_fontHeight)/2 );

            manager->ShowCaret();

        }
    }
    else
    {
        if ( ctrl == manager->GetFocused() )
        {
            manager->HideCaret();
        }
    }

    return need_draw;
}

// -----------------------------------------------------------------------

bool wxCustomTextCtrlHandler::SetSelection ( long from, long to )
{
    if ( from < 0 )
        from = 0;
    if ( to < 0 )
        to = m_text.length();

    if ( m_selStart == from && m_selEnd == to )
        return FALSE;

    m_selStart = from;
    m_selEnd = to;

    return TRUE;
}

// -----------------------------------------------------------------------

// like DEL key was pressed
void wxCustomTextCtrlHandler::DeleteSelection ()
{
    // remove selected portion?
    if ( m_selStart != -1 )
    {
        // need to move insertion point to the beginning
        //if ( m_position == (unsigned int)m_selEnd )
        //    SetInsertionPoint ( m_selStart, -1, rect, ctrl );

        m_text.erase ( m_selStart, m_selEnd - m_selStart );
        m_position = m_selStart;
        m_selStart = m_selEnd = -1;
    }
}

// -----------------------------------------------------------------------

void wxCustomTextCtrlHandler::Create ( wxCustomControl* pctrl,
                                    const wxPoint& pos,
                                    const wxSize& sz,
                                    const wxString& value
                                  )
{
    wxCustomControlHandler::Create ( pctrl, pos, sz );

    m_text = value;
    m_position = m_scrollPosition = 0;
    m_selStart = m_selEnd = -1;

    m_flags |= wxCC_FL_MODIFIED;
}


// -----------------------------------------------------------------------
// wxCCustomButtonHandler
// -----------------------------------------------------------------------

void wxCCustomButtonHandler::Draw ( wxDC& dc, const wxRect& rect )
{
    wxCustomControl* ctrl = m_control;
    //const wxRect& rect = m_rect;

    wxCustomControlManager* man = ctrl->GetManager();
    wxWindow* parent = man->m_parent;

    //wxLogDebug ( wxT("(%i,%i)"), rect.width, rect.height );
    
    long flags = 0;
    if ( ctrl->m_flags & wxCC_FL_MOUSEFOCUS )
        flags |= wxCONTROL_CURRENT;
    if ( m_label == wxEmptyString )
        flags |= wxCONTROL_POPUP_ARROW;
    if ( m_down )
        flags |= wxCONTROL_PRESSED;
    
    wxRendererNative_DrawButton (parent,dc,rect,flags);

    if ( m_label != wxEmptyString )
    {
        // text on button
        int tw, th;

        parent->GetTextExtent ( m_label, &tw, &th, NULL, NULL, &ctrl->GetFont() );

        dc.SetFont ( ctrl->GetFont() );
        dc.SetTextForeground ( man->GetButtonTextColour() );

        dc.DrawText ( m_label,
            rect.x + (rect.width - tw) / 2,
            rect.y + (rect.width - ctrl->m_fontHeight) / 2
            );
    }
    /*
    else
    {
        // arrow on button
        dc.SetBrush ( man->GetButtonTextColour() );
        dc.SetPen ( man->GetButtonTextColour() );

        int rect_mid = rect.width / 2;
        int arw_wid = rect.width/5;
        int arw_top_y = (rect.height/2) - (arw_wid/2);

        dropdown_arrow_points[0].x = rect_mid - arw_wid;
        dropdown_arrow_points[0].y = arw_top_y;
        dropdown_arrow_points[1].x = rect_mid + arw_wid; //rect.width-(rect.width/3);
        dropdown_arrow_points[1].y = arw_top_y;
        dropdown_arrow_points[2].x = rect_mid;
        dropdown_arrow_points[2].y = arw_top_y + arw_wid;

        dc.DrawPolygon ( 3, dropdown_arrow_points, rect.x, rect.y );

    }
    */
}

// -----------------------------------------------------------------------

bool wxCCustomButtonHandler::OnMouseEvent ( wxMouseEvent& event )
{
    wxCustomControl* ctrl = m_control;
    //const wxRect& rect = m_rect;

    //if ( !IsMouseFocused() ) return FALSE;

    wxCustomControlManager* manager = ctrl->m_manager;
    if ( event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK ||
         ( event.GetEventType() == wxEVT_ENTER_WINDOW && manager->IsDragging() ) )
    {
        if ( !m_down )
        {
            m_down = 1;
            DRAW_CTRL_IN_DATA_FUNC(manager)

            // Only fire event on mousedown if on a dropdown button.
            if ( !m_label.length() )
            {
                manager->AddEvent ( ctrl, wxEVT_COMMAND_BUTTON_CLICKED );
                return TRUE;
            }
        }
    }
    else if ( event.GetEventType() == wxEVT_LEFT_UP ||
         ( event.GetEventType() == wxEVT_LEAVE_WINDOW && manager->IsDragging() ) )
    {
        if ( m_down )
        {
            m_down = 0;
            DRAW_CTRL_IN_DATA_FUNC(manager)

            // Only fire event on mouseup if not a dropdown button.
            if ( event.GetEventType() == wxEVT_LEFT_UP && m_label.length() )
            {
                // If popup, close it now.
                if ( manager->GetPopup() ) manager->ClosePopup();

                manager->AddEvent ( ctrl, wxEVT_COMMAND_BUTTON_CLICKED );
                return TRUE;
            }
        }
    }
    else if ( event.GetEventType() == wxEVT_ENTER_WINDOW ||
              event.GetEventType() == wxEVT_LEAVE_WINDOW
            )
    {
        DRAW_CTRL_IN_DATA_FUNC(manager)
    }

    return FALSE;
}

// -----------------------------------------------------------------------
// wxCustomComboItem
// -----------------------------------------------------------------------

wxCustomComboItem::wxCustomComboItem()
{
}

// -----------------------------------------------------------------------

wxCustomComboItem::~wxCustomComboItem()
{
}

// -----------------------------------------------------------------------
// wxCustomComboListItem
// -----------------------------------------------------------------------

wxCustomComboListItem::wxCustomComboListItem()
{
}

// -----------------------------------------------------------------------

wxCustomComboListItem::~wxCustomComboListItem()
{
}

// -----------------------------------------------------------------------
// wxCustomComboPopup
// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxCustomComboPopup, wxCustomComboPopupBase)
    EVT_ENTER_WINDOW(wxCustomComboPopup::OnMouseEntry)
END_EVENT_TABLE()

void wxCustomComboPopup::ForcedClose ()
{
    //m_chData->OnSelect ( m_control, -2 );
    m_chData->m_listInstance = NULL;
    Destroy();
}

// -----------------------------------------------------------------------

void wxCustomComboPopup::OnMouseEntry( wxMouseEvent& event )
{

    //wxLogDebug(wxT("wxCustomComboPopup::OnMouseEntry"));

    // Cause mouse move event just on top of the list
    // (so the button triggering the event knows mouse
    // is no longer over it).
    // TODO: Make this work even when mouse moves first over
    // the scrollbar.
    //if ( !m_entryStatus )
    //{
        wxPoint pt = ::wxGetMousePosition();
        m_manager->GetWindow()->ScreenToClient(&pt.x,&pt.y);

        wxMouseEvent evt;
        evt.SetEventType(wxEVT_MOTION);
        evt.m_x = pt.x;
        evt.m_y = pt.y;

        m_manager->ProcessMouseEvent(event);
    //}

    // Also set the correct cursor.
    SetCursor(*wxSTANDARD_CURSOR);
}

// -----------------------------------------------------------------------

wxCustomComboPopup::wxCustomComboPopup ()
    : wxCustomComboPopupBase()
{
    m_wheelSum = 0;
    //m_entryStatus = 0;
}

// -----------------------------------------------------------------------

bool wxCustomComboPopup::Create ( wxWindow* swparent, wxCCustomComboBoxHandler* data,
                                  const wxRect& ctrl_rect, wxCustomControl* ctrl,
                                  const wxSize& size, int sizealign
                                )
{
    m_chData = data;
    m_control = ctrl;
    m_manager = ctrl->GetManager();

    wxPoint pos(ctrl_rect.x+ctrl_rect.width-size.x, ctrl_rect.y+ctrl_rect.height);
    m_manager->TranslatePositionToPhysical ( &pos.x, &pos.y );
    m_orientation = 2;

    wxSize sz(size);

#if wxCC_USE_POPUPWIN
    int screen_height = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );

    pos = swparent->ClientToScreen(pos);

    int space_below = screen_height - pos.y;

#else

    wxSize cs = swparent->GetClientSize();

    int space_below = cs.y - pos.y;

#endif

    // Do we have enough space below?
    if ( space_below < (size.y + 3) )
    {
        // No... then choose side with most space.

        int space_above = pos.y - ctrl_rect.height;

        //wxLogDebug ( wxT("space_below = %i, space_above = %i, size = %i"),
        //    (int)space_below,(int)space_above,(int)sz.y);

        if ( space_below < space_above )
        {
            // Above
            if ( space_above < sz.y )
            {
                sz.y = space_above - 3;
                if ( sizealign > 1 )
                    sz.y = (sz.y / sizealign) * sizealign + 2;
            }

            pos.y = pos.y - ctrl_rect.height - sz.y;

            m_orientation = 1;
        }
        else
        {
            // Below.
            if ( space_below < sz.y )
            {
                sz.y = space_below - 3;
                if ( sizealign > 1 )
                    sz.y = (sz.y / sizealign) * sizealign + 2;
            }
        }

    }

    // Extend width to the right edge of parent?
    if ( (ctrl->GetWindowStyle() & wxCH_CC_DROPDOWN_ANCHOR_RIGHT) )
    {
        int amount = swparent->GetClientSize().x - (ctrl_rect.x+ctrl_rect.width);
        sz.x += amount;
    }

#if wxCC_USE_POPUPWIN
    bool res = wxPopupWindow::Create ( swparent, wxSIMPLE_BORDER|wxCLIP_CHILDREN );

    //wxLogDebug(wxT("%i,%i"),(int)pos.x,(int)pos.y);

    //sz.y = sz.y + 100;
    //pos.x -= sz.x;
    //pos.y -= sz.y;
    //wxPopupWindow::Position( pos, sz );

    SetSize(sz);
    SetPosition(pos);

#else
    bool res = wxWindow::Create ( swparent, 16028,
        pos, sz, wxCLIP_CHILDREN|wxSIMPLE_BORDER );
#endif

    if ( res )
    {
        // This seems to be necessary for wxGTK (and probably won't hurt other platforms either)
    # if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
        SetBackgroundColour ( m_manager->GetWindowColour() );
    #else
        SetOwnBackgroundColour ( m_manager->GetWindowColour() );
    #endif
    }

    return res;
}

// -----------------------------------------------------------------------

wxCustomComboPopup::~wxCustomComboPopup()
{
    m_manager->SetPopup ( NULL );
}

// -----------------------------------------------------------------------
// wxComboPopupDefaultList
// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboPopupDefaultList, wxCustomComboPopup)
  EVT_MOTION(wxComboPopupDefaultList::OnMouseMove)
  //EVT_LEFT_UP(wxComboPopupDefaultList::OnMouseUp)
  EVT_PAINT(wxComboPopupDefaultList::OnPaint)
  EVT_COMMAND_SCROLL(16029,wxComboPopupDefaultList::OnScrollEvent)
  EVT_LEFT_DOWN(wxComboPopupDefaultList::OnMouseDown)
  EVT_KEY_DOWN(wxComboPopupDefaultList::OnKeyEvent)
  EVT_MOUSEWHEEL(wxComboPopupDefaultList::OnMouseWheelEvent)
END_EVENT_TABLE()

#define wxCC_DCP_PIXELS_PER_UNIT        m_itemHeight

#define wxCC_DCP_RECOMMENDED_MAX_HEIGHT 220

// -----------------------------------------------------------------------

int wxComboPopupDefaultList::HitTest ( int y )
{
    int ty = y + m_viewStartY;
    int index = ty / m_itemHeight;

    if ( index >= (int)((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount() ||
         index < 0 )
        index = -1;

    return index;
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnScrollEvent ( wxScrollEvent& /*event*/ )
{
    int index = m_pScrollBar->GetThumbPosition();
    if ( index != m_viewStartIndex )
        SetViewStart(index,FALSE);
}

// -----------------------------------------------------------------------

// Scrolls to given item index.
void wxComboPopupDefaultList::SetViewStart ( int index, bool adjust_sb )
{
    if ( m_pScrollBar )
    {
        int scroll_max = m_pScrollBar->GetRange() - m_pScrollBar->GetThumbSize();

        if ( index < 0 )
            index = 0;
        else if ( index > scroll_max )
            index = scroll_max;

        if ( index != m_viewStartIndex )
        {
            m_viewStartIndex = index;
            m_viewStartY = index * m_itemHeight;

            if ( adjust_sb )
            {
                m_pScrollBar->SetThumbPosition(index);

                // Putting recheckhilighted here prevents hilighted
                // change when scroll position is adjusted using
                // the scrollbar.
                int y = ScreenToClient(::wxGetMousePosition()).y;
                if ( y >= 0 && y < GetClientSize().y )
                    RecheckHilighted ( y );
            }

            wxRect rect(0,0,m_clientWidth,GetSize().y);
            RefreshRect(rect);
        }
    }
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::RecheckHilighted ( int y )
{
    int prev = m_hilighted;
    int index = HitTest ( y );

    if ( prev != index )
    {
        m_hilighted = index;

        if ( prev != -1 )
            DrawItem ( prev );

        if ( index != -1 )
            DrawItem ( index );
    }
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnMouseMove ( wxMouseEvent& event )
{
    RecheckHilighted(event.m_y);
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnMouseDown ( wxMouseEvent& WXUNUSED(event) )
{
    if ( m_hilighted != -1 )
    {
        wxCustomComboListItem item(m_hilighted);
        ((wxCCustomComboBoxDefaultHandler*)m_chData)->OnSelect ( item );
    }
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnMouseWheelEvent ( wxMouseEvent& event )
{
    //wxLogDebug(wxT("wxComboPopupDefaultList::OnMouseWheelEvent(%i)"),(int)event.GetWheelRotation());
    m_wheelSum += event.GetWheelRotation();
    int delta = event.GetWheelDelta();

    int scrollpos = m_viewStartIndex;

    if ( m_wheelSum >= delta )
    {
        while ( m_wheelSum >= delta ) { scrollpos -=3; m_wheelSum -= delta; }
        if ( scrollpos < 0 ) scrollpos = 0;
        SetViewStart(scrollpos,TRUE);
    }
    else if ( m_wheelSum <= -delta )
    {
        while ( m_wheelSum <= -delta ) { scrollpos +=3; m_wheelSum += delta; }
        //int scroll_max =
        //    ((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount()-1;
        SetViewStart(scrollpos,TRUE);
    }
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnKeyEvent ( wxKeyEvent& event )
{
    //wxCCustomComboBox* cb = (wxCCustomComboBox*)m_control;

    int prev = m_hilighted;

    int keycode = event.GetKeyCode();

    if ( keycode == WXK_DOWN )
    {
        if ( m_hilighted < (int)(((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount()-1) )
        {
            m_hilighted++;

            wxASSERT ( m_hilighted >= 0 );

            if ( prev >= 0 ) DrawItem ( prev );
            ShowItem ( m_hilighted, TRUE );
            DrawItem ( m_hilighted );
        }
    }
    else if ( keycode == WXK_UP )
    {
        if ( m_hilighted > 0 )
        {
            m_hilighted--;

            wxASSERT ( m_hilighted < (int)((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount() );

            if ( prev >= 0 ) DrawItem ( prev );
            ShowItem ( m_hilighted, TRUE );
            DrawItem ( m_hilighted );
        }
    }
    else if ( keycode == WXK_RETURN )
    {
        if ( m_hilighted != -1 )
            m_chData->IntOnSelect ( m_hilighted );
    }
    else if ( keycode == WXK_ESCAPE )
    {
        m_chData->IntOnSelect ( -2 );
    }
}

// -----------------------------------------------------------------------

// scrolls enough that the given item is visible
void wxComboPopupDefaultList::ShowItem ( const wxCustomComboItem& item, bool kbscroll )
{
    int index = ((wxCustomComboListItem&)item).m_index;

    int y_top = index*m_itemHeight;
    int y_bottom = y_top+m_itemHeight;

    int cw, ch;
    int vy = m_viewStartY;

    GetClientSize ( &cw, &ch );

    if ( y_bottom > (vy+ch) )
    {
        // Scroll down.
        if ( !kbscroll )
            SetViewStart ( index, TRUE );
        else
            SetViewStart ( (index-(ch/wxCC_DCP_PIXELS_PER_UNIT) ) + 1, TRUE );
    }
    else if ( y_top < vy )
    {
        // Scroll up.
        SetViewStart ( index, TRUE );
    }
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::DrawItem ( const wxCustomComboItem& item )
{
    int index = ((wxCustomComboListItem&)item).m_index;

    if ( index >= (int)((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount() )
        return;

    wxClientDC dc(this);

    unsigned int itemheight = (unsigned int)m_itemHeight;

    wxRect subrect(0,(itemheight*(int)index)-m_viewStartY,m_clientWidth,itemheight);

    if ( (int)index != m_hilighted /*|| !fully_visible*/ )
    {
        dc.SetBrush ( m_manager->GetWindowColour() );
        dc.SetPen ( m_manager->GetWindowColour() );
        dc.SetTextForeground ( m_manager->GetWindowTextColour() );
    }
    else
    {
        dc.SetBrush ( m_manager->GetSelectionColour() );
        dc.SetPen ( m_manager->GetSelectionColour() );
        dc.SetTextForeground ( m_manager->GetSelectionTextColour() );
    }
    dc.DrawRectangle ( subrect ); // clear background in one sweep

    dc.SetFont ( m_control->GetFont() );

    DrawItem (dc,subrect,index);

}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::DrawItem ( wxDC& dc, wxRect& rect, unsigned int index )
{
    wxCustomPaintFunc paintfunc = m_chData->GetPaintFunc();
    
    int imagewidth = m_chData->GetImageSize().x;
    int iwpm = 0;

    dc.SetClippingRegion(rect);

    if ( (int)index == m_hilighted )
    {
        dc.SetBrush ( m_manager->GetSelectionColour() );
        dc.SetPen ( m_manager->GetSelectionColour() );
        dc.DrawRectangle ( rect );
        dc.SetBrush ( m_manager->GetWindowColour() );
        dc.SetPen ( m_manager->GetWindowColour() );
        dc.SetTextForeground ( m_manager->GetSelectionTextColour() );
    }

    if ( imagewidth >= 1 )
    {
        wxRect imagerect(rect.x+wxCC_CUSTOM_IMAGE_MARGIN1,rect.y+1,imagewidth,rect.height-2);
        dc.SetPen ( m_manager->GetWindowTextColour() );
        paintfunc ( dc, imagerect, (int)index, m_chData->GetPaintFuncCustomData() );
        dc.SetPen ( m_manager->GetWindowColour() );
        dc.SetPen ( m_manager->GetWindowTextColour() );
        wxSize pntSz = paintfunc ( dc, imagerect, (int)index,
            m_chData->GetPaintFuncCustomData() );
        dc.SetPen ( m_manager->GetWindowColour() );
        iwpm = pntSz.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
        rect.x += iwpm;
        rect.width -= iwpm;
    }

    wxPoint pt( rect.x, rect.y );
    if ( imagewidth >= 1 )
        pt.x -= 1; // -1 is for border
    else
        pt.x += wxCC_TEXTCTRL_XSPACING;
    pt.y += (rect.height-m_control->GetFontHeight())/2;

    wxString& str = m_manager->GetTempString();
    str = (wxChar*) ((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().Item(index);
    dc.DrawText ( str, pt.x, pt.y );

    if ( (int)index == m_hilighted )
    {
        dc.SetTextForeground ( m_manager->GetWindowTextColour() );
    }

    if ( imagewidth >= 1 )
    {
        rect.x -= iwpm;
        rect.width += iwpm;
    }

    dc.DestroyClippingRegion();
}

// -----------------------------------------------------------------------

void wxComboPopupDefaultList::OnPaint ( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

    //wxLogDebug(wxT("wxComboPopupDefaultList::OnPaint"));

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();
    //r.y += m_viewStartY;

    unsigned int itemheight = (unsigned int)m_itemHeight;

    // first to draw
    unsigned int index = (unsigned int)(r.y / itemheight) + m_viewStartIndex;
    unsigned int last_index = (unsigned int)((r.y+r.height+itemheight) / itemheight)
         + m_viewStartIndex;

    if ( last_index > ((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount() )
        last_index = ((wxCCustomComboBoxDefaultHandler*)m_chData)->GetLabels().GetCount();

    wxRect subrect(0,(itemheight*(int)index)-m_viewStartY,m_clientWidth,itemheight);

    dc.SetBrush ( m_manager->GetWindowColour() );
    dc.SetPen ( m_manager->GetWindowColour() );
    dc.DrawRectangle ( r ); // clear background in one sweep

    dc.SetTextForeground ( m_manager->GetWindowTextColour() );
    dc.SetFont ( m_control->GetFont() );

    while ( index < last_index )
    {
        DrawItem (dc,subrect,index);
        index++;
        subrect.y += itemheight;
    }

}

// -----------------------------------------------------------------------

wxComboPopupDefaultList::wxComboPopupDefaultList ( wxWindow* frame,
        wxCCustomComboBoxHandler* data,
        const wxRect& rect,
        wxCustomControl* ctrl )
        : wxCustomComboPopup()
{

    //wxWindow* parent = ctrl->GetParent();

    wxSize sz;

    size_t i;

    // find the longest item
    wxChar* longest_item = NULL;
    size_t longest_string_len = 0;
    for ( i = 0; i < ((wxCCustomComboBoxDefaultHandler*)data)->GetLabels().GetCount(); i++ )
    {
        unsigned int sl = wxStrlen ( (wxChar*)((wxCCustomComboBoxDefaultHandler*)data)->GetLabels().Item(i) );
        if ( sl > longest_string_len )
        {
            longest_string_len = sl;
            longest_item = (wxChar*)((wxCCustomComboBoxDefaultHandler*)data)->GetLabels().Item(i);
        }
    }

    int lw, ih;

    frame->GetTextExtent ( longest_item, &lw, &ih, NULL, NULL, &ctrl->GetFont() );

    lw += 5; // just to be on the safe side
    ih = ctrl->GetFontHeight() + 1;

    int imagewidth = data->GetImageSize().x;

    if ( imagewidth >= 2 )
    {
        lw += imagewidth + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 - 2; // last is for borders
        if ( data->GetImageSize().y >= 0 && (ih < (data->GetImageSize().y+1)) )
            ih = (data->GetImageSize().y+1);
    }
    else
        imagewidth = 0;

    sz.x = lw;
    int itemcount = ((wxCCustomComboBoxDefaultHandler*)data)->GetLabels().GetCount();
    int virtual_height = (itemcount * ih) + 3;
    sz.y = virtual_height;

    // Increase recommended max height according to line height.
    int rmh1 = wxCC_DCP_RECOMMENDED_MAX_HEIGHT;
    if ( ih > 32 )
        rmh1 *= (ih/32);

    int recom_max_height = ((rmh1/ih)*ih)+3;

    bool need_scrollbar = FALSE;
    if ( sz.y > recom_max_height )
    {
        sz.y = recom_max_height;
        need_scrollbar = TRUE;
        sz.x += wxSystemSettings::GetMetric ( wxSYS_VSCROLL_X );
    }

    int min_width = rect.width;

    // Extend to cover non-integrated custom image in front of the control?
    if ( (ctrl->GetWindowStyle() & wxCH_CC_IMAGE_EXTENDS) && imagewidth )
        min_width += imagewidth + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;

    if ( sz.x < min_width )
        sz.x = min_width;

    // If no items, show an emty list of "some" height;
    if ( !itemcount )
        sz.y = 80;

    wxCustomComboPopup::Create ( frame, data, rect, ctrl, sz, ih );

    sz = GetClientSize();

    m_hilighted = ((wxCCustomComboBoxDefaultHandler*)data)->GetSelection();

    m_itemHeight = ih;

    // Create scrollbar.
    if ( need_scrollbar )
    {
        int sb_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
        if ( sb_width >= rect.height )
            sb_width = rect.height - 1;
        m_pScrollBar = new wxScrollBar ( this, 16029,
            wxPoint(sz.x-sb_width,0), wxSize(sb_width,sz.y), wxSB_VERTICAL );

        // Connect to mouse entry.
#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 4 )
        Connect( 16029, wxEVT_ENTER_WINDOW,
            (wxObjectEventFunction)&wxComboPopupDefaultList::OnMouseEntry );
#else
        Connect( 16029, wxEVT_ENTER_WINDOW,
            (wxObjectEventFunction)(wxMouseEventFunction)&wxComboPopupDefaultList::OnMouseEntry );
#endif

        int items_fit = sz.y/ih;
        m_pScrollBar->SetScrollbar(0,items_fit,itemcount,items_fit,FALSE);

        m_sbWidth = m_pScrollBar->GetSize().x;
        m_clientWidth = sz.x - m_sbWidth;

        // Verify scrollbar position.
        int correct_sb_x = sz.x - m_sbWidth;
        if ( m_pScrollBar->GetPosition().x != correct_sb_x )
            m_pScrollBar->Move(correct_sb_x,0);

    }
    else
    {
        m_pScrollBar = (wxScrollBar*) NULL;
        m_clientWidth = sz.x;
    }
    m_viewStartY = 0;
    m_viewStartIndex = 0;

    // Scroll to selection
    if ( m_hilighted != -1 )
    {
        //m_viewStartIndex = -1; // This allows correct refresh.
        ShowItem ( m_hilighted );
    }

#if wxCC_USE_POPUPWIN
    Show(TRUE);
#endif
}

// -----------------------------------------------------------------------

wxComboPopupDefaultList::~wxComboPopupDefaultList()
{
}

// -----------------------------------------------------------------------
// wxCCustomComboBoxHandler
// -----------------------------------------------------------------------

void wxCCustomComboBoxHandler::Draw ( wxDC& dc, const wxRect& rect, bool item_too )
{
    wxCustomControl* ctrl = m_control;

    wxRect r(rect);

    wxCustomControlManager* man = ctrl->GetManager();

    // Calculate button size

    int button_width = ( ctrl->GetFontHeight() * wxCC_DROPDOWN_BUTTON_WIDTH )
        / wxCC_DROPDOWN_BUTTON_WIDTH_WITH_FONT_HEIGHT;

        /*
           button_width            wxCC_DROPDOWN_BUTTON_WIDTH
          -------------- = ---------------------------------------------
            font_height     wxCC_DROPDOWN_BUTTON_WIDTH_WITH_FONT_HEIGHT
        */

    if ( !(button_width & 0x01) ) button_width++; // button width must be odd
    //wxLogDebug ( wxT("button_width=%i"),button_width);
    if ( button_width > wxPG_DROPDOWN_BUTTON_MAX_WIDTH  )
        button_width = wxPG_DROPDOWN_BUTTON_MAX_WIDTH;
    m_buttonWidth = button_width;

    // item
    if ( item_too )
    {
        dc.SetFont ( ctrl->GetFont() );

        r.width -= button_width;

        const wxChar* text_ptr = m_text.c_str();

        int flags = 1;
        
        // Mark focused (only if no visible list instance).
        if ( ctrl == man->GetFocused() && !m_listInstance )
        {
            flags |= 2;
        }

        man->CtrlWriteText ( dc, text_ptr,
            r, ctrl, flags );

        /*
        // Mark focused.
        if ( ctrl == man->GetFocused() )
        {
            // TODO: Make wxDOT a single-pixel dot.
            dc.SetPen ( wxPen( man->GetWindowTextColour(), 1, wxDOT ) );
            dc.SetBrush ( *wxTRANSPARENT_BRUSH );
            wxRect r2(r.x,r.y,r.width,r.height);
            dc.DrawRectangle ( r2 );
        }
        */
    }

    // button
    r.width = button_width;
    r.x += (rect.width - button_width);
    m_btData.Draw (dc,r);

}

// -----------------------------------------------------------------------

bool wxCCustomComboBoxHandler::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata )
{
    wxCustomControl* ctrl = m_control;
    const wxRect& rect = m_rect;

    //if ( !IsMouseFocused() ) return FALSE;

    int but_x = (rect.width-m_buttonWidth);

    if ( pdata == &m_btData )
    {
        if ( !event.Dragging() || m_prevMouseFocus != 1 )
        {
            wxRect r(rect);
            r.x += but_x;
            r.width = m_buttonWidth;
            bool res = m_btData.OnMouseEvent ( event );
            if ( res )
            {
                wxCustomControlManager* manager = ctrl->GetManager();
                if ( !m_listInstance )
                {
                    wxWindow* swparent = manager->CreatePopupParent();
                    m_listInstance = CreatePopup(swparent,ctrl->GetRect(),ctrl);
                    manager->SetPopup ( (wxCustomComboPopup*)m_listInstance );
                    // Redraw to hide "focused" blue background.
                    ctrl->Draw();
                }
                else
                {
                    manager->ClosePopup();
                    m_listInstance = NULL;
                    ctrl->Draw(); // Redraw to regain "focused" blue background.
                }
                manager->GetEvent(); // Mark event as processed.
            }
            m_prevMouseFocus = 2;
        }
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxCCustomComboBoxHandler::Create ( wxCustomControl* pctrl, const wxString& value,
    const wxPoint& pos, const wxSize& sz )
{
    m_prevMouseFocus = 0;
    m_listInstance = NULL;
    m_text = value;

    // Initial button width
    int button_width = ( pctrl->GetFontHeight() * wxCC_DROPDOWN_BUTTON_WIDTH )
        / wxCC_DROPDOWN_BUTTON_WIDTH_WITH_FONT_HEIGHT;
    if ( !(button_width & 0x01) ) button_width++; // button width must be odd
    m_buttonWidth = button_width;

    wxCustomControlHandler::Create ( pctrl, wxDefaultPosition, wxDefaultSize );

    m_rect.width -= button_width;

    m_btData.Create ( pctrl, wxDefaultPosition, wxDefaultSize );
    m_btData.SetButtonState(0);

    // correct positions
    Move ( pos.x, pos.y );
    SetSize ( sz.x, sz.y );
}

    
// -----------------------------------------------------------------------

wxCCustomComboBoxHandler::~wxCCustomComboBoxHandler()
{
    if ( m_listInstance )
        ((wxCustomComboPopup*)m_listInstance)->GetControl()->GetManager()->ClosePopup();

}

// -----------------------------------------------------------------------
// wxCCustomComboBoxDefaultHandler
// -----------------------------------------------------------------------

bool wxCCustomComboBoxDefaultHandler::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata )
{
    wxCustomControl* ctrl = m_control;

    if ( pdata != &m_btData )
    {
        if ( !event.Dragging() || m_prevMouseFocus != 2 )
        {
            if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
            {
                if ( ctrl->GetWindowStyle() & wxCH_CC_DOUBLE_CLICK_CYCLES )
                {
                    // Select next (or first if there wasn't any selection)
                    if ( m_labels.GetCount() )
                    {
                        int ind = m_selection;
                        ind += 1;
                        if ( ind >= (int)m_labels.GetCount() )
                            ind = 0;
                        wxCustomComboListItem item(ind);
                        OnSelect ( item );
                    }
                    return FALSE;
                }
            }
            m_prevMouseFocus = 1;
        }
    }
    return wxCCustomComboBoxHandler::OnMouseEvent(event,pdata);
}

// -----------------------------------------------------------------------

// Called when popup window wants to finish itself after a new selection
void wxCCustomComboBoxDefaultHandler::OnSelect ( const wxCustomComboItem& item )
{
    int index = ((wxCustomComboListItem&)item).m_index;

    wxCustomControl* ctrl = m_control;

    // Send CHOICE_SELECTED event ( -2 or lower value signals only closing of the popup)
    if ( index >= -1 )
    {
        m_selection = index;

        if ( index >= 0 )
            m_text = (const wxChar*)m_labels.Item(index);
        else
            m_text = wxEmptyString;

        wxCommandEvent& event = ctrl->GetEvent();

        wxWindow* parent = ctrl->GetParent ();

        event.SetEventObject ( parent );
        event.SetEventType ( wxEVT_COMMAND_CHOICE_SELECTED );
        event.m_id = parent->GetId();

        parent->AddPendingEvent ( event );

        // Redraw the control
        ctrl->Draw();
    }

    // Destroy the popup
    if ( m_listInstance )
    {
        ctrl->GetManager()->ClosePopup();
        m_listInstance = NULL;
        ctrl->Draw(); // Must redraw to regain correct background.
    }
}

// -----------------------------------------------------------------------

void wxCCustomComboBoxDefaultHandler::SetSelection ( const wxCustomComboItem& item )
{
    int index = ((wxCustomComboListItem&)item).m_index;

    if ( index >= -1 && index < (int)m_labels.GetCount() && index != m_selection )
    {
        if ( index >= 0 )
            m_text = (const wxChar*)m_labels.Item(index);
        else
            m_text = wxT("");
    }

    //wxLogDebug ( wxT("SetSelection: %s"), m_text.c_str() );

    if ( index < (int)m_labels.GetCount() )
        m_selection = index;

    if ( m_listInstance && index >= -1 )
        ((wxCustomComboPopup*)m_listInstance)->ShowItem ( item );
}

// -----------------------------------------------------------------------

void wxCCustomComboBoxDefaultHandler::Create ( wxCustomControl* pctrl, const wxString& value,
    const wxPoint& pos, const wxSize& sz,
    int n, const wxChar* choices[] )
{
    wxCCustomComboBoxHandler::Create(pctrl,value,pos,sz);

    m_selection = -1;

    // Fill label array.
    m_labels.Empty();

    size_t i;
    for ( i = 0; i < (size_t)n; i++ )
    {
        m_labels.Add ( (void*) choices[i] );
    }
}

// -----------------------------------------------------------------------

bool wxCCustomComboBoxDefaultHandler::OnKeyEvent ( wxKeyEvent& event )
{
    if ( m_labels.GetCount() )
    {
        if ( !m_listInstance )
        {
            int keycode = event.GetKeyCode();
            if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
            {
                int ind = m_selection + 1;
                if ( ind >= (int)m_labels.GetCount() ) ind = 0;
                IntOnSelect ( ind );
            }
            else if ( keycode == WXK_UP || keycode == WXK_LEFT )
            {
                int ind = m_selection - 1;
                if ( ind < 0 ) ind = m_labels.GetCount() - 1;
                IntOnSelect ( ind );
            }
            else if ( keycode == WXK_PRIOR )
            {
                IntOnSelect ( 0 );
            }
            else if ( keycode == WXK_NEXT )
            {
                IntOnSelect ( m_labels.GetCount() - 1 );
            }
            else
            {
                return FALSE;
            }
            return TRUE;
        }
        else
        {
            m_listInstance->OnKeyEvent(event);
            return TRUE;
        }
    }
    return FALSE;
}

// -----------------------------------------------------------------------

int wxCCustomComboBoxDefaultHandler::Append( const wxString& str )
{
    m_extraStrings.Add(str);
    m_labels.Add((void*)str.c_str());
    return m_labels.GetCount()-1;
}

// -----------------------------------------------------------------------

wxCustomComboPopup* wxCCustomComboBoxDefaultHandler::CreatePopup ( wxWindow* frame,
                                                               const wxRect& ctrl_rect,
                                                               wxCustomControl* ctrl )
{
    return new wxComboPopupDefaultList ( frame, this, ctrl_rect, ctrl );
}

// -----------------------------------------------------------------------

wxCCustomComboBoxDefaultHandler::~wxCCustomComboBoxDefaultHandler()
{
}

// -----------------------------------------------------------------------
// wxCustomControlManager
// -----------------------------------------------------------------------

wxCustomControl* wxCustomControlManager::FindWindowByPosition ( int x, int y )
{
    wxCustomControlHandler* pdata = FindDataByPosition ( x, y );
    
    if ( !pdata )
        return (wxCustomControl*) NULL;

    return pdata->GetControl();
}

// -----------------------------------------------------------------------

wxCustomControlHandler* wxCustomControlManager::FindDataByPosition ( int x, int y )
{
    size_t i;

    for ( i = 0; i < m_handlers.GetCount(); i++ )
    {
        wxCustomControlHandler* cc = (wxCustomControlHandler*) m_handlers.Item ( i );
        const wxRect& rect = cc->GetRect();
        if ( x >= rect.x && y >= rect.y &&
             x < rect.x + rect.width && y < rect.y+rect.height
           )
           return cc;
    }

    return NULL;
}

// -----------------------------------------------------------------------

// returns TRUE if event was inside some ctrl (some events may actually be processed even in this case).
bool wxCustomControlManager::ProcessMouseEvent ( wxMouseEvent& event )
{
    //wxLogDebug ( wxT("wxCustomControlManager::ProcessMouseEvent") );

#if wxCC_CORRECT_CONTROL_POSITION
    if ( m_flags & wxCCM_FL_VIEWSTARTCHANGED )
        OnParentScrollChange ();
#else
    // convert coordinates
    TranslatePositionToLogical ( &event.m_x, &event.m_y );
#endif

    wxCustomControlHandler* data = FindDataByPosition ( event.m_x, event.m_y );

    //wxLogDebug ( wxT("0x%X"), data );

    m_flags &= ~(wxCCM_FL_EVENTREADY);

    // mouse leaves?
    if ( data != m_dataMouseFocus )
    {
        DoMouseLeave ( event, data, FALSE );
    }

    //wxLogDebug ( wxT("wxCustomControlManager::ProcessMouseEvent(wxEVT_MOTION)") );

    // Send un-focused event.
    wxCustomControl* ctrl;
    
    bool mouse_focused = TRUE;

    if ( m_dataMouseFocus && data != m_dataMouseFocus )
    {
        ctrl = m_dataMouseFocus->GetControl();
        mouse_focused = FALSE;

        /*
        event.m_x -= ctrl->m_rect.x;
        event.m_y -= ctrl->m_rect.y;

        // relay event
        bool ev = m_ctrlMouseFocus->OnMouseEvent ( event, data );
        if ( ev )
            m_flags |= wxCCM_FL_EVENTREADY;
        return TRUE;
        */
    }
    else if ( data )
    {
        ctrl = data->GetControl();
    }
    else
        return FALSE;

    wxASSERT ( ctrl );

    if ( mouse_focused )
    {
        if ( !m_dataMouseFocus )
        {
        #if __MOUSE_DEBUGGING__
            wxLogDebug (wxT("CCM: Mouse focuses 0x%X"),(unsigned int)ctrl );
        #endif
            //wxLogDebug ( wxT("CC: Mouse Focuses") );
            ctrl->OnMouseFocus ( TRUE );
            m_dataMouseFocus = data;
            m_ctrlMouseFocus = ctrl;
            m_flags &= ~(wxCCM_FL_MOUSE_INSIDE);
        }

        wxASSERT ( m_ctrlMouseFocus != NULL );

        // When entering window, send Enter event.
        if ( !(m_flags & wxCCM_FL_MOUSE_INSIDE ) )
        {
            int old_type = event.GetEventType();
            event.SetEventType(wxEVT_ENTER_WINDOW);
        #if __MOUSE_DEBUGGING__
            wxLogDebug (wxT("CCM: Entering data 0x%X"),(unsigned int)m_dataMouseFocus );
        #endif
            m_ctrlMouseFocus->m_flags |= wxCC_FL_MOUSEFOCUS;
            m_dataMouseFocus->SetFlag ( wxCC_FL_MOUSEFOCUS );
            ctrl->OnMouseEvent ( event, data );
            event.SetEventType(old_type);
            m_flags |= wxCCM_FL_MOUSE_INSIDE;
        }

        // Dragging management
        if ( event.GetEventType() == wxEVT_LEFT_DOWN )
        {
            StartDragging();
            
            // Must focus now
            if ( m_ctrlKbFocus != ctrl )
                ctrl->SetFocus ();

        }
    }

    if ( event.GetEventType() == wxEVT_LEFT_UP )
        StopDragging();

    event.m_x -= ctrl->m_rect.x;
    event.m_y -= ctrl->m_rect.y;

    // relay event
    bool ev = m_ctrlMouseFocus->OnMouseEvent ( event, data );
    if ( ev )
        m_flags |= wxCCM_FL_EVENTREADY;

    return TRUE;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::DoMouseLeave ( wxMouseEvent& event, wxCustomControlHandler* newdata, bool force )
{
    if ( m_dataMouseFocus )
    {
        int evt_type = event.GetEventType();

        // When leaving window, send Leave event.
        if ( m_flags & wxCCM_FL_MOUSE_INSIDE ) 
        {
            wxASSERT ( m_ctrlMouseFocus != NULL );

            event.SetEventType(wxEVT_LEAVE_WINDOW);
            m_ctrlMouseFocus->m_flags &= ~(wxCC_FL_MOUSEFOCUS);
            m_dataMouseFocus->ClearFlag ( wxCC_FL_MOUSEFOCUS );
        #if __MOUSE_DEBUGGING__
            wxLogDebug (wxT("CCM: Leaving data 0x%X"),(unsigned int)m_dataMouseFocus );
        #endif
            m_ctrlMouseFocus->OnMouseEvent ( event, m_dataMouseFocus );
            m_flags &= ~(wxCCM_FL_MOUSE_INSIDE);
            event.SetEventType(evt_type); // must repair the event
        }

        if ( ( evt_type == wxEVT_MOTION && !IsDragging() )
               || evt_type == wxEVT_LEFT_UP || force
           )
        {
            //wxLogDebug ( wxT("  EVT_MOTION: %i"),(int)(evt_type==wxEVT_MOTION)?1:0);
            //wxLogDebug ( wxT("  EVT_LEFT_UP: %i"),(int)(evt_type==wxEVT_LEFT_UP)?1:0);
            //wxLogDebug ( wxT("  Dragging: %i"),(int)(IsDragging())?1:0);
            //wxLogDebug ( wxT("  LeftDown: %i"),(int)(event.LeftDown())?1:0);
            //wxLogDebug ( wxT("  force: %i"),(int)(force)?1:0);

            //wxLogDebug ( wxT("CC: Mouse Leaves"));

            m_flags &= ~(wxCCM_FL_DRAGGING);

            m_ctrlMouseFocus->OnMouseFocus ( FALSE );

            if ( !newdata )
            {
                // wxCustomControl* newctrl = newdata->GetControl();
                // If parent had custom cursor set, this would screw it up
                // - instead, parent must itself detect when mouse leaves
                // and then set the cursor back. wxPropertyGrid does this
                // by setting m_curcursor to invalid value when custom
                // control mouse events occur, and this causes correct
                // cursor to be switched when back in non-control events.
                //m_parent->SetCursor ( wxNullCursor );
            }

            m_ctrlMouseFocus = NULL;
            m_dataMouseFocus = NULL;
        }
    }
}

// -----------------------------------------------------------------------

bool wxCustomControlManager::ProcessKeyboardEvent ( wxKeyEvent& event )
{
    if ( m_ctrlKbFocus )
    {

        m_flags &= ~(wxCCM_FL_EVENTREADY);

        return m_ctrlKbFocus->OnKeyEvent ( event );

        //if ( ev )
        //    ;

        //return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::AddEvent ( wxCustomControl* child, int eventtype )
{
    m_event.SetEventObject ( (wxObject*)child );
    m_event.SetEventType ( eventtype );
    m_flags |= wxCCM_FL_EVENTREADY;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::SetFocus ( wxCustomControl* child )
{
    if ( child == m_ctrlKbFocus )
        return;

    HideCaret();

    if ( child->m_flags & wxCC_FL_NEEDSCARET )
    {
        // Do not show it yet
        m_pCaret->SetSize ( 1, child->m_fontHeight );
    }

    child->m_flags |= wxCC_FL_KBFOCUS;

    m_ctrlKbFocus = child;

    child->OnFocus( TRUE );

    // Redraw it
    if ( child->m_flags & wxCC_FL_DRAWN )
        child->Draw();
}

// -----------------------------------------------------------------------

void wxCustomControlManager::RemoveFocus ( wxCustomControl* child )
{
    if ( child != m_ctrlKbFocus )
        return;

    child->OnFocus( FALSE );

    HideCaret();

#if defined(__WXGTK__)
    child->Draw();
#endif
    
    child->m_flags &= ~(wxCC_FL_KBFOCUS);

    m_ctrlKbFocus = NULL;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::CtrlWriteText ( wxDC& dc, const wxChar* text,
    const wxRect& rect, wxCustomControl* ctrl, int flags )
{
    dc.DestroyClippingRegion ();
    dc.SetClippingRegion ( rect );

#if defined(__WXMSW__) || defined(__WXGTK__)
    if ( flags & 1 )
    {
        const wxColour& win_col = GetWindowColour();
        
        dc.SetBrush ( win_col );
        dc.SetPen ( win_col );
        dc.DrawRectangle ( rect );

        if ( flags & 2 )
        {
            const wxColour& sel_col = GetSelectionColour();
            dc.SetBrush ( sel_col );
            dc.SetPen ( sel_col );
            wxRect r = rect;
            r.Deflate(1);
            dc.DrawRectangle ( r );
        }
    }
#else
// Default.

    if ( flags & 1 )
    {
        if ( flags & 2 )
        {
            dc.SetBrush ( GetSelectionColour() );
            dc.SetPen ( GetSelectionColour() );
        }
        else
        {
            dc.SetBrush ( GetWindowColour() );
            dc.SetPen ( GetWindowColour() );
        }
        dc.DrawRectangle ( rect );
    }
#endif

    if ( flags & 2 )
        dc.SetTextForeground ( GetSelectionTextColour() );
    else
        dc.SetTextForeground ( GetWindowTextColour() );

    wxPoint pt( rect.x + wxCC_TEXTCTRL_XSPACING, rect.y );
    pt.y += (rect.height-ctrl->m_fontHeight)/2;

    m_tempStr1 = text;
    dc.DrawText ( m_tempStr1, pt.x, pt.y );

    dc.DestroyClippingRegion ();
}

// -----------------------------------------------------------------------

// returns index to first data of a control
int wxCustomControlManager::GetControlsFirstData ( wxCustomControl* ctrl )
{
    unsigned int i;
    int index = -1;
    for ( i = 0; i < m_handlers.GetCount(); i++ )
    {
        if ( ((wxCustomControlHandler*)m_handlers.Item(i))->GetControl() == ctrl )
        {
            index = (int)i;
            break;
        }
    }
    //wxASSERT_MSG ( index >= 0, wxT("All controls should have item(s) in m_handlers array.") );
    return index;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::AddChildData ( wxCustomControlHandler* pdata )
{
    m_handlers.Add ( (void*) pdata );
}

// -----------------------------------------------------------------------

void wxCustomControlManager::AddChild ( wxCustomControl* /*child*/ )
{
#if wxCC_CORRECT_CONTROL_POSITION
    if ( m_flags & wxCCM_FL_VIEWSTARTCHANGED )
        OnParentScrollChange ();
#endif

    //m_controls.Add ( (void*) child );
}

// -----------------------------------------------------------------------

void wxCustomControlManager::RemoveChild ( wxCustomControl* child )
{
    wxASSERT ( !(child->m_flags & wxCC_FL_REMOVED) );

    // remove mouse focus
    if ( child == m_ctrlMouseFocus )
    {
        m_ctrlMouseFocus = NULL;
        m_dataMouseFocus = NULL;
    }

    // remove keyboard focus
    if ( child == m_ctrlKbFocus )
    {
        m_ctrlKbFocus = NULL;

        HideCaret();
    }

    // remove relevant item(s) from data array
    int ind = GetControlsFirstData ( child );

    //wxASSERT ( index >= 0 );
    if ( ind >= 0 )
    {
        size_t index = ind;
        size_t orig_index = index;

        wxASSERT ( index < m_handlers.GetCount() );

        index++;

        while ( index < m_handlers.GetCount() &&
            ((wxCustomControlHandler*)m_handlers.Item(index))->GetControl() == child
            )
            index++;

        m_handlers.RemoveAt(orig_index,index-orig_index);
    }

    child->m_flags |= wxCC_FL_REMOVED;
}

// -----------------------------------------------------------------------

wxDC& wxCustomControlManager::CreateDC ( wxPoint* palignpt )
{
    wxWindow* parent = m_parent;
    wxASSERT ( parent != NULL );
    wxClientDC* new_dc = new wxClientDC(parent);

    palignpt->x = palignpt->y = 0;

    if ( m_flags & wxCCM_FL_SCROLLEDWIN )
    {
        ((wxScrolledWindow*)parent)->PrepareDC(*new_dc);
#if wxCC_CORRECT_CONTROL_POSITION
        TranslatePositionToLogical( &alignpt->x, &alignpt->y );
        /*int spx, spy;
        ((wxScrolledWindow*)parent)->GetViewStart(&palignpt->x,&palignpt->y);
        ((wxScrolledWindow*)parent)->GetScrollPixelsPerUnit(&spx,&spy);
        palignpt->x*=spx;
        palignpt->y*=spy;*/
#endif
    }

    HideCaret();

    return *new_dc;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::DestroyDC ( wxDC& dc )
{
    ShowCaret();

    delete &dc;
}
    
// -----------------------------------------------------------------------

void wxCustomControlManager::Create ( wxWindow* parent )
{
    m_parent = parent;

    m_bmpDoubleBuffer = NULL;

    m_ctrlMouseFocus = NULL;
    m_ctrlKbFocus = NULL;
    m_dataMouseFocus = NULL;

    m_flags = 0;

    if ( parent->IsKindOf(CLASSINFO(wxScrolledWindow)) )
        m_flags |= wxCCM_FL_SCROLLEDWIN;

    m_colBackground = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );
    //m_colBackground = wxColour ( 255, 192, 192 );
    m_colText = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOWTEXT );
    m_colSelection = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHT );
    m_colSelectionText = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHTTEXT );
    m_colButton = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE );
    m_colButtonText = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNTEXT );

    m_cursorIBeam = wxCursor ( wxCURSOR_IBEAM );

    m_font = parent->GetFont();

    wxASSERT ( parent->GetCaret() == NULL );

    //m_pCaret = NULL;
    // Readily create caret for our needs
    m_pCaret = new wxCaret ( m_parent, 1, 10 );

#if wxCC_CORRECT_CONTROL_POSITION
    m_prevViewStart.x = m_prevViewStart.y = 0;
#endif

    m_openPopup = NULL;

}
// -----------------------------------------------------------------------

void wxCustomControlManager::MoveCaret ( const wxRect& ctrl_rect, int x, int y )
{

    if ( m_pCaret )
    {
        m_caretRelativePos = wxPoint(x,y);

        x += ctrl_rect.x; y += ctrl_rect.y;
        //wxLogDebug ( wxT("wxCustomControlManager::MoveCaret( %i, %i )"),x,y);

        TranslatePositionToPhysical ( &x, &y );
        m_pCaret->Move ( x, y );

    }

}

// -----------------------------------------------------------------------

void wxCustomControlManager::RepositionCaret ()
{

    if ( m_pCaret && m_ctrlKbFocus )
    {

        //wxLogDebug ( wxT("wxCustomControlManager::MoveCaretRelative ( %i, %i )"),x,y);

        wxPoint cp = m_ctrlKbFocus->GetPosition() + m_caretRelativePos;
        //wxLogDebug ( wxT(" prev_x = %i, prev_y = %i"), cp.x, cp.y );
        TranslatePositionToPhysical ( &cp.x, &cp.y );
        //wxLogDebug ( wxT(" prev_x = %i, prev_y = %i"), cp.x, cp.y );
        m_pCaret->Move ( cp.x, cp.y );

    }

}

// -----------------------------------------------------------------------

/*void wxCustomControlManager::MoveCaretRelative ( int x, int y )
{
    
    if ( m_pCaret )
    {

        //wxLogDebug ( wxT("wxCustomControlManager::MoveCaretRelative ( %i, %i )"),x,y);

        wxPoint cp = m_pCaret->GetPosition();
        //wxLogDebug ( wxT(" prev_x = %i, prev_y = %i"), cp.x, cp.y );
        m_pCaret->Move ( cp.x + x, cp.y + y );

    }

}*/

// -----------------------------------------------------------------------

void wxCustomControlManager::TranslatePositionToPhysical ( int* x, int* y ) const
{
    if ( m_flags & wxCCM_FL_SCROLLEDWIN )
    {
        int vx, vy;
        int spx, spy;
        ((wxScrolledWindow*)m_parent)->GetViewStart(&vx,&vy);
        ((wxScrolledWindow*)m_parent)->GetScrollPixelsPerUnit(&spx,&spy);
        *x -= (vx*spx);
        *y -= (vy*spy);
    }
}

// -----------------------------------------------------------------------

void wxCustomControlManager::TranslatePositionToLogical ( int* x, int* y ) const
{
    if ( m_flags & wxCCM_FL_SCROLLEDWIN )
    {
        int vx, vy;
        int spx, spy;
        ((wxScrolledWindow*)m_parent)->GetViewStart(&vx,&vy);
        ((wxScrolledWindow*)m_parent)->GetScrollPixelsPerUnit(&spx,&spy);
        *x += (vx*spx);
        *y += (vy*spy);
    }
}

// -----------------------------------------------------------------------

void wxCustomControlManager::SetPopup ( wxCustomComboPopup* popup )
{
    m_openPopup = popup;
}

// -----------------------------------------------------------------------

void wxCustomControlManager::ClosePopup ()
{
    ((wxCustomComboPopup*)m_openPopup)->ForcedClose();
    m_openPopup = (wxWindow*) NULL;
}

// -----------------------------------------------------------------------

wxCustomControlManager::wxCustomControlManager ( wxWindow* parent )
{
    Create ( parent );
}

// -----------------------------------------------------------------------

void wxCustomControlManager::OnParentFocusChange ( bool state )
{
    if ( state )
    {
        if ( m_pCaret ) m_pCaret->OnSetFocus ();
    }
    else
    {
        if ( m_pCaret ) m_pCaret->OnKillFocus ();
    }
}

// -----------------------------------------------------------------------

void wxCustomControlManager::OnParentScrollWinEvent ( wxScrollWinEvent& WXUNUSED(event) )
{
#if wxCC_CORRECT_CONTROL_POSITION
    m_flags |= wxCCM_FL_VIEWSTARTCHANGED;
#endif

    // Close open popup
    if ( m_openPopup ) ClosePopup();

    //wxLogDebug(wxT("CC::OnParentScrollWinEvent"));

#if defined(__WXGTK__)
    //wxLogDebug(wxT("CC::OnParentScrollWinEvent - In GTK"));
    // Unfocus if visible caret
    if ( IsCaretVisible() /*&& */ )
    {
        if ( m_ctrlKbFocus )
        {
            RemoveFocus ( m_ctrlKbFocus );
        }
        else
        {
            HideCaret();
        }
    }
#endif
}

// -----------------------------------------------------------------------

// moves controls etc.
void wxCustomControlManager::OnParentScrollChange ()
{
#if wxCC_CORRECT_CONTROL_POSITION
    size_t i;

    int vx, vy;
    int spx, spy;

    ((wxScrolledWindow*)m_parent)->GetScrollPixelsPerUnit( &spx, &spy );
    ((wxScrolledWindow*)m_parent)->GetViewStart( &vx, &vy );
    vx *= spx;
    vy *= spy;

    int mx = m_prevViewStart.x - vx;
    int my = m_prevViewStart.y - vy;

    if ( mx != 0 || my != 0 )
    {
        //wxLogDebug ( wxT("SCROLL: %i, %i"),mx,my);

        for ( i = 0; i < m_controls.GetCount(); i++ )
        {
            wxCustomControl* cc = (wxCustomControl*)m_controls.Item(i);
            wxASSERT ( cc != NULL );
            cc->m_rect.Offset ( mx, my );
        }

        m_prevViewStart.x = vx; m_prevViewStart.y = vy;
    }

    m_flags &= ~(wxCCM_FL_VIEWSTARTCHANGED);
#endif
}

// -----------------------------------------------------------------------

wxCustomControlManager::wxCustomControlManager ()
{
    m_parent = NULL;
    m_pCaret = NULL;
}

// -----------------------------------------------------------------------

wxCustomControlManager::~wxCustomControlManager ()
{
    if ( m_pCaret )
    {
        delete m_pCaret;
    }

    wxASSERT_MSG ( m_handlers.GetCount() == 0, wxT("wxCustomControlManager::m_handlers was not empty; some controls were not removed."));
}

// -----------------------------------------------------------------------
// wxCCustomTextCtrl
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCCustomTextCtrl, wxCustomControl)

bool wxCCustomTextCtrl::OnKeyEvent ( wxKeyEvent& event )
{
    /*wxLogDebug(wxT("char=%i"),event.GetEventType());
    wxLogDebug(wxT("wxChar=%i"),(int)wxEVT_CHAR);
    wxLogDebug(wxT("wxChar=%i"),(int)wxEVT_KEY_DOWN);
    wxLogDebug(wxT("wxChar=%i"),(int)wxEVT_KEY_UP);*/
    return m_data.OnKeyEvent ( event );
}

// -----------------------------------------------------------------------

bool wxCCustomTextCtrl::OnMouseFocus ( bool entry )
{
    wxWindow* parent = m_manager->m_parent;
    if ( entry )
        parent->SetCursor ( m_manager->m_cursorIBeam );

    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCCustomTextCtrl::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* )
{
    return m_data.OnMouseEvent ( event );
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::SetValue ( const wxString& value )
{
    int prev_ins_pos = m_data.GetPosition();
    m_data.SetValue ( value );
    if ( prev_ins_pos > (int)value.length() )
        prev_ins_pos = (int)value.length();
    Draw ();
    SetInsertionPoint ( prev_ins_pos );
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::DoMove ( int x, int y )
{
    wxCustomControl::DoMove ( x, y );
    m_data.Move ( x, y );
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::DoSetSize ( int width, int height )
{
    wxCustomControl::DoSetSize ( width, height );
    m_data.SetSize ( width, height );
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::DoDraw ( wxDC& dc, const wxRect& rect )
{
    m_data.Draw ( dc, rect );
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::OnFocus ( bool focused )
{
    if ( focused )
    {
        m_data.SetInsertionPoint ( m_data.GetPosition(), -1 );
    }
}

// -----------------------------------------------------------------------

void wxCCustomTextCtrl::SetFont ( wxFont& font, wxDC* pdc )
{
    int tw, th;
    m_pFont = &font;
	m_manager->m_parent->GetTextExtent( wxT("jG"), &tw, &th, 0, 0, &font );
    m_fontHeight = th;

    // Fix caret?
    if ( m_flags & wxCC_FL_KBFOCUS )
        SetInsertionPoint ( m_data.GetPosition() );

    // Redraw
    if ( m_flags & wxCC_FL_DRAWN ) DrawPDC ( pdc );

}

// -----------------------------------------------------------------------

wxCCustomTextCtrl::wxCCustomTextCtrl( wxCustomControlManager* manager, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style )
    : wxCustomControl ( manager, id, pos, size, style )
{
    m_data.Create ( this, pos, size, value );
    manager->AddChildData(&m_data);
    m_flags |= wxCC_FL_NEEDSCARET;
}

// -----------------------------------------------------------------------

wxCCustomTextCtrl::~wxCCustomTextCtrl()
{
}

// -----------------------------------------------------------------------
// wxCCustomButton
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCCustomButton, wxCustomControl)

void wxCCustomButton::DoDraw ( wxDC& dc, const wxRect& rect )
{
    m_data.Draw ( dc, rect );
}

// -----------------------------------------------------------------------

void wxCCustomButton::DoMove ( int x, int y )
{
    wxCustomControl::DoMove ( x, y );
    m_data.Move ( x, y );
}

// -----------------------------------------------------------------------

void wxCCustomButton::DoSetSize ( int width, int height )
{
    wxCustomControl::DoSetSize ( width, height );
    m_data.SetSize ( width, height );
}

// -----------------------------------------------------------------------

bool wxCCustomButton::OnKeyEvent ( wxKeyEvent& WXUNUSED(event) )
{
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCCustomButton::OnMouseFocus ( bool entry )
{
    wxWindow* parent = m_manager->m_parent;
    if ( entry )
        parent->SetCursor ( *wxSTANDARD_CURSOR );

    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCCustomButton::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* )
{
    return m_data.OnMouseEvent ( event );
}

// -----------------------------------------------------------------------

wxCCustomButton::wxCCustomButton ( wxCustomControlManager* manager,
                                 wxWindowID id, const wxChar* label,
                                 const wxPoint& pos, const wxSize& size )
    : wxCustomControl ( manager, id, pos, size, 0 )
{
    m_data.Create ( this, pos, size );
    manager->AddChildData(&m_data);
    m_data.m_label = label;
    m_data.m_down = 0;
}

// -----------------------------------------------------------------------

wxCCustomButton::~wxCCustomButton()
{
}

// -----------------------------------------------------------------------
// wxCCustomComboBox
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCCustomComboBox, wxCustomControl)

void wxCCustomComboBox::DoDraw ( wxDC& dc, const wxRect& rect )
{
    m_chData->Draw ( dc, rect, TRUE );
}

// -----------------------------------------------------------------------

bool wxCCustomComboBox::OnKeyEvent ( wxKeyEvent& event )
{
    return m_chData->OnKeyEvent ( event );
}

// -----------------------------------------------------------------------

bool wxCCustomComboBox::OnMouseFocus ( bool entry )
{
    wxWindow* parent = m_manager->m_parent;
    if ( entry )
        parent->SetCursor ( *wxSTANDARD_CURSOR );

    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCCustomComboBox::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata )
{
    return m_chData->OnMouseEvent ( event, pdata );
}

// -----------------------------------------------------------------------

void wxCCustomComboBox::DoMove ( int x, int y )
{
    wxCustomControl::DoMove ( x, y );
    m_chData->Move ( x, y );
}

// -----------------------------------------------------------------------

void wxCCustomComboBox::DoSetSize ( int width, int height )
{
    // On resize, close on popup
    if ( m_chData->m_listInstance )
        m_chData->IntOnSelect ( -2 );

    wxCustomControl::DoSetSize ( width, height );
    m_chData->SetSize ( width, height );
}

// -----------------------------------------------------------------------

void wxCCustomComboBox::SetSelection ( int n )
{
    wxCustomComboListItem item(n);
    m_chData->SetSelection ( item );
    Draw();
}

// -----------------------------------------------------------------------

wxCCustomComboBox::wxCCustomComboBox ( wxCustomControlManager* manager, wxWindowID id,
    const wxString& value,
    const wxPoint& pos, const wxSize& size,
    int n, const wxChar* choices[], long style, const wxSize& imagesize )
    : wxCustomControl (manager,id,pos,size,style)
{
    wxCCustomComboBoxDefaultHandler* chd = new wxCCustomComboBoxDefaultHandler;
    chd->m_imageSize = imagesize;

    chd->Create ( this, value, pos, size, n, choices );

    m_chData = chd;

    manager->AddChildData(chd);
    manager->AddChildData(chd->GetButtonData());
}

// -----------------------------------------------------------------------

wxCCustomComboBox::~wxCCustomComboBox()
{
    m_manager->RemoveChild ( this );

    if ( m_chData )
        delete m_chData;
}
    
// -----------------------------------------------------------------------
// wxCCustomChoice
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCCustomChoice, wxCCustomComboBox)

wxCCustomChoice::wxCCustomChoice ( wxCustomControlManager* manager, wxWindowID id,
        const wxPoint& pos, const wxSize& size,
        int n, const wxChar* choices[], long style,
        const wxSize& imagesize )
        : wxCCustomComboBox (manager,id,wxEmptyString,pos,size,n,choices,style|wxCB_READONLY,imagesize)
{
}

// -----------------------------------------------------------------------

wxCCustomChoice::~wxCCustomChoice()
{
}

// -----------------------------------------------------------------------
// wxCustomControl
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCustomControl, wxObject)

void wxCustomControl::OnFocus ( bool WXUNUSED(focused) )
{
}

// -----------------------------------------------------------------------

bool wxCustomControl::OnKeyEvent ( wxKeyEvent& WXUNUSED(event) )
{
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCustomControl::OnMouseFocus ( bool entry )
{
    wxWindow* parent = m_manager->m_parent;
    if ( entry )
        parent->SetCursor ( *wxSTANDARD_CURSOR );
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxCustomControl::OnMouseEvent ( wxMouseEvent& WXUNUSED(event), wxCustomControlHandler* WXUNUSED(pdata) )
{
    return FALSE;
}

// -----------------------------------------------------------------------

void wxCustomControl::DrawPDC ( wxDC* dc )
{
    if ( dc )
    {
        Draw ( *dc );
    }
    else
    {
        wxWindow* parent = m_manager->m_parent;
        wxASSERT ( parent != NULL );
        wxClientDC dc(parent);
        if ( m_manager->m_flags & wxCCM_FL_SCROLLEDWIN )
            ((wxScrolledWindow*)parent)->PrepareDC(dc);
        Draw ( dc );
    }
}

// -----------------------------------------------------------------------

void wxCustomControl::Draw ( wxDC& dc )
{

#if wxCC_CORRECT_CONTROL_POSITION
    if ( m_manager->m_flags & wxCCM_FL_VIEWSTARTCHANGED )
        m_manager->OnParentScrollChange ();
#endif

#if wxCC_CORRECT_CONTROL_POSITION
    wxWindow* parent = m_manager->m_parent;
    wxASSERT ( parent != NULL );
#endif

    int vx = 0, vy = 0;

#if wxCC_CORRECT_CONTROL_POSITION
    TranslatePositionToLogical(&vx,&vy);
#endif

    m_manager->HideCaretBalanced();

    if ( !m_manager->m_bmpDoubleBuffer )
    {
        wxPoint alignpt(vx,vy);
        Draw ( dc, &alignpt );
    }
    else
    {
        // Use parent's double buffer, when provided

        wxPoint alignpt(-m_rect.x,-m_rect.y);

        wxMemoryDC mem_dc;
        mem_dc.SelectObject ( *m_manager->m_bmpDoubleBuffer );

        Draw ( mem_dc, &alignpt );

        dc.Blit ( vx+m_rect.x, vy+m_rect.y, m_rect.width, m_rect.height,
            &mem_dc, 0, 0, wxCOPY );
    }

    m_manager->ShowCaretBalanced();

}

// -----------------------------------------------------------------------

/*void wxCustomControl::Draw ( wxDC& dc )
{
#if wxCC_CORRECT_CONTROL_POSITION
    if ( m_manager->m_flags & wxCCM_FL_VIEWSTARTCHANGED )
        m_manager->OnParentScrollChange ();
#endif

    wxRect rect(m_rect);
    m_manager->TranslatePosition( &rect.x, &rect.y );

    DoDraw ( dc, rect );
    
    m_flags |= wxCC_FL_DRAWN;
}*/

// -----------------------------------------------------------------------

void wxCustomControl::Draw ( wxDC& dc, const wxPoint* align )
{
#if wxCC_CORRECT_CONTROL_POSITION
    if ( m_manager->m_flags & wxCCM_FL_VIEWSTARTCHANGED )
        m_manager->OnParentScrollChange ();
#endif

    if ( align == NULL )
    {
        DoDraw ( dc, m_rect );
    }
    else
    {
        wxRect rect(m_rect);
        rect.x += align->x;
        rect.y += align->y;
        DoDraw ( dc, rect );
    }
    
    m_flags |= wxCC_FL_DRAWN;
}

// -----------------------------------------------------------------------

void wxCustomControl::DoDraw ( wxDC& WXUNUSED(dc), const wxRect& WXUNUSED(rect) )
{
}

// -----------------------------------------------------------------------

void wxCustomControl::DoMove ( int x, int y )
{
    m_rect.x = x; m_rect.y = y;
    
    // Move caret as well
    if ( /*m_manager->m_pCaret &&*/
         m_manager->IsCaretVisible() && this == m_manager->m_ctrlKbFocus
         /*&& ( x != m_rect.x || y != m_rect.y )*/
       )
    {
        m_manager->RepositionCaret();
        //m_manager->MoveCaretRelative ( (x-m_rect.x), (y-m_rect.y) );
    }

}

// -----------------------------------------------------------------------

void wxCustomControl::DoSetSize ( int width, int height )
{
    m_rect.width = width;
    m_rect.height = height;

    // Handle caret showing/hiding
    if ( m_manager->IsCaretVisible() && this == m_manager->m_ctrlKbFocus )
    {
        wxPoint pos = m_manager->GetCaretPosition ();

        //wxLogDebug ( wxT("y = %i, pos.y = %i"),m_rect.y,pos.y);
        m_manager->TranslatePositionToLogical ( &pos.x, &pos.y );
        //wxLogDebug ( wxT("y = %i, pos.y = %i"),m_rect.y,pos.y);

        if ( m_rect.Inside ( pos ) )
            m_manager->ShowCaret ();
        else
            m_manager->HideCaret ();
    }
}

// -----------------------------------------------------------------------

void wxCustomControl::SetFont ( wxFont& font, wxDC* pdc )
{
    int tw, th;
    m_pFont = &font;
	m_manager->m_parent->GetTextExtent( wxT("jG"), &tw, &th, 0, 0, &font );
    m_fontHeight = th;

    // Redraw if already drawn
    if ( m_flags & wxCC_FL_DRAWN ) DrawPDC ( pdc );    
}

// -----------------------------------------------------------------------

wxCustomControl::wxCustomControl()
{
    m_manager = (wxCustomControlManager*) NULL;
}

// -----------------------------------------------------------------------

wxCustomControl::wxCustomControl( wxCustomControlManager* manager, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style )
{
    m_manager = manager;
    manager->AddChild ( this );
    m_id = id;
    m_rect.x = pos.x; m_rect.y = pos.y;
    m_rect.width = size.x; m_rect.height = size.y;
    //m_imageSize = imagesize;
    m_windowStyle = style;
    m_flags = 0;

    // must be last
    SetFont ( manager->m_font, NULL );
}

// -----------------------------------------------------------------------

wxCustomControl::~wxCustomControl()
{
    if ( !(m_flags & wxCC_FL_REMOVED) )
        m_manager->RemoveChild ( this );
}

// -----------------------------------------------------------------------

#endif // wxPG_USE_CUSTOM_CONTROLS
