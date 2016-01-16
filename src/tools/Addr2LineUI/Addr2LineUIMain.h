#ifndef ADDR2LINEUIMAIN_H
#define ADDR2LINEUIMAIN_H

//(*Headers(Addr2LineUIDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include <wx/arrstr.h>
#include <wx/fileconf.h>
#include <wx/string.h>

class Addr2LineUIDialog: public wxDialog
{
public:

  Addr2LineUIDialog(wxWindow* parent);
  virtual ~Addr2LineUIDialog();

private:

  //(*Handlers(Addr2LineUIDialog)
  void OnQuit(wxCommandEvent& event);
  void OnCrashLogFile(wxFileDirPickerEvent& event);
  void OnAddr2LineFile(wxFileDirPickerEvent& event);
  void OnDirPrependDir(wxFileDirPickerEvent& event);
  void OnOperateClick(wxCommandEvent& event);
  void OnReplaceClick(wxCommandEvent& event);
  //*)

  //(*Identifiers(Addr2LineUIDialog)
  static const long ID_CRASH_LOG;
  static const long ID_ADDR2LINE;
  static const long ID_DIR_PREPEND;
  static const long ID_CHK_REPLACE;
  static const long ID_TXT_REPLACE_THIS;
  static const long ID_LBL_REPLACE;
  static const long ID_TXT_REPLACE_THAT;
  static const long ID_CHK_SKIP_UNRESOLVABLE;
  static const long ID_TXT_CRASH_LOG_CONTENT;
  static const long ID_TXT_RESULT;
  static const long ID_BTN_OPERATE;
  static const long ID_BTN_QUIT;
  //*)

  //(*Declarations(Addr2LineUIDialog)
  wxDirPickerCtrl* m_DPDirPrepend;
  wxFilePickerCtrl* m_FPCrashLog;
  wxFilePickerCtrl* m_FPAddr2Line;
  wxTextCtrl* txtResult;
  wxTextCtrl* txtCrashLogContent;
  wxCheckBox* chkReplace;
  wxTextCtrl* txtReplaceThis;
  wxButton* btnOperate;
  wxCheckBox* chkSkipUnresolvable;
  wxStaticText* lblReplace;
  wxTextCtrl* txtReplaceThat;
  //*)

  wxFileConfig  mFileConfig;
  wxString      mCrashLog;
  wxArrayString mCrashLogFileContent;
  wxString      mAddr2Line;
  wxString      mDirPrepend;

  DECLARE_EVENT_TABLE()
};

#endif // ADDR2LINEUIMAIN_H
