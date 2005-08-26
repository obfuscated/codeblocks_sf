#ifndef GLOBALS_H
#define GLOBALS_H

#define APP_VENDOR				_T("Code::Blocks")
#define APP_NAME				_T("Code::Blocks")
#define APP_VERSION				_T("1.0")
#define APP_ACTUAL_VERSION_VERB	_T("1.0 CVS")
#define APP_ACTUAL_VERSION		_T("1.0-cvs")
#define APP_URL					_T("http://www.codeblocks.org")
#define APP_CONTACT_EMAIL		_T("mandrav@codeblocks.org")

#if defined(__WXMSW__)
  #define APP_PLATFORM _T("Windows")
#elif defined(__WXOS2__)
  #define APP_PLATFORM _T("OS/2")
#elif defined(__WXMAC__)
  #define APP_PLATFORM _T("Mac OS/X")
#elif defined(__UNIX__)
  #define APP_PLATFORM _T("Linux")
#else
  #define APP_PLATFORM _T("Unknown")
#endif

#if wxUSE_UNICODE
  #define APP_WXANSI_UNICODE wxT("unicode")
#else
  #define APP_WXANSI_UNICODE wxT("ANSI")
#endif

#define APP_BUILD_TIMESTAMP	(wxString(wxT(__DATE__)) + wxT(", ") + wxT(__TIME__) + wxT(" - wx") + wxString(wxT(wxVERSION_NUM_DOT_STRING)) + wxT(" (") + APP_PLATFORM + wxT(", ") + APP_WXANSI_UNICODE + wxT(")") )

#endif // GLOBALS_H
