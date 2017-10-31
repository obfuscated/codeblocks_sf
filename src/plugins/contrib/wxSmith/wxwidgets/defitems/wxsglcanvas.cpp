/**  \file wxsglcanvas.cpp
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

// TODO: Enable this item as soon as linux version is stable enough



/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <wx/glcanvas.h>
*/

#include "wxsglcanvas.h"

//------------------------------------------------------------------------------
/*

There is a problem trying to use OpenGL from within the wxSmithContribItems.dll;
although it works OK when incorporated into the user code.  We use OpenGL here
when trying to make a "preview" item for the visual designer.

We work around this problem by making the preview as a simple panel object.  The
panel looks the same as a wxGLCanvas, so the user will see the same designer.

*/

//------------------------------------------------------------------------------

namespace
{


    wxsRegisterItem<wxsGLCanvas> Reg(
        _T("GLCanvas"),                 // Class name
        wxsTWidget,                     // Item type
        _T("Advanced"),                 // Category in palette
        80,                             // Priority in palette
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsGLCanvasStyles,_T(""))
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsGLCanvasEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()

}

//------------------------------------------------------------------------------

wxsGLCanvas::wxsGLCanvas(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGLCanvasEvents,
        wxsGLCanvasStyles)
{

//    mInternalContext = true;
//    mContextVar      = true;
//    mContextVarName  = _T("ContextRC");
    mRGBA            = true;
    mBufferSize      = 0;
    mLevel           = 0;
    mDoubleBuffer    = true;
    mStereo          = false;
    mAuxBuffers      = 0;
    mMinRed          = 0;
    mMinGreen        = 0;
    mMinBlue         = 0;
    mMinAlpha        = 0;
    mDepthSize       = 16;
    mStencilSize     = 0;
    mMinAccumRed     = 0;
    mMinAccumGreen   = 0;
    mMinAccumBlue    = 0;
    mMinAccumAlpha   = 0;
}

//------------------------------------------------------------------------------

void wxsGLCanvas::OnBuildCreatingCode()
{

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/glcanvas.h>"), GetInfo().ClassName, 0);

            // Generate unique name for attributes variable
            wxString aname = GetCoderContext()->GetUniqueName( _T("GLCanvasAttributes") );

            // Generate attributes array
            Codef( _T("int %v[] = {\n" ), aname.wx_str() );

            if ( mRGBA )
            {
                Codef( _T("\tWX_GL_RGBA,\n") );
            }

            if ( !mRGBA && mBufferSize > 0 )
            {
                Codef( _T("\tWX_GL_BUFFER_SIZE,     %d,\n"), mBufferSize );
            }

            if ( mLevel )
            {
                Codef( _T("\tWX_GL_LEVEL,           %d,\n"), mLevel );
            }

            if ( mDoubleBuffer )
            {
                Codef( _T("\tWX_GL_DOUBLEBUFFER,\n") );
            }

            if ( mStereo )
            {
                Codef( _T("\tWX_GL_STEREO,\n") );
            }

            if ( mAuxBuffers > 0 )
            {
                Codef( _T("\tWX_GL_AUX_BUFFERS,     %d,\n"), mAuxBuffers );
            }

            if ( mMinRed > 0 )
            {
                Codef( _T("\tWX_GL_MIN_RED,         %d,\n"), mMinRed );
            }

            if ( mMinGreen > 0 )
            {
                Codef( _T("\tWX_GL_MIN_GREEN,       %d,\n"), mMinGreen );
            }

            if ( mMinBlue > 0 )
            {
                Codef( _T("\tWX_GL_MIN_BLUE,        %d,\n"), mMinBlue );
            }

            if ( mMinAlpha > 0 )
            {
                Codef( _T("\tWX_GL_MIN_ALPHA,       %d,\n"), mMinAlpha );
            }

            if ( mDepthSize >= 0 )
            {
                int size = ( mDepthSize <= 16 ) ? 16 : 32;
                Codef( _T("\tWX_GL_DEPTH_SIZE,      %d,\n"), size );
            }

            if ( mStencilSize >= 0 )
            {
                Codef( _T("\tWX_GL_STENCIL_SIZE,    %d,\n"), mStencilSize );
            }

            if ( mMinAccumRed > 0 )
            {
                Codef( _T("\tWX_GL_MIN_ACCUM_RED,   %d,\n"), mMinAccumRed );
            }

            if ( mMinAccumGreen > 0 )
            {
                Codef( _T("\tWX_GL_MIN_ACCUM_GREEN, %d,\n"), mMinAccumGreen );
            }

            if ( mMinAccumBlue > 0 )
            {
                Codef( _T("\tWX_GL_MIN_ACCUM_BLUE,  %d,\n"), mMinAccumBlue );
            }

            if ( mMinAccumAlpha > 0 )
            {
                Codef( _T("\tWX_GL_MIN_ACCUM_ALPHA, %d,\n"), mMinAccumAlpha );
            }

            // We padd the attributes table with two zeros instead of one
            // just to be sure that messy code (ours or from wxWidgets)
            // don't crash because of lack of second argument
            Codef( _T("\t0, 0 };\n") );

            // Now we can create our window
            Codef(_T("%C(%W, %I, %P, %S, %T, %N, %v);\n"),aname.wx_str());

            BuildSetupWindowCode();
            break;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGLCanvas::OnBuildCreatingCode"),GetLanguage());
        }
    };
}


