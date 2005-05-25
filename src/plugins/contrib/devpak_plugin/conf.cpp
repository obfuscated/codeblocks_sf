#include "conf.h"
#include <wx/url.h>
#include <wx/filename.h>
#include <globals.h>

static UpdateRec* list = 0;
static int g_RecsCount = 0;
wxString g_MasterPath;

wxString GetSizeString(int bytes)
{
    wxString ret;
    float kilobytes = (float)bytes / 1024.0f;
    float megabytes = kilobytes / 1024.0f;
    if (megabytes >= 1.0f)
        ret.Printf("%.2f MB", megabytes);
    else if (kilobytes >= 1.0f)
        ret.Printf("%.2f KB", kilobytes);
    else
        ret.Printf("%ld bytes", bytes);
    return ret;
}

UpdateRec* ReadConf(const IniParser& ini, int* recCount, const wxString& currentServer, const wxString& appPath)
{
    *recCount = 0;
    int groupsCount = ini.GetGroupsCount();
    if (groupsCount == 0)
        return 0;

    UpdateRec* list = new UpdateRec[ini.GetGroupsCount()];
    for (int i = 0; i < groupsCount; ++i)
    {
    	UpdateRec& rec = list[i];

    	rec.title = ini.GetGroupName(i);
    	rec.name = ini.GetKeyValue(i, "Name");
    	rec.desc = ini.GetKeyValue(i, "Description");
    	rec.remote_file = ini.GetKeyValue(i, "RemoteFilename");
    	rec.local_file = ini.GetKeyValue(i, "LocalFilename");
    	rec.groups = GetArrayFromString(ini.GetKeyValue(i, "Group"), ",");
    	rec.install = ini.GetKeyValue(i, "InstallPath");
    	rec.version = ini.GetKeyValue(i, "Version");
        rec.bytes = atol(ini.GetKeyValue(i, "Size"));
    	rec.date = ini.GetKeyValue(i, "Date");
    	rec.installable = ini.GetKeyValue(i, "Execute") == "1";

        // read .entry file (if exists)
        rec.entry = (!rec.name.IsEmpty() ? rec.name : wxFileName(rec.local_file).GetName()) + ".entry";
        IniParser p;
        p.ParseFile(appPath + rec.entry);
        rec.installed_version = p.GetValue("Setup", "AppVersion");
        
        rec.downloaded = wxFileExists(appPath + "/" + rec.local_file);
        rec.installed = !rec.installed_version.IsEmpty();

        // calculate size
        rec.size = GetSizeString(rec.bytes);

        // fix-up
        if (rec.name.IsEmpty())
            rec.name = rec.title;
        rec.desc.Replace("<CR>", "\n");
        rec.desc.Replace("<LF>", "\r");
        wxURL url(rec.remote_file);
        if (!url.GetHostName().IsEmpty())
        {
            rec.remote_server = url.GetProtocolName() + "://" + url.GetHostName();
            int pos = rec.remote_file.Find(url.GetHostName());
            if (pos != wxNOT_FOUND)
                rec.remote_file.Remove(0, pos + url.GetHostName().Length() + 1);
        }
        else
            rec.remote_server = currentServer;
    }
    
    *recCount = groupsCount;
    return list;
}

UpdateRec* FindRecByTitle(const wxString& title, UpdateRec* list, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (list[i].title == title)
            return &list[i];
    }
    return 0;
}
