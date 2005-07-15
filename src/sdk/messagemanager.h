#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "settings.h"
#include "messagelog.h"
#include <wx/hashmap.h>
#include <wx/notebook.h>
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
// LOGSTREAM << LOGPAGE(m_PageIndex) << "Logged to my log\n"
// LOGSTREAM << LOGPAGE(mltDebug) << "Logged to debug log\n"

enum MessageLogType
{
    mltDebug = 0,
	mltLog,
	mltOther
};

WX_DECLARE_HASH_MAP(MessageLogType, MessageLog*, wxIntegerHash, wxIntegerEqual, LogsMap);
WX_DECLARE_HASH_MAP(int, MessageLog*, wxIntegerHash, wxIntegerEqual, LogIDsMap);

// forward decls
class wxMenuBar;

/*
 * No description
 */
class DLLIMPORT MessageManager : public wxNotebook
{
	public:
        typedef short int LockToken;

        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		
		wxWindow* GetContainerWindow(){ return m_pContainerWin; }
		void SetContainerWindow(wxWindow* win){ m_pContainerWin = win; }

        int AddLog(MessageLog* log);
		void Log(const wxChar* msg, ...);
		void DebugLog(const wxChar* msg, ...);
		void DebugLogWarning(const wxChar* msg, ...);
		void DebugLogError(const wxChar* msg, ...);
		void Log(int id, const wxChar* msg, ...);
		void AppendLog(const wxChar* msg, ...);
		void AppendLog(int id, const wxChar* msg, ...);
		void SwitchTo(MessageLogType type);
		void SwitchTo(int id);
		void SetLogImage(int id, const wxBitmap& bitmap);
		void SetLogImage(MessageLog* log, const wxBitmap& bitmap);

        /** @brief Enable/disable auto-hiding. */
        void EnableAutoHide(bool enable = true);
        /** @brief Is auto-hiding enabled? */
        bool IsAutoHiding();
        int GetOpenSize();
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

		/** @brief Set the active log target for streaming */
		MessageLogType LogPage(MessageLogType lt);
		/** @brief Set the active log target for streaming */
		MessageLogType LogPage(int pageIndex);
		/** @brief Streaming operator.
		  * By default, streams to the debug log. This can be changed by
		  * using LogPage(), e.g:
		  *     *LOGGER << LogPage(pageIndex) << "Some message\n";
		  */
		template<typename T> MessageManager& operator<<(const T& val)
		{
            wxString tmp;
            tmp << val;
            AppendLog(m_OtherPageLogTarget, tmp);
            return *this;
		}        /** @brief Special streaming operator for target log */
		MessageManager& operator<<(const MessageLogType& val)
		{
            // "eat" input. the actual job is carried out in LogPage()
            return *this;
		}
    private:
		static MessageManager* Get(wxWindow* parent);
		static void Free();
		MessageManager(wxWindow* parent);
		~MessageManager();
        int DoAddLog(MessageLogType type, MessageLog* log);
		bool CheckLogType(MessageLogType type);
        void DoSwitchTo(MessageLog* ml);
        void OnSelChange(wxNotebookEvent& event);

        LogsMap m_Logs;
        LogIDsMap m_LogIDs;
		bool m_HasDebugLog;
		int m_OtherPageLogTarget; // used by the streaming operators
		short int m_LockCounter;
		int m_OpenSize; // the size when open
		bool m_AutoHide; // auto-hide?
		bool m_Open; // is open?
		wxWindow* m_pContainerWin;
		DECLARE_EVENT_TABLE();
		DECLARE_SANITY_CHECK
};

#endif // MESSAGEMANAGER_H

