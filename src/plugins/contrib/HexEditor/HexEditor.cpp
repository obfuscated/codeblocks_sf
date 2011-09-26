/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
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
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
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
#include <wx/filedlg.h>
#include "HexEditor.h"
#include "HexEditPanel.h"

namespace
{
    PluginRegistrant<HexEditor> reg( _T("HexEditor") );

    const int idOpenHexEdit = wxNewId();
    const int idOpenWithHE = wxNewId();
    const int idOpenHexEditFileBrowser = wxNewId();
}

BEGIN_EVENT_TABLE( HexEditor, cbPlugin )
    EVT_MENU( idOpenHexEdit, HexEditor::OnOpenHexEdit )
    EVT_MENU( idOpenHexEditFileBrowser, HexEditor::OnOpenHexEditFileBrowser )
    EVT_MENU( idOpenWithHE,  HexEditor::OnOpenWithHE )
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
    if ( !appShutDown )
    {
        CloseMyEditors();
    }
}

bool HexEditor::BuildToolBar(wxToolBar* /*toolBar*/)
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

        case mtUnknown: //Assuming file explorer -- fileexplorer fills the filetreedata with ftdkFile or ftdkFolder as "kind", the file/folder selected is the "FullPath" of the entry
            if(data && data->GetKind()==FileTreeData::ftdkFile)  //right clicked on folder in file explorer
            {
                wxFileName f(data->GetFolder());
                m_browserselectedfile=f.GetFullPath();
                wxMenuItem* child = menu->FindItem( menu->FindItem( _("Open with") ) );
                if ( child && child->IsSubMenu() )
                {
                    menu = child->GetSubMenu();
                }
                menu->Append( idOpenHexEditFileBrowser, _( "Open With Hex Editor" ), _( "Open this file in hex editor" ) );
            }
            break;


        default:
            break;
    }
}

void HexEditor::BuildMenu(wxMenuBar* menuBar)
{
    int fileMenuIndex = menuBar->FindMenu( _("&File") );
    if ( fileMenuIndex == wxNOT_FOUND ) return;

    wxMenu* fileMenu = menuBar->GetMenu( fileMenuIndex );
    if ( !fileMenu ) return;

    wxMenuItemList& list = fileMenu->GetMenuItems();
    int pos = 0;
    for ( wxMenuItemList::iterator i = list.begin(); i != list.end(); ++i, ++pos )
    {
        wxMenuItem* item = *i;
        #if wxCHECK_VERSION(2, 9, 0)
        wxString label = item->GetItemLabelText();
        #else
        wxString label = item->GetLabel();
        #endif
        label.Replace( _T("_"), _T("") );
        if ( label.Contains( _("Open...")) )
        {
            fileMenu->Insert( pos+1, idOpenWithHE, _("Open with hex editor"), _("Open file using hex editor") );
            return;
        }
    }

    fileMenu->Append( idOpenWithHE, _("Open with hex editor"), _("Open file using hex editor") );
}

void HexEditor::OnOpenHexEditFileBrowser( wxCommandEvent& /*event*/ )
{
    OpenFileFromName(m_browserselectedfile);
}

void HexEditor::OnOpenHexEdit( wxCommandEvent& /*event*/ )
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
        OpenProjectFile( data->GetProjectFile() );
    }
}

void HexEditor::OpenProjectFile( ProjectFile* f )
{
    if ( !f ) return;

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

void HexEditor::OpenFileFromName(const wxString& fileName)
{
    if ( Manager::Get()->GetEditorManager()->IsOpen( fileName ) )
    {
        wxMessageBox( _("This file is already opened inside editor.") );
        return;
    }

    wxString title = wxFileName( fileName ).GetFullName();
    new HexEditPanel( fileName, title );
}

void HexEditor::OnOpenWithHE(wxCommandEvent& /*event*/)
{
    wxString file = ::wxFileSelector( _("Open file with HexEditor" ) );
    if ( file.IsEmpty() ) return;

    ProjectFile* f = FindProjectFile( file );
    if ( f )
    {
        OpenProjectFile( f );
    }
    else
    {
        OpenFileFromName( file );
    }
}

ProjectFile* HexEditor::FindProjectFile(const wxString& fileName)
{
    ProjectsArray* projects = ProjectManager::Get()->GetProjects();
    if ( !projects ) return 0;

    for ( size_t i=0; i<projects->Count(); ++i )
    {
        cbProject* project = (*projects)[i];
        if ( !project ) continue;
        ProjectFile* file = project->GetFileByFilename( fileName, false, false );
        if ( file ) return file;
    }

    return 0;
}


void HexEditor::CloseMyEditors()
{
    HexEditPanel::CloseAllEditors();
}
