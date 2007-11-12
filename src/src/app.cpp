/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "app.h"
#include <wx/fs_zip.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/cmdline.h>
#include <wx/regex.h>
#include <wx/filefn.h>
#include <wx/log.h> // for wxSafeShowMessage()
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/notebook.h>
#include <wx/clipbrd.h>
#include <wx/taskbar.h>
#include <cbexception.h>
#if wxCHECK_VERSION(2,6,0)
    #include <wx/debugrpt.h>
#endif
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <personalitymanager.h>
#include <pluginmanager.h>
#include <sdk_events.h>
#include <manager.h>
#include <scriptingmanager.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <globals.h>
#include <logmanager.h>
#include <loggers.h>
#include "splashscreen.h"
#include <wx/arrstr.h>
#include "crashhandler.h"

#include <sqplus.h>

#ifndef __WXMSW__
    #include "prefix.h" // binreloc
#else
    #include "associations.h"
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <sys/param.h>
#include <mach-o/dyld.h>
#endif

#include "appglobals.h"

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include "xtra_res.h"
    #include "filemanager.h" // LoaderBase
#endif

#ifndef APP_PREFIX
#define APP_PREFIX ""
#endif

#ifndef __WXMAC__
wxString GetResourcesDir(){ return wxEmptyString; };
#endif

namespace
{
// this list will be filled with files
// (received through DDE or command line)
// to be loaded after the app has started up
wxArrayString s_DelayedFilesToOpen;
bool s_Loading = false;

#ifdef __WXMSW__

class DDEServer : public wxServer
{
    public:
        DDEServer(MainFrame* frame) : m_Frame(frame) {}
        wxConnectionBase *OnAcceptConnection(const wxString& topic);
        MainFrame* GetFrame(){ return m_Frame; }
        void SetFrame(MainFrame* frame){ m_Frame = frame; }
    private:
        MainFrame* m_Frame;
};

class DDEConnection : public wxConnection
{
    public:
        DDEConnection(MainFrame* frame) : m_Frame(frame) {}
        bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
    private:
        MainFrame* m_Frame;
};

wxConnectionBase* DDEServer::OnAcceptConnection(const wxString& topic)
{
    return topic == DDE_TOPIC ? new DDEConnection(m_Frame) : 0L;
}

bool DDEConnection::OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format)
{
    wxString strData(data);

    if (!strData.StartsWith(_T("[Open(\"")))
        return false;

    wxRegEx reCmd(_T("\"(.*)\""));
    if (reCmd.Matches(strData))
    {
        const wxString file = reCmd.GetMatch(strData, 1);
        if (s_Loading)
        {
            s_DelayedFilesToOpen.Add(file);
        }
        else if (m_Frame)
        {
            m_Frame->Open(file, true); // add to history, files that open through DDE
        }
    }
    return true;
}

DDEServer* g_DDEServer = 0L;
#endif

