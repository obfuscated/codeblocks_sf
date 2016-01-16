/***************************************************************
 * Name:      Cccc.cpp
 * Purpose:   Code::Blocks Cccc plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   12/11/2009
 * Copyright: (c) Lieven de Cock (aka killerbot)
 * License:   GPL
  **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "manager.h"
#include "logmanager.h"
#include "projectmanager.h"
#endif
#include <wx/busyinfo.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/utils.h>
#include "loggers.h"
#include "Cccc.h"

// Register the plugin
namespace
{
    PluginRegistrant<Cccc> reg(_T("Cccc"));
};

Cccc::Cccc()
{
    if(!Manager::LoadResource(_T("Cccc.zip")))
    {
        NotifyMissingFile(_T("Cccc.zip"));
    }
    m_CcccLog = 0;
    m_LogPageIndex = 0; // good init value ???
    m_CcccApp = _T("cccc");
} // end of constructor

Cccc::~Cccc()
{
} // end of destruccor

void Cccc::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    if(LogManager* LogMan = Manager::Get()->GetLogManager())
    {
        m_CcccLog = new TextCtrlLogger();
        m_LogPageIndex = LogMan->SetLog(m_CcccLog);
        LogMan->Slot(m_LogPageIndex).title = _("Cccc");
        CodeBlocksLogEvent evtAdd1(cbEVT_ADD_LOG_WINDOW, m_CcccLog, LogMan->Slot(m_LogPageIndex).title);
        Manager::Get()->ProcessEvent(evtAdd1);
    }
} // end of OnAttach

void Cccc::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be FALSE...
    if(Manager::Get()->GetLogManager())
    {
        if(m_CcccLog)
        {
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_CcccLog);
            Manager::Get()->ProcessEvent(evt);
        }
    }
    m_CcccLog = 0;
} // end of OnRelease

void Cccc::AppendToLog(const wxString& Text)
{
    if(LogManager* LogMan = Manager::Get()->GetLogManager())
    {
        CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_CcccLog);
        Manager::Get()->ProcessEvent(evtSwitch);
        LogMan->Log(Text, m_LogPageIndex);
    }
} // end of AppendToLog

namespace
{
bool CheckRequirements()
{
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    // if no project open, exit
    if (!Project)
    {
        wxString msg = _("You need to open a project\nbefore using the plugin!");
        cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        Manager::Get()->GetLogManager()->DebugLog(msg);
        return false;
    }
    return true;
}  // end of CheckRequirements
}

int Cccc::Execute()
{
    if(!CheckRequirements())
    {
        return -1;
    }

    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    ::wxSetWorkingDirectory(Project->GetBasePath());
    wxString ListOfFileNames;
    for (FilesList::iterator it = Project->GetFilesList().begin(); it != Project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        ListOfFileNames += _T("\"") + pf->relativeFilename + _T("\" ");
    }

    wxString CommandLine = m_CcccApp + _T(" ") + ListOfFileNames.Trim();
    AppendToLog(CommandLine);
    wxArrayString Output, Errors;
    long pid = -1;
    {
        wxWindowDisabler disableAll;
        wxBusyInfo running(_("Running cccc... please wait (this may take several minutes)..."),
                           Manager::Get()->GetAppWindow());
        pid = wxExecute(CommandLine, Output, Errors);
    } // end lifetime of wxWindowDisabler, wxBusyInfo
    if (pid==-1)
    {
        bool failed = true;
        if (cbMessageBox(_("Failed to lauch cccc.\nDo you want to select the cccc executable?"),
                         _("Question"), wxICON_QUESTION | wxYES_NO, Manager::Get()->GetAppWindow()) == wxID_YES)
        {
            wxString filename = wxFileSelector(_("Select the cccc executable"));
            if (!filename.empty()) // otherwise the user selected cancel
            {
                // try again using the user-provided executable
                CommandLine = filename + _T(" ") + ListOfFileNames.Trim();
                AppendToLog(CommandLine);
                {
                    wxWindowDisabler disableAll;
                    wxBusyInfo running(_("Running cccc... please wait (this may take several minutes)..."),
                                       Manager::Get()->GetAppWindow());
                    pid = wxExecute(CommandLine, Output, Errors);
                } // end lifetime of wxWindowDisabler, wxBusyInfo
                if (pid==-1)
                {
                    failed = true;
                }
                else
                {
                    m_CcccApp = filename;
                    failed = false;
                }
            }
        }
        if (failed)
        {
            AppendToLog(_("Failed to lauch cccc."));
            cbMessageBox(_("Failed to lauch cccc."), _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
            return -1;
        }
    }

    size_t Count = Output.GetCount();
    for(size_t idxCount = 0; idxCount < Count; ++idxCount)
    {
        AppendToLog(Output[idxCount]);
    } // end for : idx: idxCount
    Count = Errors.GetCount();
    for(size_t idxCount = 0; idxCount < Count; ++idxCount)
    {
        AppendToLog(Errors[idxCount]);
    } // end for : idx: idxCount
    const wxString FileName = _T("./.cccc/cccc.html");
    if(wxFile::Exists(FileName))
    {
        if (cbMimePlugin* p = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(FileName))
        {
            p->OpenFile(FileName);
        }
    }

    return 0;
} // end of Execute
