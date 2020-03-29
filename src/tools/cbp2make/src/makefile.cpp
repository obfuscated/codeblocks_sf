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
#include "tinyxml.h"
#include "stlconvert.h"
#include "cbhelper.h"
#include "makefile.h"
//------------------------------------------------------------------------------


CMakefileVariable::CMakefileVariable(void)
{
    Clear();
}

CMakefileVariable::~CMakefileVariable(void)
{
    Clear();
}

CString CMakefileVariable::GetValue(const int Index)
{
    return m_Values.GetString(Index);
}

void CMakefileVariable::SetValue(const CString& NewValue, const int Index)
{
    while (Index >= m_Values.GetCount()) {
        m_Values.Insert("");
    }
    m_Values[Index] = NewValue;
}

CString CMakefileVariable::JoinValues(void)
{
    if (m_Multiline) return m_Values.Join(" \\\n\t");
    else return m_Values.Join(" ");
}

void CMakefileVariable::AddValue(const CString& NewValue)
{
    m_Values.Insert(NewValue);
}

void CMakefileVariable::Clear(void)
{
    m_Name.Clear();
    m_Values.Clear();
    m_Multiline = false;
}

void CMakefileVariable::Show(void)
{
    std::cout<<"Variable: "<<m_Name.GetString()<<std::endl;
    ShowStringList("Values","Value",m_Values);
}

//------------------------------------------------------------------------------

CMakefileRule::CMakefileRule(void)
{
    Clear();
}

CMakefileRule::~CMakefileRule(void)
{
    Clear();
}

void CMakefileRule::Clear(void)
{
    m_Target.Clear();
    m_Dependencies.Clear();
    m_Commands.Clear();
    m_Multiline = false;
}

CString CMakefileRule::JoinDependencies(void)
{
    if (m_Multiline) return m_Dependencies.Join(" \\\n\t");
    else return m_Dependencies.Join(" ");
}

void CMakefileRule::Show(void)
{
    std::cout<<"Target: "<<m_Target.GetString()<<std::endl;
    ShowStringList("Dependencies","Dependency",m_Dependencies);
    ShowStringList("Commands","Command",m_Commands);
}

//------------------------------------------------------------------------------

CMakefileSection::CMakefileSection(void)
{
    Clear();
}

CMakefileSection::~CMakefileSection(void)
{
    Clear();
}

void CMakefileSection::Clear(void)
{
    m_Header.Clear();
    for (size_t i = 0; i < m_Macros.size(); i++) delete m_Macros[i];
    m_Macros.clear();
    for (size_t i = 0; i < m_EnvVars.size(); i++) delete m_EnvVars[i];
    m_EnvVars.clear();
    for (size_t i = 0; i < m_Rules.size(); i++) delete m_Rules[i];
    m_Rules.clear();
}

void CMakefileSection::Show(void)
{
    std::cout<<"Header:"<<std::endl;
    m_Header.Print(std::cout);
    std::cout<<"Macro variables: "<<m_Macros.size()<<std::endl;
    for (size_t i = 0; i < m_Macros.size(); i++) {
        CMakefileVariable& v = *m_Macros[i];
        std::cout<<"Macro #"<<(i+1)<<": "<<v.Name().GetString()<<" = "
                 <<v.JoinValues().GetString()<<std::endl;
    }
    std::cout<<"Environment variables: "<<m_EnvVars.size()<<std::endl;
    for (size_t i = 0; i < m_EnvVars.size(); i++) {
        CMakefileVariable& v = *m_EnvVars[i];
        std::cout<<"Variable #"<<(i+1)<<": "<<v.Name().GetString()<<" = "
                 <<v.JoinValues().GetString()<<std::endl;
    }
    std::cout<<"Rules: "<<m_Rules.size()<<std::endl;
    for (size_t i = 0; i < m_Rules.size(); i++) {
        CMakefileRule& r = *m_Rules[i];
        r.Show();
    }
}

CStringList& CMakefileSection::Header(void)
{
    return m_Header;
}

std::vector<CMakefileVariable *>& CMakefileSection::Macros(void)
{
    return m_Macros;
}

std::vector<CMakefileVariable *>& CMakefileSection::EnvVars(void)
{
    return m_EnvVars;
}

CMakefileVariable *CMakefileSection::FindMacro(const CString& Name)
{
    for (size_t i = 0; i < m_Macros.size(); i++) {
        CMakefileVariable *v = m_Macros[i];
        if (Name == v->Name()) return v;
    }
    return 0;
}

CMakefileVariable *CMakefileSection::FindEnvVar(const CString& Name)
{
    for (size_t i = 0; i < m_EnvVars.size(); i++) {
        CMakefileVariable *v = m_EnvVars[i];
        if (Name == v->Name()) return v;
    }
    return 0;
}

CMakefileVariable& CMakefileSection::AddMacro(const CString& Name, const CString& Value)
{
    if (!Name.IsEmpty()) { //if (!Value.IsEmpty())
        CMakefileVariable *v = FindMacro(Name);
        if (0==v) {
            v = new CMakefileVariable;
            m_Macros.push_back(v);
        }
        v->Name() = Name;
        v->SetValue(Value);
        return *v;
    }
    return m_NullVariable;
}

