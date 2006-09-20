/***************************************************************
 * Name:      codestat.cpp
 * Purpose:   Code::Blocks CodeStat plugin: main functions
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
  **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "configmanager.h"
#include "licenses.h" // defines some common licenses (like the GPL)
#include "manager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#endif
#include "codestat.h"
#include "codestatconfig.h"
#include "codestatexec.h"
#include "language_def.h"

// Register the plugin
namespace
{
    PluginRegistrant<CodeStat> reg(_T("CodeStat"));
};

CodeStat::CodeStat()
{
    if(!Manager::LoadResource(_T("codestat.zip")))
    {
        NotifyMissingFile(_T("codestat.zip"));
    }
}

CodeStat::~CodeStat()
{

}

void CodeStat::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void CodeStat::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be FALSE...
}

/** Open the plugin configuration panel.
 */
cbConfigurationPanel* CodeStat::GetConfigurationPanel(wxWindow* parent)
{
    // if not attached, exit
    if (!IsAttached())
        return 0;

    CodeStatConfigDlg* dlg = new CodeStatConfigDlg(parent);
    return dlg;
}

/** Main function: launch the counting process.
 *  @see CodeStatExecDlg
 */
int CodeStat::Execute()
{
    // if not attached, exit
    if (!IsAttached())
        return -1;

   const cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
   // if no project open, exit
	if (!project)
	{
		wxString msg = _("You need to open a project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}

    dlg = new CodeStatExecDlg(Manager::Get()->GetAppWindow());

    // Load the language settings and launch the main function
    LanguageDef languages[NB_FILETYPES_MAX];
    int nb_languages = LoadSettings(languages);
    int dlgReturnCode = 0;
    if(dlg->Execute(languages,nb_languages) != 0)
    {
        dlgReturnCode = -1;
    }
    dlg->Destroy();
    return dlgReturnCode;
} // end of Execute