#if wxUSE_CMDLINE_PARSER
const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, _T(""), _T("safe-mode"), _T("load in safe mode (all plugins will be disabled)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
#ifdef __WXMSW__
    { wxCMD_LINE_SWITCH, _T("na"), _T("no-check-associations"), _T("don't perform any association checks"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("nd"), _T("no-dde"), _T("don't start a DDE server"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
#endif
    { wxCMD_LINE_SWITCH, _T("ns"), _T("no-splash-screen"), _T("don't display a splash screen while loading"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("d"), _T("debug-log"), _T("display application's debug log"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("nc"), _T("no-crash-handler"), _T("don't use the crash handler (useful for debugging C::B)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("prefix"),  _T("the shared data dir prefix"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, _T("p"), _T("personality"),  _T("the personality to use: \"ask\" or <personality-name>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_SWITCH, _T(""), _T("no-log"),  _T("turn off the application log"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("log-to-file"),  _T("redirect application log to a file"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("profile"),  _T("synonym to personality"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_SWITCH, _T(""), _T("rebuild"), _T("clean and then build the project/workspace"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("build"), _T("just build the project/workspace"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("clean"), _T("clean the project/workspace"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("target"),  _T("the target for the batch build"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_SWITCH, _T(""), _T("no-batch-window-close"),  _T("do not auto-close log window when batch build is done"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("batch-build-notify"),  _T("show message when batch build is done"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("script"),  _T("execute script file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_PARAM, _T(""), _T(""),  _T("filename(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
    { wxCMD_LINE_NONE }
};
#endif // wxUSE_CMDLINE_PARSER

class Splash
{
    public:
        Splash(const bool show) : m_pSplash(0)
        {
            if (show)
            {
                wxBitmap bmp = cbLoadBitmap(ConfigManager::ReadDataPath() + _T("/images/splash_new.png"));
                m_pSplash = new cbSplashScreen(bmp, -1, 0, -1, wxNO_BORDER | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED);
                Manager::Yield();
            }
        }
        ~Splash() { Hide(); }
        void Hide()
        {
            if (m_pSplash)
            {
                m_pSplash->Destroy();
                m_pSplash = 0;
            }
        }
    private:
        cbSplashScreen* m_pSplash;
};
}; // namespace

IMPLEMENT_APP(CodeBlocksApp)

BEGIN_EVENT_TABLE(CodeBlocksApp, wxApp)
    EVT_ACTIVATE_APP(CodeBlocksApp::OnAppActivate)
END_EVENT_TABLE()

#ifdef __WXMAC__
#include "wx/mac/corefoundation/cfstring.h"
#include "wx/intl.h"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>

// returns e.g. "/Applications/appname.app/Contents/Resources" if application is bundled,
// or the directory of the binary, e.g. "/usr/local/bin/appname", if it is *not* bundled.
static wxString GetResourcesDir()
{
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    CFURLRef absoluteURL = CFURLCopyAbsoluteURL(resourcesURL); // relative -> absolute
    CFRelease(resourcesURL);
    CFStringRef cfStrPath = CFURLCopyFileSystemPath(absoluteURL,kCFURLPOSIXPathStyle);
    CFRelease(absoluteURL);
    return wxMacCFStringHolder(cfStrPath).AsString(wxLocale::GetSystemEncoding());
}
#endif

bool CodeBlocksApp::LoadConfig()
{
    if (ParseCmdLine(0L) == -1) // only abort if '--help' was passed in the command line
        return false;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

    wxString data(wxT(APP_PREFIX));

    if(platform::windows)
    {
        data.assign(GetAppPath());
    }
    else if(platform::macosx)
    {
        data.assign(GetResourcesDir());                 // CodeBlocks.app/Contents/Resources
        if (!data.Contains(wxString(_T("/Resources")))) // not a bundle, use relative path
            data = GetAppPath() + _T("/..");
    }

    if(data.IsEmpty())
    {
        data.assign(GetAppPath());  // fallback
        data.Replace(_T("/bin"),_T(""));
    }


    if (!m_Prefix.IsEmpty())        // --prefix command line switch overrides builtin value
    {
        data = m_Prefix;
    }
    else                            // also, check for environment
    {

        wxString env;
        wxGetEnv(_T("CODEBLOCKS_DATA_DIR"), &env);
        if (!env.IsEmpty())
            data = env;
    }

    data.append(_T("/share/codeblocks"));

    cfg->Write(_T("data_path"), data);

    //m_HasDebugLog = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/has_debug_log"), false) || m_HasDebugLog;
    //Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/has_debug_log"), m_HasDebugLog);

    return true;
}

void CodeBlocksApp::InitAssociations()
{
#ifdef __WXMSW__
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (!m_NoAssocs && cfg->ReadBool(_T("/environment/check_associations"), true))
    {
        if (!Associations::Check())
        {
            AskAssocDialog dlg(Manager::Get()->GetAppWindow());
            PlaceWindow(&dlg);

            switch(dlg.ShowModal())
            {
            case 0:
                Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/check_associations"), false);
                break;
            case 1:
                break;
            case 2:
                Associations::SetCore();
                break;
            case 3:
                Associations::SetAll();
                break;
            };

        }
    }
#endif
}

void CodeBlocksApp::InitDebugConsole()
{
#ifdef __WXMSW__
    #ifdef __CBDEBUG__
    // Remember to compile as a console application!
    AllocConsole();
    HANDLE myhandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD co = {80,2000};
    SetConsoleScreenBufferSize(myhandle, co);
    fprintf(stdout,"CONSOLE DEBUG ACTIVATED\n");
    // wxLogWindow *myerr = new wxLogWindow(NULL,"debug");
    #endif
#endif
}

void CodeBlocksApp::InitExceptionHandler()
{
#ifdef __WXMSW__
    m_ExceptionHandlerLib = LoadLibrary(_T("exchndl.dll"));
#endif
}


bool CodeBlocksApp::InitXRCStuff()
{
    if (!Manager::LoadResource(_T("resources.zip")))
        {
            ComplainBadInstall();
            return false;
        }
    return true;
}

MainFrame* CodeBlocksApp::InitFrame()
{
    CompileTimeAssertion<wxMinimumVersion<2,6>::eval>::Assert();
    MainFrame *frame = new MainFrame();
    wxUpdateUIEvent::SetUpdateInterval(100);
    SetTopWindow(0);
    //frame->Hide(); // shouldn't need this explicitely
#ifdef __WXMSW__
    if (!m_NoDDE)
    {
        g_DDEServer = new DDEServer(frame);
        g_DDEServer->Create(DDE_SERVICE);
        g_DDEServer->SetFrame(frame);
    }
#endif
    if (ParseCmdLine(frame) == 0)
    {
        if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/blank_workspace"), true) == false)
            Manager::Get()->GetProjectManager()->LoadWorkspace();
    }

    if (!m_Batch && m_Script.IsEmpty())
    {
#ifdef wxUSE_BINDERAPP
        SetGlobalHandler(frame);
        SetGlobalBinder(frame->m_KeyProfiles.Item(0));
#endif
    }
    return frame;
}

void CodeBlocksApp::CheckVersion()
{
    // This is a rudiment from early 2006 (Windows only), but keep the revision tag for possible future use
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

    if(cfg->Read(_T("version")) != appglobals::AppActualVersion)
        cfg->Write(_T("version"), appglobals::AppActualVersion);
}

void CodeBlocksApp::InitLocale()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));

    wxString path(ConfigManager::GetDataFolder() + _T("/locale"));

    if(cfg->ReadBool(_T("/locale/enable"), true) == false)
        return;

    wxString lang(cfg->Read(_T("/locale/language")));

    wxLocale::AddCatalogLookupPathPrefix(path);


    const wxLanguageInfo *info;

    if(lang)
        info = wxLocale::FindLanguageInfo(lang);
    else
        info = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);

    if(info == 0) // should never happen, but who knows...
        return;

    m_locale.Init(info->Language);

    path.Alloc(path.length() + 10);
    path.Append(_T('/'));
    path.Append(info->CanonicalName);

    wxDir dir(path);

    if (!dir.IsOpened())
        return;

    wxString moName;

    if(dir.GetFirst(&moName, _T("*.mo"), wxDIR_FILES))
    do
    {
        m_locale.AddCatalog(moName);
    }while(dir.GetNext(&moName));
}

bool CodeBlocksApp::OnInit()
{
    wxLog::EnableLogging(false);

    SetAppName(_T("codeblocks"));
    s_Loading = true;

    m_pBatchBuildDialog = 0;
    m_BatchExitCode = 0;
    m_Batch = false;
    m_BatchNotify = false;
    m_Build = false;
    m_ReBuild = false;
    m_Clean = false;
    m_HasProject = false;
    m_HasWorkSpace = false;
    m_SafeMode = false;

    m_BatchWindowAutoClose = true;

    wxTheClipboard->Flush();

    // NOTE: crash handler explicitly disabled because it causes problems
    //       with plugins loading/unloading...
    //
    // static CrashHandler crash_handler(m_NoCrashHandler);

    // we'll do this once and for all at startup
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
    wxInitAllImageHandlers();
    wxXmlResource::Get()->InitAllHandlers();

    try
    {
    #if (wxUSE_ON_FATAL_EXCEPTION == 1)
        wxHandleFatalExceptions(true);
    #endif

        InitExceptionHandler();

        if(!LoadConfig())
            return false;

		// set safe-mode appropriately
		PluginManager::SetSafeMode(m_SafeMode);

        if(!m_Batch && m_Script.IsEmpty() && !InitXRCStuff())
        {
           // wsSafeShowMessage(_T("Fatal error"), _T("Initialisation of resources failed."));
            return false;
        }

        Splash splash(!m_Batch && m_Script.IsEmpty() &&
                      !m_NoSplash &&
                      Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/show_splash"), true));

        InitLocale();
        m_pSingleInstance = 0;
        if(Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/single_instance"), true))
        {
            const wxString name = wxString::Format(_T("Code::Blocks-%s"), wxGetUserId().c_str());

            m_pSingleInstance = new wxSingleInstanceChecker(name, ConfigManager::GetTempFolder());
            if (m_pSingleInstance->IsAnotherRunning())
            {
                splash.Hide();
                /* NOTE: Due to a recent change in logging code, this visual warning got disabled.
                   So the wxLogError() has been changed to a wxMessageBox(). */
                wxMessageBox(_("Another program instance is already running.\nCode::Blocks is currently configured to only allow one running instance.\n\nYou can access this Setting under the menu item 'Environment'."),
                            _T("Code::Blocks"), wxOK | wxICON_ERROR);
                return false;
            }
        }

        InitDebugConsole();

        Manager::SetBatchBuild(m_Batch || !m_Script.IsEmpty());
        Manager::Get()->GetScriptingManager();
        MainFrame* frame = 0; frame = InitFrame();
        m_Frame = frame;

        if (m_Batch)
        {
        	// the compiler plugin might be waiting for this
			CodeBlocksEvent event(cbEVT_APP_STARTUP_DONE);
			Manager::Get()->ProcessEvent(event);

			Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<CodeBlocksApp, CodeBlocksEvent>(this, &CodeBlocksApp::OnBatchBuildDone));
            s_Loading = false;
            LoadDelayedFiles(frame);

            BatchJob();
            frame->Close();
            return true;
        }

        if (!m_Script.IsEmpty())
        {
            s_Loading = false;
            LoaderBase* loader = Manager::Get()->GetFileManager()->Load(m_Script);

            if(loader->GetData())
                Manager::Get()->GetScriptingManager()->LoadBuffer(cbC2U(loader->GetData()));

            frame->Close();
            return true;
        }

        CheckVersion();

        // run startup script
        try
        {
            wxString startup = ConfigManager::LocateDataFile(_T("startup.script"), sdScriptsUser | sdScriptsGlobal);
            if (!startup.IsEmpty())
            {
                Manager::Get()->GetScriptingManager()->LoadScript(startup);
                // no more need for main() in startup script
//                SqPlus::SquirrelFunction<void> f("main");
//                f();
            }
        }
        catch (SquirrelError& exception)
        {
            Manager::Get()->GetScriptingManager()->DisplayErrors(&exception);
        }
        Manager::ProcessPendingEvents();

        // finally, show the app
        splash.Hide();
        SetTopWindow(frame);
        frame->Show();

        frame->ShowTips(); // this func checks if the user wants tips, so no need to check here

        if(platform::windows)
            InitAssociations();

        s_Loading = false;

        LoadDelayedFiles(frame);
        Manager::Get()->GetProjectManager()->WorkspaceChanged();

		// all done
		CodeBlocksEvent event(cbEVT_APP_STARTUP_DONE);
		Manager::Get()->ProcessEvent(event);

        return true;
    }
    catch (cbException& exception)
    {
        exception.ShowErrorMessage();
    }
    catch (SquirrelError& exception)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&exception);
    }
    catch (const char* message)
    {
        wxSafeShowMessage(_T("Exception"), cbC2U(message));
    }
    catch (...)
    {
        wxSafeShowMessage(_T("Exception"), _T("Unknown exception was raised. The application will terminate immediately..."));
    }
    // if we reached here, return error
    return false;
}

int CodeBlocksApp::OnExit()
{
    wxTheClipboard->Flush();

#ifdef __WXMSW__
    delete g_DDEServer;
    if (m_ExceptionHandlerLib)
        FreeLibrary(m_ExceptionHandlerLib);
#endif
    if (m_pSingleInstance)
        delete m_pSingleInstance;

    // ultimate shutdown...
    Manager::Free();

    // WX docs say that this function's return value is ignored,
    // but we return our value anyway. It might not be ignored at some point...
    return m_Batch ? m_BatchExitCode : 0;
}

int CodeBlocksApp::OnRun()
{
    try
    {
        int retval = wxApp::OnRun();
        // wx 2.6.3 docs says that OnRun() function's return value is used as exit code
        return m_Batch ? m_BatchExitCode : retval;
    }
    catch (cbException& exception)
    {
        exception.ShowErrorMessage();
    }
    catch (SquirrelError& exception)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&exception);
    }
    catch (const char* message)
    {
        wxSafeShowMessage(_("Exception"), cbC2U(message));
    }
    catch (...)
    {
        wxSafeShowMessage(_("Exception"), _("Unknown exception was raised. The application will terminate immediately..."));
    }
    // if we reached here, return error
    return -1;
}

