/***************************************************************
 * Name:      cbprofilerexec.h
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Dark Lord
 * Created:   07/20/05 21:54:30
 * Copyright: (c) Dark Lord
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifndef CBPROFILEREXEC_H
#define CBPROFILEREXEC_H

#include <wx/dialog.h>
#include <wx/string.h>
#include <pipedprocess.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
//#include <wx/arrstr.h>  // uncomment for wxWiget 2.6
#include <wx/listctrl.h>

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
	protected:
      void EndModal(int retCode);
      void FindInCallGraph(wxListEvent& event);
      void WriteToFile(wxCommandEvent& event);
    private:
      size_t ParseFlatProfile(wxArrayString msg, size_t begin);
      size_t ParseCallGraph(wxArrayString msg, size_t begin);
      wxWindow*   parent;
      wxListCtrl* outputFlatProfileArea;
      wxTextCtrl* outputHelpFlatProfileArea;
      wxListCtrl* outputCallGraphArea;
      wxTextCtrl* outputHelpCallGraphArea;
      wxTextCtrl* outputMiscArea;
      wxArrayString gprof_output, gprof_errors;

      DECLARE_EVENT_TABLE()
};

#endif // CBPROFILEREXEC_H
