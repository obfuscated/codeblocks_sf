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
#include "revision.h"
#include "macros.h"
#include "cbworkspace.h"
#include "cbbuildcfg.h"
#include "cbhelper.h"
#include "stlfutils.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CWorkspaceUnit::CWorkspaceUnit(void)
{
    Clear();
}

CWorkspaceUnit::~CWorkspaceUnit(void)
{
    Clear();
}

void CWorkspaceUnit::Clear(void)
{
    m_FileName.Clear();
    m_Depends.Clear();
    m_Project.Clear();
    m_Weight = 0;
}

void CWorkspaceUnit::Read(const TiXmlElement* UnitRoot)
{
    m_FileName = UnitRoot->Attribute("filename");
    const TiXmlNode *_depends = UnitRoot->FirstChild("Depends");
    while (0!=_depends) {
        const TiXmlElement *depends = _depends->ToElement();
        if (0!=depends) {
            char *value = (char *)depends->Attribute("filename");
            if (0!=value) {
                m_Depends.Insert(value);
            }
        }
        _depends = UnitRoot->IterateChildren(_depends);
    }
    m_Depends.RemoveDuplicates();
//m_Depends.RemoveEmpty();
}

void CWorkspaceUnit::Show(void)
{
#ifdef SHOW_MODE_ONELINE
    std::cout<<m_FileName.GetString();
    std::cout<<", Weight: "<<m_Weight;
    std::cout<<", Dependencies: "<<m_Depends.GetCount()<<std::endl;
    for (int i = 0, n = m_Depends.GetCount(); i < n; i++) {
        std::cout<<"Dependency #"<<(i+1)<<": "<<m_Depends[i].GetString()<<std::endl;
    }
#else
    std::cout<<"Filename: "<<m_FileName.GetString()<<std::endl;
    std::cout<<"Weight: "<<m_Weight<<std::endl;
    ShowStringList("Dependencies","Dependency",m_Depends);
#endif
}

bool CWorkspaceUnit::LoadProject(const CString& WorkspacePath)
{
    return m_Project.LoadProject(JoinPaths(WorkspacePath,m_FileName));
}

//------------------------------------------------------------------------------

CCodeBlocksWorkspace::CCodeBlocksWorkspace(void)
{
    Clear();
}

CCodeBlocksWorkspace::~CCodeBlocksWorkspace(void)
{
    Clear();
}

int CCodeBlocksWorkspace::CalculateProjectWeight(const size_t Index)
{
    CWorkspaceUnit *unit = m_Units[Index];
    if (unit->m_Depends.GetCount()) {
        if (unit->m_Weight) {
            return unit->m_Weight;
        }
        int weight = 0;
        for (int j = 0, nj = unit->m_Depends.GetCount(); j < nj; j++) {
            CString dependency = unit->m_Depends[j];
            for (size_t i = 0, ni = m_Units.size(); i < ni; i++) {
                if (i!=Index) {
                    if (unit->m_FileName == dependency) {
                        weight += CalculateProjectWeight(i);
                    }
                }
            }
        }
        weight++;
        return weight;
    }
    return 0;
}

void CCodeBlocksWorkspace::ResolveProjectDependencies(void)
{
    for (size_t i = 0; i < m_Units.size(); i++) {
        m_Units[i]->m_Weight = CalculateProjectWeight(i);
    }
}

class CProjectWeightComparison
{
public:
    bool operator ()(CWorkspaceUnit *const &AUnit, CWorkspaceUnit *const &BUnit)
    {
        return (AUnit->m_Weight < BUnit->m_Weight);
    }
};

void CCodeBlocksWorkspace::SortProjectsByWeight(void)
{
    CProjectWeightComparison cmp;
    std::sort(m_Units.begin(),m_Units.end(),cmp);
}

void CCodeBlocksWorkspace::Clear()
{
    m_Title.Clear();
    for (size_t i = 0; i < m_Units.size(); i++) {
        delete m_Units[i];
    }
    m_Units.clear();
    m_TargetNames.Clear();
    m_MakefileNames.Clear();
    m_MakefilePaths.Clear();
    m_TargetDeps.Clear();
    m_MakefileText.Clear();
    m_Makefile.Clear();
}

