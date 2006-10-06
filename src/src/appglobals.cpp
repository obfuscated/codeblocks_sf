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

const wxString g_AppVendor              = _T("Code::Blocks");
const wxString g_AppName			    = _T("Code::Blocks");
const wxString g_AppVersion				= _T("1.0");
const wxString g_AppActualVersionVerb	= _T("1.0 revision " SVN_REVISION);
const wxString g_AppActualVersion		= _T("1.0-r" SVN_REVISION);
const wxString g_AppUrl					= _T("http://www.codeblocks.org");
const wxString g_AppContactEmail		= _T("info@codeblocks.org");

#if defined(__WXMSW__)
  const wxString g_AppPlatform = _T("Windows");
#elif defined(__WXOS2__)
  const wxString g_AppPlatform = _T("OS/2");
#elif defined(__WXMAC__)
  const wxString g_AppPlatform = _T("Mac OS X");
#elif defined(__APPLE__)
  const wxString g_AppPlatform = _T("Darwin");
#elif defined(__FreeBSD__)
  const wxString g_AppPlatform = _T("FreeBSD");
#elif defined(__UNIX__)
  const wxString g_AppPlatform = _T("Linux");
#else
  const wxString g_AppPlatform = _T("Unknown");
#endif

#if wxUSE_UNICODE
  const wxString g_AppWXAnsiUnicode = wxT("unicode");
#else
  const wxString g_AppWXAnsiUnicode = wxT("ANSI");
#endif

const wxString g_AppBuildTimestamp = (wxString(wxT(__DATE__)) + wxT(", ") + wxT(__TIME__) + wxT(" - wx") + wxString(wxT(wxVERSION_NUM_DOT_STRING)) + wxT(" (") + g_AppPlatform + wxT(", ") + g_AppWXAnsiUnicode + wxT(")") );

const wxString g_DefaultBatchBuildArgs = _T("-na -nd -ns --batch-build-notify");

operating_system_t __cb_get_os()
{
#if defined(__FreeBSD__)
    return osFreeBSD;
#elif defined(__UNIX__)
    return osLinux;
#elif defined(__WXMAC__)
    return osOSX;
#elif defined(__WXOS2__)
    return osOS2;
#elif defined(__WXMSW__)
	int Major = 0;
	int family = wxGetOsVersion(&Major, NULL);
	if(family == wxWIN95)
        return osWindows9598ME;
    if(family == wxWINDOWS_NT)
    {
        if(Major > 4)
            return osWindowsXP;
        else
            return osWindowsNT2000;
    }
    else
        return osUnknownWindows;
#else
#error Unable to determine OS
#endif
};

operating_system_t OS()
{
    static operating_system_t theOS = __cb_get_os();
    return theOS;
}


const wxString langs[LANGUAGES_SIZE] =
{
	_T("(System default)")   // Do *not* translate. If you do, people may be unable to switch back if they accidentially change the language
//        ,_T("English (U.S.)")
//        ,_T("English")
	,_("Chinese (Simplified)")
//        ,_T("German")
//        ,_T("Russian")
};

// Must have the same order than the above
const int locales[LANGUAGES_SIZE] =
{
	wxLANGUAGE_DEFAULT
//        ,wxLANGUAGE_ENGLISH_US
//        ,wxLANGUAGE_ENGLISH
	,wxLANGUAGE_CHINESE_SIMPLIFIED
//        ,wxLANGUAGE_GERMAN
//        ,wxLANGUAGE_RUSSIAN
};