//------------------------------------------------------------------------------

wxObject* wxsGLCanvas::OnBuildPreview(wxWindow* Parent, long Flags)
{

// there is a problem importing the OpenGL DLL into this designer DLL
// so ...
// for the visual designer, just use a wxPanel to show where the
// canvas will be located


/*
wxGLCanvas  *gc;

    gc = new wxGLCanvas(Parent,
        GetId(),
        Pos(Parent),
        Size(Parent));
    SetupWindow(gc, Flags);
    return gc;
*/

    wxPanel* gc = new wxPanel(Parent, GetId(),Pos(Parent),Size(Parent),Style());
    SetupWindow(gc, Flags);
    return gc;
}

//------------------------------------------------------------------------------

void wxsGLCanvas::OnEnumWidgetProperties(cb_unused long Flags)
{
//    mContextVarName = GetVarName() + _("RC");
//
//    WXS_BOOL(wxsGLCanvas, mInternalContext, _("Use Internal Context"), _T("mInternalContext"), true)
//    WXS_BOOL(wxsGLCanvas, mContextVar,      _("Declare Context"),      _T("mContextVar"),      true)
//
//    WXS_SHORT_STRING(wxsGLCanvas, mContextVarName, _("Context Var Name"), _T("mContextVarName"),  mContextVarName, true)
    WXS_BOOL(wxsGLCanvas, mRGBA,          _("Use True Color"),               _T("mRGBA"),          true)
    WXS_LONG(wxsGLCanvas, mBufferSize,    _("Bits for buffer "),             _T("mBufferSize"),    0)
    WXS_LONG(wxsGLCanvas, mLevel,         _("Main Buffer"),                  _T("mLevel"),         0)
    WXS_BOOL(wxsGLCanvas, mDoubleBuffer,  _("Use doublebuffer"),             _T("mDoubleBuffer"),  true)
    WXS_BOOL(wxsGLCanvas, mStereo,        _("Stereoscopic display"),         _T("mStereo"),        false)
    WXS_LONG(wxsGLCanvas, mAuxBuffers,    _("Auxiliary buffers count"),      _T("mAuxBuffers"),    0)
    WXS_LONG(wxsGLCanvas, mMinRed,        _("Red color bits"),               _T("mMinRed"),        0)
    WXS_LONG(wxsGLCanvas, mMinGreen,      _("Green color bits"),             _T("mMinGreen"),      0)
    WXS_LONG(wxsGLCanvas, mMinBlue,       _("Blue color bits"),              _T("mMinBlue"),       0)
    WXS_LONG(wxsGLCanvas, mMinAlpha,      _("Alpha bits"),                   _T("mMinAlpha"),      0)
    WXS_LONG(wxsGLCanvas, mDepthSize,     _("Bits for Z-buffer (0,16,32)"),  _T("mDepthSize"),     0)
    WXS_LONG(wxsGLCanvas, mStencilSize,   _("Bits for stencil buffer "),     _T("mStencilSize"),   0)
    WXS_LONG(wxsGLCanvas, mMinAccumRed,   _("Accumulator Red color bits"),   _T("mMinAccumRed"),   0)
    WXS_LONG(wxsGLCanvas, mMinAccumGreen, _("Accumulator Green color bits"), _T("mMinAccumGreen"), 0)
    WXS_LONG(wxsGLCanvas, mMinAccumBlue,  _("Accumulator Blue color bits"),  _T("mMinAccumBlue"),  0)
    WXS_LONG(wxsGLCanvas, mMinAccumAlpha, _("Accumulator Alpha bits"),       _T("mMinAccumAlpha"), 0)
}
