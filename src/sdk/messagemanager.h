/*
* This file is part of Code::Blocks Studio, and licensed under the terms of the
* GNU General Public License version 2, or (at your option) any later version.
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "settings.h"
#include "manager.h"
#include <wx/hashmap.h>
#include <wx/menu.h>
#include <wx/dialog.h>

// convenience macros
// (you have to #include manager.h and messagemanager.h to use these)
/// Shortcut for Manager::Get()->GetMessageManager()
#define LOGGER      Manager::Get()->GetMessageManager()
/// Shortcut for Manager::Get()->GetMessageManager()->Log
#define LOG         LOGGER->Log
/// Shortcut for Manager::Get()->GetMessageManager()->DebugLog
#define DBGLOG      LOGGER->DebugLog
/// Shortcut for Manager::Get()->GetMessageManager()->LogError
#define LOG_ERROR   LOGGER->LogError
/// Shortcut for Manager::Get()->GetMessageManager()->LogWarning
#define LOG_WARN    LOGGER->LogWarning
/// Shortcut for *(Manager::Get()->GetMessageManager()). Use for streaming...
#define LOGSTREAM   *LOGGER
/// Shortcut for Manager::Get()->GetMessageManager()->LogPage(). Use for streaming...
#define LOGPAGE(a)  LOGGER->LogPage(a)

class MessageLog;

// forward decls
class wxMenuBar;
class wxBitmap;
class wxFlatNotebook;
class wxFlatNotebookEvent;

/**
 * @brief This is the main logging manager in Code::Blocks.
 *
 * Whenever you want to log something, this is where you want to get at.
 * It contains functions to:
 * @li add/remove logs,
 * @li show/hide logs,
 * @li clear logs,
 * @li append to logs,
 * @li log warnings,
 * @li log errors,
 * @li and more...
 *
 * @par
 * There are also a few macros defined, to ease access to this class.
 * Macros sample usage:
 *
 * @li LOG(m_PageIndex, "This is a test %s", "hi!")
 * @li DBGLOG("This is a test %s", "hi!")
 * @li LOG_ERROR("Oh no!")
 * @li LOGGER->SwitchTo(m_PageIndex)
 * @li LOGSTREAM << "Logged to standard log (debug)\n"
 * @li LOGSTREAM << LOGPAGE(myPageId) << "Logged to log with id @c myPageId\n"
 */
