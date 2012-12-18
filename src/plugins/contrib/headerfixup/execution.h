/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EXECUTION_H
#define EXECUTION_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//(*Headers(Execution)
#include "scrollingdialog.h"
class wxGauge;
class wxCheckBox;
class wxRadioBox;
class wxBoxSizer;
class wxButton;
class wxStaticBoxSizer;
class wxCheckListBox;
//*)

#include <wx/arrstr.h>
#include <wx/event.h>

#include <cbproject.h>

#include "bindings.h"
#include "fileanalysis.h"
#include "helper.h"

class wxWindow;
class wxString;

class Execution: public wxScrollingDialog
{
public:

  Execution(wxWindow* parent,wxWindowID id = -1);
  virtual ~Execution();

  //(*Identifiers(Execution)
  static const long ID_RB_SCOPE;
  static const long ID_RB_OPTIONS;
  static const long ID_CHK_IGNORE;
  static const long ID_CHK_FWD_DECL;
  static const long ID_CHK_OBSOLETE_LOG;
  static const long ID_RDO_FILE_TYPE;
  static const long ID_CHK_DEBUG_LOG;
  static const long ID_CHK_SIMULATION;
  static const long ID_LST_SETS;
  static const long ID_BTN_SELECT_ALL;
  static const long ID_BTN_SELECT_NONE;
  static const long ID_BTN_INVERT;
  static const long ID_GAU_PROGRESS;
  static const long ID_BTN_RUN;
  static const long ID_BTN_EXIT;
  //*)

protected:

  //(*Handlers(Execution)
  void OnBtnExitClick(wxCommandEvent& event);
  void OnBtnSelectAllClick(wxCommandEvent& event);
  void OnBtnSelectNoneClick(wxCommandEvent& event);
  void OnBtnInvertClick(wxCommandEvent& event);
  void OnBtnRunClick(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnChkSimulationClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Execution)
  wxCheckBox* m_Protocol;
  wxRadioBox* m_Scope;
  wxBoxSizer* sizRunExit;
  wxButton* m_Invert;
  wxStaticBoxSizer* sizExecute;
  wxCheckBox* m_Ignore;
  wxRadioBox* m_Options;
  wxButton* m_Exit;
  wxButton* m_SelectAll;
  wxButton* m_SelectNone;
  wxCheckBox* m_ObsoleteLog;
  wxGauge* m_Progress;
  wxButton* m_Run;
  wxRadioBox* m_FileType;
  wxCheckBox* m_Simulation;
  wxCheckBox* m_FwdDecl;
  wxCheckListBox* m_Sets;
  //*)

private:

  void LoadSettings();
  void SaveSettings();

  void ToggleControls(bool DoEnable);
  int RunScan(const wxArrayString& FilesToProcess, const wxArrayString& Groups);

  void AddFilesFromProject(wxArrayString& Files, cbProject* Project);
  int ProcessFile(const wxString& GlobalFileName,const wxArrayString& Groups);
  void OperateToken(const wxString&      Token,
                    const wxArrayString& Groups,
                    const wxArrayString& IncludedHeaders,
                    const wxArrayString& ExistingFwdDecls,
                    const wxChar&        Ch,
                    const wxString&      Line,
                    wxArrayString&       RequiredHeaders,
                    wxArrayString&       RequiredFwdDecls);

  DECLARE_EVENT_TABLE()

  enum EProcessor // NOTE (Morten#5#): Ensure this matches wxRadioBox* m_FileType;
  {
    ProcessHeaderFiles,
    ProcessSourceFiles
  };

  wxArrayString m_Log;
  wxArrayString m_TokensProcessed;
  Bindings      m_Bindings;
  FileAnalysis  m_FileAnalysis;
  bool          m_Execute;
  EProcessor    m_Processor;
};

#endif
