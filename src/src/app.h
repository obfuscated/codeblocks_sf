#ifndef CODEBLOCKS_APP_H
#define CODEBLOCKS_APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

/**
@remarks VC++ 6 requires the following headers to compile correctly, so I added a check for it
*/
#if !defined(WX_PRECOMP) || defined(_MSC_VER)
    #include <wx/wx.h>
    #include <wx/intl.h>
    #include <wx/laywin.h>
    #include <wx/image.h>
    #include <wx/filename.h>
    #include <wx/hashmap.h>
    #include <wx/docview.h> // recent files history
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "resources/icons/app.xpm"
#endif

#ifdef __WXMSW__
    #include <wx/ipc.h>
#endif

#include <wx/splash.h>
#include <wx/snglinst.h>
#include "manager.h"
#include "main.h"

class cbSplashScreen;

class CodeBlocksApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        virtual int OnRun();
        wxString GetAppPath() const;
        int ParseCmdLine(MainFrame* handlerFrame);
        void OnAppActivate(wxActivateEvent& event);
        bool OnCmdLineParsed(wxCmdLineParser& parser);
        void OnFatalException();
#ifdef __WXMAC__
    // in response of an open-document apple event
    virtual void         MacOpenFile(const wxString &fileName) ;
    // in response of a print-document apple event
    virtual void         MacPrintFile(const wxString &fileName) ;
#endif
    protected:
        void OnBatchBuildDone(CodeBlocksEvent& event);
        bool LoadConfig();
        void InitDebugConsole();
        void InitExceptionHandler();
        bool InitXRCStuff();
        MainFrame* InitFrame();
        void CheckVersion();
        void InitLocale();
        int BatchJob();
        wxLocale m_locale; // locale we'll be using
    private:
        void ComplainBadInstall();
        void SetupPersonality(const wxString& personality);

        void LoadDelayedFiles(MainFrame* frame); // command line or DDE (if available) files

        wxString m_Prefix; // --prefix switch
        wxString m_BatchTarget;
        wxString m_Script;

        wxDialog* m_pBatchBuildDialog;
        MainFrame* m_Frame;
        wxSingleInstanceChecker* m_pSingleInstance;

        bool m_Batch;
        bool m_BatchNotify;
        bool m_BatchWindowAutoClose; // default: true
        bool m_Build;
        bool m_ReBuild;
        bool m_Clean;
        bool m_HasProject;
        bool m_HasWorkSpace;
        bool m_NoSplash; // no splash screen
        bool m_HasDebugLog; // display debug log
        bool m_NoCrashHandler; // no crash handler

        void InitAssociations();

#ifdef __WXMSW__
        bool m_NoDDE; // no DDE
        bool m_NoAssocs; // no associations check
        HINSTANCE m_ExceptionHandlerLib;
#endif
        int m_BatchExitCode;

        DECLARE_EVENT_TABLE()
};

DECLARE_APP(CodeBlocksApp);

#endif // CODEBLOCKS_APP_H