bool CodeBlocksApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    return wxApp::OnCmdLineParsed(parser);
}

void CodeBlocksApp::OnFatalException()
{
#if wxCHECK_VERSION(2,6,0) && wxUSE_DEBUGREPORT && wxUSE_XML && wxUSE_ON_FATAL_EXCEPTION
    wxDebugReport report;
    wxDebugReportPreviewStd preview;

    report.AddAll();
    if ( preview.Show(report) )
        report.Process();
#else
    cbMessageBox(wxString::Format(_("Something has gone wrong inside %s and it "
                                    "will terminate immediately.\n"
                                    "We are sorry for the inconvenience..."), appglobals::AppName.c_str()));
#endif
}

int CodeBlocksApp::BatchJob()
{
    if (!m_Batch)
        return -1;

    // find compiler plugin
    PluginsArray arr = Manager::Get()->GetPluginManager()->GetCompilerOffers();
    if (arr.GetCount() == 0)
        return -2;

    cbCompilerPlugin* compiler = static_cast<cbCompilerPlugin*>(arr[0]);
    if (!compiler)
        return -3;

    if (!m_Clean && m_BatchTarget.Lower() == _T("ask"))
    {
        m_BatchTarget.Clear();
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
        {
            int idx = -1;
            wxString defTarget = prj->GetActiveBuildTarget();
            // find active target's index
            // TODO: make this easier in the SDK
            for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
            {
                ProjectBuildTarget* target = prj->GetBuildTarget(i);
                if (target->GetTitle().Matches(defTarget))
                {
                    idx = i;
                    break;
                }
            }
            idx = prj->SelectTarget(idx, false);
            if (idx == -1)
                return 0; // no target selected: just abort
            m_BatchTarget = prj->GetBuildTarget(idx)->GetTitle();
        }
    }

    wxTaskBarIcon* tbIcon = new wxTaskBarIcon();
    tbIcon->SetIcon(
            #ifdef __WXMSW__
                wxICON(A_MAIN_ICON),
            #else
                wxIcon(app),
            #endif // __WXMSW__
                _("Building ") + wxFileNameFromPath(wxString(argv[argc-1])));

    m_pBatchBuildDialog = m_Frame->GetBatchBuildDialog();
    PlaceWindow(m_pBatchBuildDialog);
    m_pBatchBuildDialog->Show();

    if (m_ReBuild)
    {
        if(m_HasProject)
        {
            compiler->Rebuild(m_BatchTarget);
        }
        else if(m_HasWorkSpace)
        {
            compiler->RebuildWorkspace(m_BatchTarget);
        }
    }
    else if (m_Build)
    {
        if(m_HasProject)
        {
            compiler->Build(m_BatchTarget);
        }
        else if(m_HasWorkSpace)
        {
            compiler->BuildWorkspace(m_BatchTarget);
        }
    }
    else if (m_Clean)
    {
        if(m_HasProject)
        {
            compiler->Clean(m_BatchTarget);
        }
        else if(m_HasWorkSpace)
        {
            compiler->CleanWorkspace(m_BatchTarget);
        }
    }

    // the batch build log might have been deleted in
    // CodeBlocksApp::OnBatchBuildDone().
    // if it hasn't, it's still compiling
    //
    // also note that if operation is "--clean", there is no need
    // to display the dialog as the operation is synchronous and it
    // already has finished by the time the call to Clean() returned...
    if (!m_Clean && m_pBatchBuildDialog)
        m_pBatchBuildDialog->ShowModal();

    tbIcon->RemoveIcon();
    delete tbIcon;
    if (m_pBatchBuildDialog)
        m_pBatchBuildDialog->Destroy();
    m_pBatchBuildDialog = 0;

    return 0;
}

