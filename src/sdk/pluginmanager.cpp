/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/filesys.h>
    #include <wx/intl.h>
    #include <wx/menu.h>
    #include <wx/string.h>

    #include "pluginmanager.h"
    #include "cbexception.h"
    #include "cbplugin.h"
    #include "infowindow.h"
    #include "messagemanager.h"
    #include "manager.h"
    #include "editormanager.h"
    #include "configmanager.h"
    #include "personalitymanager.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif

#include <wx/dynlib.h>
#include <wx/filesys.h>
#include <wx/progdlg.h>
#include <wx/utils.h>
#include <wx/filename.h>

#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/txtstrm.h>

#include "filefilters.h"
#include "tinyxml/tinyxml.h"

#include "annoyingdialog.h"
#include "pluginsconfigurationdlg.h"

#include <map>

void VersionStringToNumbers(const wxString& version, long* major, long* minor, long* release)
{
    wxString majorS = version.BeforeFirst(_T('.')); // 6.3.2 -> 6
    wxString minorS = version.AfterFirst(_T('.')); // 6.3.2 -> 3.2
    wxString releaseS = version.AfterLast(_T('.')); // 6.3.2 -> 2
    minorS = minorS.BeforeFirst(_T('.')); // 3.2 -> 3
    if (major)
        majorS.ToLong(major);
    if (minor)
        minorS.ToLong(minor);
    if (release)
        releaseS.ToLong(release);
}

// returns -1 if new is less then old, 0 if equal and 1 if new is greater than old
int CompareVersions(const wxString& new_version, const wxString& old_version)
{
    long new_major, new_minor, new_release;
    long old_major, old_minor, old_release;

    VersionStringToNumbers(new_version, &new_major, &new_minor, &new_release);
    VersionStringToNumbers(old_version, &old_major, &old_minor, &old_release);

#define SIGN(a) (a>0?1:(a<0?-1:0))
    int result = 0;
    result += SIGN(new_major - old_major) << 2;
    result += SIGN(new_minor - old_minor) << 1;
    result += SIGN(new_release - old_release) << 0;
#undef SIGN

    if (result < 0) return -1;
    else if (result > 0) return 1;
    return 0;
}

namespace LibLoader
{
    struct RefCountedLib
    {
        RefCountedLib() : lib(0), ref(0) {}
        wxDynamicLibrary* lib;
        int ref;
    };
    typedef std::map<wxString, RefCountedLib> Libs;
    Libs s_Libs;

    wxDynamicLibrary* LoadLibrary(const wxString& filename)
    {
        Libs::iterator it = s_Libs.find(filename);
        if (it != s_Libs.end())
        {
            // existing lib
            it->second.ref++;
            return it->second.lib;
        }
        // new lib
        it = s_Libs.insert(s_Libs.end(), std::make_pair(filename, RefCountedLib()));
        it->second.lib = new wxDynamicLibrary;
        it->second.ref = 1;
        it->second.lib->Load(filename);
        return it->second.lib;
    }

    void RemoveLibrary(wxDynamicLibrary* lib)
    {
        Libs::iterator it;
        for (it = s_Libs.begin(); it != s_Libs.end(); ++it)
        {
            RefCountedLib& rcl = it->second;
            if (rcl.lib == lib)
            {
                // found
                rcl.ref--;
                if (rcl.ref == 0)
                {
                    // only delete the lib if not shutting down
                    // if we are shutting down, it will be deleted automatically
                    if (!Manager::IsAppShuttingDown())
                        delete rcl.lib;
                    s_Libs.erase(it);
                }
                return;
            }
        }
        // if we reached here, it's a lib that was not handled by us
        // (or had wrong refcounting)
    }

    void Cleanup()
    {
        Libs::iterator it;
        for (it = s_Libs.begin(); it != s_Libs.end(); ++it)
        {
            RefCountedLib& rcl = it->second;
            // only delete the lib if not shutting down
            // if we are shutting down, it will be deleted automatically
            if (!Manager::IsAppShuttingDown())
                delete rcl.lib;
        }
        s_Libs.clear();
    }
};

// this is used by NotifyPlugins
// it just keeps a pointer to the last active plugin in the chain...
static cbPlugin* s_LastKnownActivePlugin = 0;

// class constructor
PluginManager::PluginManager()
    : m_pCurrentlyLoadingLib(0),
    m_pCurrentlyLoadingManifestDoc(0)
{
}

// class destructor
PluginManager::~PluginManager()
{
    UnloadAllPlugins();
}

void PluginManager::CreateMenu(wxMenuBar* menuBar)
{
}

void PluginManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

