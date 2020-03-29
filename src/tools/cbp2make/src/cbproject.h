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
#ifndef CBPROJECT_H
#define CBPROJECT_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stlconfig.h"
//#include "cbptarget.h"
//#include "cbpunit.h"
#include "depsearch.h"
#include "makefile.h"
//------------------------------------------------------------------------------

class CBuildTarget;
class CVirtualTarget;
class CBuildUnit;
class CToolChain;
class CCodeBlocksBuildConfig;

class TiXmlElement;

class CCodeBlocksProject
{
private:
    //bool m_ValidProject;
    int m_VersionMajor;
    int m_VersionMinor;
    CString m_Title;
    CString m_DefaultTarget;
    CString m_Compiler;
    bool m_ExtendedObjectNames;
    CStringList m_CompilerOptions;
    CStringList m_CompilerDirectories;
    CStringList m_LinkerOptions;
    CStringList m_LinkerLibraries;
    CStringList m_LinkerDirectories;
    CStringList m_ResourceCompilerDirectories;
    CStringList m_BeforeBuildCommands;
    CStringList m_AfterBuildCommands;
    bool m_ForceBeforeBuildCommands;
    bool m_ForceAfterBuildCommands;
    std::vector<CBuildTarget*> m_BuildTargets;
    std::vector<CVirtualTarget*> m_VirtualTargets;
    std::vector<CBuildUnit*> m_Units;
    CConfiguration m_Environment;
    CStringList m_Platforms;
    //
    bool m_SinglePlatform;
    std::vector<CBuildTarget*> m_BuildTargetIndex;
    std::vector<CVirtualTarget*> m_VirtualTargetIndex;
    std::vector<CBuildUnit*> m_UnitIndex;
    std::vector<CToolChain*> m_ToolChainIndex;
    std::vector<int> m_TargetToolChainIndex;
    //
    CDependencyInfo m_Dependencies;
    //CStringList m_MakefileText;
    CMakefile m_Makefile;
protected:
    //void DecorateTargetNames(void);
    void SortUnitsByWeight(void);
public:
    void Clear(void);
    void Read(const TiXmlElement *ProjectRoot);
    bool LoadProject(const CString& FileName);
    void Show(void);
private:
    void UpdatePlatformIndex(const size_t Platform, CCodeBlocksBuildConfig& Config);
    void UpdateTargetIndex(const int Target, CCodeBlocksBuildConfig& Config);
public:
    static CString DecorateVariableName(const CString& VariableName, const int Case = 0);
    static CString DecorateTargetName(const CString& TargetName, const int Case = 2);
    CString ToolChainSuffix(const int ToolChainIndex, CCodeBlocksBuildConfig& Config);
    //bool GenerateMakefileText(const CString& FileName, CCodeBlocksBuildConfig& Config);
    bool GenerateMakefile(const CString& FileName, CCodeBlocksBuildConfig& Config);
public:
    CCodeBlocksProject(void);
    ~CCodeBlocksProject(void);
};

#endif
//------------------------------------------------------------------------------
