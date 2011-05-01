/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBEXCEPTION_H
#define CBEXCEPTION_H

#include <wx/string.h>
#include <wx/log.h> // for wxSafeShowMessage()

/**
@brief Code::Blocks error handling unit.

Two macros are defined to help error handling in Code::Blocks:
cbThrow() and cbAssert().
@note cbAssert() does nothing in release mode. For debug mode, compile with -DcbDEBUG
*/

/** @brief The base Code::Blocks exception object. */
class cbException
{
    public:
        cbException(const wxString& msg, const wxString& file, int line);
        virtual ~cbException();

        /** @brief Display exception error message.
          * @param safe If true, wxSafeShowMessage will be used to display the error,
          *             else a normal message box will be used.
          */
        void ShowErrorMessage(bool safe = true);

        // public members
        wxString Message; ///< The exception's error message.
        wxString File; ///< The file where the exception was raised.
        int Line; ///< The line in the file where the exception was raised.
};

#if wxUSE_UNICODE
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
        #define DIE() kill(getpid(), SIGTERM)
    #endif

    #if wxUSE_UNICODE
        #define cbAssertMessage(expr) \
            wxString err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), cbC2U(__PRETTY_FUNCTION__).c_str(), cbC2U(__FILE__).c_str(), __LINE__, cbC2U(#expr).c_str());
    #else
        #define cbAssertMessage(expr) \
            wxString err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), __PRETTY_FUNCTION__, __FILE__, __LINE__, #expr);
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
