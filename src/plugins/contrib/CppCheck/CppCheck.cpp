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
    #include <wx/filefn.h>
    #include <wx/fs_zip.h>
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>

    #include "cbproject.h"
    #include "cbplugin.h"
    #include "configmanager.h"
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
#include "filefilters.h"
#include "loggers.h"

#include "CppCheck.h"
#include "CppCheckListLog.h"
#include "ConfigPanel.h"

// Register the plugin
namespace
{
    PluginRegistrant<CppCheck> reg(_T("CppCheck"));
};

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
    }
}; // namespace


CppCheck::CppCheck() :
    m_LogPageIndex(0), // good init value ???
    m_CppCheckLog(0),
    m_ListLog(0),
    m_ListLogPageIndex(0),
    m_PATH(wxEmptyString)
{
    if (!Manager::LoadResource(_T("CppCheck.zip")))
        NotifyMissingFile(_T("CppCheck.zip"));
}

CppCheck::~CppCheck()
{
}

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

        wxArrayString Titles; wxArrayInt Widths;
        Titles.Add(_("File"));    Widths.Add(128);
        Titles.Add(_("Line"));    Widths.Add(48);
        Titles.Add(_("Message")); Widths.Add(640);
        m_ListLog = new CppCheckListLog(Titles, Widths);

        m_ListLogPageIndex = LogMan->SetLog(m_ListLog);
        LogMan->Slot(m_ListLogPageIndex).title = _("CppCheck messages");
        CodeBlocksLogEvent evtAdd2(cbEVT_ADD_LOG_WINDOW, m_ListLog, LogMan->Slot(m_ListLogPageIndex).title);
        Manager::Get()->ProcessEvent(evtAdd2);
    }
}

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
            m_ListLog->DestroyControls();
            Manager::Get()->ProcessEvent(evt);
        }
    }
    m_CppCheckLog = 0;
    m_ListLog = 0;
}

void CppCheck::WriteToLog(const wxString& Text)
{
    m_CppCheckLog->Clear();
    AppendToLog(Text);
}

void CppCheck::AppendToLog(const wxString& Text)
{
    if (LogManager* LogMan = Manager::Get()->GetLogManager())
    {
        CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_CppCheckLog);
        Manager::Get()->ProcessEvent(evtSwitch);
        LogMan->Log(Text, m_LogPageIndex);
    }
}

cbConfigurationPanel* CppCheck::GetConfigurationPanel(wxWindow* parent)
{
    // Called by plugin manager to show config panel in global Setting Dialog
    if ( !IsAttached() )
        return NULL;

    return new ConfigPanel(parent);
}

