#ifndef CBEXTENSION_H
#define CBEXTENSION_H

#include <wx/intl.h> // for _() and _T()
#include <wx/log.h> // for wxSafeShowMessage()
#include <wx/msgdlg.h> // for wxMessageBox()

/**
@brief Code::Blocks error handling unit.

Two macros are defined to help error handling in Code::Blocks:
cbThrow() and cbAssert().
@note cbAssert() does nothing in release mode. For debug mode, compile with -DcbDEBUG
*/

class cbException
{
	public:
		cbException(const wxString& msg, const wxString& file, int line)
            : Message(msg),
            File(file),
            Line(line)
		{
		}
		virtual ~cbException(){}
		void ShowErrorMessage(bool safe = true)
		{
            wxString title = _("Exception");
		    wxString err;
		    err.Printf(_("An exception has been raised!\n\n"
                        "The application encountered an error at %s, on line %d.\n"
                        "The error message is:\n\n%s"),
                        File.c_str(), Line, Message.c_str());
            if (safe)
                wxSafeShowMessage(title, err);
            else
                wxMessageBox(err, title, wxICON_ERROR);
		}

        // public members
		wxString Message;
		wxString File;
		int Line;
};
#define cbThrow(message) throw cbException(message, __FILE__, __LINE__)

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
            err.Printf(_("Assertion %s failed (%s:%d)"), #expr, __FILE__, __LINE__); \
            wxSafeShowMessage(_("Assertion error"), err); \
            DIE(); \
        }
#endif

#endif // CBEXTENSION_H
