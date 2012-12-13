/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef APPGLOBALS_H
#define APPGLOBALS_H

#include <wx/string.h>
#include <wx/intl.h>

//#define RELEASE "12.11"

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
    extern const wxString AppSDKVersion;
    extern const wxString AppUrl;
    extern const wxString AppContactEmail;
    extern const wxString AppPlatform;
    extern const wxString AppWXAnsiUnicode;
    extern const wxString AppBuildTimestamp;

    extern const wxString DefaultBatchBuildArgs;
};

#endif // APPGLOBALS_H
