/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */


#include <wx/window.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/image.h>
#include "cbauibook.h"
#include "prep.h"
#include "notebookstyles.h"

#if defined(__WXGTK__) && (USE_GTK_NOTEBOOK)
    #define GSocket GLibSocket
    #include <gtk/gtk.h>
    #undef GSocket
    #include <wx/artprov.h>
#endif

#if defined ( __WXGTK__ ) && wxCHECK_VERSION(2, 9, 0)
    #include <wx/gtk/dc.h>
    #include <wx/gtk/dcclient.h>
#else
    #include <wx/dc.h>
    #include <wx/dcclient.h>
#endif

// Some general constants:
namespace
{
    const int c_vertical_border_padding = 4;
}

/******************************************************************************
* Renderer for Microsoft (tm) Visual Studio 7.1 like tabs                     *
******************************************************************************/

NbStyleVC71::NbStyleVC71() : wxAuiDefaultTabArt()
{
}

wxAuiTabArt* NbStyleVC71::Clone()
{
    NbStyleVC71* clone = new NbStyleVC71();

#if wxCHECK_VERSION(2, 9, 3)
    clone->SetNormalFont(m_normalFont);
    clone->SetSelectedFont(m_selectedFont);
    clone->SetMeasuringFont(m_measuringFont);
#else
    clone->SetNormalFont(m_normal_font);
    clone->SetSelectedFont(m_selected_font);
    clone->SetMeasuringFont(m_measuring_font);
#endif

    return clone;
}

void NbStyleVC71::DrawTab(wxDC& dc, wxWindow* wnd,
                            const wxAuiNotebookPage& page,
                            const wxRect& in_rect, int close_button_state,
                            wxRect* out_tab_rect, wxRect* out_button_rect,
                            int* x_extent)
{
    // Visual studio 7.1 style
    // This code is based on the renderer included in wxFlatNotebook:
    // http://svn.berlios.de/wsvn/codeblocks/trunk/src/sdk/wxFlatNotebook/src/wxFlatNotebook/renderer.cpp?rev=5106

    // figure out the size of the tab

    wxSize tab_size = GetTabSize(dc,
                                 wnd,
                                 page.caption,
                                 page.bitmap,
                                 page.active,
                                 close_button_state,
                                 x_extent);

#if wxCHECK_VERSION(2, 9, 3)
    wxCoord tab_height = m_tabCtrlHeight - 3;
#else
    wxCoord tab_height = m_tab_ctrl_height - 3;
#endif
    wxCoord tab_width = tab_size.x;
    wxCoord tab_x = in_rect.x;
    wxCoord tab_y = in_rect.y + in_rect.height - tab_height;
    int clip_width = tab_width;
    if (tab_x + clip_width > in_rect.x + in_rect.width - 4)
        clip_width = (in_rect.x + in_rect.width) - tab_x - 4;
    dc.SetClippingRegion(tab_x, tab_y, clip_width + 1, tab_height - 3);
    if(m_flags & wxAUI_NB_BOTTOM)
        tab_y--;

    dc.SetPen((page.active) ? wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT)) : wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)));
    dc.SetBrush((page.active) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(*wxTRANSPARENT_BRUSH));

    if (page.active)
    {
//        int tabH = (m_flags & wxAUI_NB_BOTTOM) ? tab_height - 5 : tab_height - 2;
        int tabH = tab_height - 2;

        dc.DrawRectangle(tab_x, tab_y, tab_width, tabH);

        int rightLineY1 = (m_flags & wxAUI_NB_BOTTOM) ? c_vertical_border_padding - 2 : c_vertical_border_padding - 1;
        int rightLineY2 = tabH + 3;
        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)));
        dc.DrawLine(tab_x + tab_width - 1, rightLineY1 + 1, tab_x + tab_width - 1, rightLineY2);
        if(m_flags & wxAUI_NB_BOTTOM)
            dc.DrawLine(tab_x + 1, rightLineY2 - 3 , tab_x + tab_width - 1, rightLineY2 - 3);
        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW)));
        dc.DrawLine(tab_x + tab_width , rightLineY1 , tab_x + tab_width, rightLineY2);
        if(m_flags & wxAUI_NB_BOTTOM)
            dc.DrawLine(tab_x , rightLineY2 - 2 , tab_x + tab_width, rightLineY2 - 2);

    }
    else
    {
        // We dont draw a rectangle for non selected tabs, but only
        // vertical line on the right
        int blackLineY1 = (m_flags & wxAUI_NB_BOTTOM) ? c_vertical_border_padding + 2 : c_vertical_border_padding + 1;
        int blackLineY2 = tab_height - 5;
        dc.DrawLine(tab_x + tab_width, blackLineY1, tab_x + tab_width, blackLineY2);
    }

    wxPoint border_points[2];
    if (m_flags & wxAUI_NB_BOTTOM)
    {
        border_points[0] = wxPoint(tab_x, tab_y);
        border_points[1] = wxPoint(tab_x, tab_y + tab_height - 6);
    }
    else // if (m_flags & wxAUI_NB_TOP)
    {
        border_points[0] = wxPoint(tab_x, tab_y + tab_height - 4);
        border_points[1] = wxPoint(tab_x, tab_y + 2);
    }

    int drawn_tab_yoff = border_points[1].y;
    int drawn_tab_height = border_points[0].y - border_points[1].y;

    int text_offset = tab_x + 8;

    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = tab_x + 8;

        // draw bitmap
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
                      true);

        text_offset = bitmap_offset + page.bitmap.GetWidth();
        text_offset += 3; // bitmap padding
    }
     else
    {
        text_offset = tab_x + 8;
    }


    // if the caption is empty, measure some temporary text
    wxString caption = page.caption;
    if (caption.empty())
        caption = wxT("Xj");

    wxCoord textx;
    wxCoord texty;
    if (page.active)
