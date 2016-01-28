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
#ifndef DEPSEARCH_H
#define DEPSEARCH_H
//------------------------------------------------------------------------------
//#include <ostream>
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stringhash.h"
#include "platforms.h"
#include "makefile.h"
//------------------------------------------------------------------------------

class CDependencyInfo;

class CDependencyRecord
{
private:
    CString m_Name;
    hash_t m_Hash;
    std::vector<CDependencyRecord *> m_Dependencies;
    std::vector<CDependencyRecord *> m_CrossReferences;
    bool m_DependenciesComplete;
    bool m_CrossReferencesComplete;
    int m_Marker;
public:
    CString GetName(void) const
    {
        return m_Name;
    };
    void SetName(const CString& Name);
    hash_t Hash(void) const
    {
        return m_Hash;
    }
    size_t DependenciesCount(void) const;
    CDependencyRecord *Dependency(const size_t Index) const;
    void AddDependency(const CDependencyRecord *Record);
    size_t CrossReferencesCount(void) const;
    CDependencyRecord *CrossReference(const size_t Index) const;
    void AddCrossReference(const CDependencyRecord *Record);
    bool& DependenciesComplete(void)
    {
        return m_DependenciesComplete;
    }
    bool& CrossReferencesComplete(void)
    {
        return m_CrossReferencesComplete;
    }
    int& Marker(void)
    {
        return m_Marker;
    }
    void Clear(void);
    void Show(void);
public:
    CDependencyRecord(void);
    ~CDependencyRecord(void);
};

class CDependencyInfo
{
private:
    CPlatform m_Platform;
    std::vector<CDependencyRecord *> m_Records;
private:
    void ResetMarkers(void);
    size_t DependenciesCount(CDependencyRecord *Record);
    CStringList Dependencies(CDependencyRecord *Record);
    size_t CrossReferencesCount(CDependencyRecord *Record);
    CStringList CrossReferences(CDependencyRecord *Record);
public:
    CPlatform& Platform(void)
    {
        return m_Platform;
    }
    void Clear(void);
    void Show(void);
    CString OneLineReport(const size_t Index, const bool Deps, const bool XRefs);
    size_t RecordsCount(void) const;
    CString Name(const size_t Index) const;
    size_t DirectDependenciesCount(const size_t Index) const;
    size_t IndirectDependenciesCount(const size_t Index);
    size_t AllDependenciesCount(const size_t Index);
    CStringList DirectDependencies(const size_t Index) const;
    CStringList IndirectDependencies(const size_t Index);
    CStringList AllDependencies(const size_t Index);
    size_t DirectCrossReferencesCount(const size_t Index) const;
    size_t IndirectCrossReferencesCount(const size_t Index);
    size_t AllCrossReferencesCount(const size_t Index);
    CStringList DirectCrossReferences(const size_t Index) const;
    CStringList IndirectCrossReferences(const size_t Index);
    CStringList AllCrossReferences(const size_t Index);
    bool AreDependenciesComplete(const size_t Index);
    bool AreCrossReferencesComplete(const size_t Index);
    void SetDependenciesComplete(const size_t Index, const bool State = true);
    void SetCrossReferencesComplete(const size_t Index, const bool State = true);
    int FindRecord(const CString& Name);
    size_t AddRecord(const CString& Name);
    bool AddDependency(const size_t Index, const CString& DependencyName);
    size_t AddDependency(const CString& Name, const CString& DependencyName);
    void MakeRules(CMakefile& Makefile, const int Section, const bool Multiline);
public:
    CDependencyInfo(void);
    ~CDependencyInfo(void);
};

class CIncludeSearchFilter
{
protected:
    CStringList m_IncludeDirectories;
    CStringList m_DefinedMacros;
public:
    virtual void Clear(void);
    virtual void Show(void);
    virtual void Assign(const CIncludeSearchFilter& Filter);
    virtual bool Execute(const CString& FileName, CStringList& Includes);
    virtual bool Execute(const CString& FileName, CDependencyInfo& Dependencies);
    void AddIncludeDirectory(const CString& Path);
    void AddIncludeDirectories(const CStringList& Paths);
    void AddMacroDefiniton(const CString& Macro);
    void AddMacroDefinitons(const CStringList& Macros);
    CString ResolveIncludePath(const CString& IncludeName);
public:
    CIncludeSearchFilter(void);
    CIncludeSearchFilter(const CIncludeSearchFilter& Filter);
    ~CIncludeSearchFilter(void);
};

class CCppIncludeSearchFilter: public CIncludeSearchFilter
{
public:
    virtual void Assign(const CCppIncludeSearchFilter& Filter);
    virtual bool Execute(const CString& FileName, CStringList& Includes);
    virtual bool Execute(const CString& FileName, CDependencyInfo& Dependencies);
public:
    CCppIncludeSearchFilter(void);
    CCppIncludeSearchFilter(const CCppIncludeSearchFilter& Filter);
    ~CCppIncludeSearchFilter(void);
};

#endif
//------------------------------------------------------------------------------
