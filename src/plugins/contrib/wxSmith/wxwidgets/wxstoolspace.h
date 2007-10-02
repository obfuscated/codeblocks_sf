/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSTOOLSPACE_H
#define WXSTOOLSPACE_H

#include <wx/scrolwin.h>

class wxsItemResData;
class wxsTool;

/** \brief This class represents extra tools space in editor allowing to edit tools */
class wxsToolSpace: public wxScrolledWindow
{
    public:

        /** \brief Ctor */
        wxsToolSpace(wxWindow* Parent,wxsItemResData* Data);

        /** \brief Dctor */
        virtual ~wxsToolSpace();

        /** \brief Function shiwch must be called before changing preview */
        void BeforePreviewChanged();

        /** \brief Function which must be called after changing preview */
        void AfterPreviewChanged();

        /** \brief Function refreshing current selection (calculating new positions) */
        void RefreshSelection();

        /** \brief Checking whether there are any tools inside this resource */
        inline bool AreAnyTools() { return m_First!=0; }

    private:

        struct Entry;

        /** \brief Custom paint handler */
        void OnPaint(wxPaintEvent& event);

        /** \brief Custom mouse click event */
        void OnMouseClick(wxMouseEvent& event);

        /** \brief Custom mouse double-click event */
        void OnMouseDClick(wxMouseEvent& event);

        /** \brief Custom right click event */
        void OnMouseRight(wxMouseEvent& event);

        /** \brief Any mouse event */
        void OnMouse(wxMouseEvent& event);

        /** \brief Custom key down event */
        void OnKeyDown(wxKeyEvent& event);

        /** \brief Recalculating virtual space required by this window */
        void RecalculateVirtualSize();

        /** \brief Searching for tool entry at given position */
        Entry* FindEntry(int& PosX,int& PosY);

        /** \brief Entry for each tool in resource */
        struct Entry
        {
            wxsTool* m_Tool;
            Entry*   m_Next;
        };

        Entry*          m_First;        ///< \brief First tool in resource
        int             m_Count;        ///< \brief Number of enteries
        wxsItemResData* m_Data;         ///< \brief Resource's data
        bool            m_Unstable;     ///< \brief True between BeforePreviewChanged and AfterPreviewChanged to prevent some rare seg faults

        DECLARE_EVENT_TABLE()
};



#endif
