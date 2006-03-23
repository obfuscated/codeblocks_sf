#include "wxsmithmime.h"

#include "wxsmith.h"
#include "wxsproject.h"
#include "resources/wxswindowres.h"
#include "wxsextresmanager.h"

#include <licenses.h>

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
    if ( !wxsPLUGIN() ) return false;

    wxFileName FN(FileName);

    if ( FN.GetExt().Upper() == _T("WXS") ||
         FN.GetExt().Upper() == _T("XRC") )
    {
        return true;
    }

    return false;
}

int wxSmithMime::OpenFile(const wxString& FileName)
{
    if ( !wxsPLUGIN() ) return 1;

    // Scanning for projects using this file
    for ( wxSmith::ProjectMapI i = wxsPLUGIN()->ProjectMap.begin();
          i != wxsPLUGIN()->ProjectMap.end(); ++i )
    {
        wxsProject* Proj = (*i).second;
        if ( !Proj ) continue;

        if ( Proj->TryOpenEditor(FileName) )
        {
            return 0;
        }
    }

    if ( wxsEXTRES()->Open(FileName) )
    {
        return 0;
    }

    return 1;
}
