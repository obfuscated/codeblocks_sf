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
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#include <sdk.h>
#include <configurationpanel.h>
#include <manager.h>
#include <logmanager.h>
#include <projectmanager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <cbproject.h>
#include <wx/menu.h>
#include "HexEditor.h"
#include "HexEditPanel.h"

namespace
{
    PluginRegistrant<HexEditor> reg( _T("HexEditor") );

    const int idOpenHexEdit = wxNewId();
}

BEGIN_EVENT_TABLE( HexEditor, cbPlugin )
    EVT_MENU( idOpenHexEdit, HexEditor::OnOpenHexEdit )
END_EVENT_TABLE()

HexEditor::HexEditor()
{
//    if ( !Manager::LoadResource(_T("HexEditor.zip")) )
//    {
//        NotifyMissingFile(_T("HexEditor.zip"));
//    }
}

HexEditor::~HexEditor()
{
}

void HexEditor::OnAttach()
{
}

void HexEditor::OnRelease(bool appShutDown)
{
}

bool HexEditor::BuildToolBar(wxToolBar* toolBar)
{
    return false;
}

void HexEditor::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    if ( !menu || !IsAttached() ) return;

    switch ( type )
    {
        case mtProjectManager:

            if ( data && data->GetKind()==FileTreeData::ftdkFile )
            {
                wxMenuItem* child = menu->FindItem( menu->FindItem( _("Open with") ) );
                if ( child && child->IsSubMenu() )
                {
                    menu = child->GetSubMenu();
                }

                menu->AppendSeparator();
                menu->Append( idOpenHexEdit, _( "Hex editor" ), _( "Open this file in hex editor" ) );
            }
            break;
    }
}

void HexEditor::BuildMenu(wxMenuBar* menuBar)
{
}

void HexEditor::OnOpenHexEdit( wxCommandEvent& event )
{
    wxTreeCtrl *tree = Manager::Get()->GetProjectManager()->GetTree();

    if ( !tree )
        return;

    wxTreeItemId treeItem =  tree->GetSelection();

    if ( !treeItem.IsOk() )
        return;

    const FileTreeData *data = static_cast<FileTreeData*>( tree->GetItemData( treeItem ) );

    if ( !data )
        return;

    if ( data->GetKind() == FileTreeData::ftdkFile )
    {
        ProjectFile* f = data->GetProject()->GetFile( data->GetFileIndex() );
        if ( f )
        {
            if ( Manager::Get()->GetEditorManager()->IsOpen( f->file.GetFullPath() ) )
            {
                wxMessageBox( _("This file is already opened inside editor.") );
                return;
            }

            wxString title;
            if ( Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/tab_text_relative"), true) )
            {
                title = f->relativeToCommonTopLevelPath;
            }
            else
            {
                title = f->file.GetFullName();
            }

            new HexEditPanel( f->file.GetFullPath(), title );
        }
    }
}