#if wxCHECK_VERSION(2, 9, 3)
        dc.SetFont(m_selectedFont);
#else
        dc.SetFont(m_selected_font);
#endif
    else
#if wxCHECK_VERSION(2, 9, 3)
        dc.SetFont(m_normalFont);
#else
        dc.SetFont(m_normal_font);
#endif
    dc.GetTextExtent(caption, &textx, &texty);
    // draw tab text
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    dc.DrawText(page.caption, text_offset,
                drawn_tab_yoff + drawn_tab_height / 2 - texty / 2 - 1);

    // draw 'x' on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
#if wxCHECK_VERSION(2, 9, 3)
        int close_button_width = m_activeCloseBmp.GetWidth();
        wxBitmap bmp = m_disabledCloseBmp;
#else
        int close_button_width = m_active_close_bmp.GetWidth();
        wxBitmap bmp = m_disabled_close_bmp;
#endif

        if ((close_button_state == wxAUI_BUTTON_STATE_HOVER) ||
                    (close_button_state == wxAUI_BUTTON_STATE_PRESSED))
#if wxCHECK_VERSION(2, 9, 3)
            bmp = m_activeCloseBmp;
#else
            bmp = m_active_close_bmp;
#endif

        wxRect rect(tab_x + tab_width - close_button_width - 3,
                    drawn_tab_yoff + (drawn_tab_height / 2) - (bmp.GetHeight() / 2),
                    close_button_width, tab_height);

        // Indent the button if it is pressed down:
        if (close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            rect.x++;
            rect.y++;
        }
        dc.DrawBitmap(bmp, rect.x, rect.y, true);
        *out_button_rect = rect;
    }

    *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);
    dc.DestroyClippingRegion();
}

int NbStyleVC71::GetBestTabCtrlSize(wxWindow* wnd,
                                    const wxAuiNotebookPageArray& WXUNUSED(pages),
                                    const wxSize& WXUNUSED(required_bmp_size))
{
//    m_requested_tabctrl_height = -1;
//    m_tab_ctrl_height = -1;
    wxClientDC dc(wnd);
#if wxCHECK_VERSION(2, 9, 3)
    dc.SetFont(m_measuringFont);
#else
    dc.SetFont(m_measuring_font);
#endif
    int x_ext = 0;
    wxSize s = GetTabSize(dc, wnd, wxT("ABCDEFGHIj"), wxNullBitmap, true,
                            wxAUI_BUTTON_STATE_HIDDEN, &x_ext);
    return s.y + 4;
}

