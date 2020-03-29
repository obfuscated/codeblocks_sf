#ifndef CBPROFILEREXEC_H
#define CBPROFILEREXEC_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include "scrollingdialog.h"

struct struct_config
{
    bool     chkAnnSource;
    bool     chkMinCount;
    bool     chkBrief;
    bool     chkFileInfo;
    bool     chkUnusedFunctions;
    bool     chkStaticCallGraph;
    bool     chkNoStatic;
    bool     chkSum;
    int      spnMinCount;
    wxString txtAnnSource;
    wxString txtExtra;
};

class wxListCtrl;
class wxTextCtrl;
class wxProgressDialog;
class wxWindow;
class wxListEvent;
class wxCommandEvent;

class CBProfilerExecDlg : public wxScrollingDialog
{
    public:
        CBProfilerExecDlg(wxWindow* parent) : parent(parent){}
        virtual ~CBProfilerExecDlg();

        int         Execute(wxString exename, wxString dataname, struct_config config);
        void        ShowOutput(const wxArrayString&  msg, bool error);
        wxListCtrl* GetoutputFlatProfileArea() { return outputFlatProfileArea; };
        int         GetsortColumn()            { return sortColumn;            };
        int         GetsortAscending()         { return sortAscending;         };
    private:
        void EndModal(int retCode);
        void OnColumnClick(wxListEvent& event);
        void FindInCallGraph(wxListEvent& event);
        void JumpInCallGraph(wxListEvent& event);
        void WriteToFile(wxCommandEvent& event);

        void ParseFlatProfile(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t& count);
        void ParseCallGraph(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t &count);
        void ParseMisc(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t &count);

        wxWindow*     parent;
        wxListCtrl*   outputFlatProfileArea;
        wxTextCtrl*   outputHelpFlatProfileArea;
        wxListCtrl*   outputCallGraphArea;
        wxTextCtrl*   outputHelpCallGraphArea;
        wxTextCtrl*   outputMiscArea;
        wxArrayString gprof_output;
        wxArrayString gprof_errors;

        static bool   sortAscending;
        static int    sortColumn;

        DECLARE_EVENT_TABLE()
};

#endif // CBPROFILEREXEC_H
