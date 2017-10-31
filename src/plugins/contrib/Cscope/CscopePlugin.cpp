#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/process.h>

    #include "cbeditor.h"
    #include "cbproject.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
#endif
#include <configurationpanel.h>
#include <cbstyledtextctrl.h>
#include "CscopePlugin.h"

#include "CscopeStatusMessage.h"
#include "CscopeConfig.h"
#include "CscopeView.h"
#include "CscopeTab.h"
#include "CscopeParserThread.h"
#include "CscopeProcess.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<CscopePlugin> reg(_T("Cscope"));

    const int idOnFindFunctionsCallingThisFunction = wxNewId();
    const int idOnFindFunctionsCalledByThisFuncion = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(CscopePlugin, cbPlugin)
    // add any events you want to handle here
END_EVENT_TABLE()

// constructor
CscopePlugin::CscopePlugin()
:m_cfg(0),
m_pProcess(0),
m_thrd(0)
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("Cscope.zip")))
        NotifyMissingFile(_T("Cscope.zip"));
    m_cfg = new CscopeConfig();
}

// destructor
CscopePlugin::~CscopePlugin(){}

void CscopePlugin::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    m_view = new CscopeView(m_cfg);

    //Creates log image
//    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
//    wxBitmap * bmp = new wxBitmap(cbLoadBitmap(prefix + _T("cscope.png"), wxBITMAP_TYPE_PNG));

    CodeBlocksLogEvent evt(cbEVT_ADD_LOG_WINDOW, m_view, _T("Cscope")/*, bmp*/);
    Manager::Get()->ProcessEvent(evt);

//    Connect(idOnFindSymbol, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
//	  Connect(idOnFindSymbol, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
//	  Connect(idOnFindGlobalDefinition, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
//	  Connect(idOnFindGlobalDefinition, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
	Connect(idOnFindFunctionsCalledByThisFuncion, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
	Connect(idOnFindFunctionsCalledByThisFuncion, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
	Connect(idOnFindFunctionsCallingThisFunction, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
    Connect(idOnFindFunctionsCallingThisFunction, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);

	Connect(wxEVT_END_PROCESS, wxProcessEventHandler(CscopePlugin::OnCscopeReturned), NULL, this);
    Connect(wxEVT_IDLE, wxIdleEventHandler(CscopePlugin::OnIdle), NULL, this);

    Connect(wxEVT_CSCOPE_THREAD_DONE, wxCommandEventHandler(CscopePlugin::OnParserThreadEnded), NULL, this);

}

void CscopePlugin::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...


    CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_view);
    Manager::Get()->ProcessEvent(evt);


//    Disconnect(idOnFindSymbol, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
//	  Disconnect(idOnFindSymbol, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
//	  Disconnect(idOnFindGlobalDefinition, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
//	  Disconnect(idOnFindGlobalDefinition, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
	Disconnect(idOnFindFunctionsCalledByThisFuncion, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
	Disconnect(idOnFindFunctionsCalledByThisFuncion, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);
	Disconnect(idOnFindFunctionsCallingThisFunction, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CscopePlugin::OnFind), NULL, (wxEvtHandler*)this);
    Disconnect(idOnFindFunctionsCallingThisFunction, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopePlugin::OnCscopeUI), NULL, (wxEvtHandler*)this);

    Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(CscopePlugin::OnCscopeReturned), NULL, this);
    Disconnect(wxEVT_IDLE,          wxIdleEventHandler(CscopePlugin::OnIdle), NULL, this);

    Disconnect(wxEVT_CSCOPE_THREAD_DONE, wxCommandEventHandler(CscopePlugin::OnParserThreadEnded), NULL, this);


    if ( m_thrd )
    {
        if ( appShutDown )
            m_thrd->Kill();
        else
            m_thrd->Delete();
        m_thrd = NULL;
    }
    else
    {
        if ( m_pProcess && appShutDown )
            m_pProcess->Detach();
    }

}