NbStyleFF2::NbStyleFF2() : wxAuiDefaultTabArt()
{
}

wxAuiTabArt* NbStyleFF2::Clone()
{
    NbStyleFF2* clone = new NbStyleFF2();

#if wxCHECK_VERSION(2, 9, 3)
    clone->SetNormalFont(m_normalFont);
    clone->SetSelectedFont(m_selectedFont);
    clone->SetMeasuringFont(m_measuringFont);
#else
    clone->SetNormalFont(m_normal_font);
    clone->SetSelectedFont(m_selected_font);
    clone->SetMeasuringFont(m_measuring_font);
#endif

    return clone;
}

void NbStyleFF2::DrawTab(wxDC& dc, wxWindow* wnd,
                            const wxAuiNotebookPage& page,
                            const wxRect& in_rect, int close_button_state,
                            wxRect* out_tab_rect, wxRect* out_button_rect,
                            int* x_extent)
{

    // Firefox 2 style

    // figure out the size of the tab
    wxSize tab_size = GetTabSize(dc, wnd, page.caption, page.bitmap,
                                    page.active, close_button_state, x_extent);

#if wxCHECK_VERSION(2, 9, 3)
    wxCoord tab_height = m_tabCtrlHeight - 2;
#else
    wxCoord tab_height = m_tab_ctrl_height - 2;
#endif
    wxCoord tab_width = tab_size.x;
    wxCoord tab_x = in_rect.x;
    wxCoord tab_y = in_rect.y + in_rect.height - tab_height;

    int clip_width = tab_width;
    if (tab_x + clip_width > in_rect.x + in_rect.width - 4)
        clip_width = (in_rect.x + in_rect.width) - tab_x - 4;
    dc.SetClippingRegion(tab_x, tab_y, clip_width + 1, tab_height - 3);

	wxPoint tabPoints[7];
	int adjust = 0;
    if (!page.active)
    {
        adjust = 1;
    }

    tabPoints[0].x = tab_x + 3;
    tabPoints[0].y = (m_flags & wxAUI_NB_BOTTOM) ? 3 : tab_height - 2;

    tabPoints[1].x = tabPoints[0].x;
    tabPoints[1].y = (m_flags & wxAUI_NB_BOTTOM) ? tab_height - (c_vertical_border_padding + 2) - adjust : (c_vertical_border_padding + 2) + adjust;

    tabPoints[2].x = tabPoints[1].x+2;
    tabPoints[2].y = (m_flags & wxAUI_NB_BOTTOM) ? tab_height - c_vertical_border_padding - adjust: c_vertical_border_padding + adjust;

    tabPoints[3].x = tab_x +tab_width - 2;
    tabPoints[3].y = tabPoints[2].y;

    tabPoints[4].x = tabPoints[3].x + 2;
    tabPoints[4].y = tabPoints[1].y;

    tabPoints[5].x = tabPoints[4].x;
    tabPoints[5].y = tabPoints[0].y;

    tabPoints[6].x = tabPoints[0].x;
    tabPoints[6].y = tabPoints[0].y;

//    dc.SetBrush((page.active) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(wxAuiStepColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),85)));
    dc.SetBrush((page.active) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(*wxTRANSPARENT_BRUSH));

	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));

	dc.DrawPolygon(7, tabPoints);

    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    if (page.active)
    {
        dc.DrawLine(tabPoints[0].x + 1, tabPoints[0].y, tabPoints[5].x , tabPoints[0].y);
    }

    int drawn_tab_yoff = tabPoints[1].y;
    int drawn_tab_height = tabPoints[0].y - tabPoints[2].y;

    int text_offset = tab_x + 8;
    int close_button_width = 0;
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
#if wxCHECK_VERSION(2, 9, 3)
        close_button_width = m_activeCloseBmp.GetWidth();
#else
        close_button_width = m_active_close_bmp.GetWidth();
