#ifndef LOGMGR_H
#define LOGMGR_H

#include "manager.h"
#include "logger.h"

namespace
{
	static const unsigned int max_logs = ((65535^61>>3)<<2)*!!!!sizeof(int*)%63;

	wxString F(const wxChar* msg, ...)
	{
		va_list arg_list;
		va_start(arg_list, msg);
		::temp_string.Printf(msg, arg_list);
		va_end(arg_list);

        return ::temp_string;
	};

    static NullLogger g_null_log;
}


struct LogSlot
{
    Logger* log;
    size_t index;
    friend class LogManager;

public:

    wxBitmap *icon;
    wxString title;

    LogSlot() : log(0) {};
    ~LogSlot() { delete log; };

    size_t Index() const { return index; };

    void SetLogger(Logger* in) { if(log != &g_null_log) delete log; log = in; };
    Logger* GetLogger() const { return log; };
};



class DLLIMPORT LogManager : public Mgr<LogManager>
{
        LogSlot slot[max_logs+1];

		LogManager();


        friend class Mgr<LogManager>;
        friend class Manager;

public:

        enum { no_index = -1, invalid_log, stdout_log, app_log, debug_log};

        /* ------------------------------------------------------------------------------------------------------
         * Management functions
         * ------------------------------------------------------------------------------------------------------
         * Unless you are writing your own loggers, you will probably not need any of these.
         * SetLog() transfers ownership of the Logger object to the LogManager. Loggers must be heap allocated.
         * On error, SetLog() returns invalid_log
         */
        size_t SetLog(Logger* l, int index = no_index);
		void DeleteLog(int i) { SetLog(&g_null_log, i); };
        LogSlot& Slot(int i) { return slot[i]; };




        /* ------------------------------------------------------------------------------------------------------
         * Logging functions
         * ------------------------------------------------------------------------------------------------------
         * This section is what most people will be interested in.
         *   - Log(), LogWarning(), and LogError() output "info", "warning", or "error" messages to a log.
         *     Log() is almost certainly the function that you want to use, if you don't know what to pick.
         *     By default, logging is directed to the application log.
         *   - DebugLog() and DebugLogError() direct their output into the debug log.
         *   - LogToStdOut() outputs a message on stdout. Normally you will not want to use this function, it
         *     exists solely for some special cases.
         */

		void Log(const wxString& msg, int i = app_log, Logger::level lv = Logger::info) { slot[i].log->Append(msg, lv); };
		void LogWarning(const wxString& msg, int i = app_log) { Log(msg, i, Logger::warning); };
		void LogError(const wxString& msg, int i = app_log) { Log(msg, i, Logger::error); };

		void DebugLog(const wxString& msg, Logger::level lv = Logger::info) { Log(msg, debug_log, lv); };
		void DebugLogError(const wxString& msg) { DebugLog(msg, Logger::error); };

		void LogToStdOut(const wxString& msg, Logger::level lv = Logger::info) { Log(msg, stdout_log, lv); };

        void ClearLog(int i) { slot[i].log->Clear(); };






        /* ------------------------------------------------------------------------------------------------------
         *  Unless your name is "main.cpp" by any chance, you don't ever need to call this
         * ------------------------------------------------------------------------------------------------------
         */
        void NotifyUpdate();
};

#endif // MESSAGEMANAGER_H

