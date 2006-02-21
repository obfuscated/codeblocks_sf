#ifndef CBEXCEPTION_H
#define CBEXCEPTION_H

#include <wx/intl.h> // for _() and _T()
#include <wx/log.h> // for wxSafeShowMessage()
#include <globals.h>

#include "configmanager.h"

/**
@brief Code::Blocks error handling unit.

Two macros are defined to help error handling in Code::Blocks:
cbThrow() and cbAssert().
@note cbAssert() does nothing in release mode. For debug mode, compile with -DcbDEBUG
*/

class cbException
{
	public:
		cbException(const wxString& msg, const wxString& file, int line);
		virtual ~cbException();
		void ShowErrorMessage(bool safe = true);

        // public members
		wxString Message;
		wxString File;
		int Line;
};

#ifdef wxUSE_UNICODE
    #define cbThrow(message) throw cbException(message, _U(__FILE__), __LINE__)
#else
    #define cbThrow(message) throw cbException(message, __FILE__, __LINE__)
#endif

#ifndef cbDEBUG
    #define cbAssert(expr)
#else
    // In unix we use kill to terminate the application, that makes gdb
    // keep it alive which makes debugging easier.
    // (thanks go to an unknown author)
    #ifdef __WXMSW__
        #define DIE() exit(1)
    #else
        #include <csignal>
        #define DIE() kill(0, SIGTERM)
    #endif

    #define cbAssert(expr) \
        if (!(expr)) \
        { \
            wxString err; \
            err.Printf(_T("Assertion %s failed (%s:%d)"), #expr, _U(__FILE__).c_str(), __LINE__); \
            wxSafeShowMessage(_T("Assertion error"), err); \
            DIE(); \
        }
#endif

#endif // CBEXCEPTION_H
