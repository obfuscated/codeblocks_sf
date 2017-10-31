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

#ifndef HEXEDITOR_H_INCLUDED
#define HEXEDITOR_H_INCLUDED

#include <cbplugin.h>

class ProjectFile;

class HexEditor : public cbPlugin
{
    public:

        HexEditor();
        virtual ~HexEditor();

        void NotifyEditorOpened( EditorBase* );

    protected:

        virtual void BuildMenu(wxMenuBar* menuBar);
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        virtual bool BuildToolBar(wxToolBar* toolBar);
        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);

        void OnOpenHexEdit( wxCommandEvent& event );
        void OnOpenHexEditFileBrowser( wxCommandEvent& event );
        void OnOpenWithHE ( wxCommandEvent& event );
        void CloseMyEditors();

        void OpenProjectFile( ProjectFile* file );
        void OpenFileFromName( const wxString& fileName );

        ProjectFile* FindProjectFile( const wxString& fileName );

        wxString m_browserselectedfile;

        DECLARE_EVENT_TABLE()
};


#endif
