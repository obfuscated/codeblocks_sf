/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* Author: Thomas Denk
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "configmanager.h"
    #include "globals.h"
    #include "personalitymanager.h"
    #include "cbexception.h"
    #include <wx/file.h>
    #include <wx/dir.h>
#endif

#include "crc32.h"

#include <wx/url.h>
#include <wx/stream.h>

#ifdef __WXMSW__
#include <shlobj.h>
#endif

#ifdef TIXML_USE_STL
#include <string>
#endif

#include "tinyxml/tinywxuni.h"

wxString ConfigManager::app_path = wxEmptyString;
wxString ConfigManager::data_path_global = wxEmptyString;
wxString ConfigManager::data_path_user = wxEmptyString;
wxString ConfigManager::config_folder = wxEmptyString;
wxString ConfigManager::home_folder = wxEmptyString;


namespace CfgMgrConsts
{
    const wxString app_path(_T("app_path"));
    const wxString data_path(_T("data_path"));
    const wxString dotDot(_T(".."));
    const int version = 1;
}

inline void ConfigManager::Collapse(wxString& str) const
{
    const wxChar *src = str.c_str();
    wxChar *dst = (wxChar*) src;
    wxChar c;
    size_t len = 0;

    while((c = *src))
    {
        ++src;

        *dst = c;
        ++dst;
        ++len;

        if(c == _T('/'))
        while(*src == _T('/'))
            ++src;
    }
    str.Truncate(len);
};

ISerializable::ISerializable()
{}

ISerializable::~ISerializable()
{}



/* ------------------------------------------------------------------------------------------------------------------
*  "Builder pattern" class for ConfigManager
*  Do not use this class  -  Manager::Get()->GetConfigManager() is a lot friendlier
*/

CfgMgrBldr::CfgMgrBldr() : doc(0), volatile_doc(0), r(false)
{
    TiXmlBase::SetCondenseWhiteSpace(false);
    wxString personality(Manager::Get()->GetPersonalityManager()->GetPersonality());

    if(personality.StartsWith(_T("http://")))
    {
        SwitchToR(personality);
        return;
    }

    cfg = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + personality + _T(".conf");

    if(::wxFileExists(cfg) == false)
    {
    cfg = ConfigManager::LocateDataFile(wxFileName(cfg).GetFullName(), sdBase | sdConfig | sdCurrent);
    if(cfg.IsEmpty())
        {
            cfg = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + personality + _T(".conf");
            doc = new TiXmlDocument();
            doc->InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
            doc->InsertEndChild(TiXmlElement("CodeBlocksConfig"));
            doc->FirstChildElement("CodeBlocksConfig")->SetAttribute("version", CfgMgrConsts::version);
            return;
        }
    }
    SwitchTo(cfg);
}

