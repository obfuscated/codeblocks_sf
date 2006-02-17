#ifndef APPGLOBALS_H
#define APPGLOBALS_H

#include <wx/string.h>

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
#define LANGUAGES  2
//Please fix this. I have no Idea that the WXSIZEOF couldn't work whithout defining LANGUAGES
extern const wxString langs[LANGUAGES];
extern const int locales[LANGUAGES];

typedef enum
{
osUnknownWindows,
osWindows9598ME,
osWindowsNT2000,
osWindowsXP,
osLinux,
osOS2,
osOSX
}operating_system_t;

operating_system_t OS();

#endif // APPGLOBALS_H
