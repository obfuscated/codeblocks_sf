/***************************************************************
 * Name:      cbprofilerexec.cpp
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 21:54:15
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifdef CB_PRECOMP
    #include "sdk.h"
#else
    #include <wx/event.h>
    #include <wx/font.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/notebook.h>
    #include <wx/textctrl.h>
    #include <wx/utils.h>
    #include <wx/xrc/xmlres.h>
    #include "globals.h"
    #include "manager.h"
    #include "messagemanager.h"
#endif
#include <wx/colour.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/progdlg.h>
#include "cbprofilerexec.h"

BEGIN_EVENT_TABLE(CBProfilerExecDlg, wxDialog)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstFlatProfile"), CBProfilerExecDlg::FindInCallGraph)
    EVT_BUTTON             (XRCID("btnExport"),      CBProfilerExecDlg::WriteToFile)
    EVT_LIST_COL_CLICK     (XRCID("lstFlatProfile"), CBProfilerExecDlg::OnColumnClick)
END_EVENT_TABLE()

// Static data for column sorting management
bool CBProfilerExecDlg::sortAscending = false;
int  CBProfilerExecDlg::sortColumn    = -1;

int CBProfilerExecDlg::Execute(wxString exename, wxString dataname, struct_config config)
{
    //this->parent = parent;

    // gprof optional parameters
    wxString param = config.txtExtra;
    if (config.chkAnnSource) param << _T(" -A")  << config.txtAnnSource;
    if (config.chkMinCount)  param << _T(" -m ") << config.spnMinCount;
    if (config.chkBrief)     param << _T(" -b");
    if (config.chkFileInfo)  param << _T(" -i");
    if (config.chkNoStatic)  param << _T(" -a");
    if (config.chkSum)       param << _T(" -s");

    wxString cmd;
    cmd << _T("gprof ") << param << _T(" \"") << exename << _T("\" \"") << dataname << _T("\"");

    wxProgressDialog progress(_("C::B Profiler plugin"),_("Launching gprof. Please wait..."));
    int pid = wxExecute(cmd, gprof_output, gprof_errors);
    progress.Update(100);

    if (pid == -1)
    {
        wxString msg = _("Unable to execute Gprof\nBe sure it is in the OS global path\nC::B Profiler could not complete the operation");
        cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        Manager::Get()->GetMessageManager()->DebugLog(msg);

        return -1;
    }
    else
    {
        wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgCBProfilerExec"));
        wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
        outputFlatProfileArea=XRCCTRL(*this, "lstFlatProfile", wxListCtrl);
        outputHelpFlatProfileArea=XRCCTRL(*this, "txtHelpFlatProfile", wxTextCtrl);
        outputHelpFlatProfileArea->SetFont(font);
        outputCallGraphArea=XRCCTRL(*this, "lstCallGraph", wxListCtrl);
        outputHelpCallGraphArea=XRCCTRL(*this, "txtHelpCallGraph", wxTextCtrl);
        outputHelpCallGraphArea->SetFont(font);
        outputMiscArea=XRCCTRL(*this, "txtMisc", wxTextCtrl);
        outputMiscArea->SetFont(font);

        if(!gprof_output.IsEmpty())
            ShowOutput(gprof_output, false);
        else
            ShowOutput(gprof_errors, true);
    }

    return 0;
}

void CBProfilerExecDlg::ShowOutput(wxArrayString msg, bool error)
{
    wxString output;
    size_t   count = msg.GetCount();
    if ( !count )
        return;

    if (!error)
    {
        wxProgressDialog progress(_("C::B Profiler plugin"),_("Parsing profile information. Please wait..."));
        // Parsing Flat Profile
        size_t n = 0;
        if (msg[n].Find(_T("Flat profile")) != -1)
            n = ParseFlatProfile(msg, n, progress);

        // Parsing Call Graph
        if (msg[n].Find(_T("Call graph")) != -1)
            n = ParseCallGraph(msg, ++n, progress);

        // The rest of the lines, if any, is printed in the Misc tab
        progress.Update((100*n)/(count-1),_("Parsing profile information. Please wait..."));
        for ( ; n < count; ++n )
        {
            output << msg[n] << _T("\n");
            progress.Update((100*n)/(count-1));
        }
        outputMiscArea->SetValue(output);
        progress.Update(100);
    }
    else
    {
        for (size_t n = 0; n < count; ++n )
        {
            output << msg[n] << _T("\n");
        }
        outputMiscArea->SetValue(output);
        wxColour colour(255,0,0);
        outputMiscArea->SetForegroundColour(colour);
        XRCCTRL(*this, "tabs", wxNotebook)->SetSelection(2);
    }
    ShowModal();
}

CBProfilerExecDlg::~CBProfilerExecDlg()
{
}

void CBProfilerExecDlg::EndModal(int retCode)
{
    wxDialog::EndModal(retCode);
}

size_t CBProfilerExecDlg::ParseCallGraph(wxArrayString msg, size_t begin, wxProgressDialog &progress)
{
    size_t   n;
    size_t next = 0;
    wxListItem item;

    // Setting colums names
    outputCallGraphArea->InsertColumn(0, _T("index"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(1, _T("% time"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(2, _T("self"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(3, _T("children"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(4, _T("called"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(5, _T("name"));

    // Jump header lines
    while ((begin < msg.GetCount())&&(msg[begin].Find(_T("index % time")) == -1))
    {
        ++begin;
    }
    ++begin;

    progress.Update((100*begin)/(msg.GetCount()-1),_("Parsing Call Graph information. Please wait..."));

    // Parsing Call Graph
    for (n = begin ; n < msg.GetCount(); ++n )
    {
        if ((msg[n].IsEmpty())||(msg[n].Find(0x0C) != -1))
            break;
        outputCallGraphArea->InsertItem(next,_T(""));
        char first_char = msg[n].GetChar(0);
        if (first_char == '-')
        {
            outputCallGraphArea->SetItem(next, 0, _T(""));
            item.Clear();
            item.SetId(next);
            item.SetBackgroundColour(*wxLIGHT_GREY);
            outputCallGraphArea->SetItem(item);
        }
        else
        {
            outputCallGraphArea->SetItem(next, 0, ((msg[n].Mid(0,6)).Trim(true)).Trim(false));
            outputCallGraphArea->SetItem(next, 1, ((msg[n].Mid(6,6)).Trim(true)).Trim(false));
            outputCallGraphArea->SetItem(next, 2, ((msg[n].Mid(12,8)).Trim(true)).Trim(false));
            outputCallGraphArea->SetItem(next, 3, ((msg[n].Mid(20,8)).Trim(true)).Trim(false));
            outputCallGraphArea->SetItem(next, 4, ((msg[n].Mid(28,17)).Trim(true)).Trim(false));
            outputCallGraphArea->SetItem(next, 5, msg[n].Mid(45));
            if (first_char != '[')
            {
                item.Clear();
                item.SetId(next);
                item.SetTextColour(wxTheColourDatabase->Find(_T("GREY")));
                outputCallGraphArea->SetItem(item);
            }
        }
        progress.Update((100*n)/(msg.GetCount()-1));
        ++next;
    }

    // Resize columns
    outputCallGraphArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(5, wxLIST_AUTOSIZE);

    // Printing Call Graph Help
    wxString output_help;
    for ( ; n < msg.GetCount(); ++n )
    {
        if (msg[n].Find(0x0C) != -1)
            break;
        output_help << msg[n] << _T("\n");
        progress.Update((100*n)/(msg.GetCount()-1));
    }
    outputHelpCallGraphArea->SetValue(output_help);

    return ++n;
}

size_t CBProfilerExecDlg::ParseFlatProfile(wxArrayString msg, size_t begin, wxProgressDialog &progress)
{
    size_t   n;
    size_t next = 0;

    // Setting colums names
    outputFlatProfileArea->InsertColumn(0, _T("% time"), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(1, _T("cum. sec."), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(2, _T("self sec."), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(3, _T("calls"), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(4, _T("self s/call"), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(5, _T("total s/call"), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(6, _T("name"));

    // Jump header lines
    while ((begin < msg.GetCount())&&(msg[begin].Find(_T("time   seconds")) == -1))
    {
        ++begin;
    }
    ++begin;

    progress.Update((100*begin)/(msg.GetCount()-1),_("Parsing Flat Profile information. Please wait..."));

    // Parsing Call Graph
    for (n = begin ; n < msg.GetCount(); ++n )
    {
        if ((msg[n].IsEmpty())||(msg[n].Find(0x0C) != -1))
            break;
        long item = outputFlatProfileArea->InsertItem(next,_T(""));
        outputFlatProfileArea->SetItemData(item, next);
        outputFlatProfileArea->SetItem(next, 0, ((msg[n].Mid(0,6)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 1, ((msg[n].Mid(6,10)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 2, ((msg[n].Mid(16,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 3, ((msg[n].Mid(25,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 4, ((msg[n].Mid(34,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 5, ((msg[n].Mid(43,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 6, ((msg[n].Mid(52)).Trim(true)).Trim(false));
        progress.Update((100*n)/(msg.GetCount()-1));
        ++next;
    }

    // Resize columns
    outputFlatProfileArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(6, wxLIST_AUTOSIZE);

    wxString output_help;
    // Printing Flat Profile Help
    for ( ; n < msg.GetCount(); ++n )
    {
        if (msg[n].Find(0x0C) != -1)
            break;
        output_help << msg[n] << _T("\n");
        progress.Update((100*n)/(msg.GetCount()-1));
    }
    outputHelpFlatProfileArea->SetValue(output_help);

    return ++n;
}

// This function writes the gprof output to a file
void CBProfilerExecDlg::WriteToFile(wxCommandEvent& event)
{
    wxFileDialog filedialog(parent, _("Save gprof output to file"),_T(""),_T(""),_T("*.*"),wxSAVE);

    if (filedialog.ShowModal() == wxID_OK)
    {
        wxFFile file(filedialog.GetPath().c_str(), _T("w"));
        for (size_t n=0; n<gprof_output.GetCount(); ++n)
        {
            file.Write(gprof_output[n]);
            file.Write(_T("\n"));
        }
        file.Close();
    }
}

// This function retrieve the selected function in the call graph tab
void CBProfilerExecDlg::FindInCallGraph(wxListEvent& event)
{
    // We retrieve the name of the function on the line selected
    wxListItem item;
    item.SetId(event.GetIndex());
    item.SetColumn(6);
    item.SetMask(wxLIST_MASK_TEXT);
    outputFlatProfileArea->GetItem(item);
    wxString function_name = item.GetText();

    // Then search this name in the call graph
    wxString indexColumn, functionColumn;
    int n;
    for (n=0; n<outputCallGraphArea->GetItemCount(); ++n)
    {
        item.Clear();
        item.SetId(n);
        item.SetColumn(0);
        item.SetMask(wxLIST_MASK_TEXT);
        outputCallGraphArea->GetItem(item);
        indexColumn = item.GetText();
        if ((indexColumn.Mid(0,1)).CompareTo(_T("[")) == 0)
        {
            item.Clear();
            item.SetId(n);
            item.SetColumn(5);
            item.SetMask(wxLIST_MASK_TEXT);
            outputCallGraphArea->GetItem(item);
            functionColumn = item.GetText();
            if (functionColumn.Find(function_name) != -1)
                break;
        }
    }

    // Scrolling to the desired line in the "Call Graph" tab
    if (n < outputCallGraphArea->GetItemCount())
    {
        outputCallGraphArea->EnsureVisible(n);
        XRCCTRL(*this, "tabs", wxNotebook)->SetSelection(1);
    }
}

// Sorting function of the flat profile columns
int wxCALLBACK SortFunction(long item1, long item2, long sortData)
{
    CBProfilerExecDlg *dialog = (CBProfilerExecDlg*) sortData;

    wxListCtrl *listCtrl = dialog->GetoutputFlatProfileArea();
    int col = dialog->GetsortColumn();
    long itemId1 = listCtrl->FindItem(-1, item1);
    long itemId2 = listCtrl->FindItem(-1, item2);

    wxListItem listItem1, listItem2;

    listItem1.SetId(itemId1);
    listItem1.SetColumn(col);
    listItem1.SetMask(wxLIST_MASK_TEXT);
    listCtrl->GetItem(listItem1);

    listItem2.SetId(itemId2);
    listItem2.SetColumn(col);
    listItem2.SetMask(wxLIST_MASK_TEXT);
    listCtrl->GetItem(listItem2);

    // All the columns are composed with numbers except the last one
    if (col == 6)
    {
       if (dialog->GetsortAscending())
           return wxStrcmp(listItem1.GetText(), listItem2.GetText());
       else
           return wxStrcmp(listItem2.GetText(), listItem1.GetText());
    }
    else
    {
        double num1, num2;
        double success = listItem1.GetText().ToDouble(&num1);
        if (!success)
        {
            if (dialog->GetsortAscending()) return -1;
            else                            return 1;
        }
        success = listItem2.GetText().ToDouble(&num2);
        if (!success)
        {
            if (dialog->GetsortAscending()) return 1;
            else                            return -1;
        }
        if (dialog->GetsortAscending())
        {
            if (num1 < num2)      return -1;
            else if (num1 > num2) return 1;
            else                  return 0;
        }
        else
        {
            if (num1 > num2)      return -1;
            else if (num1 < num2) return 1;
            else                  return 0;
        }
    }
}

// Function called when a column header is clicked
void CBProfilerExecDlg::OnColumnClick(wxListEvent& event)
{
    if (event.GetColumn() != sortColumn)
        sortAscending = true;
    else
        sortAscending = !sortAscending;

    sortColumn = event.GetColumn();
    outputFlatProfileArea->SortItems(SortFunction, (long)this);
}
