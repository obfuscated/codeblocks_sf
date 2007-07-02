#ifndef APPGLOBALS_H
#define APPGLOBALS_H

#include <wx/string.h>
#include <wx/intl.h>

#ifndef RELEASE
    #define RELEASE "svn"
    #define SVN_BUILD 1
#endif



namespace appglobals
{
    typedef struct { int locale_code; wxString name; } Localisation;

    extern const wxString AppVendor;
    extern const wxString AppName;
    extern const wxString AppVersion;
    extern const wxString AppActualVersionVerb;
    extern const wxString AppActualVersion;
    extern const wxString AppUrl;
    extern const wxString AppContactEmail;
    extern const wxString AppPlatform;
    extern const wxString AppWXAnsiUnicode;
    extern const wxString AppBuildTimestamp;

    extern const wxString DefaultBatchBuildArgs;

};

#endif // APPGLOBALS_H
