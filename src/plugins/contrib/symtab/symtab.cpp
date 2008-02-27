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
  #include <wx/intl.h>
  #include <wx/string.h>
  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
#endif

//#define TRACE_SYMTAB
#ifdef TRACE_SYMTAB
  #ifndef CB_PRECOMP
    #include "logmanager.h"
  #endif
#endif

#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include "symtab.h"
#include "symtabconfig.h"
#include "symtabexec.h"

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

// Register the plugin
namespace
{
  PluginRegistrant<SymTab> reg(_T("SymTab"));
};

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

SymTab::SymTab() : CfgDlg(0L), ExeDlg(0L)
{
  //ctor
  if(!Manager::LoadResource(_T("SymTab.zip")))
  {
    NotifyMissingFile(_T("SymTab.zip"));
  }
}// SymTab

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

SymTab::~SymTab()
{
  //dtor
}// ~SymTab

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTab::OnAttach()
{
  // do whatever initialization you need for your plugin
  // NOTE: after this function, the inherited member variable
  // IsAttached() will be TRUE...
  // You should check for it in other functions, because if it
  // is FALSE, it means that the application did *not* "load"
  // (see: does not need) this plugin...
}// OnAttach

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

void SymTab::OnRelease(bool appShutDown)
{
  // do de-initialization for your plugin
  // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
  // which means you must not use any of the SDK Managers
  // NOTE: after this function, the inherited member variable
  // IsAttached() will be FALSE...
  if (CfgDlg) {CfgDlg->Destroy(); CfgDlg = 0L;}
  if (ExeDlg) {ExeDlg->Destroy(); ExeDlg = 0L;}
}// OnRelease

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */

int SymTab::Execute()
{
  // if not attached, exit
  if (!IsAttached())
    return -1;

#ifdef TRACE_SYMTAB
	Manager::Get()->GetLogManager()->DebugLog(F(_T("SymTab::Execute")));
#endif

  if (!CfgDlg)
    CfgDlg = new SymTabConfigDlg(Manager::Get()->GetAppWindow());

  if ((!CfgDlg) || (CfgDlg->Execute() != wxID_OK))
    return -1;

  // Load the config settings
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("symtab"));

  // Loading configuration
  struct_config config;
  config.choWhatToDo    = cfg->ReadInt (_T("/what_to_do"),   0);

  config.txtLibraryPath = cfg->Read    (_T("/library_path"), wxEmptyString);
  config.chkIncludeA    = cfg->ReadBool(_T("/include_a"),    true);
  config.chkIncludeLib  = cfg->ReadBool(_T("/include_lib"),  true);
  config.chkIncludeO    = cfg->ReadBool(_T("/include_o"),    false);
  config.chkIncludeObj  = cfg->ReadBool(_T("/include_obj"),  false);
  config.chkIncludeDll  = cfg->ReadBool(_T("/include_dll"),  false);

  config.txtLibrary     = cfg->Read    (_T("/library"),      wxEmptyString);

  config.txtSymbol      = cfg->Read    (_T("/symbol"),       wxEmptyString);

  config.txtNM          = cfg->Read    (_T("/nm"),           wxEmptyString);

  config.chkDebug       = cfg->ReadBool(_T("/debug"),        false);
  config.chkDefined     = cfg->ReadBool(_T("/defined"),      false);
  config.chkDemangle    = cfg->ReadBool(_T("/demangle"),     false);
  config.chkExtern      = cfg->ReadBool(_T("/extern"),       false);
  config.chkSpecial     = cfg->ReadBool(_T("/special"),      false);
  config.chkSynthetic   = cfg->ReadBool(_T("/synthetic"),    false);
  config.chkUndefined   = cfg->ReadBool(_T("/undefined"),    false);

  // If we got this far, all is left is to call nm
  if (!ExeDlg)
    ExeDlg = new SymTabExecDlg(Manager::Get()->GetAppWindow());

  // Do we need to show the dialog (process successful)?
  if ((!ExeDlg) || (ExeDlg->Execute(config) != wxID_OK))
    return -1;

  return 0;
}// Execute
