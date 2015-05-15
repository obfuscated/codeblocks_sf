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
#include <wx/gauge.h>
#include <wx/checklst.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "scrollingdialog.h"
#include <wx/radiobox.h>
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
  wxButton* m_Exit;
  wxGauge* m_Progress;
  wxRadioBox* m_Scope;
  wxButton* m_SelectNone;
  wxCheckBox* m_Ignore;
  wxCheckBox* m_FwdDecl;
  wxCheckBox* m_Simulation;
  wxButton* m_SelectAll;
  wxBoxSizer* sizRunExit;
  wxCheckListBox* m_Sets;
  wxCheckBox* m_Protocol;
  wxRadioBox* m_FileType;
  wxButton* m_Run;
  wxCheckBox* m_ObsoleteLog;
  wxButton* m_Invert;
  wxStaticBoxSizer* sizExecute;
  wxRadioBox* m_Options;
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
