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

#ifndef WXSSTATUSBAR_H
#define WXSSTATUSBAR_H

#include "../wxstool.h"

class wxsStatusBar: public wxsTool
{
    public:

        wxsStatusBar(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage);
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        virtual void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        void UpdateArraysSize(int Size);

        WX_DEFINE_ARRAY(wxPGId,IdsArray);

        int         m_Fields;
        wxArrayInt  m_Widths;
        wxArrayInt  m_Styles;
        wxArrayBool m_VarWidth;

        wxPGId     m_FieldsId;
        IdsArray   m_ParentIds;
        IdsArray   m_WidthsIds;
        IdsArray   m_StylesIds;
        IdsArray   m_VarWidthIds;
};



#endif
