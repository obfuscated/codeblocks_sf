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
#include "cbhelper.h"

#include "stlfutils.h"
#include "depsearch.h"
//------------------------------------------------------------------------------

CDependencyRecord::CDependencyRecord(void)
{
    Clear();
}

CDependencyRecord::~CDependencyRecord(void)
{
    Clear();
}

void CDependencyRecord::SetName(const CString& Name)
{
    m_Name = Name;
    m_Hash = sdbm_hash((unsigned char *)Name.GetCString(),Name.GetLength());
}

void CDependencyRecord::Clear(void)
{
    m_Hash = 0;
    m_Name.Clear();
    m_Dependencies.clear();
    m_CrossReferences.clear();
    m_DependenciesComplete = false;
    m_CrossReferencesComplete = false;
    m_Marker = 0;
}

void CDependencyRecord::Show(void)
{
    std::cout<<"Unit: "<<m_Name.GetCString()<<std::endl;
    if (m_Dependencies.size() > 0) {
        std::cout<<"Unit dependens on "<<m_Dependencies.size()<<" other units."<<std::endl;
        for (size_t i = 0; i < m_Dependencies.size(); i++) {
            CDependencyRecord *r = m_Dependencies[i];
            std::cout<<"Dependency #"<<(i+1)<<": "<<r->GetName().GetCString()<<std::endl;
        }
    } else {
        std::cout<<"Unit has no dependencies."<<std::endl;
    }
    if (m_CrossReferences.size() > 0) {
        std::cout<<"Unit is used by"<<m_CrossReferences.size()<<" other units."<<std::endl;
        for (size_t i = 0; i < m_CrossReferences.size(); i++) {
            CDependencyRecord *r = m_CrossReferences[i];
            std::cout<<"Cross reference #"<<(i+1)<<": "<<r->GetName().GetCString()<<std::endl;
        }
    } else {
        std::cout<<"Unit is not used by any other unit."<<std::endl;
    }
}

size_t CDependencyRecord::DependenciesCount(void) const
{
    return m_Dependencies.size();
}

CDependencyRecord *CDependencyRecord::Dependency(const size_t Index) const
{
    if ((Index>=0)&&(Index<m_Dependencies.size())) {
        return m_Dependencies[Index];
    }
    return 0;
}

void CDependencyRecord::AddDependency(const CDependencyRecord *Record)
{
    for (size_t i = 0; i < m_Dependencies.size(); i++) {
        if (Record == m_Dependencies[i]) return;
    }
    m_Dependencies.push_back((CDependencyRecord *)Record);
}

size_t CDependencyRecord::CrossReferencesCount(void) const
{
    return m_CrossReferences.size();
}

CDependencyRecord *CDependencyRecord::CrossReference(const size_t Index) const
{
    if ((Index>=0)&&(Index<m_CrossReferences.size())) {
        return m_CrossReferences[Index];
    }
    return 0;
}

void CDependencyRecord::AddCrossReference(const CDependencyRecord *Record)
{
    for (size_t i = 0; i < m_CrossReferences.size(); i++) {
        if (Record == m_CrossReferences[i]) return;
    }
    m_CrossReferences.push_back((CDependencyRecord *)Record);
}

//------------------------------------------------------------------------------

CDependencyInfo::CDependencyInfo(void)
{
    Clear();
}

CDependencyInfo::~CDependencyInfo(void)
{
    Clear();
}

void CDependencyInfo::Clear(void)
{
    for (size_t i = 0; i < m_Records.size(); i++) {
        delete m_Records[i];
    }
    m_Records.clear();
    m_Platform.Clear();
}

