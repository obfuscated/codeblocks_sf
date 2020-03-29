/*
* This file is part of wxSmith plugin for Code::Blocks Studio
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
*/

#ifndef WXSSASHWINDOW_H
#define WXSSASHWINDOW_H

#include "../wxscontainer.h"

class wxsSashWindow: public wxsContainer
{
    public:

        wxsSashWindow(wxsItemResData* Data);

    private:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode();
        virtual void OnEnumContainerProperties(long Flags);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);

        bool    mTop;
        bool    mBottom;
        bool    mLeft;
        bool    mRight;

};



#endif      // WXSASHWINDOW_H
