/**  \file wxsglcanvas.h
*
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

#ifndef _WXSGLCANVAS_H
#define _WXSGLCANVAS_H

#include "../wxswidget.h"

/** \brief Class for wxsCLCanvas widget */
class wxsGLCanvas: public wxsWidget
{
    public:

        wxsGLCanvas(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        bool        mRGBA;
        int         mBufferSize;
        int         mLevel;
        bool        mDoubleBuffer;
        bool        mStereo;
        int         mAuxBuffers;
        int         mMinRed;
        int         mMinGreen;
        int         mMinBlue;
        int         mMinAlpha;
        int         mDepthSize;
        int         mStencilSize;
        int         mMinAccumRed;
        int         mMinAccumGreen;
        int         mMinAccumBlue;
        int         mMinAccumAlpha;

};

#endif  //  _WXSGLCANVAS_H