void CDependencyInfo::Show(void)
{
    if (m_Records.size() > 0) {
        std::cout<<"Dependency info collected for "<<m_Records.size()<<" units."<<std::endl;
        std::cout<<"Unit Dependencies: Direct\tIndirect\tAll\t"
                 <<"XRefs: Direct\tIndirect\tAll\tPath"<<std::endl;
        for (size_t i = 0; i < m_Records.size(); i++) {
            CDependencyRecord *r = m_Records[i];
            /*
            std::cout<<"Unit #"<<(i+1)<<" '"<<r->GetName().GetCString()<<"' has "
            <<r->DependenciesCount()<<" direct and "<<DependenciesCount(r)
            <<" indirect dependencies."<<std::endl;
            */
            std::cout<<"#"<<(i+1)<<"\t\t"<<r->DependenciesCount();
            if (r->DependenciesComplete()) std::cout<<"*";
            std::cout<<"\t\t"<<IndirectDependenciesCount(i)<<"\t\t"
                     <<AllDependenciesCount(i)<<"\t"<<r->CrossReferencesCount();
            if (r->CrossReferencesComplete()) std::cout<<"*";
            std::cout<<"\t\t"<<IndirectCrossReferencesCount(i)<<"\t\t"
                     <<AllCrossReferencesCount(i)<<"\t"<<r->GetName().GetCString()<<std::endl;
        }
    } else {
        std::cout<<"Dependency information is not available."<<std::endl;
    }
}

CString CDependencyInfo::OneLineReport(const size_t Index, const bool Deps, const bool XRefs)
{
    CString result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        CDependencyRecord *r = m_Records[Index];
        if (Deps) {
            result += "Deps ["+IntegerToString(r->DependenciesCount())
                      + "/"+IntegerToString(IndirectDependenciesCount(Index))
                      + "/"+IntegerToString(AllDependenciesCount(Index)) + "]";
        }
        if (Deps && XRefs) result += " ";
        if (XRefs) {
            result += " XRefs ["+IntegerToString(r->CrossReferencesCount())
                      + "/"+IntegerToString(IndirectCrossReferencesCount(Index))
                      + "/"+IntegerToString(AllCrossReferencesCount(Index))+"]";
        }
        result += " (direct/indirect/all)";
    }
    return result;
}

void CDependencyInfo::ResetMarkers(void)
{
    for (size_t i = 0; i < m_Records.size(); i++) {
        m_Records[i]->Marker() = 0;
    }
}

size_t CDependencyInfo::DependenciesCount(CDependencyRecord *Record)
{
    size_t result = 0;
    if (0!=Record) {
        //ResetMarkers();
        result += Record->DependenciesCount();
        Record->Marker()++;
        for (size_t i = 0, n = Record->DependenciesCount(); i < n; i++) {
            if (0==Record->Dependency(i)->Marker()) {
                result += DependenciesCount(Record->Dependency(i));
                Record->Dependency(i)->Marker()++;
            }
        }
    }
    return result;
}

CStringList CDependencyInfo::Dependencies(CDependencyRecord *Record)
{
    CStringList result;
    if (0!=Record) {
        //ResetMarkers();
        result.Insert(Record->GetName());
        Record->Marker()++;
        for (size_t i = 0, n = Record->DependenciesCount(); i < n; i++) {
            if (0==Record->Dependency(i)->Marker()) {
                result.Insert(Dependencies(Record->Dependency(i)));
                Record->Dependency(i)->Marker()++;
            }
        }
    }
    return result;
}

size_t CDependencyInfo::CrossReferencesCount(CDependencyRecord *Record)
{
    size_t result = 0;
    if (0!=Record) {
        //ResetMarkers();
        result += Record->CrossReferencesCount();
        Record->Marker()++;
        for (size_t i = 0, n = Record->CrossReferencesCount(); i < n; i++) {
            if (0==Record->CrossReference(i)->Marker()) {
                result += CrossReferencesCount(Record->CrossReference(i));
                Record->CrossReference(i)->Marker()++;
            }
        }
    }
    return result;
}

CStringList CDependencyInfo::CrossReferences(CDependencyRecord *Record)
{
    CStringList result;
    if (0!=Record) {
        //ResetMarkers();
        result.Insert(Record->GetName());
        Record->Marker()++;
        for (size_t i = 0, n = Record->CrossReferencesCount(); i < n; i++) {
            if (0==Record->CrossReference(i)->Marker()) {
                result.Insert(CrossReferences(Record->CrossReference(i)));
                Record->CrossReference(i)->Marker()++;
            }
        }
    }
    return result;
}

size_t CDependencyInfo::RecordsCount(void) const
{
    return m_Records.size();
}

