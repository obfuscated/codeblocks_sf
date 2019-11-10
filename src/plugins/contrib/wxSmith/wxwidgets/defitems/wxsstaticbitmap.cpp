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
    wxsRegisterItem<wxsStaticBitmap> Reg(_T("StaticBitmap"),wxsTWidget,_T("Standard"),80);

    // Cryogen 25/3/10 Bug #15354. Default to wxSIMPLE_BORDER.
    WXS_ST_BEGIN(wxsStaticBitmapStyles,_T("wxSIMPLE_BORDER"))
        WXS_ST_CATEGORY("wxStaticBitmap")
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_END()

// Cryogen 24/3/10 Bug #15354. Removed class Background.
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

            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %N);\n"),BmpCode.wx_str());
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticBitmap::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsStaticBitmap::OnBuildPreview(wxWindow* Parent,long Flags)
{
    // Cryogen 24/3/10 Bug #15354. Since we're no longer using the Background class we don't need to differentiate between cases where
    // the user has assigned a bitmap and those where he hasn't and we're using the fake background. Just return the preview.
    wxStaticBitmap* Preview = new wxStaticBitmap(Parent,GetId(),Bitmap.GetPreview(Size(Parent)),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

void wxsStaticBitmap::OnEnumWidgetProperties(cb_unused long Flags)
{
   WXS_BITMAP(wxsStaticBitmap,Bitmap,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
}