void CfgMgrBldr::SwitchTo(const wxString& fileName)
{
    Close();

    doc = new TiXmlDocument();

    if(!TinyXML::LoadDocument(fileName, doc))
            cbThrow(wxString(_T("Error accessing file.")));

    if(doc->ErrorId())
    {
        cbThrow(wxString::Format(_T("TinyXML error: %s\nAt row %d, column: %d."), cbC2U(doc->ErrorDesc()).c_str(), doc->ErrorRow(), doc->ErrorCol()));
    }

    TiXmlElement* docroot = doc->FirstChildElement("CodeBlocksConfig");

    if(doc->ErrorId())
        cbThrow(wxString::Format(_T("TinyXML error: %s\nAt row %d, column: %d."), cbC2U(doc->ErrorDesc()).c_str(), doc->ErrorRow(), doc->ErrorCol()));

    const char *vers = docroot->Attribute("version");
    if(!vers || atoi(vers) != 1)
        cbMessageBox(_("ConfigManager encountered an unknown config file version. Continuing happily."), _("Warning"), wxICON_WARNING);

    doc->ClearError();

    wxString info;
#ifndef __GNUC__

    info.Printf(_T( " application info:\n"
                    "\t svn_revision:\t%d\n"
                    "\t build_date:\t%s, %s "), ConfigManager::GetRevisionNumber(), wxT(__DATE__), wxT(__TIME__));
#else

    info.Printf(_T( " application info:\n"
                    "\t svn_revision:\t%d\n"
                    "\t build_date:\t%s, %s\n"
                    "\t gcc_version:\t%d.%d.%d "), ConfigManager::GetRevisionNumber(), wxT(__DATE__), wxT(__TIME__),
                __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    if(ConfigManager::Linux())
        info.append(_T("\n\t Linux "));
    if(ConfigManager::Windows())
        info.append(_T("\n\t Windows "));
    if(ConfigManager::MacOS())
        info.append(_T("\n\t MacOS "));
    if(ConfigManager::Unix())
        info.append(_T("\n\t Unix "));

    info.append(ConfigManager::Unicode() ? _T("Unicode ") : _T("ANSI "));

    TiXmlComment c;
    c.SetValue((const char*) info.mb_str());

    TiXmlNode *firstchild = docroot->FirstChild();
    if(firstchild && firstchild->ToComment())
    {
        docroot->RemoveChild(firstchild);
        firstchild = docroot->FirstChild();
    }

    if(firstchild)
        docroot->InsertBeforeChild(firstchild, c);
    else
        docroot->InsertEndChild(c);
}

void CfgMgrBldr::SwitchToR(const wxString& absFileName)
{
    Close();

    if(doc)
        delete doc;
    doc = new TiXmlDocument();
    doc->ClearError();

    cfg = absFileName;

    wxURL url(absFileName);
    url.SetProxy(ConfigManager::GetProxy());
    if (url.GetError() == wxURL_NOERR)
    {
        wxInputStream *is = url.GetInputStream();
        if (is && is->IsOk())
        {
            size_t size = is->GetSize();
            wxString str;
            wxChar* c = str.GetWriteBuf(size);
            is->Read(c, size);
            str.UngetWriteBuf(size);

            doc = new TiXmlDocument();

            if(doc->Parse(cbU2C(str)))
            {
                doc->ClearError();
                delete is;
                return;
            }
            if(Manager::Get()->GetMessageManager())
            {
                Manager::Get()->GetMessageManager()->DebugLog(_T("##### Error loading or parsing remote config file"));
                Manager::Get()->GetMessageManager()->DebugLog(cbC2U(doc->ErrorDesc()));
                doc->ClearError();
            }
        }
        delete is;
    }
    cfg.Empty();
    SwitchTo(wxEmptyString); // fall back
}

CfgMgrBldr::~CfgMgrBldr()
{
    NamespaceMap::iterator it;
    for( it = namespaces.begin(); it != namespaces.end(); ++it )
    {
        delete it->second;
    }
    namespaces.clear();
    Close();
    delete volatile_doc;
}

void CfgMgrBldr::Close()
{
    if(doc)
    {
        if(!cfg.StartsWith(_T("http://")))
        {
            if (!TinyXML::SaveDocument(cfg, doc))
            {
                wxSafeShowMessage(_T("Could not save config file..."), _("Warning"));
                // TODO (thomas#1#): add "retry" option
            }
        }
        else
        {
            // implement WebDAV another time
        }
        delete doc;
    }
    doc = 0;
}


ConfigManager* CfgMgrBldr::GetConfigManager(const wxString& name_space)
{
    return Get()->Build(name_space);
}


ConfigManager* CfgMgrBldr::Build(const wxString& name_space)
{
    if(name_space.IsEmpty())
        cbThrow(_T("You attempted to get a ConfigManager instance without providing a namespace."));

    wxCriticalSectionLocker locker(cs);
    NamespaceMap::iterator it = namespaces.find(name_space);
    if(it != namespaces.end())
        return it->second;

    TiXmlElement* docroot;

    if(name_space.StartsWith(_T("volatile:")))
    {
        if(!volatile_doc)
        {
            volatile_doc = new TiXmlDocument();
            volatile_doc->InsertEndChild(TiXmlElement("CodeBlocksConfig"));
            volatile_doc->SetCondenseWhiteSpace(false);
        }
        docroot = volatile_doc->FirstChildElement("CodeBlocksConfig");
    }
    else
    {
        docroot = doc->FirstChildElement("CodeBlocksConfig");
        if(!docroot)
        {
            wxString err(_("Fatal error parsing supplied configuration file.\nParser error message:\n"));
            err << wxString::Format(_T("%s\nAt row %d, column: %d."), cbC2U(doc->ErrorDesc()).c_str(), doc->ErrorRow(), doc->ErrorCol());
            cbThrow(err);
        }
    }

    TiXmlElement* root = docroot->FirstChildElement(cbU2C(name_space));

    if(!root) // namespace does not exist
    {
        docroot->InsertEndChild(TiXmlElement(cbU2C(name_space)));
        root = docroot->FirstChildElement(cbU2C(name_space));
    }

    if(!root) // now what!
        cbThrow(_T("Unable to create namespace in document tree (actually not possible..?)"));

    ConfigManager *c = new ConfigManager(root);
    namespaces[name_space] = c;

    return c;
}


/*
*  Hack to enable Turkish language. wxString::Upper will convert lowercase 'i' to \u0130 instead of \u0069 in Turkish locale,
*  which will break the config file when used in a tag
*/
inline void to_upper(wxString& s)
{
    wxChar *p = (wxChar*) s.c_str();
    size_t len = s.length()+1;
    for(;--len;++p)
    {
        wxChar q = *p;
        if(q >= 'a' && q <= 'z')
            *p = q - 32;
    }
};

inline void to_lower(wxString& s)
{
    wxChar *p = (wxChar*) s.c_str();
    size_t len = s.length()+1;
    for(;--len;++p)
    {
        wxChar q = *p;
        if(q >= 'A' && q <= 'Z')
            *p = q + 32;
    }
};


/* ------------------------------------------------------------------------------------------------------------------
*  Functions to retrieve system paths and locate data files in a defined, consistent way.
*  Please note that the application determines app_path and data_path at runtime and passes the results
*  to ConfigManager. GetExecutableFolder() and GetDataFolder() are therefore under normal conditions
*  simply more efficient shortcuts for Read("app_path") and Read("data_path").
*/

wxString ConfigManager::GetProxy()
{
    return Manager::Get()->GetConfigManager(_T("app"))->Read(_T("network_proxy"));
}


wxString ConfigManager::GetFolder(SearchDirs dir)
{
    // cache home_folder
    if(ConfigManager::home_folder.IsEmpty())
    {
        wxFileName f;
        f.AssignHomeDir();
        ConfigManager::home_folder = f.GetFullPath();
        // remove trailing path separator to be consistent with the rest of the dirs
        if (ConfigManager::home_folder.Last() == wxFILE_SEP_PATH)
            ConfigManager::home_folder.RemoveLast();
    }

    // cache app_path
    if (ConfigManager::app_path.IsEmpty())
    {
        #ifdef __WXMSW__
        wxChar name[MAX_PATH];
        GetModuleFileName(0L, name, MAX_PATH);
        wxFileName fname(name);
        ConfigManager::app_path = fname.GetPath(wxPATH_GET_VOLUME);
        #else

        //    ConfigManager::app_path = wxString(SELFPATH,wxConvUTF8);
        //    ConfigManager::app_path = wxFileName(base).GetPath();
        //    if (ConfigManager::app_path.IsEmpty())
        ConfigManager::app_path = _T(".");
        #endif
    }

    // cache config_folder
    if(ConfigManager::config_folder.IsEmpty())
    {
        #ifdef __WXMSW__
        TCHAR szPath[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
        ConfigManager::config_folder = wxString(szPath) + _T("\\codeblocks");
        #else
        ConfigManager::config_folder = ConfigManager::home_folder + _T("/.codeblocks");
        #endif
        if(!wxDirExists(ConfigManager::config_folder))
            CreateDirRecursively(ConfigManager::config_folder);
    }

    // cache data_path_global
    if(ConfigManager::data_path_global.IsEmpty())
    {
        #ifdef __WXMSW__
        ConfigManager::data_path_global = ConfigManager::app_path + _T("/share/codeblocks");
        #else
        ConfigManager::data_path_global = _T("/usr/share/codeblocks"); // wildguess
        #endif
        if(!wxDirExists(ConfigManager::data_path_global))
            CreateDirRecursively(ConfigManager::data_path_global);
    }

    // cache data_path_user
    if(ConfigManager::data_path_user.IsEmpty())
    {
        ConfigManager::data_path_user = ConfigManager::config_folder + _T("/share/codeblocks");
        if(!wxDirExists(ConfigManager::data_path_user))
            CreateDirRecursively(ConfigManager::data_path_user);
    }

    switch (dir)
    {
        case sdHome:
            return ConfigManager::home_folder;

        case sdBase:
            return ConfigManager::app_path;

        case sdTemp:
        {
            static bool once = true;
            static wxString tempFolder;

            if(once)
            {
                once = false;

                wxString tempFile = wxFileName::CreateTempFileName(wxEmptyString);
                tempFolder = wxFileName(tempFile).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

                ::wxRemoveFile(tempFile);
            }
            return tempFolder;
        }

        case sdConfig:
            return ConfigManager::config_folder;

        case sdCurrent:
            return ::wxGetCwd();

        case sdPluginsGlobal:
        {
            wxString p = ConfigManager::data_path_global + _T("/plugins");
            if(!wxDirExists(p + wxFILE_SEP_PATH))
                CreateDirRecursively(p + wxFILE_SEP_PATH);
            return p;
        }

        case sdPluginsUser:
        {
            wxString p = ConfigManager::data_path_user + _T("/plugins");
            if(!wxDirExists(p + wxFILE_SEP_PATH))
                CreateDirRecursively(p + wxFILE_SEP_PATH);
            return p;
        }

        case sdScriptsGlobal:
        {
            wxString p = ConfigManager::data_path_global + _T("/scripts");
            if(!wxDirExists(p + wxFILE_SEP_PATH))
                CreateDirRecursively(p + wxFILE_SEP_PATH);
            return p;
        }

        case sdScriptsUser:
        {
            wxString p = ConfigManager::data_path_user + _T("/scripts");
            if(!wxDirExists(p + wxFILE_SEP_PATH))
                CreateDirRecursively(p + wxFILE_SEP_PATH);
            return p;
        }

        case sdDataGlobal:
            return ConfigManager::data_path_global;

        case sdDataUser:
            return ConfigManager::data_path_user;

        default:
            return wxEmptyString;
    }
}

wxString ConfigManager::LocateDataFile(const wxString& filename, int search_dirs)
{
    wxPathList searchPaths;

    // user dirs have precedence
    if (search_dirs & sdPluginsUser)
        searchPaths.Add(GetPluginsFolder(false));
    if (search_dirs & sdScriptsUser)
        searchPaths.Add(GetScriptsFolder(false));
    if (search_dirs & sdDataUser)
        searchPaths.Add(GetDataFolder(false));

    // then we have global dirs
    if (search_dirs & sdPluginsGlobal)
        searchPaths.Add(GetPluginsFolder(true));
    if (search_dirs & sdScriptsGlobal)
        searchPaths.Add(GetScriptsFolder(true));
    if (search_dirs & sdDataGlobal)
        searchPaths.Add(GetDataFolder(true));

    // rest of the dirs
    if (search_dirs & sdCurrent)
        searchPaths.Add(::wxGetCwd());
    if (search_dirs & sdConfig)
        searchPaths.Add(GetConfigFolder());
    if (search_dirs & sdHome)
        searchPaths.Add(GetHomeFolder());
    if (search_dirs & sdBase)
        searchPaths.Add(GetExecutableFolder());
    if (search_dirs & sdTemp)
        searchPaths.Add(GetTempFolder());

    // PATH env. var
    if (search_dirs & sdPath)
        searchPaths.AddEnvList(_T("PATH"));

    return searchPaths.FindValidPath(filename);
}



/* ------------------------------------------------------------------------------------------------------------------
*  ConfigManager
*/

ConfigManager::ConfigManager(TiXmlElement* r) : root(r), pathNode(r)
{
    doc = root->GetDocument();
}




/* ------------------------------------------------------------------------------------------------------------------
*  Configuration path handling
*  Note that due to namespaces, you do no longer need to care about saving/restoring the current path in the normal case.
*  Mostly, there will be only one module working with one namespace, and every namespace keeps track of its own path.
*  Also, please note that GetPath() is more expensive than it seems (not a mere accessor to a member variable!), while
*  SetPath() not only sets the current path, but also creates the respective nodes in the XML document if these don't exist.
*/

wxString ConfigManager::GetPath() const
{
    TiXmlElement *e = pathNode;
    wxString ret;
    ret.Alloc(64);

    ret = cbC2U(e->Value());
    while((e = e->Parent()->ToElement()) && e != root)
    {
        ret.Prepend(_T('/'));
        ret.Prepend(cbC2U(e->Value()));
    }
    ret.Prepend(_T('/'));
    return ret;
};

void ConfigManager::SetPath(const wxString& path)
{
    wxString p(path + _T('/'));
    pathNode = AssertPath(p);
}

wxString ConfigManager::InvalidNameMessage(const wxString& what, const wxString& sub, TiXmlElement *localPath) const
{
    wxString s;
    s.Printf(_T("The %s %s (child of node \"%s\" in namespace \"%s\") does not meet the standard for path naming (must start with a letter)."),
    what.c_str(),
    sub.c_str(),
    #if wxUSE_UNICODE
    cbC2U(localPath->Value()).c_str(),
    cbC2U(root->Value()).c_str());
    #else
    localPath->Value(),
    root->Value());
    #endif
    return s;
}


TiXmlElement* ConfigManager::AssertPath(wxString& path)
{
    Collapse(path);

    wxString illegal(_T(" -:.\"\'$&()[]<>+#"));
    size_t i = 0;
    while((i = path.find_first_of(illegal, i)) != wxString::npos)
        path[i] = _T('_');

    TiXmlElement *localPath = pathNode ? pathNode : root;

    if(path.GetChar(0) == '/')  // absolute path
    {
        localPath = root;
        path = path.Mid(1);
    }

    if(path.find(_T('/')) != wxString::npos) // need for path walking
        to_lower(path);

    wxString sub;

    while(path.find(_T('/')) != wxString::npos)
    {
        sub = path.BeforeFirst(_T('/'));
        path = path.AfterFirst(_T('/'));

        if(localPath != root && sub.IsSameAs(CfgMgrConsts::dotDot))
            localPath = localPath->Parent()->ToElement();
        else if(sub.GetChar(0) < _T('a') || sub.GetChar(0) > _T('z'))
        {
            cbThrow(InvalidNameMessage(_T("subpath"), sub, localPath));
        }
        else
        {
            TiXmlElement* n = localPath->FirstChildElement(cbU2C(sub));
            if(n)
                localPath = n;
            else
                localPath = (TiXmlElement*) localPath->InsertEndChild(TiXmlElement(cbU2C(sub)));
        }
    }

    to_upper(path);

    if(!path.IsEmpty() && (path.GetChar(0) < _T('A') || path.GetChar(0) > _T('Z')))
        cbThrow(InvalidNameMessage(_T("key"), path, localPath));

    return localPath;
}


/* -----------------------------------------------------------------------------------------------------
*  Clear all nodes from your namespace or delete the namespace alltogether (removing it from the config file).
*  After Delete() returns, the pointer to your instance is invalid.
*/

void ConfigManager::Clear()
{
    root->Clear();
}

void ConfigManager::Delete()
{
    CfgMgrBldr * bld = CfgMgrBldr::Get();
    wxString ns(cbC2U(root->Value()));

    root->Clear();
    doc->RootElement()->RemoveChild(root);

    wxCriticalSectionLocker(bld->cs);
    NamespaceMap::iterator it = bld->namespaces.find(ns);
    if(it != bld->namespaces.end())
        bld->namespaces.erase(it);

    delete this;
}

void ConfigManager::DeleteAll()
{
    CfgMgrBldr * bld = CfgMgrBldr::Get();
    wxString ns(cbC2U(root->Value()));

    if(!ns.IsSameAs(_T("app")))
        cbThrow(_T("Illegal attempt to invoke DeleteAll()."));

    wxCriticalSectionLocker(bld->cs);
    doc->RootElement()->Clear();
    for(NamespaceMap::iterator it = bld->namespaces.begin(); it != bld->namespaces.end(); ++it)
    {
        delete it->second;
        bld->namespaces.erase(it);
    }
}


/* ------------------------------------------------------------------------------------------------------------------
*  Utility functions for writing nodes
*/

TiXmlElement* ConfigManager::GetUniqElement(TiXmlElement* p, const wxString& q)
{
    TiXmlElement* r;
    if((r = p->FirstChildElement(cbU2C(q))))
        return r;

    return (TiXmlElement*)(p->InsertEndChild(TiXmlElement(cbU2C(q))));
}

void ConfigManager::SetNodeText(TiXmlElement* n, const TiXmlText& t)
{
    TiXmlNode *c = n->FirstChild();
    if(c)
        n->ReplaceChild(c, t);
    else
        n->InsertEndChild(t);
}



/* ------------------------------------------------------------------------------------------------------------------
*  Write and read values
*  Regardless of namespaces, the string keys app_path and data_path always refer to the location of the application's executable
*  and the data path, respectively. These values are never saved to the configuration, but kept in static variables.
*  The application makes use of this by "writing" to the configuration file after determining these values at runtime.
*/
void ConfigManager::Write(const wxString& name,  const wxString& value, bool ignoreEmpty)
{
    if(name.IsSameAs(CfgMgrConsts::app_path))
    {
        ConfigManager::app_path = value;
        return;
    }
    else if(name.IsSameAs(CfgMgrConsts::data_path))
    {
        ConfigManager::data_path_global = value;
        return;
    }
    if(ignoreEmpty && value.IsEmpty())
    {
        UnSet(name);
        return;
    }

    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *str = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(str, _T("str"));

    // Convert any < and > to XML entities or else the configuration will
    // not be able to open again...
    // This first happened to me when I tried to find a "<p>". When the
    // find dialog saved my last search in the configuration, I couldn't
    // launch C::B again.
    wxString convert(value);
    convert.Replace(_T("<"), _T("&lt;"));
    convert.Replace(_T(">"), _T("&gt;"));

    SetNodeText(s, TiXmlText(convert.mb_str(wxConvUTF8)));
}

void ConfigManager::Write(const wxString& key, const char* str)
{
    /* NOTE (mandrav#1#): Do *not* remove 'false' from the call because in ANSI builds,
    it matches this very function and overflows the stack... */
    Write(key, cbC2U(str), false);
};

wxString ConfigManager::Read(const wxString& name, const wxString& defaultVal)
{
    if(name.IsSameAs(CfgMgrConsts::app_path))
        return ConfigManager::app_path;
    else if(name.IsSameAs(CfgMgrConsts::data_path))
        return ConfigManager::data_path_global;

    wxString ret;

    if(Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name, wxString* str)
{
    if(name.IsSameAs(CfgMgrConsts::app_path))
    {
        str->assign(ConfigManager::app_path);
        return true;
    }
    else if(name.IsSameAs(CfgMgrConsts::data_path))
    {
        str->assign(ConfigManager::data_path_global);
        return true;
    }

    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlText *t = (TiXmlText *) parentHandle.FirstChild(cbU2C(key)).FirstChild("str").FirstChild().Node();

    if(t)
    {
        str->assign(cbC2U(t->Value()));
        return true;
    }
    return false;
}

void ConfigManager::Write(const wxString& name,  const wxColour& c)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(leaf, _T("colour"));
    s->SetAttribute("r", c.Red());
    s->SetAttribute("g", c.Green());
    s->SetAttribute("b", c.Blue());
}

wxColour ConfigManager::ReadColour(const wxString& name, const wxColour& defaultVal)
{
    wxColour ret;

    if(Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name, wxColour* ret)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *c = (TiXmlElement *) parentHandle.FirstChild(cbU2C(key)).FirstChild("colour").Element();

    if(c)
    {
        int r, g, b;
        if(c->QueryIntAttribute("r", &r) == TIXML_SUCCESS
                && c->QueryIntAttribute("g", &g) == TIXML_SUCCESS
                && c->QueryIntAttribute("b", &b) == TIXML_SUCCESS)
            ret->Set(r, g, b);
        return true;
    }
    return false;
}

void ConfigManager::Write(const wxString& name,  int value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetAttribute("int", value);
}

int  ConfigManager::ReadInt(const wxString& name,  int defaultVal)
{
    int ret;

    if(Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  int* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(cbU2C(key)).Element();

    if(leaf)
        return leaf->QueryIntAttribute("int", value) == TIXML_SUCCESS;
    return false;
}


void ConfigManager::Write(const wxString& name,  bool value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetAttribute("bool", value ? "1" : "0");
}

bool  ConfigManager::ReadBool(const wxString& name,  bool defaultVal)
{
    bool ret;

    if(Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  bool* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(cbU2C(key)).Element();

    if(leaf && leaf->Attribute("bool"))
    {
        *value = leaf->Attribute("bool")[0] == '1';
        return true;
    }
    return false;
}


void ConfigManager::Write(const wxString& name,  double value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetDoubleAttribute("double", value);
}

double  ConfigManager::ReadDouble(const wxString& name,  double defaultVal)
{
    double ret;

    if(Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  double* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(cbU2C(key)).Element();

    if(leaf)
        return leaf->QueryDoubleAttribute("double", value) == TIXML_SUCCESS;
    return false;
}


void ConfigManager::Set(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    GetUniqElement(e, key);
}

void ConfigManager::UnSet(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlNode *leaf = GetUniqElement(e, key);
    e->RemoveChild(leaf);
}

bool ConfigManager::Exists(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(cbU2C(key)).Element();

    return leaf;
}



void ConfigManager::Write(const wxString& name,  const wxArrayString& arrayString)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *as;
    as = GetUniqElement(leaf, _T("astr"));
    leaf->RemoveChild(as);
    as = GetUniqElement(leaf, _T("astr"));

    for(unsigned int i = 0; i < arrayString.GetCount(); ++i)
    {
        TiXmlElement s("s");
        s.InsertEndChild(TiXmlText(arrayString[i].mb_str(wxConvUTF8)));
        as->InsertEndChild(s);
    }
}

void ConfigManager::Read(const wxString& name, wxArrayString *arrayString)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlNode *asNode = parentHandle.FirstChild(cbU2C(key)).FirstChild("astr").Node();

    TiXmlNode *curr = 0;
    if(asNode)
    {
        while(curr = asNode->IterateChildren("s", curr))
            arrayString->Add(cbC2U(curr->FirstChild()->ToText()->Value()));
    }
}

wxArrayString ConfigManager::ReadArrayString(const wxString& name)
{
    wxArrayString as;
    Read(name, &as);
    return as;
}

void ConfigManager::WriteBinary(const wxString& name,  const wxString& source)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *str = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(str, _T("bin"));
    s->SetAttribute("crc", wxCrc32::FromString(source));
    SetNodeText(s, TiXmlText(wxBase64::Encode(source).mb_str(wxConvUTF8)));
}

void ConfigManager::WriteBinary(const wxString& name,  void* ptr, size_t len)
{
    wxString s((wxChar*)ptr, len);
    WriteBinary(name,  s);
}

wxString ConfigManager::ReadBinary(const wxString& name)
{
    wxString str;
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    unsigned int crc;

    TiXmlHandle parentHandle(e);
    TiXmlElement* bin = parentHandle.FirstChild(cbU2C(key)).FirstChild("bin").Element();

    if(!bin)
        return wxEmptyString;

    if(bin->QueryIntAttribute("crc", (int*)&crc) != TIXML_SUCCESS)
        return wxEmptyString;

    TiXmlText *t = bin->FirstChild()->ToText();
    if (t)
    {
        str.assign(cbC2U(t->Value()));
        str = wxBase64::Decode(str);
        if(crc ==  wxCrc32::FromString(str))
            return str;
    }
    return wxEmptyString;
}


wxArrayString ConfigManager::EnumerateSubPaths(const wxString& path)
{
    wxString key(path + _T('/')); // the trailing slash hack is required because AssertPath expects a key name
    TiXmlNode* e = AssertPath(key);
    wxArrayString ret;

    TiXmlElement *curr = 0;
    if(e)
    {
        while(e->IterateChildren(curr) && (curr = e->IterateChildren(curr)->ToElement()))
        {
            wxChar c = *(cbC2U(curr->Value()));
            if(c < _T('A') || c > _T('Z')) // first char must be a letter, uppercase letters are key names
                ret.Add(cbC2U(curr->Value()));
        }
    }
    return ret;
}

void ConfigManager::DeleteSubPath(const wxString& thePath)
{
    if(doc->ErrorId())
    {
        cbMessageBox(wxString(_T("### TinyXML error:\n")) << cbC2U(doc->ErrorDesc()));
        doc->ClearError();
    }

    wxString path(thePath);
    to_lower(path);

    Collapse(path);

    wxString illegal(_T(" :.,;!\"\'$%&()[]<>{}?*+-|#"));
    size_t i;
    while((i = path.find_first_of(illegal)) != wxString::npos)
        path[i] = _T('_');

    if(path.Last() == _T('/'))
        path.RemoveLast();

    if(path.IsSameAs(_T("/"))) // this function will refuse to remove root!
        return;

    TiXmlElement* parent = pathNode ? pathNode : root;

    if(path.find(_T('/')) != wxString::npos)
    {
        wxString sub;
        do
        {
            sub = path.BeforeFirst(_T('/'));
            path = path.AfterFirst(_T('/'));

            if(sub.IsEmpty())
                parent = root;
            else if(sub.IsSameAs(_T(".")))
                ;
            else if(parent != root && sub.IsSameAs(_T("..")))
                parent = parent->Parent()->ToElement();
            else
            {
                TiXmlElement* n = parent->FirstChildElement(cbU2C(sub));
                if(n)
                    parent = n;
                else
                    return;
            }
        }
        while(path.find(_T('/')) != wxString::npos);
    }

    if(!path.IsEmpty())
    {
        if(TiXmlNode *toRemove = parent->FirstChild(cbU2C(path)))
        {
            toRemove->Clear();
            parent->RemoveChild(toRemove);
        }
    }
}


wxArrayString ConfigManager::EnumerateKeys(const wxString& path)
{
    wxString key(path + _T('/')); // the trailing slash hack is required because AssertPath expects a key name
    TiXmlNode* e = AssertPath(key);
    wxArrayString ret;

    TiXmlElement *curr = 0;
    if(e)
    {
        while(e->IterateChildren(curr) && (curr = e->IterateChildren(curr)->ToElement()))
        {
            wxChar c = *(cbC2U(curr->Value()));
            if(c >= _T('A') && c <= _T('Z')) // opposite of the above
                ret.Add(cbC2U(curr->Value()));
        }
    }
    return ret;
}

void ConfigManager::Write(const wxString& name, const ISerializable& object)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *obj = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(obj, _T("obj"));
    SetNodeText(s, TiXmlText(cbU2C(wxBase64::Encode(object.SerializeOut()))));
}

