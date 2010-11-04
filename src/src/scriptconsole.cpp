/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

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

//(*InternalHeaders(ScriptConsole)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(ScriptConsole)
const long ScriptConsole::ID_TEXTCTRL1 = wxNewId();
const long ScriptConsole::ID_STATICTEXT1 = wxNewId();
const long ScriptConsole::ID_COMBOBOX1 = wxNewId();
const long ScriptConsole::ID_BITMAPBUTTON1 = wxNewId();
const long ScriptConsole::ID_BITMAPBUTTON2 = wxNewId();
const long ScriptConsole::ID_BITMAPBUTTON3 = wxNewId();
const long ScriptConsole::ID_PANEL1 = wxNewId();
//*)

static ScriptConsole* s_Console = 0;
static SQPRINTFUNCTION s_OldPrintFunc = 0;

static void ScriptConsolePrintFunc(HSQUIRRELVM /*v*/, const SQChar * s, ...)
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
	//*)
END_EVENT_TABLE()

ScriptConsole::ScriptConsole(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(ScriptConsole)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	txtConsole = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	wxFont txtConsoleFont(10,wxMODERN,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	txtConsole->SetFont(txtConsoleFont);
	BoxSizer1->Add(txtConsole, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	lblCommand = new wxStaticText(Panel1, ID_STATICTEXT1, _("Command:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(lblCommand, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	txtCommand = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN|wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_COMBOBOX1"));
	wxFont txtCommandFont(10,wxMODERN,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	txtCommand->SetFont(txtCommandFont);
	BoxSizer2->Add(txtCommand, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	btnExecute = new wxBitmapButton(Panel1, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	btnExecute->SetToolTip(_("Execute current command"));
	BoxSizer2->Add(btnExecute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnLoad = new wxBitmapButton(Panel1, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	btnLoad->SetDefault();
	btnLoad->SetToolTip(_("Load from file"));
	BoxSizer2->Add(btnLoad, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	btnClear = new wxBitmapButton(Panel1, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DELETE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
	btnClear->SetDefault();
	btnClear->SetToolTip(_("Clear output window"));
	BoxSizer2->Add(btnClear, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(Panel1);
	BoxSizer2->SetSizeHints(Panel1);
	BoxSizer1->Add(Panel1, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(ID_COMBOBOX1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&ScriptConsole::OnbtnExecuteClick);
	Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ScriptConsole::OnbtnExecuteClick);
	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ScriptConsole::OnbtnLoadClick);
	Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ScriptConsole::OnbtnClearClick);
	//*)

	txtCommand->Append(wxEmptyString);
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

    //(*Destroy(ScriptConsole)
	//*)
}

void ScriptConsole::Log(const wxString& msg)
{
    txtConsole->AppendText(msg);
    if (msg.Last() != _T('\n'))
        txtConsole->AppendText(_T('\n'));
//	txtConsole->ScrollLines(-1);
	Manager::ProcessPendingEvents();
}

void ScriptConsole::OnbtnExecuteClick(wxCommandEvent& /*event*/)
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
    {
    	if (txtCommand->FindString(cmd) == wxNOT_FOUND)
			txtCommand->Insert(cmd, 1); // right after the blank entry
        txtCommand->SetValue(wxEmptyString);
    }
    else
        txtConsole->AppendText(Manager::Get()->GetScriptingManager()->GetErrorString());
	txtCommand->SetFocus();
}

void ScriptConsole::OnbtnLoadClick(wxCommandEvent& /*event*/)
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
    wxString path = mgr->Read(_T("/file_dialogs/file_run_script/directory"), wxEmptyString);
    wxFileDialog dlg(this,
                     _("Load script"),
                     path,
                     wxEmptyString,
                     _T("Script files (*.script)|*.script"),
                     wxFD_OPEN | compatibility::wxHideReadonly);
    if (dlg.ShowModal() == wxID_OK)
    {
        mgr->Write(_T("/file_dialogs/file_run_script/directory"), dlg.GetDirectory());
        if (Manager::Get()->GetScriptingManager()->LoadScript(dlg.GetPath()))
            Log(_("Loaded succesfully"));
        else
        {
            Log(_("Failed..."));
            txtConsole->AppendText(Manager::Get()->GetScriptingManager()->GetErrorString());
        }
    }
	txtCommand->SetFocus();
}

void ScriptConsole::OnbtnClearClick(wxCommandEvent& /*event*/)
{
    txtConsole->Clear();
	txtCommand->SetFocus();
}
