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
* $Revision$
* $Id$
* $HeadURL$
*/

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinywxuni.h>

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
        delete Libraries[i];
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
    if ( !TinyXML::LoadDocument( Name, &Doc ) || Doc.Error() ) return 0;
    return LoadXmlDoc( Doc );
}

int LibraryDetectionManager::LoadXmlDoc( TiXmlDocument& Doc )
{
    int loaded = 0;
    for ( TiXmlElement* Elem = Doc.FirstChildElement("library");
          Elem;
          Elem = Elem->NextSiblingElement("library") )
    {
        // Load the version of this set
        int version = 0;
        if ( Elem->QueryIntAttribute( "version", &version ) != TIXML_SUCCESS )
            version = 0;

        // Load shortcode
        wxString ShortCode = wxString(Elem->Attribute("short_code"),wxConvUTF8);
        if ( ShortCode.IsEmpty() )
            continue;

        // Load name
        wxString Name = wxString( Elem->Attribute("name"), wxConvUTF8 );
        if ( Name.IsEmpty() )
            continue;

        // Check if we already have setting of this library
        // I'm to lazy to firbid const_cast here ;)
        LibraryDetectionConfigSet* OldSet = const_cast< LibraryDetectionConfigSet* > ( GetLibrary( ShortCode ) );
        LibraryDetectionConfigSet* NewSet = 0;

        if ( OldSet )
        {
            // There are detection settings yet, we override only when there's newer
            // or same version already
            if ( OldSet->Version > version )
                continue; // We do not upgrade

            OldSet->Categories.Clear();
            OldSet->Configurations.clear();
            OldSet->LibraryName.Clear();
            NewSet = OldSet;
        }
        else
        {
            NewSet = new LibraryDetectionConfigSet;
            Libraries.Add( NewSet );
        }

        // Setup configuration set
        NewSet->ShortCode = ShortCode;
        NewSet->Version = version;
        NewSet->LibraryName = Name;

        // Read categories of library
        for ( TiXmlAttribute* attr = Elem->FirstAttribute();
              attr;
              attr = attr->Next() )
        {
//            if ( !strncasecmp(attr->Name(),"category",8) )
            if ( !strncmp(attr->Name(),"category",8) )
                NewSet->Categories.Add( wxString( attr->Value(),wxConvUTF8 ) );
        }

        // Check if there's corresponding pkg-config entry
        if ( IsPkgConfigEntry(ShortCode) )
        {
            LibraryDetectionConfig Config;
            Config.PkgConfigVar = ShortCode;
            Config.Description = NewSet->LibraryName + _T(" (pkg-config)");
            LibraryDetectionFilter Filter;
            Filter.Type = LibraryDetectionFilter::PkgConfig;
            Filter.Value = ShortCode;
            Config.Filters.push_back(Filter);
            loaded += AddConfig(Config,NewSet) ? 1 : 0;
        }

        // Load libraries
        LibraryDetectionConfig Initial;
        loaded += LoadXml( Elem, Initial, NewSet );
    }
    return loaded;
}