int CppCheck::Execute()
{
    WriteToLog(_("Running cppcppcheck analysis... please wait..."));

    if (!CheckRequirements() || !DoCppCheckVersion())
        return -1;

    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (Project->GetFilesCount() < 1)
        return 0;

    TCppCheckAttribs CppCheckAttribs;

    const wxString BasePath = Project->GetBasePath();
    AppendToLog(_T("Switching working directory to : ") + BasePath);
    ::wxSetWorkingDirectory(BasePath);

    wxFile InputFile;
    CppCheckAttribs.InputFileName = _T("CppCheckInput.txt");
    if ( !InputFile.Create(CppCheckAttribs.InputFileName, true) )
    {
        cbMessageBox(_("Failed to create input file 'CppCheckInput.txt' for cppcheck.\nPlease check file/folder access rights."),
                     _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        return -1;
    }

    for (FilesList::iterator it = Project->GetFilesList().begin(); it != Project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        // filter to avoid including non C/C++ files
        if (   pf->relativeFilename.EndsWith(FileFilters::C_DOT_EXT)
            || pf->relativeFilename.EndsWith(FileFilters::CPP_DOT_EXT)
            || pf->relativeFilename.EndsWith(FileFilters::CC_DOT_EXT)
            || pf->relativeFilename.EndsWith(FileFilters::CXX_DOT_EXT)
            || pf->relativeFilename.EndsWith(FileFilters::CPLPL_DOT_EXT)
            || (FileTypeOf(pf->relativeFilename) == ftHeader) )
        {
            InputFile.Write(pf->relativeFilename + _T("\n"));
        }
    }
    InputFile.Close();

    MacrosManager*      MacrosMgr = Manager::Get()->GetMacrosManager();
    ProjectBuildTarget* Target    = Project->GetBuildTarget(Project->GetActiveBuildTarget());

    // project include dirs
    const wxArrayString& IncludeDirs = Project->GetIncludeDirs();
    for (unsigned int Dir = 0; Dir < IncludeDirs.GetCount(); ++Dir)
    {
        wxString IncludeDir(IncludeDirs[Dir]);
        if (Target)
            MacrosMgr->ReplaceMacros(IncludeDir, Target);
        else
            MacrosMgr->ReplaceMacros(IncludeDir);
        CppCheckAttribs.IncludeList += _T("-I\"") + IncludeDir + _T("\" ");
    }
    if (Target)
    {
        // target include dirs
        const wxArrayString& targetIncludeDirs = Target->GetIncludeDirs();
        for (unsigned int Dir = 0; Dir < targetIncludeDirs.GetCount(); ++Dir)
        {
            wxString IncludeDir(targetIncludeDirs[Dir]);
            MacrosMgr->ReplaceMacros(IncludeDir, Target);
            CppCheckAttribs.IncludeList += _T("-I\"") + IncludeDir + _T("\" ");
        }
    }

    // project #defines
    const wxArrayString& Defines = Project->GetCompilerOptions();
    for (unsigned int Opt = 0; Opt < Defines.GetCount(); ++Opt)
    {
        wxString Define(Defines[Opt]);
        if (Target)
            MacrosMgr->ReplaceMacros(Define, Target);
        else
            MacrosMgr->ReplaceMacros(Define);

        if ( Define.StartsWith(_T("-D")) )
            CppCheckAttribs.DefineList += Define + _T(" ");
    }
    if (Target)
    {
        // target #defines
        const wxArrayString& targetDefines = Target->GetCompilerOptions();
        for (unsigned int Opt = 0; Opt < targetDefines.GetCount(); ++Opt)
        {
            wxString Define(targetDefines[Opt]);
            MacrosMgr->ReplaceMacros(Define, Target);

            if ( Define.StartsWith(_T("-D")) )
                CppCheckAttribs.DefineList += Define + _T(" ");
        }
    }

    return DoCppCheckExecute(CppCheckAttribs);
}

bool CppCheck::DoCppCheckVersion()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cppcheck"));
    wxString cpp_exe = GetExecutable(cfg);

    wxArrayString Output, Errors;
    wxString CommandLine = cpp_exe + _T(" --version");
    if ( !CppCheckExecute(CommandLine, Output, Errors) )
        return false;

    return true;
}

int CppCheck::DoCppCheckExecute(TCppCheckAttribs& CppCheckAttribs)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cppcheck"));
    wxString cpp_exe = GetExecutable(cfg);
    wxString CommandLine = cpp_exe + _T(" ")
                         + cfg->Read(_T("cppcheck_args"), _T("--verbose --enable=all --enable=style --xml"))
                         + _T(" --file-list=") + CppCheckAttribs.InputFileName;
    if ( !CppCheckAttribs.IncludeList.IsEmpty() )
        CommandLine += _T(" ") + CppCheckAttribs.IncludeList.Trim() + _T(" ")
                     + CppCheckAttribs.DefineList.Trim();

    wxArrayString Output, Errors;
    bool isOK = CppCheckExecute(CommandLine, Output, Errors);
    ::wxRemoveFile(CppCheckAttribs.InputFileName);
    if (!isOK)
        return -1;

    wxString Xml;
    for (size_t idxCount = 0; idxCount < Errors.GetCount(); ++idxCount)
        Xml += Errors[idxCount];
    DoCppCheckAnalysis(Xml);

    return 0;
}

