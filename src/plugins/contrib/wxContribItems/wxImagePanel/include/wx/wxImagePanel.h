/** \file wximagepanel.h
*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2010 Ron Collins
*
* wxSmithContribItems is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithContribItems is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithContribItems. If not, see <http://www.gnu.org/licenses/>.
*
*/


#ifndef _IMAGEPANEL_H_
#define _IMAGEPANEL_H_

#include    <wx/object.h>
#include    <wx/panel.h>
#include    <wx/image.h>
#include    <wx/bitmap.h>
#include    <wx/dcclient.h>


class wxImagePanel : public wxPanel
{
public:

        wxImagePanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = _("ImagePanel"));

        void        SetBitmap(wxBitmap &inBitmap);
        wxBitmap    GetBitmap(void);

        void        SetStretch(bool inStretch);
        bool        GetStretch(void);

        void        DoPaint(wxPaintEvent& event);



protected:

    wxBitmap    mBitmap;
    bool        mStretch;

    DECLARE_DYNAMIC_CLASS(wxImagePanel)

};



#endif      // _IMAGEPANEL_H_
