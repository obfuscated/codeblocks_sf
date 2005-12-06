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
* $Id$
* $Date$
*/

#include <sdk.h>
#include "app.h"
#include <wx/fs_zip.h>
#include <wx/xrc/xmlres.h>
#include <wx/cmdline.h>
#include <wx/regex.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/notebook.h>
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

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
	#include <shlobj.h> // for SHChangeNotify()
	#define DDE_SERVICE	_T("CODEBLOCKS")
	#define DDE_TOPIC	_T("CodeBlocksDDEServer")
	#ifdef __CBDEBUG__
	#include <windows.h>
	#include <wincon.h>
	#include <wx/log.h>
	#endif
#else
    #include "prefix.h" // binreloc
#endif

#include "globals.h"

#if wxUSE_CMDLINE_PARSER
static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
#ifdef __WXMSW__
    { wxCMD_LINE_SWITCH, _T("na"), _T("no-check-associations"), _T("don't perform any association checks"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("nd"), _T("no-dde"), _T("don't start a DDE server"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
#endif
    { wxCMD_LINE_SWITCH, _T("ns"), _T("no-splash-screen"), _T("don't display a splash screen while loading"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("d"), _T("debug-log"), _T("display application's debug log"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("clear-configuration"), _T("completely clear program's configuration"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("prefix"),  _T("the shared data dir prefix"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, _T("p"), _T("personality"),  _T("the personality to use: \"ask\" or <personality-name>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_SWITCH, _T(""), _T("rebuild"), _T("clean and then build the project/workspace"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T(""), _T("build"), _T("just build the project/workspace"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("target"),  _T("the target for the batch build"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_PARAM, _T(""), _T(""),  _T("filename(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
    { wxCMD_LINE_NONE }
};
#endif // wxUSE_CMDLINE_PARSER

#ifdef __WXMSW__
DDEServer* g_DDEServer = 0L;
#endif

IMPLEMENT_APP(CodeBlocksApp)

BEGIN_EVENT_TABLE(CodeBlocksApp, wxApp)
	EVT_ACTIVATE_APP(CodeBlocksApp::OnAppActivate)
END_EVENT_TABLE()

bool CodeBlocksApp::LoadConfig()
{
    if (ParseCmdLine(0L) != 0)

        return false;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    cfg->Write(_T("app_path"), GetAppPath());

    // find out about data path
#ifdef __WXMSW__
    wxString data = GetAppPath(); // under windows it is under the exe dir
#else
    wxString data = wxT(APP_PREFIX); // under linux, get the preprocessor value
#endif
    wxString actualData = _T("/share/codeblocks");
    data << actualData;
    // check if the user has passed --prefix in the command line
    if (!m_Prefix.IsEmpty())
        data = m_Prefix + actualData;
    else
    {
        // if no --prefix passed, check for the environment variable
        wxString env;
        wxGetEnv(_T("CODEBLOCKS_DATA_DIR"), &env);
        if (!env.IsEmpty())
            data = env + actualData;
    }

    cfg->Write(_T("data_path"), data);
    m_HasDebugLog = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/has_debug_log"), false) || m_HasDebugLog;
    Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/has_debug_log"), m_HasDebugLog);
    return true;
}

void CodeBlocksApp::InitAssociations()
{
#ifdef __WXMSW__
	ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
	if (!m_NoAssocs && cfg->ReadBool(_T("/environment/check_associations"), true))
		CheckAssociations();

	if (!m_NoDDE && cfg->ReadBool(_T("/environment/use_dde"), true))
	{
		g_DDEServer = new DDEServer(0);
		g_DDEServer->Create(DDE_SERVICE);
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

void CodeBlocksApp::InitImageHandlers()
{
    wxImage::AddHandler(new wxBMPHandler);
    wxImage::AddHandler(new wxPNGHandler);
	wxInitAllImageHandlers();
}

void CodeBlocksApp::ClearConf()
{
    int ret = wxMessageBox(_("Do you want to clear all Code::Blocks configuration settings?"), _("Clear configuration settings"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT);
    if (ret == wxYES)
    {
        ret = wxMessageBox(_("Are you *really* sure you want to clear all Code::Blocks configuration settings?"), _("Clear configuration settings"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT);
        if (ret == wxYES)
        {
//            OldConfigManager::Get()->DeleteAll(); // leave this for now
            //Manager::Get()->GetConfigManager(_T("app"))->DeleteAll();
            ret = wxMessageBox(_("Code::Blocks configuration settings cleared"), _("Information"), wxICON_INFORMATION);
        }
    }
    // When using the --clear-configuration switch, the program doesn't run
    // no matter what the answer is for the above questions.
    // This switch is used by the uninstaller also...
}

bool CodeBlocksApp::InitXRCStuff()
{
    // load all application-resources
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();

    wxString res = ConfigManager::ReadDataPath() + _T("/resources.zip");

    if (!CheckResource(res))
    	return false;
    /// @todo Checkout why it doesn't work with VC++ unless "#zip:*.xrc" appended
	wxXmlResource::Get()->Load(res+_T("#zip:*.xrc"));
	return true;
}

MainFrame* CodeBlocksApp::InitFrame()
{
    MainFrame *frame = new MainFrame(m_locale, (wxFrame*)0L);
    SetTopWindow(0);
    frame->Hide(); // frame is shown by the caller
#ifdef __WXMSW__
    if(g_DDEServer)
        g_DDEServer->SetFrame(frame);
#endif
    if (ParseCmdLine(frame) == 0)
    {
        if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/blank_workspace"), true) == false)
            Manager::Get()->GetProjectManager()->LoadWorkspace();
    }

    if (!m_Batch)
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
#ifdef __WXMSW__
    // for windows users only, display a message that no compiler is provided
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

    if (cfg->Read(_T("version")) != APP_ACTUAL_VERSION)
    {
        // this is a (probably) newer version; show a message box with
        // important notes

        // NOTE:
        // the info box, has moved to the installer, because now there are
        // setup files including a compiler...

        // update the version
        cfg->Write(_T("version"), APP_ACTUAL_VERSION);
    }
#endif
}

void CodeBlocksApp::InitLocale()
{
    const wxString langs[] =
    {
        _T("(System default)"),
//        _T("English (U.S.)")
//        _T("English"),
//        _T("Chinese (Simplified)"),
//        _T("German"),
//        _T("Russian"),
    };

    // Must have the same order than the above
    const long int locales[] =
    {
        wxLANGUAGE_DEFAULT,
        wxLANGUAGE_ENGLISH_US,
        wxLANGUAGE_ENGLISH,
        wxLANGUAGE_CHINESE_SIMPLIFIED,
        wxLANGUAGE_GERMAN,
        wxLANGUAGE_RUSSIAN
    };

    long int lng = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/locale/language"),(long int)-2);

    if(lng==-2) // -2 = Undefined / ask
    {
        lng = -1;
        if(WXSIZEOF(langs)>=2)
            lng = wxGetSingleChoiceIndex(_T("Please choose language:"), _T("Language"),
                                           WXSIZEOF(langs), langs);

        if(lng >= 0 && abs(lng) < WXSIZEOF(locales))
            lng = locales[lng];
        else
            lng = -1; // -1 = Don't use locale
    }


    if(lng>=0)
    {
        m_locale.Init(lng);
        wxLocale::AddCatalogLookupPathPrefix(ConfigManager::GetDataFolder() + _T("/locale"));
        wxLocale::AddCatalogLookupPathPrefix(wxT("."));
        wxLocale::AddCatalogLookupPathPrefix(wxT(".."));
        m_locale.AddCatalog(wxT("codeblocks"));
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/locale/language"), (int)lng);
    }
    else
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/locale/language"),(int)-1);
}

bool CodeBlocksApp::OnInit()
{
    m_Batch = false;
    m_Build = false;
    m_ReBuild = false;
    m_BatchExitCode = 0;

    try
    {
        m_pSplash = 0;

    #if (wxUSE_ON_FATAL_EXCEPTION == 1)
        wxHandleFatalExceptions(true);
    #endif
        InitExceptionHandler();


        if(!LoadConfig())
            return false;

        InitImageHandlers();
        if (!m_Batch)
            ShowSplashScreen();

        InitLocale();
        m_pSingleInstance = 0;
        if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/single_instance"), true))
        {
            const wxString name = wxString::Format(_T("Code::Blocks-%s"), wxGetUserId().c_str());
            m_pSingleInstance = new wxSingleInstanceChecker(name);
            if (m_pSingleInstance->IsAnotherRunning())
            {
                HideSplashScreen();
                wxLogError(_("Another program instance is already running, aborting."));
                return false;
            }
        }

        InitAssociations();
        InitDebugConsole();
        if(m_ClearConf)
        {
            ClearConf();
            HideSplashScreen();
            return false;
        }
        if(!InitXRCStuff())
        {
            HideSplashScreen();
            return false;
        }

        MainFrame* frame = InitFrame();

        if (m_Batch)
        {
            m_BatchExitCode = BatchJob();
            return false;
        }

        CheckVersion();

        CodeBlocksEvent event(cbEVT_APP_STARTUP_DONE);
        Manager::Get()->ProcessEvent(event);
        wxYield();

        // run startup script
        Manager::Get()->GetScriptingManager()->LoadScript(_T("startup.script"));

        // finally, show the app
        HideSplashScreen();
        SetTopWindow(frame);
        frame->Show();
        frame->ShowTips(); // this func checks if the user wants tips, so no need to check here

        return true;
    }
    catch (cbException& exception)
    {
        exception.ShowErrorMessage();
    }
    catch (const char* message)
    {
        wxSafeShowMessage(_("Exception"), _U(message));
    }
    catch (...)
    {
        wxSafeShowMessage(_("Exception"), _("Unknown exception was raised. The application will terminate immediately..."));
    }
    // if we reached here, return error
    return false;
}

int CodeBlocksApp::OnExit()
{
#ifdef __WXMSW__
	if (g_DDEServer)
		delete g_DDEServer;
    if (m_ExceptionHandlerLib)
        FreeLibrary(m_ExceptionHandlerLib);
#endif
    if (m_pSingleInstance)
        delete m_pSingleInstance;
    return m_Batch ? m_BatchExitCode : 0;
}

int CodeBlocksApp::OnRun()
{
    try
    {
        return wxApp::OnRun();
    }
    catch (cbException& exception)
    {
        exception.ShowErrorMessage();
    }
    catch (const char* message)
    {
        wxSafeShowMessage(_("Exception"), _U(message));
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
#if wxCHECK_VERSION(2,6,0) && wxUSE_DEBUGREPORT && wxUSE_XML \
    && defined(__WXGTK__) // doesn't seem to work on windows...
    wxDebugReport report;
    wxDebugReportPreviewStd preview;

    report.AddAll();
    if ( preview.Show(report) )
        report.Process();
#else
    wxMessageBox(_("Something has gone wrong inside " APP_NAME " and it "
                    "will terminate immediately.\n"
                    "We are sorry for the inconvenience..."));
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

    if (m_ReBuild)
        compiler->RebuildWorkspace(m_BatchTarget);
    else if (m_Build)
        compiler->BuildWorkspace(m_BatchTarget);

    // wait for compiler to finish
    while (compiler->IsRunning())
    {
        wxMilliSleep(10);
        Yield(true);
    }

    return compiler->GetExitCode();
}

void CodeBlocksApp::ShowSplashScreen()
{
    HideSplashScreen();

	if (!m_NoSplash && Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/show_splash"), true) == true)
	{
		wxBitmap bitmap;
		#ifdef __WXMSW__
			#define SPLASH_IMAGE _T("/images/splash_new.png")
		#else
			#define SPLASH_IMAGE _T("/images/splash.png")
		#endif
		if (bitmap.LoadFile(ConfigManager::ReadDataPath() + SPLASH_IMAGE, wxBITMAP_TYPE_PNG))
		{
			m_pSplash = new wxSplashScreen(bitmap,
										wxSPLASH_CENTRE_ON_SCREEN,// | wxSPLASH_TIMEOUT,
										6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
										wxBORDER_NONE);
		}
		wxSafeYield();
	}
}

void CodeBlocksApp::HideSplashScreen()
{
    if (m_pSplash)
        delete m_pSplash;
    m_pSplash = 0;
}

bool CodeBlocksApp::CheckResource(const wxString& res)
{
    if (!wxFileExists(res))
    {
        HideSplashScreen();
    	wxString msg;
    	msg.Printf("Cannot find %s...\n"
    		APP_NAME" was configured to be installed in '%s'.\n"
    		"Please use the command-line switch '--prefix' or "
            "set the CODEBLOCKS_DATA_DIR environment variable "
    		"to point where "APP_NAME" is installed,\n"
    		"or try re-installing the application...",
    		res.c_str(),
    		ConfigManager::ReadDataPath().c_str());
    	wxMessageBox(msg);
    	return false;
    }

    return true;
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

    // SELFPATH is a macro from prefix.h (binreloc)
    // it returns the absolute filename of us
    // similar to win32 GetModuleFileName()...
    base = wxString(SELFPATH,wxConvUTF8);
    base = wxFileName(base).GetPath();
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
    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    // wxApp::argc is a wxChar**

    switch ( parser.Parse() )
    {
        case -1:
            return -1;

        case 0:
            {
                if (handlerFrame)
                {
                    int count = parser.GetParamCount();
					filesInCmdLine = count != 0;
					bool hasProjOrWksp = false;
                    for ( int param = 0; param < count; ++param )
                    {
                        if (handlerFrame->Open(parser.GetParam(param)) && !hasProjOrWksp)
                        {
                            // is it a project/workspace?
                            FileType ft = FileTypeOf(parser.GetParam(param));
                            if (ft == ftCodeBlocksProject || ft == ftCodeBlocksWorkspace)
                                hasProjOrWksp = true;
                        }
                    }

                    // batch jobs
                    m_Batch = hasProjOrWksp;
                    m_Batch = m_Batch && (m_Build || m_ReBuild);
                }
                else
                {
                    wxString val;
                    parser.Found(_T("prefix"), &m_Prefix);
#ifdef __WXMSW__
					m_NoDDE = parser.Found(_T("no-dde"), &val);
					m_NoAssocs = parser.Found(_T("no-check-associations"), &val);
#else
                    m_NoDDE = false;
                    m_NoAssocs = false;
#endif
					m_NoSplash = parser.Found(_T("no-splash-screen"), &val);
					m_ClearConf = parser.Found(_T("clear-configuration"), &val);
					m_HasDebugLog = parser.Found(_T("debug-log"), &val);
					if (parser.Found(_T("personality"), &val))
                        SetupPersonality(val);

                    // batch jobs
                    m_Build = parser.Found(_T("build"), &val);
                    m_ReBuild = parser.Found(_T("rebuild"), &val);
                    parser.Found(_T("target"), &m_BatchTarget);

                    // initial setting for batch flag (will be reset when ParseCmdLine() is called again).
                    m_Batch = m_Build || m_ReBuild;
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
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
        // wx < 2.5.x single choice dialog wants wxString*
        const wxArrayString& list = Manager::Get()->GetPersonalityManager()->GetPersonalitiesList();
        wxString* strings = new wxString[list.GetCount()];
        for (unsigned int i = 0; i < list.GetCount(); ++i)
            strings[i] = list[i];
#endif
        // display personality selection dialog
        wxSingleChoiceDialog dlg(0,
                                _("Please choose which personality to load:"),
                                _("Personalities"),
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
                                list.GetCount(), strings);
#else
                                Manager::Get()->GetPersonalityManager()->GetPersonalitiesList());
#endif
        if (dlg.ShowModal() == wxID_OK)
            Manager::Get()->GetPersonalityManager()->SetPersonality(dlg.GetStringSelection());
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
        delete[] strings;
#endif
    }
    else
        Manager::Get()->GetPersonalityManager()->SetPersonality(personality, true);
}

#ifdef __WXMSW__
void CodeBlocksApp::SetAssociations()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	DoSetAssociation(CODEBLOCKS_EXT, wxString(APP_NAME) + _(" project file"), name, _T("1"));
	DoSetAssociation(WORKSPACE_EXT, wxString(APP_NAME) +  _("workspace file"), name, _T("1"));
	DoSetAssociation(C_EXT, _("C source file"), name, _T("2"));
	DoSetAssociation(CPP_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CC_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CXX_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(H_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HPP_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HH_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HXX_EXT, _("C/C++ header file"), name, _T("4"));

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

void CodeBlocksApp::DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
	wxRegKey key; // defaults to HKCR

	key.SetName(_T("HKEY_CLASSES_ROOT\\.") + ext);
	key.Create();
	key = _T("CodeBlocks.") + ext;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext);
	key.Create();
	key = descr;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\DefaultIcon"));
	key.Create();
	key = exe + _T(",") + icoNum;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\command"));
	key.Create();
	key = _T("\"") + exe + _T("\" \"%1\"");

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec"));
	key.Create();
	key = _T("[Open(\"%1\")]");

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\Application"));
	key.Create();
	key = DDE_SERVICE;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\topic"));
	key.Create();
	key = DDE_TOPIC;

	if(ext.IsSameAs("cbp") || ext.IsSameAs("workspace"))
	{
		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Build\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" --build /na /nd /ns \"%1\"");

		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Rebuild (clean)\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" --rebuild /na /nd /ns \"%1\"");
	}
}

// sets file associations if not there (depending on user prefs)
void CodeBlocksApp::CheckAssociations()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	if (!DoCheckAssociation(CODEBLOCKS_EXT, wxString(APP_NAME) + _(" project file"), name, _T("1")) ||
        !DoCheckAssociation(WORKSPACE_EXT, wxString(APP_NAME) + _(" workspace file"), name, _T("1")) ||
        !DoCheckAssociation(C_EXT, _T("C source file"), name, _T("2")) ||
        !DoCheckAssociation(CPP_EXT, _T("C++ source file"), name, _T("3")) ||
        !DoCheckAssociation(CC_EXT, _T("C++ source file"), name, _T("3")) ||
        !DoCheckAssociation(CXX_EXT, _T("C++ source file"), name, _T("3")) ||
        !DoCheckAssociation(H_EXT, _T("C/C++ header file"), name, _T("4")) ||
        !DoCheckAssociation(HPP_EXT, _T("C/C++ header file"), name, _T("4")) ||
        !DoCheckAssociation(HH_EXT, _T("C/C++ header file"), name, _T("4")) ||
        !DoCheckAssociation(HXX_EXT, _T("C/C++ header file"), name, _T("4")))
    {
        HideSplashScreen();
        wxString msg;
        msg.Printf(_("%s is not currently the default application for C/C++ source files\nDo you want to set it as default?"), APP_NAME);
        int answer = wxMessageBox(msg,
                                    _("File associations"),
                                    wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT);
        if (answer == wxYES)
            SetAssociations();
        else
        {
            wxMessageBox(_("File associations will *not* be checked from now on, on program startup.\n"
                           "If you want to enable the check, go to \"Settings/Environment\" and check \"Check & set file associations\"..."),
                         _("Information"),
                         wxICON_INFORMATION);
            Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/check_associations"), false);
        }
    }
}

// returns true if association is already established
bool CodeBlocksApp::DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
    wxLogNull no_log_here;
	wxRegKey key; // defaults to HKCR
	wxString strVal;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\.")) + ext);
	if (!key.Open())
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext);
	if (!key.Open())
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\DefaultIcon"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("%s,%s"), exe.c_str(), icoNum.c_str()))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\command"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("\"%s\" \"%%1\""), exe.c_str()))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != _T("[Open(\"%1\")]"))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec\\application"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_SERVICE)
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec\\topic"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_TOPIC)
        return false;

    return true;
}

//// DDE

wxConnectionBase* DDEServer::OnAcceptConnection(const wxString& topic)
{
    if (topic == DDE_TOPIC)
        return new DDEConnection(m_Frame);

    // unknown topic
    return 0L;
}

bool DDEConnection::OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format)
{
	wxString strData = data;
	if (!strData.StartsWith(_T("[Open(\"")))
		return false;

	wxRegEx reCmd(_T("\"(.*)\""));
	if (reCmd.Matches(strData))
	{
		wxString file = reCmd.GetMatch(strData, 1);
		if(m_Frame)
            m_Frame->Open(file, false); // don't add to history, files that open through DDE
	}
    return true;
}
#endif

// event handlers

void CodeBlocksApp::OnAppActivate(wxActivateEvent& event)
{
	if (!event.GetActive())
		return;
    if (!Manager::Get())
        return;

    if (Manager::Get()->GetEditorManager() && Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/check_modified_files"), true))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idEditorManagerCheckFiles);
        wxPostEvent(Manager::Get()->GetEditorManager(), evt);
//        Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
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
