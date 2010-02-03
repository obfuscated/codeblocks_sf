/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef NOTEBOOKSTYLES_H
#define NOTEBOOKSTYLES_H

#include "cbauibook.h"

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

#endif // NOTEBOOKSTYLES_H
