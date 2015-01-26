#ifndef FILEEXPLORERUPDATER_H_INCLUDED
#define FILEEXPLORERUPDATER_H_INCLUDED

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/thread.h>
#include <wx/process.h>


#include <vector>
class VCSstatearray;
class FileExplorer;

BEGIN_DECLARE_EVENT_TYPES()
// SIMPLE wxCommandEvent DERIVED CUSTOM EVENTS THAT USE THE BUILTIN EVT_COMMAND EVENT TABLE ENTRY
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_EXEC_REQUEST, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_TREE, -1)
END_DECLARE_EVENT_TYPES()


//WX_DEFINE_ARRAY_INT(int, wxArrayInt);


struct FileData
{
    wxString name;
    int state;
    // could also add full path, modified time
};

typedef std::vector<FileData> FileDataVec;

class FileExplorerUpdater: public wxEvtHandler, public wxThread
{
public:
    FileExplorerUpdater(FileExplorer *fe) : wxThread(wxTHREAD_JOINABLE)
    {
        m_kill=false;
        m_fe=fe;
        m_exec_proc=NULL;
        return;
    }
    ~FileExplorerUpdater();
    FileDataVec m_adders;
    FileDataVec m_removers;
    void Update(const wxTreeItemId &ti); //call on main thread to do the background magic
    void ExecMain();
    void OnExecTerminate(wxProcessEvent &e);
    void OnExecTimer(wxTimerEvent &e);
private:
    FileExplorer *m_fe;
    FileDataVec m_treestate;
    FileDataVec m_currentstate;
    wxMutex *m_exec_mutex;
    wxCondition *m_exec_cond;
    wxProcess *m_exec_proc;
    wxInputStream *m_exec_stream;
    int m_exec_proc_id;
    wxTimer *m_exec_timer;
    wxString m_exec_cmd;
    wxString m_path;
    wxString m_wildcard;
    wxArrayString m_exec_output;
    virtual ExitCode Entry();
    bool ParseGITstate(const wxString &path, VCSstatearray &sa);
    bool ParseBZRstate(const wxString &path, VCSstatearray &sa);
    bool ParseHGstate(const wxString &path, VCSstatearray &sa);
    bool ParseCVSstate(const wxString &path, VCSstatearray &sa);
    bool ParseSVNstate(const wxString &path, VCSstatearray &sa);
    int Exec(const wxString &command, wxArrayString &output);
    void ReadStream(bool all=false);
    void GetTreeState(const wxTreeItemId &ti);
    bool GetCurrentState(const wxString &path);
    bool CalcChanges(); //creates the vector of adders and removers
    bool m_kill;
    DECLARE_EVENT_TABLE()
};


#endif //FILEEXPLORERUPDATER_H_INCLUDED
