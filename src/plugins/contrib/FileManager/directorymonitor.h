#ifndef WXFILESYSTEMMONITOR_H
#define WXFILESYSTEMMONITOR_H



#include <wx/wx.h>

#define MONITOR_TERMINATE 0x010000
#define MONITOR_TOO_MANY_CHANGES 0x020000
#define MONITOR_ERROR 0x040000
#define MONITOR_FILE_CHANGED 0x001
#define MONITOR_FILE_DELETED 0x002
#define MONITOR_FILE_CREATED 0x004
//TODO: Decide if it is worth having these
#define MONITOR_FILE_ATTRIBUTES 0x080
#define MONITOR_FILE_STARTEXEC 0x010
#define MONITOR_FILE_STOPEXEC 0x020

#define DEFAULT_MONITOR_FILTER MONITOR_FILE_CHANGED|MONITOR_FILE_DELETED|MONITOR_FILE_CREATED|MONITOR_FILE_ATTRIBUTES

class DirMonitorThread;

class wxDirectoryMonitor;

///////////////////////////////////////
// EVENT CODE /////////////////////////
///////////////////////////////////////

/*
Defines a wxDirectoryMonitorEvent with public member naming the path
monitored, the file or directory creating the event and the code for
the event. Also used to send Termination events (on win32)

Also defines event table macro EVT_MONITOR_NOTIFY to notify the
caller of change events
*/

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE(wxEVT_MONITOR_NOTIFY, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_MONITOR_NOTIFY2, -1)
END_DECLARE_EVENT_TYPES()

class wxDirectoryMonitorEvent: public wxNotifyEvent
{
public:
    wxDirectoryMonitorEvent(const wxString &mon_dir, int event_type, const wxString &uri);
    wxDirectoryMonitorEvent(const wxDirectoryMonitorEvent& c);
    wxEvent *Clone() const { return new wxDirectoryMonitorEvent(*this); }
    ~wxDirectoryMonitorEvent() {}
    wxString m_mon_dir;
    int m_event_type;
    wxString m_info_uri;
};

typedef void (wxEvtHandler::*wxDirectoryMonitorEventFunction)(wxDirectoryMonitorEvent&);

#define EVT_MONITOR_NOTIFY(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_MONITOR_NOTIFY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxDirectoryMonitorEventFunction, & fn ), (wxObject *) NULL ),

///////////////////////////////////////
// DIRECTORY MONITOR CLASS ////////////
///////////////////////////////////////

class wxDirectoryMonitor: public wxEvtHandler
{
public:
    wxDirectoryMonitor(wxEvtHandler *parent, const wxArrayString &uri, int eventfilter=DEFAULT_MONITOR_FILTER);
    virtual ~wxDirectoryMonitor();
    bool Start();
    void ChangePaths(const wxArrayString &uri);
    void OnMonitorEvent(wxDirectoryMonitorEvent &e);
    void OnMonitorEvent2(wxCommandEvent &e);
private:
    wxArrayString m_uri;
    wxEvtHandler *m_parent;
    int m_eventfilter;
    DirMonitorThread *m_monitorthread;
    DECLARE_EVENT_TABLE()
};

#endif // WXFILESYSTEMMONITOR_H
