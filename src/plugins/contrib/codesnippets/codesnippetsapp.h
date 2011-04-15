/***************************************************************
 * Name:      CodeSnippetsApp.h
 * Purpose:   Defines Application Class
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/
/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id$

#ifndef CODESNIPPETSAPP_H
#define CODESNIPPETSAPP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/docview.h> //wxFileHistory

// ----------------------------------------------------------------------------
class CodeSnippetsApp : public wxApp
// ----------------------------------------------------------------------------
{
	public:
		virtual bool OnInit();
		virtual int OnExit();
		//-void OnActivateApp(wxActivateEvent& event);
    private:

		DECLARE_EVENT_TABLE()
};

#endif // CODESNIPPETSAPP_H
/***************************************************************
 * Name:      CodeSnippetsAppFrame.h
 * Purpose:   Defines Application Frame
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/

#ifndef CODESNIPPETSAPPMAIN_H
#define CODESNIPPETSAPPMAIN_H

#include <wx/snglinst.h>    //single instance checker
#include <wx/string.h>    //single instance checker

#include "codesnippetsapp.h"
#include "snippetsconfig.h"
#include "codesnippetswindow.h"

class CodeSnippetsEvent;
class cbDragScroll;

// ----------------------------------------------------------------------------
class CodeSnippetsAppFrame: public wxFrame
// ----------------------------------------------------------------------------
{
	public:
		CodeSnippetsAppFrame(wxFrame *frame, const wxString& title);
		~CodeSnippetsAppFrame();

		bool GetFileChanged(){ return GetConfig()->pSnippetsWindow->GetFileChanged(); }
        wxString FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName);
        void OnRecentFileReopen(wxCommandEvent& event);
        void OnRecentFileClearHistory(wxCommandEvent& event);
        void InitializeDragScroll();
        bool GetInitXRCResult() {return m_bInitXRC_Result;}

	private:

        void InitCodeSnippetsAppFrame(wxFrame *frame, const wxString& title);
		CodeSnippetsWindow* GetSnippetsWindow(){return GetConfig()->GetSnippetsWindow();}
		CodeSnippetsTreeCtrl* GetSnippetsTreeCtrl(){return GetConfig()->GetSnippetsWindow()->GetSnippetsTreeCtrl();}

        void OnFileLoad(wxCommandEvent& event);
        void OnFileSave(wxCommandEvent& event);
        void OnFileSaveAs(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnQuit(wxCommandEvent& event);
        void OnSettings(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnActivate(wxActivateEvent& event);
		void OnFileBackup(wxCommandEvent& event);
		void OnTimerAlarm(wxTimerEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnEventTest(wxCommandEvent &event);

        void StartKeepAliveTimer(int secs){ m_Timer.Start( secs*1000, wxTIMER_ONE_SHOT); }
        void StopKeepAliveTimer(){ m_Timer.Stop();}
        bool RemoveKeepAliveFile();

        void InitializeRecentFilesHistory();
        void TerminateRecentFilesHistory();
        void AddToRecentFilesHistory(const wxString& FileName);
        void AskToRemoveFileFromHistory(wxFileHistory* hist, int id);

        bool InitializeSDK();
        bool LoadConfig();
        wxString GetAppPath();
        bool InitXRCStuff();
        void ComplainBadInstall();
        int ParseCmdLine(wxFrame* handlerFrame);
        void ImportCBResources();
        wxString GetCBExeFolder();
        bool FileImport(const wxString& source, const wxString& dest);
        wxString Normalize(const wxString& filenamein);

		wxString            buildInfo;
        wxString            versionStr;

        wxSingleInstanceChecker*  m_checker ;
        int                 m_bOnActivateBusy;
        wxString            m_KeepAliveFileName;
        wxFileHistory*      m_pFilesHistory;
        bool                m_bInitXRC_Result;
        EditorManager*      m_pEdMan;
        LogManager*         m_pMsgMan;

        wxTimer             m_Timer;
        wxString            m_Prefix;           // --prefix="data resources base folder"
        wxString            m_ConfigFolder;   // dir name of .conf file

		DECLARE_EVENT_TABLE()
};


#endif // CODESNIPPETSAPPMAIN_H
