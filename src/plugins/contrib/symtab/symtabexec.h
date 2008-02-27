/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SYMTABEXEC_H
#define SYMTABEXEC_H

#include <wx/arrstr.h>
#include <wx/dialog.h>
#include <wx/string.h>

struct struct_config
{
  int      choWhatToDo;

  wxString txtLibraryPath;
  bool     chkIncludeA;
  bool     chkIncludeLib;
  bool     chkIncludeO;
  bool     chkIncludeObj;
  bool     chkIncludeDll;

  wxString txtLibrary;
  wxString txtSymbol;

  wxString txtNM;
  bool     chkDebug;
  bool     chkDefined;
  bool     chkDemangle;
  bool     chkExtern;
  bool     chkSpecial;
  bool     chkSynthetic;
  bool     chkUndefined;
};

class wxListCtrl;
class wxTextCtrl;
class wxProgressDialog;
class wxWindow;
class wxListEvent;
class wxCommandEvent;

class SymTabExecDlg : public wxDialog
{
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
public:
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
              SymTabExecDlg(wxWindow* parent) :
                parent(parent), SymTabExecDlgLoaded(false),
                m_ListCtrl(0L), m_TextHelp(0L),
                m_TextMisc(0L) {}
  virtual    ~SymTabExecDlg();

  int         Execute  (struct_config config);

  // The following needs to be public (used by a global wxCALLBACK method)
  wxListCtrl* GetListCtrl()      { return m_ListCtrl;        };
  int         GetSortColumn()    { return ms_iSortColumn;    };
  int         GetSortAscending() { return ms_bSortAscending; };

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
private:
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
  void DoInitDialog ();
  void EndModal     (int             retCode);
  void OnWriteToFile(wxCommandEvent& event);
  void OnNext       (wxCommandEvent& event);
  void OnCancel     (wxCommandEvent& event);
  // Global function required by OnColumnClick:
  // int wxCALLBACK SortFunction(long item1, long item2, long dlg);
  void OnColumnClick(wxListEvent& event);

  int  ExecuteMulti      (struct_config &config, wxString cmd);
  int  ExecuteSingle     (struct_config &config, wxString cmd);
  bool ExecuteNM         (wxString lib, wxString cmd);
  int  ParseOutput       (wxString lib, wxString filter);
  void ParseOutputError  ();
  int  ParseOutputSuccess(wxString lib, wxString filter);

  void CleanUp      ();
  void ClearUserData();

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

  wxWindow*     parent;
  bool          SymTabExecDlgLoaded;
  wxListCtrl*   m_ListCtrl;
  wxTextCtrl*   m_TextHelp;
  wxTextCtrl*   m_TextMisc;

  wxArrayString nm_result;
  wxArrayString nm_errors;
  static bool   ms_bSortAscending;
  static int    ms_iSortColumn;

  DECLARE_EVENT_TABLE()
};

#endif // SYMTABEXEC_H
