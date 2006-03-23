#include "wxsresourcefactory.h"

wxsResourceFactory wxsResourceFactory::Singleton;

wxsResource* wxsResourceFactory::Build(const wxString& ResourceType,wxsProject* Project)
{
    #define RES(ClassName,TypeName) if ( _T(#TypeName) == ResourceType ) return new ClassName(Project);
//    RES(wxsDialog,dialog)
//    RES(wxsFrame,frame)
//    RES(wxsPanel,panel)
    #undef RES
    return NULL;
}
