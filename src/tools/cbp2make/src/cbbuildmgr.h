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
#ifndef CBBUILDMGR_H
#define CBBUILDMGR_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "cbproject.h"
#include "cbworkspace.h"
//#include "toolchains.h"
#include "cbbuildcfg.h"
//------------------------------------------------------------------------------

class TiXmlElement;

class CCodeBlocksBuildManager
{
private:
    CCodeBlocksWorkspace m_Workspace;
    CCodeBlocksProject m_Project;
    CCodeBlocksBuildConfig m_Config;
    bool m_ProjectLoaded;
    bool m_WorkspaceLoaded;
protected:
public:
    CPlatformSet& Platforms(void)
    {
        return m_Config.Platforms();
    }
    CToolChainSet& ToolChains(void)
    {
        return m_Config.ToolChains();
    }
    CCodeBlocksBuildConfig& Config(void)
    {
        return m_Config;
    }
    void Clear(void);
    bool LoadProjectOrWorkspace(const CString& FileName);
    void Show(void);
    void GenerateMakefile(const CString& FileName);
public:
    CCodeBlocksBuildManager(void);
    ~CCodeBlocksBuildManager(void);
};

#endif
//------------------------------------------------------------------------------
