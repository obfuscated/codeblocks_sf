#include <sdk.h>

#include <wx/sstream.h>
#include <wx/regex.h>
#include <set>

#include "FileExplorerUpdater.h"
#include "FileExplorer.h"
#include "se_globals.h"

DEFINE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_COMPLETE)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_COMMITS_UPDATE_COMPLETE)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_LOADER_UPDATE_COMPLETE)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_EXEC_REQUEST)

int ID_EXEC_TIMER=wxNewId();

BEGIN_EVENT_TABLE(Updater, wxEvtHandler)
    EVT_TIMER(ID_EXEC_TIMER, FileExplorerUpdater::OnExecTimer)
    EVT_END_PROCESS(wxID_ANY, FileExplorerUpdater::OnExecTerminate)
    EVT_COMMAND(0, wxEVT_NOTIFY_EXEC_REQUEST, FileExplorerUpdater::OnExecMain)
END_EVENT_TABLE()

Updater::~Updater()
{
    if (m_exec_proc)
    {
        if (m_exec_timer)
            m_exec_timer->Stop();
        delete m_exec_timer;
        m_exec_proc->Detach();
        m_exec_cond->Signal();
        m_exec_mutex->Unlock();
    }
    if (IsRunning())
    {
        m_kill=true;
        Wait();
    }
}

int Updater::Exec(const wxString &command, wxString &output, const wxString &path)
{
    int exitcode=0;
    m_exec_mutex=new wxMutex();
    m_exec_cond=new wxCondition(*m_exec_mutex);
    m_exec_cmd=command;
    m_exec_path=path;
    m_exec_mutex->Lock();
    CodeBlocksThreadEvent ne(wxEVT_NOTIFY_EXEC_REQUEST,0);
    this->AddPendingEvent(ne);
    m_exec_cond->Wait();
    m_exec_mutex->Unlock();
    delete m_exec_cond;
    delete m_exec_mutex;
    if (m_exec_proc_id==0)
        exitcode=1;
//    DetectEncodingAndConvert(m_exec_output.mb_str(), output);
//    wxString a(m_exec_output.c_str(), wxConvAuto);
    output = wxString(m_exec_output.c_str());
    return exitcode;
}

int Updater::Exec(const wxString &command, wxArrayString &output, const wxString &path)
{
    wxString out;
    int exitcode = Exec(command, out, path);
    while(out.Len()>0)
    {
        output.Add(out.BeforeFirst(_T('\n')));
        out = out.AfterFirst(_T('\n'));
    }
    return exitcode;
}

void Updater::OnExecMain(wxCommandEvent &/*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(_T("File Manager Command: ")+m_exec_cmd+_T("\nin ")+m_exec_path);
//    m_exec_output.Empty();
    wxString str(_T(""), wxConvISO8859_1);
    str.reserve(10000);
    m_exec_output = str;
    m_exec_sstream = new wxStringOutputStream(&m_exec_output);
    m_exec_proc=new wxProcess(this);
    m_exec_proc->Redirect();
    m_exec_mutex->Lock();
    wxString wdir=wxGetCwd();
    wxSetWorkingDirectory(m_exec_path);
    m_exec_proc_id=wxExecute(m_exec_cmd,wxEXEC_ASYNC,m_exec_proc);
    wxSetWorkingDirectory(wdir);
//    if (!wxProcess::Exists(m_exec_proc_id))
//        m_exec_proc_id = 0;
    if (m_exec_proc_id==0)
    {
        m_exec_cond->Signal();
        m_exec_mutex->Unlock();
        Manager::Get()->GetLogManager()->DebugLog(_T("File Manager Command failed to execute: "));
        return;
    }
    m_exec_timer=new wxTimer(this,ID_EXEC_TIMER);
    m_exec_timer->Start(100,true);
}

void Updater::OnExecTerminate(wxProcessEvent &e)
{
    ReadStream(true);
    if (m_exec_timer)
        m_exec_timer->Stop();
    delete m_exec_sstream;
    delete m_exec_timer;
    delete m_exec_proc;
    if (e.GetExitCode() == 255)
        m_exec_proc_id = 0;
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_T("process finished with exit code %i, pid %i"), e.GetExitCode(), e.GetPid()));
    m_exec_proc=NULL;
    m_exec_cond->Signal();
    m_exec_mutex->Unlock();
//    Manager::Get()->GetLogManager()->DebugLog(_T("Exec Output\n")+m_exec_output);
}

void Updater::OnExecTimer(wxTimerEvent &/*e*/)
{
    if (m_exec_proc)
        ReadStream();
}

void Updater::ReadStream(bool all)
{
    m_exec_timer->Stop();
    m_exec_stream=m_exec_proc->GetInputStream();
    wxStopWatch sw;
    while(m_exec_proc->IsInputAvailable())
    {
        int c;
        if (m_exec_stream->CanRead())
            c = m_exec_stream->GetC();
        if (m_exec_stream->LastRead()>0)
            m_exec_sstream->PutC(c);
        if (!all && sw.Time()>30)
            break;
    }
    if (!all)
    {
        m_exec_timer->Start(150,true);
    }
}

void FileExplorerUpdater::Update(const wxTreeItemId &ti)
{
    m_path=wxString(m_fe->GetFullPath(ti).c_str());
    m_wildcard=wxString(m_fe->m_WildCards->GetValue().c_str());
    m_vcs_type=wxString(m_fe->m_VCS_Type->GetLabel().c_str());
    m_vcs_commit_string=wxString(m_fe->m_VCS_Control->GetString(m_fe->m_VCS_Control->GetSelection()).c_str());
    m_vcs_changes_only = m_fe->m_VCS_ChangesOnly->IsChecked();
    if (m_vcs_type != wxEmptyString)
        m_repo_path=wxString(m_fe->GetRootFolder().c_str());
    GetTreeState(ti);
    if (Create()==wxTHREAD_NO_ERROR)
    {
        SetPriority(20);
        Run();
    }
}

