/***************************************************************
 * Name:      CppCheck.cpp
 * Purpose:   Code::Blocks CppCheck plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   12/11/2009
 * Copyright: (c) Lieven de Cock (aka killerbot)
 * License:   GPL
  **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "cbplugin.h"
#include "manager.h"
#include "logmanager.h"
#include "pluginmanager.h"
#include "projectmanager.h"
#include "macrosmanager.h"
#endif
#include <wx/busyinfo.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/utils.h>
#include "tinyxml/tinyxml.h"
#include "loggers.h"
#include "CppCheck.h"
#include "CppCheckListLog.h"

// Register the plugin
namespace
{
    PluginRegistrant<CppCheck> reg(_T("CppCheck"));
};

CppCheck::CppCheck()
{
    if (!Manager::LoadResource(_T("CppCheck.zip")))
    {
        NotifyMissingFile(_T("CppCheck.zip"));
    }
    m_LogPageIndex = 0; // good init value ???
    m_CppCheckLog = 0;
    m_ListLog = 0;
    m_ListLogPageIndex = 0;
    m_CppCheckApp = _T("cppcheck");
} // end of constructor

CppCheck::~CppCheck()
{
} // end of destruccor

void CppCheck::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    if (LogManager* LogMan = Manager::Get()->GetLogManager())
    {
        m_CppCheckLog = new TextCtrlLogger();
        m_LogPageIndex = LogMan->SetLog(m_CppCheckLog);
        LogMan->Slot(m_LogPageIndex).title = _("CppCheck");
        CodeBlocksLogEvent evtAdd1(cbEVT_ADD_LOG_WINDOW, m_CppCheckLog, LogMan->Slot(m_LogPageIndex).title);
        Manager::Get()->ProcessEvent(evtAdd1);

        wxArrayString Titles;
        wxArrayInt Widths;
        Titles.Add(_("File"));
        Titles.Add(_("Line"));
        Titles.Add(_("Message"));
        Widths.Add(128);
        Widths.Add(48);
        Widths.Add(640);
        m_ListLog = new CppCheckListLog(Titles, Widths);
        m_ListLogPageIndex = LogMan->SetLog(m_ListLog);
        LogMan->Slot(m_ListLogPageIndex).title = _("CppCheck messages");
        CodeBlocksLogEvent evtAdd2(cbEVT_ADD_LOG_WINDOW, m_ListLog, LogMan->Slot(m_ListLogPageIndex).title);
        Manager::Get()->ProcessEvent(evtAdd2);
    }
} // end of OnAttach

void CppCheck::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be FALSE...
    if (Manager::Get()->GetLogManager())
    {
        if (m_CppCheckLog)
        {
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_CppCheckLog);
            Manager::Get()->ProcessEvent(evt);
        }
        if (m_ListLog)
        {
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_ListLog);
            Manager::Get()->ProcessEvent(evt);
        }
    }
    m_CppCheckLog = 0;
    m_ListLog = 0;
} // end of OnRelease

void CppCheck::WriteToLog(const wxString& Text)
{
    m_CppCheckLog->Clear();
    AppendToLog(Text);
} // end of WriteToLog

void CppCheck::AppendToLog(const wxString& Text)
{
    if (LogManager* LogMan = Manager::Get()->GetLogManager())
    {
        CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_CppCheckLog);
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
} // namespace

bool CppCheck::DoCppCheckVersion()
{
    wxString CommandLine = m_CppCheckApp + _T(" --version");
    WriteToLog(CommandLine);
    wxArrayString Output, Errors;
    long pid = wxExecute(CommandLine, Output, Errors);
    if (pid==-1)
    {
        bool failed = true;
        if (cbMessageBox(_("Failed to launch cppcheck.\nDo you want to select the cppcheck executable?"),
                         _("Question"), wxICON_QUESTION | wxYES_NO, Manager::Get()->GetAppWindow()) == wxID_YES)
        {
            wxString filename = wxFileSelector(_("Select the cppcheck executable"));
            if (!filename.empty()) // otherwise the user selected cancel
            {
                // try again using the user-provided executable
                CommandLine = filename + _T(" --version");
                pid = wxExecute(CommandLine, Output, Errors);
                if (pid==-1)
                {
                    failed = true;
                }
                else
                {
                    m_CppCheckApp = filename;
                    failed = false;
                }
            }
        }
        if (failed)
        {
            AppendToLog(_("Failed to launch cppcheck."));
            cbMessageBox(_("Failed to launch cppcheck."), _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
            return false;
        }
    }
    int Count = Output.GetCount();
    for (int idxCount = 0; idxCount < Count; ++idxCount)
    {
        AppendToLog(Output[idxCount]);
    } // end for : idx: idxCount
    Count = Errors.GetCount();
    for (int idxCount = 0; idxCount < Count; ++idxCount)
    {
        AppendToLog(Errors[idxCount]);
    } // end for : idx: idxCount
    // and clear the list
    m_ListLog->Clear();
    return true;
} // end of DoCppCheckVersion


int CppCheck::Execute()
{
    if (!CheckRequirements() || !DoCppCheckVersion())
    {
        return -1;
    }

    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    const long Files = Project->GetFilesCount();
    if(!Files)
    {
    	return 0;
    }
    const wxString Basepath = Project->GetBasePath();
    ::wxSetWorkingDirectory(Basepath);
    AppendToLog(_T("switching working directory to : ") + Basepath);
    wxFile Input;
    const wxString InputFileName = _T("CppCheckInput.txt");
    if(!Input.Create(InputFileName, true))
    {
    	return -1;
    }
    for (int File = 0; File < Files; ++File)
    {
        ProjectFile* pf = Project->GetFile(File);
        Input.Write(pf->relativeFilename + _T("\n"));
    }
    Input.Close();
    // project include dirs
    wxString IncludeList;
    const wxArrayString& IncludeDirs = Project->GetIncludeDirs();
    MacrosManager* MacrosMgr = Manager::Get()->GetMacrosManager();
    ProjectBuildTarget* target = Project->GetBuildTarget(Project->GetActiveBuildTarget());
    for (unsigned int Dir = 0; Dir < IncludeDirs.GetCount(); ++Dir)
    {
        wxString IncludeDir(IncludeDirs[Dir]);
        if(target)
        {
            MacrosMgr->ReplaceMacros(IncludeDir, target);
        }
        IncludeList += _T("-I\"") + IncludeDir + _T("\" ");
    }
    if(target)
    {
        //target include dirs
        const wxArrayString& targetIncludeDirs = target->GetIncludeDirs();
        for (unsigned int Dir = 0; Dir < targetIncludeDirs.GetCount(); ++Dir)
        {
            wxString IncludeDir(targetIncludeDirs[Dir]);
            MacrosMgr->ReplaceMacros(IncludeDir, target);
            IncludeList += _T("-I\"") + IncludeDir + _T("\" ");
        }
    }

    wxString CommandLine = m_CppCheckApp + _T(" --verbose --all --style --xml --file-list=") + InputFileName;
    if(!IncludeList.IsEmpty())
    {
		CommandLine += _T(" ") + IncludeList.Trim();
    }
    AppendToLog(CommandLine);
    wxArrayString Output, Errors;
    {
        wxWindowDisabler disableAll;
        wxBusyInfo running(_("Running cppcheck... please wait (this may take several minutes)..."),
                           Manager::Get()->GetAppWindow());
        const long pid = wxExecute(CommandLine, Output, Errors, wxEXEC_SYNC);
        if (pid==-1)
        {
            wxString msg = _("Failed to launch cppcheck.\nMake sure the application is in the path!");
            AppendToLog(msg);
            cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
            ::wxRemoveFile(InputFileName);
            return -1;
        }
    } // end lifetime of wxWindowDisabler, wxBusyInfo
    ::wxRemoveFile(InputFileName);
    size_t Count = Output.GetCount();
    for (size_t idxCount = 0; idxCount < Count; ++idxCount)
    {
        AppendToLog(Output[idxCount]);
    } // end for : idx: idxCount
    wxString Xml;
    Count = Errors.GetCount();
    for (size_t idxCount = 0; idxCount < Count; ++idxCount)
    {
        Xml += Errors[idxCount];
        AppendToLog(Errors[idxCount]);
    } // end for : idx: idxCount
    const bool UseXml = true;
    if (UseXml)
    {
        TiXmlDocument Doc;
        Doc.Parse(Xml.ToAscii());
        if (Doc.Error())
        {
            wxString msg = _("Failed to parse cppcheck XML file.\nProbably it's not produced correctly.");
            AppendToLog(msg);
            cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        }
        else
        {
            bool ErrorsPresent = false;
            TiXmlHandle Handle(&Doc);
            Handle = Handle.FirstChildElement("results");
            for (const TiXmlElement* Error = Handle.FirstChildElement("error").ToElement(); Error;
                    Error = Error->NextSiblingElement("error"))
            {
                wxString File;
                if (const char* FileValue = Error->Attribute("file"))
                {
                    File = wxString::FromAscii(FileValue);
                }
                wxString Line;
                if (const char* LineValue = Error->Attribute("line"))
                {
                    Line = wxString::FromAscii(LineValue);
                }
                wxString Id;
                if (const char* IdValue = Error->Attribute("id"))
                {
                    Id = wxString::FromAscii(IdValue);
                }
                wxString Severity;
                if (const char* SeverityValue = Error->Attribute("severity"))
                {
                    Severity = wxString::FromAscii(SeverityValue);
                }
                wxString Message;
                if (const char* MessageValue = Error->Attribute("msg"))
                {
                    Message = wxString::FromAscii(MessageValue);
                }
                const wxString FulllMessage = Id + _T(" : ") + Severity + _T(" : ") + Message;
                if (!File.IsEmpty() && !Line.IsEmpty() && !FulllMessage.IsEmpty())
                {
                    wxArrayString Arr;
                    Arr.Add(File);
                    Arr.Add(Line);
                    Arr.Add(FulllMessage);
                    m_ListLog->Append(Arr);
                    ErrorsPresent = true;
                }
            }
            if (ErrorsPresent)
            {
                if (Manager::Get()->GetLogManager())
                {
                    CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_ListLog);
                    Manager::Get()->ProcessEvent(evtSwitch);
                }
            }
        }
    }

    return 0;
} // end of Execute
