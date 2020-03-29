/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiToolBarSpacer.h"

#include <wx/aui/auibar.h>

#include <wxwidgets/wxsflags.h>

using namespace wxsFlags;

namespace
{
    #include "../images/wxsAuiToolBarSpacer16.xpm"
    #include "../images/wxsAuiToolBarSpacer32.xpm"

    wxsRegisterItem<wxsAuiToolBarSpacer> Reg(
        _T("wxAuiToolBarSpacer"),            // Class name
        wxsTWidget,                             // Item type
        _T("wxWindows"),                        // License
        _T("Benjamin I. Williams"),             // Author
        _T(""),                                 // Author's email
        _T(""),                                 // Item's homepage
        _T("Aui"),                              // Category in palette
        1,                                     // Priority in palette
        _T("AuiToolBarSpacer"),                                 // Base part of names for new items
        wxsCPP,                                 // List of coding languages supported by this item
        2, 8,                                   // Version
        wxBitmap(wxsAuiToolBarSpacer32_xpm),    // 32x32 bitmap
        wxBitmap(wxsAuiToolBarSpacer16_xpm),    // 16x16 bitmap
        false);
}

wxsAuiToolBarSpacer::wxsAuiToolBarSpacer(wxsItemResData* Data)
    : wxsAuiToolBarItemBase(Data,&Reg.Info,flId,0,0),
    m_Stretch(false),
    m_Pixels(0),
    m_Proportion(1)
{}

void wxsAuiToolBarSpacer::OnEnumItemProperties(long Flags)
{
    WXS_BOOL(wxsAuiToolBarSpacer,m_Stretch,_("Is strecht spacer"),_T("stretch"),false)
    WXS_LONG(wxsAuiToolBarSpacer,m_Pixels,_("Pixels"),_T("pixels"),0)
    WXS_LONG(wxsAuiToolBarSpacer,m_Proportion,_("Proportion"),_T("proportion"),1)
}