size_t CDependencyInfo::DirectDependenciesCount(const size_t Index) const
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        result += m_Records[Index]->DependenciesCount();
    }
    return result;
}

size_t CDependencyInfo::IndirectDependenciesCount(const size_t Index)
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->DependenciesCount(); i < n; i++) {
            result += DependenciesCount(r->Dependency(i));
        }
    }
    return result;
}

size_t CDependencyInfo::AllDependenciesCount(const size_t Index)
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        return DependenciesCount(m_Records[Index]);
    }
    return result;
}

CString CDependencyInfo::Name(const size_t Index) const
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        return m_Records[Index]->GetName();
    }
    return "";
}

CStringList CDependencyInfo::DirectDependencies(const size_t Index) const
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->DependenciesCount(); i < n; i++) {
            result.Insert(r->Dependency(i)->GetName());
        }
    }
    return result;
}

CStringList CDependencyInfo::IndirectDependencies(const size_t Index)
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->DependenciesCount(); i < n; i++) {
            const CDependencyRecord *rr = r->Dependency(i);
            for (size_t j = 0, m = rr->DependenciesCount(); j < m; j++) {
                result.Insert(Dependencies(rr->Dependency(j)));
            }
        }
    }
    return result;
}

CStringList CDependencyInfo::AllDependencies(const size_t Index)
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->DependenciesCount(); i < n; i++) {
            result.Insert(Dependencies(r->Dependency(i)));
        }
    }
    return result;
}

size_t CDependencyInfo::DirectCrossReferencesCount(const size_t Index) const
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        result += m_Records[Index]->CrossReferencesCount();
    }
    return result;
}

size_t CDependencyInfo::IndirectCrossReferencesCount(const size_t Index)
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->CrossReferencesCount(); i < n; i++) {
            result += CrossReferencesCount(r->CrossReference(i));
        }
    }
    return result;
}

size_t CDependencyInfo::AllCrossReferencesCount(const size_t Index)
{
    size_t result = 0;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        return CrossReferencesCount(m_Records[Index]);
    }
    return result;
}

CStringList CDependencyInfo::DirectCrossReferences(const size_t Index) const
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->CrossReferencesCount(); i < n; i++) {
            result.Insert(r->CrossReference(i)->GetName());
        }
    }
    return result;
}

CStringList CDependencyInfo::IndirectCrossReferences(const size_t Index)
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->CrossReferencesCount(); i < n; i++) {
            const CDependencyRecord *rr = r->CrossReference(i);
            for (size_t j = 0, m = rr->CrossReferencesCount(); j < m; j++) {
                result.Insert(CrossReferences(rr->CrossReference(j)));
            }
        }
    }
    return result;
}

CStringList CDependencyInfo::AllCrossReferences(const size_t Index)
{
    CStringList result;
    if ((Index>=0)&&(Index<m_Records.size())) {
        ResetMarkers();
        const CDependencyRecord *r = m_Records[Index];
        for (size_t i = 0, n = r->CrossReferencesCount(); i < n; i++) {
            result.Insert(CrossReferences(r->CrossReference(i)));
        }
    }
    return result;
}

bool CDependencyInfo::AreDependenciesComplete(const size_t Index)
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        return m_Records[Index]->DependenciesComplete();
    }
    return false;
}

bool CDependencyInfo::AreCrossReferencesComplete(const size_t Index)
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        return m_Records[Index]->CrossReferencesComplete();
    }
    return false;
}

void CDependencyInfo::SetDependenciesComplete(const size_t Index, const bool State)
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        m_Records[Index]->DependenciesComplete() = State;
    }
}

void CDependencyInfo::SetCrossReferencesComplete(const size_t Index, const bool State)
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        m_Records[Index]->CrossReferencesComplete() = State;
    }
}

int CDependencyInfo::FindRecord(const CString& Name)
{
    const hash_t h = sdbm_hash((unsigned char *)Name.GetCString(),Name.GetLength());
    for (size_t i = 0; i < m_Records.size(); i++) {
        CDependencyRecord *r = m_Records[i];
        if (r->Hash() == h) {
            if (r->GetName() == Name) {
                return i;
            }
        }
    }
    return -1;
}