void CodeBlocksApp::OnBatchBuildDone(CodeBlocksEvent& event)
{
    event.Skip();
    // the event comes more than once. deal with it...
    static bool one_time_only = false;
    if (!m_Batch || one_time_only)
        return;
    one_time_only = true;

    cbCompilerPlugin* compiler = static_cast<cbCompilerPlugin*>(event.GetPlugin());
    m_BatchExitCode = compiler->GetExitCode();

    if (m_BatchNotify)
    {
        wxString msg;
        if (m_BatchExitCode == 0)
            msg << _("Batch build ended.\n");
        else
            msg << _("Batch build stopped with errors.\n");
        msg << wxString::Format(_("Process exited with status code %d."), m_BatchExitCode);
        cbMessageBox(msg, appglobals::AppName, m_BatchExitCode == 0 ? wxICON_INFORMATION : wxICON_WARNING);
    }
    else
        wxBell();

    if (m_pBatchBuildDialog && m_BatchWindowAutoClose)
    {
        m_pBatchBuildDialog->EndModal(wxID_OK);
        m_pBatchBuildDialog->Destroy();
        m_pBatchBuildDialog = 0;
    }
}

void CodeBlocksApp::ComplainBadInstall()
{
    wxString msg;
    msg.Printf(_T("Cannot find resources...\n"
        "%s was configured to be installed in '%s'.\n"
        "Please use the command-line switch '--prefix' or "
        "set the CODEBLOCKS_DATA_DIR environment variable "
        "to point where %s is installed,\n"
        "or try re-installing the application..."),
        appglobals::AppName.c_str(),
        ConfigManager::ReadDataPath().c_str(),
        appglobals::AppName.c_str());
    cbMessageBox(msg);
}