void CCodeBlocksWorkspace::Read(const TiXmlElement* WorkspaceRoot)
{
    const TiXmlNode *_workspace = WorkspaceRoot->FirstChild("Workspace");
    if (0!=_workspace) {
        const TiXmlElement *workspace = _workspace->ToElement();
        if (0!=workspace) {
            m_Title = workspace->Attribute("title");
            TiXmlNode *_project = (TiXmlNode *)_workspace->FirstChild("Project");
            while (0!=_project) {
                TiXmlElement* project = _project->ToElement();
                if (0!=project) {
                    CWorkspaceUnit *unit = new CWorkspaceUnit();
                    unit->Read(project);
                    m_Units.push_back(unit);
                }
                _project = (TiXmlNode *)_workspace->IterateChildren(_project);
            }
        }
    }
}

bool CCodeBlocksWorkspace::LoadWorkspace(const CString& FileName)
{
    bool result = false;
    TiXmlDocument cbw;
    result = cbw.LoadFile(FileName.GetCString());
    if (!result) return false;
    Clear();
    const TiXmlElement *root = cbw.RootElement();
    if (0==strcmp(root->Value(),"CodeBlocks_workspace_file")) {
        Read(root);
        result = true;
    }
//
    return result;
}

bool CCodeBlocksWorkspace::LoadWorkspaceProjects(const CString& WorkspacePath)
{
    bool result = true;
    for (size_t i = 0; i < m_Units.size(); i++) {
        result &= m_Units[i]->LoadProject(WorkspacePath);
    }
// resolve project dependencies and sort projects by weight
    ResolveProjectDependencies();
    SortProjectsByWeight();
    return result;
}

void CCodeBlocksWorkspace::Show(const bool ShowProjects)
{
    std::cout<<"Workspace title: "<<m_Title.GetString()<<std::endl;
    std::cout<<"Projects: "<<m_Units.size()<<std::endl;
    for (size_t i = 0; i < m_Units.size(); i++) {
        std::cout<<"Project #"<<(i+1)<<": ";
#ifndef SHOW_MODE_ONELINE
        std::cout<<std::endl;
#endif
        m_Units[i]->Show();
        if (ShowProjects) {
            m_Units[i]->m_Project.Show();
        }
    }
}

