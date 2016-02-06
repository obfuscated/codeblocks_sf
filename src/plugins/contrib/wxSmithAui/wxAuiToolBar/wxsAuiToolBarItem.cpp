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

#include "wxsAuiToolBarItem.h"

#include <wx/aui/auibar.h>

#include <wxwidgets/wxsflags.h>

using namespace wxsFlags;

namespace
{
    #include "../images/wxsAuiToolBarItem16.xpm"
    #include "../images/wxsAuiToolBarItem32.xpm"

    wxsRegisterItem<wxsAuiToolBarItem> Reg(
        _T("wxAuiToolBarItem"),            // Class name
        wxsTWidget,                             // Item type
        _T("wxWindows"),                        // License
        _T("Benjamin I. Williams"),             // Author
        _T(""),                                 // Author's email
        _T(""),                                 // Item's homepage
        _T("Aui"),                              // Category in palette
        30,                                     // Priority in palette
        _T("AuiToolBarItem"),                                 // Base part of names for new items
        wxsCPP,                                 // List of coding languages supported by this item
        2, 8,                                   // Version
        wxBitmap(wxsAuiToolBarItem32_xpm), // 32x32 bitmap
        wxBitmap(wxsAuiToolBarItem16_xpm), // 16x16 bitmap
        false);

    WXS_EV_BEGIN(wxsAuiToolBarItemEvents)
        WXS_EVI(EVT_TOOL,wxEVT_COMMAND_TOOL_CLICKED,wxCommandEvent,Click)
    WXS_EV_END()
}

wxsAuiToolBarItem::wxsAuiToolBarItem(wxsItemResData* Data):
    wxsAuiToolBarItemBase(Data,&Reg.Info,flVariable|flId,wxsAuiToolBarItemEvents,0),
    m_ItemKind(wxITEM_NORMAL),
    m_DropDown(false)
{}

void wxsAuiToolBarItem::OnEnumItemProperties(long Flags)
{
    WXS_BITMAP(wxsAuiToolBarItem, m_Bitmap, _("Bitmap"), _T("bitmap"),wxART_TOOLBAR)
    WXS_BITMAP(wxsAuiToolBarItem, m_DisabledBitmap, _("Disabled bitmap"), _T("disabledbitmap"),wxART_TOOLBAR)
    WXS_SHORT_STRING(wxsAuiToolBarItem, m_ShortHelp, _("Short help"), _T("shorthelp"), wxEmptyString, false)
    WXS_STRING(wxsAuiToolBarItem, m_LongHelp, _("Long help"), _T("longhelp"), wxEmptyString, false)

    static const long    ItemKindValues[] =
        { wxITEM_NORMAL,
          wxITEM_CHECK,
          0 };
    static const wxChar* ItemKindNames[]  =
        { _("Normal"),
          _("Check"),
          0 };
    WXS_ENUM(wxsAuiToolBarItem, m_ItemKind, _("Item kind"), _T("itemkind"), ItemKindValues, ItemKindNames, wxITEM_NORMAL)

    WXS_BOOL(wxsAuiToolBarItem,m_DropDown,_("DropDown"),_T("dropdown"),false);
}
