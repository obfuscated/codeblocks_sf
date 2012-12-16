#include "directorymonitor.h"
#include <vector>
#include "se_globals.h"

#include <iostream>

DEFINE_EVENT_TYPE(wxEVT_MONITOR_NOTIFY)
DEFINE_EVENT_TYPE(wxEVT_MONITOR_NOTIFY2)

wxDirectoryMonitorEvent::wxDirectoryMonitorEvent(const wxString &mon_dir, int event_type, const wxString &uri): wxNotifyEvent(wxEVT_MONITOR_NOTIFY)
{
    m_mon_dir=mon_dir;
    m_event_type=event_type;
    m_info_uri=wxString(uri.c_str());
}
wxDirectoryMonitorEvent::wxDirectoryMonitorEvent(const wxDirectoryMonitorEvent& c) : wxNotifyEvent(c)
{
    m_mon_dir=wxString(c.m_mon_dir.c_str());
    m_event_type=c.m_event_type;
    m_info_uri=wxString(c.m_info_uri.c_str());
}

#ifdef __WXGTK__

#include <map>

#include <fam.h> //USES EITHER GAMIN OR FAM (IDENTICAL FILE MONITORING APIS)
#include <sys/time.h>

typedef std::map<FAMRequest, wxString> MonMap;

struct MonDescriptors
{
    MonDescriptors(int event_pipe)
    {
        FAMOpen(&fc);
        read_pipe=event_pipe;
    }
    ~MonDescriptors()
    {
        FAMClose(&fc);
    }
    int pipe_set()
    {
        return FD_ISSET(read_pipe,&readset);
    }
    int fam_set()
    {
        return FD_ISSET(famfd(),&readset);
    }
    int nfds()
    {
        return famfd()>read_pipe?famfd()+1:read_pipe+1;
    }
    int famfd()
    {
        return FAMCONNECTION_GETFD(&fc);
    }
    FAMConnection *fam()
    {
        return &fc;
    }
    int do_select()
    {
        int result;
        do {
           FD_ZERO(&readset);
           FD_SET(famfd(), &readset);
           FD_SET(read_pipe, &readset);
           result = select(nfds(), &readset, NULL, NULL, NULL);
        } while (false);//result == -1 && errno == EINTR);
        if(result>0)
        {
            if (FD_ISSET(famfd(), &readset))
                return 1;
            if (FD_ISSET(read_pipe, &readset))
                return 2;
        }
        return -1;
    }
    FAMConnection fc;
    int read_pipe;
    fd_set readset;
};