void CppCheck::DoCppCheckAnalysis(const wxString& Xml)
{
    // clear the list
    m_ListLog->Clear();

    TiXmlDocument Doc;
    Doc.Parse( Xml.ToAscii() );
    if ( Doc.Error() )
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
                File = wxString::FromAscii(FileValue);
            wxString Line;
            if (const char* LineValue = Error->Attribute("line"))
                Line = wxString::FromAscii(LineValue);
            wxString Id;
            if (const char* IdValue = Error->Attribute("id"))
                Id = wxString::FromAscii(IdValue);
            wxString Severity;
            if (const char* SeverityValue = Error->Attribute("severity"))
                Severity = wxString::FromAscii(SeverityValue);
            wxString Message;
            if (const char* MessageValue = Error->Attribute("msg"))
                Message = wxString::FromAscii(MessageValue);
            const wxString FullMessage = Id + _T(" : ") + Severity + _T(" : ") + Message;
            if (!File.IsEmpty() && !Line.IsEmpty() && !FullMessage.IsEmpty())
            {
                wxArrayString Arr;
                Arr.Add(File);
                Arr.Add(Line);
                Arr.Add(FullMessage);
                m_ListLog->Append(Arr);
                ErrorsPresent = true;
            }
            else if (!Message.IsEmpty())
                AppendToLog(Message); // might be something important like config not found...
        }
        if (ErrorsPresent)
        {
            if ( Manager::Get()->GetLogManager() )
            {
                CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_ListLog);
                Manager::Get()->ProcessEvent(evtSwitch);
            }
        }

        if ( !Doc.SaveFile("CppCheckResults.xml") )
        {
            cbMessageBox(_("Failed to create output file 'CppCheckResults.xml' for cppcheck.\nPlease check file/folder access rights."),
                         _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        }
    }
}

wxString CppCheck::GetExecutable(ConfigManager* cfg)
{
    wxString Executable = ConfigPanel::GetDefaultExecutableName();
    if (cfg)
        Executable = cfg->Read(_T("cppcheck_app"), Executable);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(Executable);

    AppendToLog(wxString::Format(_("Executable cppcheck: '%s'."),
                                 Executable.wx_str()));

    // Make sure file is accessible, otherwise add path to cppcheck to PATH envvar
    wxFileName fn(Executable);
    if (fn.IsOk() && fn.FileExists())
    {
        wxString CppCheckPath = fn.GetPath();
        AppendToLog(wxString::Format(_("Path to cppcheck: '%s'."),
                                     CppCheckPath.wx_str()));

        if ( CppCheckPath.Trim().IsEmpty() )
            return Executable; // Nothing to do, lets hope it works and cppcheck is in the PATH

        bool PrependCppCheckPath = true;
        wxString NewPathEnvVar = wxEmptyString;

        wxPathList PathList;
        PathList.AddEnvList(wxT("PATH"));
        for (size_t i=0; i<PathList.GetCount(); ++i)
        {
            wxString PathItem = PathList.Item(i);
            if ( PathItem.IsSameAs(CppCheckPath, (platform::windows ? false : true)) )
            {
                AppendToLog(_("Executable of cppcheck is in the path."));
                PrependCppCheckPath = false;
                break; // Exit for-loop
            }

            if ( !NewPathEnvVar.IsEmpty() )
                NewPathEnvVar << wxPATH_SEP;
            NewPathEnvVar << PathItem;
        }

        if (m_PATH.IsEmpty())
            m_PATH = NewPathEnvVar;


        if (PrependCppCheckPath)
        {
            NewPathEnvVar = NewPathEnvVar.Prepend(wxPATH_SEP);
            NewPathEnvVar = NewPathEnvVar.Prepend(CppCheckPath);
            wxSetEnv(wxT("PATH"), NewPathEnvVar); // Don't care about return value
            AppendToLog(wxString::Format(_("Updated PATH environment to include path to cppcheck: '%s' ('%s')."),
                                         CppCheckPath.wx_str(), NewPathEnvVar.wx_str()));
        }
    }

    return Executable;
}

bool CppCheck::CppCheckExecute(const wxString& CommandLine, wxArrayString& Output, wxArrayString& Errors)
{
    wxWindowDisabler disableAll;
    wxBusyInfo running(_("Running cppcheck... please wait (this may take several minutes)..."),
                       Manager::Get()->GetAppWindow());

    AppendToLog(CommandLine);
    if ( -1 == wxExecute(CommandLine, Output, Errors, wxEXEC_SYNC) )
    {
        wxString msg = _("Failed to launch cppcheck.\n"
                         "Please setup the cppcheck executable accordingly in the settings\n"
                        "and make sure its also in the path so cppcheck resources are found.");
        AppendToLog(msg);
        cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        if (!m_PATH.IsEmpty()) wxSetEnv(wxT("PATH"), m_PATH); // Restore
        return false;
    }

    int Count = Output.GetCount();
    for (int idxCount = 0; idxCount < Count; ++idxCount)
        AppendToLog(Output[idxCount]);

    Count = Errors.GetCount();
    for (int idxCount = 0; idxCount < Count; ++idxCount)
        AppendToLog(Errors[idxCount]);

    if (!m_PATH.IsEmpty()) wxSetEnv(wxT("PATH"), m_PATH); // Restore
    return true;
}
