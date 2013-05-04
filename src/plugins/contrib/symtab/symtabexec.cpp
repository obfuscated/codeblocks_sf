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
  #include <wx/button.h>
  #include <wx/dir.h>
  #include <wx/event.h>
  #include <wx/font.h>
  #include <wx/intl.h>
  #include <wx/listctrl.h>
  #include <wx/notebook.h>
  #include <wx/textctrl.h>
  #include <wx/stattext.h>
  #include <wx/utils.h>
  #include <wx/xrc/xmlres.h>
  #include "globals.h" // cbMessageBox
  #include "manager.h"
  #include "logmanager.h"
#endif

//#define TRACE_SYMTAB_EXE

#include <wx/busyinfo.h>
#include <wx/colour.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/progdlg.h>
#include <wx/button.h>

#include "symtabexec.h"

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

BEGIN_EVENT_TABLE(SymTabExecDlg, wxScrollingDialog)
  EVT_BUTTON         (XRCID("btnExport"),     SymTabExecDlg::OnWriteToFile)
  EVT_BUTTON         (XRCID("btnNext"),       SymTabExecDlg::OnNext)
  EVT_BUTTON         (XRCID("btnCancel"),     SymTabExecDlg::OnCancel)
  EVT_LIST_COL_CLICK (XRCID("lstLib2Symbol"), SymTabExecDlg::OnColumnClick)
END_EVENT_TABLE()

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// Static data for column sorting management
bool SymTabExecDlg::ms_bSortAscending = false;
int  SymTabExecDlg::ms_iSortColumn    = -1;

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

SymTabExecDlg::~SymTabExecDlg()
{
  //dtor
  CleanUp();
}// ~SymTabExecDlg

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabExecDlg::Execute(struct_config config)
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::Execute"));
#endif

  // Intialise GUI (does nothing if already done)
  DoInitDialog();

  // nm optional parameters
  wxString param(_T(""));
  if (config.chkDebug)     param << _T(" --debug-syms");
  if (config.chkDefined)   param << _T(" --defined-only");
  if (config.chkDemangle)  param << _T(" --demangle");
  if (config.chkExtern)    param << _T(" --extern-only");
  if (config.chkSpecial)   param << _T(" --special-syms");
  if (config.chkSynthetic) param << _T(" --synthetic");
  if (config.chkUndefined) param << _T(" --undefined-only");

  wxString cmd;
  if (config.txtNM.Trim().IsEmpty())
    cmd << _T("nm")                         << param;
  else
    cmd << (config.txtNM.Trim()) << _T(" ") << param;

  int retval = -1;
  // Search for a symbol given a library path
  if      (config.choWhatToDo == 0)
  {
    retval = ExecuteMulti(config, cmd);
  }// if
  // Search for all symbols in a given library
  else if (config.choWhatToDo == 1)
  {
    retval = ExecuteSingle(config, cmd);
  }// else if
  else
  {
    Manager::Get()->GetLogManager()->DebugLog(_T("SymTab: Invalid (unsupported) choice."));
  }// else

  CleanUp(); // free memory
  return retval;
}// Execute

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabExecDlg::DoInitDialog()
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::DoInitDialog"));
#endif

  // Avoid multiple load of resources
  if (!SymTabExecDlgLoaded)
  {
    // Instantiate and initialise dialog
    SymTabExecDlgLoaded =
      wxXmlResource::Get()->LoadObject(this, parent, _T("dlgSymTabExec"),_T("wxScrollingDialog"));

    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);

    m_ListCtrl = XRCCTRL(*this, "lstLib2Symbol",     wxListCtrl);
    // Setting colums names
    m_ListCtrl->InsertColumn(0, _T("item"),  wxLIST_FORMAT_RIGHT);
    m_ListCtrl->InsertColumn(1, _T("value"), wxLIST_FORMAT_LEFT);
    m_ListCtrl->InsertColumn(2, _T("type"),  wxLIST_FORMAT_CENTRE);
    m_ListCtrl->InsertColumn(3, _T("name"),  wxLIST_FORMAT_LEFT);
    // Setting help and misc
    m_TextHelp = XRCCTRL(*this, "txtHelp", wxTextCtrl);
    m_TextHelp->SetFont(font);
    m_TextMisc = XRCCTRL(*this, "txtMisc", wxTextCtrl);
    m_TextMisc->SetFont(font);
  }
}// DoInitDialog

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabExecDlg::EndModal(int retCode)
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::EndModal"));
#endif

  wxScrollingDialog::EndModal(retCode);
}// EndModal

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// This function writes the nm output to a file
void SymTabExecDlg::OnWriteToFile(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::OnWriteToFile"));
#endif

  wxString     es = wxEmptyString;
  wxFileDialog fd(parent, _("Save NM output to file"), es, es, _T("*.*"), wxFD_SAVE);

  if (fd.ShowModal() == wxID_OK)
  {
    wxFFile file(fd.GetPath().c_str(), _T("w"));
    for (size_t n=0; n<nm_result.GetCount(); ++n)
    {
      file.Write(nm_result[n]);
      file.Write(_T("\n"));
    }
    file.Close();
  }
}// OnWriteToFile

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabExecDlg::OnNext(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::OnNext"));
#endif

  wxScrollingDialog::EndModal(wxID_OK);
}// OnNext

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabExecDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::OnCancel"));
#endif

  wxScrollingDialog::EndModal(wxID_CANCEL);
}// OnCancel

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// struct used as user-data for columns
struct customListEntry
{
  customListEntry(long i, const wxString& col1, const wxString& col2, const wxString& col3)
  {
    index = i;
    column_texts[0] = col1; // value
    column_texts[1] = col2; // type
    column_texts[2] = col3; // name
  }

