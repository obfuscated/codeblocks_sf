#include "wxsheaders.h"
#include "wxsmithmime.h"

#include "wxsmith.h"
#include "resources/wxswindowres.h"

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
	m_PluginInfo.hasConfigure = false;
}

bool wxSmithMime::CanHandleFile(const wxString& FileName) const
{
    if ( !wxsPLUGIN() ) return false;
    
    wxFileName FN(FileName);
    
    if ( FN.GetExt().Upper() == _T("WXS") )
    {
        for ( wxSmith::ProjectMapI i = wxsPLUGIN()->ProjectMap.begin();
              i != wxsPLUGIN()->ProjectMap.end();
              ++i )
        {
            wxsProject* Proj = (*i).second;
            if ( !Proj ) continue;
            
            for ( wxsProject::DialogListI i = Proj->Dialogs.begin();
                  i != Proj->Dialogs.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName ) return true;
            }
            
            for ( wxsProject::FrameListI i = Proj->Frames.begin();
                  i != Proj->Frames.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName ) return true;
            }

            for ( wxsProject::PanelListI i = Proj->Panels.begin();
                  i != Proj->Panels.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName ) return true;
            }
        }
    }
    
    return false;
}

int wxSmithMime::OpenFile(const wxString& FileName)
{
    if ( !wxsPLUGIN() ) return 1;
    
    wxFileName FN(FileName);
    
    if ( FN.GetExt().Upper() == _T("WXS") )
    {
        for ( wxSmith::ProjectMapI i = wxsPLUGIN()->ProjectMap.begin();
              i != wxsPLUGIN()->ProjectMap.end();
              ++i )
        {
            wxsProject* Proj = (*i).second;
            if ( !Proj ) continue;
            
            for ( wxsProject::DialogListI i = Proj->Dialogs.begin();
                  i != Proj->Dialogs.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName )
                {
                    (*i)->EditOpen();
                    return 0;
                }
            }
            
            for ( wxsProject::FrameListI i = Proj->Frames.begin();
                  i != Proj->Frames.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName )
                {
                    (*i)->EditOpen();
                    return 0;
                }
            }

            for ( wxsProject::PanelListI i = Proj->Panels.begin();
                  i != Proj->Panels.end();
                  ++i )
            {
                if ( (*i)->GetWxsFile() == FileName )
                {
                    (*i)->EditOpen();
                    return 0;
                }
            }
        }
    }
    
    return 1;
}
