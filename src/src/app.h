/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CODEBLOCKS_APP_H
#define CODEBLOCKS_APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !defined(WX_PRECOMP)
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

#include <wx/taskbar.h>
#include <wx/splash.h>
#include <wx/snglinst.h>
#include "manager.h"
#include "main.h"

/** \mainpage Code::Blocks SDK
 *
 * The open source, cross platform, free C, C++ and Fortran IDE. The majority
 * of features in Code::Blocks are implemented through a variety of plugins.
 *
 * The following are some useful places to jump into the sdk.
 *
 * Plugin interfaces:
 * \dot
 * digraph Plugins
 * {
 *   node [fontname="FreeSans", fontsize="10", shape=record];
 *   rankdir="LR";
 *   a [label="cbPlugin", URL="\ref cbPlugin", tooltip="Base class for all Code::Blocks plugins.", height=0.2];
 *
 *   b [label="cbCodeCompletionPlugin", URL="\ref cbCodeCompletionPlugin", tooltip="Base class for code-completion plugins.",  height=0.2];
 *   a -> b [dir="back", color="MidnightBlue"];
 *   c [label="cbCompilerPlugin",       URL="\ref cbCompilerPlugin",       tooltip="Base class for compiler plugins.",         height=0.2];
 *   a -> c [dir="back", color="MidnightBlue"];
 *   d [label="cbDebuggerPlugin",       URL="\ref cbDebuggerPlugin",       tooltip="Base class for debugger plugins.",         height=0.2];
 *   a -> d [dir="back", color="MidnightBlue"];
 *   e [label="cbMimePlugin",           URL="\ref cbMimePlugin",           tooltip="Base class for mime plugins.",             height=0.2];
 *   a -> e [dir="back", color="MidnightBlue"];
 *   f [label="cbSmartIndentPlugin",    URL="\ref cbSmartIndentPlugin",    tooltip="Base class for automatic indent plugins.", height=0.2];
 *   a -> f [dir="back", color="MidnightBlue"];
 *   g [label="cbToolPlugin",           URL="\ref cbToolPlugin",           tooltip="Base class for tool plugins.",             height=0.2];
 *   a -> g [dir="back", color="MidnightBlue"];
 *   h [label="cbWizardPlugin",         URL="\ref cbWizardPlugin",         tooltip="Base class for wizard plugins.",           height=0.2];
 *   a -> h [dir="back", color="MidnightBlue"];
 * }
 * \enddot
 *
 * Managers:
 * \dot
 * digraph Managers
 * {
 *   node [fontname="FreeSans", fontsize="10", shape=record];
 *   rankdir="LR";
 *   a [label="Mgr\<MgrT\>", URL="\ref Mgr", tooltip="Singleton base for managers.", height=0.2];
 *
 *   b [label="CCManager",        URL="\ref CCManager",        tooltip="Governs access and shared code-completion functions.",  height=0.2];
 *   a -> b [dir="back", color="MidnightBlue"];
 *   c [label="ColourManager",    URL="\ref ColourManager",    tooltip="Manages configurable colour settings.",                 height=0.2];
 *   a -> c [dir="back", color="MidnightBlue"];
 *   d [label="ConfigManager",    URL="\ref ConfigManager",    tooltip="Unified settings storage.",                             height=0.2];
 *   a -> d [dir="back", color="MidnightBlue"];
 *   e [label="DebuggerManager",  URL="\ref DebuggerManager",  tooltip="Handles debugger plugins.",                             height=0.2];
 *   a -> e [dir="back", color="MidnightBlue"];
 *   f [label="EditorManager",    URL="\ref EditorManager",    tooltip="Handles open editors.",                                 height=0.2];
 *   a -> f [dir="back", color="MidnightBlue"];
 *   g [label="FileManager",      URL="\ref FileManager",      tooltip="Manages low level file access.",                        height=0.2];
 *   a -> g [dir="back", color="MidnightBlue"];
 *   h [label="LogManager",       URL="\ref LogManager",       tooltip="Access shared logging functions.",                      height=0.2];
 *   a -> h [dir="back", color="MidnightBlue"];
 *   i [label="MacrosManager",    URL="\ref MacrosManager",    tooltip="Handles text replacement macros.",                      height=0.2];
 *   a -> i [dir="back", color="MidnightBlue"];
 *   j [label="PluginManager",    URL="\ref PluginManager",    tooltip="Manages installed plugins.",                            height=0.2];
 *   a -> j [dir="back", color="MidnightBlue"];
 *   k [label="ProjectManager",   URL="\ref ProjectManager",   tooltip="Manages projects and the workspace.",                   height=0.2];
 *   a -> k [dir="back", color="MidnightBlue"];
 *   l [label="ScriptingManager", URL="\ref ScriptingManager", tooltip="Handles script registration and usage.",                height=0.2];
 *   a -> l [dir="back", color="MidnightBlue"];
 * }
 * \enddot
 *
 * Also see the <a href="http://wiki.codeblocks.org/index.php?title=Developer_documentation">developer documentation</a>
 * in the wiki.
 */


class cbSplashScreen;

class CodeBlocksApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        virtual int OnRun();
        wxString GetAppPath() const;
        int ParseCmdLine(MainFrame* handlerFrame, const wxString& CmdLine = wxEmptyString);
        void OnAppActivate(wxActivateEvent& event);
        bool OnCmdLineParsed(wxCmdLineParser& parser);
        void OnFatalException();
        void LoadDelayedFiles(MainFrame* frame); // command line or DDE (if available) files
        void SetAutoFile(wxString& file); // method to set m_AutoFile
#ifdef __WXMAC__
        // in response of an open-document apple event
        virtual void         MacOpenFile(const wxString &fileName) ;
        // in response of a print-document apple event
        virtual void         MacPrintFile(const wxString &fileName) ;
#endif
        void AddFileToOpenDelayed(const wxString& filename);
    protected:
        void OnBatchBuildDone(CodeBlocksEvent& event);
        void OnTBIconLeftDown(wxTaskBarIconEvent& event);
        bool LoadConfig();
        void InitDebugConsole();
        void InitExceptionHandler();
        bool InitXRCStuff();
        MainFrame* InitFrame();
        void CheckVersion();
        void InitLocale();
        int BatchJob();
        wxLocale m_locale; // locale we'll be using
        wxArrayString m_DelayedFilesToOpen;
    private:
        void SetupPersonality(const wxString& personality);


        wxString m_Prefix; // directory specified in --prefix switch
        wxString m_UserDataDir; // directory specified in --user-data-dir switch
        wxString m_BatchTarget;
        wxString m_Script;
        wxString m_AutoFile; // --file foo.cpp[:line]

        wxScrollingDialog* m_pBatchBuildDialog;
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
        bool m_Splash; // splash screen enabled
        bool m_HasDebugLog; // display debug log
        bool m_CrashHandler; // crash handler enabled
        bool m_SafeMode; // all plugins disabled

        void InitAssociations();

        bool m_DDE; // DDE enabled?
#ifdef __WXMSW__
        bool m_Assocs; // associations check enabled
        HINSTANCE m_ExceptionHandlerLib;
#endif
        int m_BatchExitCode;

        DECLARE_EVENT_TABLE()
};

DECLARE_APP(CodeBlocksApp);

#endif // CODEBLOCKS_APP_H