  // the entry's index
  long index;
  // keeps the text for each column after 0
  wxString column_texts[3]; // value, type, name
}; // customListEntry

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// clears all data to free memory
void SymTabExecDlg::CleanUp()
{
  nm_result.Empty(); // remove old results
  nm_errors.Empty(); // remove old erros
  ClearUserData();
}// CleanUp

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// clears the list control allocated user-data
void SymTabExecDlg::ClearUserData()
{
  for (long i = 0; i < m_ListCtrl->GetItemCount(); ++i)
  {
    customListEntry* data = (customListEntry*)m_ListCtrl->GetItemData(i);
    delete data;
  }
  m_ListCtrl->DeleteAllItems();
}// ClearUserData

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// Sorting function of the nm output columns
inline int wxCALLBACK SortFunction(intptr_t item1, intptr_t item2, intptr_t dlg)
{
  SymTabExecDlg   *dialog = (SymTabExecDlg*)   dlg;
  customListEntry *data1  = (customListEntry*) item1;
  customListEntry *data2  = (customListEntry*) item2;

  if (!data1)
    return -1;
  if (!data2)
    return 1;

  // All the columns are composed with characters, except the first column
  long col       = dialog->GetSortColumn();
  bool ascending = dialog->GetSortAscending();
  if (col == 0)
  {
    int ret = data1->index - data2->index; // simple arithmetic
    if (ascending)
      return ret;
    else
      return -ret;
  }

  // adjust
  col -= 1;

  int ret = data1->column_texts[col].CmpNoCase(data2->column_texts[col]);
  if (ascending)
    return ret;
  else
    return -ret;
}// SortFunction

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// Function called when a column header is clicked
void SymTabExecDlg::OnColumnClick(wxListEvent& event)
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::OnColumnClick"));
#endif

  if (event.GetColumn() != ms_iSortColumn)
    ms_bSortAscending = true;
  else
    ms_bSortAscending = !ms_bSortAscending;

  ms_iSortColumn = event.GetColumn();
  wxBusyInfo wait(_("Please wait, sorting..."));
  m_ListCtrl->SortItems(SortFunction, (intptr_t)this);
}// OnColumnClick

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
/* ----- ----- ----- ----- -----PRIVATE----- ----- ----- ----- ----- ----- */
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabExecDlg::ExecuteMulti(struct_config &config, wxString cmd)
{
  wxString the_symbol = config.txtSymbol.Trim();
  wxDir dir(config.txtLibraryPath);
  // wxDir failed (already has logged an error message with the exact failure)
  if (!dir.IsOpened())
    return -1;

  size_t        num_files = 0;
  wxArrayString files;

  // Note: GetAllFiles *appends* to an wxArrayString
  if (config.chkIncludeA)
    num_files += wxDir::GetAllFiles(config.txtLibraryPath, &files, _T("*.a"));
  if (config.chkIncludeLib)
    num_files += wxDir::GetAllFiles(config.txtLibraryPath, &files, _T("*.lib"));
  if (config.chkIncludeO)
    num_files += wxDir::GetAllFiles(config.txtLibraryPath, &files, _T("*.o"));
  if (config.chkIncludeObj)
    num_files += wxDir::GetAllFiles(config.txtLibraryPath, &files, _T("*.obj"));
  if (config.chkIncludeDll)
    num_files += wxDir::GetAllFiles(config.txtLibraryPath, &files, _T("*.dll"));

  if (!num_files)
  {
    cbMessageBox(_("Could not find any files matching the criteria."), _("Error"),
                 wxICON_ERROR | wxOK, (wxWindow*)Manager::Get()->GetAppWindow());
  }// if
  else
  {
    XRCCTRL(*this, "btnNext", wxButton)->Enable(true);

    bool something_found = false;
    for (size_t i=0; i<num_files; i++)
    {
      // Compile nm command for this library (file)
      wxString this_cmd = cmd;
      this_cmd << _T(" \"") << files[i] << _T("\"");

      if (!ExecuteNM(files[i], this_cmd)) // fatal.
        return -1;

      int parse_result = ParseOutput(files[i], the_symbol);
      if (parse_result != 0)
      {
        something_found = true;
        // In the last step, disable the "Next" button
        if (i==(num_files-1))
          XRCCTRL(*this, "btnNext", wxButton)->Enable(false);
        XRCCTRL(*this, "stxtFile", wxStaticText)->SetLabel(_("File: ") + files[i]);
        int retval = wxScrollingDialog::ShowModal();
        if      (retval == wxID_OK)
          ;      // continue
        else if (retval == wxID_CANCEL)
        {
//          cbMessageBox(_("Operation cancelled."), _("Info"),
//                       wxICON_INFORMATION | wxOK,
//                       (wxWindow*)Manager::Get()->GetAppWindow());
          break; // cancel operation on any further files
        }
        else
          return -1;
      }
    }// for
    if (!something_found)
    {
      wxString msg;
      msg << _("The search for \"") << the_symbol
          << _("\" produced no results.");
      cbMessageBox(msg, _("Info"), wxICON_INFORMATION | wxOK,
                   (wxWindow*)Manager::Get()->GetAppWindow());
    }
  }// else

  return (wxID_OK);
}// ExecuteMulti

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabExecDlg::ExecuteSingle(struct_config &config, wxString cmd)
{
  wxString the_library = config.txtLibrary.Trim();
  wxString the_symbol  = config.txtSymbol.Trim();
  cmd << _T(" \"") << the_library << _T("\"");
  if (!ExecuteNM(the_library, cmd))
    return -1;

  int retval = ParseOutput(the_library, the_symbol);
  if (retval == 0)
  {
    wxString msg;
    msg << _("The search in:\n") << the_library
        << _("\nfor \"") << the_symbol << _("\" produced no results.");
    cbMessageBox(msg, _("Info"), wxICON_INFORMATION | wxOK,
                 (wxWindow*)Manager::Get()->GetAppWindow());
  }
  else
  {
    XRCCTRL(*this, "stxtFile", wxStaticText)->SetLabel(_("File: ") + the_library);
    XRCCTRL(*this, "btnNext",  wxButton)->Enable(false); // there is no next
    wxScrollingDialog::ShowModal(); // Done on success only.
  }

  return (wxID_OK); // always success.
}// ExecuteSingle

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

