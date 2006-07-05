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
#include "messagelog.h"
#include <wx/hashmap.h>
#include <wx/menu.h>
#include <wx/dialog.h>

// convenience macros
// (you have to #include manager.h and messagemanager.h to use these)
#define LOGGER      Manager::Get()->GetMessageManager()
#define LOG         LOGGER->Log
#define DBGLOG      LOGGER->DebugLog
#define LOGSTREAM   *LOGGER
#define LOGPAGE(a)  LOGGER->LogPage(a)
// sample usage:
//
// LOG(m_PageIndex, "This is a test %s", "hi!")
// DBGLOG("This is a test %s", "hi!")
// LOGGER->SwitchTo(m_PageIndex)
// LOGSTREAM << "Logged to standard log (debug)\n"

struct LogStruct
{
    LogStruct() : log(0), visible(true) {}
    MessageLog* log;
    bool visible;
    wxString title;
    wxBitmap bitmap;
};
WX_DECLARE_HASH_MAP(int, LogStruct*, wxIntegerHash, wxIntegerEqual, LogsMap);

// forward decls
class wxMenuBar;
class wxBitmap;
class wxFlatNotebook;
class wxFlatNotebookEvent;

/*
 * No description
 */
class DLLIMPORT MessageManager : public Mgr<MessageManager>, public wxEvtHandler
{
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
        /** Remove a log window.
          * @param log The window to remove.
          */
        void RemoveLog(MessageLog* log);
        /** Remove a log window.
          * @param log The ID of the window to remove.
          */
        void RemoveLog(int id);
        /** Show/hide a log window.
          * Hiding it doesn't remove it from MessageManager.
          * @param log The window to show/hide.
          * @param show If true, show the window else hide it.
          */
        void ShowLog(MessageLog* log, bool show = true);
        /** Show/hide a log window.
          * Hiding it doesn't remove it from MessageManager.
          * @param log The ID of the window to show/hide.
          * @param show If true, show the window else hide it.
          */
        void ShowLog(int id, bool show = true);

        void ResetLogFont();

        void SetBatchBuildLog(int log);
        wxDialog* GetBatchBuildDialog();

		void LogToStdOut(const wxString& msg);
		void LogToStdOut(const wxChar* msg, ...);
		void Log(const wxString& msg);
		void Log(const wxChar* msg, ...);
		void DebugLog(const wxChar* msg, ...);
		void DebugLogWarning(const wxChar* msg, ...);
		void DebugLogError(const wxChar* msg, ...);
		void Log(int id, const wxString& msg);
		void Log(int id, const wxChar* msg, ...);
		void AppendLog(const wxChar* msg, ...);
		void AppendLog(int id, const wxChar* msg, ...);
		void SwitchTo(int id);
		void SetLogImage(int id, const wxBitmap& bitmap);
		void SetLogImage(MessageLog* log, const wxBitmap& bitmap);

        /** @brief Enable/disable auto-hiding. */
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

		DECLARE_EVENT_TABLE();
};

#endif // MESSAGEMANAGER_H