bool PluginManager::AttachPlugin(cbPlugin* plugin)
{
    if (!plugin)
        return false;
    if (plugin->IsAttached())
        return true;

    plugin->Attach();
    s_LastKnownActivePlugin = plugin;
    return true;
}

bool PluginManager::DetachPlugin(cbPlugin* plugin)
{
    if (!plugin)
        return false;
    if (!plugin->IsAttached())
        return true;

    plugin->Release(Manager::IsAppShuttingDown());
    if (s_LastKnownActivePlugin == plugin)
    {
        if (m_Plugins.GetCount())
            s_LastKnownActivePlugin = m_Plugins[m_Plugins.GetCount() - 1]->plugin;
        else
            s_LastKnownActivePlugin = 0;
    }
    return true;
}

bool PluginManager::InstallPlugin(const wxString& pluginName)
{
    if (pluginName.IsEmpty())
        return false;

    // base name
    wxString basename = wxFileName(pluginName).GetName();
    wxString version;
    if (basename.Contains(_T('-')))
    {
        version = basename.AfterFirst(_T('-'));
        basename = basename.BeforeFirst(_T('-'));
    }

//    DBGLOG(_T("InstallPlugin: basename='%s', version=%s"), basename.c_str(), version.c_str());

    // if plugin with the same name exists, ask to uninstall first
    cbPlugin* existingPlugin = FindPluginByName(basename);
    if (existingPlugin)
    {
        wxString msg = _("A plugin with the same name is already installed.\n");
        if (!version.IsEmpty())
        {
            const PluginInfo* existingInfo = GetPluginInfo(existingPlugin);
            if (CompareVersions(version, existingInfo->version) < 0)
            {
                msg = _("The plugin you are trying to install, is older "
                        "than the one currently installed.");
            }
        }

        if (cbMessageBox(msg + _T('\n') +
                        _("If you want to proceed, the installed plugin will be "
                        "uninstalled first.\n"
                        "Do you want to proceed?"),
                        _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_NO)
        {
            return false;
        }
        if (!UninstallPlugin(existingPlugin))
            return false;
    }

    wxProgressDialog pd(_("Installing: ") + basename, _T("A description wide enough for the dialog ;)"), 4);

    wxString localName = basename + FileFilters::DYNAMICLIB_DOT_EXT;
    wxString resourceName = basename + _T(".zip");
    #ifndef __WXMSW__
    if (resourceName.StartsWith(_T("lib")))
        resourceName.Remove(0, 3);
    #endif
    wxString pluginFilename = ConfigManager::GetPluginsFolder() + _T('/') + localName;
//    DBGLOG(_T("Plugin filename: ") + pluginFilename);
//    DBGLOG(_T("Plugin resources: ") + ConfigManager::GetDataFolder() + _T('/') + resourceName);

    pd.Update(1, _("Extracting plugin"));

    // extract plugin from bundle
    if (!ExtractFile(pluginName,
                    localName,
                    pluginFilename))
        return false;
//    DBGLOG(_T("Extracted plugin"));

    pd.Update(2, _("Extracting plugin resources"));

    // extract resources from bundle
    if (!ExtractFile(pluginName,
                    resourceName,
                    ConfigManager::GetDataFolder() + _T('/') + resourceName))
        return false;
//    DBGLOG(_T("Extracted resources"));

    pd.Update(3, _("Loading plugin"));

    // bundle extracted; now load the plugin on-the-fly
//    DBGLOG(_T("Loading plugin..."));
    ScanForPlugins(ConfigManager::GetPluginsFolder());
    LoadAllPlugins();
    cbPlugin* plugin = FindPluginByFileName(pluginFilename);
    const PluginInfo* info = GetPluginInfo(plugin);
    if (!plugin || !info)
    {
        DBGLOG(_T("Failed"));
        return false;
    }
//    DBGLOG(_T("Succeeded"));

    // inform app to update menus and toolbars
    pd.Update(4, _("Updating menus and toolbars"));
    CodeBlocksEvent evt(cbEVT_PLUGIN_INSTALLED);
    evt.SetPlugin(plugin);
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
//    DBGLOG(_T("Menus updated"));

    return true;
}

bool PluginManager::UninstallPlugin(cbPlugin* plugin, bool removeFiles)
{
    if (!plugin)
        return false;

    wxString title;
    wxString pluginFilename;
    wxString resourceFilename;

    // find the plugin element
    for (size_t i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* elem = m_Plugins[i];
        if (elem && elem->plugin == plugin)
        {
            // got it
            title = elem->info.title;
            pluginFilename = elem->fileName;
            // now get the resource name
            wxFileName fname(pluginFilename);
            resourceFilename = fname.GetName() + _T(".zip");
            #ifndef __WXMSW__
            if (resourceFilename.StartsWith(_T("lib")))
                resourceFilename.Remove(0, 3);
            #endif
            resourceFilename = ConfigManager::LocateDataFile(resourceFilename, sdDataGlobal | sdDataUser);
            break;
        }
    }

//    DBGLOG(_T("UninstallPlugin:"));
//    DBGLOG(_T("Plugin filename: ") + pluginFilename);
//    DBGLOG(_T("Plugin resources: ") + resourceFilename);

    wxProgressDialog pd(wxString::Format(_("Uninstalling %s"), title.c_str()),
                        _T("A description wide enough for the dialog ;)"), 3);

    pd.Update(1, _("Detaching plugin"));
    DetachPlugin(plugin);
//    DBGLOG(_T("Plugin released"));

    pd.Update(2, _("Updating menus and toolbars"));
    CodeBlocksEvent event(cbEVT_PLUGIN_UNINSTALLED);
    event.SetPlugin(plugin);
    Manager::Get()->GetAppWindow()->ProcessEvent(event);
//    DBGLOG(_T("Menus updated"));

    pd.Update(3, _("Unloading plugin"));
    UnloadPlugin(plugin);
//    DBGLOG(_T("Plugin unloaded"));

    if (!removeFiles)
        return true;

    // under linux, if the progress dialog is still visible and updated
    // causes a crash because it re-enters gtk_main_iteration() calling
    // eventually OnUpdateUI() in the config dialog, which in turn references
    // an invalid plugin...
//    pd.Update(4, _("Removing files"));

    if (!pluginFilename.IsEmpty())
    {
        if (wxRemoveFile(pluginFilename))
        {
//            DBGLOG(_T("Plugin file removed"));
            if (!resourceFilename.IsEmpty())
            {
                if (!wxRemoveFile(resourceFilename))
                    LOG_WARN(_T("Failed to remove plugin resources: ") + resourceFilename);
            }
            return true;
        }
        else
        {
            LOG_WARN(_T("Failed to remove plugin file: ") + pluginFilename);
            cbMessageBox(_("Plugin could not be completely uninstalled because its files could not be removed.\n\n"
                            "This can happen if the plugin's file is in-use like, for "
                            "example, when the same plugin file provides more than one "
                            "plugin.\n"
                            "In this case either uninstall all other plugins "
                            "which are provided by the same file, or remove it yourself "
                            "(manually) when you shut down Code::Blocks.\n"
                            "The files that could not be deleted are:\n\n") +
                            pluginFilename + _T('\n') +
                            resourceFilename,
                            _("Warning"), wxICON_WARNING);
            return false;
        }
    }
    return false;
}

bool PluginManager::ExportPlugin(cbPlugin* plugin, const wxString& filename)
{
    if (!plugin)
        return false;

    wxArrayString sourcefiles;

    // find the plugin element
    for (size_t i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* elem = m_Plugins[i];
        if (elem && elem->plugin == plugin)
        {
            // got it
            sourcefiles.Add(elem->fileName);
            // now get the resource name
            wxFileName fname(elem->fileName);
            wxString resourceFilename = fname.GetName() + _T(".zip");
            #ifndef __WXMSW__
            if (resourceFilename.StartsWith(_T("lib")))
                resourceFilename.Remove(0, 3);
            #endif
            resourceFilename = ConfigManager::LocateDataFile(resourceFilename, sdDataGlobal | sdDataUser);
            sourcefiles.Add(resourceFilename);
            break;
        }
    }

    if (wxFileExists(filename))
    {
        if (!wxFile::Access(filename, wxFile::write))
        {
            cbMessageBox(wxString::Format(_("%s is in use.\nAborting..."), filename.c_str()),
                        _("Warning"), wxICON_WARNING);
            return false;
        }
    }

//    DBGLOG(_T("Creating archive: ") + filename);
    wxFileOutputStream out(filename);
    wxZipOutputStream zip(out, 9); // max compression
    for (size_t i = 0; i < sourcefiles.GetCount(); ++i)
    {
        wxFileInputStream in(sourcefiles[i]);
        zip.PutNextEntry(wxFileName(sourcefiles[i]).GetFullName());
        zip << in;
    }
    zip.SetComment(_T("This is a redistributable plugin for the Code::Blocks IDE.\n"
                        "See http://www.codeblocks.org for details..."));

    return true;
}

bool PluginManager::ExtractFile(const wxString& bundlename,
                                const wxString& src_filename,
                                const wxString& dst_filename)
{
//    DBGLOG(_T("ExtractFile:"));
//    DBGLOG(_T("Plugin filename: ") + bundlename);
//    DBGLOG(_T("Source filename: ") + src_filename);
//    DBGLOG(_T("Destination filename: ") + dst_filename);

    // check if the destination file already exists
    if (wxFileExists(dst_filename) && !wxFile::Access(dst_filename, wxFile::write))
    {
//        DBGLOG(_T("Destination file in use"));
        cbMessageBox(_("The destination file is in use.\nAborting..."), _("Warning"), wxICON_WARNING);
        return false;
    }

    // actually extract file
//    DBGLOG(_T("Extracting..."));
    wxFileSystem* fs = new wxFileSystem;
    wxFSFile* f = fs->OpenFile(bundlename + _T("#zip:") + src_filename);
    if (f)
    {
        // open output file for writing
        wxFile output(dst_filename, wxFile::write);
        if (!output.IsOpened())
        {
//            DBGLOG(_T("Can't open destination file for writing"));
            wxString msg = wxString::Format(_T("Can't open destination file '%s' for writing..."),
                                            dst_filename.c_str());
            cbMessageBox(msg, _("Error"), wxICON_ERROR);
            delete f;
            delete fs;
            return false;
        }

        // copy file
        wxInputStream* is = f->GetStream();
        char tmp[1025] = {};
        while (!is->Eof() && is->CanRead())
        {
            memset(tmp, 0, sizeof(tmp));
            is->Read(tmp, sizeof(tmp) - 1);
            output.Write(tmp, is->LastRead());
        }
        delete f;
//        DBGLOG(_T("Extracted"));
    }
    else
    {
//        DBGLOG(_T("File not found in plugin"));
        wxString msg = wxString::Format(_T("File '%s' not found in plugin '%s'"),
                                        src_filename.c_str(), bundlename.c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        delete fs;
        return false;
    }
    delete fs;
    return true;
}

void PluginManager::RegisterPlugin(const wxString& name,
                                    CreatePluginProc createProc,
                                    FreePluginProc freeProc,
                                    PluginSDKVersionProc versionProc)
{
    // sanity checks
    if (name.IsEmpty() || !createProc || !freeProc || !versionProc)
        return;

    // first check to see it's not already loaded
    if (FindPluginByName(name))
        return; // yes, already loaded

    // read manifest file for plugin
    PluginInfo info;
    if (!ReadManifestFile(m_CurrentlyLoadingFilename, name, &info) ||
        info.name.IsEmpty())
    {
        LOG_ERROR(_T("Invalid manifest file for: ") + name);
        return;
    }

    // now get the SDK version number (extra check)
    int major;
    int minor;
    int release;
    versionProc(&major, &minor, &release);
    if (major != PLUGIN_SDK_VERSION_MAJOR ||
        minor != PLUGIN_SDK_VERSION_MINOR ||
        release != PLUGIN_SDK_VERSION_RELEASE)
    {
        // wrong version: in this case, inform the user...
        wxString fmt;
        fmt.Printf(_("SDK version mismatch for %s (%d.%d.%d). Expecting %d.%d.%d"),
                    name.c_str(),
                    major,
                    minor,
                    release,
                    PLUGIN_SDK_VERSION_MAJOR,
                    PLUGIN_SDK_VERSION_MINOR,
                    PLUGIN_SDK_VERSION_RELEASE);
        LOG_ERROR(fmt);
        return;
    }

    // all done
    // add this plugin in the temporary registration vector to be loaded
    // by LoadPlugin() (which triggered the call to this function).
    PluginRegistration pr;
    pr.name = name;
    pr.createProc = createProc;
    pr.freeProc = freeProc;
    pr.versionProc = versionProc;
    pr.info = info;
    m_RegisteredPlugins.push_back(pr);
}

bool PluginManager::ReadManifestFile(const wxString& pluginFilename,
                                    const wxString& pluginName,
                                    PluginInfo* infoOut)
{
    if (!m_pCurrentlyLoadingManifestDoc)
    {
        // find and load plugin's resource file
        // (pluginFilename contains no path info)
        wxFileName fname(pluginFilename);
        fname.SetExt(_T("zip"));
        wxString actual = fname.GetFullName();
        #if defined(__WXGTK__) || defined(__WXMAC__)
        // remove 'lib' prefix from plugin name (if any)
        if (actual.StartsWith(_T("lib")))
            actual.Remove(0, 3);
        #endif
        actual = ConfigManager::LocateDataFile(actual, sdPluginsUser | sdDataUser | sdPluginsGlobal | sdDataGlobal);
        if (actual.IsEmpty())
        {
            LOG_ERROR(_T("Plugin resource not found: %s"), fname.GetFullName().c_str());
            return false; // not found
        }

        // load XML from ZIP
        wxString contents;
        wxFileSystem* fs = new wxFileSystem;
        wxFSFile* f = fs->OpenFile(actual + _T("#zip:manifest.xml"));
        if (f)
        {
            wxInputStream* is = f->GetStream();
            char tmp[1024] = {};
            while (!is->Eof() && is->CanRead())
            {
                memset(tmp, 0, sizeof(tmp));
                is->Read(tmp, sizeof(tmp) - 1);
                contents << cbC2U((const char*)tmp);
            }
            delete f;
        }
        else
        {
            LOG_ERROR(_T("No plugin manifest file in resource: %s"), actual.c_str());
            delete fs;
            return false;
        }
        delete fs;

        // actually load XML document
        m_pCurrentlyLoadingManifestDoc = new TiXmlDocument;
        if (!m_pCurrentlyLoadingManifestDoc->Parse(cbU2C(contents)))
            return false;
    }

    TiXmlElement* root = m_pCurrentlyLoadingManifestDoc->FirstChildElement("CodeBlocks_plugin_manifest_file");
    if (!root)
        return false;

    TiXmlElement* version = root->FirstChildElement("SdkVersion");
    if (!version)
        return false;

    // check version
//    int major;
//    int minor;
//    int release;
//    if (version->QueryIntAttribute("major", &major) != TIXML_SUCCESS)
//        major = 0;
//    if (version->QueryIntAttribute("minor", &minor) != TIXML_SUCCESS)
//        minor = 0;
//    if (version->QueryIntAttribute("release", &release) != TIXML_SUCCESS)
//        release = 0;
//
//    if (major != PLUGIN_SDK_VERSION_MAJOR ||
//        minor != PLUGIN_SDK_VERSION_MINOR ||
//        release != PLUGIN_SDK_VERSION_RELEASE)
//    {
//        // wrong version: in this case, inform the user...
//        wxString fmt;
//        fmt.Printf(_("SDK version mismatch for %s (%d.%d.%d). Expecting %d.%d.%d"),
//                    pluginName.c_str(),
//                    major,
//                    minor,
//                    release,
//                    PLUGIN_SDK_VERSION_MAJOR,
//                    PLUGIN_SDK_VERSION_MINOR,
//                    PLUGIN_SDK_VERSION_RELEASE);
//        LOG_ERROR(fmt);
//        return false;
//    }

    // if no plugin name specified, we 're done here (succesfully)
    if (pluginName.IsEmpty() || !infoOut)
        return true;

    TiXmlElement* plugin = root->FirstChildElement("Plugin");
    while (plugin)
    {
        const char* name = plugin->Attribute("name");
        if (name && cbC2U(name) == pluginName)
        {
            infoOut->name = pluginName;
            TiXmlElement* value = plugin->FirstChildElement("Value");
            while (value)
            {
                if (value->Attribute("title"))
                    infoOut->title = cbC2U(value->Attribute("title"));
                if (value->Attribute("version"))
                    infoOut->version = cbC2U(value->Attribute("version"));
                if (value->Attribute("description"))
                    infoOut->description = cbC2U(value->Attribute("description"));
                if (value->Attribute("author"))
                    infoOut->author = cbC2U(value->Attribute("author"));
                if (value->Attribute("authorEmail"))
                    infoOut->authorEmail = cbC2U(value->Attribute("authorEmail"));
                if (value->Attribute("authorWebsite"))
                    infoOut->authorWebsite = cbC2U(value->Attribute("authorWebsite"));
                if (value->Attribute("thanksTo"))
                    infoOut->thanksTo = cbC2U(value->Attribute("thanksTo"));
                if (value->Attribute("license"))
                    infoOut->license = cbC2U(value->Attribute("license"));

                value = value->NextSiblingElement("Value");
            }
            break;
        }

        plugin = plugin->NextSiblingElement("Plugin");
    }

    return true;
}

int PluginManager::ScanForPlugins(const wxString& path)
{
#ifdef __WXMSW__
    #define PLUGINS_MASK _T("*.dll")
#else
    #define PLUGINS_MASK _T("*.so")
#endif

    int count = 0;
    wxDir dir(path);

    if (!dir.IsOpened())
        return count;

    bool batch = Manager::IsBatchBuild();

    wxString filename;
    wxString failed;
    bool ok = dir.GetFirst(&filename, PLUGINS_MASK, wxDIR_FILES);
    while (ok)
    {
        if(batch && filename.Matches(_T("*compiler*")) == false)
        {
            ok = dir.GetNext(&filename);
            continue;
        }

        // load manifest
        m_pCurrentlyLoadingManifestDoc = 0;
        if (ReadManifestFile(filename))
        {
            if (LoadPlugin(path + _T('/') + filename))
                ++count;
            else
                failed << _T('\n') << filename;
        }
        delete m_pCurrentlyLoadingManifestDoc;
        m_pCurrentlyLoadingManifestDoc = 0;
        ok = dir.GetNext(&filename);
    }
    Manager::Get()->GetMessageManager()->Log(_("Found %d plugins"), count);
    if (!failed.IsEmpty())
    {
        InfoWindow::Display(_("Warning"),
                            _("One or more plugins were not loaded.\n"
                            "This usually happens when a plugin is built for\n"
                            "a different version of the Code::Blocks SDK.\n"
                            "Check the application log for more info.\n\n"
                            "List of failed plugins:\n") + failed,
                            15000, 3000);
    }
    return count;

#undef PLUGINS_MASK
}

bool PluginManager::LoadPlugin(const wxString& pluginName)
{
    wxLogNull zero; // no need for error messages; we check everything ourselves...

    // clear registration temporary vector
    m_RegisteredPlugins.clear();

    // load library
    m_CurrentlyLoadingFilename = pluginName;
    m_pCurrentlyLoadingLib = LibLoader::LoadLibrary(pluginName);
    if (!m_pCurrentlyLoadingLib->IsLoaded())
    {
        LOG_ERROR(_T("%s: not loaded (missing symbols?)"), pluginName.c_str());
        LibLoader::RemoveLibrary(m_pCurrentlyLoadingLib);
        m_pCurrentlyLoadingLib = 0;
        m_CurrentlyLoadingFilename.Clear();
        return false;
    }

    // by now, the library has loaded and its global variables are initialized.
    // this means it has already called RegisterPlugin()
    // now we can actually create the plugin(s) instance(s) :)

    // try to load the plugin(s)
    std::vector<PluginRegistration>::iterator it;
    for (it = m_RegisteredPlugins.begin(); it != m_RegisteredPlugins.end(); ++it)
    {
        PluginRegistration& pr = *it;
        cbPlugin* plug = 0L;
        try
        {
            plug = pr.createProc();
        }
        catch (cbException& exception)
        {
            exception.ShowErrorMessage(false);
            continue;
        }

        // all done; add it to our list
        PluginElement* plugElem = new PluginElement;
        plugElem->fileName = m_CurrentlyLoadingFilename;
        plugElem->info = pr.info;
        plugElem->library = m_pCurrentlyLoadingLib;
        plugElem->freeProc = pr.freeProc;
        plugElem->plugin = plug;
        m_Plugins.Add(plugElem);

        SetupLocaleDomain(pr.name);

        Manager::Get()->GetMessageManager()->DebugLog(_T("%s: loaded"), pr.name.c_str());
    }

    if (m_RegisteredPlugins.empty())
    {
        // no plugins loaded from this library, but it's not an error
        LibLoader::RemoveLibrary(m_pCurrentlyLoadingLib);
    }
    m_pCurrentlyLoadingLib = 0;
    m_CurrentlyLoadingFilename.Clear();
    return true;
}

void PluginManager::LoadAllPlugins()
{
    // check if a plugin crashed the app last time
    wxString probPlugin = Manager::Get()->GetConfigManager(_T("plugins"))->Read(_T("/try_to_activate"), wxEmptyString);
    if (!probPlugin.IsEmpty())
    {
        wxString msg;
        msg.Printf(_("Plugin \"%s\" failed to load last time Code::Blocks was executed.\n"
                    "Do you want to disable this plugin from loading?"), probPlugin.c_str());
        if (cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO) == wxID_NO)
            probPlugin = _T("");
    }

    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* elem = m_Plugins[i];
        cbPlugin* plug = elem->plugin;
        if (!plug || plug->IsAttached())
            continue;

        // do not load it if the user has explicitly asked not to...
        wxString baseKey;
        baseKey << _T("/") << elem->info.name;
        bool loadIt = Manager::Get()->GetConfigManager(_T("plugins"))->ReadBool(baseKey, true);

        // if we have a problematic plugin, check if this is it
        if (loadIt && !probPlugin.IsEmpty())
        {
            loadIt = elem->info.title != probPlugin;
            // if this is the problematic plugin, don't load it
            if (!loadIt)
                Manager::Get()->GetConfigManager(_T("plugins"))->Write(baseKey, false);
        }

        if (loadIt)
        {
            Manager::Get()->GetConfigManager(_T("plugins"))->Write(_T("/try_to_activate"), elem->info.title);
            Manager::Get()->GetMessageManager()->AppendLog(_("%s "), elem->info.name.c_str());
            try
            {
                AttachPlugin(plug);
                Manager::Get()->GetConfigManager(_T("plugins"))->Write(_T("/try_to_activate"), wxEmptyString, false);
            }
            catch (cbException& exception)
            {
                Manager::Get()->GetMessageManager()->AppendLog(_T("[failed] "));
                exception.ShowErrorMessage(false);

                wxString msg;
                msg.Printf(_("Plugin \"%s\" failed to load...\n"
                            "Do you want to disable this plugin from loading next time?"), elem->info.title.c_str());
                if (cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO) == wxID_YES)
                    Manager::Get()->GetConfigManager(_T("plugins"))->Write(baseKey, false);
            }
        }
    }
    Manager::Get()->GetMessageManager()->Log(_T(""));

    wxLogNull ln;
    Manager::Get()->GetConfigManager(_T("plugins"))->Write(_T("/try_to_activate"), wxEmptyString, false);
}

