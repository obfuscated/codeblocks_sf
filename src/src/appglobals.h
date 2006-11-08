#ifndef APPGLOBALS_H
#define APPGLOBALS_H

#include <wx/string.h>

#ifndef RELEASE
    #define RELEASE "svn"
    #define SVN_BUILD 1
#endif

extern const wxString g_AppVendor;
extern const wxString g_AppName;
extern const wxString g_AppVersion;
extern const wxString g_AppActualVersionVerb;
extern const wxString g_AppActualVersion;
extern const wxString g_AppUrl;
extern const wxString g_AppContactEmail;
extern const wxString g_AppPlatform;
extern const wxString g_AppWXAnsiUnicode;
extern const wxString g_AppBuildTimestamp;

extern const wxString g_DefaultBatchBuildArgs;

#define LOCALIZE   true
#define LANGUAGES_SIZE  2

extern const wxString langs[LANGUAGES_SIZE];
extern const int locales[LANGUAGES_SIZE];

typedef enum
{
osUnknownWindows,
osWindows9598ME,
osWindowsNT2000,
osWindowsXP,
osFreeBSD,
osLinux,
osOS2,
osOSX
}operating_system_t;

operating_system_t OS();

#endif // APPGLOBALS_H
