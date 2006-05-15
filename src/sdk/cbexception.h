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
    #define cbThrow(message) throw cbException(message, cbC2U(__FILE__), __LINE__)
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

    #ifdef wxUSE_UNICODE
        #define cbAssertMessage(expr) \
            wxString err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), cbC2U(__PRETTY_FUNCTION__).c_str(), cbC2U(__FILE__).c_str(), __LINE__, cbC2U(#expr).c_str());
    #else
        #define cbAssertMessage(expr) \
            wxString err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), __PRETTY_FUNCTION__.c_str(), __FILE__.c_str(), __LINE__, #expr.c_str());
    #endif

    // non-fatal assertion
    #define cbAssertNonFatal(expr) \
        if (!(expr)) \
        { \
            cbAssertMessage(expr); \
            wxSafeShowMessage(_T("Assertion error"), err); \
        }

    // fatal assertion
    #define cbAssert(expr) \
        if (!(expr)) \
        { \
            cbAssertMessage(expr); \
            wxSafeShowMessage(_T("Fatal assertion error"), err); \
            DIE(); \
        }
#endif

#endif // CBEXCEPTION_H
