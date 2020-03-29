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

#include "wxsAuiToolBarItemBase.h"

#include "wxSmithAuiToolBar.h"
#include "../wxAuiManager/wxsAuiManager.h"
#include <wxwidgets/wxsparent.h>
#include <wx/window.h>

namespace
{
    class wxSmithWindow : public wxWindow
    {
        public:
            wxSmithWindow(wxWindow *parent,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize)
                : wxWindow(parent,-1,pos,size)
            {
                Show(false);
            }

            virtual bool IsShown() const { return true; }
    };
}

wxObject* wxsAuiToolBarItemBase::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSmithAuiToolBar* ParentToolBar = wxDynamicCast(Parent,wxSmithAuiToolBar);
    if ( !ParentToolBar ) return NULL;

    wxRect  ItemRect     = ParentToolBar->GetToolRect(m_ItemId);
    wxPoint ItemPosition = ItemRect.GetPosition();
    wxSize  ItemSize     = ItemRect.GetSize();


    if ( GetClassName() == _T("wxAuiToolBarSpacer") )
    {
        int Margin = Parent->ClientToScreen(ItemPosition).y - Parent->GetParent()->ClientToScreen(ParentToolBar->GetPosition()).y;
        ItemSize.y = ParentToolBar->GetClientSize().y - 2*Margin;
    }
    if ( m_HasGripper == wxLEFT ) ItemPosition.x += m_GripperSize;
    if ( m_HasGripper == wxTOP  ) ItemPosition.y += m_GripperSize;

    return new wxSmithWindow(Parent,ItemPosition,ItemSize);
}

bool wxsAuiToolBarItemBase::OnCanAddToParent(wxsParent* Parent,bool ShowMessage)
{
    if ( Parent->GetClassName() != _T("wxAuiToolBar") )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("wxAuiToolBarItems can only be added to an wxAuiToolBar."));
        }
        return false;
    }

	return true;
}
