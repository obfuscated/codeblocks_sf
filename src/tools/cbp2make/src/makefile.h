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
#ifndef MAKEFILE_H
#define MAKEFILE_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stlconfig.h"
//------------------------------------------------------------------------------

class CMakefileVariable
{
private:
    CString m_Name;
    CStringList m_Values;
    bool m_Multiline;
public:
    CString& Name(void)
    {
        return m_Name;
    }
    CStringList& Values(void)
    {
        return m_Values;
    }
    CString GetValue(const int Index = 0);
    void SetValue(const CString& NewValue, const int Index = 0);
    void AddValue(const CString& NewValue);
    bool& Multiline(void)
    {
        return m_Multiline;
    }
    CString JoinValues(void);
public:
    void Clear(void);
    void Show(void);
public:
    CMakefileVariable(void);
    ~CMakefileVariable(void);
};

class CMakefileRule
{
private:
    CString m_Target;
    CStringList m_Dependencies;
    CStringList m_Commands;
    bool m_Multiline;
public:
    CString& Target(void)
    {
        return m_Target;
    }
    CStringList& Dependencies(void)
    {
        return m_Dependencies;
    }
    CStringList& Commands(void)
    {
        return m_Commands;
    }
    bool& Multiline(void)
    {
        return m_Multiline;
    }
    CString JoinDependencies(void);
public:
    void Clear(void);
    void Show(void);
public:
    CMakefileRule(void);
    ~CMakefileRule(void);
};

class CMakefileSection
{
private:
    CStringList m_Header;
    std::vector<CMakefileVariable *> m_Macros;
    std::vector<CMakefileVariable *> m_EnvVars;
    std::vector<CMakefileRule *> m_Rules;
    CMakefileVariable m_NullVariable;
    CMakefileRule m_NullRule;
protected:
    CMakefileVariable *FindMacro(const CString& Name);
    CMakefileVariable *FindEnvVar(const CString& Name);
public:
    void Clear(void);
    void Show(void);
public:
    CStringList& Header(void);
    std::vector<CMakefileVariable *>& Macros(void);
    std::vector<CMakefileVariable *>& EnvVars(void);
    CMakefileVariable& AddMacro(const CString& Name, const CString& Value);
    CMakefileVariable& AddEnvVar(const CString& Name, const CString& Value);
    size_t RulesCount(void) const;
    CMakefileRule& GetRule(const size_t Index);
    CMakefileRule& AddRule(const CString& TargetName);
public:
    CMakefileSection(void);
    ~CMakefileSection(void);
};

class CMakefile
{
private:
    std::vector<CMakefileSection *> m_Sections;
    CStringList m_Text;
public:
    void Clear(void);
    void Show(void);
public:
    size_t SectionCount(void) const;
    CMakefileSection& GetSection(const size_t Section);
    CStringList& Header(const size_t Section = 0);
    CMakefileSection& AddSection(size_t *Section = 0);
    CMakefileVariable& AddMacro(const CString& Name, const CString& Value, const size_t Section = 0);
    CMakefileVariable& AddEnvVar(const CString& Name, const CString& Value, const size_t Section = 0);
    size_t RulesCount(const size_t Section = 0);
    CMakefileRule& GetRule(const size_t Index, const size_t Section = 0);
    CMakefileRule& AddRule(const CString& TargetName, const size_t Section = 0);
    CStringList& GetText(void);
    CStringList& Update(void);
public:
    CMakefile(void);
    ~CMakefile(void);
};

#endif
//------------------------------------------------------------------------------
