#ifndef APPGLOBALS_H
#define APPGLOBALS_H

// revision.h is auto-created by tools/AutoRevision/autorevision.exe
// whenever the repository revision is changed...
#include "revision.h"

const wxString APP_VENDOR               = _T("Code::Blocks");
const wxString APP_NAME				    = _T("Code::Blocks");
const wxString APP_VERSION				= _T("1.0");
const wxString APP_ACTUAL_VERSION_VERB	= _T("1.0 revision ") + svnRevision;
const wxString APP_ACTUAL_VERSION		= _T("1.0-r") + svnRevision;
const wxString APP_URL					= _T("http://www.codeblocks.org");
const wxString APP_CONTACT_EMAIL		= _T("info@codeblocks.org");

#if defined(__WXMSW__)
  const wxString APP_PLATFORM = _T("Windows");
#elif defined(__WXOS2__)
  const wxString APP_PLATFORM = _T("OS/2");
#elif defined(__WXMAC__)
  const wxString APP_PLATFORM = _T("Mac OS/X");
#elif defined(__UNIX__)
  const wxString APP_PLATFORM = _T("Linux");
#else
  const wxString APP_PLATFORM = _T("Unknown");
#endif

#if wxUSE_UNICODE
  const wxString APP_WXANSI_UNICODE = wxT("unicode");
#else
  const wxString APP_WXANSI_UNICODE = wxT("ANSI");
#endif

const wxString APP_BUILD_TIMESTAMP = (wxString(wxT(__DATE__)) + wxT(", ") + wxT(__TIME__) + wxT(" - wx") + wxString(wxT(wxVERSION_NUM_DOT_STRING)) + wxT(" (") + APP_PLATFORM + wxT(", ") + APP_WXANSI_UNICODE + wxT(")") );

#endif // APPGLOBALS_H