class DirMonitorThread : public wxThread
{
public:
    DirMonitorThread(wxEvtHandler *parent, wxArrayString pathnames, bool singleshot, bool subtree, int notifyfilter, int waittime_ms)
        : wxThread(wxTHREAD_JOINABLE)
    {
        m_active=false;
        m_parent=parent;
        m_waittime=waittime_ms;
        m_subtree=subtree;
        m_singleshot=singleshot;
        for(unsigned int i=0;i<pathnames.GetCount();i++)
            m_pathnames.Add(pathnames[i].c_str());
        m_notifyfilter=notifyfilter;
        int pipehandles[2];
        pipe(pipehandles);
        m_msg_rcv=pipehandles[0];
        m_msg_send=pipehandles[1];
        return;
    }
    void UpdatePathsThread(MonDescriptors &fd)
    {
        std::vector<FAMRequest *> new_h(m_update_paths.GetCount(),NULL); //TODO: initialize vector size
        for(size_t i=0;i<m_pathnames.GetCount();i++) //delete monitors that aren't needed
        {
            int index=m_update_paths.Index(m_pathnames[i]);
            if(index==wxNOT_FOUND)
            {
                if(m_h[i])
                {
                    FAMCancelMonitor(fd.fam(),m_h[i]);
                    //m_active_count--;
                    delete m_h[i];
                }
            }
        }
        for(size_t i=0;i<m_update_paths.GetCount();i++) // copy existing monitors and add new ones
        {
            int index=m_pathnames.Index(m_update_paths[i]);
            if(index!=wxNOT_FOUND)
            {
                new_h[i]=m_h[index];
            }
            else
            {
                FAMRequest *fr=new FAMRequest;
                if(FAMMonitorDirectory(fd.fam(),m_update_paths[i].mb_str(wxConvLocal),fr,new wxString(m_update_paths[i].c_str()))>=0)
                {
                    new_h[i]=fr;
                    m_active_count++;
                } else
                    delete fr;
            }
        }
        m_h=new_h; //replace the old with the new
        m_pathnames=m_update_paths;
    }
    void *Entry()
    {
        MonDescriptors fd(m_msg_rcv);

        m_interrupt_mutex.Lock();
        m_thread_notify=false;
        //TODO: Add a timer for killing singleshot instances
        m_active=true;
        m_interrupt_mutex.Unlock();

        UpdatePathsThread(fd);

        bool quit=false;
        m_active_count=0;
//        while(!(quit && m_active_count==0))
        // (hopefully) temporary fix for broken gamin and/or kernel
        // auto-update does not work anyway, but without this, C::B
        // hangs on close
        while(!quit)
        {
            const int result1 = fd.do_select();
            if(result1 < 0)
                break; //todo: error handling
            if(fd.fam_set())
            {
                while(FAMPending(fd.fam()))
                {
                    FAMEvent fe;
                    int result=FAMNextEvent(fd.fam(), &fe);
                    if(result>0)
                    {
                        wxString target_path(fe.filename, wxConvLocal);
                        //wxString target_path=wxString::FromUTF8(fe.filename);
                        int action=0;
                        switch(fe.code)
                        {
                            //TODO: Not handling FAMStartExecuting, FAMStopExecuting, FAMAcknowledge
                            case FAMChanged:
                                action=MONITOR_FILE_CHANGED;
                                break;
                            case FAMDeleted:
                            case FAMMoved:
                                action=MONITOR_FILE_DELETED;
                                break;
                            case FAMCreated:
                                action=MONITOR_FILE_CREATED;
                                break;
                            case FAMEndExist:
                            case FAMExists:
                                break;
                            case FAMAcknowledge:
                                if(fe.userdata)
                                {
                                    delete (wxString*)fe.userdata;
                                }
                                m_active_count--;
                                break;
                            case FAMStartExecuting:
                            case FAMStopExecuting:
                            default:
                                break;
//                            case ?????:
//                                action=MONITOR_FILE_ATTRIBUTES;
//                                break;
                        }
                        if(action&m_notifyfilter)
                        {
                            wxDirectoryMonitorEvent e(((wxString *)(fe.userdata))->c_str(),action,target_path.c_str());
                            m_parent->AddPendingEvent(e);
                        }

                    }
                }
            }
            if(fd.pipe_set())
            {
                char c = 0;
                read(m_msg_rcv, &c, 1);
                switch(c)
                {
                    case 'm':
                        UpdatePathsThread(fd);
                        break;
                    case 'q':
                        quit=true;
                        m_interrupt_mutex.Lock();
                        m_active=false;
                        m_update_paths.Empty();
                        m_interrupt_mutex.Unlock();
                        UpdatePathsThread(fd);
                        break;
                    default:
                        break;
                }
            }
        }
        return NULL;
    }

    virtual ~DirMonitorThread()
    {
        m_interrupt_mutex.Lock();
        m_active=false;
        char m='q';
        write(m_msg_send,&m,1);
        m_interrupt_mutex.Unlock();
        if(IsRunning())
            Wait();//Delete();
        close(m_msg_rcv);
        close(m_msg_send);
    }

