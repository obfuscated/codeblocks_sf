/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#ifndef WXSCUSTOMBUTTON_H
#define WXSCUSTOMBUTTON_H

#include <wxwidgets/wxswidget.h>


class wxsCustomButton : public wxsWidget
{
    public:

        wxsCustomButton(wxsItemResData* Data);
        virtual ~wxsCustomButton();

    private:

        void OnBuildCreatingCode();
        wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        void OnEnumWidgetProperties(long Flags);

        long m_Type;
        bool m_Flat;
        wxString m_Label;
        long m_LabelPosition;
        wxsBitmapData m_Bitmap;
        wxsBitmapData m_BitmapSelected;
        wxsBitmapData m_BitmapFocused;
        wxsBitmapData m_BitmapDisabled;
        wxsSizeData m_Margins;
        wxsSizeData m_LabelMargins;
        wxsSizeData m_BitmapMargins;
};

#endif
