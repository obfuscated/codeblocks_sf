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

#ifndef WXSITEMEDITORDRAGASSIST_H
#define WXSITEMEDITORDRAGASSIST_H

#include "wxsitemeditorcontent.h"
#include "wxsitem.h"
#include <wx/dc.h>
#include <wx/bitmap.h>

/** \brief Class drawing additional data to help dragging */
class wxsItemEditorDragAssist
{
    public:

        enum DragAssistType
        {
            dtNone,
            dtOutline,
            dtColourMix
        };

        /** \brief Ctor */
        wxsItemEditorDragAssist(wxsItemEditorContent* Content);

        /** \brief Dctor */
        ~wxsItemEditorDragAssist();

        /** \brief Function starting new dragging process */
        void NewDragging();

        /** \brief Function draging additional stuff */
        void DrawExtra(wxsItem* Target,wxsItem* Parent,bool AddAfter,wxDC* DC);

    private:

        wxsItem* m_PreviousTarget;
        wxsItem* m_PreviousParent;
        bool m_PreviousAddAfter;

        wxBitmap* m_TargetBitmap;
        wxRect    m_TargetRect;
        bool      m_IsTarget;

        wxBitmap* m_ParentBitmap;
        wxRect    m_ParentRect;
        bool      m_IsParent;

        wxsItemEditorContent* m_Content;

        void UpdateAssist(wxsItem* NewTarget,wxsItem* NewParent,bool NewAddAfter);
        void RebuildParentAssist();
        void RebuildTargetAssist();
        inline DragAssistType AssistType();
        inline wxColour TargetColour();
        inline wxColour ParentColour();
        void ColourMix(wxImage& Image,const wxColour& Colour);
        void UpdateRect(wxRect& Rect,const wxBitmap& bmp);
};

#endif
