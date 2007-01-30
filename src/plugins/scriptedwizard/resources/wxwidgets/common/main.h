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

class [PROJECT_NAME]Frame: public wxFrame
{
	public:
		[PROJECT_NAME]Frame(wxFrame *frame, const wxString& title);
		~[PROJECT_NAME]Frame();
	private:
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // [PROJECT_NAME]MAIN_H
