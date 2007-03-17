/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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

#include "wxsresourcetree.h"
#include "wxsresourcetreeitemdata.h"

#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/intl.h>

#include "configmanager.h"
#include "globals.h"

namespace
{
    class wxsResourceTreeProjectData: public wxsResourceTreeItemData
    {
        public:
            wxsResourceTreeProjectData(wxsProject* Project): m_Project(Project) {}
            wxsProject* m_Project;
            virtual void OnRightClick()
            {

            }

    };
}

wxsResourceTree* wxsResourceTree::m_Singleton = NULL;
int wxsResourceTree::m_RootImageId = LoadImage(_T("/images/wxsmith/wxSmith16.png"));
int wxsResourceTree::m_ProjectImageId = LoadImage(_T("/images/codeblocks.png"));
int wxsResourceTree::m_ExternalImageId = LoadImage(_T("/images/wxsmith/deletewidget16.png"));

wxsResourceTree::wxsResourceTree(wxWindow* Parent): wxTreeCtrl(Parent,-1), m_IsExt(false), m_BlockCount(0)
{
    m_Singleton = this;
    SetImageList(&GetGlobalImageList());
    Expand(AddRoot(_("Resources"),m_RootImageId));
}

wxsResourceTree::~wxsResourceTree()
{
    if ( m_Singleton == this ) m_Singleton = NULL;
}

wxsResourceItemId wxsResourceTree::NewProjectItem(const wxString& ProjectTitle,wxsProject* Project)
{
    wxsResourceItemId Id;
    if ( !m_IsExt )
    {
        Id = AppendItem(GetRootItem(),
            ProjectTitle,m_ProjectImageId,m_ProjectImageId,
            new wxsResourceTreeProjectData(Project));
    }
    else
    {
        Id = InsertItem(GetRootItem(),GetChildrenCount(GetRootItem(),false),
            ProjectTitle,m_ProjectImageId,m_ProjectImageId,
            new wxsResourceTreeProjectData(Project));
    }

    Expand(GetRootItem());
    return Id;
}

wxsResourceItemId wxsResourceTree::ExternalResourcesId()
{
    if ( !m_IsExt )
    {
        m_ExtId = AppendItem(GetRootItem(),
            _("External resources"),m_ExternalImageId,m_ExternalImageId,
            NULL);
        m_IsExt = true;
    }
    return m_ExtId;
}

void wxsResourceTree::DeleteExternalResourcesId()
{
    if ( m_IsExt )
    {
        Delete(m_ExtId);
        m_IsExt = false;
    }
}

void wxsResourceTree::OnSelect(wxTreeEvent& event)
{
    if ( m_BlockCount ) return;
    wxsResourceTreeItemData* Data = (wxsResourceTreeItemData*)GetItemData(event.GetItem());
    if ( Data ) Data->OnSelect();
}

void wxsResourceTree::OnRightClick(wxTreeEvent& event)
{
    wxsResourceTreeItemData* Data = (wxsResourceTreeItemData*)GetItemData(event.GetItem());
    if ( Data ) Data->OnRightClick();
}

wxImageList& wxsResourceTree::GetGlobalImageList()
{
    static wxImageList List(16,16);
    static bool FirstTime = true;
    if ( FirstTime )
    {
        // Adding some zero-index image
        List.Add(wxBitmap(wxImage(16,16)),*wxBLACK);

        FirstTime = false;
    }
    return List;
}

int wxsResourceTree::LoadImage(const wxString& FileName)
{
    wxBitmap Bmp(cbLoadBitmap(ConfigManager::GetDataFolder()+FileName,wxBITMAP_TYPE_ANY));
    return InsertImage(Bmp);
}

int wxsResourceTree::InsertImage(const wxBitmap& Bitmap)
{
    if ( !Bitmap.Ok() )
    {
        return 0;
    }

    int Index = 0;
    do
    {
        if ( GetFreedList().IsEmpty() )
        {
            return GetGlobalImageList().Add(Bitmap);
        }
        Index = GetFreedList().Last();
        GetFreedList().RemoveAt(GetFreedList().Count()-1);
    }
    while ( Index<=0 || Index>=GetGlobalImageList().GetImageCount() );

    if ( GetGlobalImageList().Replace(Index,Bitmap) )
    {
        return Index;
    }

    return GetGlobalImageList().Add(Bitmap);
}

wxArrayInt& wxsResourceTree::GetFreedList()
{
    static wxArrayInt List;
    return List;
}

void wxsResourceTree::FreeImage(int Index)
{
    GetFreedList().Add(Index);
}

void wxsResourceTree::BlockSelect()
{
    m_BlockCount++;
}

void wxsResourceTree::UnblockSelect()
{
    m_BlockCount--;
}

BEGIN_EVENT_TABLE(wxsResourceTree,wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(wxID_ANY,wxsResourceTree::OnSelect)
    EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,wxsResourceTree::OnRightClick)
END_EVENT_TABLE()
