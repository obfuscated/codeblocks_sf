/***************************************************************
 * Name:      cbprofilerexec.cpp
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 21:54:15
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#include "cbprofilerexec.h"

BEGIN_EVENT_TABLE(CBProfilerExecDlg, wxDialog)
EVT_LIST_ITEM_ACTIVATED(XRCID("lstFlatProfile"), CBProfilerExecDlg::FindInCallGraph)
EVT_BUTTON(XRCID("btnExport"), CBProfilerExecDlg::WriteToFile)
END_EVENT_TABLE()

// This function retrieve the selected function in the call graph tab
void CBProfilerExecDlg::FindInCallGraph(wxListEvent& event)
{
	 // We retrieve the name of the function on the line selected
	 wxListItem item;
	 item.m_itemId = event.GetIndex();
	 item.m_col = 6;
	 item.m_mask = wxLIST_MASK_TEXT;
	 outputFlatProfileArea->GetItem(item);
	 wxString function_name = item.m_text;

	 // Then search this name in the call graph
	 wxString indexColumn, functionColumn;
	 size_t n;
	 for (n=0; n<(size_t)outputCallGraphArea->GetItemCount(); n++)
	 {
	 	 item.Clear();
	 	 item.m_itemId = n;
	 	 item.m_col = 0;
	 	 item.m_mask = wxLIST_MASK_TEXT;
	 	 outputCallGraphArea->GetItem(item);
	 	 indexColumn = item.m_text;
       if ((indexColumn.Mid(0,1)).CompareTo(_T("[")) == 0)
       {
          item.Clear();
	 	    item.m_itemId = n;
	 	    item.m_col = 5;
	 	    item.m_mask = wxLIST_MASK_TEXT;
	 	    outputCallGraphArea->GetItem(item);
	 	    functionColumn = item.m_text;
	 	    if (functionColumn.Find(function_name) != -1)
	 	       break;
       }
	 }

	 // Scrolling to the desired line in the "Call Graph" tab
	 if (n < (size_t)outputCallGraphArea->GetItemCount())
	 {
       outputCallGraphArea->EnsureVisible(n);
       XRCCTRL(*this, "tabs", wxNotebook)->SetSelection(1);
	 }
}

int CBProfilerExecDlg::Execute(wxString exename, wxString dataname, struct_config config)
{
    //this->parent = parent;

    // gprof optional parameters
    wxString param = config.txtExtra;
    if (config.chkAnnSource) param << _T(" -A") << config.txtAnnSource;
    if (config.chkMinCount) param << _T(" -m ") << config.spnMinCount;
    if (config.chkBrief) param << _T(" -b");
    if (config.chkFileInfo) param << _T(" -i");
    if (config.chkNoStatic) param << _T(" -a");
    if (config.chkSum) param << _T(" -s");

    wxString cmd;
    cmd << _T("gprof ") << param << _T(" \"") << exename << _T("\" \"") << dataname << _T("\"");

    wxProgressDialog progress(_("C::B Profiler plugin"),_("Launching gprof. Please wait..."));
    int pid = wxExecute(cmd, gprof_output, gprof_errors);
    progress.Update(100);

    if (pid == -1)
    {
        wxString msg = _("Unable to execute Gprof\nBe sure it is in the OS global path\nC::B Profiler could not complete the operation");
        wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
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
    size_t   n;
    wxString output;
    size_t   count = msg.GetCount();
    if ( !count )
        return;

   if (!error)
   {
      wxProgressDialog progress(_("C::B Profiler plugin"),_("Parsing profile information. Please wait..."));
      // Parsing Flat Profile
      n = 0;
      if (msg[n].Find(_T("Flat profile")) != -1)
         n = ParseFlatProfile(msg, n);
      progress.Update(50);

      // Parsing Call Graph
      if (msg[n].Find(_T("Call graph")) != -1)
         n = ParseCallGraph(msg, ++n);
      progress.Update(90);

      // The rest of the lines, if any, is printed in the Misc tab
      for ( ; n < count; n++ )
      {
         output << msg[n] << _T("\n");
      }
      outputMiscArea->SetValue(output);
      progress.Update(100);
   }
    else
    {
    	  for ( n = 0; n < count; n++ )
        {
            output << msg[n] << _T("\n");
        }
        outputMiscArea->SetValue(output);
        wxColour color(255,0,0);
        outputMiscArea->SetForegroundColour(color);
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

size_t CBProfilerExecDlg::ParseCallGraph(wxArrayString msg, size_t begin)
{
	size_t   n;
	size_t next = 0;
	char first_char;
	wxString output_help;

	// Setting colums names
	outputCallGraphArea->InsertColumn(0, _T("index"));
	outputCallGraphArea->InsertColumn(1, _T("% time"));
	outputCallGraphArea->InsertColumn(2, _T("self"));
	outputCallGraphArea->InsertColumn(3, _T("children"));
	outputCallGraphArea->InsertColumn(4, _T("called"));
	outputCallGraphArea->InsertColumn(5, _T("name"));

	// Jump header lines
	while ((begin < msg.GetCount())&&(msg[begin].Find(_T("index % time")) == -1))
	   begin++;
   begin++;

   // Parsing Call Graph
   for (n = begin ; n < msg.GetCount(); n++ )
   {
      if ((msg[n].IsEmpty())||(msg[n].Find(0x0C) != -1))
    	   break;
      outputCallGraphArea->InsertItem(next,_T(""));
      first_char = msg[n].GetChar(0);
      if (first_char == '-')
         continue;
      outputCallGraphArea->SetItem(next, 0, ((msg[n].Mid(0,6)).Trim(true)).Trim(false));
      outputCallGraphArea->SetItem(next, 1, ((msg[n].Mid(6,6)).Trim(true)).Trim(false));
      outputCallGraphArea->SetItem(next, 2, ((msg[n].Mid(12,8)).Trim(true)).Trim(false));
      outputCallGraphArea->SetItem(next, 3, ((msg[n].Mid(20,8)).Trim(true)).Trim(false));
      outputCallGraphArea->SetItem(next, 4, ((msg[n].Mid(28,17)).Trim(true)).Trim(false));
      outputCallGraphArea->SetItem(next, 5, msg[n].Mid(45));
      next++;
   }

   // Resize columns
   outputCallGraphArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
   outputCallGraphArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
   outputCallGraphArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
   outputCallGraphArea->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER );
   outputCallGraphArea->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER );
   outputCallGraphArea->SetColumnWidth(5, wxLIST_AUTOSIZE);

   // Printing Call Graph Help
   for ( ; n < msg.GetCount(); n++ )
   {
      if (msg[n].Find(0x0C) != -1)
         break;
      output_help << msg[n] << _T("\n");
   }
   outputHelpCallGraphArea->SetValue(output_help);

   return ++n;
}

size_t CBProfilerExecDlg::ParseFlatProfile(wxArrayString msg, size_t begin)
{
	size_t   n;
	size_t next = 0;
	wxString output_help;

	// Setting colums names
	outputFlatProfileArea->InsertColumn(0, _T("% time"));
	outputFlatProfileArea->InsertColumn(1, _T("cum. sec."));
	outputFlatProfileArea->InsertColumn(2, _T("self sec."));
	outputFlatProfileArea->InsertColumn(3, _T("calls"));
	outputFlatProfileArea->InsertColumn(4, _T("self s/call"));
	outputFlatProfileArea->InsertColumn(5, _T("total s/call"));
	outputFlatProfileArea->InsertColumn(6, _T("name"));

	// Jump header lines
	while ((begin < msg.GetCount())&&(msg[begin].Find(_T("Ts/call")) == -1))
	   begin++;
   begin++;

   // Parsing Call Graph
   for (n = begin ; n < msg.GetCount(); n++ )
   {
      if ((msg[n].IsEmpty())||(msg[n].Find(0x0C) != -1))
    	   break;
      outputFlatProfileArea->InsertItem(next,_T(""));
      outputFlatProfileArea->SetItem(next, 0, ((msg[n].Mid(0,6)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 1, ((msg[n].Mid(6,10)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 2, ((msg[n].Mid(16,9)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 3, ((msg[n].Mid(25,9)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 4, ((msg[n].Mid(34,9)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 5, ((msg[n].Mid(43,9)).Trim(true)).Trim(false));
      outputFlatProfileArea->SetItem(next, 6, ((msg[n].Mid(52)).Trim(true)).Trim(false));
      next++;
   }

   // Resize columns
   outputFlatProfileArea->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER );
   outputFlatProfileArea->SetColumnWidth(6, wxLIST_AUTOSIZE);

   // Printing Flat Profile Help
   for ( ; n < msg.GetCount(); n++ )
   {
   	 if (msg[n].Find(0x0C) != -1)
   	     break;
      output_help << msg[n] << _T("\n");
   }
   outputHelpFlatProfileArea->SetValue(output_help);

   return ++n;
}

// This function writes the gprof output to a file
void CBProfilerExecDlg::WriteToFile(wxCommandEvent& event)
{
	size_t n;
	wxFileDialog filedialog(parent, _("Save gprof output to file"),_T(""),_T(""),_T("*.*"),wxSAVE);

	if (filedialog.ShowModal() == wxID_OK)
	{
		wxFFile file(filedialog.GetPath().c_str(), _T("w"));
		for (n=0; n<gprof_output.GetCount(); n++)
		{
		   file.Write(gprof_output[n]);
		   file.Write(_T("\n"));
		}
		file.Close();
	}
}