size_t CDependencyInfo::AddRecord(const CString& Name)
{
    int Index = FindRecord(Name);
    if (Index>=0) {
        return Index;
    } else {
        CDependencyRecord *r = new CDependencyRecord;
        r->SetName(Name);
        m_Records.push_back(r);
        Index = m_Records.size()-1;
    }
    return Index;
}

bool CDependencyInfo::AddDependency(const size_t Index, const CString& DependencyName)
{
    if ((Index>=0)&&(Index<m_Records.size())) {
        size_t dep_index = AddRecord(DependencyName);
        CDependencyRecord *r = m_Records[Index];
        CDependencyRecord *d = m_Records[dep_index];
        r->AddDependency(d);
        d->AddCrossReference(r);
        return true;
    }
    return false;
}

size_t CDependencyInfo::AddDependency(const CString& Name, const CString& DependencyName)
{
    size_t rec_index = AddRecord(Name);
    size_t dep_index = AddRecord(DependencyName);
    CDependencyRecord *r = m_Records[rec_index];
    CDependencyRecord *d = m_Records[dep_index];
    r->AddDependency(d);
    d->AddCrossReference(r);
    return rec_index;
}

void CDependencyInfo::MakeRules(CMakefile& Makefile, const int Section, const bool Multiline)
{
    for (size_t i = 0; i < m_Records.size(); i++) {
        const CDependencyRecord *r = m_Records[i];
        CStringList deps(DirectDependencies(i));
        if (deps.GetCount() > 0) {
            deps.RemoveDuplicates();
            CMakefileRule& rule = Makefile.AddRule(Platform().Pd(r->GetName()),Section);
            rule.Multiline() = Multiline;
            for (int j = 0; j < deps.GetCount(); j++) {
                rule.Dependencies().Insert(Platform().Pd(deps[j]));
            }
        }
    }
}

//------------------------------------------------------------------------------

CIncludeSearchFilter::CIncludeSearchFilter(void)
{
    Clear();
}

CIncludeSearchFilter::CIncludeSearchFilter(const CIncludeSearchFilter& Filter)
{
    Assign(Filter);
}

CIncludeSearchFilter::~CIncludeSearchFilter(void)
{
    Clear();
}

void CIncludeSearchFilter::Clear(void)
{
    m_IncludeDirectories.Clear();
    m_DefinedMacros.Clear();
}

void CIncludeSearchFilter::Show(void)
{
//
}

void CIncludeSearchFilter::Assign(const CIncludeSearchFilter& Filter)
{
    m_IncludeDirectories = Filter.m_IncludeDirectories;
    m_DefinedMacros = Filter.m_DefinedMacros;
}

bool CIncludeSearchFilter::Execute(const CString& FileName, CStringList& Includes)
{
    return false;
}

bool CIncludeSearchFilter::Execute(const CString& FileName, CDependencyInfo& Dependencies)
{
    return false;
}

void CIncludeSearchFilter::AddIncludeDirectory(const CString& Path)
{
    m_IncludeDirectories.Insert(Path);
}

void CIncludeSearchFilter::AddIncludeDirectories(const CStringList& Paths)
{
    m_IncludeDirectories.Insert(Paths);
}

void CIncludeSearchFilter::AddMacroDefiniton(const CString& Macro)
{
    m_DefinedMacros.Insert(Macro);
}

void CIncludeSearchFilter::AddMacroDefinitons(const CStringList& Macros)
{
    m_DefinedMacros.Insert(Macros);
}

CString CIncludeSearchFilter::ResolveIncludePath(const CString& IncludeName)
{
    CString name = ExtractFileName(IncludeName);
//std::cout<<"include = "<<IncludeName.GetCString()<<std::endl;
//std::cout<<"name = "<<name.GetCString()<<std::endl;
    CString cwd = GetCurrentDir();
    CString path = IncludeTrailingPathDelimiter(MakeNativePath(cwd));
    CString pathname = path+name;
//std::cout<<"checking "<<pathname.GetCString()<<std::endl;
    if (FileExists(pathname)) {
        //std::cout<<"found "<<pathname.GetCString()<<std::endl;
        return name;
    }
//
    for (int i = 0; i < m_IncludeDirectories.GetCount(); i++) {
        path = IncludeTrailingPathDelimiter(MakeNativePath(m_IncludeDirectories[i]));
        pathname = path+name;
        //std::cout<<"checking "<<pathname.GetCString()<<std::endl;
        if (FileExists(pathname)) {
            //std::cout<<"found "<<pathname.GetCString()<<std::endl;
            return pathname;
        }
    }
//std::cout<<"cannot resolve "<<IncludeName.GetCString()<<std::endl;
    return "";
}