bool ConfigManager::Read(const wxString& name, ISerializable* object)
{
    wxString str;
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlText *t = (TiXmlText *) parentHandle.FirstChild(cbU2C(key)).FirstChild("obj").FirstChild().Node();

    if(t)
    {
        str.assign(cbC2U(t->Value()));
        object->SerializeIn(wxBase64::Decode(str));
    }
    return wxEmptyString;
}

void ConfigManager::Write(const wxString& name, const ConfigManagerContainer::StringToStringMap& map)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *mNode;
    mNode = GetUniqElement(leaf, _T("ssmap"));
    leaf->RemoveChild(mNode);
    mNode = GetUniqElement(leaf, _T("ssmap"));

    for(ConfigManagerContainer::StringToStringMap::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        TiXmlElement s(cbU2C(it->first));
        s.InsertEndChild(TiXmlText(cbU2C(it->second)));
        mNode->InsertEndChild(s);
    }
}

void ConfigManager::Read(const wxString& name, ConfigManagerContainer::StringToStringMap* map)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlNode *mNode = parentHandle.FirstChild(cbU2C(key)).FirstChild("ssmap").Node();

    TiXmlNode *curr = 0;
    if(mNode)
    {
        while((curr = mNode->IterateChildren(curr)))
            (*map)[cbC2U(curr->Value())] = cbC2U(curr->FirstChild()->ToText()->Value());
    }
}