#endif

    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = tab_x + 8;

        // draw bitmap
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
                      true);

        text_offset = bitmap_offset + page.bitmap.GetWidth();
        text_offset += 3; // bitmap padding
    }
     else
    {
        text_offset = tab_x + 8;
    }


    // if the caption is empty, measure some temporary text
    wxString caption = page.caption;
    if (caption.empty())
        caption = wxT("Xj");

    wxCoord textx;
    wxCoord texty;
    if (page.active)
#if wxCHECK_VERSION(2, 9, 3)
        dc.SetFont(m_selectedFont);
#else
        dc.SetFont(m_selected_font);
#endif
    else
#if wxCHECK_VERSION(2, 9, 3)
        dc.SetFont(m_normalFont);
#else
        dc.SetFont(m_normal_font);
#endif
    dc.GetTextExtent(caption, &textx, &texty);
    // draw tab text
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    dc.DrawText(page.caption, text_offset,
                drawn_tab_yoff + drawn_tab_height / 2 - texty / 2 - 1);

    // draw 'x' on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
#if wxCHECK_VERSION(2, 9, 3)
        int close_button_width = m_activeCloseBmp.GetWidth();
        wxBitmap bmp = m_disabledCloseBmp;
#else
        int close_button_width = m_active_close_bmp.GetWidth();
        wxBitmap bmp = m_disabled_close_bmp;
#endif

        if ((close_button_state == wxAUI_BUTTON_STATE_HOVER) ||
                    (close_button_state == wxAUI_BUTTON_STATE_PRESSED))
#if wxCHECK_VERSION(2, 9, 3)
            bmp = m_activeCloseBmp;
#else
            bmp = m_active_close_bmp;
#endif

        wxRect rect(tab_x + tab_width - close_button_width - 3,
                    drawn_tab_yoff + (drawn_tab_height / 2) - (bmp.GetHeight() / 2),
                    close_button_width, tab_height);

        // Indent the button if it is pressed down:
        if (close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            rect.x++;
            rect.y++;
        }
        dc.DrawBitmap(bmp, rect.x, rect.y, true);
        *out_button_rect = rect;
    }

    *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);
    dc.DestroyClippingRegion();
}

int NbStyleFF2::GetBestTabCtrlSize(wxWindow* wnd,
                                    const wxAuiNotebookPageArray& WXUNUSED(pages),
                                    const wxSize& WXUNUSED(required_bmp_size))
{
//    m_requested_tabctrl_height = -1;
//    m_tab_ctrl_height = -1;
    wxClientDC dc(wnd);
#if wxCHECK_VERSION(2, 9, 3)
    dc.SetFont(m_measuringFont);
#else
    dc.SetFont(m_measuring_font);
#endif
    int x_ext = 0;
    wxSize s = GetTabSize(dc, wnd, wxT("ABCDEFGHIj"), wxNullBitmap, true,
                            wxAUI_BUTTON_STATE_HIDDEN, &x_ext);
    return s.y + 6;
}

#if defined(__WXGTK__) && (USE_GTK_NOTEBOOK)

namespace
{

static GtkWidget *g_window = nullptr;
static GtkWidget *g_container = nullptr;
static GtkWidget *g_notebook = nullptr;
static GtkWidget *g_button = nullptr;
static int s_CloseIconSize = 16; // default size


void SetCloseIconSize(int size)
{
    s_CloseIconSize = size;
}

static void setup_widget_prototype(GtkWidget* widget)
{
    if (!g_window)
    {
        g_window = gtk_window_new(GTK_WINDOW_POPUP);
        gtk_widget_realize(g_window);
    }
    if (!g_container)
    {
        g_container = gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(g_window), g_container);
    }

    gtk_container_add(GTK_CONTAINER(g_container), widget);
    gtk_widget_realize(widget);
}

static GtkStyle * get_style_button()
{
    if (!g_button)
    {
        g_button = gtk_button_new();
        setup_widget_prototype(g_button);
    }
    return gtk_widget_get_style(g_button);
}

static GtkStyle * get_style_notebook()
{
    if (!g_notebook)
    {
        g_notebook = gtk_notebook_new();
        setup_widget_prototype(g_notebook);
    }
    return gtk_widget_get_style(g_notebook);
}

}

