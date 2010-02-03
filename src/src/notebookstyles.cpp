/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <wx/dc.h>
#include <wx/window.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/dcclient.h>
#include "cbauibook.h"
#include "notebookstyles.h"

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

    clone->SetNormalFont(m_normal_font);
    clone->SetSelectedFont(m_selected_font);
    clone->SetMeasuringFont(m_measuring_font);

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

    wxCoord tab_height = m_tab_ctrl_height - 3;
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
    int close_button_width = 0;
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        close_button_width = m_active_close_bmp.GetWidth();
    }


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
        dc.SetFont(m_selected_font);
    else
        dc.SetFont(m_normal_font);
    dc.GetTextExtent(caption, &textx, &texty);
    // draw tab text
    dc.DrawText(page.caption, text_offset,
                drawn_tab_yoff + drawn_tab_height / 2 - texty / 2 - 1);

    // draw 'x' on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        int close_button_width = m_active_close_bmp.GetWidth();
        wxBitmap bmp = m_disabled_close_bmp;

        if ((close_button_state == wxAUI_BUTTON_STATE_HOVER) ||
                    (close_button_state == wxAUI_BUTTON_STATE_PRESSED))
            bmp = m_active_close_bmp;

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
    dc.SetFont(m_measuring_font);
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

    clone->SetNormalFont(m_normal_font);
    clone->SetSelectedFont(m_selected_font);
    clone->SetMeasuringFont(m_measuring_font);

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

    wxCoord tab_height = m_tab_ctrl_height - 2;
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
    {
        close_button_width = m_active_close_bmp.GetWidth();
    }


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
        dc.SetFont(m_selected_font);
    else
        dc.SetFont(m_normal_font);
    dc.GetTextExtent(caption, &textx, &texty);
    // draw tab text
    dc.DrawText(page.caption, text_offset,
                drawn_tab_yoff + drawn_tab_height / 2 - texty / 2 - 1);

    // draw 'x' on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        int close_button_width = m_active_close_bmp.GetWidth();
        wxBitmap bmp = m_disabled_close_bmp;

        if ((close_button_state == wxAUI_BUTTON_STATE_HOVER) ||
                    (close_button_state == wxAUI_BUTTON_STATE_PRESSED))
            bmp = m_active_close_bmp;

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
    dc.SetFont(m_measuring_font);
    int x_ext = 0;
    wxSize s = GetTabSize(dc, wnd, wxT("ABCDEFGHIj"), wxNullBitmap, true,
                            wxAUI_BUTTON_STATE_HIDDEN, &x_ext);
    return s.y + 6;
}

