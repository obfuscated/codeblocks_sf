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
// RCS-ID: $Id: codesnippetsapp.h 83 2007-05-19 20:30:36Z Pecan $

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
		//-void OnActivateApp(wxActivateEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif // CODESNIPPETSAPP_H
/***************************************************************
 * Name:      CodeSnippetsAppMain.h
 * Purpose:   Defines Application Frame
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/

#ifndef CODESNIPPETSAPPMAIN_H
#define CODESNIPPETSAPPMAIN_H

#include <wx/snglinst.h>    //single instance checker

#include "codesnippetsapp.h"
#include "snippetsconfig.h"
#include "codesnippetswindow.h"

class wxMemoryMappedFile;

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

	private:

		CodeSnippetsWindow* GetSnippetsWindow(){return GetConfig()->GetSnippetsWindow();}

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

        void StartKeepAliveTimer(int secs){ m_Timer.Start( secs*1000, wxTIMER_ONE_SHOT); }
        void StopKeepAliveTimer(){ m_Timer.Stop();}
        bool ReleaseMemoryMappedFile();

        void InitializeRecentFilesHistory();
        void TerminateRecentFilesHistory();
        void AddToRecentFilesHistory(const wxString& FileName);

		wxString            buildInfo;
        wxString            versionStr;

        wxSingleInstanceChecker*  m_checker ;
        int                 m_bOnActivateBusy;
        long                m_lKeepAlivePid;
        wxMemoryMappedFile* m_pMappedFile;
        wxFileHistory*      m_pFilesHistory;

        wxTimer             m_Timer;

		DECLARE_EVENT_TABLE()
};


#endif // CODESNIPPETSAPPMAIN_H