bool CCodeBlocksWorkspace::GenerateMakefile
(const CString& FileName, CCodeBlocksBuildConfig& Config)
{
// generate individual makefiles for projects
    CString cwd = GetCurrentDir();
    CString workspace_path = ExtractFilePath(FileName);
    if (!workspace_path.IsEmpty()) {
        ChangeDir(workspace_path);
    }
//
    m_TargetNames.Clear();
    m_MakefileNames.Clear();
    m_TargetDeps.Clear();
    for (size_t i = 0; i < m_Units.size(); i++) {
        CString project_name = m_Units[i]->m_FileName;
        //CString makefile_path = JoinPaths(workspace_path,ExtractFilePath(project_name));
        CString makefile_path = ExtractFilePath(project_name);
        //CString makefile_pathname = JoinPaths(workspace_path,project_name+".mak");
        CString makefile_pathname = project_name+".mak";
        CString target_name = ChangeFileExt(project_name,"");
        CString makefile_name = ExtractFileName(makefile_pathname);
        target_name = CCodeBlocksProject::DecorateTargetName(target_name,Config.TargetNameCase());
        CString target_deps;
        for (int j = 0; j < m_Units[i]->m_Depends.GetCount(); j++) {
            CString dep_name = ChangeFileExt(m_Units[i]->m_Depends[j],"");
            if (j>0) target_deps += " ";
            target_deps += CCodeBlocksProject::DecorateTargetName(dep_name,Config.TargetNameCase());
        }
        m_TargetDeps.Insert(target_deps);
        m_TargetNames.Insert(target_name);
        m_MakefilePaths.Insert(makefile_path);
        m_MakefileNames.Insert(makefile_name);
        makefile_pathname = MakeNativePath(makefile_pathname);
        //std::cout<<"cwd "<<cwd.GetCString()<<std::endl;
        //std::cout<<"gen_makefile "<<makefile_pathname.GetCString()<<std::endl;
        m_Units[i]->m_Project.GenerateMakefile(makefile_pathname,Config);
    }
// generate workspace makefile
    int active_platforms = 0;
    for (size_t pi = 0, pn = Config.Platforms().GetCount(); pi < pn; pi++) {
        if (Config.Platforms().Platform(pi)->Active()) active_platforms++;
    }
    bool single_platform = (1==active_platforms);
    for (size_t pi = 0, pn = Config.Platforms().GetCount(); pi < pn; pi++) {
        CPlatform *pl = Config.Platforms().Platform(pi);
        if (!pl->Active()) continue;
        CString makefile_path = ExtractFilePath(FileName);
        CString makefile_name = ExtractFileName(FileName);
        CString platform_suffix;
        if (!single_platform) platform_suffix = "."+LowerCase(pl->Name());
        makefile_name += platform_suffix;
        ChangeDir(makefile_path);
        // begin makefile
        m_Makefile.Clear();
        int section = 0;
        // head comment
        const int header_width = 80;
        m_Makefile.Header().Insert(FillStr("#",'-',"#",header_width));
#ifdef REVISION_NUMBER
        {
            CString rn = IntegerToString(REVISION_NUMBER);
            CString line = FillStr("# This makefile was generated by 'cbp2make' tool rev."+rn,' ',"#",header_width);
            m_Makefile.Header().Insert(line);
        }
#else
        CString line = FillStr("# This makefile was generated by 'cbp2make' tool rev.",' ',"#",header_width)
#endif
        m_Makefile.Header().Insert(FillStr("#",'-',"#",header_width));
        m_Makefile.Header().Insert("");
        section++;
        // macros
        CString line = pl->EvalWorkDir();
        m_Makefile.AddMacro("WRKDIR",line,section);
        m_Makefile.AddMacro("MAKE",pl->Tool_Make(),section);
        section++;
        // targets
        CMakefileRule& rule_all = m_Makefile.AddRule("all",section);
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            rule_all.Dependencies().Insert(m_TargetNames[i]);
        }
        section++;
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            CString makefile_path = m_MakefilePaths[i];
            CMakefileRule& rule_target = m_Makefile.AddRule(m_TargetNames[i],section);
            rule_target.Dependencies().Insert(m_TargetDeps[i]);
            line = "$(MAKE)";
            if (!makefile_path.IsEmpty()) {
                line += " -C "+pl->ProtectPath(pl->Pd(makefile_path),Config.QuotePathMode());
            }
            line += " all -f "+pl->ProtectPath(pl->Pd(m_MakefileNames[i])+platform_suffix,Config.QuotePathMode());
            rule_target.Commands().Insert(line);
        }
        section++;
        //
        CMakefileRule& rule_clean = m_Makefile.AddRule("clean",section);
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            rule_clean.Dependencies().Insert("clean_"+m_TargetNames[i]);
        }
        section++;
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            CString makefile_path = m_MakefilePaths[i];
            CMakefileRule& rule_clean_target = m_Makefile.AddRule("clean_"+m_TargetNames[i],section);
            line = "$(MAKE)";
            if (!makefile_path.IsEmpty()) {
                line += " -C "+pl->ProtectPath(pl->Pd(makefile_path),Config.QuotePathMode());
            }
            line += " clean -f "+pl->ProtectPath(pl->Pd(m_MakefileNames[i])+platform_suffix,Config.QuotePathMode());
            rule_clean_target.Commands().Insert(line);
        }
        // save makefile
        CStringList& text = m_Makefile.Update();
        text.SaveToFile(makefile_name);
        m_Makefile.Clear();
        ChangeDir(cwd);
    }
    return true;
}