void PluginManager::UnloadAllPlugins()
{
//    Manager::Get()->GetMessageManager()->DebugLog("Count %d", m_Plugins.GetCount());

    s_LastKnownActivePlugin = 0;

    while (m_Plugins.GetCount())
    {
        UnloadPlugin(m_Plugins[0]->plugin);
    }
    m_Plugins.Clear();
    LibLoader::Cleanup();
}

void PluginManager::UnloadPlugin(cbPlugin* plugin)
{
    if (!plugin)
        return;

    // detach plugin if needed
    DetachPlugin(plugin);

    // find plugin element
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->plugin == plugin)
        {
            // found
            // free plugin
            if (plugElem->freeProc)
                plugElem->freeProc(plugin);
            else
                delete plugin; // try to delete it ourselves...
            // remove lib
            LibLoader::RemoveLibrary(plugElem->library);
            // and delete plugin element
            delete plugElem;
            m_Plugins.RemoveAt(i);

            if (s_LastKnownActivePlugin == plugin)
            {
                if (m_Plugins.GetCount())
                    s_LastKnownActivePlugin = m_Plugins[m_Plugins.GetCount() - 1]->plugin;
                else
                    s_LastKnownActivePlugin = 0;
            }
            break;
        }
    }
}

PluginElement* PluginManager::FindElementByName(const wxString& pluginName)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->info.name == pluginName)
            return plugElem;
    }

    return 0;
}