    void UpdatePaths(const wxArrayString &paths)
    {
        m_interrupt_mutex.Lock();
        if(!m_active)
        {
            m_interrupt_mutex.Unlock();
            return;
        }
        m_update_paths.Empty();
        for(unsigned int i=0;i<paths.GetCount();i++)
            m_update_paths.Add(paths[i].c_str());
        char m='m';
        write(m_msg_send,&m,1);
        m_interrupt_mutex.Unlock();

    }
    int m_active_count;
    int m_msg_rcv;
    int m_msg_send;
    bool m_thread_notify;
    bool m_active;
    wxMutex m_interrupt_mutex;
    int m_waittime;
    bool m_subtree;
    bool m_singleshot;
    wxArrayString m_pathnames, m_update_paths;
    int m_notifyfilter;
    MonMap m_monmap;
    std::vector<FAMRequest *> m_h;
    wxEvtHandler *m_parent;
};


#endif
#ifdef __WXMSW__

#define DEFAULT_MONITOR_FILTER_WIN32 FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_LAST_ACCESS|FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_SECURITY


#include <map>

// Structure contains all of the data required to monitor a single directory
struct MonData
{
    OVERLAPPED m_overlapped;
    wxString m_path;
    HANDLE m_handle;
    PFILE_NOTIFY_INFORMATION m_changedata;
    DirMonitorThread *m_monitor;
    bool m_fail;
    bool m_cancel;
    MonData();
    MonData(DirMonitorThread *monitor, const wxString &path, bool subtree);
    void ReadCancel();
    void ReadRequest(bool subtree);
    ~MonData();
    static OVERLAPPED new_overlapped();
};


// watched directories are maintained as a map by pathname
typedef std::map<wxString,MonData*> MonMap;


//WIN32 ONLY THREADED CLASS TO HANDLE WAITING ON DIR CHANGES ASYNCHRONOUSLY
class DirMonitorThread : public wxThread
{
public:
    DirMonitorThread(wxEvtHandler *parent, wxArrayString pathnames, bool singleshot, bool subtree, DWORD notifyfilter, DWORD waittime_ms)
        : wxThread(wxTHREAD_JOINABLE)
    {
        m_interrupt_event[0]=CreateEvent(NULL, FALSE, FALSE, NULL); //used to signal update path request
        m_interrupt_event[1]=CreateEvent(NULL, FALSE, FALSE, NULL); //used to signal quit request

        m_parent=parent;
        m_waittime=waittime_ms;
        m_subtree=subtree;
        m_singleshot=singleshot;
        for(unsigned int i=0;i<pathnames.GetCount();i++)
            m_update_paths.Add(pathnames[i].c_str());
        m_notifyfilter=notifyfilter;
        return;

    }
    void WaitKill()
    {
        SetEvent(m_interrupt_event[1]);
    }
    void UpdatePaths(const wxArrayString &paths)
    {
        m_interrupt_mutex2.Lock();
        m_update_paths.Empty();
        for(unsigned int i=0;i<paths.GetCount();i++)
            m_update_paths.Add(paths[i].c_str());
        m_update_paths=paths;
        SetEvent(m_interrupt_event[0]);
        m_interrupt_mutex2.Unlock();
    }
    bool UpdatePathsThread()
    {
        m_interrupt_mutex2.Lock();
        wxArrayString update_paths;
        for(unsigned int i=0;i<m_update_paths.GetCount();i++)
            update_paths.Add(m_update_paths[i].c_str());

        for(MonMap::iterator it=m_monmap.begin();it!=m_monmap.end();++it)
        {
            int index=update_paths.Index(it->first);
            if(index==wxNOT_FOUND)
            {
                it->second->ReadCancel(); //request cancel. will be removed from the map
                if(it->second->m_fail)
                {
                    delete it->second;
                    m_monmap.erase(it);
                }
            }
        }
        for(size_t i=0;i<update_paths.GetCount();i++)
        {
            MonMap::iterator it=m_monmap.find(update_paths[i]);
            if(it==m_monmap.end())
            {
                MonData *md=new MonData(this,update_paths[i],m_subtree);
                if(md->m_fail)
                    delete md;
                else
                    m_monmap[update_paths[i]]=md;
            }
        }
        m_interrupt_mutex2.Unlock();
        return true;
    }

