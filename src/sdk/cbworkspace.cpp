/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "cbworkspace.h"
#include "manager.h"
#include "messagemanager.h"
#include "workspaceloader.h"
#include <wx/filedlg.h>
#include <wx/intl.h>

cbWorkspace::cbWorkspace(const wxString& filename)
{
	//ctor
    if (filename.IsEmpty())
	{
        wxString tmp;
        // if no filename given, use the default workspace
        tmp = wxGetHomeDir();
        tmp << "/.CodeBlocks";
        if (!wxDirExists(tmp))
            wxMkdir(tmp, 0755);
        tmp << "/" << DEFAULT_WORKSPACE;
        m_Filename = tmp;
        m_IsDefault = true;
	}
	else
	{
        m_Filename = filename;
        m_IsDefault = false;
    }
    Load();
}

cbWorkspace::~cbWorkspace()
{
	//dtor
}

void cbWorkspace::Load()
{
	Manager::Get()->GetMessageManager()->DebugLog("Loading workspace \"%s\"", m_Filename.GetFullPath().c_str());
	WorkspaceLoader wsp;
	m_IsOK = wsp.Open(m_Filename.GetFullPath()) || m_IsDefault;
	m_Title = wsp.GetTitle();
    SetModified(false);
}

bool cbWorkspace::Save(bool force)
{
    if (!force && !m_Modified)
        return true;

	Manager::Get()->GetMessageManager()->DebugLog("Saving workspace \"%s\"", m_Filename.GetFullPath().c_str());
	WorkspaceLoader wsp;
	bool ret = wsp.Save(m_Title, m_Filename.GetFullPath());
    SetModified(!ret);
    return ret;
}

bool cbWorkspace::SaveAs(const wxString& filename)
{
    wxFileDialog* dlg = new wxFileDialog(0,
                            _("Save workspace"),
                            m_Filename.GetPath(),
                            m_Filename.GetFullName(),
                            WORKSPACES_FILES_FILTER,
                            wxSAVE | wxHIDE_READONLY | wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() != wxID_OK)
        return false;
    m_Filename = dlg->GetPath();
    return Save(true);
}

void cbWorkspace::SetTitle(const wxString& title)
{
    m_Title = title;
    SetModified(true);
}

void cbWorkspace::SetModified(bool modified)
{
    m_Modified = modified;
}