cbPlugin* PluginManager::FindPluginByName(const wxString& pluginName)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->info.name == pluginName)
            return plugElem->plugin;
    }

    return 0;
}

cbPlugin* PluginManager::FindPluginByFileName(const wxString& pluginFileName)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->fileName == pluginFileName)
            return plugElem->plugin;
    }

    return 0;
}

const PluginInfo* PluginManager::GetPluginInfo(const wxString& pluginName)
{
    PluginElement* plugElem = FindElementByName(pluginName);
    if (plugElem && plugElem->info.name == pluginName)
        return &plugElem->info;

    return 0;
}

const PluginInfo* PluginManager::GetPluginInfo(cbPlugin* plugin)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        PluginElement* plugElem = m_Plugins[i];
        if (plugElem->plugin == plugin)
            return &plugElem->info;
    }

    return 0;
}

int PluginManager::ExecutePlugin(const wxString& pluginName)
{
    PluginElement* elem = FindElementByName(pluginName);
    cbPlugin* plug = elem ? elem->plugin : 0;
    if (plug)
    {
        if (plug->GetType() != ptTool)
        {
            LOG_ERROR(_T("Plugin %s is not a tool to have Execute() method!"), elem->info.name.c_str());
        }
        else
        {
            try
            {
                return ((cbToolPlugin*)plug)->Execute();
            }
            catch (cbException& exception)
            {
                exception.ShowErrorMessage(false);
            }
        }
    }

    return 0;
}