wxString CodeBlocksApp::GetAppPath() const
{
    wxString base;
#ifdef __WXMSW__
    wxChar name[MAX_PATH] = {0};
    GetModuleFileName(0L, name, MAX_PATH);
    wxFileName fname(name);
    base = fname.GetPath(wxPATH_GET_VOLUME);
#else
    if (!m_Prefix.IsEmpty())
        return m_Prefix;

#ifdef SELFPATH
    // SELFPATH is a macro from prefix.h (binreloc)
    // it returns the absolute filename of us
    // similar to win32 GetModuleFileName()...
    base = wxString(SELFPATH,wxConvUTF8);
    base = wxFileName(base).GetPath();
#endif
#if defined(sun) || defined(__sun)
    base = wxString(getexecname(),wxConvCurrent);
    base = wxFileName(base).GetPath();
#endif
#if defined(__APPLE__) && defined(__MACH__)
    char path[MAXPATHLEN+1];
    uint32_t path_len = MAXPATHLEN;
    // SPI first appeared in Mac OS X 10.2
    _NSGetExecutablePath(path, &path_len);
    base = wxString(path, wxConvUTF8);
    base = wxFileName(base).GetPath();
#endif
    if (base.IsEmpty())
        base = _T(".");
#endif
    return base;
}

int CodeBlocksApp::ParseCmdLine(MainFrame* handlerFrame)
{
    // code shamelessely taken from the console wxWindows sample :)
    bool filesInCmdLine = false;

#if wxUSE_CMDLINE_PARSER
    wxCmdLineParser& parser = *Manager::GetCmdLineParser();
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(argc, argv);
    // wxApp::argc is a wxChar**

    // don't display errors as plugins will have the chance to parse the command-line
    // too, so we don't know here what exactly are the supported options
    switch ( parser.Parse(false) )
    {
        case -1:
            parser.Usage();
            return -1;

        case 0:
            {
                if (handlerFrame)
                {
                    int count = parser.GetParamCount();
                    filesInCmdLine = count != 0;
                    m_HasProject = false;
                    m_HasWorkSpace = false;
                    for ( int param = 0; param < count; ++param )
                    {
                        // is it a project/workspace?
                        FileType ft = FileTypeOf(parser.GetParam(param));
                        if (ft == ftCodeBlocksProject)
                        {
                            m_HasProject = true;
                            s_DelayedFilesToOpen.Add(parser.GetParam(param));
                        }
                        else if (ft == ftCodeBlocksWorkspace)
                        {
                            // only one workspace can be opened
                            m_HasWorkSpace = true;
                            s_DelayedFilesToOpen.Clear(); // remove all other files
                            s_DelayedFilesToOpen.Add(parser.GetParam(param)); // and add only the workspace
                            break; // and stop processing any more files
                        }
                    }

                    // batch jobs
                    m_Batch = m_HasProject || m_HasWorkSpace;
                    m_Batch = m_Batch && (m_Build || m_ReBuild || m_Clean);
                }
                else
                {
                    wxString val;
                    parser.Found(_T("prefix"), &m_Prefix);
#ifdef __WXMSW__
                    m_NoDDE = parser.Found(_T("no-dde"));
                    m_NoAssocs = parser.Found(_T("no-check-associations"));
#endif
                    m_SafeMode = parser.Found(_T("safe-mode"));
                    m_NoSplash = parser.Found(_T("no-splash-screen"));
                    m_HasDebugLog = parser.Found(_T("debug-log"));
                    m_NoCrashHandler = parser.Found(_T("no-crash-handler"));
                    if (parser.Found(_T("personality"), &val) ||
                        parser.Found(_T("profile"), &val))
                    {
                        SetupPersonality(val);
                    }

                    // batch jobs
                    m_BatchNotify = parser.Found(_T("batch-build-notify"));
                    m_BatchWindowAutoClose = !parser.Found(_T("no-batch-window-close"));
                    m_Build = parser.Found(_T("build"));
                    m_ReBuild = parser.Found(_T("rebuild"));
                    m_Clean = parser.Found(_T("clean"));
                    parser.Found(_T("target"), &m_BatchTarget);
                    parser.Found(_T("script"), &m_Script);
                    // initial setting for batch flag (will be reset when ParseCmdLine() is called again).
                    m_Batch = m_Build || m_ReBuild || m_Clean;


                    if(parser.Found(_T("no-log")) == false)
                        Manager::Get()->GetLogManager()->SetLog(new TextCtrlLogger, LogManager::app_log);
                    if(parser.Found(_T("log-to-file")))
                        Manager::Get()->GetLogManager()->SetLog(new FileLogger(_T("codeblocks.log")), LogManager::app_log);
                    if(m_HasDebugLog)
                        Manager::Get()->GetLogManager()->SetLog(new TextCtrlLogger, LogManager::debug_log);
                }
            }
            break;

        default:
            return 1; // syntax error / unknown option
    }
#endif // wxUSE_CMDLINE_PARSER
    return filesInCmdLine ? 1 : 0;
}