class DLLIMPORT MessageManager : public Mgr<MessageManager>, public wxEvtHandler
{
        struct LogStruct
        {
            LogStruct() : log(0), visible(true) {}
            MessageLog* log;
            bool visible;
            wxString title;
            wxBitmap bitmap;
        };
        WX_DECLARE_HASH_MAP(int, LogStruct*, wxIntegerHash, wxIntegerEqual, LogsMap);
	public:
        friend class Mgr<MessageManager>;
        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);

        /** @return the wxFlatNotebook control that MessageManager controls. */
		wxFlatNotebook* GetNotebook() { return m_pNotebook; }

        /** @brief Add a new log window.
          * @param log The log window to add.
          * @param title The log's title.
          * @param bitmap The optional bitmap to use.
          * @return An identifier for this log window, to be used when calling other MessageManager functions.
          */
        int AddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap = wxNullBitmap);
        /** @brief Remove a log window.
          * @param log The window to remove.
          */
        void RemoveLog(MessageLog* log);
        /** @brief Remove a log window.
          * @param id The ID of the window to remove.
          */
        void RemoveLog(int id);
        /** @brief Show/hide a log window.
          *
          * Hiding it doesn't remove it from MessageManager.
          * @param log The window to show/hide.
          * @param show If true, show the window else hide it.
          */
        void ShowLog(MessageLog* log, bool show = true);
        /** @brief Show/hide a log window.
          *
          * Hiding it doesn't remove it from MessageManager.
          * @param id The ID of the window to show/hide.
          * @param show If true, show the window else hide it.
          */
        void ShowLog(int id, bool show = true);
        /** @brief Clear a log.
          * @param id The log's id.
          */
        void ClearLog(int id);

        /** @brief Asks all logs to reset their fonts. */
        void ResetLogFont();

        /** @brief Check for emitted warnings.
          * @return True if at least one warning was printed, false if not.
          */
        bool HasWarnings();
        /** @brief Check for emitted errors.
          * @return True if at least one error was printed, false if not.
          */
        bool HasErrors();

        /** @brief Set the batch build log.
          * @param id The log's id.
          */
        void SetBatchBuildLog(int id);
        /** @brief Get the batch build dialog.
          * @return The batch build dialog.
          */
        wxDialog* GetBatchBuildDialog();

        /** @brief Log to stdout.
          * @param msg The message.
          */
		void LogToStdOut(const wxString& msg);
        /** @brief Log to stdout (varargs version).
          * @param msg The message format.
          */
		void LogToStdOut(const wxChar* msg, ...);
		/** @brief Log to the main log.
		  * @param msg The message.
		  */
		void Log(const wxString& msg);
		/** @brief Log to the main log (varargs version).
		  * @param msg The message.
		  */
		void Log(const wxChar* msg, ...);
		/** @brief Log to the debug log.
		  * @param msg The message.
		  */
		void DebugLog(const wxChar* msg, ...);
		/** @brief Log to the log with ID @c id.
		  * @param id The log's id.
		  * @param msg The message.
		  */
		void Log(int id, const wxString& msg);
		/** @brief Log to the log with ID @c id (varargs version).
		  * @param id The log's id.
		  * @param msg The message.
		  */
		void Log(int id, const wxChar* msg, ...);
		/** @brief Append to the main log (no LF is automatically added).
		  * @param msg The message.
		  */
		void AppendLog(const wxChar* msg, ...);
		/** @brief Append to the log with ID @c id (no LF is automatically added).
		  * @param id The log's id.
		  * @param msg The message.
		  */
		void AppendLog(int id, const wxChar* msg, ...);
		/** @brief Log a warning.
		  *
		  * This is logged in the main log, as well as the debug log
		  * and is colored blue to stand out.
		  * @param msg The message.
		  */
		void LogWarning(const wxChar* msg, ...);
		/** @brief Log an error.
		  * This is logged in the main log, as well as the debug log
		  * and is colored red to stand out.
		  * @param msg The message.
		  */
		void LogError(const wxChar* msg, ...);
		/** @brief Switch to log with ID @c id.
		  * @param id The log's id.
		  */
		void SwitchTo(int id);
		/** @brief Set the image of the log with ID @c id.
		  * @param id The log's id.
		  * @param bitmap The bitmap image.
		  */
		void SetLogImage(int id, const wxBitmap& bitmap);
		/** @brief Set the image of the @c log.
		  * @param log The log.
		  * @param bitmap The bitmap image.
		  */
		void SetLogImage(MessageLog* log, const wxBitmap& bitmap);

        /** @brief Enable/disable auto-hiding.
          * @param enable If true enable auto-hiding, else disable it.
          */
        void EnableAutoHide(bool enable = true);
        /** @brief Is auto-hiding enabled? */
        bool IsAutoHiding();
        /** @brief Open message manager. */
        void Open();
        /** @brief Close message manager.
          * Does nothing if locked and not forced.
          * @param force If true, force closing.
          * @see LockOpen(), Unlock()
          */
        void Close(bool force = false);
        /** @brief Open message manager and lock it open until unlocked.
          * Locking is cumulative, i.e. as many times it's locked, the same number
          * of times it must be unlocked.
          * @see Unlock()
          */
        void LockOpen();
        /** @brief Unlock the previously locked message manager.
          * Locking is cumulative, i.e. as many times it's locked, the same number
          * of times it must be unlocked.
          * @param force If true, the message manager is unlocked immediately (don't use this).
          * @see LockOpen()
          */
        void Unlock(bool force = false);

		/** @brief Streaming operator.
		  * By default, streams to the debug log. This can be changed by
		  * using LogPage(), e.g:
		  *     *LOGGER << LogPage(pageIndex) << "Some message\n";
		  */
		template<typename T> MessageManager& operator<<(const T& val)/* NOTE (thomas#9#): This template is inherently unsafe for Unicode */
		{
            wxString tmp;
            tmp << val;
            AppendLog(m_DebugLog, tmp);
            return *this;
		}
    private:
		MessageManager();
		~MessageManager();
        int DoAddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap = wxNullBitmap);
        bool CheckLogId(int id);

        void OnTabPosition(wxCommandEvent& event);
        void OnAppDoneStartup(wxCommandEvent& event);
        void OnAppStartShutdown(wxCommandEvent& event);
        void OnShowHideLog(wxCommandEvent& event);
        void OnPageChanged(wxFlatNotebookEvent& event);
        void OnPageContextMenu(wxFlatNotebookEvent& event);

        wxFlatNotebook* m_pNotebook;
        LogsMap m_Logs;
		int m_AppLog;
		int m_DebugLog;
		int m_BatchBuildLog;
		wxDialog* m_BatchBuildLogDialog;
		short int m_LockCounter;
		bool m_AutoHide; // auto-hide?
		bool m_HasWarnings;
		bool m_HasErrors;

		DECLARE_EVENT_TABLE();
};

#endif // MESSAGEMANAGER_H

