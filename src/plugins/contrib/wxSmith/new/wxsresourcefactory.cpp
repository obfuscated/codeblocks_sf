#include "wxsresourcefactory.h"

#include "resources/wxsdialogres.h"
#include "resources/wxsframeres.h"
#include "resources/wxspanelres.h"

wxsResourceFactory wxsResourceFactory::Singleton;

wxsResource* wxsResourceFactory::Build(const wxString& ResourceType,wxsProject* Project)
{
    #define RES(ClassName,TypeName) if ( _T(#TypeName) == ResourceType ) return new ClassName(Project);

    // For compatibility with older resource types, using old names
    RES(wxsDialogRes,dialog)
    RES(wxsFrameRes,frame)
    RES(wxsPanelRes,panel)

    RES(wxsDialogRes,wxDialog)
    RES(wxsFrameRes,wxFrame)
    RES(wxsPanelRes,wxPanel)
    #undef RES
    return NULL;
}

int wxsResourceFactory::GetResTypesCnt()
{
    return 3;
}

wxString wxsResourceFactory::GetResType(int No)
{
    switch ( No )
    {
        case 0: return _T("wxDialog");
        case 1: return _T("wxFrame");
        case 2: return _T("wxPanel");
    }
    return _T("");
}

wxString wxsResourceFactory::GetResBrowserName(int No)
{
    switch ( No )
    {
        case 0: return _("wxDialog resources");
        case 1: return _("wxFrame resources");
        case 2: return _("wxPanel resources");
    }
    return _T("");
}
