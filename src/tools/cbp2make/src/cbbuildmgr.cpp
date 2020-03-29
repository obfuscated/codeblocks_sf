/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#include "stlfutils.h"
#include "cbbuildmgr.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CCodeBlocksBuildManager::CCodeBlocksBuildManager(void)
{
    Clear();
}

CCodeBlocksBuildManager::~CCodeBlocksBuildManager(void)
{
    Clear();
}

void CCodeBlocksBuildManager::Clear(void)
{
    m_ProjectLoaded = false;
    m_WorkspaceLoaded = false;
    m_Project.Clear();
    m_Workspace.Clear();
}

bool CCodeBlocksBuildManager::LoadProjectOrWorkspace(const CString& FileName)
{
    bool result = false;
    TiXmlDocument cbpw;
    result = cbpw.LoadFile(FileName.GetCString());
    if (!result) return false;
    Clear();
    const TiXmlElement *root = cbpw.RootElement();
    if (0==strcmp(root->Value(),"CodeBlocks_project_file")) {
        m_Project.Read(root);
        m_ProjectLoaded = true;
        result = true;
    } else if (0==strcmp(root->Value(),"CodeBlocks_workspace_file")) {
        m_Workspace.Read(root);
        m_Workspace.LoadWorkspaceProjects(ExtractFilePath(FileName));
        m_WorkspaceLoaded = true;
        result = true;
    }
    return result;
}

void CCodeBlocksBuildManager::Show(void)
{
    if (m_ProjectLoaded) {
        std::cout<<"Loaded file is Code::Blocks project."<<std::endl;
        m_Project.Show();
    } else if (m_WorkspaceLoaded) {
        std::cout<<"Loaded file is Code::Blocks workspace."<<std::endl;
        m_Workspace.Show();
    } else {
        std::cout<<"No project or workspace loaded."<<std::endl;
    }
}

void CCodeBlocksBuildManager::GenerateMakefile(const CString& FileName)
{
    if (m_ProjectLoaded) {
        m_Project.GenerateMakefile(FileName,m_Config);
    }
    if (m_WorkspaceLoaded) {
        m_Workspace.GenerateMakefile(FileName,m_Config);
    }
}

//------------------------------------------------------------------------------