    void *Entry()
    {
        UpdatePathsThread();
        bool kill_request=false;
        //TODO: Error checking
        while(!(kill_request && m_monmap.size()==0)) //don't exit until we have gracefully closed all of the directory monitors
        {
            DWORD result=::WaitForMultipleObjectsEx(2,m_interrupt_event,false,INFINITE,true);
            if(result==WAIT_FAILED || result==WAIT_ABANDONED_0)
                break; //most likely will cause crash if this happens
            if(result==WAIT_OBJECT_0+1 && !kill_request)
            {
                kill_request=true;
                for(MonMap::iterator it=m_monmap.begin();it!=m_monmap.end();++it)
                {
                    it->second->ReadCancel();
                    if(it->second->m_fail)
                    {
                        delete it->second;
                        m_monmap.erase(it);
                    }
                }
            }
            if(result==WAIT_OBJECT_0)
            {
                m_interrupt_mutex2.Lock();
                UpdatePathsThread();
                ResetEvent(m_interrupt_event[0]);
                m_interrupt_mutex2.Unlock();
            }
            if(m_singleshot)
                break;
        }
        //wxMessageBox(_("quitting monitor"));
//        wxDirectoryMonitorEvent e(wxEmptyString,MONITOR_FINISHED,wxEmptyString);
//        m_parent->AddPendingEvent(e);
        return NULL;
    }
    virtual ~DirMonitorThread()
    {
        if(IsRunning())
        {
            WaitKill();
            Wait();//Delete();
        }
        CloseHandle(m_interrupt_event[0]);
        CloseHandle(m_interrupt_event[1]);
    }
    void ReadChanges(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, MonData *mondata)
    {
//        wxMessageBox(wxString::Format(_("%i"),dwErrorCode));
        //TODO: Error checking - dwErrorCode should be zero, then need to check other error conditions
        int off=0;
        unsigned int i=0;
//        if(mondata->m_cancel && dwNumberOfBytesTransfered>0)
//            wxMessageBox(wxString::Format(_("message, code %i, bytes %i, path %s"),dwErrorCode, dwNumberOfBytesTransfered,mondata->m_path.c_str()));
        if(dwNumberOfBytesTransfered==0 || dwErrorCode>0) //mondata->m_cancel ||
        {
            MonMap::iterator it=m_monmap.find(mondata->m_path);
            if(it!=m_monmap.end())
            {
                delete it->second;
                m_monmap.erase(it);
            }
            return;
        }
        PFILE_NOTIFY_INFORMATION chptr=&mondata->m_changedata[i];
        if(dwNumberOfBytesTransfered>0)
        do
        {
            DWORD a=chptr->Action;
            //TODO: Convert to the MONITOR_FILE_XXX action types, filtering those that aren't wanted
            int action=0;
            switch(a)
            {
                case FILE_ACTION_ADDED:
                case FILE_ACTION_RENAMED_NEW_NAME:
                    action=MONITOR_FILE_CREATED;
                    break;
                case FILE_ACTION_REMOVED:
                case FILE_ACTION_RENAMED_OLD_NAME:
                    action=MONITOR_FILE_DELETED;
                    break;
                case FILE_ACTION_MODIFIED:
                    action=MONITOR_FILE_CHANGED;
                    break;
                default:
                  break;
            }
            if(action&m_notifyfilter)
            {
                wxString filename(chptr->FileName,chptr->FileNameLength/2); //TODO: check the div by 2
                wxDirectoryMonitorEvent e(mondata->m_path,action,filename);
                m_parent->AddPendingEvent(e);
            }
            off=chptr->NextEntryOffset;
            chptr=(PFILE_NOTIFY_INFORMATION)((char*)chptr+off);
        } while(off>0);
        else
        {
            //too many changes, tell parent to manually read the directory
            wxDirectoryMonitorEvent e(mondata->m_path,MONITOR_TOO_MANY_CHANGES,wxEmptyString);
            m_parent->AddPendingEvent(e);

        }
        mondata->ReadRequest(m_subtree);
    }
    static VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
        MonData *mondata=(MonData*)lpOverlapped;
        mondata->m_monitor->ReadChanges(dwErrorCode, dwNumberOfBytesTransfered, mondata);
    }

    HANDLE m_interrupt_event[2];
    wxMutex m_interrupt_mutex2;
    DWORD m_waittime;
    bool m_subtree;
    bool m_singleshot;
    MonMap m_monmap;
    wxArrayString m_update_paths;
    DWORD m_notifyfilter;
    wxEvtHandler *m_parent;
};


