#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "settings.h"
#include "messagelog.h"
#include <wx/hashmap.h>
#include <wx/notebook.h>
#include "sanitycheck.h"

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
class MessageManager : public wxNotebook
{
	public:
        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
        int AddLog(MessageLog* log);
		void Log(const wxChar* msg, ...);
		void DebugLog(const wxChar* msg, ...);
		void Log(int id, const wxChar* msg, ...);
		void AppendLog(const wxChar* msg, ...);
		void AppendLog(int id, const wxChar* msg, ...);
		void SwitchTo(MessageLogType type);
		void SwitchTo(int id);
		void SetLogImage(int id, const wxBitmap& bitmap);
		void SetLogImage(MessageLog* log, const wxBitmap& bitmap);
    private:
		static MessageManager* Get(wxWindow* parent);
		static void Free();
		MessageManager(wxWindow* parent);
		~MessageManager();
        int DoAddLog(MessageLogType type, MessageLog* log);
		bool CheckLogType(MessageLogType type);
        void DoSwitchTo(MessageLog* ml);
        LogsMap m_Logs;
        LogIDsMap m_LogIDs;
		bool m_HasDebugLog;
		DECLARE_SANITY_CHECK
};

#endif // MESSAGEMANAGER_H