void CodeBlocksApp::SetupPersonality(const wxString& personality)
{
    if (personality.CmpNoCase(_T("ask")) == 0)
    {
        CompileTimeAssertion<wxMinimumVersion<2,5>::eval>::Assert(); // just to make sure: wxWidgets 2.4 is dead

        const wxArrayString items(Manager::Get()->GetPersonalityManager()->GetPersonalitiesList());

        wxSingleChoiceDialog dlg(0, _("Please choose which personality (profile) to load:"),
                                    _("Personalities (profiles)"),
                                    items);

        if (dlg.ShowModal() == wxID_OK)
            Manager::Get()->GetPersonalityManager()->SetPersonality(dlg.GetStringSelection());
    }
    else
    {
        Manager::Get()->GetPersonalityManager()->SetPersonality(personality, true);
    }
}

void CodeBlocksApp::LoadDelayedFiles(MainFrame *const frame)
{
    for (size_t i = 0; i < s_DelayedFilesToOpen.GetCount(); ++i)
    {
        frame->Open(s_DelayedFilesToOpen[i], true);
    }
    s_DelayedFilesToOpen.Clear();
}


#ifdef __WXMAC__

void CodeBlocksApp::MacOpenFile(const wxString & fileName )
{
    if (s_Loading)
    {
        s_DelayedFilesToOpen.Add(fileName);
    }
    else if (m_Frame)
    {
        m_Frame->Open(fileName, true);
    }
}