bool SymTabExecDlg::ExecuteNM(wxString lib, wxString cmd)
{
#ifdef TRACE_SYMTAB_EXE
  Manager::Get()->GetLogManager()->(_T("SymTabExecDlg::ExecuteNM"));
  Manager::Get()->GetLogManager()->(_T("Library: ")   + lib);
#endif
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTab: Executing: ") + cmd);

  wxString p_msg;
  p_msg << _("Launching NM tool for:\n") << lib
        << _("\nPlease wait, this can take a long time...");
  wxBusyInfo* wait = new wxBusyInfo(p_msg);

  CleanUp(); // Clean any old outputs
  int pid = wxExecute(cmd, nm_result, nm_errors);

  if (wait) delete wait;

  // Process could not be launched.
  if (pid == -1)
  {
    wxString msg;
    msg << _("Unable to execute NM.\n")
        << _("Be sure it is in the OS global path.\n")
        << _("SymTab could not complete the operation.");
    cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK,
                 (wxWindow*)Manager::Get()->GetAppWindow());
    return false;
  }

  return true;
}// ExecuteNM

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabExecDlg::ParseOutput(wxString lib, wxString filter)
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->(_T("SymTabExecDlg::ParseOutput"));
#endif

  int symbols_processed = -1;

  // No output to std::cout -> error, show error message
  if(nm_result.IsEmpty())
    ParseOutputError();
  // Output to std::cout    -> success, process
  else
    symbols_processed = ParseOutputSuccess(lib, filter);

	if (symbols_processed == 0)
    Manager::Get()->GetLogManager()->DebugLog(_T("SymTab: Parsing produced no match (no results)."));

  return symbols_processed;
}// ParseOutput

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabExecDlg::ParseOutputError()
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::ParseOutputError"));
#endif

  wxString output;
  size_t   count = nm_errors.GetCount();

  if (count)
  {
    for (size_t n = 0; n < count; ++n )
      output << nm_errors[n] << _T("\n");
  }
  else
  {
    output = _("An unknown error has occured. NM produced no output.");
  }

  m_TextMisc->SetValue(output);
  wxColour colour(255,0,0);
  m_TextMisc->SetForegroundColour(colour);

  // Select tab with error message.
  XRCCTRL(*this, "nbTabs", wxNotebook)->SetSelection(1);
}// ParseOutputError

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabExecDlg::ParseOutputSuccess(wxString lib, wxString filter)
{
#ifdef TRACE_SYMTAB_EXE
	Manager::Get()->GetLogManager()->DebugLog(_T("SymTabExecDlg::ParseOutputSuccess"));
#endif

  size_t count = nm_result.GetCount();
  if (!count)
  {
#ifdef TRACE_SYMTAB_EXE
    Manager::Get()->GetLogManager()->DebugLog(_T("SymTab: No entries. Skipping."));
#endif
    return 0;
  }

	Manager::Get()->GetLogManager()->DebugLog(F(_T("SymTab: Parsing %lu items..."), static_cast<unsigned long>(count)));

  wxProgressDialog* progress = 0L;
  if (count>2000) // avoid flickering for small libs
  {
    wxString p_msg;
    p_msg << _("Parsing NM information for:\n") << lib << _("\nPlease wait...");
    progress = new wxProgressDialog(_("SymTab plugin"), p_msg);
  }

  // caching variables (not to be re-instantiated every time in the for-loop)
  int      entries = 0;
  bool     do_show = true;
  wxString the_line;
  wxString the_value;
  wxString the_type;
  wxString the_name;
  wxString s_item;

  // Parsing output
  for (size_t n = 0 ; n < count; ++n )
  {
    the_line = nm_result[n];
    if (!the_line.IsEmpty())
    {
      if (!filter.IsEmpty())
        do_show = the_line.Contains(filter);

      if (do_show)
      {
        long item = m_ListCtrl->InsertItem(entries, _T(""));

        if (item!=-1)
        {
          s_item.Printf(_T("%6ld"), item);
          m_ListCtrl->SetItem(item, 0, s_item);

          // Symbols can have colons in it, too        -> like Class::Method()
          // Only object files have a colon at the end -> like ABC.o:
          if (the_line.Last() == _T(':'))
          {
            m_ListCtrl->SetItem(item, 3, the_line.Trim());
            m_ListCtrl->SetItemBackgroundColour(item,
              wxTheColourDatabase->Find(_T("LIGHT GREY")));
          }
          else
          {
            the_value = ((the_line.Mid( 0,8)).Trim(true)).Trim();
            m_ListCtrl->SetItem(item, 1, the_value);

            the_type  = ((the_line.Mid( 9,1)).Trim(true)).Trim();
            m_ListCtrl->SetItem(item, 2, the_type);

            the_name  = ((the_line.Mid(11  )).Trim(true)).Trim();
            m_ListCtrl->SetItem(item, 3, the_name);
            if (the_name.IsEmpty())
              m_ListCtrl->SetItemBackgroundColour(item,
                wxTheColourDatabase->Find(_T("RED")));
          }

          // now associate a user-data with this entry
          m_ListCtrl->SetItemData(item, (intptr_t)new customListEntry(n, the_value, the_type, the_name));

          ++entries;
        }
      }// if
    }// if

    if (progress)
      progress->Update((100*n)/(count-1));
  }// for

#ifdef TRACE_SYMTAB_EXE
  Manager::Get()->GetLogManager()->DebugLog(_T("SymTab: Operated %d entries."), entries);
#endif

  if (entries)
  {
    // Resize columns
    m_ListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE           );
    m_ListCtrl->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
    m_ListCtrl->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
    m_ListCtrl->SetColumnWidth(3, wxLIST_AUTOSIZE           );
  }

  if (progress)
  {
    progress->Update(100);
    progress->Destroy();
  }

  return entries;
}// ParseOutputSuccess
