/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <manager.h>
#include <configmanager.h>

#include "resultmap.h"
#include "pkgconfigmanager.h"

ResultMap::ResultMap()
{
}

ResultMap::ResultMap(const ResultMap& source)
{
    *this = source;
}

ResultMap::~ResultMap()
{
    Clear();
}

void ResultMap::Clear()
{
    for ( ResultHashMap::iterator it = Map.begin(); it != Map.end(); ++it )
    {
        ResultArray& RA = it->second;
        for ( size_t i = 0; i<RA.Count(); ++i )
        {
            delete RA[i];
        }
    }
    Map.clear();
}

void ResultMap::GetAllResults(ResultArray& Array)
{
    for ( ResultHashMap::iterator it = Map.begin(); it != Map.end(); ++it )
    {
        ResultArray& RA = it->second;
        for ( size_t i = 0; i<RA.Count(); ++i )
        {
            Array.Add(RA[i]);
        }
    }
}

void ResultMap::GetShortCodes(wxArrayString& Array)
{
    for ( ResultHashMap::const_iterator it = Map.begin(); it != Map.end(); ++it )
    {
        if ( !it->second.IsEmpty() )
        {
            Array.Add(it->first);
        }
    }
}

bool ResultMap::IsShortCode(const wxString& Name)
{
    if ( Map.find(Name) == Map.end() ) return false;
    return !Map[Name].IsEmpty();
}

void ResultMap::WriteDetectedResults()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    ResultArray Results;
    GetAllResults(Results);

    for ( size_t i=0; i<Results.Count(); i++ )
    {
        LibraryResult* Result = Results[i];
        wxString Path = wxString::Format(_T("/stored_results/res%06d/"),i);

        cfg->Write(Path+_T("name"),Result->LibraryName);
        cfg->Write(Path+_T("short_code"),Result->ShortCode);
        cfg->Write(Path+_T("base_path"),Result->BasePath);
        cfg->Write(Path+_T("description"),Result->Description);
        cfg->Write(Path+_T("pkg_config_var"),Result->PkgConfigVar);

        cfg->Write(Path+_T("categories"),Result->Categories);
        cfg->Write(Path+_T("include_paths"),Result->IncludePath);
        cfg->Write(Path+_T("lib_paths"),Result->LibPath);
        cfg->Write(Path+_T("obj_paths"),Result->ObjPath);
        cfg->Write(Path+_T("libs"),Result->Libs);
        cfg->Write(Path+_T("defines"),Result->Defines);
        cfg->Write(Path+_T("cflags"),Result->CFlags);
        cfg->Write(Path+_T("lflags"),Result->LFlags);
        cfg->Write(Path+_T("compilers"),Result->Compilers);
    }
}

void ResultMap::ReadDetectedResults()
{
    Clear();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
    if ( !cfg ) return;

    wxArrayString Results = cfg->EnumerateSubPaths(_T("/stored_results"));
    for ( size_t i=0; i<Results.Count(); i++ )
    {
        wxString Path = _T("/stored_results/") + Results[i] + _T("/");
        LibraryResult* Result = new LibraryResult();

        Result->Type         = rtDetected;

        Result->LibraryName  = cfg->Read(Path+_T("name"),wxEmptyString);
        Result->ShortCode    = cfg->Read(Path+_T("short_code"),wxEmptyString);
        Result->BasePath     = cfg->Read(Path+_T("base_path"),wxEmptyString);
        Result->Description  = cfg->Read(Path+_T("description"),wxEmptyString);
        Result->PkgConfigVar = cfg->Read(Path+_T("pkg_config_var"),wxEmptyString);

        Result->Categories   = cfg->ReadArrayString(Path+_T("categories"));
        Result->IncludePath  = cfg->ReadArrayString(Path+_T("include_paths"));
        Result->LibPath      = cfg->ReadArrayString(Path+_T("lib_paths"));
        Result->ObjPath      = cfg->ReadArrayString(Path+_T("obj_paths"));
        Result->Libs         = cfg->ReadArrayString(Path+_T("libs"));
        Result->Defines      = cfg->ReadArrayString(Path+_T("defines"));
        Result->CFlags       = cfg->ReadArrayString(Path+_T("cflags"));
        Result->LFlags       = cfg->ReadArrayString(Path+_T("lflags"));
        Result->Compilers    = cfg->ReadArrayString(Path+_T("compilers"));

        if ( Result->ShortCode.IsEmpty() )
        {
            delete Result;
            continue;
        }

        GetShortCode(Result->ShortCode).Add(Result);
    }
}

void ResultMap::ReadPkgConfigResults(PkgConfigManager* m_Manager)
{
    m_Manager->DetectLibraries( *this );
}

