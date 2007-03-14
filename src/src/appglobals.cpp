/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#ifndef CB_PRECOMP
#include <wx/utils.h>       //wxGetOsVersion
#include <wx/intl.h>
#endif
#include "appglobals.h"
#include <autorevision.h>

namespace appglobals
{
    const wxString AppVendor              = _T("Code::Blocks");
    const wxString AppName			    = _T("Code::Blocks");

    #if SVN_BUILD
        const wxString AppVersion				= _T("svn build");
        const wxString AppActualVersionVerb	= _T("svn build  rev " SVN_REVISION);
        const wxString AppActualVersion		= _T("svn-r" SVN_REVISION);
    #else
        const wxString AppVersion				= _T(RELEASE);
        const wxString AppActualVersionVerb	= _T("Release " RELEASE "  rev " SVN_REVISION);
        const wxString AppActualVersion		= _T(RELEASE "-r" SVN_REVISION);
    #endif

    const wxString AppUrl					= _T("http://www.codeblocks.org");
    const wxString AppContactEmail		= _T("info@codeblocks.org");

    #if defined(__WXMSW__)
      const wxString AppPlatform = _T("Windows");
    #elif defined(__WXOS2__)
      const wxString AppPlatform = _T("OS/2");
    #elif defined(__WXMAC__)
      const wxString AppPlatform = _T("Mac OS X");
    #elif defined(__APPLE__)
      const wxString AppPlatform = _T("Darwin");
    #elif defined(__FreeBSD__)
      const wxString AppPlatform = _T("FreeBSD");
    #elif defined(__UNIX__)
      const wxString AppPlatform = _T("Linux");
    #else
      const wxString AppPlatform = _T("Unknown");
    #endif

    const wxString AppWXAnsiUnicode = platform::unicode ? _T("unicode") : wxT("ANSI");

    const wxString AppBuildTimestamp = (wxString(wxT(__DATE__)) + wxT(", ") + wxT(__TIME__) + wxT(" - wx") + wxString(wxT(wxVERSION_NUM_DOT_STRING)) + wxT(" (") + AppPlatform + wxT(", ") + AppWXAnsiUnicode + wxT(")") );

    const wxString DefaultBatchBuildArgs = _T("-na -nd -ns --batch-build-notify");
};


windows_version_t cb_get_os()
{
    if(!platform::windows)
        return winver_NotWindows;


	int Major = 0;
	int family = wxGetOsVersion(&Major, NULL);
	if(family == wxWIN95)
        return winver_Windows9598ME;

    if(family == wxWINDOWS_NT)
    {
        if(Major == 5)
            return winver_WindowsXP;

        if(Major == 6) // just guessing here, not sure if this is right
            return winver_Vista;

        return winver_WindowsNT2000;
    }
    else
        return winver_UnknownWindows;
};

windows_version_t WindowsVersion()
{
    static windows_version_t theOS = cb_get_os();
    return theOS;
}