//------------------------------------------------------------------------------

CCppIncludeSearchFilter::CCppIncludeSearchFilter(void)
{
//
}

CCppIncludeSearchFilter::CCppIncludeSearchFilter(const CCppIncludeSearchFilter& Filter)
{
    Assign(Filter);
}

CCppIncludeSearchFilter::~CCppIncludeSearchFilter(void)
{
//
}

void CCppIncludeSearchFilter::Assign(const CCppIncludeSearchFilter& Filter)
{
//
}

bool CCppIncludeSearchFilter::Execute(const CString& FileName, CStringList& Includes)
{
    const int STATE_UNDEFINED         = 0;
    const int STATE_ENTER_COMMENT     = 1;
    const int STATE_CHAR              = 2;
    const int STATE_STRING            = 3;
    const int STATE_BEGIN_DIRECTIVE   = 4;
    const int STATE_ONELINE_COMMENT   = 5;
    const int STATE_MULTILINE_COMMENT = 6;
    const int STATE_EXIT_COMMENT      = 7;
    const int STATE_CHAR_ESC_SEQ      = 8;
    const int STATE_STRING_ESC_SEQ    = 9;
    const int STATE_INCLUDE_DIRECTIVE = 10;
    const int STATE_INCLUDE_FILENAME  = 11;
    const int STATE_END_OF_INPUT      = -1;
    CStringList source;
    if (!source.LoadFromFile(FileName)) return false;
    if (source.GetCount()==0) return false;
//std::cout<<"CWD: "<<GetCurrentDir().GetString()<<std::endl;
//std::cout<<"Searching includes of "<<FileName.GetString()<<"..."<<std::endl;
//ShowStringList("Found includes","include",Includes);
//
    CString include_filename;
    CStringList local_includes;
    for (int l = 0; l < source.GetCount(); l++) {
        CString line(source.GetString(l));
        CStringIterator it(&line);
        int state = STATE_UNDEFINED;
        bool eol = false;
        //while (state!=STATE_END_OF_INPUT)
        while (!eol) {
            switch (state) {
            default:
            case STATE_UNDEFINED: {
                //if (it.This()=='\0') { state = STATE_END_OF_INPUT; break; }
                if (it.This()=='/') {
                    state = STATE_ENTER_COMMENT;
                    break;
                }
                if (it.This()=='\'') {
                    state = STATE_CHAR;
                    break;
                }
                if (it.This()=='"') {
                    state = STATE_STRING;
                    break;
                }
                if (it.This()=='#') {
                    state = STATE_BEGIN_DIRECTIVE;
                    break;
                }
                break;
            }
            case STATE_ENTER_COMMENT: {
                if (it.This()=='/') {
                    state = STATE_ONELINE_COMMENT;
                    break;
                }
                if (it.This()=='*') {
                    state = STATE_MULTILINE_COMMENT;
                    break;
                }
                break;
            }
            case STATE_ONELINE_COMMENT: {
                if (it.This()=='\0') {
                    state = STATE_UNDEFINED;
                    break;
                }
                break;
            }
            case STATE_MULTILINE_COMMENT: {
                if (it.This()=='*') {
                    state = STATE_EXIT_COMMENT;
                    break;
                }
                break;
            }
            case STATE_EXIT_COMMENT: {
                if (it.This()=='/') {
                    state = STATE_UNDEFINED;
                    break;
                }
                state = STATE_MULTILINE_COMMENT;
                break;
            }
            case STATE_CHAR: {
                if (it.This()=='\\') {
                    state = STATE_CHAR_ESC_SEQ;
                    break;
                }
                if (it.This()=='\'') {
                    state = STATE_UNDEFINED;
                    break;
                }
                break;
            }
            case STATE_CHAR_ESC_SEQ: {
                state = STATE_CHAR;
                break;
            }
            case STATE_STRING: {
                if (it.This()=='\\') {
                    state = STATE_STRING_ESC_SEQ;
                    break;
                }
                if (it.This()=='"') {
                    state = STATE_UNDEFINED;
                    break;
                }
                break;
            }
            case STATE_STRING_ESC_SEQ: {
                state = STATE_STRING;
                break;
            }
            case STATE_BEGIN_DIRECTIVE: {
                if (it.Match("include",true)) {
                    state = STATE_INCLUDE_DIRECTIVE;
                    break;
                }
                break;
            }
            case STATE_INCLUDE_DIRECTIVE: {
                if (it.This()=='<') {
                    state = STATE_INCLUDE_FILENAME;
                    break;
                }
                if (it.This()=='"') {
                    state = STATE_INCLUDE_FILENAME;
                    break;
                }
                break;
            }
            case STATE_INCLUDE_FILENAME: {
                if (it.This()=='>') {
                    state = STATE_UNDEFINED;
                }
                if (it.This()=='"') {
                    state = STATE_UNDEFINED;
                }
                if (state == STATE_INCLUDE_FILENAME) {
                    include_filename.Append(it.This());
                } else {
                    local_includes.Insert(include_filename);
                    include_filename.Clear();
                }
                break;
            }
            case STATE_END_OF_INPUT: {
                break;
            }
            }
            eol = (it.This()=='\0');
            it.Next();
        }
    }
    bool result = true;
//ShowStringList("Local includes","include",local_includes);
    for (int i = 0; i < local_includes.GetCount(); i++) {
        CString resolved_include = ResolveIncludePath(local_includes[i]);
        if (!resolved_include.IsEmpty()) {
            if (INVALID_INDEX==Includes.FindString(resolved_include)) {
                Includes.Insert(resolved_include);
                result = result && Execute(resolved_include,Includes);
            }
        }
    }
    Includes.RemoveDuplicates();
    return result;
}

