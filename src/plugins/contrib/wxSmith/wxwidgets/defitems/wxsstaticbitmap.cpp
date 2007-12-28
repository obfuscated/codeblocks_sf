/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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

#include <wx/statbmp.h>
#include <wx/dc.h>
#include "wxsstaticbitmap.h"
#include "../wxsflags.h"

namespace
{
    wxsRegisterItem<wxsStaticBitmap> Reg(_T("StaticBitmap"),wxsTWidget,_T("Standard"),70);

    WXS_ST_BEGIN(wxsStaticBitmapStyles,_T(""))
        WXS_ST_CATEGORY("wxStaticBitmap")
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_END()


    class Background: public wxPanel
    {
        public:

            Background(wxWindow* Parent): wxPanel(Parent)
            {
            }

            void OnPaint(wxPaintEvent& event)
            {
                wxPaintDC dc(this);
                dc.SetPen(wxColour(0x80,0x80,0x80));
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(Background,wxPanel)
        EVT_PAINT(Background::OnPaint)
    END_EVENT_TABLE()
}

wxsStaticBitmap::wxsStaticBitmap(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStaticBitmapEvents,
        wxsStaticBitmapStyles)
{}

void wxsStaticBitmap::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/statbmp.h>"),GetInfo().ClassName,hfInPCH);

            // Can not use %i in Codef because it doesn't take size into consideration
            wxsSizeData& SizeData = GetBaseProps()->m_Size;
            bool DontResize = SizeData.IsDefault;
            wxString SizeCodeStr = SizeData.GetSizeCode(GetCoderContext());
            wxString BmpCode = Bitmap.IsEmpty() ? _T("wxNullBitmap") : Bitmap.BuildCode(DontResize,SizeCodeStr,GetCoderContext(),_T("wxART_OTHER"));

            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %N);\n"),BmpCode.c_str());
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticBitmap::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsStaticBitmap::OnBuildPreview(wxWindow* Parent,long Flags)
{
    if ( Flags & wxsFlags::pfExact )
    {
        wxStaticBitmap* Preview = new wxStaticBitmap(Parent,GetId(),Bitmap.GetPreview(Size(Parent)),Pos(Parent),Size(Parent),Style());
        return SetupWindow(Preview,Flags);
    }

    // We do fake background under the bitmap - that's because bitmaps tend to be
    // invisible when not selected

    Background* Back = new Background(Parent);
    wxStaticBitmap* Preview = new wxStaticBitmap(Back,GetId(),Bitmap.GetPreview(Size(Parent)),Pos(Parent),Size(Parent),Style());
    SetupWindow(Preview,Flags);
    wxBoxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
    Sizer->Add(Preview,1,wxEXPAND,0);
    Back->SetSizer(Sizer);
    Sizer->SetSizeHints(Back);
    Sizer->Fit(Back);
    return Back;
}

void wxsStaticBitmap::OnEnumWidgetProperties(long Flags)
{
   WXS_BITMAP(wxsStaticBitmap,Bitmap,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
}

