#include "wxsheaders.h"
#include "wxsmithmime.h"

#include "wxsmith.h"
#include "resources/wxswindowres.h"
#include "wxsextresmanager.h"

#include <licenses.h>

wxSmithMime::wxSmithMime()
{
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
    else if ( FN.GetExt().Upper() == _T("XRC") )
    {
        return true;
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
    else if ( FN.GetExt().Upper() == _T("XRC") )
    {
        wxsEXTRES()->OpenXrc(FileName);
        return 0;
    }

    return 1;
}
