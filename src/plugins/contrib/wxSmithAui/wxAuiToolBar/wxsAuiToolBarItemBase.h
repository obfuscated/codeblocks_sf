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

#ifndef WXSAUITOOLBARITEMBASE_H
#define WXSAUITOOLBARITEMBASE_H

#include <wxwidgets/wxsitem.h>

#include <prep.h>

/** \brief Base class for wxsAuiToolBarItem widgets */
class wxsAuiToolBarItemBase: public wxsItem
{
    public:

        wxWindowID m_ItemId;
        long m_HasGripper;
        int  m_GripperSize;

        wxsAuiToolBarItemBase(wxsItemResData* Data,const wxsItemInfo* Info,long PropertiesFlags,const wxsEventDesc* Events,const wxsStyleSet* StyleSet)
            : wxsItem(Data,Info,PropertiesFlags,Events,StyleSet)
        {}

    private:

        virtual void OnBuildDeclarationsCode() {};
        virtual void OnBuildCreatingCode() {};
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumItemProperties(cb_unused long Flags) {};
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);
};

#endif