ConfigManagerContainer::StringToStringMap ConfigManager::ReadSSMap(const wxString& name)
{
    ConfigManagerContainer::StringToStringMap ret;
    Read(name, &ret);
    return ret;
}

void ConfigManager::Write(const wxString& name, const ConfigManagerContainer::IntToStringMap& map)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *mNode;
    mNode = GetUniqElement(leaf, _T("ismap"));
    leaf->RemoveChild(mNode);
    mNode = GetUniqElement(leaf, _T("ismap"));

    wxString tmp;
    for(ConfigManagerContainer::IntToStringMap::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        tmp.Printf(_T("x%d"), (int) it->first);
        TiXmlElement s(cbU2C(tmp));
        s.InsertEndChild(TiXmlText(cbU2C(it->second)));
        mNode->InsertEndChild(s);
    }
}

void ConfigManager::Read(const wxString& name, ConfigManagerContainer::IntToStringMap* map)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlNode *mNode = parentHandle.FirstChild(cbU2C(key)).FirstChild("ismap").Node();

    TiXmlNode *curr = 0;
    long tmp;
    if(mNode)
    {
        while((curr = mNode->IterateChildren(curr)))
        {
            cbC2U(curr->Value()).Mid(1).ToLong(&tmp);
            (*map)[tmp] = cbC2U(curr->FirstChild()->ToText()->Value());
        }
    }
}