void ResultMap::ReadPredefinedResults()
{
    SearchDirs Dirs[] = { sdDataGlobal, sdDataUser };
    size_t DirsCnt = sizeof(Dirs) / sizeof(Dirs[0]);

    for ( size_t i=0; i<DirsCnt; i++ )
    {
        wxString Path = ConfigManager::GetFolder(Dirs[i]) + wxFileName::GetPathSeparator() + _T("lib_finder/predefined");
        wxDir Dir(Path);
        wxString Name;
        if ( !Dir.IsOpened() ) continue;
        if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_FILES|wxDIR_HIDDEN) )
        {
            do
            {
                LoadPredefinedResultFromFile(Path+wxFileName::GetPathSeparator()+Name);
            }
            while ( Dir.GetNext(&Name) );
        }
    }
}

void ResultMap::LoadPredefinedResultFromFile(const wxString& FileName)
{
    TiXmlDocument Doc;

    if ( !Doc.LoadFile(FileName.mb_str(wxConvFile)) ) return;

    wxString CBBase = ConfigManager::GetFolder(sdBase) + wxFileName::GetPathSeparator();

    for ( TiXmlElement* RootElem = Doc.FirstChildElement("predefined_library");
          RootElem;
          RootElem = RootElem->NextSiblingElement("predefined_library") )
    {
        for ( TiXmlElement* Elem = RootElem->FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            LibraryResult* Result = new LibraryResult();
            Result->Type         = rtPredefined;
            Result->LibraryName  = wxString(Elem->Attribute("name")      ,wxConvUTF8);
            Result->ShortCode    = wxString(Elem->Attribute("short_code"),wxConvUTF8);
            Result->BasePath     = wxString(Elem->Attribute("base_path") ,wxConvUTF8);
            Result->PkgConfigVar = wxString(Elem->Attribute("pkg_config"),wxConvUTF8);
            if ( TiXmlElement* Sub = Elem->FirstChildElement("description") )
            {
                Result->Description  = wxString(Sub->GetText(),wxConvUTF8);
            }

            for ( TiXmlAttribute* Attr = Elem->FirstAttribute(); Attr; Attr=Attr->Next() )
            {
//                if ( !strncasecmp(Attr->Name(),"category",8) )
                if ( !strncmp(Attr->Name(),"category",8) )
                {
                    Result->Categories.Add(wxString(Attr->Value(),wxConvUTF8));
                }
            }

            for ( TiXmlElement* Sub = Elem->FirstChildElement(); Sub; Sub=Sub->NextSiblingElement() )
            {
                wxString Name = wxString(Sub->Value(),wxConvUTF8).Lower();

                if ( Name == _T("path") )
                {
                    wxString Include = wxString(Sub->Attribute("include"),wxConvUTF8);
                    wxString Lib     = wxString(Sub->Attribute("lib"),wxConvUTF8);
                    wxString Obj     = wxString(Sub->Attribute("obj"),wxConvUTF8);

                    if ( !Include.IsEmpty() )
                    {
                        Result->IncludePath.Add(wxFileName(Include).IsRelative() ? CBBase + Include : Include);
                    }

                    if ( !Lib.IsEmpty() )
                    {
                        Result->LibPath.Add(wxFileName(Lib).IsRelative() ? CBBase + Lib : Lib);
                    }

                    if ( !Obj.IsEmpty() )
                    {
                        Result->ObjPath.Add(wxFileName(Obj).IsRelative() ? CBBase + Obj : Obj);
                    }
                }

                if ( Name == _T("add") )
                {
                    wxString Lib = wxString(Sub->Attribute("lib"),wxConvUTF8);
                    wxString Define = wxString(Sub->Attribute("define"),wxConvUTF8);
                    wxString CFlags = wxString(Sub->Attribute("cflags"),wxConvUTF8);
                    wxString LFlags = wxString(Sub->Attribute("lflags"),wxConvUTF8);

                    if ( !Lib.IsEmpty() ) Result->Libs.Add(Lib);
                    if ( !Define.IsEmpty() ) Result->Defines.Add(Define);
                    if ( !CFlags.IsEmpty() ) Result->CFlags.Add(CFlags);
                    if ( !LFlags.IsEmpty() ) Result->LFlags.Add(LFlags);
                }

                if ( Name == _T("compiler") )
                {
                    Result->Compilers.Add(wxString(Sub->Attribute("name"),wxConvUTF8));
                }
            }

            if ( Result->LibraryName.IsEmpty() ||
                 Result->ShortCode.IsEmpty() )
            {
                delete Result;
                continue;
            }

            GetShortCode(Result->ShortCode).Add(Result);
        }
    }
}

ResultMap& ResultMap::operator=(const ResultMap& source)
{
    Clear();

    for ( ResultHashMap::const_iterator it = source.Map.begin(); it != source.Map.end(); ++it )
    {
        const ResultArray& RA = it->second;
        ResultArray& RA2 = Map[it->first];

        for ( size_t i = 0; i<RA.Count(); ++i )
        {
            RA2.Add( new LibraryResult( *RA[i] ) );
        }
    }

    return *this;
}
