#include "Addr2LineUIMain.h"

//(*InternalHeaders(Addr2LineUIDialog)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/textfile.h>
#include <wx/utils.h> // wxExecute

//(*IdInit(Addr2LineUIDialog)
const long Addr2LineUIDialog::ID_TXT_CRASH_LOG = wxNewId();
const long Addr2LineUIDialog::ID_BTN_CRASH_LOG = wxNewId();
const long Addr2LineUIDialog::ID_TXT_ADDR2LINE = wxNewId();
const long Addr2LineUIDialog::ID_BTN_ADDR2LINE = wxNewId();
const long Addr2LineUIDialog::ID_CHK_REPLACE = wxNewId();
const long Addr2LineUIDialog::ID_TXT_REPLACE_THIS = wxNewId();
const long Addr2LineUIDialog::ID_LBL_REPLACE = wxNewId();
const long Addr2LineUIDialog::ID_TXT_REPLACE_THAT = wxNewId();
const long Addr2LineUIDialog::ID_CHK_SKIP_UNRESOLVABLE = wxNewId();
const long Addr2LineUIDialog::ID_TXT_CRASH_LOG_CONTENT = wxNewId();
const long Addr2LineUIDialog::ID_TXT_RESULT = wxNewId();
const long Addr2LineUIDialog::ID_BTN_OPERATE = wxNewId();
const long Addr2LineUIDialog::ID_BTN_QUIT = wxNewId();
//*)

BEGIN_EVENT_TABLE(Addr2LineUIDialog,wxDialog)
  //(*EventTable(Addr2LineUIDialog)
  //*)
END_EVENT_TABLE()

