/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSGRIDPANEL_H
#define WXSGRIDPANEL_H

#include <wx/panel.h>

/** \brief Class used for containers like wxPanel, where items can be freely placed,
 *         it shows extra grid and helps visual editor to snap items to grid
 */
class wxsGridPanel: public wxPanel
{
    public:

        /** \brief Empty ctor */
        wxsGridPanel();

        /** \brief Ctor */
        wxsGridPanel(wxWindow* parent, wxWindowID id=wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString& name=_T("wxsGridPanel"));

        /** \brief Dctor */
        virtual ~wxsGridPanel();

        /** \brief Reading grid size from system configuration */
        static int GetGridSize();

    protected:

        /** \brief Paint procedure */
        void OnPaint(wxPaintEvent& event);

        /** \brief Checking whether we should draw border around client area */
        virtual bool DrawBorder() { return false; }

        DECLARE_CLASS(wxsGridPanel)
        DECLARE_EVENT_TABLE()
};

#endif
