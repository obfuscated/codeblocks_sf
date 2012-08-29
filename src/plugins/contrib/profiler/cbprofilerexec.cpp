/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
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
    #include "logmanager.h"
#endif
#include <wx/busyinfo.h>
#include <wx/colour.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/progdlg.h>
#include "cbprofilerexec.h"

BEGIN_EVENT_TABLE(CBProfilerExecDlg, wxScrollingDialog)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstFlatProfile"), CBProfilerExecDlg::FindInCallGraph)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstCallGraph"),   CBProfilerExecDlg::JumpInCallGraph)
    EVT_BUTTON             (XRCID("btnExport"),      CBProfilerExecDlg::WriteToFile)
    EVT_LIST_COL_CLICK     (XRCID("lstFlatProfile"), CBProfilerExecDlg::OnColumnClick)
END_EVENT_TABLE()

// Static data for column sorting management
bool CBProfilerExecDlg::sortAscending = false;
int  CBProfilerExecDlg::sortColumn    = -1;

int CBProfilerExecDlg::Execute(wxString exename, wxString dataname, struct_config config)
{
    // gprof optional parameters
    wxString param = config.txtExtra;
    if (config.chkAnnSource && !config.txtAnnSource.IsEmpty()) param << _T(" -A")  << config.txtAnnSource;
    if (config.chkMinCount)                                    param << _T(" -m ") << config.spnMinCount;
    if (config.chkBrief)                                       param << _T(" -b");
    if (config.chkFileInfo)                                    param << _T(" -i");
    if (config.chkUnusedFunctions)                             param << _T(" -z");
    if (config.chkStaticCallGraph)                             param << _T(" -c");
    if (config.chkNoStatic)                                    param << _T(" -a");
    if (config.chkSum)                                         param << _T(" -s");

    wxString cmd;
    cmd << _T("gprof ") << param << _T(" \"") << exename << _T("\" \"") << dataname << _T("\"");

    int pid = -1;

    { // begin lifetime of wxBusyInfo
      wxBusyInfo wait(_("Please wait, while running gprof..."), this);
      Manager::Get()->GetLogManager()->DebugLog(F(_T("Profiler: Running command %s"), cmd.wx_str()));
      pid = wxExecute(cmd, gprof_output, gprof_errors);
    } // end lifetime of wxBusyInfo

    if (pid == -1)
    {
        wxString msg = _("Unable to execute gprof.\nBe sure the gprof executable is in the OS global path.\nC::B Profiler could not complete the operation.");
        cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, (wxWindow*)Manager::Get()->GetAppWindow());
        Manager::Get()->GetLogManager()->DebugLog(msg);

        return -1;
    }
    else
    {
        wxXmlResource::Get()->LoadObject(this, parent, _T("dlgCBProfilerExec"),_T("wxScrollingDialog"));
        wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
        outputFlatProfileArea     = XRCCTRL(*this, "lstFlatProfile",     wxListCtrl);
        outputHelpFlatProfileArea = XRCCTRL(*this, "txtHelpFlatProfile", wxTextCtrl);
        outputHelpFlatProfileArea->SetFont(font);
        outputCallGraphArea       = XRCCTRL(*this, "lstCallGraph",       wxListCtrl);
        outputHelpCallGraphArea   = XRCCTRL(*this, "txtHelpCallGraph",   wxTextCtrl);
        outputHelpCallGraphArea->SetFont(font);
        outputMiscArea            = XRCCTRL(*this, "txtMisc",            wxTextCtrl);
        outputMiscArea->SetFont(font);

        if(!gprof_output.IsEmpty())
            ShowOutput(gprof_output, false);
        else
            ShowOutput(gprof_errors, true);
    }

    return 0;
}

void CBProfilerExecDlg::ShowOutput(const wxArrayString& msg, bool error)
{
    const size_t maxcount(msg.GetCount());
    if ( !maxcount )
        return;

    if (!error)
    {
        wxProgressDialog progress(_("C::B Profiler plugin"),_("Parsing profile information. Please wait..."), maxcount, NULL, wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_SMOOTH);

        // Parsing Flat Profile
        size_t count(0);
        if (msg[count].Find(_T("Flat profile")) != -1)
            ParseFlatProfile(msg, progress, maxcount, count);

        // Parsing Call Graph
        if ((count < maxcount) &&
            (msg[count].Find(_T("Call graph"))   != -1))
            ParseCallGraph(msg, progress, maxcount, count);

        // The rest of the lines, if any, is printed in the Misc tab
        ParseMisc(msg, progress, maxcount, count);
    }
    else
    {
        wxString output;
        for (size_t count(0); count < maxcount; ++count )
        {
            output << msg[count] << _T("\n");
        }
        outputMiscArea->SetValue(output);
        const wxColour colour(255,0,0);
        outputMiscArea->SetForegroundColour(colour);
    }

    ShowModal();
}

