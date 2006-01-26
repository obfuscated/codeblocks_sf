#include <sdk.h>
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
  const wxString g_AppPlatform = _T("Mac OS/X");
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
