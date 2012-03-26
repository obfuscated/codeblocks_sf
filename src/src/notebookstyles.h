/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef NOTEBOOKSTYLES_H
#define NOTEBOOKSTYLES_H

#include "cbauibook.h"

#if defined(__WXGTK__) && (USE_GTK_NOTEBOOK)
    #define GSocket GLibSocket
    #include <gtk/gtk.h>
    #undef GSocket
    #include <wx/artprov.h>
#endif


class wxDC;
class wxWindow;
class wxRect;
class wxString;
class wxBitmap;

wxColor wxAuiStepColour(const wxColor& c, int percent);

class NbStyleVC71 : public wxAuiDefaultTabArt
{
public:
    NbStyleVC71();
    wxAuiTabArt* Clone();

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                        const wxRect& in_rect, int close_button_state,
                        wxRect* out_tab_rect, wxRect* out_button_rect,
                        int* x_extent);

    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
                            const wxSize& required_bmp_size);
};

class NbStyleFF2 : public wxAuiDefaultTabArt
{
public:
    NbStyleFF2();
    wxAuiTabArt* Clone();
    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                        const wxRect& in_rect, int close_button_state,
                        wxRect* out_tab_rect, wxRect* out_button_rect,
                        int* x_extent);

    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
                            const wxSize& required_bmp_size);
};

#if defined(__WXGTK__) && (USE_GTK_NOTEBOOK) && !wxCHECK_VERSION(2, 9, 4)
class NbStyleGTK : public wxAuiDefaultTabArt
{
public:
    NbStyleGTK();

    virtual wxAuiTabArt* Clone();
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiNotebookPage& page,
                         const wxRect& in_rect,
                         int close_button_state,
                         wxRect* out_tab_rect,
                         wxRect* out_button_rect,
                         int* x_extent);
    void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id,
                    int button_state, int orientation, wxRect* out_rect);
    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
                            const wxSize& required_bmp_size);
    virtual wxSize GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active,
                              int close_button_state, int* x_extent);
private:
    int m_Xthickness;
    int m_Ythickness;
    int m_TabHBorder;
    int m_TabVBorder;
    wxBitmap m_ActiveCloseButton;
};
#endif // #if defined(__WXGTK__) && (USE_GTK_NOTEBOOK) && !wxCHECK_VERSION(2, 9, 4)

#endif // NOTEBOOKSTYLES_H
