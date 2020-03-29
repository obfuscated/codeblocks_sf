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
*/

#ifndef wxsRICHTEXTCTRL_H
#define wxsRICHTEXTCTRL_H

#include    <wxswidget.h>
#include    <wxsitemresdata.h>
#include    <wxsarraystringproperty.h>
#include    <wxsenumproperty.h>
#include    <wxsboolproperty.h>
#include    <wxsbaseproperties.h>
#include    <wxspositionsizeproperty.h>

#include    <wx/richtext/richtextctrl.h>

class wxsRichTextCtrl : public wxsWidget
{
    public:
        wxsRichTextCtrl(wxsItemResData* Data);

    protected:
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode();
        virtual void OnEnumWidgetProperties(long Flags);

        wxSize          mSize;                          // used to check for default size
        wxArrayString   mText;                          // initial text contents
        wxsPositionSizeData     mVirtualSize;
};

#endif      // wxsRICHTEXTCTRL_H
