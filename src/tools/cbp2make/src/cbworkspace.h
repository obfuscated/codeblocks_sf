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
#ifndef CBWORKSPACE_H
#define CBWORKSPACE_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "cbproject.h"
//------------------------------------------------------------------------------

class TiXmlElement;
class CToolChainSet;

class CCodeBlocksWorkspace;
class CProjectWeightComparison;

class CWorkspaceUnit
{
    friend class CCodeBlocksWorkspace;
    friend class CProjectWeightComparison;
private:
    CString m_FileName;
    CStringList m_Depends;
    CCodeBlocksProject m_Project;
    int m_Weight;
public:
    void Clear(void);
    void Read(const TiXmlElement* UnitRoot);
    void Show(void);
    bool LoadProject(const CString& WorkspacePath);
public:
    CWorkspaceUnit(void);
    ~CWorkspaceUnit(void);
};

class CCodeBlocksWorkspace
{
private:
    CString m_Title;
    std::vector<CWorkspaceUnit *> m_Units;
    CStringList m_TargetNames;
    CStringList m_MakefileNames;
    CStringList m_MakefilePaths;
    CStringList m_TargetDeps;
    CStringList m_MakefileText;
    CMakefile m_Makefile;
protected:
    int CalculateProjectWeight(const size_t Index = 0);
    void ResolveProjectDependencies(void);
    void SortProjectsByWeight(void);
public:
    void Clear(void);
    void Read(const TiXmlElement* WorkspaceRoot);
    bool LoadWorkspaceProjects(const CString& WorkspacePath);
    bool LoadWorkspace(const CString& FileName);
    void Show(const bool ShowProjects = false);
    bool GenerateMakefile(const CString& FileName, CCodeBlocksBuildConfig& Config);
    void GenerateMakefileText(const CString& FileName, CCodeBlocksBuildConfig& Config);
public:
    CCodeBlocksWorkspace(void);
    ~CCodeBlocksWorkspace(void);
};

#endif
//------------------------------------------------------------------------------
