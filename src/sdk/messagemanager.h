#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "settings.h"
#include "messagelog.h"
#include <wx/hashmap.h>
#include "sanitycheck.h"

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

WX_DECLARE_HASH_MAP(int, MessageLog*, wxIntegerHash, wxIntegerEqual, LogsMap);

// forward decls
class wxMenuBar;
class wxBitmap;
class wxFlatNotebook;
class wxFlatNotebookEvent;

/*
 * No description
 */
class DLLIMPORT MessageManager : public wxEvtHandler
{
	public:
        typedef short int LockToken;

        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);

		wxFlatNotebook* GetNotebook() { return m_pNotebook; }

        int AddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap = wxNullBitmap);
        void RemoveLog(MessageLog* log);

		void LogToStdOut(const wxChar* msg, ...);
		void Log(const wxChar* msg, ...);
		void DebugLog(const wxChar* msg, ...);
		void DebugLogWarning(const wxChar* msg, ...);
		void DebugLogError(const wxChar* msg, ...);
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
		}        /** @brief Special streaming operator for target log */
    private:
		static MessageManager* Get();
		static void Free();
		MessageManager();
		~MessageManager();
        int DoAddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap = wxNullBitmap);
        bool CheckLogId(int id);

        void OnAppDoneStartup(wxCommandEvent& event);
        void OnAppStartShutdown(wxCommandEvent& event);
        void OnPageChanged(wxFlatNotebookEvent& event);

        wxFlatNotebook* m_pNotebook;
        LogsMap m_Logs;
		int m_AppLog;
		int m_DebugLog;
		short int m_LockCounter;
		bool m_AutoHide; // auto-hide?

		DECLARE_EVENT_TABLE();
		DECLARE_SANITY_CHECK
};

#endif // MESSAGEMANAGER_H

