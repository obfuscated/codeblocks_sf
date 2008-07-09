/***************************************************************
 * Name:      Valgrind.cpp
 * Purpose:   Code::Blocks Valgrind plugin: main functions
 * Author:    killerbot
 * Created:   28/07/2007
 * Copyright: (c) killerbot
 * License:   GPL
  **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "manager.h"
#include "logmanager.h"
#include "projectmanager.h"
#endif
#include "tinyxml/tinyxml.h"
#include "loggers.h"
#include "Valgrind.h"
#include "ValgrindListLog.h"

// Register the plugin
namespace
{
    PluginRegistrant<Valgrind> reg(_T("Valgrind"));
};

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

void Valgrind::OnRelease(bool appShutDown)
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

int Valgrind::Execute()
{
    // if not attached, exit
    if (!IsAttached())
        return -1;
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
   // if no project open, exit
	if (!Project)
	{
		wxString msg = _("You need to open a project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return -1;
	}
	// check the project s active target -> it should be executable !!
	wxString strTarget = Project->GetActiveBuildTarget();
	if(strTarget.empty())
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return -1;
	}
	// let's get the target
	ProjectBuildTarget* Target = Project->GetBuildTarget(strTarget); // NOT const because of GetNativeFilename() :-(
	if(!Target)
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return -1;
	}
	// check the type of the target
	wxString ExeTarget;
	const TargetType TType = Target->GetTargetType();
	if(!(TType == ttExecutable || TType == ttConsoleOnly))
	{
		wxString msg = _("You need to have an ***executable*** target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return -1;
	}
	else
	{
		if(TType == ttExecutable || ttConsoleOnly)
		{
//			ExeTarget = Target->GetExecutableFilename(); /// hmmm : this doesn't return correct stuff !!!
			ExeTarget = Target->GetOutputFilename();
		}
	}
	if(Target->GetCompilerOptions().Index(_T("-g")) == wxNOT_FOUND)
	{
		wxString msg = _("Your target needs to have been compiled with the -g option\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return -1;
	}
	wxString CommandLineArgs = Target->GetExecutionParameters();
	// allright let's start it up with Valgrind
	wxString CommandLine = _("valgrind --version");
	WriteToLog(CommandLine);
	wxArrayString Output, Errors;
	wxExecute(CommandLine, Output, Errors);
	int Count = Output.GetCount();
	for(int idxCount = 0; idxCount < Count; ++idxCount)
	{
		AppendToLog(Output[idxCount]);
	} // end for : idx: idxCount
	Count = Errors.GetCount();
	for(int idxCount = 0; idxCount < Count; ++idxCount)
	{
		AppendToLog(Errors[idxCount]);
	} // end for : idx: idxCount
	Output.clear();
	Errors.clear();
	// and now the real stuff
	const bool UseXml = true;
	CommandLine = _("valgrind --leak-check=yes --xml=yes \"") + ExeTarget + _("\" ") + CommandLineArgs;
//	CommandLine = _("valgrind --leak-check=yes \"") + ExeTarget + _("\" ") + CommandLineArgs;
	AppendToLog(CommandLine);
	wxExecute(CommandLine, Output, Errors);
	Count = Output.GetCount();
	for(int idxCount = 0; idxCount < Count; ++idxCount)
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
	for(int idxCount = 0; idxCount < Count; ++idxCount)
	{
		Xml += Errors[idxCount];
		AppendToLog(Errors[idxCount]);
	} // end for : idx: idxCount
	if(UseXml)
	{
		TiXmlDocument Doc;
		Doc.Parse(Xml.ToAscii());
		if(!Doc.Error())
		{
			bool ErrorsPresent = false;
			TiXmlHandle Handle(&Doc);
			Handle = Handle.FirstChildElement("valgrindoutput");
			for(const TiXmlElement* Error = Handle.FirstChildElement("error").ToElement(); Error;
					Error = Error->NextSiblingElement("error"))
			{
				ErrorsPresent = true;
				wxString WhatValue;
				if(const TiXmlElement* What = Error->FirstChildElement("what"))
				{
					WhatValue = wxString::FromAscii(What->GetText());
				}
				// process the first stack
				if(const TiXmlElement* Stack = Error->FirstChildElement("stack"))
				{
					ProcessStack(*Stack, WhatValue);
				}
			} // end for
			if(ErrorsPresent)
			{
				if(Manager::Get()->GetLogManager())
				{
					CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_ListLog);
					Manager::Get()->ProcessEvent(evtSwitch);
				}
			}
		}
		// loop over the errors
	}
	return 0;
} // end of Execute

void Valgrind::ProcessStack(const TiXmlElement& Stack, const wxString& What)
{
	// start by doing the first frame (that contains dir/file/line)
	for(const TiXmlElement* Frame = Stack.FirstChildElement("frame"); Frame;
		Frame = Frame->NextSiblingElement("frame"))
	{
		const TiXmlElement* Dir = Frame->FirstChildElement("dir");
		const TiXmlElement* File = Frame->FirstChildElement("file");
		const TiXmlElement* Line = Frame->FirstChildElement("line");
		const TiXmlElement* Function = Frame->FirstChildElement("fn");
		if(Dir && File && Line)
		{
			const wxString FullName = wxString::FromAscii(Dir->GetText()) + _("/") + wxString::FromAscii(File->GetText());
			wxArrayString Arr;
			if(Function)
			{
				Arr.Add(FullName);
				Arr.Add(_(""));
				Arr.Add(_("In function '") + wxString::FromAscii(Function->GetText()) + _("' :"));
				m_ListLog->Append(Arr);
			}
			Arr.Clear();
			Arr.Add(FullName);
			Arr.Add(wxString::FromAscii(Line->GetText()));
			Arr.Add(What);
			m_ListLog->Append(Arr);
		}
	} // end while
} // end of ProcessStack
