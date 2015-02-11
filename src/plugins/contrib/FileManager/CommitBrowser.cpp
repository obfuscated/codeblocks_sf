#include "CommitBrowser.h"
#include "FileExplorerUpdater.h"

//(*InternalHeaders(CommitBrowser)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(CommitBrowser)
//*)

BEGIN_EVENT_TABLE(CommitBrowser,wxDialog)
	//(*EventTable(CommitBrowser)
	//*)
    EVT_COMMAND(0, wxEVT_NOTIFY_UPDATE_COMPLETE, CommitBrowser::OnUpdateComplete)
    EVT_COMMAND(0, wxEVT_NOTIFY_COMMITS_UPDATE_COMPLETE, CommitBrowser::OnCommitsUpdateComplete)
    EVT_LIST_ITEM_SELECTED(XRCID("ID_COMMITLIST"), CommitBrowser::OnListItemSelected)
    EVT_CHOICE(XRCID("ID_BRANCH_CHOICE"), CommitBrowser::OnBranchSelected)
    EVT_CHECKBOX(XRCID("ID_CHECK_COMMIT_AFTER"), CommitBrowser::OnCheckCommitAfterDate)
    EVT_CHECKBOX(XRCID("ID_CHECK_COMMIT_BEFORE"), CommitBrowser::OnCheckCommitBeforeDate)
    EVT_CHECKBOX(XRCID("ID_CHECK_START_COMMIT"), CommitBrowser::OnCheckCommitStart)
    EVT_CHECKBOX(XRCID("ID_CHECK_END_COMMIT"), CommitBrowser::OnCheckCommitEnd)
END_EVENT_TABLE()

