#ifndef FILEEXPLORERUPDATER_H_INCLUDED
#define FILEEXPLORERUPDATER_H_INCLUDED

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/thread.h>
#include <wx/process.h>

#include <queue>
#include <vector>
class VCSstatearray;
class FileExplorer;
class wxStringOutputStream;

// SIMPLE wxCommandEvent DERIVED CUSTOM EVENTS THAT USE THE BUILTIN EVT_COMMAND EVENT TABLE ENTRY
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_EXEC_REQUEST, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_COMPLETE, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_LOADER_UPDATE_COMPLETE, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_COMMITS_UPDATE_COMPLETE, -1)
END_DECLARE_EVENT_TYPES()


struct CommitEntry
{
    wxString id;
    wxString author;
    wxString message;
    wxString date;
};

struct FileData
{
    wxString name;
    int state;
    // could also add full path, modified time
};

typedef std::vector<FileData> FileDataVec;

class Updater:public wxEvtHandler, public wxThread
{
public:
    Updater() : wxThread(wxTHREAD_JOINABLE)
    {
        m_kill=false;
        m_exec_mutex=NULL;
        m_exec_proc=NULL;
        m_exec_stream=NULL;
        m_exec_sstream=NULL;
        m_exec_timer=NULL;
        m_cancelled = false;
    }
    ~Updater();
    bool m_cancelled;
protected:
    // Thread and process handling -- generic stuff
    virtual ExitCode Entry() = 0;
    int Exec(const wxString &command, wxArrayString &output, const wxString &path = wxEmptyString);
    int Exec(const wxString &command, wxString &output, const wxString &path = wxEmptyString);
    wxMutex *m_exec_mutex;
    wxCondition *m_exec_cond;
    wxProcess *m_exec_proc;
    wxInputStream *m_exec_stream;
    wxStringOutputStream *m_exec_sstream;
    int m_exec_proc_id;
    wxTimer *m_exec_timer;
    wxString m_exec_cmd;
    wxString m_exec_path;
    bool m_kill;
    wxString m_exec_output;
private:
    void ReadStream(bool all=false);
    void OnExecMain(wxCommandEvent &event);
    void OnExecTerminate(wxProcessEvent &e);
    void OnExecTimer(wxTimerEvent &e);
    DECLARE_EVENT_TABLE()
};

class FileExplorerUpdater: public Updater
{
public:
    FileExplorerUpdater(FileExplorer *fe) : Updater()
    {
        m_fe=fe;
    }
    ~FileExplorerUpdater() {}
    FileDataVec m_adders;
    FileDataVec m_removers;
    wxString m_vcs_type;
    wxString m_vcs_commit_string;
    bool m_vcs_changes_only;
    void Update(const wxTreeItemId &ti); //call on main thread to do the background magic
private:
    //FileExplorerUpdater specific code
    virtual ExitCode Entry(); //Entry point of the thread
    FileExplorer *m_fe;
    FileDataVec m_treestate;
    FileDataVec m_currentstate;
    wxString m_path;
    wxString m_repo_path;
    wxString m_wildcard;
    bool ParseGITstate(const wxString &path, VCSstatearray &sa);
    bool ParseBZRstate(const wxString &path, VCSstatearray &sa);
    bool ParseHGstate(const wxString &path, VCSstatearray &sa);
    bool ParseCVSstate(const wxString &path, VCSstatearray &sa);
    bool ParseSVNstate(const wxString &path, VCSstatearray &sa);
    bool GetGITCommitState(const wxString &path);
    bool GetHgCommitState(const wxString &path);
    bool GetVCSCommitState(const wxString &path, const wxString &cmd);
    void GetTreeState(const wxTreeItemId &ti);
    bool GetCurrentState(const wxString &path);
    bool CalcChanges(); //creates the vector of adders and removers
};

struct LoaderQueueItem
{
    wxString op;
    wxString source;
    wxString destination;
    wxString comp_commit;
};

class LoaderQueue: public std::queue<LoaderQueueItem>
{
public:
    void Add(const LoaderQueueItem &item) { push(item); }
    void Add(const wxString &op, const wxString &source, const wxString &destination, const wxString &comp_commit = wxEmptyString) { LoaderQueueItem item; item.op = op; item.source = source; item.destination = destination; item.comp_commit = comp_commit; push(item);}
    LoaderQueueItem Pop() { LoaderQueueItem it = front(); pop(); return it; }
};

class VCSFileLoader: public Updater
{
public:
    VCSFileLoader(FileExplorer *fe) : Updater()
    {
        m_fe=fe;
    }
    void Update(const wxString &op, const wxString &source_path, const wxString &destination_path, //call on main thread to do the background magic
                    const wxString &comp_commit);
    wxString m_source_path;
    wxString m_destination_path;
private:
    virtual ExitCode Entry(); //Entry point of the thread
    wxString m_vcs_type;
    wxString m_vcs_commit_string;
    wxString m_vcs_comp_commit;
    wxString m_vcs_op;
    FileExplorer *m_fe;
    wxString m_repo_path;
};

class CommitUpdaterOptions
{
public:
    CommitUpdaterOptions() {}
    CommitUpdaterOptions(const wxString &grep0, const wxString &start_commit0, const wxString &end_commit0,
                       const wxString &date_before0, const wxString &date_after0, long commits_per_retrieve0)
    {
        grep = wxString(grep0.c_str());
        start_commit = wxString(start_commit0.c_str());
        end_commit = wxString(end_commit0.c_str());
        date_before = wxString(date_before0.c_str());
        date_after = wxString(date_after0.c_str());
        commits_per_retrieve = commits_per_retrieve0;
    }
    wxString grep;
    wxString date_before;
    wxString date_after;
    wxString start_commit;
    wxString end_commit;
    unsigned long commits_per_retrieve;
};

class CommitUpdater: public Updater
{
public:
    CommitUpdater(wxEvtHandler *parent, const wxString& repo_path, const wxString &repo_type) : Updater()
    {
        m_parent = parent;
        m_repo_path = wxString(repo_path.c_str());
        m_repo_type = repo_type;
    }
    CommitUpdater(const CommitUpdater& updater);
    ~CommitUpdater() {}

    //call on main thread to do the background magic
    //what can be "BRANCHES", "COMMITS:<BRANCHNAME>", "DETAIL:<COMMITID>"
    bool Update(const wxString &what, const wxString &repo_branch = wxEmptyString,
                CommitUpdaterOptions opts = CommitUpdaterOptions());
    bool UpdateContinueCommitRetrieve();

    //Inputs
    CommitUpdaterOptions m_opts;
    wxString m_what;
    wxString m_repo_path;
    wxString m_repo_type;
    wxString m_repo_branch;
    wxEvtHandler *m_parent;

    //Outputs
    wxString m_detailed_commit_log; //the detailed commit log (if what == "DETAIL:")
    bool m_retrieved_all; // false if there are more commits to retrieve (if what == "COMMITS:")
    std::vector<CommitEntry> m_commits;
    wxArrayString m_branches;

private:
    wxString m_last_commit_retrieved; // the ID of the last commit retrieved (only filled if what == "COMMIT:")
    long m_continue_count;

    virtual ExitCode Entry(); //Entry point of the thread
};

#endif //FILEEXPLORERUPDATER_H_INCLUDED
