/***************************************************************
 * Name:      [PROJECT_NAME]Main.cpp
 * Purpose:   Code for Application Frame
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "[PROJECT_NAME]Main.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

[IF WXFRAME]//(*InternalHeaders([PROJECT_NAME]Frame)
//*)

//(*IdInit([PROJECT_NAME]Frame)
//*)

BEGIN_EVENT_TABLE([PROJECT_NAME]Frame,wxFrame)
	//(*EventTable([PROJECT_NAME]Frame)
	//*)
END_EVENT_TABLE()

[PROJECT_NAME]Frame::[PROJECT_NAME]Frame(wxWindow* parent,wxWindowID id)
{
	//(*Initialize([PROJECT_NAME]Frame)
	//*)
}

[PROJECT_NAME]Frame::~[PROJECT_NAME]Frame()
{
	//(*Destroy([PROJECT_NAME]Frame)
	//*)
}

void [PROJECT_NAME]Frame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void [PROJECT_NAME]Frame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}[ENDIF WXFRAME][IF WXDIALOG]//(*InternalHeaders([PROJECT_NAME]Dialog)
//*)

//(*IdInit([PROJECT_NAME]Dialog)
//*)

BEGIN_EVENT_TABLE([PROJECT_NAME]Dialog,wxDialog)
	//(*EventTable([PROJECT_NAME]Dialog)
	//*)
END_EVENT_TABLE()

[PROJECT_NAME]Dialog::[PROJECT_NAME]Dialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize([PROJECT_NAME]Dialog)
	//*)
}

[PROJECT_NAME]Dialog::~[PROJECT_NAME]Dialog()
{
	//(*Destroy([PROJECT_NAME]Dialog)
	//*)
}

void [PROJECT_NAME]Dialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void [PROJECT_NAME]Dialog::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}[ENDIF WXDIALOG]
