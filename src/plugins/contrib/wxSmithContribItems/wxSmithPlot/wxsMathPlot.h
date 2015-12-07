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
*/

#ifndef wxsMATHPLOT_H
#define wxsMATHPLOT_H

#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
#include    <cmath>
#include    <iostream>

#include    <wx/settings.h>
#include    <wx/utils.h>
#include    <wx/string.h>
#include    <wx/filename.h>


#include <wxwidgets/wxscontainer.h>
#include <wxwidgets/wxsitemresdata.h>

#include    <mathplot.h>

class wxsMathPlot : public wxsContainer
{
    public:

        wxsMathPlot(wxsItemResData* Data);


    protected:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode();
        virtual void OnEnumContainerProperties(long Flags);
        virtual bool OnCanAddChild(wxsItem* Item, bool ShowMessage);

        double      mXScale, mYScale;

};

#endif      // wxsMATHPLOT_H
