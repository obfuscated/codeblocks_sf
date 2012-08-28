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

#ifndef WXSSTORINGSPLITTERWINDOW_H
#define WXSSTORINGSPLITTERWINDOW_H

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/event.h>

/** \brief This class extends behavior of standard splitter window
 *         fixing some bugs with them when using docking systems
 */
class wxsStoringSplitterWindow : public wxPanel
{
    public:

        /** \brief Ctor */
        wxsStoringSplitterWindow(wxWindow* Parent);

        /** \brief Function splitting window */
        void Split(wxWindow* Top,wxWindow* Bottom,int SashPosition = 0);

        /** \brief Getting splitter window associated with this object */
        inline wxSplitterWindow* GetSplitter() { return Splitter; }

    private:

        void OnSize(wxSizeEvent& event);
        void OnSplitterChanging(wxSplitterEvent& event);
        void OnSplitterChanged(wxSplitterEvent& event);

        int SplitterFixup(int ProposedPosition);

        wxSplitterWindow* Splitter;
        int SplitPosition;
        DECLARE_EVENT_TABLE()
};

#endif