int PluginManager::ConfigurePlugin(const wxString& pluginName)
{
    cbPlugin* plug = FindPluginByName(pluginName);
    if (plug)
    {
        try
        {
            return plug->Configure();
        }
        catch (cbException& exception)
        {
            exception.ShowErrorMessage(false);
        }
    }
    return 0;
}

int SortByConfigurationPriority(cbPlugin** first, cbPlugin** second)
{
    return (*first)->GetConfigurationPriority() - (*second)->GetConfigurationPriority();
}

void PluginManager::GetConfigurationPanels(int group, wxWindow* parent, ConfigurationPanelsArray& arrayToFill)
{
    // build an array of Plugins* because we need to order it by configuration priority
    PluginsArray arr;
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        // all check are done here
        if (plug && plug->IsAttached() && (plug->GetConfigurationGroup() & group))
            arr.Add(plug);
    }

    // sort the array
    arr.Sort(SortByConfigurationPriority);

    // now enumerate the array and fill the supplied configurations panel array
    arrayToFill.Clear();
    for (unsigned int i = 0; i < arr.GetCount(); ++i)
    {
        cbPlugin* plug = arr[i];
        cbConfigurationPanel* pnl = plug->GetConfigurationPanel(parent);
        if (pnl)
            arrayToFill.Add(pnl);
    }
}

