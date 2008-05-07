/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2008  Bartlomiej Swiecki
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

#include <tinyxml/tinyxml.h>

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/string.h>

#include <manager.h>
#include <configmanager.h>

#include "librarydetectionmanager.h"

LibraryDetectionManager::LibraryDetectionManager(TypedResults& CurrentResults): m_CurrentResults(CurrentResults)
{
}

LibraryDetectionManager::~LibraryDetectionManager()
{
    Clear();
}

void LibraryDetectionManager::Clear()
{
    for ( size_t i=0; i<Libraries.Count(); ++i )
    {
        delete Libraries[i];
    }
    Libraries.Clear();
}

int LibraryDetectionManager::LoadXmlConfig(const wxString& Path)
{
    wxDir Dir(Path);
    wxString Name;
    if ( !Dir.IsOpened() ) return 0;

    int loaded = 0;
    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN) )
    {
        do
        {
            loaded += LoadXmlConfig(Path+wxFileName::GetPathSeparator()+Name);
        }
        while ( Dir.GetNext(&Name) );
    }

    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_FILES|wxDIR_HIDDEN) )
    {
        do
        {
            loaded += LoadXmlFile(Path+wxFileName::GetPathSeparator()+Name) ? 1 : 0;
        }
        while ( Dir.GetNext(&Name) );
    }

    return loaded;
}

int LibraryDetectionManager::LoadXmlFile(const wxString& Name)
{
    TiXmlDocument Doc;

    // TODO: Use built-in loader
    if ( !Doc.LoadFile( Name.mb_str() ) ) return 0;

    return LoadXmlDoc( Doc );
}

int LibraryDetectionManager::LoadXmlDoc( TiXmlDocument& Doc )
{
    int loaded = 0;
    for ( TiXmlElement* Elem = Doc.FirstChildElement("library");
          Elem;
          Elem = Elem->NextSiblingElement("library") )
    {
        LibraryConfig Initial;

        // Read global var name and library name
        Initial.ShortCode = wxString(Elem->Attribute("short_code"),wxConvUTF8);
        if ( Initial.ShortCode.empty() ) continue;
        Initial.LibraryName = wxString(Elem->Attribute("name"),wxConvUTF8);

        // Read categories of library
        for ( TiXmlAttribute* attr = Elem->FirstAttribute();
              attr;
              attr = attr->Next() )
        {
//            if ( !strncasecmp(attr->Name(),"category",8) )
            if ( !strncmp(attr->Name(),"category",8) )
            {
                Initial.Categories.Add(wxString(attr->Value(),wxConvUTF8));
            }
        }

        // Check if there's corresponding pkg-config entry
        if ( IsPkgConfigEntry(Initial.ShortCode) )
        {
            LibraryConfig* Config = new LibraryConfig(Initial);
            Config->PkgConfigVar = Initial.ShortCode;
            Config->Description = Config->LibraryName + _T(" (pkg-config)");
            LibraryFilter Filter;
            Filter.Type = LibraryFilter::PkgConfig;
            Filter.Value = Initial.ShortCode;
            Config->Filters.push_back(Filter);
            loaded += AddConfig(Config) ? 1 : 0;
        }

        // Load base configuration of library
        loaded += LoadXml(Elem,new LibraryConfig(Initial));
    }
    return loaded;
}

int LibraryDetectionManager::LoadXml(TiXmlElement* Elem,LibraryConfig* Config,bool Filters,bool Settings)
{
    wxString Description = wxString(Elem->Attribute("description"),wxConvUTF8);
    if ( !Description.empty() ) Config->Description = Description;

    int loaded = 0;
    for ( TiXmlElement* Data = Elem->FirstChildElement();
          Data;
          Data = Data->NextSiblingElement() )
    {
        wxString Node = wxString(Data->Value(),wxConvUTF8).MakeLower();

        if ( Filters && Settings )
        {
            // Load subnodes
            if ( Node == _T("filters") )
            {
                loaded += LoadXml(Data,Config,true,false);
                continue;
            }

            if ( Node == _T("settings") )
            {
                loaded += LoadXml(Data,Config,false,true);
                continue;
            }

            // pkgconfig does define both filter and setting
            if ( Node == _T("pkgconfig") )
            {
                Config->PkgConfigVar = wxString(Data->Attribute("name"),wxConvUTF8);
                LibraryFilter Filter;
                Filter.Type = LibraryFilter::PkgConfig;
                Filter.Value = Config->PkgConfigVar;
                Config->Filters.push_back(Filter);
                continue;
            }
        }

        if ( Filters )
        {
            // Load filter
            LibraryFilter::FilterType Type = LibraryFilter::None;

            if ( Node == _T("platform") ) Type = LibraryFilter::Platform; else
            if ( Node == _T("file") )     Type = LibraryFilter::File;     else
            if ( Node == _T("exec") )     Type = LibraryFilter::Exec;     else
            if ( Node == _T("compiler") ) Type = LibraryFilter::Compiler;

            if ( Type != LibraryFilter::None )
            {
                LibraryFilter Filter;
                Filter.Type = Type;
                Filter.Value = wxString(Data->Attribute("name"),wxConvUTF8);
                if ( !Filter.Value.IsEmpty() )
                {
                    Config->Filters.push_back(Filter);
                }
                continue;
            }
        }

        if ( Settings )
        {
            // Load setting
            if ( Node==_T("path") )
            {
                wxString Include = wxString(Data->Attribute("include"),wxConvUTF8);
                wxString Lib = wxString(Data->Attribute("lib"),wxConvUTF8);
                wxString Obj = wxString(Data->Attribute("obj"),wxConvUTF8);
                if ( !Include.empty() ) Config->IncludePaths.Add(Include);
                if ( !Lib.empty()     ) Config->LibPaths.Add(Lib);
                if ( !Obj.empty()     ) Config->ObjPaths.Add(Obj);
                continue;
            }

            if ( Node==_T("flags") )
            {
                wxString cFlags = wxString(Data->Attribute("cflags"),wxConvUTF8);
                wxString lFlags = wxString(Data->Attribute("lflags"),wxConvUTF8);
                if ( !cFlags.empty() ) Config->CFlags.Add(cFlags);
                if ( !lFlags.empty() ) Config->LFlags.Add(lFlags);
                continue;
            }

            if ( Node==_T("add") )
            {
                wxString cFlags = wxString(Data->Attribute("cflags"),wxConvUTF8);
                wxString lFlags = wxString(Data->Attribute("lflags"),wxConvUTF8);
                wxString Lib    = wxString(Data->Attribute("lib")   ,wxConvUTF8);
                wxString Define = wxString(Data->Attribute("define"),wxConvUTF8);
                if ( !cFlags.empty() ) Config->CFlags.Add(cFlags);
                if ( !lFlags.empty() ) Config->LFlags.Add(lFlags);
                if ( !Lib.empty()    ) Config->Libs.Add(Lib);
                if ( !Define.empty() ) Config->Defines.Add(Define);
            }

            if ( Node==_T("header") )
            {
                wxString file = wxString(Data->Attribute("file"),wxConvUTF8);
                if ( !file.empty() ) Config->Headers.Add(file);
            }
        }
    }

    if ( Settings && Filters )
    {
        TiXmlElement* Cfg = Elem->FirstChildElement("config");
        if ( Cfg )
        {
            // If there are any sub-configurations, let's
            // iterate through them and load config-specific settings
            for ( ;Cfg; Cfg = Cfg->NextSiblingElement("config") )
            {
                // Append sub-configuration data
                loaded += LoadXml(Cfg,new LibraryConfig(*Config));
            }

            // Config won't be added anywhere so we have to delete it here
            delete Config;
        }
        else
        {
            // No sub-config entry, so let's add this one
            loaded += AddConfig(Config) ? 1 : 0;
        }
    }
    return loaded;
}

