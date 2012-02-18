/***************************************************************
 * Name:      Valgrind.cpp
 * Purpose:   Code::Blocks Valgrind plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   28/07/2007
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
#include "macrosmanager.h"
#endif
#include <wx/filefn.h>
#include "tinyxml/tinyxml.h"
#include "loggers.h"
#include "Valgrind.h"
#include "ValgrindListLog.h"

// Register the plugin
namespace
{
    PluginRegistrant<Valgrind> reg(_T("Valgrind"));
	int IdMemCheck = wxNewId();
	int IdCacheGrind = wxNewId();
};

BEGIN_EVENT_TABLE(Valgrind, cbPlugin)
	EVT_MENU(IdMemCheck, Valgrind::OnMemCheck)
	EVT_MENU(IdCacheGrind, Valgrind::OnCachegrind)
END_EVENT_TABLE()

Valgrind::Valgrind()
{
    if(!Manager::LoadResource(_T("Valgrind.zip")))
    {
        NotifyMissingFile(_T("Valgrind.zip"));
    }
    m_LogPageIndex = 0; // good init value ???
    m_ValgrindLog = 0;
    m_ListLog = 0;
    m_ListLogPageIndex = 0;
} // end of constructor

Valgrind::~Valgrind()
{
} // end of destruccor

void Valgrind::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
	if(LogManager* LogMan = Manager::Get()->GetLogManager())
	{
		m_ValgrindLog = new TextCtrlLogger();
		m_LogPageIndex = LogMan->SetLog(m_ValgrindLog);
		LogMan->Slot(m_LogPageIndex).title = _("Valgrind");
		CodeBlocksLogEvent evtAdd1(cbEVT_ADD_LOG_WINDOW, m_ValgrindLog, LogMan->Slot(m_LogPageIndex).title);
		//Manager::Get()->GetAppWindow()->ProcessEvent(evtAdd1);
		Manager::Get()->ProcessEvent(evtAdd1);

		wxArrayString Titles;
		wxArrayInt Widths;
		Titles.Add(_("File"));
		Titles.Add(_("Line"));
		Titles.Add(_("Message"));
		Widths.Add(128);
		Widths.Add(48);
		Widths.Add(640);
		m_ListLog = new ValgrindListLog(Titles, Widths);
		m_ListLogPageIndex = LogMan->SetLog(m_ListLog);
		LogMan->Slot(m_ListLogPageIndex).title = _("Valgrind messages");
		CodeBlocksLogEvent evtAdd2(cbEVT_ADD_LOG_WINDOW, m_ListLog, LogMan->Slot(m_ListLogPageIndex).title);
		Manager::Get()->ProcessEvent(evtAdd2);
	}
} // end of OnAttach

void Valgrind::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // IsAttached() will be FALSE...
	if(Manager::Get()->GetLogManager())
	{
		if(m_ValgrindLog)
		{
			CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_ValgrindLog);
			Manager::Get()->ProcessEvent(evt);
		}
		if(m_ListLog)
		{
			CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_ListLog);
			Manager::Get()->ProcessEvent(evt);
		}
	}
	m_ValgrindLog = 0;
	m_ListLog = 0;
} // end of OnRelease

void Valgrind::BuildMenu(wxMenuBar* MenuBar)
{
	//The application is offering its menubar for your plugin,
	//to add any menu items you want...
	//Append any items you need in the menu...
	//NOTE: Be careful in here... The application's menubar is at your disposal.
	if(!m_IsAttached || !MenuBar)
	{
		return;
	}
	// we will add our new menu just before the last menu(normally help menu)
	int MenusCount = MenuBar->GetMenuCount();
	wxMenu* Menu = new wxMenu;
	if(MenuBar->Insert(MenusCount - 1, Menu, _("Valgrind")))
	{ // let's add all the menu entries
		Menu->Append(IdMemCheck, _("Run Valgrind::MemCheck"), _("Run Valgrind::MemCheck"));
		Menu->Append(IdCacheGrind, _("Run Valrind::Cachegrind"), _("Run Valrind::Cachegrind"));
	}
} // end of BuildMenu

void Valgrind::WriteToLog(const wxString& Text)
{
	m_ValgrindLog->Clear();
	// maybe also show event needed ??
#if 0
CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
Manager::Get()->ProcessEvent(evtShow);
#endif
	AppendToLog(Text);
} // end of WriteToLog

void Valgrind::AppendToLog(const wxString& Text)
{
    if(LogManager* LogMan = Manager::Get()->GetLogManager())
    {
		CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_ValgrindLog);
		Manager::Get()->ProcessEvent(evtSwitch);
	// maybe also show event needed ??
#if 0
CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
Manager::Get()->ProcessEvent(evtShow);
#endif
    	LogMan->Log(Text, m_LogPageIndex);
    }
} // end of AppendToLog

void Valgrind::ProcessStack(const TiXmlElement& Stack, bool AddHeader)
{
    wxArrayString Arr;
    if (AddHeader)
    {
        Arr.Add(wxEmptyString);
        Arr.Add(wxEmptyString);
        Arr.Add(_("Call stack:"));
        m_ListLog->Append(Arr);
    }

    // start by doing the first frame (that contains dir/file/line)
    for(const TiXmlElement* Frame = Stack.FirstChildElement("frame"); Frame;
        Frame = Frame->NextSiblingElement("frame"))
    {
        const TiXmlElement* Dir = Frame->FirstChildElement("dir");
        const TiXmlElement* File = Frame->FirstChildElement("file");
        const TiXmlElement* Line = Frame->FirstChildElement("line");
        const TiXmlElement* Function = Frame->FirstChildElement("fn");
        const TiXmlElement* IP = Frame->FirstChildElement("ip");

        if (!Function)
            continue;

        wxString FullName;
        if (Dir && File)
            FullName = wxString::FromAscii(Dir->GetText()) + _("/") + wxString::FromAscii(File->GetText());
        else if (const TiXmlElement* Obj = Frame->FirstChildElement("obj"))
            FullName = wxString::FromAscii(Obj->GetText());

        Arr.Clear();
        Arr.Add(FullName);
        if (Line)
            Arr.Add(wxString::FromAscii(Line->GetText()));
        else
            Arr.Add(wxEmptyString);
        wxString StrFunction;
        if (IP)
            StrFunction = wxString::FromAscii(IP->GetText()) + wxT(": ");
        StrFunction += wxString::FromAscii(Function->GetText());
        Arr.Add(StrFunction);
        m_ListLog->Append(Arr);
    } // end while
} // end of ProcessStack

bool CheckRequirements(wxString& ExeTarget, wxString &WorkDir, wxString& CommandLineArguments)
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
	// check the project s active target -> it should be executable !!
	wxString strTarget = Project->GetActiveBuildTarget();
	if(strTarget.empty())
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return false;
	}
	// let's get the target
    ProjectBuildTarget* Target =  nullptr; //Project->GetBuildTarget(strTarget); // NOT const because of GetNativeFilename() :-(
    if (!Project->BuildTargetValid(strTarget, false))
    {
        const int tgtIdx = Project->SelectTarget();
        if (tgtIdx == -1)
        {
            return false;
        }
        Target = Project->GetBuildTarget(tgtIdx);
        strTarget = Target->GetTitle();
    }
    else
    {
        Target = Project->GetBuildTarget(strTarget);
    }
	if(!Target)
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return false;
	}
	// check the type of the target
	const TargetType TType = Target->GetTargetType();
	if(!(TType == ttExecutable || TType == ttConsoleOnly))
	{
		wxString msg = _("You need to have an ***executable*** target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return false;
	}
	else
	{
        ExeTarget = Project->GetBasePath() + Target->GetOutputFilename();
        MacrosManager* MacrosMgr = Manager::Get()->GetMacrosManager();
        MacrosMgr->ReplaceMacros(ExeTarget, Target);
        WorkDir = Target->GetWorkingDir();
	}
// Disable this check, because it is not a real requirement.
// And also it breaks if the -g option is set for the project, not for the target!
//	if(Target->GetCompilerOptions().Index(_T("-g")) == wxNOT_FOUND)
//	{
//		wxString msg = _("Your target needs to have been compiled with the -g option\nbefore using the plugin!");
//		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
//		Manager::Get()->GetLogManager()->DebugLog(msg);
//		return false;
//	}
	CommandLineArguments = Target->GetExecutionParameters();
	return true;
}  // end of CheckRequirements

long Valgrind::DoValgrindVersion()
{
	wxString CommandLine = _("valgrind --version");
	WriteToLog(CommandLine);
	wxArrayString Output, Errors;
	wxExecute(CommandLine, Output, Errors);
	int Count = Output.GetCount();
	wxString ValgrindVersion;
	for(int idxCount = 0; idxCount < Count; ++idxCount)
	{
		ValgrindVersion = Output[0];
		AppendToLog(Output[idxCount]);
	} // end for : idx: idxCount
	Count = Errors.GetCount();
	for(int idxCount = 0; idxCount < Count; ++idxCount)
	{
		AppendToLog(Errors[idxCount]);
	} // end for : idx: idxCount
	// and clear the list
	m_ListLog->Clear();
	long VersionValue = 0;
	wxString Version;
	if(ValgrindVersion.StartsWith(_T("valgrind-"), &Version))
	{
		Version.Replace(_T("."), _T(""));
		Version.ToLong(&VersionValue);
	}
	return VersionValue;
} // end of DoValgrindVersion

void Valgrind::OnMemCheck(wxCommandEvent& )
{
	wxString ExeTarget;
	wxString CommandLineArguments;
	wxString WorkDir;
	if(!CheckRequirements(ExeTarget, WorkDir, CommandLineArguments))
	{
		return;
	}
	long Version = DoValgrindVersion();

	cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();

	const wxString XmlOutputFile = Project->GetBasePath() + _T("ValgrindOut.xml");
	wxString XmlOutputCommand;
	if(Version >= 350)
	{
		XmlOutputCommand = _T(" --xml-file=") + XmlOutputFile;
	}
	const bool UseXml = true;
	wxString CommandLine = _T("valgrind --leak-check=yes --track-origins=yes --xml=yes") + XmlOutputCommand + _T(" \"")
		+ ExeTarget + _T("\" ") + CommandLineArguments;
//	CommandLine = _("valgrind --leak-check=yes \"") + ExeTarget + _("\" ") + CommandLineArguments;
	AppendToLog(CommandLine);
	wxArrayString Output, Errors;
	wxString OldWorkDir = wxGetCwd();
	wxSetWorkingDirectory(WorkDir);
	wxExecute(CommandLine, Output, Errors);
	wxSetWorkingDirectory(OldWorkDir);
	size_t Count = Output.GetCount();
	for(size_t idxCount = 0; idxCount < Count; ++idxCount)
	{
		// EXTRA NOTE : it seems the output from valgrind comes on the error channel, not here !!!
		// it seems that all valgrind stuff starts with == (in case of not xml)
		// filter on that, so we can remove regular output from the tested exe
//		if(Output[idxCount].StartsWith(_("==")))
		{
			AppendToLog(Output[idxCount]);
		}
	} // end for : idx: idxCount
	wxString Xml;
	Count = Errors.GetCount();
	for(size_t idxCount = 0; idxCount < Count; ++idxCount)
	{
		Xml += Errors[idxCount];
		AppendToLog(Errors[idxCount]);
	} // end for : idx: idxCount
	if(UseXml)
	{
		TiXmlDocument Doc;
		if(Version >= 350)
		{
			Doc.LoadFile(XmlOutputFile.ToAscii());
		}
		else
		{
			Doc.Parse(Xml.ToAscii());
		}
		if(!Doc.Error())
		{
			bool ErrorsPresent = false;
			TiXmlHandle Handle(&Doc);
			Handle = Handle.FirstChildElement("valgrindoutput");
			for(const TiXmlElement* Error = Handle.FirstChildElement("error").ToElement(); Error;
					Error = Error->NextSiblingElement("error"))
			{
				ErrorsPresent = true;
				wxString WhatValue, KindValue;
				if(const TiXmlElement* What = Error->FirstChildElement("xwhat"))
				{	// style use since Valgrind 3.5.0
					if(const TiXmlElement* Text = What->FirstChildElement("text"))
					{
						WhatValue = wxString::FromAscii(Text->GetText());
					}
				}
				else if(const TiXmlElement* What = Error->FirstChildElement("what"))
				{
					WhatValue = wxString::FromAscii(What->GetText());
				}
                if (const TiXmlElement *Kind = Error->FirstChildElement("kind"))
                    KindValue = wxString::FromAscii(Kind->GetText());

                wxArrayString Arr;
                Arr.Add(KindValue);
                Arr.Add(wxT("===="));
                Arr.Add(WhatValue);
                m_ListLog->Append(Arr, Logger::error);

				// process the first stack
                const TiXmlElement* Stack = Error->FirstChildElement("stack");
				if(Stack)
				{
					ProcessStack(*Stack, true);
                    if (const TiXmlElement *AuxWhat = Error->FirstChildElement("auxwhat"))
                    {
                        Arr.Clear();
                        Arr.Add(wxEmptyString);
                        Arr.Add(wxEmptyString);
                        Arr.Add(wxString::FromAscii(AuxWhat->GetText()));
                        m_ListLog->Append(Arr, Logger::warning);
                    }
                    Stack = Stack->NextSiblingElement("stack");
                    if (Stack)
                        ProcessStack(*Stack, false);
				}

			} // end for
			if(ErrorsPresent)
			{
				if(Manager::Get()->GetLogManager())
				{
					CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_ListLog);
					Manager::Get()->ProcessEvent(evtSwitch);
				}
				m_ListLog->Fit();
			}
		}
		// loop over the errors
	}
} // end of OnMemCheck

void Valgrind::OnCachegrind(wxCommandEvent& )
{
	wxString ExeTarget;
	wxString CommandLineArguments;
	wxString WorkDir;
	if(!CheckRequirements(ExeTarget, WorkDir, CommandLineArguments))
	{
		return;
	}
	DoValgrindVersion();
//	wxString CommandLine = _("valgrind --tool=cachegrind --cachegrind-out-file=\"./") + ExeTarget + _(".cachegrind.out\" \"") + ExeTarget + _("\" ") + CommandLineArguments;
	wxString CommandLine = _T("valgrind --tool=cachegrind \"") + ExeTarget + _T("\" ") + CommandLineArguments;
	AppendToLog(CommandLine);
	wxArrayString Output, Errors;
	wxString CurrentDirName = ::wxGetCwd();
	wxDir CurrentDir(CurrentDirName);
	wxArrayString CachegrindFiles;
	if(CurrentDir.IsOpened())
	{
		wxString File;
		if(CurrentDir.GetFirst(&File, _T("cachegrind.out.*"), wxDIR_FILES))
		{
			CachegrindFiles.Add(File);
			while(CurrentDir.GetNext(&File))
			{
				CachegrindFiles.Add(File);
			} // end while
		}
	}
	wxExecute(CommandLine, Output, Errors);
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
	// try to find out how the file is named --> cachegrind.out.21807 [cachegrind.out.pid]
	// or To use an output file name other than the default cachegrind.out, use the --cachegrind-out-file  switch.
	// http://docs.wxwidgets.org/stable/wx_wxdir.html#wxdirgetfirst
	// idea : store all cachegrind.out.* filenames, and at finish do the same thing, and when one
	// is not in that list --> the new one
	wxString TheCachegrindFile;
	if(CurrentDir.IsOpened())
	{
		wxString File;
		if(CurrentDir.GetFirst(&File, _T("cachegrind.out.*"), wxDIR_FILES))
		{
			if(CachegrindFiles.Index(File) == wxNOT_FOUND)
			{
				TheCachegrindFile = File;
			}
			while(CurrentDir.GetNext(&File) && TheCachegrindFile.IsEmpty())
			{
				if(CachegrindFiles.Index(File) == wxNOT_FOUND)
				{
					TheCachegrindFile = File;
					AppendToLog(File);
				}
			} // end while
		}
	}
	CommandLine = _T("kcachegrind ") + TheCachegrindFile;
	wxExecute(CommandLine);
} // end of OnCachegrind
