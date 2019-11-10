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

#include "wxSmithAuiToolBar.h"

// missing wxITEM_* items, the same as in wx/auibar.cpp, needed for adding spacer exception
enum
{
    wxITEM_CONTROL = wxITEM_MAX,
    wxITEM_LABEL,
    wxITEM_SPACER
};

wxSmithAuiToolBar::wxSmithAuiToolBar(wxWindow* parent,wxWindowID id,const wxPoint& position,const wxSize& size,long style)
    : wxAuiToolBar(parent,id,position,size,style)
{
    //ctor
}

wxSmithAuiToolBar::~wxSmithAuiToolBar()
{
    //dtor
}

int wxSmithAuiToolBar::HitTest(const wxPoint& pt)
{
    for (unsigned int i=0; i<m_items.Count(); i++)
    {
        wxAuiToolBarItem& Item = m_items.Item(i);

        if (!Item.GetSizerItem())
            continue;

        wxRect rect = Item.GetSizerItem()->GetRect();
        if ( Item.GetKind() == wxITEM_SPACER )
        {
            if ( rect.Contains(pt.x,rect.y) )
            {
                return GetToolIndex(Item.GetId());
            }
        }

        if ( rect.Contains(pt.x,pt.y) )
        {
            return GetToolIndex(Item.GetId());
        }
    }

    return wxNOT_FOUND;
}

void wxSmithAuiToolBar::AddSpacer(int pixels,wxWindowID SpacerId)
{
    wxAuiToolBarItem Item;
    Item.SetHasDropDown(false);
    Item.SetSpacerPixels(pixels);
    Item.SetId(SpacerId);
    Item.SetKind(wxITEM_SPACER);
    Item.SetSticky(false);

    m_items.Add(Item);
}

void wxSmithAuiToolBar::AddStretchSpacer(int proportion,wxWindowID SpacerId)
{
    wxAuiToolBarItem item;
    item.SetHasDropDown(false);
    item.SetId(SpacerId);
    item.SetProportion(proportion);
    item.SetSticky(false);

    m_items.Add(item);
}