void *FileExplorerUpdater::Entry()
{
    CodeBlocksThreadEvent ne(wxEVT_NOTIFY_UPDATE_COMPLETE,0);
    if (!GetCurrentState(m_path))
    {
        m_cancelled=true; //TODO: SEND A CANCEL EVENT INSTEAD
        m_fe->AddPendingEvent(ne);
        return NULL;
    }
    if (!CalcChanges())
    {
        m_cancelled=true;
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
    if (m_fe->m_show_hidden)
        flags|=wxDIR_HIDDEN;
    VCSstatearray sa;
    bool is_vcs=false;
    bool is_cvs=false;
    bool is_git=false;

    //If we are browsing a repo we get the tree for the requested commit
    if (m_vcs_type != wxEmptyString && m_vcs_commit_string != _T("Working copy") &&
            m_vcs_commit_string != wxEmptyString && !m_vcs_changes_only)
    {
        if (m_vcs_type == _T("GIT"))
        {
            if (GetGITCommitState(path))
                return !TestDestroy();
        }
        else if (m_vcs_type == _T("Hg"))
        {
            if (GetHgCommitState(path))
                return !TestDestroy();
        }
        else if (m_vcs_type == _T("BZR"))
        {
            if (GetVCSCommitState(path, _T("bzr")))
                return !TestDestroy();
        }
        else if (m_vcs_type == _T("SVN"))
        {
            if (GetVCSCommitState(path, _T("svn")))
                return !TestDestroy();
        }
    }
    if (m_vcs_type != wxEmptyString  && m_vcs_changes_only)
    {
        bool parsed = false;
        if (m_vcs_type == _T("GIT"))
        {
            if (m_vcs_commit_string == _T("Working copy"))
                parsed = ParseGITChanges(path, sa, true);
            else
                parsed = ParseGITChangesTree(path, sa, true);
        }
        else if (m_vcs_type == _T("Hg"))
        {
            if (m_vcs_commit_string == _T("Working copy"))
                parsed = ParseHGChanges(path, sa, true);
            else
                parsed = ParseHGChangesTree(path, sa, true);
        }
        else if (m_vcs_type == _T("BZR"))
        {
            if (m_vcs_commit_string == _T("Working copy"))
                parsed = ParseBZRChanges(path, sa, true);
            else
                parsed = ParseBZRChangesTree(path, sa, true);
        }
        else if (m_vcs_type == _T("SVN"))
        {
            if (m_vcs_commit_string == _T("Working copy"))
                parsed = ParseSVNChanges(path, sa, true);
            else
                parsed = ParseSVNChangesTree(path, sa, true);
        }
        if (parsed)
        {
            for (unsigned int i = 0; i<sa.size(); ++i)
            {
                FileData fd;
                fd.name=sa[i].path;
                fd.state=sa[i].state;
                m_currentstate.push_back(fd);
            }
            return !TestDestroy();
        }
    }

    //Otherwise we are browsing an unversioned folder or the working copy
    m_vcs_type = _T("");

    // TODO: THIS NEEDS TO BE CALLED FROM MAIN THREAD BUT CAN'T BE UI-BLOCKING (CAN'T CALL wxExecute FROM THREADS)
    // TODO: IDEALLY THE VCS COMMAND LINE PROGRAM SHOULD BE CALLED ONCE ON THE BASE DIRECTORY (SINCE THEY ARE USUALLY RECURSIVE) TO AVOID REPEATED CALLS FOR SUB-DIRS
    if (m_fe->m_parse_git)
        if (ParseGITChanges(path,sa))
        {
            is_vcs=true;
            is_git=true;
            m_vcs_type = _T("GIT");
        }
    if (m_fe->m_parse_svn)
        if (ParseSVNChanges(path,sa))
        {
            is_vcs=true;
            m_vcs_type = _T("SVN");
        }
    if (m_fe->m_parse_bzr && !is_vcs)
        if (ParseBZRChanges(path,sa))
        {
            is_vcs=true;
            m_vcs_type = _T("BZR");
        }
    if (m_fe->m_parse_hg && !is_vcs)
        if (ParseHGChanges(path,sa))
        {
            is_vcs=true;
            m_vcs_type = _T("Hg");
        }
/*    if (m_fe->m_parse_cvs && !is_vcs)
        if (ParseCVSChanges(path,sa))
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
        if (wxFileName::DirExists(fullpath))
            itemstate=fvsFolder;
        if (wxFileName::FileExists(fullpath))
        {
//            if (is_vcs&&!is_cvs&&!is_git)
//                itemstate=fvsVcUpToDate;
//            else
//            itemstate=fvsNormal;
            wxFileName fn(path,filename);
#if wxCHECK_VERSION(2,8,0)
            if (!fn.IsFileWritable())
                itemstate=fvsReadOnly;
#endif
            for(size_t i=0;i<sa.GetCount();i++)
            {
                if (fn.GetFullPath() == sa[i].path || fn.SameAs(sa[i].path))
                {
                    itemstate=sa[i].state;
                    break;
                }
            }
            if (!WildCardListMatch(m_wildcard,filename,true))
                match=false;
        }
        if (match)
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
            if (it->name==tree_it->name)
            {
                match=true;
                if (it->state!=tree_it->state)
                {
                    m_adders.push_back(*it);
                    m_removers.push_back(*tree_it);
                }
                m_currentstate.erase(it);
                tree_it=m_treestate.erase(tree_it);
                break;
            }
        if (!match)
            tree_it++;
    }
    for(FileDataVec::iterator tree_it2 = m_treestate.begin(); tree_it2 != m_treestate.end(); ++tree_it2)
        m_removers.push_back(*tree_it2);
    for(FileDataVec::iterator it=m_currentstate.begin();it!=m_currentstate.end();it++)
        m_adders.push_back(*it);
    return !TestDestroy();
}

bool FileExplorerUpdater::ParseSVNChanges(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxString parent = path;
// Older versions of subversion had a hidden .svn folder in every directory in the repo, but
// that's no longer the case
//    if (!wxFileName::DirExists(wxFileName(path,_T(".svn")).GetFullPath()))
//        return false;
    while(true)
    {
        if (wxFileName::DirExists(wxFileName(parent,_T(".svn")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if (oldparent==parent||parent.IsEmpty())
            return false;
    }
    wxArrayString output;
    int name_pos = 8;
    int hresult = Exec(_T("svn stat -N ."), output, path);
    if (hresult != 0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=7)
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
        if (relative_paths)
        {
            s.path = output[i].Mid(name_pos);
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseSVNChangesTree(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxArrayString output;
    int name_pos = 8;
    if (m_vcs_commit_string == wxEmptyString)
        return false;
    wxFileName rel_root_fn = wxFileName(path);
    rel_root_fn.MakeRelativeTo(m_repo_path);
    wxString rel_root_path = rel_root_fn.GetFullPath();
    int hresult = Exec(_T("svn diff --summarize -c") + m_vcs_commit_string + _T(" ") + rel_root_path, output, m_repo_path);
    if (hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=3)
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
        if (relative_paths)
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeRelativeTo(rel_root_path);
            s.path=f.GetFullPath();
//            s.path = output[i].Mid(name_pos);
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(m_repo_path);
            s.path=f.GetFullPath();
        }
        //TODO: Filter out subpaths
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseGITChanges(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxString parent=path;
    while(true)
    {
        if (wxFileName::DirExists(wxFileName(parent,_T(".git")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if (oldparent==parent||parent.IsEmpty())
            return false;
    }
    if (parent.IsEmpty())
        return false;
    wxArrayString output;
    wxString rpath=parent;
    int name_pos;
    #ifdef __WXMSW__
    int hresult = Exec(_T("cmd /c git status --short"), output, parent);
    #else
    int hresult = Exec(_T("git status --short"), output, parent);
    #endif
    if (hresult!=0)
        return false;
    name_pos = 3;
    for (size_t i=0;i<output.GetCount();i++)
    {

/*
Per git status --help.
Status code is 2 letter code
       o   ' ' = unmodified
       o   M = modified
       o   A = added
       o   D = deleted
       o   R = renamed
       o   C = copied
       o   U = updated but unmerged

*/
        if (output[i].Len()<=3)
            break;
        VCSstate s;
        wxChar a=output[i][0];
        switch(a)
        {
            case 'M':
                s.state=fvsVcUpToDate;
                break;
            case 'A':
                s.state=fvsVcUpToDate;
                break;
            case 'D':
                s.state=fvsVcUpToDate;
                break;
            case 'R':
                s.state=fvsVcUpToDate;
                break;
            case 'C':
                s.state=fvsVcUpToDate;
                break;
            case 'U':
                s.state=fvsVcUpToDate;
                break;
            case '?':
                s.state=fvsVcNonControlled;
                break;
            default:
                s.state=fvsNormal;
                break;
        }
        a=output[i][1];
        switch(a)
        {
            case 'M':
                s.state=fvsVcModified;
                break;
            case 'A':
                s.state=fvsVcAdded;
                break;
            case 'D':
                s.state=fvsVcModified;
                break;
            case 'R':
                s.state=fvsVcModified;
                break;
            case 'C':
                s.state=fvsVcModified;
                break;
            case 'U':
                s.state=fvsVcModified;
                break;
            case '?':
                s.state=fvsVcNonControlled;
                break;
            case ' ':
                break;
            default:
                s.state=fvsNormal;
                break;
        }
        if (output[i][0]!=' ' && output[i][1]!=' ' && output[i][0]!=output[i][1])
            s.state=fvsVcConflict;
        if (relative_paths)
        {
            s.path = output[i].Mid(name_pos);
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseGITChangesTree(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxArrayString output;
    wxFileName rel_root_fn = wxFileName(path);
    rel_root_fn.MakeRelativeTo(m_repo_path);
    wxString rel_root_path = rel_root_fn.GetFullPath();
    int name_pos = 2;
    if (m_vcs_commit_string == wxEmptyString)
        return false;
    int hresult = Exec(_T("git show --name-status --format=oneline ") + m_vcs_commit_string + _T(" ")+rel_root_path, output, m_repo_path);
    if (hresult!=0)
        return false;
    if (output.GetCount() > 0) // first line is the one-line commit summary
        output.RemoveAt(0);
    for (size_t i=0;i<output.GetCount();i++)
    {

/*
Per git status --help.
Status code is 2 letter code
       o   ' ' = unmodified
       o   M = modified
       o   A = added
       o   D = deleted
       o   R = renamed
       o   C = copied
       o   U = updated but unmerged

*/
        if (output[i].Len()<=3)
            continue;
        VCSstate s;
        wxChar a=output[i][0];
        switch(a)
        {
            case 'M':
                s.state=fvsVcModified;
                break;
            case 'A':
                s.state=fvsVcAdded;
                break;
            case 'D':
                s.state=fvsVcModified;
                break;
            case 'R':
                s.state=fvsVcModified;
                break;
            case 'C':
                s.state=fvsVcModified;
                break;
            case 'U':
                s.state=fvsVcModified;
                break;
            case '?':
                s.state=fvsVcNonControlled;
                break;
            case ' ':
                break;
            default:
                s.state=fvsNormal;
                break;
        }
        s.path = output[i].Mid(name_pos);
        if (!s.path.StartsWith(rel_root_path))
            continue;
        //GIT returns all paths relative to the root of the repo path, so need to convert it to be relative to the sub-path
        if (relative_paths)
        {
            if (path != m_repo_path)
            {
                wxFileName f(s.path);
                f.MakeRelativeTo(rel_root_path);
                s.path = f.GetFullPath();
            }
        }
        else
        {
            wxFileName f(s.path);
            f.MakeAbsolute(m_repo_path);
            s.path=f.GetFullPath();
        }
        //TODO: Filter out subpaths if we don't want to recurse into subdirs
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseBZRChanges(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxString parent=path;
    while(true)
    {
        if (wxFileName::DirExists(wxFileName(parent,_T(".bzr")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if (oldparent==parent||parent.IsEmpty())
            return false;
    }
    if (parent.IsEmpty())
        return false;
    wxArrayString output;
    int name_pos = 4;
    wxString rpath=parent;
    #ifdef __WXMSW__
    int hresult=Exec(_T("cmd /c bzr stat --short ")+path, output, path);
    #else
    int hresult=Exec(_T("bzr stat --short ")+path, output, path);
    #endif
    if (hresult!=0)
    {
        return false;
    }
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=4)
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
        if (output[i][0]=='C')
            s.state=fvsVcConflict;
        if (relative_paths)
        {
            s.path = output[i].Mid(name_pos);
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        //TODO: Filter out subpaths (if required) and make sure the file path is expressed relative to the parent path
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseBZRChangesTree(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxArrayString output;
    int name_pos = 4;
    if (m_vcs_commit_string == wxEmptyString)
        return false;
    wxFileName rel_root_fn = wxFileName(path);
    rel_root_fn.MakeRelativeTo(m_repo_path);
    wxString rel_root_path = rel_root_fn.GetFullPath();
    int hresult = Exec(_T("bzr status --short -c ") + m_vcs_commit_string + _T(" ") + rel_root_path, output, m_repo_path);
    if (hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=4)
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
        if (output[i][0]=='C')
            s.state=fvsVcConflict;
        if (relative_paths)
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeRelativeTo(rel_root_path);
            s.path=f.GetFullPath();
        }
        else
        {
            wxFileName f(s.path);
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseHGChanges(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxString parent=path;
    while(true)
    {
        if (wxFileName::DirExists(wxFileName(parent,_T(".hg")).GetFullPath()))
            break;
        wxString oldparent=parent;
        parent=wxFileName(parent).GetPath();
        if (oldparent==parent||parent.IsEmpty())
            return false;
    }
    if (parent.IsEmpty())
        return false;
    int name_pos = 2;
    wxArrayString output;
    int hresult=Exec(_T("hg -y stat ."), output, path);
    if (hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=2)
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
        if (relative_paths)
        {
            s.path = output[i].Mid(name_pos);
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseHGChangesTree(const wxString &path, VCSstatearray &sa, bool relative_paths)
{
    wxArrayString output;
    int name_pos = 2;
    if (m_vcs_commit_string == wxEmptyString)
        return false;
    wxFileName rel_root_fn = wxFileName(path);
    rel_root_fn.MakeRelativeTo(m_repo_path);
    wxString rel_root_path = rel_root_fn.GetFullPath();
    int hresult = Exec(_T("hg status --change ") + m_vcs_commit_string + _T(" ") + rel_root_path, output, m_repo_path);
    if (hresult!=0)
        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        if (output[i].Len()<=2)
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
        if (relative_paths)
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeRelativeTo(rel_root_path);
            s.path=f.GetFullPath();
        }
        else
        {
            wxFileName f(output[i].Mid(name_pos));
            f.MakeAbsolute(path);
            s.path=f.GetFullPath();
        }
        //TODO: Filter out subpaths (if required) and make sure the file path is expressed relative to the parent path
        sa.Add(s);
    }
    return true;
}

bool FileExplorerUpdater::ParseCVSChanges(const wxString &path, VCSstatearray &sa)
{
    wxArrayString output;
    wxString wdir=wxGetCwd();
    Exec(_T("cvs stat -q -l  ."),output,path);
//    if (hresult!=0)
//        return false;
    for(size_t i=0;i<output.GetCount();i++)
    {
        int ind1=output[i].Find(_T("File: "));
        int ind2=output[i].Find(_T("Status: "));
        if (ind1<0||ind2<0)
            return false;
        wxString state=output[i].Mid(ind2+8).Strip();
        VCSstate s;
        while(1)
        {
            if (state==_T("Up-to-date"))
            {
                s.state=fvsVcUpToDate;
                break;
            }
            if (state==_T("Locally Modified"))
            {
                s.state=fvsVcModified;
                break;
            }
            if (state==_T("Locally Added"))
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
    if (output.GetCount()>0)
        return true;
    else
        return false;
}

bool FileExplorerUpdater::GetGITCommitState(const wxString &path)
{
    //TODO: Should check TestDestroy here and return prematurely if necessary
    wxArrayString output, dir_output;

    // git ls-tree requires a relative path with appended separator to work correctly
    wxFileName root_fn = wxFileName(path);
//    fn.AppendDir(_T("a")); //append a fake part to the path
    root_fn.MakeRelativeTo(m_repo_path); //make it relative to the repo_path
    wxString rel_path = root_fn.GetFullPath(); //then extract the path (without the "a") with separator
    if (rel_path == wxEmptyString)
        rel_path = _T(".");
    else
        rel_path += wxFileName::GetPathSeparator();
    //Reads the tree in two commands
    //TODO: Alternatively read and parse a single "git ls-tree" i.e. without --name-only
    Exec(_T("git ls-tree --name-only ")+m_vcs_commit_string + _T(" ") + rel_path, output, m_repo_path);
    Exec(_T("git ls-tree -d --name-only ")+m_vcs_commit_string + _T(" ") + rel_path, dir_output, m_repo_path);

    VCSstatearray sa;
    ParseGITChangesTree(path, sa, true);

    for (unsigned int i=0; i<output.GetCount(); ++i)
    {
        FileData fd;
        if (output[i] == wxEmptyString)
            continue;
        wxFileName fn(output[i]);
        fn.MakeRelativeTo(rel_path);
        fd.name = fn.GetFullName();
        fd.state = fvsNormal;
        for (unsigned int j=0; j<dir_output.GetCount(); ++j)
        {
            if (dir_output[j] == output[i])
            {
                dir_output.RemoveAt(j);
                fd.state = fvsFolder;
                break;
            }
        }
        for(size_t j=0;j<sa.GetCount();j++)
        {
            if (fn.GetFullPath() == sa[j].path || fn.SameAs(sa[j].path))
            {
                fd.state=sa[j].state;
                sa.RemoveAt(j);
                break;
            }
        }
        m_currentstate.push_back(fd);
    }

    //TODO: We could also get the changed file info from the previous commit
    //git show --name-status --oneline <commit>
    return !TestDestroy();
}

bool FileExplorerUpdater::GetHgCommitState(const wxString &path)
{
    //TODO: Should check TestDestroy here and return prematurely if necessary
    wxArrayString output, dir_output;

    wxFileName root_fn = wxFileName(path);
    root_fn.MakeRelativeTo(m_repo_path); //make the target path relative to the repo_path
    wxString rel_path = root_fn.GetFullPath();
    if (rel_path != wxEmptyString)
        rel_path += wxFileName::GetPathSeparator();

    //Hg doesn't have an ls command, but manifest dumps the entire directory structure
    //However, even if the working directory is a subdirectory of the repo root, the listing will include
    //all files from the repo root (so probably should only allow user to browse the repo history
    // from the root of the repo)
    Exec(_T("hg manifest -r") + m_vcs_commit_string, output, m_repo_path);

    VCSstatearray sa;
    ParseHGChangesTree(path, sa, true);

    std::set<wxString> dirs;

    for (unsigned int i=0; i<output.GetCount(); ++i)
    {
        FileData fd;
        if (output[i] == wxEmptyString)
            continue;
        if (!output[i].StartsWith(rel_path))
            continue;
        wxFileName fn(output[i]);
        fn.MakeRelativeTo(rel_path);
        wxString name = fn.GetFullPath();
        wxString subdir = name.BeforeFirst(_T('/'));
        if (subdir != name)
        {
            if (dirs.find(subdir) != dirs.end())
                continue;
            dirs.insert(subdir);
            fd.state = fvsFolder;
            fd.name = subdir;
        }
        else
        {
            fd.state = fvsNormal;
            fd.name = name;
        }
        for(size_t j=0;j<sa.GetCount();j++)
        {
            if (fn.GetFullPath() == sa[j].path || fn.SameAs(sa[j].path))
            {
                if (fd.state != fvsFolder)
                    fd.state=sa[j].state;
                sa.RemoveAt(j);
                break;
            }
        }
        m_currentstate.push_back(fd);
    }

    return !TestDestroy();
}

bool FileExplorerUpdater::GetVCSCommitState(const wxString &path, const wxString &cmd)
{
    //TODO: Should check TestDestroy here and return prematurely if necessary
    wxArrayString output, dir_output;

    wxFileName root_fn = wxFileName(path);
    root_fn.MakeRelativeTo(m_repo_path); //make it relative to the repo_path
    wxString rel_path = root_fn.GetFullPath(); //then extract the relative path
    if (rel_path == wxEmptyString)
        rel_path = _T(".");
    else
        rel_path += wxFileName::GetPathSeparator();

    Exec(cmd + _T(" ls -r") + m_vcs_commit_string + _T(" ") + rel_path, output, m_repo_path);

    VCSstatearray sa;
    if (m_vcs_type == _T("SVN"))
        ParseSVNChangesTree(path, sa, true);
    else if (m_vcs_type == _T("BZR"))
        ParseBZRChangesTree(path, sa, true);

    for (unsigned int i=0; i<output.GetCount(); ++i)
    {
        FileData fd;
        if (output[i] == wxEmptyString)
            continue;
        wxFileName fn(output[i]);
        fn.MakeRelativeTo(rel_path);
        fd.state = fvsNormal;
        if (fn.IsDir())
        {
            fd.state = fvsFolder;
            fn = wxFileName(fn.GetPath());
        }
        else
        for(size_t j=0;j<sa.GetCount();j++)
        {
            if (fn.GetFullName() == sa[j].path || fn.SameAs(sa[j].path))
            {
                fd.state = sa[j].state;
                sa.RemoveAt(j);
                break;
            }
        }
        fd.name = fn.GetFullName();;
        m_currentstate.push_back(fd);
    }
    //TODO: We could also get the changed file info from the previous commit
    return !TestDestroy();
}

void VCSFileLoader::Update(const wxString &op, const wxString &source_path, const wxString &destination_path,
                           const wxString &comp_commit)
{
    m_source_path=wxString(source_path.c_str());
    m_destination_path=wxString(destination_path.c_str());
    m_vcs_type=wxString(m_fe->m_VCS_Type->GetLabel().c_str());
    m_vcs_commit_string=wxString(m_fe->m_VCS_Control->GetString(m_fe->m_VCS_Control->GetSelection()).c_str());
    m_vcs_op = wxString(op.c_str());
    m_vcs_comp_commit = wxString(comp_commit.c_str());
    if (m_vcs_type != wxEmptyString)
        m_repo_path=wxString(m_fe->GetRootFolder().c_str());
    if (Create()==wxTHREAD_NO_ERROR)
    {
        SetPriority(20);
        Run();
    }
}

void *VCSFileLoader::Entry()
{
    CodeBlocksThreadEvent ne(wxEVT_NOTIFY_LOADER_UPDATE_COMPLETE,0);
    if (m_vcs_op == _T("cat"))
    {
        if (m_vcs_type == _T("GIT"))
        {
            wxString output;
            wxFileName source_rel_path(m_source_path);
            source_rel_path.MakeRelativeTo(m_repo_path);
            Exec(_T("git show ")+m_vcs_commit_string+_T(":")+source_rel_path.GetFullPath(),output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("Hg"))
        {
            wxString output;
            wxFileName source_rel_path(m_source_path);
            source_rel_path.MakeRelativeTo(m_repo_path);
            Exec(_T("hg cat -r")+m_vcs_commit_string+_T(" ")+source_rel_path.GetFullPath(),output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("BZR"))
        {
            wxString output;
            wxFileName source_rel_path(m_source_path);
            source_rel_path.MakeRelativeTo(m_repo_path);
            Exec(_T("bzr cat -r")+m_vcs_commit_string+_T(" ")+source_rel_path.GetFullPath(),output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("SVN"))
        {
            wxString output;
            wxFileName source_rel_path(m_source_path);
            source_rel_path.MakeRelativeTo(m_repo_path);
            Exec(_T("svn cat -r")+m_vcs_commit_string+_T(" ")+source_rel_path.GetFullPath(),output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
    }
    if (m_vcs_op == _T("diff"))
    {
        if (m_vcs_type == _T("GIT"))
        {
            wxString output, comp_string;
            if (m_vcs_commit_string == _T("Working copy"))
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string = wxEmptyString;
                else
                    comp_string = m_vcs_comp_commit;
            }
            else
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string =  m_vcs_commit_string + _T("^..") + m_vcs_commit_string;
                else if (m_vcs_comp_commit == _T("Working copy"))
                    comp_string = m_vcs_commit_string;
                else
                    comp_string = m_vcs_comp_commit + _T("..") + m_vcs_commit_string;
            }
            Exec(_T("git diff ") + comp_string + _T(" -- ")+ m_source_path, output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("Hg"))
        {
            wxString output, comp_string;
            if (m_vcs_commit_string == _T("Working copy"))
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string = wxEmptyString;
                else
                    comp_string = _T(" -r ") + m_vcs_comp_commit;
            }
            else
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string =  _T(" -c") + m_vcs_commit_string;
                else if (m_vcs_comp_commit == _T("Working copy"))
                    comp_string = _T(" -r ") + m_vcs_commit_string;
                else
                    comp_string = _T(" -r ") + m_vcs_comp_commit + _T(" -r ") + m_vcs_commit_string;
            }
            Exec(_T("hg diff ") + comp_string + _T(" ") + m_source_path, output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("BZR"))
        {
            wxString output, comp_string;
            if (m_vcs_commit_string == _T("Working copy"))
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string = wxEmptyString;
                else
                    comp_string = _T(" -r") + m_vcs_comp_commit;
            }
            else
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string =  _T(" -c") + m_vcs_commit_string;
                else if (m_vcs_comp_commit == _T("Working copy"))
                    comp_string = _T(" -r") + m_vcs_commit_string;
                else
                    comp_string = _T(" -r") + m_vcs_comp_commit + _T("..") + m_vcs_commit_string;
            }
            Exec(_T("bzr diff ") + comp_string + _T(" ") + m_source_path, output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
        else if (m_vcs_type == _T("SVN"))
        {
            wxString output, comp_string;
            if (m_vcs_commit_string == _T("Working copy"))
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string = wxEmptyString;
                else
                    comp_string = _T(" -r" ) + m_vcs_comp_commit;
            }
            else
            {
                if (m_vcs_comp_commit == _T("Previous"))
                    comp_string =  _T(" -c ") + m_vcs_commit_string;
                else if (m_vcs_comp_commit == _T("Working copy"))
                    comp_string = _T(" -r ") + m_vcs_commit_string;
                else
                    comp_string = _T(" -r" ) + m_vcs_comp_commit + _T(":") + m_vcs_commit_string;
            }
            Exec(_T("svn diff ") + comp_string + _T(" ") + m_source_path, output, m_repo_path);
            wxFileName::Mkdir(wxFileName(m_destination_path).GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFile(m_destination_path, wxFile::write).Write(output);
        }
    }
    m_fe->AddPendingEvent(ne);
    return 0;
}

CommitUpdater::CommitUpdater(const CommitUpdater& updater) : Updater()
{
    //Inputs
    m_what = updater.m_what;
    m_repo_path = updater.m_repo_path;
    m_repo_type = updater.m_repo_type;
    m_repo_branch = updater.m_repo_branch;
    m_opts = updater.m_opts;
    m_parent = updater.m_parent;
    m_last_commit_retrieved = updater.m_last_commit_retrieved; // the ID of the last commit retrieved (only filled if what == "COMMIT:")
    m_retrieved_all = updater.m_retrieved_all;
    m_continue_count = updater.m_continue_count;
}


bool CommitUpdater::Update(const wxString &what, const wxString &repo_branch, CommitUpdaterOptions opts)
{
    if (IsRunning())
        return false;
    if (!(what.StartsWith(_T("BRANCHES")) ||
        what.StartsWith(_T("COMMITS:")) ||
        what.StartsWith(_T("DETAIL:"))) )
        return false;
    m_what = wxString(what.c_str());
    m_repo_branch = repo_branch;
    m_opts = opts;
    m_continue_count = 0;
    m_last_commit_retrieved = wxEmptyString;
    if (Create()!=wxTHREAD_NO_ERROR)
        return false;
    SetPriority(20);
    Run();
    return true;
}

bool CommitUpdater::UpdateContinueCommitRetrieve()
{
    if (IsRunning())
        return false;
    if (m_retrieved_all)
        return false;
    if (!m_what.StartsWith(_T("COMMITS:")))
        return false;
    if (Create()!=wxTHREAD_NO_ERROR)
        return false;
    SetPriority(20);
    Run();
    return true;
}

void *CommitUpdater::Entry()
{
    CodeBlocksThreadEvent ne(wxEVT_NOTIFY_UPDATE_COMPLETE,0);
    CodeBlocksThreadEvent nce(wxEVT_NOTIFY_COMMITS_UPDATE_COMPLETE,0);
    if (m_what.StartsWith(_T("BRANCHES")))
    {
        if (m_repo_type == _T("GIT"))
        {
            wxArrayString output;
            Exec(_T("git branch"),output, m_repo_path);
            for (unsigned int i=0; i<output.GetCount(); ++i)
                if (output[i].Strip(wxString::both) != wxEmptyString)
                {
                    if (output[i].StartsWith(_T("*")))
                        m_branches.Insert(output[i].Mid(2), 0);
                    else
                        m_branches.Add(output[i].Mid(2));
                }
        }
        if (m_repo_type == _T("Hg"))
        {
            wxArrayString output;
            Exec(_T("hg branches -q"),output, m_repo_path);
            for (unsigned int i=0; i<output.GetCount(); ++i)
                if (output[i].Strip(wxString::both) != wxEmptyString)
                    m_branches.Add(output[i].Strip(wxString::both));
        }
        if (m_repo_type == _T("SVN") || m_repo_type == _T("BZR"))
        {
            m_branches.Add(_T("Active Tree"));
        }
        m_parent->AddPendingEvent(ne);
        return 0;
    }
    if (m_what.StartsWith(_T("COMMITS:")))
    {
        wxArrayString output;
        std::vector<CommitEntry> commits;
        m_commits.clear();
        wxString branch = m_what.AfterFirst(_T(':'));
        if (m_repo_type == _T("GIT"))
        {
            wxString commit_date_range;
            if (m_opts.date_after != wxEmptyString)
                commit_date_range = _T(" --since=\"") + m_opts.date_after + _T("\" ");
            if (m_opts.date_before != wxEmptyString)
                commit_date_range += _T(" --until=\"") + m_opts.date_before + _T("\" ");
            wxString n;
            if (m_opts.commits_per_retrieve > 0)
            {
                n = wxString::Format(_T(" -n %i "),m_opts.commits_per_retrieve);
                n+= wxString::Format(_T("--skip %i "),m_continue_count*m_opts.commits_per_retrieve);
            }
            wxString file;
            if (m_opts.file != wxEmptyString)
                file = _T(" -- ") + m_opts.file;
            Exec(_T("git log --pretty=format:%H~%an~%ad~%s ") + commit_date_range + n + branch + file,output, m_repo_path);
            for (unsigned int i=0; i<output.GetCount(); ++i)
            {
                wxString s = output[i];
                CommitEntry cdata;
                cdata.id = s.BeforeFirst(_T('~'));
                s = s.AfterFirst(_T('~'));
                cdata.author = s.BeforeFirst(_T('~'));
                s = s.AfterFirst(_T('~'));
                cdata.date = s.BeforeFirst(_T('~'));
                s = s.AfterFirst(_T('~'));
                cdata.message = s;
                if (cdata.id.Strip(wxString::both) != wxEmptyString)
                    commits.push_back(cdata);
            }
        }
        if (m_repo_type == _T("Hg"))
        {
            wxString commit_date_range;
            if (m_opts.date_after != wxEmptyString && m_opts.date_before == wxEmptyString)
                commit_date_range = _T(" --date=\">") + m_opts.date_after + _T("\" ");
            if (m_opts.date_after == wxEmptyString && m_opts.date_before != wxEmptyString)
                commit_date_range += _T(" --date=\"<") + m_opts.date_before + _T("\" ");
            if (m_opts.date_after != wxEmptyString && m_opts.date_before != wxEmptyString)
                commit_date_range += _T(" --date=\"") + m_opts.date_after + _T(" to ") + m_opts.date_before + _T("\" ");
            wxString commit_range = wxEmptyString;
            if (m_opts.commits_per_retrieve > 0)
            {
                if (m_last_commit_retrieved == wxEmptyString)
                    commit_range = wxString::Format(_T(" -l%i "),m_opts.commits_per_retrieve);
                else
                {
                    long hi_commit;
                    if (m_last_commit_retrieved.ToLong(&hi_commit))
                    {
                        hi_commit--;
                        long low_commit = hi_commit - m_opts.commits_per_retrieve + 1;
                        if (hi_commit < 0)
                            hi_commit = 0;
                        if (low_commit < 0)
                            low_commit = 0;
                        if (low_commit == hi_commit && low_commit == 0)
                        {
                            m_parent->AddPendingEvent(ne);
                            return 0;
                        }
                        m_last_commit_retrieved = wxString::Format(_T("%i"),low_commit); //THIS IS SPECIAL LOGIC FOR HG BRANCHES (SEE BELOW)
                        commit_range = wxString::Format(_T(" -r%i:%i "),hi_commit,low_commit);
                    }
                }
            }
            wxString file;
            if (m_opts.file != wxEmptyString)
                file = _T(" ") + m_opts.file;

            Exec(_T("hg log --only-branch ") + m_repo_branch + commit_date_range + commit_range + file, output, m_repo_path);
            size_t i=0;
            size_t n = output.GetCount();
            while (i<n && !TestDestroy())
            {
                CommitEntry cdata;
                if (i>=n)
                    continue;
                wxString s = output[i];
                ++i;
                if (!s.StartsWith(_T("changeset:")))
                    continue;
                cdata.id = s.AfterFirst(_T(':')).BeforeFirst(_T(':')).Strip(wxString::both);
                if (i>=n)
                    continue;
                s = output[i];
                ++i;
                if (s.StartsWith(_T("branch:")))
                {
                    if (i>=n)
                        continue;
                    s = output[i];
                    ++i;
                }
                if (s.StartsWith(_T("tag:")))
                {
                    if (i>=n)
                        continue;
                    s = output[i];
                    ++i;
                }
                if (!s.StartsWith(_T("user:")))
                    continue;
                cdata.author = s.AfterFirst(_T(':')).Strip(wxString::both);

                if (i>=n)
                    continue;
                s = output[i];
                ++i;
                if (!s.StartsWith(_T("date:")))
                    continue;
                cdata.date = s.AfterFirst(_T(':')).Strip(wxString::both);

                if (i>=n)
                    continue;
                s = output[i];
                ++i;
                if (!s.StartsWith(_T("summary:")))
                    continue;
                cdata.message = s.AfterFirst(_T(':')).Strip(wxString::both);
                commits.push_back(cdata);
            }
        }
        if (m_repo_type == _T("BZR"))
        {
            wxString commit_range = wxEmptyString;
            if (m_continue_count == 0)
            {
                if (m_opts.date_after != wxEmptyString && m_opts.date_before == wxEmptyString)
                    commit_range = _T(" -r date:\"") + m_opts.date_after + _T("\".. ");
                else if (m_opts.date_after == wxEmptyString && m_opts.date_before != wxEmptyString)
                    commit_range = _T("-r ..date:\"") + m_opts.date_before + _T("\" ");
                else if (m_opts.date_after != wxEmptyString && m_opts.date_before != wxEmptyString)
                    commit_range = _T("-r date:\"") + m_opts.date_after + _T("\"..date:\"") + m_opts.date_before + _T("\" ");
                if (m_opts.commits_per_retrieve > 0)
                    commit_range += wxString::Format(_T(" -l%i "),m_opts.commits_per_retrieve);
            }
            else
            {
                long hi_commit;
                if (m_last_commit_retrieved.ToLong(&hi_commit))
                {
                    hi_commit--;
                    long low_commit = hi_commit - m_opts.commits_per_retrieve + 1;
                    if (hi_commit < 1)
                        hi_commit = 1;
                    if (low_commit < 1)
                        low_commit = 1;
                    if (low_commit == hi_commit && low_commit == 1)
                    {
                        m_parent->AddPendingEvent(ne);
                        return 0;
                    }
                    if (m_opts.date_after != wxEmptyString)
                        commit_range = wxString::Format(_T(" -rdate:\"%s\"..%i -l%i "), m_opts.date_after.c_str(),hi_commit, m_opts.commits_per_retrieve);
                    else
                        commit_range = wxString::Format(_T(" -r%i..%i "),low_commit,hi_commit);
                }
            }
            wxString file;
            if (m_opts.file != wxEmptyString)
                file = _T(" ") + m_opts.file;
            Exec(_T("bzr log ") + commit_range + file, output, m_repo_path);
            size_t i=0;
            size_t n = output.GetCount();
            output.RemoveAt(0); //First line of ----------------------
            CommitEntry cdata;
            while (i<n && !TestDestroy())
            {
                wxString s = output[i];
                ++i;
                if (s.StartsWith(_T("revno:")))
                    cdata.id = s.AfterFirst(_T(' ')).BeforeFirst(_T(' ')).Strip(wxString::both);
                else if (s.StartsWith(_T("committer:")))
                    cdata.author = s.AfterFirst(_T(':')).Strip(wxString::both);
                else if (s.StartsWith(_T("timestamp:")))
                    cdata.date = s.AfterFirst(_T(':')).Strip(wxString::both);
                else if (s.StartsWith(_T("message:")))
                {
                    while(i<n && !output[i].StartsWith(_T("------------------")))
                    {
                        cdata.message += output[i] + _T(" ");
                        ++i;
                    }
                }
                else if (s.StartsWith(_T("---------------------")))
                {
                    //finalize
                    commits.push_back(cdata);
                    cdata = CommitEntry();
                    continue;
                }
            }
        }
        if (m_repo_type == _T("SVN"))
        {
            wxString commit_range = wxEmptyString;
            //TODO: Check required date format
            if (m_continue_count == 0)
            {
                if (m_opts.date_after != wxEmptyString && m_opts.date_before == wxEmptyString)
                    commit_range = _T(" -r {") + m_opts.date_after + _T("}:1 ");
                else if (m_opts.date_after == wxEmptyString && m_opts.date_before != wxEmptyString)
                    commit_range = _T("-r HEAD:{") + m_opts.date_before + _T("} "); //or BASE??
                else if (m_opts.date_after != wxEmptyString && m_opts.date_before != wxEmptyString)
                    commit_range = _T("-r {") + m_opts.date_before + _T("}:{") + m_opts.date_after + _T("} ");
                if (m_opts.commits_per_retrieve > 0)
                    commit_range += wxString::Format(_T(" -l%i "), m_opts.commits_per_retrieve);
            }
            else
            {
                long hi_commit;
                if (m_last_commit_retrieved.ToLong(&hi_commit))
                {
                    hi_commit--;
                    long low_commit = hi_commit - m_opts.commits_per_retrieve + 1;
                    if (hi_commit < 1)
                        hi_commit = 1;
                    if (low_commit < 1)
                        low_commit = 1;
                    if (low_commit == hi_commit && low_commit == 1)
                    {
                        m_parent->AddPendingEvent(ne);
                        return 0;
                    }
                    if (m_opts.date_after != wxEmptyString)
                        commit_range = wxString::Format(_T(" -r%i:{%s} -l%i "), hi_commit, m_opts.date_after.c_str(), m_opts.commits_per_retrieve);
                    else
                        commit_range = wxString::Format(_T(" -r%i:%i "),hi_commit,low_commit);
                }
            }
            wxString file;
            if (m_opts.file != wxEmptyString)
                file = _T(" ") + m_opts.file;
            Exec(_T("svn log ") + commit_range + file, output, m_repo_path);
            size_t i=0;
            size_t n = output.GetCount();
            while (i<n && !TestDestroy())
            {
                CommitEntry cdata;
                wxString s = output[i];
                ++i;
                if (!s.StartsWith(_T("---------------------")))
                    continue;
                if (i>=n)
                    continue;
                s = output[i];
                ++i; ++i; //Blank line between commit info and the message
                if (!s.StartsWith(_T("r")))
                    continue;
                cdata.id = s.BeforeFirst(_T('|')).AfterFirst(_T('r')).Strip(wxString::both);
                s = s.AfterFirst(_T('|'));
                cdata.author = s.BeforeFirst(_T('|')).Strip(wxString::both);
                s = s.AfterFirst(_T('|'));
                cdata.date = s.BeforeFirst(_T('|')).Strip(wxString::both);
                s = s.AfterFirst(_T('|'));
                wxString lines = s.BeforeFirst(_T('l')).Strip(wxString::both);
                long line;
                if (!lines.ToLong(&line))
                    continue;
                while(i<n && line>0 && !output[i].StartsWith(_T("------------------")))
                {
                    cdata.message += output[i] + _T(" ");
                    ++i;
                    --line;
                }
                commits.push_back(cdata);
            }
        }
        if (m_opts.commits_per_retrieve > 0)
            m_continue_count++;
        else
            m_continue_count = 0;
        bool hg_not_done = false;
        if (m_repo_type == _T("Hg"))
        {
            long last;
            if (m_last_commit_retrieved == wxEmptyString)
                if (commits.size()>0)
                    m_last_commit_retrieved = commits.back().id;
            if (m_last_commit_retrieved.ToLong(&last))
                if (last > 0)
                    hg_not_done = true;
        }
        if (hg_not_done)
            m_retrieved_all = false;
        else if (commits.size() != 0)//== m_opts.commits_per_retrieve)
            m_retrieved_all = false;
        else
            m_retrieved_all = true;
        if (m_repo_type != _T("Hg"))
        {
            if (commits.size()>0)
            {
                m_last_commit_retrieved = commits.back().id;
            }
            else
            {
                m_last_commit_retrieved = wxEmptyString;
            }
        }
        if (m_opts.grep != wxEmptyString)
        {
            wxRegEx re(m_opts.grep, wxRE_ADVANCED|wxRE_ICASE);
            if (re.IsValid())
            {
                for (unsigned int i=0; i<commits.size(); ++i)
                {
                    if (re.Matches(commits[i].author) || re.Matches(commits[i].message))
                        m_commits.push_back(commits[i]);
                }
            }
        }
        else
            m_commits = commits;
        m_parent->AddPendingEvent(nce);
        return 0;
    }
    if (m_what.StartsWith(_T("DETAIL:")))
    {
        wxString commit = m_what.AfterFirst(_T(':'));
        wxArrayString output;
        if (m_repo_type == _T("GIT"))
        {
            Exec(_T("git log -1 --stat ")+commit,output, m_repo_path);
            for (unsigned int i=0; i<output.GetCount(); ++i)
                m_detailed_commit_log += output[i]+_T("\n");
        }
        if (m_repo_type == _T("Hg"))
        {
            Exec(_T("hg log -v -r")+commit,output, m_repo_path);
            for (unsigned int i=0; i<output.GetCount()-1; ++i) //skips last output line
                m_detailed_commit_log += output[i]+_T("\n");
        }
        if (m_repo_type == _T("BZR"))
        {
            Exec(_T("bzr log -v -r")+commit,output, m_repo_path);
            for (unsigned int i=1; i<output.GetCount(); ++i) //skips first output line cruft
                m_detailed_commit_log += output[i]+_T("\n");
        }
        if (m_repo_type == _T("SVN"))
        {
            Exec(_T("svn log -v -r")+commit,output, m_repo_path);
            for (unsigned int i=1; i<output.GetCount()-1; ++i) //skips first and last output line cruft
                m_detailed_commit_log += output[i]+_T("\n");
        }
        m_parent->AddPendingEvent(ne);
        return 0;

    }
    m_parent->AddPendingEvent(ne);
    return 0;
}