void CodeBlocksApp::MacPrintFile(const wxString & fileName )
{
    //TODO
    wxApp::MacPrintFile(fileName);
}

#endif // __WXMAC__

// event handlers

void CodeBlocksApp::OnAppActivate(wxActivateEvent& event)
{
	// allow others to process this event
	event.Skip();

    if (s_Loading)
        return; // still loading; we can't possibly be interested for this event ;)
    if (!event.GetActive())
        return;
    if (!Manager::Get())
        return;

    if (Manager::Get()->GetEditorManager() && Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/check_modified_files"), true))
    {
        // for some reason a mouse up event doen's make it into scintilla (scintilla bug)
        // therefor the workaournd is not to directly call the editorManager, but
        // take a detour through an event
        // the bug is when the file has been offered to reload, no matter what answer you
        // give the mouse is in a selecting mode, adding/removing things to it's selection as you
        // move it around
        // so : idEditorManagerCheckFiles, EditorManager::OnCheckForModifiedFiles just exist for this workaround
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idEditorManagerCheckFiles);
        wxPostEvent(Manager::Get()->GetEditorManager(), evt);
//        Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
        Manager::Get()->GetProjectManager()->CheckForExternallyModifiedProjects();
    }
    cbEditor* ed = Manager::Get()->GetEditorManager()
                    ? Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()
                    : 0;
    if (ed)
    {
        // hack for linux: without it, the editor loses the caret every second activate o.O
        Manager::Get()->GetEditorManager()->GetNotebook()->SetFocus();

        ed->GetControl()->SetFocus();
    }
}