NbStyleGTK::NbStyleGTK():
    m_Xthickness(0),
    m_Ythickness(0),
    m_TabHBorder(0),
    m_TabVBorder(0)

{
}

wxAuiTabArt* NbStyleGTK::Clone()
{
    NbStyleGTK* clone = new NbStyleGTK();

#if wxCHECK_VERSION(2, 9, 3)
    clone->SetNormalFont(m_normalFont);
    clone->SetSelectedFont(m_normalFont);
    clone->SetMeasuringFont(m_normalFont);
#else
    clone->SetNormalFont(m_normal_font);
    clone->SetSelectedFont(m_normal_font);
    clone->SetMeasuringFont(m_normal_font);
#endif

    return clone;
}

void NbStyleGTK::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    GtkStyle* style_notebook = get_style_notebook();
    GtkNotebook* notebook = GTK_NOTEBOOK (g_notebook);

    // if one of the parameters have changed, the height needs to be recalculated, so we force it,
    if(m_Xthickness  != style_notebook->xthickness ||
       m_Ythickness  != style_notebook->ythickness ||
       m_TabVBorder != notebook->tab_vborder ||
       m_TabHBorder != notebook->tab_hborder)
    {
        m_Xthickness  = style_notebook->xthickness;
        m_Ythickness  = style_notebook->ythickness;
        m_TabVBorder = notebook->tab_vborder;
        m_TabHBorder = notebook->tab_hborder;
        wxAuiNotebook* nb = nullptr;
        if(wnd)
             nb = (cbAuiNotebook*)wnd->GetParent();
        if(nb)
            nb->SetTabCtrlHeight(-1);
    }
#if wxCHECK_VERSION(2, 9, 0)
    wxGTKDCImpl *impldc = (wxGTKDCImpl*) dc.GetImpl();
    GdkWindow* pWin = impldc->GetGDKWindow();
#else
    GdkWindow* pWin = dc.GetGDKWindow();
#endif
    gtk_style_apply_default_background(style_notebook, pWin, 1, GTK_STATE_NORMAL, nullptr,
                                       rect.x, rect.y, rect.width, rect.height);
}

wxRect DrawSimpleButton(wxDC& dc, GtkWidget *widget, int button_state, wxRect const &in_rect)
{
    wxRect r;

    r.height = in_rect.height - 3 * get_style_notebook()->ythickness;
    r.width = r.height;
    r.x = in_rect.x + in_rect.width - r.width;

#if wxCHECK_VERSION(2, 9, 0)
    wxGTKDCImpl *impldc = (wxGTKDCImpl*) dc.GetImpl();
    GdkWindow* pWin = impldc->GetGDKWindow();
#else
    GdkWindow* pWin = dc.GetGDKWindow();
#endif

    if (button_state == wxAUI_BUTTON_STATE_HOVER)
    {
        gtk_paint_box(get_style_button(), pWin,
                      GTK_STATE_PRELIGHT, GTK_SHADOW_OUT, nullptr, widget, "button",
                      r.x, r.y, r.width, r.height);
    }
    else if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        gtk_paint_box(get_style_button(), pWin,
                      GTK_STATE_ACTIVE, GTK_SHADOW_IN, nullptr, widget, "button",
                      r.x, r.y, r.width, r.height);
    }

    return r;
}

