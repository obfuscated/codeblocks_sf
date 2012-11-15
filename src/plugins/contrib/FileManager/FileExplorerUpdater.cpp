#include "FileExplorerUpdater.h"
#include "FileExplorer.h"
#include "se_globals.h"

DEFINE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_TREE)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_EXEC_REQUEST)

int ID_EXEC_TIMER=wxNewId();

BEGIN_EVENT_TABLE(FileExplorerUpdater, wxEvtHandler)
    EVT_TIMER(ID_EXEC_TIMER, FileExplorerUpdater::OnExecTimer)
    EVT_END_PROCESS(wxID_ANY, FileExplorerUpdater::OnExecTerminate)
END_EVENT_TABLE()

FileExplorerUpdater::~FileExplorerUpdater()
{
    if(m_exec_proc)
    {
        m_exec_timer->Stop();
        delete m_exec_timer;
        m_exec_proc->Detach();
        m_exec_cond->Signal();
        m_exec_mutex->Unlock();
    }
    if(IsRunning())
    {
        m_kill=true;
        Wait();
    }
}


void FileExplorerUpdater::Update(const wxTreeItemId &ti)
{
    wxString chkpath=m_fe->GetFullPath(ti);
    m_path=wxString(m_fe->GetFullPath(ti).c_str());
    m_wildcard=wxString(m_fe->m_WildCards->GetValue().c_str());
    GetTreeState(ti);
    if(Create()==wxTHREAD_NO_ERROR)
    {
        SetPriority(20);
        Run();
    }
}

void *FileExplorerUpdater::Entry()
{
    wxCommandEvent ne(wxEVT_NOTIFY_UPDATE_TREE,0);
    if(!GetCurrentState(m_path))
    {
        m_fe->m_updater_cancel=true; //TODO: SEND A CANCEL EVENT INSTEAD
        m_fe->AddPendingEvent(ne);
        return NULL;
    }
    if(!CalcChanges())
    {
        m_fe->m_updater_cancel=true;
        m_fe->AddPendingEvent(ne);
        return NULL;
    }
    m_fe->AddPendingEvent(ne);
    return NULL;
}

// Call from main thread prior to thread entry point
void FileExplorerUpdater::GetTreeState(const wxTreeItemId &ti)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId ch=m_fe->m_Tree->GetFirstChild(ti,cookie);
    m_treestate.clear();
    while(ch.IsOk())
    {
        FileData fd;
        fd.name=m_fe->m_Tree->GetItemText(ch);
        fd.state=m_fe->m_Tree->GetItemImage(ch);
        m_treestate.push_back(fd);
        ch=m_fe->m_Tree->GetNextChild(ti,cookie);
    }
}

// called from Thread::Entry
bool FileExplorerUpdater::GetCurrentState(const wxString &path)
{
    //wxString wildcard=m_fe->m_WildCards->GetValue();
//    m_Tree->DeleteChildren(ti); //Don't delete everything, we will add new files/dirs, remove deleted files/dirs and update state of all other files.

    m_currentstate.clear();
    wxDir dir(path);

    if (!dir.IsOpened())
    {
        // deal with the error here - wxDir would already log an error message
        // explaining the exact reason for the failure
        return false;
    }
    wxString filename;
    int flags=wxDIR_FILES|wxDIR_DIRS;
    if(m_fe->m_show_hidden)
        flags|=wxDIR_HIDDEN;
    VCSstatearray sa;
    bool is_vcs=false;
    bool is_cvs=false;

    // TODO: THIS NEEDS TO BE CALLED FROM MAIN THREAD BUT CAN'T BE UI-BLOCKING (CAN'T CALL wxExecute FROM THREADS)
    // TODO: IDEALLY THE VCS COMMAND LINE PROGRAM SHOULD BE CALLED ONCE ON THE BASE DIRECTORY (SINCE THEY ARE USUALLY RECURSIVE) TO AVOID REPEATED CALLS FOR SUB-DIRS
    if(m_fe->m_parse_svn)
        if(ParseSVNstate(path,sa))
            is_vcs=true;
    if(m_fe->m_parse_bzr && !is_vcs)
        if(ParseBZRstate(path,sa))
            is_vcs=true;
    if(m_fe->m_parse_hg && !is_vcs)
        if(ParseHGstate(path,sa))
            is_vcs=true;
/*    if(m_fe->m_parse_cvs && !is_vcs)
        if(ParseCVSstate(path,sa))
        {
            is_vcs=true;
            is_cvs=true;
        }*/

    bool cont = dir.GetFirst(&filename,wxEmptyString,flags);
    while ( cont && !TestDestroy() && !m_kill)
    {
        int itemstate=fvsNormal;
        bool match=true;
        wxString fullpath=wxFileName(path,filename).GetFullPath();
        if(wxFileName::DirExists(fullpath))
            itemstate=fvsFolder;
        if(wxFileName::FileExists(fullpath))
        {
            if(is_vcs&&!is_cvs)
                itemstate=fvsVcUpToDate;
            else
                itemstate=fvsNormal;
            wxFileName fn(path,filename);
#if wxCHECK_VERSION(2,8,0)
            if(!fn.IsFileWritable())
                itemstate=fvsReadOnly;
#endif
            int deli=-1;
            for(size_t i=0;i<sa.GetCount();i++)
            {
                if(fn.SameAs(sa[i].path))
                {
                    itemstate=sa[i].state;
                    deli=i;
                    break;
                }
            }
            if(deli>=0)
                sa.RemoveAt(deli);
            if(!WildCardListMatch(m_wildcard,filename,true))
                match=false;
        }
        if(match)
        {
            FileData fd;
            fd.name=filename;
            fd.state=itemstate;
            m_currentstate.push_back(fd);
        }
        cont = dir.GetNext(&filename);
    }
    return !TestDestroy();
}