bool CCppIncludeSearchFilter::Execute(const CString& FileName, CDependencyInfo& Dependencies)
{
    const int STATE_UNDEFINED         = 0;
    const int STATE_ENTER_COMMENT     = 1;
    const int STATE_CHAR              = 2;
    const int STATE_STRING            = 3;
    const int STATE_BEGIN_DIRECTIVE   = 4;
    const int STATE_ONELINE_COMMENT   = 5;
    const int STATE_MULTILINE_COMMENT = 6;
    const int STATE_EXIT_COMMENT      = 7;
    const int STATE_CHAR_ESC_SEQ      = 8;
    const int STATE_STRING_ESC_SEQ    = 9;
    const int STATE_INCLUDE_DIRECTIVE = 10;
    const int STATE_INCLUDE_FILENAME  = 11;
    const int STATE_END_OF_INPUT      = -1;
//
    bool result = true;
    CString file_name = MakeNativePath(FileName);
// check if unit is already scanned
    bool scan_unit = true;
    const size_t unit_index = Dependencies.AddRecord(file_name);
    if (unit_index >= 0) {
        scan_unit = !Dependencies.AreDependenciesComplete(unit_index);
    }
    if (scan_unit) {
        CStringList source;
        if (!source.LoadFromFile(file_name)) return false;
        if (source.GetCount()==0) return false;
        //debug
        //std::cout<<"CWD: "<<GetCurrentDir().GetString()<<std::endl;
        //std::cout<<"Searching includes of "<<FileName.GetString()<<"..."<<std::endl;
        //ShowStringList("Found includes","include",Includes);
        //
        CString include_filename;
        CStringList direct_includes;
        for (int l = 0; l < source.GetCount(); l++) {
            CString line(source.GetString(l));
            CStringIterator it(&line);
            int state = STATE_UNDEFINED;
            bool eol = false;
            //while (state!=STATE_END_OF_INPUT)
            while (!eol) {
                switch (state) {
                default:
                case STATE_UNDEFINED: {
                    //if (it.This()=='\0') { state = STATE_END_OF_INPUT; break; }
                    if (it.This()=='/') {
                        state = STATE_ENTER_COMMENT;
                        break;
                    }
                    if (it.This()=='\'') {
                        state = STATE_CHAR;
                        break;
                    }
                    if (it.This()=='"') {
                        state = STATE_STRING;
                        break;
                    }
                    if (it.This()=='#') {
                        state = STATE_BEGIN_DIRECTIVE;
                        break;
                    }
                    break;
                }
                case STATE_ENTER_COMMENT: {
                    if (it.This()=='/') {
                        state = STATE_ONELINE_COMMENT;
                        break;
                    }
                    if (it.This()=='*') {
                        state = STATE_MULTILINE_COMMENT;
                        break;
                    }
                    break;
                }
                case STATE_ONELINE_COMMENT: {
                    if (it.This()=='\0') {
                        state = STATE_UNDEFINED;
                        break;
                    }
                    break;
                }
                case STATE_MULTILINE_COMMENT: {
                    if (it.This()=='*') {
                        state = STATE_EXIT_COMMENT;
                        break;
                    }
                    break;
                }
                case STATE_EXIT_COMMENT: {
                    if (it.This()=='/') {
                        state = STATE_UNDEFINED;
                        break;
                    }
                    state = STATE_MULTILINE_COMMENT;
                    break;
                }
                case STATE_CHAR: {
                    if (it.This()=='\\') {
                        state = STATE_CHAR_ESC_SEQ;
                        break;
                    }
                    if (it.This()=='\'') {
                        state = STATE_UNDEFINED;
                        break;
                    }
                    break;
                }
                case STATE_CHAR_ESC_SEQ: {
                    state = STATE_CHAR;
                    break;
                }
                case STATE_STRING: {
                    if (it.This()=='\\') {
                        state = STATE_STRING_ESC_SEQ;
                        break;
                    }
                    if (it.This()=='"') {
                        state = STATE_UNDEFINED;
                        break;
                    }
                    break;
                }
                case STATE_STRING_ESC_SEQ: {
                    state = STATE_STRING;
                    break;
                }
                case STATE_BEGIN_DIRECTIVE: {
                    if (it.Match("include",true)) {
                        state = STATE_INCLUDE_DIRECTIVE;
                        break;
                    }
                    break;
                }
                case STATE_INCLUDE_DIRECTIVE: {
                    if (it.This()=='<') {
                        state = STATE_INCLUDE_FILENAME;
                        break;
                    }
                    if (it.This()=='"') {
                        state = STATE_INCLUDE_FILENAME;
                        break;
                    }
                    break;
                }
                case STATE_INCLUDE_FILENAME: {
                    if (it.This()=='>') {
                        state = STATE_UNDEFINED;
                    }
                    if (it.This()=='"') {
                        state = STATE_UNDEFINED;
                    }
                    if (state == STATE_INCLUDE_FILENAME) {
                        include_filename.Append(it.This());
                    } else {
                        direct_includes.Insert(include_filename);
                        include_filename.Clear();
                    }
                    break;
                }
                case STATE_END_OF_INPUT: {
                    break;
                }
                }
                eol = (it.This()=='\0');
                it.Next();
            }
        }
        //
        //ShowStringList("Direct includes","include",direct_includes);//debug
        Dependencies.SetDependenciesComplete(unit_index);
        int resolved_count = 0;
        for (int i = 0; i < direct_includes.GetCount(); i++) {
            CString resolved_include = ResolveIncludePath(direct_includes[i]);
            //std::cout<<direct_includes[i].GetCString()<<" -> "<<resolved_include.GetCString()<<std::endl;
            if (!resolved_include.IsEmpty()) {
                Dependencies.AddDependency(unit_index,resolved_include);
                resolved_count++;
                int resolved_index = Dependencies.FindRecord(resolved_include);
                if (!Dependencies.AreDependenciesComplete(resolved_index)) {
                    result = result && Execute(resolved_include,Dependencies);
                }
            }
        }
        //no need to check this: recursive search will do the job
        //if (0==resolved_count)
        //{
        //Dependencies.SetDependenciesComplete(unit_index);
        //}
    }
    return result;
}

//------------------------------------------------------------------------------
