#include <sdk.h>
#include <sqplus.h>

#include "scriptconsole.h"

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <manager.h>
    #include <configmanager.h>
    #include <scriptingmanager.h>
#endif

#include <wx/filedlg.h>

static ScriptConsole* s_Console = 0;
static SQPRINTFUNCTION s_OldPrintFunc = 0;

static void ScriptConsolePrintFunc(HSQUIRRELVM v, const SQChar * s, ...)
{
    static SQChar temp[2048];
    va_list vl;
    va_start(vl,s);
    scvsprintf( temp,s,vl);
    wxString msg = cbC2U(temp);
    va_end(vl);

    if (s_Console)
        s_Console->Log(msg);
    Manager::Get()->GetScriptingManager()->InjectScriptOutput(msg);
}

BEGIN_EVENT_TABLE(ScriptConsole,wxPanel)
	//(*EventTable(ScriptConsole)
	EVT_TEXT_ENTER(ID_TEXTCTRL2,ScriptConsole::OnbtnExecuteClick)
	EVT_BUTTON(ID_BUTTON1,ScriptConsole::OnbtnExecuteClick)
	EVT_BUTTON(ID_BUTTON2,ScriptConsole::OnbtnLoadClick)
	EVT_BUTTON(ID_BUTTON3,ScriptConsole::OnbtnClearClick)
	//*)
END_EVENT_TABLE()

ScriptConsole::ScriptConsole(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(ScriptConsole)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	txtConsole = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) txtConsole->SetMaxLength(0);
	Panel1 = new wxPanel(this,ID_PANEL1,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL1"));
	Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(Panel1,ID_STATICTEXT1,_("Command:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	txtCommand = new wxTextCtrl(Panel1,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL2"));
	if ( 0 ) txtCommand->SetMaxLength(0);
	btnExecute = new wxButton(Panel1,ID_BUTTON1,_("Execute"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON1"));
	if (false) btnExecute->SetDefault();
	btnLoad = new wxButton(Panel1,ID_BUTTON2,_("Load from file"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) btnLoad->SetDefault();
	btnClear = new wxButton(Panel1,ID_BUTTON3,_("Clear"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON3"));
	if (false) btnClear->SetDefault();
	BoxSizer2->Add(StaticText1,0,wxLEFT|wxRIGHT|wxALIGN_CENTER,4);
	BoxSizer2->Add(txtCommand,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer2->Add(btnExecute,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	BoxSizer2->Add(btnLoad,0,wxLEFT|wxALIGN_LEFT|wxALIGN_TOP,16);
	BoxSizer2->Add(btnClear,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	Panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(Panel1);
	BoxSizer2->SetSizeHints(Panel1);
	BoxSizer1->Add(txtConsole,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	BoxSizer1->Add(Panel1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	if (!s_Console)
	{
        s_Console = this;
        s_OldPrintFunc = sq_getprintfunc(SquirrelVM::GetVMPtr());
        sq_setprintfunc(SquirrelVM::GetVMPtr(), ScriptConsolePrintFunc);
	}

	Log(_("Welcome to the script console!"));
}

ScriptConsole::~ScriptConsole()
{
    if (s_Console == this)
    {
        s_Console = 0;
        if (SquirrelVM::GetVMPtr())
            sq_setprintfunc(SquirrelVM::GetVMPtr(), s_OldPrintFunc);
    }
}

void ScriptConsole::Log(const wxString& msg)
{
    txtConsole->AppendText(msg);
    if (msg.Last() != _T('\n'))
        txtConsole->AppendText(_T('\n'));
	txtConsole->ScrollLines(-1);
	Manager::ProcessPendingEvents();
}

void ScriptConsole::OnbtnExecuteClick(wxCommandEvent& event)
{
    wxString cmd = txtCommand->GetValue();
    cmd.Trim(false);
    cmd.Trim(true);
    if (cmd.IsEmpty())
    {
        wxBell();
        return;
    }

    Log(_T("> ") + cmd);
    if (Manager::Get()->GetScriptingManager()->LoadBuffer(cmd, _T("ScriptConsole")))
        txtCommand->Clear();
    else
        txtConsole->AppendText(Manager::Get()->GetScriptingManager()->GetErrorString());
}

void ScriptConsole::OnbtnLoadClick(wxCommandEvent& event)
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
    wxString path = mgr->Read(_T("/file_dialogs/file_run_script/directory"), wxEmptyString);
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Load script"),
                            path,
                            wxEmptyString,
                            _T("Script files (*.script)|*.script"),
                            wxOPEN
#if (WXWIN_COMPATIBILITY_2_4)
                            | wxHIDE_READONLY
#endif
                            );
    if (dlg->ShowModal() == wxID_OK)
    {
        mgr->Write(_T("/file_dialogs/file_run_script/directory"), dlg->GetDirectory());
        if (Manager::Get()->GetScriptingManager()->LoadScript(dlg->GetPath()))
        {
            Log(_("Loaded succesfully"));
        }
        else
        {
            Log(_("Failed..."));
            txtConsole->AppendText(Manager::Get()->GetScriptingManager()->GetErrorString());
        }
    }
    dlg->Destroy();
}

void ScriptConsole::OnbtnClearClick(wxCommandEvent& event)
{
    txtConsole->Clear();
}
