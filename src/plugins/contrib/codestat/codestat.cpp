/***************************************************************
 * Name:      codestat.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/
 
#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "cbprofiler.h"
#endif
#include "codestat.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <configmanager.h>
#include "codestatconfig.h"
#include "codestatexec.h"
#include <cbproject.h>
#include <manager.h>
#include <projectmanager.h>
#include <messagemanager.h>
//#include <wx/choicdlg.h>

cbPlugin* GetPlugin()
{
    return new CodeStat;
}
CodeStat::CodeStat()
{
    //ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + _T("/codestat.zip#zip:*.xrc"));
    
    m_PluginInfo.name = _T("CodeStatistics");
    m_PluginInfo.title = _("Code Statistics");
    m_PluginInfo.version = _("0.1");
    m_PluginInfo.description = _("A simple plugin for counting code, comments and empty lines of a project.");
    m_PluginInfo.author = _("Zlika");
    m_PluginInfo.authorEmail = _("");
    m_PluginInfo.authorWebsite = _("");
    m_PluginInfo.thanksTo = _("All the Code::Blocks team!");
    m_PluginInfo.license = LICENSE_GPL;
    m_PluginInfo.hasConfigure = true;
    
    ConfigManager::AddConfiguration(m_PluginInfo.title, _T("/codestat"));
}
CodeStat::~CodeStat()
{
    //dtor
}
void CodeStat::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
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
    // m_IsAttached will be FALSE...
}
int CodeStat::Configure()
{
    // if not attached, exit
    if (!m_IsAttached)
        return -1;
    
    LanguageDef languages[NB_FILETYPES];
    LoadSettings(languages);
    CodeStatConfigDlg dlg(Manager::Get()->GetAppWindow(), languages);
    if (dlg.ShowModal() == wxID_OK)
    {
    }
    return 0;
}
int CodeStat::Execute()
{
    // if not attached, exit
    if (!m_IsAttached)
        return -1;
        
   cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
   // if no project open, exit
	if (!project)
	{
		wxString msg = _("You need to open a project\nbefore using the plugin!");
		wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}
	
    dlg = new CodeStatExecDlg(Manager::Get()->GetAppWindow());
    
    // Load the language settings and launch the main function
    LanguageDef languages[NB_FILETYPES];
    LoadSettings(languages);
    if(dlg->Execute(languages) != 0)
        return -1;
   
    return 0;
}

// Definition of the comments for each language
void CodeStat::LoadSettings(LanguageDef languages[NB_FILETYPES])
{
	// C/C++ style comments
	languages[0].name = _T("C/C++");
	languages[0].ext.Add(_T("c"));
	languages[0].ext.Add(_T("cpp"));
	languages[0].ext.Add(_T("h"));
	languages[0].ext.Add(_T("hpp"));
	languages[0].single_line_comment = _T("//");
   languages[0].multiple_line_comment[0] = _T("/*");
   languages[0].multiple_line_comment[1] = _T("*/");
   
   // Java style comments
	languages[1].name = _T("Java");
	languages[1].ext.Add(_T("java"));
	languages[1].single_line_comment = _T("//");
   languages[1].multiple_line_comment[0] = _T("/*");
   languages[1].multiple_line_comment[1] = _T("*/");
   
   // Python style comments
	languages[2].name = _T("Python");
	languages[2].ext.Add(_T("py"));
	languages[2].single_line_comment = _T("#");
   languages[2].multiple_line_comment[0] = _T("");
   languages[2].multiple_line_comment[1] = _T("");
}
