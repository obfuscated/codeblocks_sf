/***************************************************************
 * Name:      cbprofiler.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord
 * Created:   07/20/05 11:12:57
 * Copyright: (c) Dark Lord
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/
 
#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "cbprofiler.h"
#endif
#include "cbprofiler.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <configmanager.h>
#include "cbprofilerconfig.h"
#include "cbprofilerexec.h"
#include <cbproject.h>
#include <manager.h>
#include <projectmanager.h>
#include <messagemanager.h>
#include <wx/choicdlg.h>

cbPlugin* GetPlugin()
{
    return new CBProfiler;
}
CBProfiler::CBProfiler()
{
    //ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString);
    wxXmlResource::Get()->Load(resPath + _T("/profiler.zip#zip:*.xrc"));
    
    m_PluginInfo.name = _T("CBProfiler");
    m_PluginInfo.title = _("C::B Profiler");
    m_PluginInfo.version = _("1.0 beta3");
    m_PluginInfo.description = _("A simple graphical interface to the GNU GProf Profiler\n\nGNU GProf Online Reference:\nhttp://www.gnu.org/software/binutils/manual/gprof-2.9.1/html_mono/gprof.html");
    m_PluginInfo.author = _("Dark Lord & Zlika");
    m_PluginInfo.authorEmail = _("");
    m_PluginInfo.authorWebsite = _("");
    m_PluginInfo.thanksTo = _("Mandrav, for the n00b intro to profiling\nand the sources of his Source code\nformatter (AStyle) Plugin, whose clean\ncode structure served as a basis for this\nplugin\n:)");
    m_PluginInfo.license = LICENSE_GPL;
    m_PluginInfo.hasConfigure = true;
    
    ConfigManager::AddConfiguration(m_PluginInfo.title, _T("/cbprofiler"));
}
CBProfiler::~CBProfiler()
{
    //dtor
}
void CBProfiler::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}
void CBProfiler::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}
int CBProfiler::Configure()
{
    // if not attached, exit
    if (!m_IsAttached)
        return -1;
    
    CBProfilerConfigDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() == wxID_OK)
    {
    }
    return 0;
}
int CBProfiler::Execute()
{
    // if not attached, exit
    if (!m_IsAttached)
        return -1;
        
   cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
   // if no project open, exit
	if (!project)
	{
		wxString msg = _("You need to open a project\nbefore using the plugin!\nC::B Profiler could not complete the operation");
		wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}
	
	ProjectBuildTarget* target = 0L;
	if (project->GetBuildTargetsCount() > 1)
	{
		// more than one executable target? ask...
		wxString choices[project->GetBuildTargetsCount()];
		for (int i=0; i<project->GetBuildTargetsCount(); i++)
		   choices[i] = project->GetBuildTarget(i)->GetTitle();
		wxSingleChoiceDialog dialog(Manager::Get()->GetAppWindow(),_("Select the target you want to profile"),
                                 _("Select Target"),project->GetBuildTargetsCount(),choices);
		dialog.SetSelection(0);
		if (dialog.ShowModal() != wxID_OK)
		   return -1;
		int targetIndex = dialog.GetSelection();
      target = project->GetBuildTarget(targetIndex);
	}
	else if (project->GetBuildTargetsCount() == 1)
	   target = project->GetBuildTarget(0);
	else
	{
		// not even one executable target...
		wxString msg = _("No executable targets found in project!\nC::B Profiler could not complete the operation");
		wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}
	
	if ((target->GetTargetType() != ttExecutable) && (target->GetTargetType() != ttConsoleOnly))
	{
		wxString msg = _("The target is not executable!");
		wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}
	
	// Scope...
	wxString exename,dataname;
	
	if (target)
	{	
        exename = target->GetOutputFilename();
        wxFileName ename(exename);
        ename.Normalize(wxPATH_NORM_ALL, project->GetBasePath());
		  exename = ename.GetFullPath();
		
		  wxChar separator = wxFileName::GetPathSeparator();
        
        // The user either hasn't built the target yet or cleaned the project
        if (!ename.FileExists())
        {
            wxString msg = _("No executable found!\nYou either have not built the target or\njust cleaned the project\nTry to find profiling info?");
            if (wxMessageBox(msg,_("Confirmation"),wxYES_NO | wxICON_QUESTION) == wxNO)
                return -2;
        }
        
        // We locate gmon.out
        dataname=exename.BeforeLast(separator);
        dataname+=separator;
        dataname+=_T("gmon.out");
        wxFileName dname(dataname);
        // The gmon.out file doesn't exist?
        if (!dname.FileExists())
        {
            wxString msg = _("No profile data found!\nBe sure to enable \"Profile Code when executed\" for the current target.\nDo you want to search for the profile data file?");
            if (wxMessageBox(msg, _("Cannot find gmon.out"), wxICON_QUESTION | wxYES_NO) == wxNO)
               return -1;
            else
            {
            	wxFileDialog filedialog(Manager::Get()->GetAppWindow(), _("Locate profile information"),_T(""),_T("gmon.out"),_T("*.*"),wxOPEN|wxFILE_MUST_EXIST);
            	if (filedialog.ShowModal() == wxID_OK)
            	{
            		dataname = filedialog.GetPath();
            	   dname = wxFileName(dataname);
            	}
            	else return -1;
            }
        }
        
        // If we got so far, it means both the executable and the profile data exist
        wxDateTime exetime=ename.GetModificationTime();
        wxDateTime datatime=dname.GetModificationTime();
        
        // Profile data might be old...
        if(exetime>datatime)
        {
            wxString msg = _("It seems like the profile data is older than the executable\nYou probably have not run the executable to update this data\nContinue anyway?");
            if (wxMessageBox(msg,_("Confirmation"),wxYES_NO | wxICON_QUESTION) == wxNO)
                return -2;
        }

	}
	
	// Loading configuration
	struct_config config;
	config.chkAnnSource = ConfigManager::Get()->Read(_T("/cbprofiler/ann_source_chk"), 0L);
	config.txtAnnSource = ConfigManager::Get()->Read(_T("/cbprofiler/ann_source_txt"), _T(""));
	config.chkMinCount = ConfigManager::Get()->Read(_T("/cbprofiler/min_count_chk"), 0L);
	config.spnMinCount = ConfigManager::Get()->Read(_T("/cbprofiler/min_count_spn"), 0L);
	config.chkBrief = ConfigManager::Get()->Read(_T("/cbprofiler/brief"), 0L);
	config.chkFileInfo = ConfigManager::Get()->Read(_T("/cbprofiler/file_info"), 0L);
	config.chkNoStatic = ConfigManager::Get()->Read(_T("/cbprofiler/no_static"), 0L);
	config.chkMinCount = ConfigManager::Get()->Read(_T("/cbprofiler/min_count_chk"), 0L);
	config.spnMinCount = ConfigManager::Get()->Read(_T("/cbprofiler/min_count_spn"), 0L);
	config.chkSum = ConfigManager::Get()->Read(_T("/cbprofiler/sum"), 0L);
	config.txtExtra = ConfigManager::Get()->Read(_T("/cbprofiler/extra_txt"), _T(""));

    // If we got this far, all is left is to call gprof!!!
    dlg = new CBProfilerExecDlg(Manager::Get()->GetAppWindow());
    
    // Do we need to show the dialog (process succesful)?
    if(dlg->Execute(exename, dataname, config) != 0)
        return -1;
   
    return 0;
}
