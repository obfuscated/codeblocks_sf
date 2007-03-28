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

    static const Localisation languages[] = /* TODO: Some day in the future, remove that hardcoded stuff alltogether */
    {
        { wxLANGUAGE_DEFAULT,            _T("(System default)") },  // don't translate system default
    //    { wxLANGUAGE_ENGLISH,          _("English") },
    //    { wxLANGUAGE_ENGLISH_US,       _("English (US)") },
    //    { wxLANGUAGE_FRENCH,           _("French") },
    //    { wxLANGUAGE_GERMAN,           _("German") },
    //    { wxLANGUAGE_SPANISH,          _("Spanish") },
    //    { wxLANGUAGE_RUSSIAN,          _("Russian") },
        { wxLANGUAGE_CHINESE_SIMPLIFIED, _("Chinese (Simplified)") }
    };


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
