#ifndef CBPROFILEREXEC_H
#define CBPROFILEREXEC_H

#include <wx/arrstr.h>
#include <wx/dialog.h>
#include <wx/string.h>

struct struct_config
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
};

class wxListCtrl;
class wxTextCtrl;
class wxProgressDialog;
class wxWindow;
class wxListEvent;
class wxCommandEvent;

class CBProfilerExecDlg : public wxDialog
{
    public:
        CBProfilerExecDlg(wxWindow* parent) : parent(parent){}
        virtual ~CBProfilerExecDlg();

        int Execute(wxString exename, wxString dataname, struct_config config);
        void ShowOutput(wxArrayString  msg, bool error);
        wxListCtrl* GetoutputFlatProfileArea() {return outputFlatProfileArea;};
        int GetsortColumn() {return sortColumn;};
        int GetsortAscending() {return sortAscending;};
    private:
        void EndModal(int retCode);
        void FindInCallGraph(wxListEvent& event);
        void WriteToFile(wxCommandEvent& event);
        void OnColumnClick(wxListEvent& event);

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
