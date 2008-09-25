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

        bool        mInternalContext;
        bool        mContextVar;
        wxString    mContextVarName;

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

/*
WX_GL_RGBA  Use true colour
WX_GL_BUFFER_SIZE  Bits for buffer if not WX_GL_RGBA
WX_GL_LEVEL  0 for main buffer, >0 for overlay, <0 for underlay
WX_GL_DOUBLEBUFFER  Use doublebuffer
WX_GL_STEREO  Use stereoscopic display
WX_GL_AUX_BUFFERS  Number of auxiliary buffers (not all implementation support this option)
WX_GL_MIN_RED  Use red buffer with most bits (> MIN_RED bits)
WX_GL_MIN_GREEN  Use green buffer with most bits (> MIN_GREEN bits)
WX_GL_MIN_BLUE  Use blue buffer with most bits (> MIN_BLUE bits)
WX_GL_MIN_ALPHA  Use alpha buffer with most bits (> MIN_ALPHA bits)
WX_GL_DEPTH_SIZE  Bits for Z-buffer (0,16,32)
WX_GL_STENCIL_SIZE  Bits for stencil buffer
WX_GL_MIN_ACCUM_RED  Use red accum buffer with most bits (> MIN_ACCUM_RED bits)
WX_GL_MIN_ACCUM_GREEN  Use green buffer with most bits (> MIN_ACCUM_GREEN bits)
WX_GL_MIN_ACCUM_BLUE  Use blue buffer with most bits (> MIN_ACCUM_BLUE bits)
WX_GL_MIN_ACCUM_ALPHA  Use blue buffer with most bits (> MIN_ACCUM_ALPHA bits)
*/

};

#endif  //  _WXSGLCANVAS_H
