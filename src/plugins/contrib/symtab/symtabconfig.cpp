#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/button.h>
  #include <wx/checkbox.h>
  #include <wx/choice.h>
  #include <wx/event.h>
  #include <wx/intl.h>
  #include <wx/string.h>
  #include <wx/textctrl.h>
  #include <wx/xrc/xmlres.h>
  #include "globals.h" // cbMessageBox
  #include "manager.h"
  #include "configmanager.h"
#endif

//#define TRACE_SYMTAB_CFG
#ifdef TRACE_SYMTAB_CFG
  #ifndef CB_PRECOMP
    #include "messagemanager.h"
  #endif
#endif

#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include "symtabconfig.h"
#include "prep.h"

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

BEGIN_EVENT_TABLE(SymTabConfigDlg, wxDialog)
  EVT_BUTTON(XRCID("btnSearch"),      SymTabConfigDlg::OnSearch)
  EVT_BUTTON(XRCID("btnClose"),       SymTabConfigDlg::OnClose)
  EVT_CHOICE(XRCID("choWhatToDo"),    SymTabConfigDlg::OnWhatToDo)
  EVT_BUTTON(XRCID("btnLibraryPath"), SymTabConfigDlg::OnLibraryPath)
  EVT_BUTTON(XRCID("btnLibrary"),     SymTabConfigDlg::OnLibrary)
  EVT_BUTTON(XRCID("btnNM"),          SymTabConfigDlg::OnNM)
END_EVENT_TABLE()

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