bool FileExplorerUpdater::CalcChanges()
{
    m_adders.clear();
    m_removers.clear();
    FileDataVec::iterator tree_it=m_treestate.begin();
    while(tree_it!=m_treestate.end() && !TestDestroy())
    {
        bool match=false;
        for(FileDataVec::iterator it=m_currentstate.begin();it!=m_currentstate.end();it++)
            if(it->name==tree_it->name)
            {
                match=true;
                if(it->state!=tree_it->state)
                {
                    m_adders.push_back(*it);
                    m_removers.push_back(*tree_it);
                }
                m_currentstate.erase(it);
                tree_it=m_treestate.erase(tree_it);
                break;
            }
        if(!match)
            tree_it++;
    }
    for(FileDataVec::iterator tree_it2 = m_treestate.begin(); tree_it2 != m_treestate.end(); ++tree_it2)
        m_removers.push_back(*tree_it2);
    for(FileDataVec::iterator it=m_currentstate.begin();it!=m_currentstate.end();it++)
        m_adders.push_back(*it);
    return !TestDestroy();
}

int FileExplorerUpdater::Exec(const wxString &command, wxArrayString &output)
{
    int exitcode=0;
    m_exec_mutex=new wxMutex();
    m_exec_cond=new wxCondition(*m_exec_mutex);
    m_exec_cmd=command;
    m_exec_mutex->Lock();
    wxCommandEvent ne(wxEVT_NOTIFY_EXEC_REQUEST,0);
    m_fe->AddPendingEvent(ne);
    m_exec_cond->Wait();
    m_exec_mutex->Unlock();
    delete m_exec_cond;
    delete m_exec_mutex;
    if(m_exec_proc_id==0)
        exitcode=1;
    output=m_exec_output;
    return exitcode;
}

void FileExplorerUpdater::ExecMain()
{
    m_exec_output.Empty();
    m_exec_proc=new wxProcess(this);
    m_exec_proc->Redirect();
    m_exec_mutex->Lock();
    m_exec_proc_id=wxExecute(m_exec_cmd,wxEXEC_ASYNC,m_exec_proc);
    if(m_exec_proc_id==0)
    {
        m_exec_cond->Signal();
        m_exec_mutex->Unlock();
        return;
    }
    m_exec_timer=new wxTimer(this,ID_EXEC_TIMER);
    m_exec_timer->Start(100,true);
}

void FileExplorerUpdater::OnExecTerminate(wxProcessEvent &/*e*/)
{
    ReadStream(true);
    m_exec_timer->Stop();
    delete m_exec_timer;
    delete m_exec_proc;
    m_exec_proc=NULL;
    m_exec_cond->Signal();
    m_exec_mutex->Unlock();
}

void FileExplorerUpdater::OnExecTimer(wxTimerEvent &/*e*/)
{
    if(m_exec_proc)
        ReadStream();
}

void FileExplorerUpdater::ReadStream(bool all)
{
    m_exec_timer->Stop();
    m_exec_stream=m_exec_proc->GetInputStream();
    wxTextInputStream tis(*m_exec_stream);
    wxStopWatch sw;
    while(m_exec_proc->IsInputAvailable())
    {
        m_exec_output.Add(tis.ReadLine());
        if(!all && sw.Time()>30)
            break;
    }
    if(!all)
    {
        m_exec_timer->Start(150,true);
    }
}