ConfigManagerContainer::IntToStringMap ConfigManager::ReadISMap(const wxString& name)
{
    ConfigManagerContainer::IntToStringMap ret;
    Read(name, &ret);
    return ret;
}






void ConfigManager::Write(const wxString& name, const ConfigManagerContainer::StringSet& set)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *mNode;
    mNode = GetUniqElement(leaf, _T("sset"));
    leaf->RemoveChild(mNode);
    mNode = GetUniqElement(leaf, _T("sset"));

    for(ConfigManagerContainer::StringSet::const_iterator it = set.begin(); it != set.end(); ++it)
    {
        TiXmlElement s("s");
        s.InsertEndChild(TiXmlText(cbU2C(*it)));
        mNode->InsertEndChild(s);
    }
}


void ConfigManager::Read(const wxString& name, ConfigManagerContainer::StringSet* set)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlNode *mNode = parentHandle.FirstChild(cbU2C(key)).FirstChild("sset").Node();

    TiXmlNode *curr = 0;
    if(mNode)
    {
        while((curr = mNode->IterateChildren(curr)))
            set->insert(cbC2U(curr->FirstChild()->ToText()->Value()));
    }
}

ConfigManagerContainer::StringSet ConfigManager::ReadSSet(const wxString& name)
{
    ConfigManagerContainer::StringSet ret;
    Read(name, &ret);
    return ret;
}


