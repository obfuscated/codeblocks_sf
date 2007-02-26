/***************************************************************
 * Name:      [PROJECT_NAME]Main.h
 * Purpose:   Defines Application Frame
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:   
 **************************************************************/

#ifndef [PROJECT_HDR]MAIN_H
#define [PROJECT_HDR]MAIN_H

#include "[PROJECT_NAME]App.h"

[IF WXFRAME][IF NONE]class [PROJECT_NAME]Frame: public wxFrame
{
	public:
		[PROJECT_NAME]Frame(wxFrame *frame, const wxString& title);
		~[PROJECT_NAME]Frame();
	private:
		enum
		{
			idMenuQuit = 1000,
			idMenuAbout
		};
		void OnClose(wxCloseEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};[ENDIF NONE]
[IF WXFB]#include "GUIFrame.h"

class [PROJECT_NAME]Frame: public GUIFrame
{
	public:
		[PROJECT_NAME]Frame(wxFrame *frame);
		~[PROJECT_NAME]Frame();
	private:
		virtual void OnClose(wxCloseEvent& event);
		virtual void OnQuit(wxCommandEvent& event);
		virtual void OnAbout(wxCommandEvent& event);
};[ENDIF WXFB][ENDIF WXFRAME]
[IF WXDIALOG][IF NONE]#include <wx/button.h>
#include <wx/statline.h>
class [PROJECT_NAME]Dialog: public wxDialog
{
	public:
		[PROJECT_NAME]Dialog(wxDialog *dlg, const wxString& title);
		~[PROJECT_NAME]Dialog();
		
	protected:
		enum
		{
			idBtnQuit = 1000,
			idBtnAbout
		};
		wxStaticText* m_staticText1;
		wxButton* BtnAbout;
		wxStaticLine* m_staticline1;
		wxButton* BtnQuit;
		
	private:
		void OnClose(wxCloseEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};[ENDIF NONE]
[IF WXFB]#include "GUIDialog.h"

class [PROJECT_NAME]Dialog: public GUIDialog
{
	public:
		[PROJECT_NAME]Dialog(wxDialog *dlg);
		~[PROJECT_NAME]Dialog();
	private:
		virtual void OnClose(wxCloseEvent& event);
		virtual void OnQuit(wxCommandEvent& event);
		virtual void OnAbout(wxCommandEvent& event);
};[ENDIF WXFB][ENDIF WXDIALOG]
#endif // [PROJECT_NAME]MAIN_H
