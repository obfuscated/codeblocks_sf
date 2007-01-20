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

#include "wxsresource.h"
#include "wxsextresmanager.h"
#include "wxsresourcetreeitemdata.h"

#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <editormanager.h>

namespace
{
    // This object will cause resource to open itself
    // when it's clicked in resource tree
    class wxsResourceOpening: public wxsResourceTreeItemData
    {
        public:
            wxsResourceOpening(wxsResource* Resource):
                wxsResourceTreeItemData(),
                m_Resource(Resource)
            {
            }

        private:

            virtual void OnSelect()
            {
                m_Resource->EditOpen();
            }

            wxsResource* m_Resource;
    };
}

wxsResource::wxsResource(wxsProject* Owner,const wxString& ResourceType,const wxString& GUI):
    m_ResourceType(ResourceType),
    m_ResourceName(wxEmptyString),
    m_GUI(GUI),
    m_Owner(Owner),
    m_Editor(NULL),
    m_Language(wxsCPP)
{}

wxsResource::~wxsResource()
{
    if ( m_Editor )
    {
        wxsEditor* EditorStore = m_Editor;
        m_Editor = NULL;
        EditorStore->Close();
    }
}

void wxsResource::EditOpen()
{
    if ( m_Editor )
    {
        m_Editor->Activate();
    }
    else
    {
        m_Editor = OnCreateEditor(Manager::Get()->GetEditorManager()->GetNotebook());
    }
}

void wxsResource::EditClose()
{
    if ( m_Editor )
    {
        m_Editor->Close();
        // Inside Close() m_Editor should be zeroed
    }
}

void wxsResource::EditorClosed()
{
    m_Editor = NULL;
    if ( !m_Owner )
    {
        wxsExtRes()->EditorClosed(this);
    }
}

void wxsResource::BuildTreeEntry(const wxsResourceItemId& Parent)
{
    m_TreeItemId = wxsTree()->AppendItem(
        Parent,
        GetResourceName(),
        -1, -1,
        new wxsResourceOpening(this));
}

bool wxsResource::ReadConfig(const TiXmlElement* Node)
{
    m_ResourceName = cbC2U(Node->Attribute("name"));
    m_Language = wxsCodeMarks::Id(cbC2U(Node->Attribute("language")));
    if ( GetResourceName().empty() ) return false;
    return OnReadConfig(Node);
}

bool wxsResource::WriteConfig(TiXmlElement* Node)
{
    bool Result = OnWriteConfig(Node);
    Node->SetAttribute("name",cbU2C(m_ResourceName));
    Node->SetAttribute("language",cbU2C(wxsCodeMarks::Name(m_Language)));
    return Result;
}

IMPLEMENT_CLASS(wxsResource,wxObject)
