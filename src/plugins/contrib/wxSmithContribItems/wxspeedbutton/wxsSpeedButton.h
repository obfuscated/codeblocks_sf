/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
*
*/

#ifndef wxsSpeedButton_H
#define wxsSpeedButton_H

#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
#include    <cmath>
#include    <iostream>

#include    <wx/settings.h>
#include    <wx/utils.h>
#include    <wx/string.h>
#include    <wx/filename.h>
#include    <wx/wfstream.h>
#include    <wx/txtstrm.h>


#include    <wxswidget.h>
#include    <wxsitemresdata.h>
#include    <wxsarraystringproperty.h>
#include    <wxsenumproperty.h>
#include    <wxslongproperty.h>
#include    <wxsbitmapiconproperty.h>
#include    <wxsboolproperty.h>

#include    "wxSpeedButton.h"

class wxsSpeedButton : public wxsWidget
{
    public:

        wxsSpeedButton(wxsItemResData* Data);


    protected:

        virtual wxObject*   OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void        OnBuildCreatingCode();
        virtual void        OnEnumWidgetProperties(long Flags);

                void        BuildBitmap(void);
                bool        IsImageXPM(wxsBitmapData &inData);
                wxString    GetXPMName(wxsBitmapData &inData);

        wxString            mLabel;                 // text on button
        wxsBitmapData       mGlyph;                 // image on button
        int                 mGlyphCount;            // number of images
        bool                mUseInclude;            // #include XPM file?
        int                 mMargin;                // spacing between components
        int                 mGroupIndex;            // group ID
        bool                mAllowAllUp;            // all in group can be up?
        long                mUserData;              // anything the user wants
        long                mButtonType;            // quick ref to update GroupIndex
        bool                mButtonDown;            // start off DOWN or UP ?

};

#endif      // wxsSpeedButton_H