void NbStyleGTK::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                             const wxRect& in_rect, int close_button_state, wxRect* out_tab_rect,
                             wxRect* out_button_rect, int* x_extent)
{
    GtkWidget *widget = wnd->GetHandle();
    GtkStyle *style_notebook = get_style_notebook();
    GtkStyle *style_button = get_style_button();
    wxRect const &window_rect = wnd->GetRect();

    int padding;
    int focus_width = 0;

    gtk_widget_style_get (widget,
        "focus-line-width", &focus_width,
        NULL);

    padding = focus_width + m_TabHBorder;


    // figure out the size of the tab
    wxSize tab_size = GetTabSize(dc, wnd, page.caption, page.bitmap,
                                    page.active, close_button_state, x_extent);

    wxRect tab_rect = in_rect;
    tab_rect.width = tab_size.x;
    tab_rect.height = page.active ? tab_size.y + m_Ythickness : tab_size.y;

    int clip_width = tab_rect.width;
    if (tab_rect.x + tab_rect.width > in_rect.x + in_rect.width - 4)
        clip_width = (in_rect.x + in_rect.width) - tab_rect.x - 4;

    dc.SetClippingRegion(tab_rect.x, tab_rect.y, clip_width, tab_rect.height);

    GdkRectangle area;
    area.x = ((tab_rect.x > 20)  && !page.active) ? tab_rect.x + m_TabVBorder : tab_rect.x;
    area.y = tab_rect.y;
    area.width = ((tab_rect.x > 20)  && !page.active) ? clip_width - m_TabVBorder : clip_width;
    area.height = page.active ? tab_rect.height : tab_rect.height + m_Ythickness;

#if wxCHECK_VERSION(2, 9, 0)
    wxGTKDCImpl *impldc = (wxGTKDCImpl*) dc.GetImpl();
    GdkWindow* pWin = impldc->GetGDKWindow();
#else
    GdkWindow* pWin = dc.GetGDKWindow();
#endif

    // if page is active, we draw a box without border, in some styles the gap is transparent (e.g. Human)
    // and a line would be visible at the bottom of the tab
    gtk_paint_box(style_notebook, pWin, GTK_STATE_NORMAL,page.active?GTK_SHADOW_NONE:GTK_SHADOW_OUT,
                  NULL, widget, "notebook",
                  window_rect.x, in_rect.y + in_rect.height - 3 * m_Ythickness, window_rect.width, 4 * m_Ythickness);
    if (page.active)
    {
        gtk_paint_box_gap(style_notebook, pWin, GTK_STATE_NORMAL, GTK_SHADOW_OUT,
                          NULL, widget, "notebook",
                          window_rect.x, in_rect.y + in_rect.height - 3 * m_Ythickness, window_rect.width, 4 * m_Ythickness,
                          GTK_POS_TOP, tab_rect.x - 1 , tab_rect.width);
    }
    gtk_paint_extension(style_notebook, pWin,
                       page.active ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE, GTK_SHADOW_OUT,
                       &area, widget, "tab",
                       tab_rect.x, page.active ? tab_rect.y : tab_rect.y + m_Ythickness,
                       tab_rect.width, tab_rect.height,
                       GTK_POS_BOTTOM);
    wxCoord textX = tab_rect.x + padding + m_Xthickness;

    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = textX;

        // draw bitmap
        int bitmapY = tab_rect.y +(tab_rect.height - page.bitmap.GetHeight()) / 2;
        if(!page.active)
            bitmapY += m_Ythickness;
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      bitmapY,
                      true);

        textX += page.bitmap.GetWidth() + padding;
    }

    wxCoord textW, textH, textY;

#if wxCHECK_VERSION(2, 9, 3)
    dc.SetFont(m_normalFont);
#else
    dc.SetFont(m_normal_font);