void ConfigManager::Write(const wxString& name, const ConfigManagerContainer::SerializableObjectMap* map)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *mNode;
    mNode = GetUniqElement(leaf, _T("objmap"));
    leaf->RemoveChild(mNode);
    mNode = GetUniqElement(leaf, _T("objmap"));

    for(ConfigManagerContainer::SerializableObjectMap::const_iterator it = map->begin(); it != map->end(); ++it)
    {
        TiXmlElement s(cbU2C(it->first));
        s.InsertEndChild(TiXmlText(cbU2C(wxBase64::Encode(it->second->SerializeOut()))));
        mNode->InsertEndChild(s);
    }
}



#if wxUSE_UNICODE
bool ConfigManager::Unicode()
{
    return true;
}
#else
bool ConfigManager::Unicode()
{
    return false;
}
#endif

#ifdef __WIN32__
bool ConfigManager::Windows()
{
    return true;
}
bool ConfigManager::Unix()
{
    return false;
}
#endif

#if (defined(__WXGTK__)  || defined(__WXMAC__))
bool ConfigManager::Windows()
{
    return false;
}
bool ConfigManager::Unix()
{
    return true;
}
#endif

#ifdef __WXMAC__
bool ConfigManager::MacOS()
{
    return true;
}
#else
bool ConfigManager::MacOS()
{
    return false;
}
#endif

#ifdef __linux__
bool ConfigManager::Linux()
{
    return true;
}
#else
bool ConfigManager::Linux()
{
    return false;
}
#endif