void CscopePlugin::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* /*data*/)
{
    if ( !IsAttached() || m_pProcess) return;

    if(type != mtEditorManager || !menu ) return;

    EditorManager* emngr = Manager::Get()->GetEditorManager();
    if ( !emngr ) return;

    EditorBase *edb = emngr->GetActiveEditor();
    if ( !edb || !edb->IsBuiltinEditor() ) return;

    cbStyledTextCtrl* stc = ((cbEditor*)edb)->GetControl();
    if ( !stc ) return;

    if ( stc->GetLexer()  != wxSCI_LEX_CPP) return;

    wxString word = GetWordAtCaret();
    if ( word.IsEmpty() ) return;


    // Looks after the "Find implementation of:" menu item
    const wxMenuItemList ItemsList = menu->GetMenuItems();
    int idximp=-1;
    int idxocc=-1;
    for (int idx = 0; idx < (int)ItemsList.GetCount(); ++idx)
    {
        #if wxCHECK_VERSION(3, 0, 0)
        if (ItemsList[idx]->GetItemLabelText().StartsWith(_("Find implementation of:")) )
        #else
        if (ItemsList[idx]->GetLabel().StartsWith(_("Find implementation of:")) )
        #endif
        {
            idximp = idx;
        }
        #if wxCHECK_VERSION(3, 0, 0)
        if (ItemsList[idx]->GetItemLabelText().StartsWith(_("Find occurrences of:")) )
        #else
        if (ItemsList[idx]->GetLabel().StartsWith(_("Find occurrences of:")) )
        #endif
        {
            idxocc = idx;
        }
    }

    if ( idxocc == -1 && idximp == -1 )
    {
        //for consistency, add a separator as the first item:
        menu->AppendSeparator();

        //menu->Append(idOnFindSymbol,                       _T("Find C symbol '") + word + _T("'"));
        //menu->Append(idOnFindGlobalDefinition,             _T("Find '") + word + _T("' global definition"));
        menu->Append(idOnFindFunctionsCalledByThisFuncion, _("Find functions called by '") + word + _T("'"));
        menu->Append(idOnFindFunctionsCallingThisFunction, _("Find functions calling '") + word + _T("'"));
    }
    else
    {
        if ( idxocc >= 0 ) // if find occurences
            idximp = idxocc;
        //menu->Insert(++idximp,idOnFindSymbol,                       _T("Find C symbol '") + word + _T("'"));
        //menu->Insert(++idximp,idOnFindGlobalDefinition,             _T("Find '") + word + _T("' global definition"));
        menu->Insert(++idximp,idOnFindFunctionsCalledByThisFuncion, _("Find functions called by '") + word + _T("'"));
        menu->Insert(++idximp,idOnFindFunctionsCallingThisFunction, _("Find functions calling '") + word + _T("'"));
    }
}

