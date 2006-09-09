#include "wxsmithmime.h"

#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsextresmanager.h"

#include <licenses.h>

// TODO: Do not access wxSmith's maps directly

wxSmithMime::wxSmithMime()
{
	m_PluginInfo.name = _("wxSmithMime");
	m_PluginInfo.title = _("wxSmith - MIME plugin");
	m_PluginInfo.version = _("1.0");
	m_PluginInfo.description = _("Mime extension for wxSmith");
	m_PluginInfo.author = _("BYO");
	m_PluginInfo.authorEmail = _("byo.spoon@gmail.com");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _T("");
	m_PluginInfo.license = LICENSE_GPL;
}

bool wxSmithMime::CanHandleFile(const wxString& FileName) const
{
    if ( !wxsPlugin() ) return false;

    // Scanning for projects using this file
    for ( wxSmith::ProjectMapI i = wxsPlugin()->m_ProjectMap.begin();
          i!=wxsPlugin()->m_ProjectMap.end();
          ++i )
    {
        wxsProject* Proj = (*i).second;
        if ( Proj->CanOpenEditor(FileName) )
        {
            return true;
        }
    }

    if ( wxsExtRes()->CanOpen(FileName) )
    {
        return true;
    }

    return false;
}

int wxSmithMime::OpenFile(const wxString& FileName)
{
    if ( !wxsPlugin() ) return 1;

    // Scanning for projects using this file
    for ( wxSmith::ProjectMapI i = wxsPlugin()->m_ProjectMap.begin();
          i!=wxsPlugin()->m_ProjectMap.end();
          ++i )
    {
        wxsProject* Proj = (*i).second;
        if ( Proj->TryOpenEditor(FileName) )
        {
            return 0;
        }
    }

    if ( wxsExtRes()->Open(FileName) )
    {
        return 0;
    }

    return 1;
}