//MonData implementations
MonData::MonData()
{
    m_path=_("");
    m_monitor=NULL;
    m_changedata=NULL;
    m_handle=NULL;
    m_fail=false;
    m_cancel=false;
}

MonData::MonData(DirMonitorThread *monitor, const wxString &path, bool subtree)
{
    MonData();
    m_monitor=monitor;
    m_path=path.c_str();
    m_handle=::CreateFile(path.c_str(),FILE_LIST_DIRECTORY,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
    if(m_handle!=INVALID_HANDLE_VALUE)
    {
        m_overlapped=new_overlapped();
        m_changedata=(PFILE_NOTIFY_INFORMATION)(new char[4096]);
        ReadRequest(subtree);
    } else
    {
        wxMessageBox(_("WARNING: Failed to open handle for ")+m_path);
        m_handle=NULL;
        m_fail=true;
    }
}

void MonData::ReadCancel()
{
    if(!m_fail && m_handle!=NULL)
    {
        if(!::CancelIo(m_handle))
        {
            wxMessageBox(_("WARNING: Failed to initiate cancel io for ")+m_path);
            m_fail=true;
        }
        else
        {
            m_cancel=true;
        }
    }
}

void MonData::ReadRequest(bool subtree)
{
    if(!::ReadDirectoryChangesW(m_handle, m_changedata, 4096, subtree, DEFAULT_MONITOR_FILTER_WIN32, NULL, &m_overlapped, m_monitor->FileIOCompletionRoutine))
    {
        m_fail=true;
//        wxMessageBox(_("WARNING: Failed to initiate read request for ")+m_path);
    }
    else
    {
        m_fail=false;
    }
}

MonData::~MonData()
{
    if(m_handle)
    {
        if(!::CloseHandle(m_handle))
        {
//            wxMessageBox(_("WARNING: Failed to close monitor handle for ")+m_path);
        }
    }
    if(m_changedata)
        delete m_changedata;
}

OVERLAPPED MonData::new_overlapped()
{
    OVERLAPPED o;
    o.Internal=0;
    o.InternalHigh=0;
    o.Offset=0;
    o.OffsetHigh=0;
    o.hEvent=NULL;
    return o;
}


#endif

BEGIN_EVENT_TABLE(wxDirectoryMonitor, wxEvtHandler)
    EVT_MONITOR_NOTIFY(0, wxDirectoryMonitor::OnMonitorEvent)
//    EVT_COMMAND(0, wxEVT_MONITOR_NOTIFY2, FileExplorer::OnMonitorEvent2)
END_EVENT_TABLE()

void wxDirectoryMonitor::OnMonitorEvent(wxDirectoryMonitorEvent &e)
{
    if(m_parent)
        m_parent->AddPendingEvent(e);
}

wxDirectoryMonitor::wxDirectoryMonitor(wxEvtHandler *parent, const wxArrayString &uri, int eventfilter)
{
    //TODO: put init and shutdown in static members
    m_parent=parent;
    m_uri=uri;
    m_eventfilter=eventfilter;
}

bool wxDirectoryMonitor::Start()
{
    m_monitorthread=new DirMonitorThread(this, m_uri, false, false, m_eventfilter, 100);
    m_monitorthread->Create();
    m_monitorthread->Run();
    return true;
}

void wxDirectoryMonitor::ChangePaths(const wxArrayString &uri)
{
    m_uri=uri;
    m_monitorthread->UpdatePaths(uri);
}

wxDirectoryMonitor::~wxDirectoryMonitor()
{
    delete m_monitorthread;
}