CommitBrowser::CommitBrowser(wxWindow* parent, const wxString& repo_path, const wxString &repo_type, const wxString &files)
{
	//(*Initialize(CommitBrowser)
	wxXmlResource::Get()->LoadObject(this,parent,_T("CommitBrowser"),_T("wxDialog"));
	StaticText1 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT1"));
	Choice1 = (wxChoice*)FindWindow(XRCID("ID_BRANCH_CHOICE"));
	CommitList = (wxListCtrl*)FindWindow(XRCID("ID_COMMITLIST"));
	TextCtrl1 = (wxTextCtrl*)FindWindow(XRCID("ID_COMMIT_MESSAGE"));
	ButtonMore = (wxButton*)FindWindow(XRCID("ID_BUTTON_MORE"));
	CommitStatus = (wxStaticText*)FindWindow(XRCID("ID_COMMIT_STATUS"));
	Button1 = (wxButton*)FindWindow(XRCID("ID_CANCEL"));
	Button2 = (wxButton*)FindWindow(XRCID("ID_BROWSE_COMMIT"));

    CheckStartCommit = (wxCheckBox*)FindWindow(XRCID("ID_CHECK_START_COMMIT"));
    StartCommit = (wxTextCtrl*)FindWindow(XRCID("ID_START_COMMIT"));
    CheckEndCommit = (wxCheckBox*)FindWindow(XRCID("ID_CHECK_END_COMMIT"));
    EndCommit = (wxTextCtrl*)FindWindow(XRCID("ID_END_COMMIT"));
    CheckAfterDate = (wxCheckBox*)FindWindow(XRCID("ID_CHECK_COMMIT_AFTER"));
    AfterDate = (wxDatePickerCtrl*)FindWindow(XRCID("ID_COMMIT_AFTER_DATE"));
    CheckBeforeDate = (wxCheckBox*)FindWindow(XRCID("ID_CHECK_COMMIT_BEFORE"));
    BeforeDate = (wxDatePickerCtrl*)FindWindow(XRCID("ID_COMMIT_BEFORE_DATE"));
    Grep = (wxTextCtrl*)FindWindow(XRCID("ID_GREP_ENTRY"));
    FileEntry = (wxTextCtrl*)FindWindow(XRCID("ID_FILE_ENTRY"));

	Connect(XRCID("ID_SEARCH_BUTTON"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommitBrowser::OnSearch);
	Connect(XRCID("ID_CANCEL"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommitBrowser::OnButton1Click);
	Connect(XRCID("ID_BROWSE_COMMIT"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommitBrowser::OnButton2Click);
	Connect(XRCID("ID_BUTTON_MORE"),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommitBrowser::OnButtonMore);
	//*)


    m_autofetch_count = 0;
	m_repo_path = repo_path;
	m_repo_type = repo_type;

	CommitList->InsertColumn(0,_T("ID"));
	CommitList->InsertColumn(1,_T("Author"));
	CommitList->InsertColumn(2,_T("Date"));
	CommitList->InsertColumn(3,_T("Message"));
    ButtonMore->Disable();
    Button2->Disable();
	TextCtrl1->Clear();
	this->SetSize(600, 500);
	//TODO: The amount fetched should be reduced if fetching over a network
	m_rev_fetch_amt[_T("SVN")] = 20;
	m_rev_fetch_amt[_T("GIT")] = 100;
	m_rev_fetch_amt[_T("BZR")] = 100;
	m_rev_fetch_amt[_T("Hg")] = 100;
	FileEntry->SetValue(files);

    m_updater_commits = new CommitUpdater(this, m_repo_path, m_repo_type);
	m_updater = new CommitUpdater(this, m_repo_path, m_repo_type);
	m_updater->Update(_T("BRANCHES"));
}

CommitBrowser::~CommitBrowser()
{
	//(*Destroy(CommitBrowser)
	//*)
}

wxString CommitBrowser::GetRepoBranch()
{
    if (Choice1->GetSelection()>=0)
        return Choice1->GetString(Choice1->GetSelection());
    else
        return wxEmptyString;
}


wxString CommitBrowser::GetSelectedCommit()
{
    long item = CommitList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item>=0)
    {
        wxListItem li;
        li.SetId(item);
        li.SetColumn(0);
        CommitList->GetItem(li);
        return li.GetText();
    }
    return wxEmptyString;
}

void CommitBrowser::OnSearch(wxCommandEvent& /*event*/)
{
    wxString br = GetRepoBranch();
    CommitsUpdaterQueue(_T("COMMITS:")+br);
    CommitList->DeleteAllItems();
    TextCtrl1->Clear();
    Button2->Disable();
    ButtonMore->Disable();
    CommitStatus->SetLabel(_T("Loading commits..."));
}

void CommitBrowser::OnButton1Click(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void CommitBrowser::OnButton2Click(wxCommandEvent& /*event*/)
{
    EndModal(wxID_OK);
}

void CommitBrowser::OnCheckCommitAfterDate(wxCommandEvent &event)
{
    AfterDate->Enable(event.GetInt());
}

void CommitBrowser::OnCheckCommitBeforeDate(wxCommandEvent &event)
{
    BeforeDate->Enable(event.GetInt());
}

void CommitBrowser::OnCheckCommitStart(wxCommandEvent &event)
{
    StartCommit->Enable(event.GetInt());
}

void CommitBrowser::OnCheckCommitEnd(wxCommandEvent &event)
{
    EndCommit->Enable(event.GetInt());
}

void CommitBrowser::OnButtonMore(wxCommandEvent& /*event*/)
{
    CommitsUpdaterQueue(_T("CONTINUE"));
    ButtonMore->Disable();
    CommitStatus->SetLabel(_T("Loading commits..."));
}

void CommitBrowser::CommitsUpdaterQueue(const wxString &cmd)
{
    if (m_updater_commits->IsRunning())
        m_update_commits_queue = cmd;
    else
    {
        m_update_commits_queue = wxEmptyString;
        CommitUpdater* cu = m_updater_commits;
        if (cmd == _T("CONTINUE"))
        {
            m_updater_commits = new CommitUpdater(*m_updater_commits);
            m_updater_commits->UpdateContinueCommitRetrieve();
        }
        else
        {
            m_autofetch_count = 0;
            m_updater_commits = new CommitUpdater(this, m_repo_path, m_repo_type);
            m_updater_commits->Update(cmd, GetRepoBranch(), GetCommitOptions());
        }
        delete cu;
    }
}

void CommitBrowser::OnListItemSelected(wxListEvent &event)
{
    wxListItem li = event.GetItem();
    wxString id = li.GetText();

    Button2->Enable();
    TextCtrl1->Clear();
    if (m_updater == 0 && id != wxEmptyString)
    {
        m_updater = new CommitUpdater(this, m_repo_path, m_repo_type);
        m_updater->Update(_T("DETAIL:")+id);
    }
    else
        m_update_queue = _T("DETAIL:")+id;
}

CommitUpdaterOptions CommitBrowser::GetCommitOptions()
{
    wxString e(wxEmptyString);
    return CommitUpdaterOptions(FileEntry->GetValue(),
                                Grep->GetValue(),
                                CheckStartCommit->IsChecked()? StartCommit->GetValue(): e,
                                CheckEndCommit->IsChecked()? EndCommit->GetValue(): e,
                                CheckBeforeDate->IsChecked()? wxString(BeforeDate->GetValue().FormatISODate()): e,
                                CheckAfterDate->IsChecked()? wxString(AfterDate->GetValue().FormatISODate()): e,
                                m_repo_type != wxEmptyString? m_rev_fetch_amt[m_repo_type]: 0
                                );
}

void CommitBrowser::OnBranchSelected(wxCommandEvent & /*event*/)
{
    wxString br = Choice1->GetString(Choice1->GetSelection());
    if (m_updater == 0)
    {
        m_updater = new CommitUpdater(this, m_repo_path, m_repo_type);
        m_updater->Update(_T("COMMITS:")+br);
    } else
        m_update_queue = _T("COMMITS:")+br;
    CommitList->DeleteAllItems();
    ButtonMore->Disable();
    Button2->Disable();
    CommitStatus->SetLabel(_T("Loading commits..."));
}

void CommitBrowser::OnCommitsUpdateComplete(wxCommandEvent& /*event*/)
{
    if (m_updater_commits == 0)
        return;
    m_updater_commits->Wait();
    if(m_updater_commits->m_what.StartsWith(_T("COMMITS:")))
    {
        wxString branch = m_updater_commits->m_what.AfterFirst(_T(':'));
        m_autofetch_count += m_updater_commits->m_commits.size();
        for (unsigned int i = 0; i<m_updater_commits->m_commits.size(); ++i)
        {
            CommitEntry ce = m_updater_commits->m_commits[i];
            wxListItem li;
            li.SetId(CommitList->GetItemCount());
            int pos = CommitList->InsertItem(li);
            CommitList->SetItem(pos, 0, ce.id);
            CommitList->SetItem(pos, 1, ce.author);
            CommitList->SetItem(pos, 2, ce.date);
            CommitList->SetItem(pos, 3, ce.message);
        }
        CommitList->SetColumnWidth(2,wxLIST_AUTOSIZE);
        CommitList->SetColumnWidth(3,wxLIST_AUTOSIZE);
        if (m_updater_commits->m_retrieved_all)
        {
            m_autofetch_count = 0;
            ButtonMore->Disable();
        }
        else
        {
            if (m_autofetch_count < m_rev_fetch_amt[m_repo_type])
            {
                CommitsUpdaterQueue(_T("CONTINUE"));
            }
            else
            {
                m_autofetch_count = 0;
                ButtonMore->Enable();
            }
        }
        if (CommitList->GetItemCount()!=1)
            CommitStatus->SetLabel(wxString::Format(_T("Showing %i commits"),CommitList->GetItemCount()));
        else
            CommitStatus->SetLabel(_T("Showing 1 commit"));
    }

    if (m_update_commits_queue != wxEmptyString)
        CommitsUpdaterQueue(m_update_commits_queue);
}
void CommitBrowser::OnUpdateComplete(wxCommandEvent& /*event*/)
{
    if (m_updater == 0)
        return;
    m_updater->Wait();
    if(m_updater->m_what == _T("BRANCHES"))
    {
        if (m_updater->m_branches.GetCount()==0)
        {
            delete m_updater;
            m_updater = 0;
            return;
        }
        for (unsigned int i = 0; i<m_updater->m_branches.GetCount(); ++i)
        {
            Choice1->Append(m_updater->m_branches[i]);
        }
        Choice1->Select(0);
        CommitsUpdaterQueue(_T("COMMITS:")+m_updater->m_branches[0]);
    }
    else if(m_updater->m_what.StartsWith(_T("DETAIL:")))
    {
        wxString commit = m_updater->m_what.AfterFirst(_T(':'));
        TextCtrl1->Clear();
        TextCtrl1->SetValue(m_updater->m_detailed_commit_log);
    }
    delete m_updater;
    m_updater = 0;
    if (m_update_queue != wxEmptyString)
    {
        m_updater = new CommitUpdater(this, m_repo_path, m_repo_type);
        m_updater->Update(m_update_queue);
        m_update_queue = wxEmptyString;
    }
}