void PluginManager::GetProjectConfigurationPanels(wxWindow* parent, cbProject* project, ConfigurationPanelsArray& arrayToFill)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (plug && plug->IsAttached())
        {
            cbConfigurationPanel* pnl = plug->GetProjectConfigurationPanel(parent, project);
            if (pnl)
                arrayToFill.Add(pnl);
        }
    }
}

PluginsArray PluginManager::GetToolOffers()
{
    return GetOffersFor(ptTool);
}

PluginsArray PluginManager::GetMimeOffers()
{
    return GetOffersFor(ptMime);
}

PluginsArray PluginManager::GetCompilerOffers()
{
    return GetOffersFor(ptCompiler);
}

PluginsArray PluginManager::GetDebuggerOffers()
{
    return GetOffersFor(ptDebugger);
}

PluginsArray PluginManager::GetCodeCompletionOffers()
{
    return GetOffersFor(ptCodeCompletion);
}

PluginsArray PluginManager::GetOffersFor(PluginType type)
{
    PluginsArray arr;

    // special case for MIME plugins
    // we 'll add the default MIME handler, last in the returned array
    cbPlugin* dflt = 0;

    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (plug && plug->IsAttached() && plug->GetType() == type)
        {
            if (type == ptMime)
            {
                // default MIME handler?
                if (((cbMimePlugin*)plug)->HandlesEverything())
                    dflt = plug;
                else
                    arr.Add(plug);
            }
            else
                arr.Add(plug);
        }
    }

    // add default MIME handler last
    if (dflt)
        arr.Add(dflt);

    return arr;
}

