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

// TODO: Enable this item as soon as linux version is stable enough

#define ITEM_DISABLED


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


#ifndef ITEM_DISABLED

    wxsRegisterItem<wxsGLCanvas> Reg(
        _T("GLCanvas"),                 // Class name
        wxsTWidget,                     // Item type
        _T("Advanced"),                 // Category in palette
        75,                             // Priority in palette
        false);                         // We do not allow this item inside XRC files

#else

    // Create dummy struct to prevent compilation errors
    struct dummy
    {
        wxsItemInfo Info;
    };

    dummy Reg;

#endif

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

    mInternalContext = true;
    mContextVar      = true;
    mContextVarName  = _("ContextRC");
    mRGBA            = true;
    mBufferSize      = 32;
    mLevel           = 0;
    mDoubleBuffer    = true;
    mStereo          = false;
    mAuxBuffers      = 0;
    mMinRed          = 8;
    mMinGreen        = 8;
    mMinBlue         = 8;
    mMinAlpha        = 8;
    mDepthSize       = 32;
    mStencilSize     = 32;
    mMinAccumRed     = 8;
    mMinAccumGreen   = 8;
    mMinAccumBlue    = 8;
    mMinAccumAlpha   = 8;
    }

//------------------------------------------------------------------------------

void wxsGLCanvas::OnBuildCreatingCode() {
wxString    vname;
wxString    aname;
wxString    dtext;

    vname = GetVarName();
    aname = vname + _("Attrib");

    if (GetLanguage() == wxsCPP) {
        AddHeader(_T("<wx/glcanvas.h>"), GetInfo().ClassName, 0);

        Codef( _T("wxGLContext* ") + mContextVarName + _T(";\n") );

        Codef( _T( "int " ) + aname + _T("[] = {\n") );
        Codef( _T("\tWX_GL_RGBA,            %d,\n"), (mRGBA ? 1 : 0)            );
        Codef( _T("\tWX_GL_BUFFER_SIZE,     %d,\n"), mBufferSize                );
        Codef( _T("\tWX_GL_LEVEL,           %d,\n"), mLevel                     );
        Codef( _T("\tWX_GL_DOUBLEBUFFER,    %d,\n"), (mDoubleBuffer ? 1 : 0)    );
        Codef( _T("\tWX_GL_STEREO,          %d,\n"), (mStereo ? 1 : 0)          );
        Codef( _T("\tWX_GL_AUX_BUFFERS,     %d,\n"), mAuxBuffers                );
        Codef( _T("\tWX_GL_MIN_RED,         %d,\n"), mMinRed                    );
        Codef( _T("\tWX_GL_MIN_GREEN,       %d,\n"), mMinGreen                  );
        Codef( _T("\tWX_GL_MIN_BLUE,        %d,\n"), mMinBlue                   );
        Codef( _T("\tWX_GL_MIN_ALPHA,       %d,\n"), mMinAlpha                  );
        Codef( _T("\tWX_GL_DEPTH_SIZE,      %d,\n"), mDepthSize                 );
        Codef( _T("\tWX_GL_STENCIL_SIZE,    %d,\n"), mStencilSize               );
        Codef( _T("\tWX_GL_MIN_ACCUM_RED,   %d,\n"), mMinAccumRed               );
        Codef( _T("\tWX_GL_MIN_ACCUM_GREEN, %d,\n"), mMinAccumGreen             );
        Codef( _T("\tWX_GL_MIN_ACCUM_BLUE,  %d,\n"), mMinAccumBlue              );
        Codef( _T("\tWX_GL_MIN_ACCUM_ALPHA, %d,\n"), mMinAccumAlpha             );
        Codef( _T("\t0, 0 };\n") );

        if (mInternalContext) {
            Codef(_T("%C(%W, %I, %P, %S, %T, %N, %s);\n"),aname.c_str());
            if (mContextVar) {
                Codef(_T("%s = %s->GetContext();\n"), mContextVarName.c_str(), vname.c_str());
            };
        }
        else {
            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %N);\n"),aname.c_str());
            if (mContextVar) {
                Codef(_T("%s = new wxGLContext(%s);\n"), mContextVarName.c_str(), vname.c_str());
                Codef(_T("%s->SetCurrent(*%s);\n"), mContextVarName.c_str(), vname.c_str());
            };
        };

        BuildSetupWindowCode();

    }
    else {
        wxsCodeMarks::Unknown(_T("wxsGLCanvas::OnBuildCreatingCode"),GetLanguage());
    };
}


//------------------------------------------------------------------------------

wxObject* wxsGLCanvas::OnBuildPreview(wxWindow* Parent, long Flags) {

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

wxPanel     *gc;

    gc = new wxPanel(Parent, GetId(),Pos(Parent),Size(Parent),Style());
    SetupWindow(gc, Flags);
    return gc;
}

//------------------------------------------------------------------------------

void wxsGLCanvas::OnEnumWidgetProperties(long Flags) {

    mContextVarName = GetVarName() + _("RC");

    WXS_BOOL(wxsGLCanvas, mInternalContext, _("Use Internal Context"), _T("mInternalContext"), true)
    WXS_BOOL(wxsGLCanvas, mContextVar,      _("Declare Context"),      _T("mContextVar"),      true)

    WXS_SHORT_STRING(wxsGLCanvas, mContextVarName, _("Context Var Name"), _T("mContextVarName"),  mContextVarName, true)

    WXS_BOOL(wxsGLCanvas, mRGBA,          _("Use True Color"),               _T("mRGBA"),         true)
    WXS_LONG(wxsGLCanvas, mBufferSize,    _("Bits for buffer "),             _T("mBufferSize"),   32)
    WXS_LONG(wxsGLCanvas, mLevel,         _("Main Buffer"),                  _T("mLevel"),         0)
    WXS_BOOL(wxsGLCanvas, mDoubleBuffer,  _("Use doublebuffer"),             _T("mDoubleBuffer"), true)
    WXS_BOOL(wxsGLCanvas, mStereo,        _("Stereoscopic display"),         _T("mStereo"),       false)
    WXS_LONG(wxsGLCanvas, mAuxBuffers,    _("Auxiliary buffers count"),      _T("mAuxBuffers"),    0)
    WXS_LONG(wxsGLCanvas, mMinRed,        _("Red color bits"),               _T("mMinRed"),        8)
    WXS_LONG(wxsGLCanvas, mMinGreen,      _("Green color bits"),             _T("mMinGreen"),      8)
    WXS_LONG(wxsGLCanvas, mMinBlue,       _("Blue color bits"),              _T("mMinBlue"),       8)
    WXS_LONG(wxsGLCanvas, mMinAlpha,      _("Alpha bits"),                   _T("mMinAlpha"),      8)
    WXS_LONG(wxsGLCanvas, mDepthSize,     _("Bits for Z-buffer (0,16,32)"),  _T("mDepthSize"),    32)
    WXS_LONG(wxsGLCanvas, mStencilSize,   _("Bits for stencil buffer "),     _T("mStencilSize"),  32)
    WXS_LONG(wxsGLCanvas, mMinAccumRed,   _("Accumulator Red color bits"),   _T("mMinAccumRed"),   8)
    WXS_LONG(wxsGLCanvas, mMinAccumGreen, _("Accumulator Green color bits"), _T("mMinAccumGreen"), 8)
    WXS_LONG(wxsGLCanvas, mMinAccumBlue,  _("Accumulator Blue color bits"),  _T("mMinAccumBlue"),  8)
    WXS_LONG(wxsGLCanvas, mMinAccumAlpha, _("Accumulator Alpha bits"),       _T("mMinAccumAlpha"), 8)
}
