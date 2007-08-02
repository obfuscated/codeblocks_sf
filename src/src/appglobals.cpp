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
#include <wx/utils.h>
#include <wx/intl.h>
#include "configmanager.h"
#endif
#include "appglobals.h"

namespace appglobals
{
    const wxString AppVendor              = _T("Code::Blocks");
    const wxString AppName			    = _T("Code::Blocks");

    #if SVN_BUILD
        const wxString AppVersion				= _T("svn build");
        const wxString AppActualVersionVerb	= _T("svn build  rev ") +  ConfigManager::GetRevisionString();
        const wxString AppActualVersion		= _T("svn-r") +  ConfigManager::GetRevisionString();
    #else
        const wxString AppVersion				= _T(RELEASE);
        const wxString AppActualVersionVerb	= _T("Release " RELEASE "  rev ") + ConfigManager::GetRevisionString();
        const wxString AppActualVersion		= _T(RELEASE "-r") + ConfigManager::GetRevisionString();
    #endif

    const wxString AppUrl					= _T("http://www.codeblocks.org");
    const wxString AppContactEmail		= _T("info@codeblocks.org");

    #if defined(__WXMSW__)
      const wxString AppPlatform = _T("Windows");
    #elif defined(__WXOS2__)
      const wxString AppPlatform = _T("OS/2");
    #elif defined(__WXMAC__) || defined(__WXCOCOA__)
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



