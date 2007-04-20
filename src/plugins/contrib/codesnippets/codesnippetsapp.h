/***************************************************************
 * Name:      CodeSnippetsApp.h
 * Purpose:   Defines Application Class
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/

#ifndef CODESNIPPETSAPP_H
#define CODESNIPPETSAPP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

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

// ----------------------------------------------------------------------------
class CodeSnippetsAppFrame: public wxFrame
// ----------------------------------------------------------------------------
{
	public:
		CodeSnippetsAppFrame(wxFrame *frame, const wxString& title);
		~CodeSnippetsAppFrame();

		bool GetFileChanged(){ return GetConfig()->pSnippetsWindow->GetFileChanged(); }

	private:

		CodeSnippetsWindow* GetSnippetsWindow(){return GetConfig()->pSnippetsWindow;}

        void OnFileLoad(wxCommandEvent& event);
        void OnFileSave(wxCommandEvent& event);
        void OnFileSaveAs(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnQuit(wxCommandEvent& event);
        void OnSettings(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnActivate(wxActivateEvent& event);

		wxString            buildInfo;
        wxSingleInstanceChecker*  m_checker ;


		DECLARE_EVENT_TABLE()
};


#endif // CODESNIPPETSAPPMAIN_H
