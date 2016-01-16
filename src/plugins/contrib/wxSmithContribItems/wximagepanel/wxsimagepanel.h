/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2010 Ron Collins
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
*/

#ifndef wxsIMAGEPANEL_H
#define wxsIMAGEPANEL_H

#include    <wxwidgets/wxscontainer.h>
#include    <wxwidgets/defitems/wxsimage.h>
#include    <wxwidgets/properties/wxsimagelisteditordlg.h>
#include    "wx/wxImagePanel.h"


class wxsImagePanel : public wxsContainer
{
    public:

        wxsImagePanel(wxsItemResData* Data);
        ~wxsImagePanel();

    protected:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode();
        virtual void OnEnumContainerProperties(long Flags);

        wxString        mImage;                         // wxsImage info
        bool            mStretch;                       // fit image to panel



};

#endif      // wxsIMAGEPANEL_H
