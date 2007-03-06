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

[IF WXFRAME]//(*Headers([PROJECT_NAME]Frame)
//*)

class [PROJECT_NAME]Frame: public wxFrame
{
	public:

		[PROJECT_NAME]Frame(wxWindow* parent,wxWindowID id = -1);
		virtual ~[PROJECT_NAME]Frame();

	private:

		//(*Handlers([PROJECT_NAME]Frame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		//*)

		//(*Identifiers([PROJECT_NAME]Frame)
		//*)

		//(*Declarations([PROJECT_NAME]Frame)
		//*)

		DECLARE_EVENT_TABLE()
};[ENDIF WXFRAME][IF WXDIALOG]//(*Headers([PROJECT_NAME]Dialog)
//*)

class [PROJECT_NAME]Dialog: public wxDialog
{
	public:

		[PROJECT_NAME]Dialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~[PROJECT_NAME]Dialog();

	private:

		//(*Handlers([PROJECT_NAME]Dialog)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		//*)

		//(*Identifiers([PROJECT_NAME]Dialog)

		//*)

		//(*Declarations([PROJECT_NAME]Dialog)

		//*)

		DECLARE_EVENT_TABLE()
};[ENDIF WXDIALOG]

#endif // [PROJECT_HDR]MAIN_H
