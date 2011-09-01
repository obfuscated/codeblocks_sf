/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
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

#include <wx/cmdproc.h>
#include <configmanager.h>
#include "cbEditorPanel.h"


cbEditorPanel::cbEditorPanel( const wxString& fileName, const wxString& /*title*/, FileContent *fc ):
    EditorBase( (wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook(), fileName ),
    m_IsOK(false),
    m_filecontent(fc)
    //m_cmdprocessor(0)
{
    if ( !m_filecontent ) return;
    /*m_cmdprocessor = */fc->CreateCommandProcessor();

    if ( !fileName.IsEmpty() )
        m_IsOK = m_filecontent->Open(GetFilename());

    // if !m_IsOK then it's a new file, so set the modified flag ON
    if (!m_IsOK || fileName.IsEmpty())
    {
        m_filecontent->SetModified(true);
        m_IsOK = false;
    }
}
cbEditorPanel::~cbEditorPanel()
{
    if ( m_filecontent ) delete m_filecontent;
}
bool cbEditorPanel::GetModified() const
{
    if ( !m_filecontent ) return false;
    return m_filecontent->GetModified();
}
void cbEditorPanel::SetModified(bool modified)
{
    if ( !m_filecontent ) return;
    m_filecontent->SetModified(modified);
}
bool cbEditorPanel::Save()
{
    if ( !m_filecontent ) return false;

    if (!m_IsOK)
        return SaveAs();

    bool ret = m_filecontent->Save(GetFilename());
    m_IsOK = ret;
    UpdateModified();
    return ret;
}

bool cbEditorPanel::SaveAs()
{
    wxFileName fname;
    fname.Assign(GetFilename());
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));

    wxString Path = fname.GetPath();
    wxString Extension = _T("nsd");

    if(mgr && Path.IsEmpty())
        Path = mgr->Read(_T("/file_dialogs/save_file_as/directory"), Path);

    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                      _("Save file"),
                      Path,
                      fname.GetFullName(),
                      m_filecontent->GetWildcard(),
                      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() != wxID_OK) // cancelled out
    {
        UpdateModified();
        return false;
    }

    SetFilename(dlg.GetPath());
    SetModified(true);
    m_IsOK = true;
    bool ret = m_filecontent->Save(GetFilename());
    UpdateModified();
    return ret;
}

bool cbEditorPanel::CanUndo()const
{
    return m_filecontent->GetCommandProcessor()->CanUndo();
}
bool cbEditorPanel::CanRedo()const
{
    return m_filecontent->GetCommandProcessor()->CanRedo();
}
void cbEditorPanel::Undo()
{
    m_filecontent->GetCommandProcessor()->Undo();
    //UpdateModified();
}
void cbEditorPanel::Redo()
{
    m_filecontent->GetCommandProcessor()->Redo();
    //UpdateModified();
}

void cbEditorPanel::UpdateModified()
{
    if ( GetModified() )
        SetTitle( _T("*") + GetShortName() );
    else
        SetTitle( GetShortName() );
}

void cbEditorPanel::SetFilename(const wxString& filename)
{
    m_Filename = filename;
    wxFileName fname;
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
}