Addr2LineUIDialog::Addr2LineUIDialog(wxWindow* parent,wxWindowID id) :
  mFileConfig(),
  mCrashLogFile(),
  mCrashLogFileContent(),
  mAddr2Line(wxT("addr2line"))
{
  //(*Initialize(Addr2LineUIDialog)
  wxBoxSizer* bszAddr2Line;
  wxButton* btnQuit;
  wxBoxSizer* bszMainV;
  wxBoxSizer* bszReplace;
  wxBoxSizer* bszLogFile;
  wxButton* btnCrashLog;
  wxBoxSizer* bszMainH;
  wxButton* btnAddr2Line;
  wxStaticLine* stlLine;

  Create(parent, wxID_ANY, _("Addr2LineUI"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
  bszMainH = new wxBoxSizer(wxHORIZONTAL);
  bszMainV = new wxBoxSizer(wxVERTICAL);
  bszLogFile = new wxBoxSizer(wxHORIZONTAL);
  txtCrashLog = new wxTextCtrl(this, ID_TXT_CRASH_LOG, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_CRASH_LOG"));
  bszLogFile->Add(txtCrashLog, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  btnCrashLog = new wxButton(this, ID_BTN_CRASH_LOG, _("Crash log..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_CRASH_LOG"));
  bszLogFile->Add(btnCrashLog, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP, 5);
  bszMainV->Add(bszLogFile, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  bszAddr2Line = new wxBoxSizer(wxHORIZONTAL);
  txtAddr2Line = new wxTextCtrl(this, ID_TXT_ADDR2LINE, _("addr2line"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_ADDR2LINE"));
  bszAddr2Line->Add(txtAddr2Line, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  btnAddr2Line = new wxButton(this, ID_BTN_ADDR2LINE, _("addr2line"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_ADDR2LINE"));
  bszAddr2Line->Add(btnAddr2Line, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP, 5);
  bszMainV->Add(bszAddr2Line, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  bszReplace = new wxBoxSizer(wxHORIZONTAL);
  chkReplace = new wxCheckBox(this, ID_CHK_REPLACE, _("Replace:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_REPLACE"));
  chkReplace->SetValue(false);
  bszReplace->Add(chkReplace, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  txtReplaceThis = new wxTextCtrl(this, ID_TXT_REPLACE_THIS, _("this"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_REPLACE_THIS"));
  txtReplaceThis->Disable();
  bszReplace->Add(txtReplaceThis, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  lblReplace = new wxStaticText(this, ID_LBL_REPLACE, _("...with:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_REPLACE"));
  lblReplace->Disable();
  bszReplace->Add(lblReplace, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  txtReplaceThat = new wxTextCtrl(this, ID_TXT_REPLACE_THAT, _("that"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_REPLACE_THAT"));
  txtReplaceThat->Disable();
  bszReplace->Add(txtReplaceThat, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  chkSkipUnresolvable = new wxCheckBox(this, ID_CHK_SKIP_UNRESOLVABLE, _("Skip unresolvable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_SKIP_UNRESOLVABLE"));
  chkSkipUnresolvable->SetValue(false);
  bszReplace->Add(chkSkipUnresolvable, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  bszMainV->Add(bszReplace, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  txtCrashLogContent = new wxTextCtrl(this, ID_TXT_CRASH_LOG_CONTENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TXT_CRASH_LOG_CONTENT"));
  txtCrashLogContent->SetMinSize(wxSize(450,200));
  wxFont txtCrashLogContentFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
  if ( !txtCrashLogContentFont.Ok() ) txtCrashLogContentFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  txtCrashLogContent->SetFont(txtCrashLogContentFont);
  bszMainV->Add(txtCrashLogContent, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  txtResult = new wxTextCtrl(this, ID_TXT_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TXT_RESULT"));
  txtResult->SetMinSize(wxSize(450,200));
  wxFont txtResultFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
  if ( !txtResultFont.Ok() ) txtResultFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  txtResult->SetFont(txtResultFont);
  bszMainV->Add(txtResult, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  stlLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("wxID_ANY"));
  bszMainV->Add(stlLine, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
  btnOperate = new wxButton(this, ID_BTN_OPERATE, _("Operate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_OPERATE"));
  btnOperate->Disable();
  bszMainV->Add(btnOperate, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
  btnQuit = new wxButton(this, ID_BTN_QUIT, _("Quit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_QUIT"));
  bszMainV->Add(btnQuit, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
  bszMainH->Add(bszMainV, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
  SetSizer(bszMainH);
  fdCrashLog = new wxFileDialog(this, _("Select crash log"), wxEmptyString, wxEmptyString, _("Report files (*.rpt)|*.rpt|Log files (*.log)|*.log|All files (*.*)|*.*"), wxFD_DEFAULT_STYLE|wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
  fdAddr2Line = new wxFileDialog(this, _("Select addr2line tool"), wxEmptyString, _("addr2line"), _("Executables (*.exe)|*.exe|All files (*.*)|*.*"), wxFD_DEFAULT_STYLE|wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
  bszMainH->Fit(this);
  bszMainH->SetSizeHints(this);

  Connect(ID_BTN_CRASH_LOG,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Addr2LineUIDialog::OnFileClick);
  Connect(ID_BTN_ADDR2LINE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Addr2LineUIDialog::OnAddr2LineClick);
  Connect(ID_CHK_REPLACE,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&Addr2LineUIDialog::OnReplaceClick);
  Connect(ID_BTN_OPERATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Addr2LineUIDialog::OnOperateClick);
  Connect(ID_BTN_QUIT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Addr2LineUIDialog::OnQuit);
  //*)

  wxString CrashLog;         mFileConfig.Read(wxT("CrashLog"),         &CrashLog,         wxT("")); txtCrashLog->SetValue(CrashLog);
  wxString Addr2Line;        mFileConfig.Read(wxT("Addr2Line"),        &Addr2Line,        wxT("")); txtAddr2Line->SetValue(Addr2Line);
  bool     Replace;          mFileConfig.Read(wxT("Replace"),          &Replace,          false  ); chkReplace->SetValue(Replace);
  if (Replace) { txtReplaceThis->Enable(); lblReplace->Enable(); txtReplaceThat->Enable(); }
  wxString ReplaceThis;      mFileConfig.Read(wxT("ReplaceThis"),      &ReplaceThis,      wxT("")); txtReplaceThis->SetValue(ReplaceThis);
  wxString ReplaceThat;      mFileConfig.Read(wxT("ReplaceThat"),      &ReplaceThat,      wxT("")); txtReplaceThat->SetValue(ReplaceThat);
  bool     SkipUnresolvable; mFileConfig.Read(wxT("SkipUnresolvable"), &SkipUnresolvable, false  ); chkSkipUnresolvable->SetValue(SkipUnresolvable);
}

Addr2LineUIDialog::~Addr2LineUIDialog()
{
  //(*Destroy(Addr2LineUIDialog)
  //*)
}

void Addr2LineUIDialog::OnFileClick(wxCommandEvent& event)
{
  if (wxID_OK == fdCrashLog->ShowModal())
  {
    wxString full_path = fdCrashLog->GetPath();
    txtCrashLog->SetValue(full_path);
    mCrashLogFile = full_path;
    wxTextFile file(full_path);

    if (file.Open())
    {
      mCrashLogFileContent.Clear();
      txtCrashLogContent->Clear();
      for (wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine())
      {
        mCrashLogFileContent.Add(line);
        txtCrashLogContent->AppendText(line+wxT("\n"));
      }

      if (mCrashLogFileContent.Count()>0) btnOperate->Enable(); else btnOperate->Disable();
    }
    else
      wxMessageBox(wxT("Error: File could not be opened."), wxT("Addr2LineUI"));
  }
}

void Addr2LineUIDialog::OnAddr2LineClick(wxCommandEvent& event)
{
  if (wxID_OK == fdAddr2Line->ShowModal())
  {
    wxString full_path = fdAddr2Line->GetPath();
    txtAddr2Line->SetValue(full_path);
    mAddr2Line = full_path;
  }
}

void Addr2LineUIDialog::OnReplaceClick(wxCommandEvent& event)
{
  if (event.IsChecked())
  {
    txtReplaceThis->Enable(); lblReplace->Enable(); txtReplaceThat->Enable();
  }
  else
  {
    txtReplaceThis->Disable(); lblReplace->Disable(); txtReplaceThat->Disable();
  }
}

void Addr2LineUIDialog::OnOperateClick(wxCommandEvent& event)
{
  // wxRegEx re(wxT("([A-Fa-f0-9]{8})([ \\tA-Za-z0-9_:+/\\\\.-]+):([A-Fa-f0-9]{8})([ \\tA-Za-z0-9_:+/\\\\.-]+)"));
  wxRegEx reAddr(wxT("([A-Fa-f0-9]{8})"));
  wxRegEx reFull(wxT("([A-Fa-f0-9]{8})([ \\tA-Za-z0-9_:+/\\\\.-]+):"));
  txtResult->Clear();

  bool operate_line = false;
  for (size_t i=0; i<mCrashLogFileContent.Count(); i++)
  {
    wxString line = mCrashLogFileContent.Item(i);
    if (line.IsSameAs(wxT("Call stack:"), false))
    {
      txtResult->AppendText(wxT("******************************\n"));
      txtResult->AppendText(wxT("* Found (another) call stack *\n"));
      txtResult->AppendText(wxT("******************************\n"));
      operate_line = true;
      continue;
    }

    if (operate_line)
    {
      if (line.Trim(true).Trim(false).IsEmpty())
      {
        operate_line = false; // stop operation
      }
      else if (reFull.Matches(line))
      {
        wxString ADDR, FILE;
        for (size_t j=1; j<=reFull.GetMatchCount(); j++)
        {
          switch (j)
          {
            case 1: { ADDR = reFull.GetMatch(line, 1).Trim(true).Trim(false); } break;
            case 2: { FILE = reFull.GetMatch(line, 2).Trim(true).Trim(false); } break;
            default: break; // should never happen
          }
        }

        // verify content
        if (!ADDR.IsEmpty() && !FILE.IsEmpty())
        {
          // append mask if needed
          if (FILE.Contains(wxT(" "))) FILE = wxT("\"") + FILE + wxT("\"");

          // compute (initial) command line argument to addr2line
          wxString command_args = wxT(" -e ") + FILE + wxT(" ") + ADDR;

          // replacements in command (if any):
          if (chkReplace->IsChecked())
          {
            wxString repl_this = txtReplaceThis->GetValue();
            wxString repl_that = txtReplaceThat->GetValue();
            // avoid endless loops
            if (   !repl_this.IsSameAs(repl_that)
                && !(   repl_this.Trim(true).Trim(false).IsEmpty()
                     && repl_that.Trim(true).Trim(false).IsEmpty() )
                && !(repl_this.Trim(true).Trim(false).IsSameAs(
                       repl_that.Trim(true).Trim(false)) ) )
            {
              command_args.Replace(repl_this, repl_that);
            }
            else
            {
              wxMessageBox(wxT("Error: Invalid setup for replacements (would cause a loop)."), wxT("Addr2LineUI"));
              chkReplace->SetValue(false);
            }
          }

          // Now prepend the addr2line tool and compile the full command
          wxString command = mAddr2Line + command_args;

          { // Lifetime of wxWindowDisabler and wxBusyInfo
            wxWindowDisabler disableAll;
            wxBusyInfo       wait(wxT("Please wait, operating:\n") + command);

            wxArrayString output, error;
            long ret = wxExecute(command, output, error); // sync process
            if (ret==-1)
            {
              txtResult->AppendText(command + wxT(":\n"));
              txtResult->AppendText(wxT("-1 for: ") + command + wxT("\n"));
              txtResult->AppendText(wxT("----------------------------------------\n"));
            }
            else if (!error.IsEmpty())
            {
              txtResult->AppendText(command + wxT(":\n"));
              txtResult->AppendText(wxT("Error for: ") + command + wxT("\n:"));
              for (size_t j=0; j<error.Count(); j++)
                txtResult->AppendText(error.Item(j) + wxT("\n"));
              txtResult->AppendText(wxT("----------------------------------------\n"));
            }
            else
            {
              bool do_show = true;
              if (chkSkipUnresolvable->IsChecked())
              {
                if (output.Count()>0 && output.Item(0).Contains(wxT("??:0")))
                  do_show = false;
              }

              if (do_show)
              {
                txtResult->AppendText(command + wxT(":\n"));
                txtResult->AppendText(FILE + wxT("[") + ADDR + wxT("]:\n"));
                for (size_t j=0; j<output.Count(); j++)
                  txtResult->AppendText(output.Item(j) + wxT("\n"));
                txtResult->AppendText(wxT("----------------------------------------\n"));
              }
            }
          }
        }
      }
      else if (reAddr.Matches(line))
      {
        wxString ADDR;
        for (size_t j=1; j<=reAddr.GetMatchCount(); j++)
        {
          switch (j)
          {
            case 1: { ADDR = reAddr.GetMatch(line, 1).Trim(true).Trim(false); } break;
            default: break; // should never happen
          }
        }

        // verify content
        if (!ADDR.IsEmpty())
        {
          txtResult->AppendText(wxT("Skipping empty address : ") + ADDR + wxT("\n"));
        }
      }
      else // if (line.Trim(true).Trim(false).IsEmpty()); else if (reFull.Matches(line))
      {
        txtResult->AppendText(wxT("Stopping addr2line operations for line:\n") + line + wxT("\n"));
        txtResult->AppendText(wxT("----------------------------------------\n"));
        operate_line = false; // stop operation
      }
    }// if (operate_line)
  }
}

void Addr2LineUIDialog::OnQuit(wxCommandEvent& event)
{
  mFileConfig.Write(wxT("CrashLog"),         txtCrashLog->GetValue()         );
  mFileConfig.Write(wxT("Addr2Line"),        txtAddr2Line->GetValue()        );
  mFileConfig.Write(wxT("Replace"),          chkReplace->IsChecked()         );
  mFileConfig.Write(wxT("ReplaceThis"),      txtReplaceThis->GetValue()      );
  mFileConfig.Write(wxT("ReplaceThat"),      txtReplaceThat->GetValue()      );
  mFileConfig.Write(wxT("SkipUnresolvable"), chkSkipUnresolvable->IsChecked());

  Close();
}