bool FileExplorerUpdater::ParseSVNstate(const wxString &path, VCSstatearray &sa)
{
    if(!wxFileName::DirExists(wxFileName(path,_T(".svn")).GetFullPath()))
        return false;
    wxArrayString output;
    int hresult=Exec(_T("svn stat -N ")+path,output);
    if(hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if(output[i].Len()<=7)
            break;
        VCSstate s;
        wxChar a=output[i][0];
        switch(a)
        {
            case ' ':
                s.state=fvsVcUpToDate;
                break;
            case '?':
                s.state=fvsVcNonControlled;
                break;
            case 'A':
                s.state=fvsVcAdded;
                break;
            case 'M':
                s.state=fvsVcModified;
                break;
            case 'C':
                s.state=fvsVcConflict;
                break;
            case 'D':
                s.state=fvsVcMissing;
                break;
            case 'I':
                s.state=fvsVcNonControlled;
                break;
            case 'X':
                s.state=fvsVcExternal;
                break;
            case '!':
                s.state=fvsVcMissing;
                break;
            case '~':
                s.state=fvsVcLockStolen;
                break;
            default:
                break;
        }
#ifdef __WXMSW__
        wxFileName f(output[i].Mid(1).Strip(wxString::both));
        f.MakeAbsolute(path);
        s.path=f.GetFullPath();
#else
        s.path=wxFileName(output[i].Mid(1).Strip(wxString::both)).GetFullPath();
#endif
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseBZRstate(const wxString &path, VCSstatearray &sa)
{
    wxString parent=path;
    while(true)
    {
        if(wxFileName::DirExists(wxFileName(parent,_T(".bzr")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if(oldparent==parent||parent.IsEmpty())
            return false;
    }
    if(parent.IsEmpty())
        return false;
    wxArrayString output;
    wxString rpath=parent;
    #ifdef __WXMSW__
    int hresult=Exec(_T("cmd /c bzr stat --short ")+path,output);
    #else
    int hresult=Exec(_T("bzr stat --short ")+path,output);
    #endif
    if(hresult!=0)
    {
        return false;
    }
    for(size_t i=0;i<output.GetCount();i++)
    {
        if(output[i].Len()<=4)
            break;
        VCSstate s;
        wxChar a=output[i][0];
        switch(a)
        {
            case '+':
                s.state=fvsVcUpToDate;
                break;
            case '-':
                s.state=fvsVcNonControlled;
                break;
//            case 'C':
//                s.state=fvsVcConflict;
//                break;
            case '?':
                s.state=fvsVcNonControlled;
                break;
            case 'R':
                s.state=fvsVcModified;
                break;
            case 'P': //pending merge
                s.state=fvsVcOutOfDate;
                break;
            default:
                break;
        }
        a=output[i][1];
        switch(a)
        {
            case 'N': // created
                s.state=fvsVcAdded;
                break;
            case 'D': //deleted
                s.state=fvsVcMissing;
                break;
            case 'K': //kind changed
            case 'M': //modified
                s.state=fvsVcModified;
                break;
            default:
                break;
        }
        if(output[i][0]=='C')
            s.state=fvsVcConflict;
        wxFileName f(output[i].Mid(4));
        f.MakeAbsolute(rpath);
        s.path=f.GetFullPath();
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseHGstate(const wxString &path, VCSstatearray &sa)
{
    wxString parent=path;
    while(true)
    {
        if(wxFileName::DirExists(wxFileName(parent,_T(".hg")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if(oldparent==parent||parent.IsEmpty())
            return false;
    }
    if(parent.IsEmpty())
        return false;
    wxArrayString output;
    wxString wdir=wxGetCwd();
    wxSetWorkingDirectory(path); //TODO: Check if thread safe!
    int hresult=Exec(_T("hg -y stat ."),output);
    wxSetWorkingDirectory(wdir);
    if(hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if(output[i].Len()<=2)
            break;
        VCSstate s;
        wxChar a=output[i][0];
        switch(a)
        {
            case 'C': //clean
                s.state=fvsVcUpToDate;
                break;
            case '?': //not tracked
                s.state=fvsVcNonControlled;
                break;
            case '!': // local copy removed -- will not see this file
                s.state=fvsVcMissing;
                break;
            case 'A': // added
                s.state=fvsVcAdded;
                break;
            case 'R': //removed from branch, but exists in local copy
                s.state=fvsVcMissing;
                break;
            case 'M': //modified
                s.state=fvsVcModified;
                break;
            default:
                break;
        }
        wxFileName f(output[i].Mid(2));
        f.MakeAbsolute(path);
        s.path=f.GetFullPath();
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseCVSstate(const wxString &path, VCSstatearray &sa)
{
    wxArrayString output;
    wxString wdir=wxGetCwd();
    wxSetWorkingDirectory(path);
    Exec(_T("cvs stat -q -l  ."),output);
    wxSetWorkingDirectory(wdir);
//    if(hresult!=0)
//        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        int ind1=output[i].Find(_T("File: "));
        int ind2=output[i].Find(_T("Status: "));
        if(ind1<0||ind2<0)
            return false;
        wxString state=output[i].Mid(ind2+8).Strip();
        VCSstate s;
        while(1)
        {
            if(state==_T("Up-to-date"))
            {
                s.state=fvsVcUpToDate;
                break;
            }
            if(state==_T("Locally Modified"))
            {
                s.state=fvsVcModified;
                break;
            }
            if(state==_T("Locally Added"))
            {
                s.state=fvsVcAdded;
                break;
            }
            break;
        }
        wxFileName f(output[i].Mid(ind1+6,ind2+6-ind1).Strip());
        f.MakeAbsolute(path);
        s.path=f.GetFullPath();
        sa.Add(s);
    }
    if(output.GetCount()>0)
        return true;
    else
        return false;
}