void CscopePlugin::MakeOutputPaneVisible()
{
    CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
    Manager::Get()->ProcessEvent(evtShow);
    CodeBlocksLogEvent event(cbEVT_SWITCH_TO_LOG_WINDOW, m_view);
    Manager::Get()->ProcessEvent(event);
}
void CscopePlugin::ClearOutputWindow()
{
    m_view->GetWindow()->Clear();
}
bool CscopePlugin::CreateListFile(wxString &list_file)
{
    cbProject *prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (! prj) return false;

    std::vector< wxFileName > files;
    m_view->GetWindow()->SetMessage(_("Creating file list..."), 5);

    for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
    {
        wxFileName fn( (*it)->file.GetFullPath() );

        // To prevent cscope from going into an infinite loop while parsing the file list,
        // only keep track of existing files (in case it was removed with an external tool like git).
        if (fn.IsFileReadable())
            files.push_back(fn);
    }

	//create temporary file and save the file-list there
	wxFileName projectfilename(prj->GetFilename());

	list_file = prj->GetBasePath() + projectfilename.GetName() + _T(".cscope_file_list");

	wxFFile file(list_file, _T("w+b"));
	if (!file.IsOpened())
	{
		wxLogMessage(_("Failed to open temporary file ") + list_file);
		list_file.Empty();
		return false;
	}

	//write the content of the files into the temporary file
	wxString content;
	for (size_t i=0; i< files.size(); i++)
		content << _T("\"") + files[i].GetFullPath() << _T("\"\n");

	file.Write( content );
	file.Flush();
	file.Close();
	return true;
}
void CscopePlugin::DoCscopeCommand(const wxString &cmd, const wxString &endMsg)
{
    ClearOutputWindow();
	MakeOutputPaneVisible();
	m_CscouptOutput.clear();

    cbProject * prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    wxString path;
    if ( prj )
        path = prj->GetBasePath();

    Manager::Get()->GetLogManager()->Log(cmd);
    m_EndMsg = endMsg;

    if ( m_pProcess ) return;


    wxString curDir = wxGetCwd();
	wxSetWorkingDirectory(path);
    //set environment variables for cscope
	wxSetEnv(_T("TMPDIR"), _T("."));

    m_view->GetWindow()->SetMessage(_T("Executing cscope..."), 10);

	m_pProcess = new CscopeProcess(this);
    if ( !wxExecute(cmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, m_pProcess) )
    {
        delete m_pProcess;
        m_pProcess = NULL;
        m_view->GetWindow()->SetMessage(_T("Error while calling cscope occurred!"), 0);
    }

    //set environment variables back
    Manager::Get()->GetLogManager()->Log(_T("cscope process started"));
    wxSetWorkingDirectory(curDir);
}
void CscopePlugin::OnCscopeReturned(wxProcessEvent & /*event*/)
{
    Manager::Get()->GetLogManager()->Log(_T("cscope returned"));
    if ( !m_pProcess )
        return;

    m_view->GetWindow()->SetMessage(_T("Parsing results..."), 50);
    Manager::Get()->GetLogManager()->Log(_T("Parsing results..."));

    while (m_pProcess->ReadProcessOutput())
        ;

    m_thrd = new CscopeParserThread(this, m_CscouptOutput);
    m_thrd->Create();
    m_thrd->Run();

    Manager::Get()->GetLogManager()->Log(_T("parser Thread started"));
}
void CscopePlugin::OnParserThreadEnded(wxCommandEvent &event)
{
    delete m_pProcess;
    m_pProcess = NULL;

    m_thrd = NULL;

	CscopeResultTable *result = (CscopeResultTable*)event.GetClientData();
	m_view->GetWindow()->SetMessage(m_EndMsg, 100);
	m_view->GetWindow()->BuildTable( result );
}
wxString CscopePlugin::GetWordAtCaret()
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

	if ( ed )
	{
	    cbStyledTextCtrl *control = ed->GetControl();
	    if (control)
	    {
            const int pos = control->GetCurrentPos();
            const int ws = control->WordStartPosition(pos, true);
            const int we = control->WordEndPosition(pos, true);

            return control->GetTextRange(ws, we);
	    }
	}
    return wxEmptyString;
}
void CscopePlugin::OnFind(wxCommandEvent &event)
{
    wxString WordAtCaret = GetWordAtCaret();
    if (WordAtCaret.IsEmpty()) return;

    wxString list_file, outputfilename;
    if ( !CreateListFile(list_file) ) return;

    wxString cmd( GetCscopeBinaryName() + _T(" ") + //_T(" -f ")  + reffilename +
                  _T(" -L") );
    wxString endMsg(_T("Results for: "));
    if ( event.GetId() == idOnFindFunctionsCallingThisFunction)
    {
        cmd += _T(" -3 ");
        endMsg += _T("find functions calling '") + WordAtCaret + _T("'");
    }
    else //if( event.GetId() == idOnFindFunctionsCalledByThisFuncion)
    {
        cmd += _T(" -2 ");
        endMsg += _T("find functions called by '") + WordAtCaret + _T("'");
    }
//    else if ( event.GetId() == idOnFindGlobalDefinition )
//    {
//        cmd += _T(" -1 ");
//        endMsg += _T("find '") + WordAtCaret + _T("' global definition");
//    }
//    else //idOnFindSymbol
//    {
//        cmd += _T(" -0 ");
//        endMsg += _T("find C symbol '") + WordAtCaret + _T("'");
//    }

    cmd += WordAtCaret + _T(" -i \"") + list_file + _T("\"");
    DoCscopeCommand(cmd, endMsg);
}
wxString CscopePlugin::GetCscopeBinaryName()
{
	return _T("cscope");
}
void CscopePlugin::OnCscopeUI(wxUpdateUIEvent &event)
{
	bool isEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() ? true : false;
	bool HasProjectsInWorkspace = Manager::Get()->GetProjectManager()->GetActiveProject() ? true : false;
	event.Enable(HasProjectsInWorkspace && isEditor && !m_pProcess);
}


//when program idle, read the output
void CscopePlugin::OnIdle(wxIdleEvent& event)
{
  if ( m_pProcess && m_pProcess->ReadProcessOutput() )
  {
    event.RequestMore();
  }
  event.Skip();
}

void CscopePlugin::OnProcessGeneratedOutputLine(const wxString &line)
{
    m_CscouptOutput.Add(line);
}
