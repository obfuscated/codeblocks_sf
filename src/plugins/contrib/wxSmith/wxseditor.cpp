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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxseditor.h"
#include "wxsresource.h"
#include "wxsmith.h"

#include <wx/wx.h>

wxsEditor::wxsEditor(wxWindow* parent, const wxString& title,wxsResource* Resource):
    EditorBase(parent,title),
    m_Resource(Resource)
{
    //active the "Resources" Tab in the "Management" Panel
    wxSmith::Get()->ShowResourcesTab();
}

wxsEditor::~wxsEditor()
{
    if ( m_Resource )
    {
        m_Resource->EditorClosed();
        m_Resource = 0;
    }
}