bool LibraryDetectionManager::CheckConfig(const LibraryConfig* Cfg) const
{
    if ( Cfg->LibraryName.empty() ) return false;
    if ( Cfg->ShortCode.empty()   ) return false;
    if ( Cfg->Filters.empty()     ) return false;
    return true;
}

const LibraryConfig* LibraryDetectionManager::GetLibrary(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetLibraryCount() ) return NULL;
    return Libraries[Index];
}

const LibraryConfig* LibraryDetectionManager::GetLibrary(const wxString& ShortCode)
{
    for ( int i=0; i<GetLibraryCount(); i++ )
    {
        if ( Libraries[i]->ShortCode == ShortCode )
        {
            return Libraries[i];
        }
    }
    return 0;
}

bool LibraryDetectionManager::IsPkgConfigEntry(const wxString& Name)
{
    return m_CurrentResults[rtPkgConfig].IsShortCode(Name);
}

bool LibraryDetectionManager::AddConfig(LibraryConfig* Cfg)
{
    if ( CheckConfig(Cfg) )
    {
        Libraries.push_back(Cfg);
        return true;
    }
    else
    {
        delete Cfg;
        return false;
    }
}

bool LibraryDetectionManager::LoadSearchFilters()
{
    wxString Sep = wxFileName::GetPathSeparator();

    int loaded = 0;
    loaded += LoadXmlConfig(ConfigManager::GetFolder(sdDataGlobal) + Sep + _T("lib_finder"));
    loaded += LoadXmlConfig(ConfigManager::GetFolder(sdDataUser)   + Sep + _T("lib_finder"));

    return loaded>0;
}

int LibraryDetectionManager::StoreNewSettingsFile( const wxString& shortcut, const std::vector< char >& content )
{
    // Try to parse file's content
    TiXmlDocument doc;
    if ( !doc.Parse( &content[0] ) ) return -1;

    // Ensure that this file contains required shortcut
    if ( !doc.RootElement() ) return -1;
    if ( !doc.RootElement()->Attribute("short_code") ) return -1;
    if ( strcmp( doc.RootElement()->Attribute("short_code"), cbU2C(shortcut) ) ) return -1;

    // Finally load new data - this will make sure that we have valid xml structure
    int AddedConfigs = LoadXmlDoc( doc );
    if ( !AddedConfigs ) return -1;

    // Search for not-yet existing file name
    int i=0;
    wxString BaseName = ConfigManager::GetFolder(sdDataUser) + wxFileName::GetPathSeparator() + _T("lib_finder") + wxFileName::GetPathSeparator();
    if ( !wxFileName::Mkdir( BaseName, 0777, wxPATH_MKDIR_FULL ) )
    {
        return -2;
    }
    wxString FileName = BaseName + shortcut + _T(".xml");
    while ( wxFileName::FileExists( FileName ) || wxFileName::DirExists( FileName ) )
    {
        FileName = BaseName + shortcut + wxString::Format(_T("%d.xml"),i++);
    }

    // Store data
    wxFile fl( FileName, wxFile::write_excl );
    if ( !fl.IsOpened() )
    {
        return -2;
    }
    const char* ptr = &content[0];
    wxFileOffset len = strlen(ptr);
    if ( fl.Write( ptr, len ) != len )
    {
        return -2;
    }

    return AddedConfigs;
}