void PluginManager::AskPluginsForModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
    {
        cbPlugin* plug = m_Plugins[i]->plugin;
        if (plug)
        {
            try
            {
                plug->BuildModuleMenu(type, menu, data);
            }
            catch (cbException& exception)
            {
                exception.ShowErrorMessage(false);
            }
        }
    }
}

void PluginManager::NotifyPlugins(CodeBlocksEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    /* Things are simpler than before.
     * Just ask the last active plugin to process this event.
     * Because plugins are linked to the main app's event handler,
     * the event will travel up the chain normally.
     */
    if (s_LastKnownActivePlugin)
        s_LastKnownActivePlugin->ProcessEvent(event);
    else
        Manager::Get()->GetAppWindow()->ProcessEvent(event);
//    // notify plugins
//    for (unsigned int i = 0; i < m_Plugins.GetCount(); ++i)
//    {
//        cbPlugin* plug = m_Plugins[i]->plugin;
//        if (plug && plug->IsAttached())
//            plug->ProcessEvent(event);
//    }
//
//    // notify the app too
//    Manager::Get()->GetAppWindow()->ProcessEvent(event);
}

cbMimePlugin* PluginManager::GetMIMEHandlerForFile(const wxString& filename)
{
    PluginsArray mimes = GetMimeOffers();
    for (unsigned int i = 0; i < mimes.GetCount(); ++i)
    {
        cbMimePlugin* plugin = (cbMimePlugin*)mimes[i];
        if (plugin && plugin->CanHandleFile(filename))
            return plugin;
    }
    return 0;
}

int PluginManager::Configure()
{
    PluginsConfigurationDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
    return dlg.ShowModal();
}

void PluginManager::SetupLocaleDomain(const wxString& DomainName)
{
    int catalogNum=Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/locale/catalogNum"),(int)0);
    int i = 1;
    for (; i <= catalogNum; ++i)
    {
        wxString catalogName=Manager::Get()->GetConfigManager(_T("app"))->Read(wxString::Format(_T("/locale/Domain%d"), i), wxEmptyString);
        if (catalogName.Cmp(DomainName) == 0)
            break;
    }
    if (i > catalogNum)
    {
        ++catalogNum;
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/locale/catalogNum"), (int)catalogNum);
        Manager::Get()->GetConfigManager(_T("app"))->Write(wxString::Format(_T("/locale/Domain%d"), i), DomainName);
    }
}
