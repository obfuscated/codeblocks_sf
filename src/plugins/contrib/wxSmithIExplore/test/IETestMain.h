/***************************************************************
 * Name:      IETestMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef IETESTMAIN_H
#define IETESTMAIN_H

//(*Headers(IETestFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <IEHtmlWin.h>
#include <wx/bmpbuttn.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class IETestFrame: public wxFrame
{
    public:

        IETestFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~IETestFrame();

    private:

        //(*Handlers(IETestFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnBitmapButton1Click(wxCommandEvent& event);
        void OnBitmapButton2Click(wxCommandEvent& event);
        void OnBitmapButton3Click(wxCommandEvent& event);
        void OnBitmapButton4Click(wxCommandEvent& event);
        void OnBitmapButton5Click(wxCommandEvent& event);
        void OnIE1BeforeNavigate2(wxMSHTMLEvent& event);
        void OnIE1NewWindow2(wxMSHTMLEvent& event);
        void OnIE1DocumentComplete(wxMSHTMLEvent& event);
        void OnIE1ProgressChange(wxMSHTMLEvent& event);
        void OnIE1StatusTextChange(wxMSHTMLEvent& event);
        void OnIE1TitleChange(wxMSHTMLEvent& event);
        //*)

        //(*Identifiers(IETestFrame)
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON2;
        static const long ID_BITMAPBUTTON3;
        static const long ID_BITMAPBUTTON4;
        static const long ID_TEXTCTRL1;
        static const long ID_BITMAPBUTTON5;
        static const long ID_IE1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(IETestFrame)
        wxBitmapButton* BitmapButton5;
        wxIEHtmlWin* IE1;
        wxBitmapButton* BitmapButton2;
        wxBitmapButton* BitmapButton1;
        wxStatusBar* StatusBar1;
        wxBitmapButton* BitmapButton4;
        wxTextCtrl* TextCtrl1;
        wxBitmapButton* BitmapButton3;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // IETESTMAIN_H