int LibraryDetectionManager::LoadXml(TiXmlElement* Elem,LibraryDetectionConfig& Config,LibraryDetectionConfigSet* ConfigSet,bool Filters,bool Settings)
{
    wxString Description = wxString(Elem->Attribute("description"),wxConvUTF8);
    if ( !Description.empty() ) Config.Description = Description;

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
                loaded += LoadXml(Data,Config,ConfigSet,true,false);
                continue;
            }

            if ( Node == _T("settings") )
            {
                loaded += LoadXml(Data,Config,ConfigSet,false,true);
                continue;
            }

            // pkgconfig does define both filter and setting
            if ( Node == _T("pkgconfig") )
            {
                Config.PkgConfigVar = wxString(Data->Attribute("name"),wxConvUTF8);
                LibraryDetectionFilter Filter;
                Filter.Type = LibraryDetectionFilter::PkgConfig;
                Filter.Value = Config.PkgConfigVar;
                Config.Filters.push_back(Filter);
                continue;
            }
        }

        if ( Filters )
        {
            // Load filter
            LibraryDetectionFilter::FilterType Type = LibraryDetectionFilter::None;

            if ( Node == _T("platform") ) Type = LibraryDetectionFilter::Platform; else
            if ( Node == _T("file") )     Type = LibraryDetectionFilter::File;     else
            if ( Node == _T("exec") )     Type = LibraryDetectionFilter::Exec;     else
            if ( Node == _T("compiler") ) Type = LibraryDetectionFilter::Compiler;

            if ( Type != LibraryDetectionFilter::None )
            {
                LibraryDetectionFilter Filter;
                Filter.Type = Type;
                Filter.Value = wxString(Data->Attribute("name"),wxConvUTF8);
                if ( !Filter.Value.IsEmpty() )
                    Config.Filters.push_back(Filter);
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
                if ( !Include.empty() ) Config.IncludePaths.Add(Include);
                if ( !Lib.empty()     ) Config.LibPaths.Add(Lib);
                if ( !Obj.empty()     ) Config.ObjPaths.Add(Obj);
                continue;
            }

            if ( Node==_T("flags") )
            {
                wxString cFlags = wxString(Data->Attribute("cflags"),wxConvUTF8);
                wxString lFlags = wxString(Data->Attribute("lflags"),wxConvUTF8);
                if ( !cFlags.empty() ) Config.CFlags.Add(cFlags);
                if ( !lFlags.empty() ) Config.LFlags.Add(lFlags);
                continue;
            }

            if ( Node==_T("add") )
            {
                wxString cFlags = wxString(Data->Attribute("cflags"),wxConvUTF8);
                wxString lFlags = wxString(Data->Attribute("lflags"),wxConvUTF8);
                wxString Lib    = wxString(Data->Attribute("lib")   ,wxConvUTF8);
                wxString Define = wxString(Data->Attribute("define"),wxConvUTF8);
                if ( !cFlags.empty() ) Config.CFlags.Add(cFlags);
                if ( !lFlags.empty() ) Config.LFlags.Add(lFlags);
                if ( !Lib.empty()    ) Config.Libs.Add(Lib);
                if ( !Define.empty() ) Config.Defines.Add(Define);
            }

            if ( Node==_T("header") )
            {
                wxString file = wxString(Data->Attribute("file"),wxConvUTF8);
                if ( !file.empty() ) Config.Headers.Add(file);
            }

            if ( Node==_T("require") )
            {
                wxString lib = wxString(Data->Attribute("library"),wxConvUTF8);
                if ( !lib.empty() ) Config.Require.Add(lib);
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
                LibraryDetectionConfig Copy(Config);
                loaded += LoadXml(Cfg,Copy,ConfigSet);
            }
        }
        else
        {
            // No sub-config entry, so let's add this one
            loaded += AddConfig(Config,ConfigSet) ? 1 : 0;
        }
    }
    return loaded;
}

bool LibraryDetectionManager::CheckConfig(const LibraryDetectionConfig& Cfg) const
{
    if ( Cfg.Filters.empty() ) return false;
    return true;
}

const LibraryDetectionConfigSet* LibraryDetectionManager::GetLibrary(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetLibraryCount() ) return NULL;
    return Libraries[Index];
}

const LibraryDetectionConfigSet* LibraryDetectionManager::GetLibrary(const wxString& ShortCode)
{
    for ( int i=0; i<GetLibraryCount(); i++ )
    {
        if ( Libraries[i]->ShortCode == ShortCode )
            return Libraries[i];
    }
    return 0;
}

bool LibraryDetectionManager::IsPkgConfigEntry(const wxString& Name)
{
    return m_CurrentResults[rtPkgConfig].IsShortCode(Name);
}

bool LibraryDetectionManager::AddConfig(LibraryDetectionConfig& Cfg,LibraryDetectionConfigSet* Set)
{
    if ( CheckConfig(Cfg) )
    {
        Set->Configurations.push_back(Cfg);
        return true;
    }
    return false;
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
        return -2;

    wxString FileName = BaseName + shortcut + _T(".xml");
    while ( wxFileName::FileExists( FileName ) || wxFileName::DirExists( FileName ) )
        FileName = BaseName + shortcut + wxString::Format(_T("%d.xml"),i++);

    // Store data
    wxFile fl( FileName, wxFile::write_excl );
    if ( !fl.IsOpened() )
        return -2;

    const char* ptr = &content[0];
    size_t len = strlen(ptr);
    if ( fl.Write( ptr, len ) != len )
        return -2;

    return AddedConfigs;
}
