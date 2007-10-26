/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2007  olivetti (fabricio.olivetti at gmail dot com)
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

#ifndef WXSFLATNOTEBOOK_H
#define WXSFLATNOTEBOOK_H

#include <wxwidgets/wxscontainer.h>


class wxsFlatNotebook : public wxsContainer
{
    public:
        wxsFlatNotebook(wxsItemResData* Data);
    //    virtual ~wxsFlatNotebook();
    protected:
    private:
        virtual void OnEnumContainerProperties(long Flags);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual wxsPropertyContainer* OnBuildExtra();
        virtual wxString OnXmlGetExtraObjectClass();
        virtual void OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long PreviewFlags);
        virtual void OnBuildCreatingCode();
        virtual bool OnMouseClick(wxWindow* Preview,int PosX,int PosY);
        virtual bool OnIsChildPreviewVisible(wxsItem* Child);
        virtual bool OnEnsureChildPreviewVisible(wxsItem* Child);
        virtual void OnPreparePopup(wxMenu* Menu);
        virtual bool OnPopup(long Id);

        void UpdateCurrentSelection();

        wxsItem* m_CurrentSelection;
};



#endif // WXSFLATNOTEBOOK_H
