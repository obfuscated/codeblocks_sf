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
const long Addr2LineUIDialog::ID_CRASH_LOG = wxNewId();
const long Addr2LineUIDialog::ID_ADDR2LINE = wxNewId();
const long Addr2LineUIDialog::ID_DIR_PREPEND = wxNewId();
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

Addr2LineUIDialog::Addr2LineUIDialog(wxWindow* parent) :
  mFileConfig(),
  mCrashLog(),
  mCrashLogFileContent(),
  mAddr2Line(wxT("addr2line")),
  mDirPrepend()
{
  //(*Initialize(Addr2LineUIDialog)
  wxStaticText* lblDirPrepend;
  wxBoxSizer* bszAddr2Line;
  wxButton* btnQuit;
  wxBoxSizer* bszMainV;
  wxStaticText* lblCrashLog;
  wxBoxSizer* bszReplace;
  wxBoxSizer* bszMainH;
  wxStaticText* lblAddr2Line;
  wxStaticLine* stlLine;

  Create(parent, wxID_ANY, _("Addr2LineUI"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
  bszMainH = new wxBoxSizer(wxHORIZONTAL);
  bszMainV = new wxBoxSizer(wxVERTICAL);
  bszAddr2Line = new wxBoxSizer(wxVERTICAL);
  lblCrashLog = new wxStaticText(this, wxID_ANY, _("Select crash log file:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
  bszAddr2Line->Add(lblCrashLog, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_FPCrashLog = new wxFilePickerCtrl(this, ID_CRASH_LOG, wxEmptyString, _("Select crash log"), _T("Report files (*.rpt)|*.rpt|Log files (*.log)|*.log|All files (*.*)|*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_CRASH_LOG"));
  bszAddr2Line->Add(m_FPCrashLog, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  lblAddr2Line = new wxStaticText(this, wxID_ANY, _("Select Addr2Line tool:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
  bszAddr2Line->Add(lblAddr2Line, 0, wxTOP|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_FPAddr2Line = new wxFilePickerCtrl(this, ID_ADDR2LINE, wxEmptyString, _("Select addr2line tool"), _T("Executables (*.exe)|*.exe|All files (*.*)|*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_ADDR2LINE"));
  bszAddr2Line->Add(m_FPAddr2Line, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  lblDirPrepend = new wxStaticText(this, wxID_ANY, _("(Optionally) Select directory to prepend:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
  bszAddr2Line->Add(lblDirPrepend, 0, wxTOP|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_DPDirPrepend = new wxDirPickerCtrl(this, ID_DIR_PREPEND, wxEmptyString, _("Select directory to prepend"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_DIR_PREPEND"));
  bszAddr2Line->Add(m_DPDirPrepend, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  bszMainV->Add(bszAddr2Line, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
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
  bszMainH->Fit(this);
  bszMainH->SetSizeHints(this);

  Connect(ID_CRASH_LOG,wxEVT_COMMAND_FILEPICKER_CHANGED,wxFileDirPickerEventHandler(Addr2LineUIDialog::OnCrashLogFile));
  Connect(ID_ADDR2LINE,wxEVT_COMMAND_FILEPICKER_CHANGED,wxFileDirPickerEventHandler(Addr2LineUIDialog::OnAddr2LineFile));
  Connect(ID_DIR_PREPEND,wxEVT_COMMAND_DIRPICKER_CHANGED,wxFileDirPickerEventHandler(Addr2LineUIDialog::OnDirPrependDir));
  Connect(ID_CHK_REPLACE,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(Addr2LineUIDialog::OnReplaceClick));
  Connect(ID_BTN_OPERATE,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(Addr2LineUIDialog::OnOperateClick));
  Connect(ID_BTN_QUIT,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(Addr2LineUIDialog::OnQuit));
  //*)

  mFileConfig.Read(wxT("CrashLog"),   &mCrashLog,   wxT("")); m_FPCrashLog->SetPath(mCrashLog);
  mFileConfig.Read(wxT("Addr2Line"),  &mAddr2Line,  wxT("")); m_FPAddr2Line->SetPath(mAddr2Line);
  mFileConfig.Read(wxT("DirPrepend"), &mDirPrepend, wxT("")); m_DPDirPrepend->SetPath(mDirPrepend);

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

void Addr2LineUIDialog::OnCrashLogFile(wxFileDirPickerEvent& event)
{
  mCrashLog = event.GetPath();

  wxTextFile file(mCrashLog);
  if (file.Open())
  {
    mCrashLogFileContent.Clear();
    txtCrashLogContent->Clear();
    for (wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine())
    {
      mCrashLogFileContent.Add(line);
      txtCrashLogContent->AppendText(line + wxT("\n"));
    }

    if (mCrashLogFileContent.Count()>0) btnOperate->Enable(); else btnOperate->Disable();
  }
  else
    wxMessageBox(wxT("Error: File could not be opened."), wxT("Addr2LineUI"), wxOK|wxICON_ERROR, this);
}

void Addr2LineUIDialog::OnAddr2LineFile(wxFileDirPickerEvent& event)
{
  mAddr2Line = event.GetPath();
}

void Addr2LineUIDialog::OnDirPrependDir(wxFileDirPickerEvent& event)
{
  mDirPrepend = event.GetPath();
}

void Addr2LineUIDialog::OnReplaceClick(wxCommandEvent& event)
{
  if (event.IsChecked())
  { txtReplaceThis->Enable();  lblReplace->Enable();  txtReplaceThat->Enable();  }
  else
  { txtReplaceThis->Disable(); lblReplace->Disable(); txtReplaceThat->Disable(); }
}

void Addr2LineUIDialog::OnOperateClick(wxCommandEvent& WXUNUSED(event))
{
  // Style w/  debug symbols:
  /*
004013AE 00000008 60000000 40166666  sample_d.exe!Function  [C:\Devel\CodeBlocks\MinGW\bin\DrMinGW\demo/sample.cpp @ 10]
	...

	static void Function(int i, double j, const char * pszString) {
>	    sscanf("12345", "%i", (int *)1);
	}

	...

00401CCE 00000004 40B33333 0028FF08  sample_d.exe!StaticMethod  [C:\Devel\CodeBlocks\MinGW\bin\DrMinGW\demo/sample.cpp @ 15]
	...
	struct Class {
	    static void StaticMethod(int i, float j) {
>	        Function(i * 2, j, "Hello");
	    }

	...
  */

  // Sytle w/o debug symbols:
  /*
004013AE 00000008 60000000 40166666  sample_r.exe!Function(int, double, char const*)
00401CCE 00000004 40B33333 0028FF08  sample_r.exe!Class::StaticMethod(int, float)
  */

  // The address element "XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX " needs to be removed later, compute size here.
  wxRegEx reAddr32(wxT("([A-Fa-f0-9]{8})"));  wxString tag32(wxT("AddrPC   Params"));         size_t len32 = 4*8 +4; // 4 times 8bit  address + 4 times space
  wxRegEx reAddr64(wxT("([A-Fa-f0-9]{16})")); wxString tag64(wxT("AddrPC           Params")); size_t len64 = 4*16+4; // 4 times 16bit address + 4 times space
  bool is64Bit = false;
  txtResult->Clear();

  txtResult->AppendText(wxT("Working directory is '") + wxGetCwd() + wxT("'.\n"));


  bool operate_line = false;
  for (size_t i=0; i<mCrashLogFileContent.Count(); i++)
  {
    wxString line = mCrashLogFileContent.Item(i);
    if (line.IsSameAs(tag32))
    {
      txtResult->AppendText(wxT("*************************************\n"));
      txtResult->AppendText(wxT("* Found (another) 32 bit call stack *\n"));
      txtResult->AppendText(wxT("*************************************\n"));
      operate_line = true;
      continue;
    }
    else if (line.IsSameAs(tag64))
    {
      txtResult->AppendText(wxT("*************************************\n"));
      txtResult->AppendText(wxT("* Found (another) 64 bit call stack *\n"));
      txtResult->AppendText(wxT("*************************************\n"));
      operate_line = true;
      is64Bit = true;
      continue;
    }

    if (!operate_line)
      continue;

    line = line.Trim(true).Trim(false);
    if (line.IsEmpty())
      continue;

    if (!is64Bit && !reAddr32.Matches(line) )
      continue;
    if ( is64Bit && !reAddr64.Matches(line) )
      continue;

    wxString theAddr;
    wxString theFile;


    // Obtain address
    if (is64Bit)
    {
      for (size_t j=1; j<=reAddr64.GetMatchCount(); j++)
      {
        switch (j)
        {
          case 1: { theAddr = reAddr64.GetMatch(line, 1); } break;
          case 2: // fall through
          case 3: // fall through
          case 4: // fall through
          default: break;
        }
      }
    }
    else // 32 bit
    {
      for (size_t j=1; j<=reAddr32.GetMatchCount(); j++)
      {
        switch (j)
        {
          case 1: { theAddr = reAddr32.GetMatch(line, 1); } break;
          case 2: // fall through
          case 3: // fall through
          case 4: // fall through
          default: break;
        }
      }
    }

    // Verify address
    if (theAddr.IsEmpty())
    {
      txtResult->AppendText(wxT("Skipping empty address '") + line + wxT("'\n"));
      continue;
    }

    // Remove "XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX "
    line = line.Right(line.Length() - (is64Bit ? len64 : len32)).Trim(true).Trim(false) ; // 8 times address + 8 times space
    wxString sep = wxT("!");
    int sep_pos = line.Find(wxT('!'), true);
    if (sep_pos!=wxNOT_FOUND)
      theFile = line.SubString(0,sep_pos-1);

    // Verify file
    if (theFile.IsEmpty())
    {
      txtResult->AppendText(wxT("Skipping address '") + theAddr + wxT("' for unknown file : '") + theFile + wxT("'\n"));
      continue;
    }

    // prepend directory if requested
    if (!mDirPrepend.IsEmpty())
      theFile = mDirPrepend + wxFILE_SEP_PATH + theFile;

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
        theFile.Replace(repl_this, repl_that);
      }
      else
      {
        wxMessageBox(wxT("Error: Invalid setup for replacements (would cause a loop)."), wxT("Addr2LineUI"), wxOK|wxICON_ERROR, this);
        chkReplace->SetValue(false);
      }
    }

    if ( !wxFileExists(theFile) )
    {
      txtResult->AppendText(wxT("Skipping non-existent file '") + theFile + wxT("'\n"));
      continue;
    }

    // apply file mask if needed
    if (theFile.Contains(wxT(" "))) theFile = wxT("\"") + theFile + wxT("\"");

    // compute (initial) command line argument to addr2line
    wxString command_args = wxT(" -C -e ") + theFile + wxT(" ") + theAddr;
    // Now prepend the addr2line tool and compile the full command
    wxString command      = mAddr2Line + command_args;

    { // BEGIN Lifetime of wxWindowDisabler and wxBusyInfo
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
          txtResult->AppendText(theFile + wxT("[") + theAddr + wxT("]:\n"));
          for (size_t j=0; j<output.Count(); j++)
            txtResult->AppendText(output.Item(j) + wxT("\n"));
          txtResult->AppendText(wxT("----------------------------------------\n"));
        }
      }
    }// END Lifetime of wxWindowDisabler and wxBusyInfo
  }// for
}

void Addr2LineUIDialog::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  mFileConfig.Write(wxT("CrashLog"),         mCrashLog                       );
  mFileConfig.Write(wxT("Addr2Line"),        mAddr2Line                      );
  mFileConfig.Write(wxT("DirPrepend"),       mDirPrepend                     );
  mFileConfig.Write(wxT("Replace"),          chkReplace->IsChecked()         );
  mFileConfig.Write(wxT("ReplaceThis"),      txtReplaceThis->GetValue()      );
  mFileConfig.Write(wxT("ReplaceThat"),      txtReplaceThat->GetValue()      );
  mFileConfig.Write(wxT("SkipUnresolvable"), chkSkipUnresolvable->IsChecked());

  EndModal(wxID_OK);
  Destroy();
}
