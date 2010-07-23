/**  \file wxsgrid.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2008  Bartlomiej Swiecki
* Copyright (C) 2008 Ron Collins
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

#ifndef WXSGRID_H
#define WXSGRID_H

#include "../wxswidget.h"

/** \brief Class for wxsTextCtrl widget */
class wxsGrid: public wxsWidget
{
    public:

        wxsGrid(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        long            m_ColsCount;                ///< \brief Number of columns
        long            m_RowsCount;                ///< \brief number of rows
        bool            m_ReadOnly;                 ///< \brief Can edit grid?
        bool            m_GridLines;                ///< \brief Display grid lines?
        long            m_LabelRowHeight;           ///< \brief Size of top row of labels
        long            m_LabelColWidth;            ///< \brief Size of left column of labels
        long            m_DefaultRowSize;           ///< \brief Default row height
        long            m_DefaultColSize;           ///< \brief Default col width
        wxsColourData   m_LabelTextColour;          ///< \brief Colour for all labels
        wxsFontData     m_LabelFont;                ///< \brief Font for all labels

        wxArrayString   m_ColLabels;                ///< \brief Text for column labels
        wxArrayString   m_RowLabels;                ///< \brief Text for row labels
        wxArrayString   m_CellText;                 ///< \brief Text for each cell

};

#endif