CMakefileVariable& CMakefileSection::AddEnvVar(const CString& Name, const CString& Value)
{
    if (!Name.IsEmpty()) { //if (!Value.IsEmpty())
        CMakefileVariable *v = FindEnvVar(Name);
        if (0==v) {
            v = new CMakefileVariable;
            m_EnvVars.push_back(v);
        }
        v->Name() = Name;
        v->SetValue(Value);
        return *v;
    }
    return m_NullVariable;
}

size_t CMakefileSection::RulesCount(void) const
{
    return m_Rules.size();
}

CMakefileRule& CMakefileSection::GetRule(const size_t Index)
{
    if (Index<m_Rules.size()) {
        return *m_Rules[Index];
    }
    return m_NullRule;
}

CMakefileRule& CMakefileSection::AddRule(const CString& TargetName)
{
    CMakefileRule *r = 0;
    for (size_t i = 0; i < m_Rules.size(); i++) {
        r = m_Rules[i];
        if (r->Target() == TargetName) {
            return *r;
        }
    }
    r = new CMakefileRule();
    r->Target() = TargetName;
    m_Rules.push_back(r);
    return *r;
}

//------------------------------------------------------------------------------

CMakefile::CMakefile(void)
{
    Clear();
    m_Sections.push_back(new CMakefileSection);
}

CMakefile::~CMakefile(void)
{
    Clear();
}

void CMakefile::Clear(void)
{
    for (size_t i = 0; i < m_Sections.size(); i++) delete m_Sections[i];
    m_Sections.clear();
    m_Text.Clear();
}

void CMakefile::Show(void)
{
//m_Header.Print(std::cout);
}

size_t CMakefile::SectionCount(void) const
{
    return m_Sections.size();
}

CMakefileSection& CMakefile::GetSection(const size_t Section)
{
    while (Section >= m_Sections.size()) {
        AddSection();
    }
    return *m_Sections[Section];
    /*
    if (m_Sections.size() > 0)
    {
     if (Section < m_Sections.size())
     {
      return *m_Sections[Section];
     }
     return *m_Sections[0];
    }
    else
    {
     return AddSection();
    }
    */
}

CStringList& CMakefile::Header(const size_t Section)
{
    return GetSection(Section).Header();
}

CMakefileSection& CMakefile::AddSection(size_t *Section)
{
    CMakefileSection *section = new CMakefileSection;
    m_Sections.push_back(section);
    if (0!=Section) *Section = m_Sections.size()-1;
    return *section;
}

CMakefileVariable& CMakefile::AddMacro(const CString& Name, const CString& Value, const size_t Section)
{
    return GetSection(Section).AddMacro(Name,Value);
}

CMakefileVariable& CMakefile::AddEnvVar(const CString& Name, const CString& Value, const size_t Section)
{
    return GetSection(Section).AddEnvVar(Name,Value);
}

size_t CMakefile::RulesCount(const size_t Section)
{
    return GetSection(Section).RulesCount();
}

CMakefileRule& CMakefile::GetRule(const size_t Index, const size_t Section)
{
    return GetSection(Section).GetRule(Index);
}

CMakefileRule& CMakefile::AddRule(const CString& TargetName, const size_t Section)
{
    return GetSection(Section).AddRule(TargetName);
}

CStringList& CMakefile::GetText(void)
{
    return m_Text;
}

CStringList& CMakefile::Update(void)
{
    CStringList phony_targets;
//
    m_Text.Clear();
    for (size_t i = 0; i < m_Sections.size(); i++) {
        CMakefileSection& section = *m_Sections[i];
        // add header
        if (section.Header().GetLength() > 0) {
            m_Text.Insert(section.Header()).Insert("");
        }
        // add macro variables
        std::vector<CMakefileVariable *>& macros = section.Macros();
        for (size_t j = 0; j < macros.size(); j++) {
            CMakefileVariable& v = *macros[j];
            v.Values().RemoveEmpty();
            m_Text.Insert(v.Name()+" = "+v.JoinValues());
        }
        if (macros.size() > 0) {
            m_Text.Insert("");
        }
        // add environment variables
        std::vector<CMakefileVariable *>& env_vars = section.EnvVars();
        for (size_t j = 0; j < env_vars.size(); j++) {
            CMakefileVariable& v = *env_vars[j];
            v.Values().RemoveEmpty();
            m_Text.Insert(v.Name()+" = "+v.JoinValues());
        }
        if (env_vars.size() > 0) {
            m_Text.Insert("");
        }
        // add rules
        for (size_t j = 0; j < section.RulesCount(); j++) {
            CMakefileRule& r = section.GetRule(j);
            m_Text.Insert(r.Target()+": "+r.JoinDependencies());
            for (int k = 0; k < r.Commands().GetCount(); k++) {
                m_Text.Insert("\t"+r.Commands().GetString(k));
            }
            m_Text.Insert("");
            if (0 == r.Dependencies().GetCount()) {
                phony_targets.Insert(r.Target());
            }
        }
    }
// add phony targets
    m_Text.Insert(".PHONY: "+phony_targets.Join(" "));
    m_Text.Insert("");
//
    return GetText();
}

//------------------------------------------------------------------------------