#endif
    dc.GetTextExtent(page.caption, &textW, &textH);
    textY = tab_rect.y + (tab_rect.height - textH) / 2;
    if(!page.active)
        textY += m_Ythickness;


    // draw tab text
    GdkColor text_colour = page.active ? style_notebook->fg[GTK_STATE_NORMAL] : style_notebook->fg[GTK_STATE_ACTIVE];
    dc.SetTextForeground(wxColor(text_colour));
    GdkRectangle focus_area;

    int padding_focus = padding - focus_width;
    focus_area.x = tab_rect.x + padding_focus;
    focus_area.y = textY - focus_width;
    focus_area.width = tab_rect.width - 2 * padding_focus;
    focus_area.height = textH + 2 * focus_width;

    if(page.active && (wnd->FindFocus() == wnd) && focus_area.x <= (area.x + area.width))
    {
        // clipping seems not to work here, so we we have to recalc the focus-area manually
        if((focus_area.x + focus_area.width) > (area.x + area.width))
            focus_area.width = area.x + area.width - focus_area.x + focus_width;
        gtk_paint_focus (style_notebook, pWin,
                         GTK_STATE_ACTIVE, &area, widget, "tab",
                         focus_area.x, focus_area.y, focus_area.width, focus_area.height);
    }

    dc.DrawText(page.caption, textX, textY);

    // draw close-button on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        wxBitmap bmp;
        bmp.SetPixbuf(gtk_widget_render_icon(widget, GTK_STOCK_CLOSE, GTK_ICON_SIZE_SMALL_TOOLBAR, "tab"));
        if(bmp.GetWidth() != s_CloseIconSize || bmp.GetHeight() != s_CloseIconSize)
        {
            wxImage img = bmp.ConvertToImage();
            img.Rescale(s_CloseIconSize, s_CloseIconSize);
            bmp = img;
        }

        int button_size = s_CloseIconSize + 2 * m_Xthickness;
        int buttonY = tab_rect.y + (tab_rect.GetHeight() - button_size) / 2;
        if(!page.active)
            buttonY += m_Ythickness;
        wxRect rect(tab_rect.x + tab_rect.width - button_size - padding,
                    buttonY ,
                    button_size,
                    button_size);

        if (close_button_state == wxAUI_BUTTON_STATE_HOVER)
        {
            gtk_paint_box(style_button, pWin,
                          GTK_STATE_PRELIGHT, GTK_SHADOW_OUT, &area, widget, "button",
                          rect.x, rect.y, rect.width, rect.height);
        }
        else if (close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            gtk_paint_box(style_button, pWin,
                          GTK_STATE_ACTIVE, GTK_SHADOW_IN, &area, widget, "button",
                          rect.x, rect.y, rect.width, rect.height);
        }

        dc.DrawBitmap(bmp, rect.x + m_Xthickness, rect.y + style_notebook->ythickness, true);
        *out_button_rect = rect;
    }

    tab_rect.width = std::min(tab_rect.width, clip_width);
    *out_tab_rect = tab_rect;

    dc.DestroyClippingRegion();
}

void ArrowStateAndShadow(int button_state, GtkStateType &state, GtkShadowType &shadow)
{

    if (button_state & wxAUI_BUTTON_STATE_DISABLED)
    {
        state = GTK_STATE_INSENSITIVE;
        shadow = GTK_SHADOW_ETCHED_IN;
    }
    else if (button_state & wxAUI_BUTTON_STATE_HOVER)
    {
        state = GTK_STATE_PRELIGHT;
        shadow = GTK_SHADOW_OUT;
    }
    else if (button_state & wxAUI_BUTTON_STATE_PRESSED)
    {
        state = GTK_STATE_ACTIVE;
        shadow = GTK_SHADOW_IN;
    }
    else
    {
        state = GTK_STATE_NORMAL;
        shadow = GTK_SHADOW_OUT;
    }
}

wxRect DrawSimpleArrow(wxDC& dc,
                       GtkWidget *widget,
                       int button_state,
                       wxRect const &in_rect,
                       int orientation,
                       GtkArrowType arrow_type)
{
    int scroll_arrow_hlength, scroll_arrow_vlength;
    gtk_widget_style_get(widget,
                         "scroll-arrow-hlength", &scroll_arrow_hlength,
                         "scroll-arrow-vlength", &scroll_arrow_vlength,
                         NULL);

    GtkStateType state;
    GtkShadowType shadow;
    ArrowStateAndShadow(button_state, state, shadow);

    wxRect out_rect;

    if (orientation == wxLEFT)
        out_rect.x = in_rect.x;
    else
        out_rect.x = in_rect.x + in_rect.width - scroll_arrow_hlength;
    out_rect.y = (in_rect.y + in_rect.height - 3 * get_style_notebook()->ythickness - scroll_arrow_vlength) / 2;
    out_rect.width = scroll_arrow_hlength;
    out_rect.height = scroll_arrow_vlength;

#if wxCHECK_VERSION(2, 9, 0)
    wxGTKDCImpl *impldc = (wxGTKDCImpl*) dc.GetImpl();
    GdkWindow* pWin = impldc->GetGDKWindow();
#else
    GdkWindow* pWin = dc.GetGDKWindow();
#endif
    gtk_paint_arrow (get_style_button(), pWin, state, shadow, nullptr, widget, "notebook",
                     arrow_type, TRUE, out_rect.x, out_rect.y, out_rect.width, out_rect.height);

    return out_rect;
}