void CCodeBlocksWorkspace::GenerateMakefileText(const CString& FileName,
        CCodeBlocksBuildConfig& Config)
{
// generate individual makefiles for projects
//std::cout<<"Workspace: "<<FileName.GetCString()<<std::endl;
    CString cwd = GetCurrentDir();
    CString workspace_path = ExtractFilePath(FileName);
    if (!workspace_path.IsEmpty()) {
        ChangeDir(workspace_path);
    }
//std::cout<<"Workspace path: "<<workspace_path.GetCString()<<std::endl;
    m_TargetNames.Clear();
    m_MakefileNames.Clear();
    for (size_t i = 0; i < m_Units.size(); i++) {
        CString project_name = m_Units[i]->m_FileName;
        CString makefile_path = JoinPaths(workspace_path,ExtractFilePath(project_name));
        CString makefile_pathname = JoinPaths(workspace_path,project_name+".mak");
        CString target_name = LowerCase(ChangeFileExt(project_name,""));
        CString makefile_name = ExtractFileName(makefile_pathname);
        //std::cout<<"Project name: "<<project_name.GetCString()<<std::endl;
        //std::cout<<"Makefile path: "<<makefile_path.GetCString()<<std::endl;
        //std::cout<<"Makefile pathname: "<<makefile_pathname.GetCString()<<std::endl;
        target_name = CCodeBlocksProject::DecorateTargetName(target_name);
        //std::cout<<"Target name: "<<target_name.GetCString()<<std::endl;
        //std::cout<<"Makefile name: "<<makefile_name.GetCString()<<std::endl;
        m_TargetNames.Insert(target_name);
        m_MakefilePaths.Insert(makefile_path);
        m_MakefileNames.Insert(makefile_name);
        makefile_pathname = MakeNativePath(makefile_pathname);
        //std::cout<<"Native makefile pathname: "<<makefile_pathname.GetCString()<<std::endl;
        m_Units[i]->m_Project.GenerateMakefile(makefile_pathname,Config);
    }
//CString cwd = GetCurrentDir();
// generate workspace makefile
    int active_platforms = 0;
    for (size_t pi = 0, pn = Config.Platforms().GetCount(); pi < pn; pi++) {
        if (Config.Platforms().Platform(pi)->Active()) active_platforms++;
    }
    bool single_platform = (1==active_platforms);
    for (size_t pi = 0, pn = Config.Platforms().GetCount(); pi < pn; pi++) {
        CPlatform *pl = Config.Platforms().Platform(pi);
        if (!pl->Active()) continue;
        CString makefile_path = ExtractFilePath(FileName);
        CString makefile_name = ExtractFileName(FileName);
        CString platform_suffix;
        if (!single_platform) platform_suffix = "."+LowerCase(pl->Name());
        makefile_name += platform_suffix;
        //ChangeDir(makefile_path);
        //
        m_MakefileText.Clear();
        // head comment
        m_MakefileText.Insert("#------------------------------------------------------------------------------#");
#ifdef REVISION_NUMBER
        {
            CString s = "# This makefile was generated by 'cbp2make' tool rev.                          #";
            CString n = IntegerToString(REVISION_NUMBER);
            int o = FindStr(s,"rev.") + 4;
            for (int i = 0; i < n.GetLength(); i++) s.SetChar(i+o,n[i]);
            m_MakefileText.Insert(s);
        }
#else
        m_MakefileText.Insert("# This makefile was generated by 'cbp2make' tool                               #");
#endif
        m_MakefileText.Insert("#------------------------------------------------------------------------------#");
        m_MakefileText.Insert("");
        // macros
        CString line = "WRKDIR = "+pl->EvalWorkDir();
        m_MakefileText.Insert(line);
        line = "MAKE = "+pl->Tool_Make();
        m_MakefileText.Insert(line);
        m_MakefileText.Insert("");
        // targets
        line = "all:";
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            line.Append(" ").Append(m_TargetNames[i]);
        }
        m_MakefileText.Insert(line);
        m_MakefileText.Insert("");
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            CString makefile_path = m_MakefilePaths[i];
            line = m_TargetNames[i]+":";
            m_MakefileText.Insert(line);
            line = "\t$(MAKE)";
            if (!makefile_path.IsEmpty()) {
                line += " -C "+pl->ProtectPath(pl->Pd(makefile_path),Config.QuotePathMode());
            }
            line += " all -f "+pl->ProtectPath(pl->Pd(m_MakefileNames[i])+platform_suffix,Config.QuotePathMode());
            m_MakefileText.Insert(line);
            m_MakefileText.Insert("");
        }
        //
        line = "clean:";
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            line.Append(" clean_").Append(m_TargetNames[i]);
        }
        m_MakefileText.Insert(line);
        m_MakefileText.Insert("");
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            CString makefile_path = m_MakefilePaths[i];
            line = "clean_"+m_TargetNames[i]+":";
            m_MakefileText.Insert(line);
            line = "\t$(MAKE)";
            if (!makefile_path.IsEmpty()) {
                line += " -C "+pl->ProtectPath(pl->Pd(makefile_path),Config.QuotePathMode());
            }
            line += " clean -f "+pl->ProtectPath(pl->Pd(m_MakefileNames[i])+platform_suffix,Config.QuotePathMode());
            m_MakefileText.Insert(line);
            m_MakefileText.Insert("");
        }
        //
        line = ".PHONY:";
        for (int i = 0; i < m_TargetNames.GetCount(); i++) {
            line.Append(" ").Append(m_TargetNames[i]);
            line.Append(" clean_").Append(m_TargetNames[i]);
        }
        m_MakefileText.Insert(line);
        m_MakefileText.Insert("");
        m_MakefileText.SaveToFile(makefile_name);
        m_MakefileText.Clear();
        ChangeDir(cwd);
    }
}

//------------------------------------------------------------------------------
