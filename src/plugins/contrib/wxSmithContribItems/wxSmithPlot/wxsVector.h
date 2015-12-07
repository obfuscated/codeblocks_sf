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

#ifndef WXSVECTOR_H
#define WXSVECTOR_H

#include    <iostream>

#include    <wx/stattext.h>
#include    <wxwidgets/wxswidget.h>
#include    <mathplot.h>


class wxsVector: public wxsWidget
{
    public:

        wxsVector(wxsItemResData* Data);

    private:

        virtual void        OnBuildCreatingCode();
        virtual wxObject*   OnBuildPreview(wxWindow* Parent, long Flags);
        virtual void        OnBuildDeclarationsCode();
        virtual void        OnEnumWidgetProperties(long Flags);

                void        ParseXY(void);



        wxString            mLabel;
        int                 mAlign;
        bool                mContinuous;
        wxsColourData       mPenColour;     // color to draw
        wxsFontData         mPenFont;       // for drawing the text
        wxArrayString       mXYData;

        wxArrayString       mXs;            // internal use; X data as a string
        wxArrayString       mYs;            // Y data as a string
        std::vector<double>  mXf;            // X data as a double value
        std::vector<double>  mYf;            // Y data as a double

};

#endif      // WXSVECTOR_H