wxRect MakeButtonScaledRect(wxRect const &button_rect, float scale)
{
    GtkStyle *style_button = get_style_button();
    wxRect r;
    r.width = (button_rect.width - 2 * style_button->xthickness);
    r.height = (button_rect.height - 2 * style_button->ythickness);
    r.x = button_rect.x + roundf(r.width * (1.0f - scale) / 2.0) + style_button->xthickness;
    r.y = button_rect.y + roundf(r.height * (1.0f - scale) / 2.0) + style_button->ythickness;
    r.width = roundf(r.width * scale);
    r.height = roundf(r.height * scale);
    return r;
}

wxRect MakeButtonIconRect(wxRect const &button_rect, int icon_size)
{
    GtkStyle *style_button = get_style_button();
    wxRect r;

    r.x = button_rect.x + style_button->xthickness
          + (button_rect.width - 2 * style_button->xthickness - icon_size) / 2;
    r.y = button_rect.y + style_button->ythickness
          + (button_rect.width - 2 * style_button->ythickness - icon_size) / 2;
    r.width = r.height = icon_size;
    return r;
}

void NbStyleGTK::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id,
                                int button_state, int orientation, wxRect* out_rect)
{
    GtkWidget *widget = wnd->GetHandle();
    wxRect r;

    switch (bitmap_id)
    {
        case wxAUI_BUTTON_LEFT:
            r = DrawSimpleArrow(dc, widget, button_state, in_rect, orientation, GTK_ARROW_LEFT);
            break;

        case wxAUI_BUTTON_RIGHT:
            r = DrawSimpleArrow(dc, widget, button_state, in_rect, orientation, GTK_ARROW_RIGHT);
            break;

        case wxAUI_BUTTON_WINDOWLIST:
            {
                float const scale = 0.5f;

                r = DrawSimpleButton(dc, widget, button_state, in_rect);//, s_CloseIconSize);

                wxRect const &arrow = MakeButtonScaledRect(r, scale);
                GtkStateType state;
                GtkShadowType shadow;

                ArrowStateAndShadow(button_state, state, shadow);

#if wxCHECK_VERSION(2, 9, 0)
                wxGTKDCImpl *impldc = (wxGTKDCImpl*) dc.GetImpl();
                GdkWindow* pWin = impldc->GetGDKWindow();
#else
                GdkWindow* pWin = dc.GetGDKWindow();
#endif

                gtk_paint_arrow(get_style_notebook(), pWin, state,
                                shadow, nullptr, widget, "notebook",
                                GTK_ARROW_DOWN, TRUE,
                                arrow.x, arrow.y, arrow.width, arrow.height);
            }
            break;
    }

    *out_rect = r;
}


int NbStyleGTK::GetBestTabCtrlSize(wxWindow* wnd,
                                    const wxAuiNotebookPageArray& pages,
                                    const wxSize& required_bmp_size)
{
#if wxCHECK_VERSION(2, 9, 3)
    SetMeasuringFont(m_normalFont);
    SetSelectedFont(m_normalFont);
#else
    SetMeasuringFont(m_normal_font);
    SetSelectedFont(m_normal_font);
#endif
    int tab_height = 3 * get_style_notebook()->ythickness + wxAuiDefaultTabArt::GetBestTabCtrlSize(wnd, pages, required_bmp_size);
    return tab_height;
}

wxSize NbStyleGTK::GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active,
                                  int close_button_state, int* x_extent)
{
    wxSize s = wxAuiDefaultTabArt::GetTabSize(dc, wnd, caption, bitmap, active, close_button_state, x_extent);
    *x_extent -= GTK_NOTEBOOK (g_notebook)->tab_hborder;
    return s;
}

#endif // #if defined(__WXGTK__) && (USE_GTK_NOTEBOOK)
