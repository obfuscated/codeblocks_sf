/***************************************************************
 * Name:      cbprofilerexec.h
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 21:54:30
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifndef CBPROFILEREXEC_H
#define CBPROFILEREXEC_H

#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/font.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/datstrm.h>
#include <wx/progdlg.h>
#include <wx/fs_zip.h>
#include <wx/gdicmn.h>
#include <wx/filedlg.h>
#include <wx/ffile.h>
#include <wx/dialog.h>
#include <wx/string.h>

#include <cbproject.h>
#include <manager.h>
#include <projectmanager.h>
#include <messagemanager.h>
#include <pipedprocess.h>

typedef struct
{
	bool chkAnnSource;
	wxString txtAnnSource;
	bool chkMinCount;
	int  spnMinCount;
	bool chkBrief;
	bool chkFileInfo;
	bool chkNoStatic;
	bool chkSum;
	wxString txtExtra;
} struct_config;

class CBProfilerExecDlg : public wxDialog
{
	public:
		CBProfilerExecDlg(wxWindow* parent)
            : parent(parent){}
		virtual ~CBProfilerExecDlg();

		int Execute(wxString exename, wxString dataname, struct_config config);
		void ShowOutput(wxArrayString  msg, bool error);
		wxListCtrl* GetoutputFlatProfileArea() {return outputFlatProfileArea;};
		int GetsortColumn() {return sortColumn;};
		int GetsortAscending() {return sortAscending;};
	protected:
      void EndModal(int retCode);
      void FindInCallGraph(wxListEvent& event);
      void WriteToFile(wxCommandEvent& event);
      void OnColumnClick(wxListEvent& event);
    private:
      size_t ParseFlatProfile(wxArrayString msg, size_t begin, wxProgressDialog &progress);
      size_t ParseCallGraph(wxArrayString msg, size_t begin, wxProgressDialog &progress);
      wxWindow*   parent;
      wxListCtrl* outputFlatProfileArea;
      wxTextCtrl* outputHelpFlatProfileArea;
      wxListCtrl* outputCallGraphArea;
      wxTextCtrl* outputHelpCallGraphArea;
      wxTextCtrl* outputMiscArea;
      wxArrayString gprof_output, gprof_errors;
      static bool sortAscending;
      static int sortColumn;
      int LastListClickedCol;

      DECLARE_EVENT_TABLE()
};

#endif // CBPROFILEREXEC_H