SymTabConfigDlg::~SymTabConfigDlg()
{
  //dtor
}// ~SymTabConfigDlg

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTabConfigDlg::Execute()
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::Execute"));
#endif

  // Avoid multiple load of resources
  if (!SymTabConfigDlgLoaded)
  {
    // Instantiate and initialise dialog
    SymTabConfigDlgLoaded =
      wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgSymTabConfig"));
  }

  LoadSettings();
  return wxDialog::ShowModal();
}// Execute

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
/* ----- ----- ----- ----- -----PRIVATE----- ----- ----- ----- ----- ----- */
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::EndModal(int retCode)
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::EndModal"));
#endif

  wxDialog::EndModal(retCode);
}// EndModal

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnWhatToDo(wxCommandEvent& event)
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnWhatToDo"));
#endif

  int choice = event.GetInt();
  ToggleWidgets(choice);
}// OnWhatToDo

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnSearch(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnSearch"));
#endif

  // user pressed Search; save settings
  SaveSettings();

  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("symtab"));
  int choice = cfg->ReadInt(_T("/what_to_do"), 0);

  // Search for a symbol given a library path
  if      (choice==0)
  {
    wxString library_path = (cfg->Read(_T("/library_path"))).Trim();
    if (library_path.IsEmpty())
    {
      cbMessageBox(_("No library path provided."), _("Error"), wxICON_ERROR | wxOK,
                   (wxWindow*)Manager::Get()->GetAppWindow());
      return;
    }

    if ( !(   cfg->ReadBool(_T("/include_a"),   true)
           || cfg->ReadBool(_T("/include_lib"), true)
           || cfg->ReadBool(_T("/include_o"),   false)
           || cfg->ReadBool(_T("/include_obj"), false)
           || cfg->ReadBool(_T("/include_dll"), false)) )
    {
      cbMessageBox(_("No file type (include) provided."), _("Error"), wxICON_ERROR | wxOK,
                   (wxWindow*)Manager::Get()->GetAppWindow());
      return;
    }

    wxString symbol = (cfg->Read(_T("/symbol"))).Trim();
    if (symbol.IsEmpty())
    {
      wxString msg;
      msg << _("Warning: You did not select a symbol to search for in a path.\n")
          << _("You may operate on many files - this can be a lengthy operation.\n")
          << _("Are you really sure that you want to do this?");
      if (cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO,
                       (wxWindow*)Manager::Get()->GetAppWindow()) == wxID_NO)
        return;
    }
  }
  // Search for all symbols in a given library
  else if (choice==1)
  {
    wxString library = (cfg->Read(_T("/library"))).Trim();
    if (library.IsEmpty())
    {
      cbMessageBox(_("No library provided."), _("Error"), wxICON_ERROR | wxOK,
                   (wxWindow*)Manager::Get()->GetAppWindow());
      return;
    }
  }

  wxDialog::EndModal(wxID_OK);
}// OnSearch

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnClose(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnClose"));
#endif

  wxDialog::EndModal(wxID_CANCEL);
}// OnClose

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnLibraryPath(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnLibraryPath"));
#endif

  wxDirDialog dd(parent, _("Select directory for search"));
  if (dd.ShowModal() == wxID_OK)
  {
    wxString path = dd.GetPath();
    XRCCTRL(*this, "txtLibraryPath", wxTextCtrl)->SetValue(path);
  }
}// OnLibraryPath

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnLibrary(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnLibrary"));
#endif

  wxString caption  = _T("Choose a (library) file");
  wxString wildcard;
           wildcard << _T("Library files (*.a)|*.a|")
                    << _T("Library files (*.lib)|*.lib|")
                    << _T("Object files (*.o)|*.o|")
                    << _T("Object files (*.obj)|*.obj|")
#ifdef __WXMSW__
                    << _T("Object files (*.dll)|*.dll|")
                    << _T("All files (*.*)|*.*");
#else
                    << _T("All files (*)|*");
#endif
  wxString es       = wxEmptyString;

  wxFileDialog fd(parent, caption, es, es, wildcard, wxOPEN|compatibility::wxHideReadonly);
  if (fd.ShowModal() == wxID_OK)
  {
    wxString path = fd.GetPath();
    //int filterIndex = fd.GetFilterIndex();
    XRCCTRL(*this, "txtLibrary", wxTextCtrl)->SetValue(path);
  }
}// OnLibrary

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::OnNM(wxCommandEvent& WXUNUSED(event))
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::OnNM"));
#endif

  wxString caption  = _T("Choose NM application");
  wxString wildcard;
  if (platform::windows)
    wildcard = _T("All files (*.*)|*.*");
  else
    wildcard = _T("All files (*)|*");

  wxString es = wxEmptyString;

  wxFileDialog fd(parent, caption, es, es, wildcard, wxOPEN|compatibility::wxHideReadonly);
  if (fd.ShowModal() == wxID_OK)
  {
    wxString path = fd.GetPath();
    //int filterIndex = fd.GetFilterIndex();
    XRCCTRL(*this, "txtNM", wxTextCtrl)->SetValue(path);
  }
}// OnNM

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::ToggleWidgets(int choice)
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::ToggleWidgets"));
#endif

  // Strategy: Disable all widgets, enable required.
  XRCCTRL(*this, "txtLibraryPath", wxTextCtrl)->Enable(false);
  XRCCTRL(*this, "btnLibraryPath", wxButton)->Enable(false);
  XRCCTRL(*this, "chkIncludeA",    wxCheckBox)->Enable(false);
  XRCCTRL(*this, "chkIncludeLib",  wxCheckBox)->Enable(false);
  XRCCTRL(*this, "chkIncludeO",    wxCheckBox)->Enable(false);
  XRCCTRL(*this, "chkIncludeObj",  wxCheckBox)->Enable(false);
  XRCCTRL(*this, "chkIncludeDll",  wxCheckBox)->Enable(false);

  XRCCTRL(*this, "txtLibrary",     wxTextCtrl)->Enable(false);
  XRCCTRL(*this, "btnLibrary",     wxButton)->Enable(false);

  // Search for a symbol given a library path
  if      (choice==0)
  {
    XRCCTRL(*this, "txtLibraryPath", wxTextCtrl)->Enable(true);
    XRCCTRL(*this, "btnLibraryPath", wxButton)->Enable(true);
    XRCCTRL(*this, "chkIncludeA",    wxCheckBox)->Enable(true);
    XRCCTRL(*this, "chkIncludeLib",  wxCheckBox)->Enable(true);
    XRCCTRL(*this, "chkIncludeO",    wxCheckBox)->Enable(true);
    XRCCTRL(*this, "chkIncludeObj",  wxCheckBox)->Enable(true);
    XRCCTRL(*this, "chkIncludeDll",  wxCheckBox)->Enable(true);
  }
  // Search for all symbols in a given library
  else if (choice==1)
  {
    XRCCTRL(*this, "txtLibrary",     wxTextCtrl)->Enable(true);
    XRCCTRL(*this, "btnLibrary",     wxButton)->Enable(true);
  }
}// ToggleWidgets

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::LoadSettings()
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::LoadSettings"));
#endif

  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("symtab"));

  // What to do options
  int choice = cfg->ReadInt(_T("/what_to_do"), 0);
  XRCCTRL(*this, "choWhatToDo",     wxChoice)->SetSelection(choice);
  ToggleWidgets(choice); // Event won't fire if not changed,
                         // do manual to ensure proper initialisation
  XRCCTRL(*this, "txtLibraryPath",    wxTextCtrl)->SetValue(
    cfg->Read(_T("/library_path"),    _T("")));
  XRCCTRL(*this, "chkIncludeA",       wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/include_a"),   true));
  XRCCTRL(*this, "chkIncludeLib",     wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/include_lib"), true));
  XRCCTRL(*this, "chkIncludeO",       wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/include_o"),   false));
  XRCCTRL(*this, "chkIncludeObj",     wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/include_obj"), false));
  XRCCTRL(*this, "chkIncludeDll",     wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/include_dll"), false));

  XRCCTRL(*this, "txtLibrary",        wxTextCtrl)->SetValue(
    cfg->Read(_T("/library"),         _T("")));

  XRCCTRL(*this, "txtSymbol",         wxTextCtrl)->SetValue(
    cfg->Read(_T("/symbol"),          _T("")));

  // Options
  XRCCTRL(*this, "txtNM",             wxTextCtrl)->SetValue(
    cfg->Read(_T("/nm"),              _T("")));

  XRCCTRL(*this, "chkDebug",          wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/debug"),       false));
  XRCCTRL(*this, "chkDefined",        wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/defined"),     false));
  XRCCTRL(*this, "chkDemangle",       wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/demangle"),    false));
  XRCCTRL(*this, "chkExtern",         wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/extern"),      false));
  XRCCTRL(*this, "chkSpecial",        wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/special"),     false));
  XRCCTRL(*this, "chkSynthetic",      wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/synthetic"),   false));
  XRCCTRL(*this, "chkUndefined",      wxCheckBox)->SetValue(
    cfg->ReadBool(_T("/undefined"),   false));
}// LoadSettings

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTabConfigDlg::SaveSettings()
{
#ifdef TRACE_SYMTAB_CFG
	DBGLOG(_T("SymTabConfigDlg::SaveSettings"));
#endif

  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("symtab"));

  // What to do options
  cfg->Write(_T("/what_to_do"),
    XRCCTRL(*this, "choWhatToDo",    wxChoice  )->GetSelection());

  cfg->Write(_T("/library_path"),
    XRCCTRL(*this, "txtLibraryPath", wxTextCtrl)->GetValue().Trim());
  cfg->Write(_T("/include_a"),
    XRCCTRL(*this, "chkIncludeA",    wxCheckBox)->GetValue());
  cfg->Write(_T("/include_lib"),
    XRCCTRL(*this, "chkIncludeLib",  wxCheckBox)->GetValue());
  cfg->Write(_T("/include_o"),
    XRCCTRL(*this, "chkIncludeO",    wxCheckBox)->GetValue());
  cfg->Write(_T("/include_obj"),
    XRCCTRL(*this, "chkIncludeObj",  wxCheckBox)->GetValue());
  cfg->Write(_T("/include_dll"),
    XRCCTRL(*this, "chkIncludeDll",  wxCheckBox)->GetValue());

  cfg->Write(_T("/library"),
    XRCCTRL(*this, "txtLibrary",     wxTextCtrl)->GetValue().Trim());

  cfg->Write(_T("/symbol"),
    XRCCTRL(*this, "txtSymbol",      wxTextCtrl)->GetValue().Trim());

  // Options
  cfg->Write(_T("/nm"),
    XRCCTRL(*this, "txtNM",          wxTextCtrl)->GetValue().Trim());

  cfg->Write(_T("/debug"),
    XRCCTRL(*this, "chkDebug",       wxCheckBox)->GetValue());
  cfg->Write(_T("/defined"),
    XRCCTRL(*this, "chkDefined",     wxCheckBox)->GetValue());
  cfg->Write(_T("/demangle"),
    XRCCTRL(*this, "chkDemangle",    wxCheckBox)->GetValue());
  cfg->Write(_T("/extern"),
    XRCCTRL(*this, "chkExtern",      wxCheckBox)->GetValue());
  cfg->Write(_T("/special"),
    XRCCTRL(*this, "chkSpecial",     wxCheckBox)->GetValue());
  cfg->Write(_T("/synthetic"),
    XRCCTRL(*this, "chkSynthetic",   wxCheckBox)->GetValue());
  cfg->Write(_T("/undefined"),
    XRCCTRL(*this, "chkUndefined",   wxCheckBox)->GetValue());
}// SaveSettings