CBProfilerExecDlg::~CBProfilerExecDlg()
{
}

void CBProfilerExecDlg::EndModal(int retCode)
{
    wxScrollingDialog::EndModal(retCode);
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

void CBProfilerExecDlg::ParseMisc(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t &count)
{
    // parsing
    wxString output;
    progress.Update(count, _("Parsing miscellaneous information. Please wait..."));
    for ( ; count < maxcount; ++count )
    {
        if ((count%10) == 0) progress.Update(count);
        output << msg[count] << _T("\n");
    }
    outputMiscArea->SetValue(output);
}

void CBProfilerExecDlg::ParseCallGraph(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t& count)
{
    // Setting colums names
    outputCallGraphArea->InsertColumn(0, _T("index"),    wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(1, _T("% time"),   wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(2, _T("self"),     wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(3, _T("children"), wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(4, _T("called"),   wxLIST_FORMAT_CENTRE);
    outputCallGraphArea->InsertColumn(5, _T("name"));

    // Jump header lines
    progress.Update(count,_("Parsing call graph information. Please wait..."));
    while ( (count < maxcount) && (msg[count].Find(_T("index % time")) == -1) )
    {
        if ((count%10) == 0) progress.Update(count);
        ++count;
    }
    ++count;

    // Parsing Call Graph
    size_t next(0);
    wxListItem item;
    wxString TOKEN;

    // setting listctrl default text colour for secondary lines
    const wxColour COLOUR(wxTheColourDatabase->Find(_T("GREY")));

    for ( ; count < maxcount; ++count )
    {
        if ((count%10) == 0) progress.Update(count);

        TOKEN = msg[count];
        if ( (TOKEN.IsEmpty()) || (TOKEN.Find(wxChar(0x0C)) != -1) )
            break;

        outputCallGraphArea->InsertItem(count,_T(""));
        char first_char = TOKEN.GetChar(0);
        // treating the empty separator lines
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
            outputCallGraphArea->SetItem(next, 0, TOKEN(0,6).Trim(true).Trim(false));
            outputCallGraphArea->SetItem(next, 1, TOKEN(6,6).Trim(true).Trim(false));
            outputCallGraphArea->SetItem(next, 2, TOKEN(12,8).Trim(true).Trim(false));
            outputCallGraphArea->SetItem(next, 3, TOKEN(20,8).Trim(true).Trim(false));
            outputCallGraphArea->SetItem(next, 4, TOKEN(28,17).Trim(true).Trim(false));
            outputCallGraphArea->SetItem(next, 5, TOKEN.Mid(45));
            if (first_char != '[')
            {
                outputCallGraphArea->SetItemTextColour(next,COLOUR);
            }
        }
        ++next;
    }

    // Resize columns
    outputCallGraphArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER );
    outputCallGraphArea->SetColumnWidth(4, wxLIST_AUTOSIZE );
    outputCallGraphArea->SetColumnWidth(5, wxLIST_AUTOSIZE);

    // Printing Call Graph Help
    wxString output_help;
    for ( ; count < maxcount; ++count )
    {
        if ((count%10) == 0) progress.Update(count);

        TOKEN = msg[count];
        if (TOKEN.Find(wxChar(0x0C)) != -1)
            break;

        output_help << TOKEN << _T("\n");
    }
    outputHelpCallGraphArea->SetValue(output_help);

    ++count;
}

void CBProfilerExecDlg::ParseFlatProfile(const wxArrayString& msg, wxProgressDialog &progress, const size_t maxcount, size_t &count)
{
    // Setting colums names
    outputFlatProfileArea->InsertColumn(0, _T("% time"),        wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(1, _T("cum. sec."),     wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(2, _T("self sec."),     wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(3, _T("calls"),         wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(4, _T("self ms/call"),  wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(5, _T("total ms/call"), wxLIST_FORMAT_CENTRE);
    outputFlatProfileArea->InsertColumn(6, _T("name"));

    // Jump header lines
    progress.Update(count,_("Parsing flat profile information. Please wait..."));
    while ((count < maxcount)&&(msg[count].Find(_T("time   seconds")) == -1))
    {
        ++count;
    }
    ++count;

    // Parsing Call Graph
    size_t next(0);
    unsigned int spacePos[6] = {6, 16, 25, 34, 43, 52};
    wxString TOKEN;
    for ( ; count < maxcount; ++count )
    {
        if ((count%10) == 0) progress.Update(count);

        TOKEN = msg[count];
        if ( (TOKEN.IsEmpty()) || (TOKEN.Find(wxChar(0x0C)) != -1) )
            break;

        long item = outputFlatProfileArea->InsertItem(next,_T(""));
        outputFlatProfileArea->SetItemData(item, next);
        // check that we have spaces where spaces are supposed to be
        if (TOKEN.Len() > spacePos[5]) {
            bool need_parsing = false;
            for (int i=0; i<6; ++i)
            {
                if (TOKEN[spacePos[i]] != ' ')
                {
                    need_parsing = true;
                    break;
                }
            }
            // if profile output is not in perfect table format
            // manually parse for space positions
            if (need_parsing)
            {
                int count=0; int i=0; int len = TOKEN.Len();
                while (i < len && count < 6) {
                    // we start with spaces
                    while (TOKEN[i] == ' ' && ++i < len);
                    if (i>=len) break;
                    // now we parse everything else than
                    while (TOKEN[i] != ' ' && ++i < len);
                    if (i>=len) break;
                    // found a new space position
                    spacePos[count++] = i;
                }
            }
        }

        outputFlatProfileArea->SetItem(next, 0, ((TOKEN.Mid(0,spacePos[0])).Trim(true)).Trim(false));
        for (int i(1); i<6; ++i)
            outputFlatProfileArea->SetItem(next, i,((TOKEN.Mid(spacePos[i-1],spacePos[i] - spacePos[i-1])).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 6, ((TOKEN.Mid(spacePos[5])).Trim(true)).Trim(false));

/*
        outputFlatProfileArea->SetItem(next, 0, ((TOKEN.Mid(0,6)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 1, ((TOKEN.Mid(6,10)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 2, ((TOKEN.Mid(16,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 3, ((TOKEN.Mid(25,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 4, ((TOKEN.Mid(34,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 5, ((TOKEN.Mid(43,9)).Trim(true)).Trim(false));
        outputFlatProfileArea->SetItem(next, 6, ((TOKEN.Mid(52)).Trim(true)).Trim(false));
*/
        ++next;
    }

    // Resize columns
    outputFlatProfileArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(3, wxLIST_AUTOSIZE );
    outputFlatProfileArea->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER );
    outputFlatProfileArea->SetColumnWidth(6, wxLIST_AUTOSIZE);

    // Printing Flat Profile Help
    wxString output_help;
    for ( ; count < maxcount; ++count )
    {
        if ((count%10) == 0) progress.Update(count);

        TOKEN = msg[count];
        if (TOKEN.Find(wxChar(0x0C)) != -1)
            break;

        output_help << msg[count] << _T("\n");
    }
    outputHelpFlatProfileArea->SetValue(output_help);

    ++count;
}

// This function writes the gprof output to a file
void CBProfilerExecDlg::WriteToFile(wxCommandEvent& /*event*/)
{
    wxFileDialog filedialog(parent,
                            _("Save gprof output to file"),
                            wxEmptyString,
                            wxEmptyString,
                            _T("*.*"),
                            wxFD_SAVE);

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

// This function retrieves the selected function in the call graph tab
void CBProfilerExecDlg::FindInCallGraph(wxListEvent& event)
{
    // We retrieve the name of the function on the line selected
    wxListItem item;
    item.SetId(event.GetIndex());
    item.SetColumn(6);
    item.SetMask(wxLIST_MASK_TEXT);
    outputFlatProfileArea->GetItem(item);
    const wxString function_name(item.GetText());

    // Then search this name in the call graph
    wxString indexColumn;
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
            if (item.GetText().Find(function_name) != -1)
                break;
        }
    }

    // Scrolling to the desired line in the "Call Graph" tab
    outputCallGraphArea->SetItemState(item,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
    outputCallGraphArea->EnsureVisible(n);
    XRCCTRL(*this, "tabs", wxNotebook)->SetSelection(1);
}

// This function jumps to the selected function in the call graph tab
void CBProfilerExecDlg::JumpInCallGraph(wxListEvent& event)
{
    // We retrieve the name of the function on the line selected
    wxListItem item;
    item.SetId(event.GetIndex());
    item.SetColumn(5);
    item.SetMask(wxLIST_MASK_TEXT);
    outputCallGraphArea->GetItem(item);
    const wxString function_name(item.GetText());

    // Then search this name in the call graph
    wxString indexColumn;
    int n;
    const int maxcount(outputCallGraphArea->GetItemCount());
    for (n=0; n<maxcount; ++n)
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

            if (function_name.Find(item.GetText()) != wxNOT_FOUND)
                break;
        }
    }

    // Scrolling to the desired line in the "Call Graph" tab
    outputCallGraphArea->SetItemState(item,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
    outputCallGraphArea->EnsureVisible(n);
}
