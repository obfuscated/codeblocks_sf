#include <tinyxml/tinyxml.h>
#include "libraryconfigmanager.h"

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/string.h>

LibraryConfigManager::LibraryConfigManager()
{
}

LibraryConfigManager::~LibraryConfigManager()
{
    Clear();
}

void LibraryConfigManager::Clear()
{
    for ( size_t i=0; i<Libraries.Count(); ++i )
        delete Libraries[i];
    Libraries.Clear();
}

void LibraryConfigManager::LoadXmlConfig(const wxString& Path)
{
    wxLogNull LogNull;

    wxDir Dir(Path);
    wxString Name;
    if ( !Dir.IsOpened() ) return;

    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN) )
    {
        do
        {
            LoadXmlConfig(Path+wxFileName::GetPathSeparator()+Name);
        }
        while ( Dir.GetNext(&Name) );
    }

    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_FILES|wxDIR_HIDDEN) )
    {
        do
        {
            LoadXmlFile(Path+wxFileName::GetPathSeparator()+Name);
        }
        while ( Dir.GetNext(&Name) );
    }
}

void LibraryConfigManager::LoadXmlFile(const wxString& Name)
{
    TiXmlDocument Doc;

    if ( !Doc.LoadFile(Name.mb_str()) ) return;

    for ( TiXmlElement* Elem = Doc.FirstChildElement("library");
          Elem;
          Elem = Elem->NextSiblingElement("library") )
    {
        wxString GlobalVar = wxString(Elem->Attribute("global_var"),wxConvUTF8);
        if ( GlobalVar.empty() ) continue;
        wxString GlobalLibName = wxString(Elem->Attribute("name"),wxConvUTF8);

        wxArrayString GlobalFiles;
        wxArrayString GlobalIncludes;
        wxArrayString GlobalLibs;
        wxArrayString GlobalObjs;
        wxString GlobalCFlags;
        wxString GlobalLFlags;

        LoadXmlDefaults(Elem,GlobalFiles,GlobalIncludes,GlobalLibs,GlobalObjs,GlobalCFlags,GlobalLFlags);

        TiXmlElement* Cfg = Elem->FirstChildElement("config");
        if ( Cfg )
        {
            while ( Cfg )
            {
                LibraryConfig* Config = new LibraryConfig;

                Config->LibraryName = GlobalLibName;
                Config->GlobalVar = GlobalVar;
                Config->FileNames = GlobalFiles;
                Config->IncludePaths = GlobalIncludes;
                Config->LibPaths = GlobalLibs;
                Config->ObjPaths = GlobalObjs;
                Config->CFlags = GlobalCFlags;
                Config->LFlags = GlobalLFlags;

                LoadXmlDefaults(
                    Cfg,
                    Config->FileNames,
                    Config->IncludePaths,
                    Config->LibPaths,
                    Config->ObjPaths,
                    Config->CFlags,
                    Config->LFlags);

                wxString NewName = wxString(Cfg->Attribute("new_name"),wxConvUTF8);
                if ( !NewName.empty() ) Config->LibraryName = NewName;

                if ( CheckConfig(Config) )
                {
                    Libraries.push_back(Config);
                }
                else
                {
                    delete Config;
                }

                Cfg = Cfg->NextSiblingElement("config");
            }
        }
        else
        {
            LibraryConfig* Config = new LibraryConfig;

            Config->LibraryName = GlobalLibName;
            Config->GlobalVar = GlobalVar;
            Config->FileNames = GlobalFiles;
            Config->IncludePaths = GlobalIncludes;
            Config->LibPaths = GlobalLibs;
            Config->ObjPaths = GlobalObjs;
            Config->CFlags = GlobalCFlags;
            Config->LFlags = GlobalLFlags;

            if ( CheckConfig(Config) )
            {
                Libraries.push_back(Config);
            }
            else
            {
                delete Config;
            }
        }
    }
}

void LibraryConfigManager::LoadXmlDefaults(
    TiXmlElement* Elem,
    wxArrayString& Files,
    wxArrayString& Includes,
    wxArrayString& Libs,
    wxArrayString& Objs,
    wxString& CFlags,
    wxString& LFlags)
{
    for ( TiXmlElement* File = Elem->FirstChildElement("file");
          File;
          File = File->NextSiblingElement("file") )
    {
        wxString Name = wxString(File->Attribute("name"),wxConvUTF8);
        if ( !Name.empty() ) Files.Add(Name);
    }

    for ( TiXmlElement* Path = Elem->FirstChildElement("path");
          Path;
          Path = Path->NextSiblingElement("path") )
    {
        wxString Include = wxString(Path->Attribute("include"),wxConvUTF8);
        wxString Lib = wxString(Path->Attribute("lib"),wxConvUTF8);
        wxString Obj = wxString(Path->Attribute("obj"),wxConvUTF8);
        if ( !Include.empty() ) Includes.Add(Include);
        if ( !Lib.empty() ) Libs.Add(Lib);
        if ( !Obj.empty() ) Objs.Add(Obj);
    }

    for ( TiXmlElement* Flags = Elem->FirstChildElement("flags");
          Flags;
          Flags = Flags->NextSiblingElement("flags") )
    {
        wxString cFlags = wxString(Flags->Attribute("cflags"),wxConvUTF8);
        wxString lFlags = wxString(Flags->Attribute("lflags"),wxConvUTF8);
        if ( !cFlags.empty() )
        {
            if ( !CFlags.empty() ) CFlags.Append(_T(' '));
            CFlags.Append(cFlags);
        }
        if ( !lFlags.empty() )
        {
            if ( !LFlags.empty() ) LFlags.Append(_T(' '));
            LFlags.Append(lFlags);
        }
    }
}

bool LibraryConfigManager::CheckConfig(const LibraryConfig* Cfg) const
{
    if ( Cfg->LibraryName.empty() ) return false;
    if ( Cfg->FileNames.empty()   ) return false;
    if ( Cfg->GlobalVar.empty()   ) return false;
    return true;
}

const LibraryConfig* LibraryConfigManager::GetLibrary(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetLibraryCount() ) return NULL;
    return Libraries[Index];
}

LibraryConfigManager LibraryConfigManager::Singleton;
