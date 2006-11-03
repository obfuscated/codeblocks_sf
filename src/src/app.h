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
#include "../sdk/manager.h"
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

#ifdef __WXMSW__
        void InitAssociations();
        HINSTANCE m_ExceptionHandlerLib;
        bool m_NoDDE; // no DDE
        bool m_NoAssocs; // no associations check
#endif
        // batch jobs - start
        wxDialog* m_pBatchBuildDialog;
        wxString m_BatchTarget;
        int m_BatchExitCode;
        MainFrame* m_Frame;
        bool m_Batch;
        bool m_BatchNotify;
        bool m_BatchWindowAutoClose; // default: true
        bool m_Build;
        bool m_ReBuild;
        bool m_HasProject;
        bool m_HasWorkSpace;
        // batch jobs - end

        bool m_NoSplash; // no splash screen
        bool m_HasDebugLog; // display debug log
        wxString m_Prefix; // --prefix switch

		wxSingleInstanceChecker* m_pSingleInstance;

        DECLARE_EVENT_TABLE()
};

DECLARE_APP(CodeBlocksApp);

#endif // CODEBLOCKS_APP_H
