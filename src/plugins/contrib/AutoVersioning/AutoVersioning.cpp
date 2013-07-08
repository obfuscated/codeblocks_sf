#include <ctime>
#include <cstdlib> //For random numbers
#include <sdk.h>

#ifndef CB_PRECOMP
#include <wx/dynarray.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <cbproject.h>
#include "globals.h"
#include <manager.h>
#include <projectbuildtarget.h>
#include <projectmanager.h>
#include <tinyxml/tinyxml.h>
#endif
#include <tinyxml/tinywxuni.h>
#include <wx/textfile.h>

#include "projectloader_hooks.h"

#include "AutoVersioning.h"
#include "avSvnRevision.h"
#include "avVersionEditorDlg.h"
#include "avChangesDlg.h"
#include "avHeader.h"

using namespace std;

const int idMenuAutoVersioning = wxNewId();
const int idMenuCommitChanges = wxNewId();
const int idMenuChangesLog = wxNewId();

/*
    KILLERBOT : TODO : is this really needed  ??
    EVT_UPDATE_UI(idMenuCommitChanges, AutoVersioning::OnUpdateUI)
    EVT_UPDATE_UI(idMenuAutoVersioning, AutoVersioning::OnUpdateUI)
*/


//{Event Table
BEGIN_EVENT_TABLE(AutoVersioning,wxEvtHandler)
    EVT_TIMER(-1, AutoVersioning::OnTimerVerify)

    EVT_UPDATE_UI(idMenuCommitChanges, AutoVersioning::OnUpdateUI)
    EVT_UPDATE_UI(idMenuAutoVersioning, AutoVersioning::OnUpdateUI)
    EVT_UPDATE_UI(idMenuChangesLog, AutoVersioning::OnUpdateUI)
    EVT_MENU(idMenuAutoVersioning, AutoVersioning::OnMenuAutoVersioning)
    EVT_MENU(idMenuCommitChanges, AutoVersioning::OnMenuCommitChanges)
    EVT_MENU(idMenuChangesLog, AutoVersioning::OnMenuChangesLog)
END_EVENT_TABLE()
//}

namespace
{
    PluginRegistrant<AutoVersioning> reg(_T("AutoVersioning"));
}

//{Constructor and Destructor
AutoVersioning::AutoVersioning()
{
    // hook to project loading procedure
    ProjectLoaderHooks::HookFunctorBase* AutoVerHook =
      new ProjectLoaderHooks::HookFunctor<AutoVersioning>(this, &AutoVersioning::OnProjectLoadingHook);
    m_AutoVerHookId = ProjectLoaderHooks::RegisterHook(AutoVerHook);
    m_Modified = false;
    m_Project = 0;
} // end of constructor

AutoVersioning::~AutoVersioning()
{
    ProjectLoaderHooks::UnregisterHook(m_AutoVerHookId, true);
} // end of destructor
//}

//{Virtual overrides
void AutoVersioning::OnAttach()
{
    if (!IsAttached())
    {
        wxMessageBox(_("Error loading AutoVersioning Plugin!"),_("Error"),wxICON_ERROR );
    }

    m_timerStatus = new wxTimer(this,30000);
    m_timerStatus->Start(1000);

    //Register functions to events
  // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<AutoVersioning, CodeBlocksEvent>(this, &AutoVersioning::OnProjectActivated));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE,    new cbEventFunctor<AutoVersioning, CodeBlocksEvent>(this, &AutoVersioning::OnProjectClosed));
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_STARTED, new cbEventFunctor<AutoVersioning, CodeBlocksEvent>(this, &AutoVersioning::OnCompilerStarted));
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<AutoVersioning, CodeBlocksEvent>(this, &AutoVersioning::OnCompilerFinished));
    //NOTE : all those registered through EventSink don't need to call event.skip() [that is only needed when going through wx]
}

void AutoVersioning::OnRelease(bool /*appShutDown*/)
{
    if (m_timerStatus->IsRunning())
    {
        m_timerStatus->Stop();
    }
    delete m_timerStatus;
    m_timerStatus = 0;
}

void AutoVersioning::BuildMenu(wxMenuBar* menuBar)
{
    int idProject = menuBar->FindMenu(_("&Project"));
    if (idProject != wxNOT_FOUND)
    {
        wxMenu* project = menuBar->GetMenu(idProject);
        project->AppendSeparator();
        project->Append(idMenuAutoVersioning, _("Autoversioning"), _("Manage your project version"));
        project->Append(idMenuCommitChanges, _("Increment version"), _("Increments and update the version info"));
        project->Append(idMenuChangesLog, _("Changes log"), _("View and edit the actual changes"));
    }
}
//}

//{Envent Functions
void AutoVersioning::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading)
{
    if(loading)
    {
        // TODO (KILLERBOT) : should we have default values, in case something would be missing ?
        // OPTI : we could choose not to write out default values in the xml --> smaller cbp
        avConfig Config;
        m_IsVersioned[project] = false; // default not active unless we find xml for it
        const TiXmlElement* Node = elem->FirstChildElement("AutoVersioning");
        if (Node)
        {
            m_IsVersioned[project] = true;
            TiXmlHandle Handle(const_cast<TiXmlElement*>(Node));
            if(const TiXmlElement* pElem = Handle.FirstChildElement("Scheme").ToElement())
            {
                int Help = 0;
                if(pElem->QueryIntAttribute("minor_max", &Help) == TIXML_SUCCESS)
                {
                    Config.Scheme.MinorMax = static_cast<long>(Help);
                }
                if(pElem->QueryIntAttribute("build_max", &Help) == TIXML_SUCCESS)
                {
                    Config.Scheme.BuildMax = static_cast<long>(Help);
                }
                if(pElem->QueryIntAttribute("rev_max", &Help) == TIXML_SUCCESS)
                {
                    Config.Scheme.RevisionMax = static_cast<long>(Help);
                }
                if(pElem->QueryIntAttribute("rev_rand_max", &Help) == TIXML_SUCCESS)
                {
                    Config.Scheme.RevisionRandMax = static_cast<long>(Help);
                }
                if(pElem->QueryIntAttribute("build_times_to_increment_minor", &Help) == TIXML_SUCCESS)
                {
                    Config.Scheme.BuildTimesToIncrementMinor = static_cast<long>(Help);
                }
            }
            if(const TiXmlElement* pElem = Handle.FirstChildElement("Settings").ToElement())
            {
                Config.Settings.Language = pElem->Attribute("language");
                Config.Settings.SvnDirectory = pElem->Attribute("svn_directory");
                Config.Settings.HeaderPath = pElem->Attribute("header_path");

                int Help = 0;
                if(pElem->QueryIntAttribute("autoincrement", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.Autoincrement = Help?true:false;
                }
                if(pElem->QueryIntAttribute("date_declarations", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.DateDeclarations = Help?true:false;
                }
                if(pElem->QueryIntAttribute("use_define", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.UseDefine = Help?true:false;
                }
                // GJH 03/03/10 Added manifest updating.
                if(pElem->QueryIntAttribute("update_manifest", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.UpdateManifest = Help?true:false;
                }
                if(pElem->QueryIntAttribute("do_auto_increment", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.DoAutoIncrement = Help?true:false;
                }
                if(pElem->QueryIntAttribute("ask_to_increment", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.AskToIncrement = Help?true:false;
                }
                if(pElem->QueryIntAttribute("svn", &Help) == TIXML_SUCCESS)
                {
                    Config.Settings.Svn = Help?true:false;
                }
            }
            if(const TiXmlElement* pElem = Handle.FirstChildElement("Code").ToElement())
            {
                Config.Code.HeaderGuard = pElem->Attribute("header_guard");
                Config.Code.NameSpace = pElem->Attribute("namespace");
                Config.Code.Prefix = pElem->Attribute("prefix");
            }
            if(const TiXmlElement* pElem = Handle.FirstChildElement("Changes_Log").ToElement())
            {
                Config.ChangesLog.AppTitle = pElem->Attribute("app_title");
                Config.ChangesLog.ChangesLogPath = pElem->Attribute("changeslog_path");

                int Help = 0;
                if(pElem->QueryIntAttribute("show_changes_editor", &Help) == TIXML_SUCCESS)
                {
                    Config.ChangesLog.ShowChangesEditor = Help?true:false;
                }
            }
            // do not try to read from version.h, if autoversioning is not enabled for the project,
            // to avoid error messages if verbose debuglog is on
            avVersionState VersionState;
            m_versionHeaderPath = FileNormalize(cbC2U(Config.Settings.HeaderPath.c_str()),project->GetBasePath());

            avHeader VersionHeader;
            if(VersionHeader.LoadFile(m_versionHeaderPath))
            {
                VersionState.Values.Major = VersionHeader.GetValue(_("MAJOR"));
                VersionState.Values.Minor = VersionHeader.GetValue(_("MINOR"));
                VersionState.Values.Build = VersionHeader.GetValue(_("BUILD"));
                VersionState.Values.Revision = VersionHeader.GetValue(_("REVISION"));
                VersionState.Values.BuildCount = VersionHeader.GetValue(_("BUILDS_COUNT"));
                VersionState.Status.SoftwareStatus = cbU2C(VersionHeader.GetString(_("STATUS")));
                VersionState.Status.Abbreviation = cbU2C(VersionHeader.GetString(_("STATUS_SHORT")));
                VersionState.BuildHistory = VersionHeader.GetValue(_("BUILD_HISTORY"));
            }
            m_ProjectMap[project] = Config;
            m_ProjectMapVersionState[project] = VersionState;
        }
    }
    else
    {
        // Hook called when saving project file.

        // since rev4332, the project keeps a copy of the <Extensions> element
        // and re-uses it when saving the project (so to avoid losing entries in it
        // if plugins that use that element are not loaded atm).
        // so, instead of blindly inserting the element, we must first check it's
        // not already there (and if it is, clear its contents)
        if(m_IsVersioned[project])
        {
            TiXmlElement* node = elem->FirstChildElement("AutoVersioning");
            if (!node)
            {
                node = elem->InsertEndChild(TiXmlElement("AutoVersioning"))->ToElement();
            }
            node->Clear();

            //Used this instead of GetConfig() since if the project is not activated
            //before saving, then the m_Project is not updated.
            //This will happen when having multiple projects opened.
            avConfig NewConfig = m_ProjectMap[project];

            TiXmlElement Scheme("Scheme");
            Scheme.SetAttribute("minor_max", NewConfig.Scheme.MinorMax);
            Scheme.SetAttribute("build_max", NewConfig.Scheme.BuildMax);
            Scheme.SetAttribute("rev_max", NewConfig.Scheme.RevisionMax);
            Scheme.SetAttribute("rev_rand_max", NewConfig.Scheme.RevisionRandMax);
            Scheme.SetAttribute("build_times_to_increment_minor", NewConfig.Scheme.BuildTimesToIncrementMinor);
            node->InsertEndChild(Scheme);
            TiXmlElement Settings("Settings");
            Settings.SetAttribute("autoincrement", NewConfig.Settings.Autoincrement);
            Settings.SetAttribute("date_declarations", NewConfig.Settings.DateDeclarations);
            Settings.SetAttribute("use_define", NewConfig.Settings.UseDefine);
            // GJH 03/03/10 Added manifest updating.
            Settings.SetAttribute("update_manifest", NewConfig.Settings.UpdateManifest);
            Settings.SetAttribute("do_auto_increment", NewConfig.Settings.DoAutoIncrement);
            Settings.SetAttribute("ask_to_increment", NewConfig.Settings.AskToIncrement);
            Settings.SetAttribute("language", NewConfig.Settings.Language.c_str());
            Settings.SetAttribute("svn", NewConfig.Settings.Svn);
            Settings.SetAttribute("svn_directory", NewConfig.Settings.SvnDirectory.c_str());
            Settings.SetAttribute("header_path", NewConfig.Settings.HeaderPath.c_str());
            node->InsertEndChild(Settings);
            TiXmlElement ChangesLog("Changes_Log");
            ChangesLog.SetAttribute("show_changes_editor", NewConfig.ChangesLog.ShowChangesEditor);
            ChangesLog.SetAttribute("app_title", NewConfig.ChangesLog.AppTitle.c_str());
            ChangesLog.SetAttribute("changeslog_path", NewConfig.ChangesLog.ChangesLogPath.c_str());
            node->InsertEndChild(ChangesLog);
            TiXmlElement Code("Code");
            Code.SetAttribute("header_guard", NewConfig.Code.HeaderGuard.c_str());
            Code.SetAttribute("namespace", NewConfig.Code.NameSpace.c_str());
            Code.SetAttribute("prefix", NewConfig.Code.Prefix.c_str());
            node->InsertEndChild(Code);
        }
        // TODO (KILLERBOT) : what if we decide to not version anymore : how to remove ??
    }
}// OnProjectLoadingHook

void AutoVersioning::OnProjectActivated(CodeBlocksEvent& event)
{
    if (IsAttached())
    {
        // switch to the settings of the now activated project [I assume it has already been loaded before this triggers]
        m_Project = event.GetProject();
    }
}// OnProjectActivated

void AutoVersioning::OnProjectClosed(CodeBlocksEvent& event)
{
    if (IsAttached())
    {
        m_ProjectMap.erase(event.GetProject());
        m_ProjectMapVersionState.erase(event.GetProject());
        m_IsVersioned.erase(event.GetProject());
        if(m_Project == event.GetProject())
        {   // should always be the case (??? we hope ??)
            m_Project = 0;
        }
    }
}// OnProjectClosed

// KILLERBOT : TODO : only do this when active (!!!!!!!!!!!), ie when autoversioning this project
// BIG QUESTION : what will happen on rebuild workspace, will every project being build
// be activated and each has the compilerstarted/Finished ?????
void AutoVersioning::OnCompilerStarted(CodeBlocksEvent& event)
{
    if (m_Project && IsAttached() && m_IsVersioned[event.GetProject()])
    {
        if (m_Modified)
        {
            const bool doAutoIncrement = GetConfig().Settings.DoAutoIncrement;
            const bool askToIncrement = GetConfig().Settings.AskToIncrement;
            if (doAutoIncrement && askToIncrement)
            {
                if (wxMessageBox(_("Do you want to increment the version?"),_T(""),wxYES_NO) == wxYES)
                {
                    CommitChanges();
                }
            }
            else if(doAutoIncrement)
            {
                CommitChanges();
            }
        }
    }
} // end of OnCompilerStarted

void AutoVersioning::OnCompilerFinished(CodeBlocksEvent& event)
{
    if (m_Project && IsAttached() && m_IsVersioned[event.GetProject()])
    {
        ++(GetVersionState().Values.BuildCount);
    }
} // end of OnCompilerFinished

void AutoVersioning::OnTimerVerify(wxTimerEvent& WXUNUSED(event))
{
    if (m_Project && IsAttached() && m_IsVersioned[m_Project])
    {
        if (!m_Modified)
        {
            for (FilesList::iterator it = m_Project->GetFilesList().begin(); it != m_Project->GetFilesList().end(); ++it)
            {
                const ProjectFile* file = *it;
                if (file->GetFileState() == fvsModified)
                {
                    m_Modified = true;
                    break;
                }
            }
        }
    }
} // end of OnTimerVerify

void AutoVersioning::OnMenuAutoVersioning(wxCommandEvent&)
{
    if (IsAttached())
    {
        if (m_Project)
        {
            if (m_IsVersioned[m_Project])
            {
                SetVersionAndSettings(*m_Project, true);
                UpdateVersionHeader();
            }
            else
            {
                if (wxMessageBox(_("Configure the project \"") + m_Project->GetTitle() + _("\" for Autoversioning?"),_("Autoversioning"),wxYES_NO) == wxYES)
                {
                    if(wxFileExists(m_Project->GetBasePath() + _T("version.h")))
                    {
                        wxMessageBox(
                         _T("The header version.h already exist on your projects path. "
                            "The content will be overwritten by the the version info generated code."
                            "\n\nYou can change the default version.h file on the \"Settings\" Tab."
                           ),
                         _T("Warning"),
                         wxICON_EXCLAMATION  | wxOK
                        );
                    }

                    // we activated
                    m_IsVersioned[m_Project] = true;
                    // just becasue we activated the project becomes modified
                    m_Project->SetModified();

                    SetVersionAndSettings(*m_Project);
                    UpdateVersionHeader();

                    wxArrayInt target_array;
                    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
                    {
                        target_array.Add(i);
                    }
                    Manager::Get()->GetProjectManager()->AddFileToProject(m_versionHeaderPath, m_Project, target_array);
                    Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
                    wxMessageBox(_("Project configured!"));
                }
            }
        }
        else
        {
            cbMessageBox(_("No active project!"), _("Error"), wxICON_ERROR | wxOK);
        }
    }
}

void AutoVersioning::OnMenuCommitChanges(wxCommandEvent&)
{
    if (m_Project && IsAttached() && m_IsVersioned[m_Project])
    {
        if(m_Modified)
        {
            CommitChanges();
        }
    }
}

void AutoVersioning::OnMenuChangesLog(wxCommandEvent&)
{
    GenerateChanges();
}

void AutoVersioning::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (IsAttached())
    {
        if (m_Project)
        {
            if(event.GetId() == idMenuAutoVersioning)
            {
                event.Enable(true);
            }
            else if (m_IsVersioned[m_Project])
            {
                if (m_Modified)
                {
                    event.Enable(true);
                }
                else if(event.GetId() == idMenuChangesLog)
                {
                    event.Enable(true);
                }
                else
                {
                    event.Enable(false);
                }
            }
            else
            {
                event.Enable(false);
            }
        }
        else
        {
            event.Enable(false);
        }
    }
}
//}

//{Functions
void AutoVersioning::SetVersionAndSettings(cbProject& Project, bool update)
{
    // KILLERBOT : in case our struct would have a constructor, then no need to have an if/else here
    // --> init in correct place then !!!!!!! TODO
    m_timerStatus->Stop();
    avVersionEditorDlg VersionEditorDialog((wxWindow*) Manager::Get()->GetAppWindow(), 0L);

    VersionEditorDialog.SetCurrentProject(Project.GetTitle());

    VersionEditorDialog.SetMajor(GetVersionState().Values.Major);
    VersionEditorDialog.SetMinor(GetVersionState().Values.Minor);
    VersionEditorDialog.SetBuild(GetVersionState().Values.Build);
    VersionEditorDialog.SetRevision(GetVersionState().Values.Revision);
    VersionEditorDialog.SetCount(GetVersionState().Values.BuildCount);

    VersionEditorDialog.SetAuto(GetConfig().Settings.Autoincrement);
    VersionEditorDialog.SetDates(GetConfig().Settings.DateDeclarations);
    VersionEditorDialog.SetDefine(GetConfig().Settings.UseDefine);
	// GJH 03/03/10 Added manifest updating.
    VersionEditorDialog.SetManifest(GetConfig().Settings.UpdateManifest);

    VersionEditorDialog.SetSvn(GetConfig().Settings.Svn);
    VersionEditorDialog.SetSvnDirectory(cbC2U(GetConfig().Settings.SvnDirectory.c_str()));
    VersionEditorDialog.SetCommit(GetConfig().Settings.DoAutoIncrement);
    VersionEditorDialog.SetCommitAsk(GetConfig().Settings.AskToIncrement);
    VersionEditorDialog.SetLanguage(cbC2U(GetConfig().Settings.Language.c_str()));
    VersionEditorDialog.SetHeaderPath(cbC2U(GetConfig().Settings.HeaderPath.c_str()));

    VersionEditorDialog.SetStatus(cbC2U(GetVersionState().Status.SoftwareStatus.c_str()));
    VersionEditorDialog.SetStatusAbbreviation(cbC2U(GetVersionState().Status.Abbreviation.c_str()));

    VersionEditorDialog.SetMinorMaximum(GetConfig().Scheme.MinorMax);
    VersionEditorDialog.SetBuildMaximum(GetConfig().Scheme.BuildMax);
    VersionEditorDialog.SetRevisionMaximum(GetConfig().Scheme.RevisionMax);
    VersionEditorDialog.SetRevisionRandomMaximum(GetConfig().Scheme.RevisionRandMax);
    VersionEditorDialog.SetBuildTimesToMinorIncrement(GetConfig().Scheme.BuildTimesToIncrementMinor);

    VersionEditorDialog.SetChanges(GetConfig().ChangesLog.ShowChangesEditor);
    VersionEditorDialog.SetChangesTitle(cbC2U(GetConfig().ChangesLog.AppTitle.c_str()));
    VersionEditorDialog.SetChangesLogPath(cbC2U(GetConfig().ChangesLog.ChangesLogPath.c_str()));

    VersionEditorDialog.SetHeaderGuard(cbC2U(GetConfig().Code.HeaderGuard.c_str()));
    VersionEditorDialog.SetNamespace(cbC2U(GetConfig().Code.NameSpace.c_str()));
    VersionEditorDialog.SetPrefix(cbC2U(GetConfig().Code.Prefix.c_str()));

    VersionEditorDialog.ShowModal();
    // allright let's call all the getters
    avConfig OldConfig = GetConfig();
    GetConfig().Scheme.MinorMax = VersionEditorDialog.GetMinorMaximum();
    GetConfig().Scheme.BuildMax = VersionEditorDialog.GetBuildMaximum();
    GetConfig().Scheme.RevisionMax = VersionEditorDialog.GetRevisionMaximum();
    GetConfig().Scheme.RevisionRandMax = VersionEditorDialog.GetRevisionRandomMaximum();
    GetConfig().Scheme.BuildTimesToIncrementMinor = VersionEditorDialog.GetBuildTimesToMinorIncrement();
    GetConfig().Settings.Autoincrement = VersionEditorDialog.GetAuto();
    GetConfig().Settings.DateDeclarations = VersionEditorDialog.GetDates();
    GetConfig().Settings.UseDefine = VersionEditorDialog.GetDefine();
	// GJH 03/03/10 Added manifest updating.
    GetConfig().Settings.UpdateManifest = VersionEditorDialog.GetManifest();
    GetConfig().Settings.AskToIncrement = VersionEditorDialog.GetCommitAsk();
    GetConfig().Settings.DoAutoIncrement = VersionEditorDialog.GetCommit();
    GetConfig().Settings.Language = cbU2C(VersionEditorDialog.GetLanguage());
    GetConfig().Settings.Svn = VersionEditorDialog.GetSvn();
    GetConfig().Settings.SvnDirectory = cbU2C(VersionEditorDialog.GetSvnDirectory());
    GetConfig().Settings.HeaderPath = cbU2C(VersionEditorDialog.GetHeaderPath());
    GetConfig().ChangesLog.ShowChangesEditor = VersionEditorDialog.GetChanges();
    GetConfig().ChangesLog.AppTitle = cbU2C(VersionEditorDialog.GetChangesTitle());
    GetConfig().ChangesLog.ChangesLogPath = cbU2C(VersionEditorDialog.GetChangesLogPath());
    GetConfig().Code.HeaderGuard = cbU2C(VersionEditorDialog.GetHeaderGuard());
    GetConfig().Code.NameSpace = cbU2C(VersionEditorDialog.GetNamespace());
    GetConfig().Code.Prefix = cbU2C(VersionEditorDialog.GetPrefix());

    //Save Header path
    m_versionHeaderPath = cbC2U(GetConfig().Settings.HeaderPath.c_str());

    if(OldConfig != GetConfig())
    {   // settings have changed => Project is to be considered changed
        Project.SetModified(true);
    }
    // let's update the current version state values in case they were adjusted in the gui
    // and when the time is right they are put in the version.h (when is that time ???)
    avVersionState OldState = GetVersionState();
    GetVersionState().Values.Major = VersionEditorDialog.GetMajor();
    GetVersionState().Values.Minor = VersionEditorDialog.GetMinor();
    GetVersionState().Values.Build = VersionEditorDialog.GetBuild();
    GetVersionState().Values.Revision = VersionEditorDialog.GetRevision();
    GetVersionState().Values.BuildCount = VersionEditorDialog.GetCount();

    GetVersionState().Status.SoftwareStatus = cbU2C(VersionEditorDialog.GetStatus());
    GetVersionState().Status.Abbreviation = cbU2C(VersionEditorDialog.GetStatusAbbreviation());

    if (!update)
    {   // first time; we just activated the plug-in on the project; clean start
        GetVersionState().BuildHistory = 0;
        m_Modified = false;
    }
    m_timerStatus->Start(1000);
}

/*
    KILLERBOT : this method will be called when the user has triggered the Autoversioning through it's main menu entry
    just after SetVersionAndSettings() has shown the GUI to enter plugin configuration settings and version state variables
*/
void AutoVersioning::UpdateVersionHeader()
{
    m_timerStatus->Stop();

    //Declares the header guard to be used based on the filename
    wxFileName filename(cbC2U(GetConfig().Settings.HeaderPath.c_str()));
    wxString headerGuard;

    if(cbC2U(GetConfig().Code.HeaderGuard.c_str()) == _T(""))
    {
        headerGuard = filename.GetName() + _T("_") + filename.GetExt();
        headerGuard.Replace(_T(" "), _T("_"), true);
        headerGuard.UpperCase();
    }
    else
    {
        headerGuard = cbC2U(GetConfig().Code.HeaderGuard.c_str());
    }

    wxString prefix = cbC2U(GetConfig().Code.Prefix.c_str());
    wxString def_define_char = _T("");
    wxString def_define_long = _T("");
    wxString def_equal = _T("");
    wxString def_array = _T("");
    wxString def_end = _T("");

    if(prefix != _T(""))
    {
        prefix = prefix + _T("_");
    }

    wxString headerOutput = _T("");
    headerOutput << _T("#ifndef ") << headerGuard << _T("\n");
    headerOutput << _T("#define ") << headerGuard  << _T("\n");
    headerOutput << _T("\n");

    if(cbC2U(GetConfig().Settings.Language.c_str()) == _T("C++"))
    {
        headerOutput << _T("namespace ") << cbC2U(GetConfig().Code.NameSpace.c_str()) << _T("{") << _T("\n");
        headerOutput << _T("\t") << _T("\n");
    }
    if(GetConfig().Settings.UseDefine)
    {
        def_define_char << _T("#define ");
        def_define_long << def_define_char;
        def_equal << _T(" ");
        def_array << _T("");
        def_end << _T("");
    }
    else
    {
        def_define_char << _T("static const char ");
        def_define_long << _T("static const long ");
        def_equal << _T(" = ");
        def_array << _T("[]");
        def_end << _T(";");
    }

    if(GetConfig().Settings.DateDeclarations)
    {
        wxDateTime actualDate = wxDateTime::Now();
        headerOutput << _T("\t") << _T("//Date Version Types") << _T("\n");
        headerOutput << _T("\t") << def_define_char << prefix << _T("DATE") << def_array << def_equal << actualDate.Format(_T("\"%d\"")) << def_end << _T("\n");
        headerOutput << _T("\t") << def_define_char << prefix << _T("MONTH") << def_array << def_equal << actualDate.Format(_T("\"%m\"")) << def_end << _T("\n");
        headerOutput << _T("\t") << def_define_char << prefix << _T("YEAR") << def_array << def_equal << actualDate.Format(_T("\"%Y\"")) << def_end << _T("\n");
        long ubuntuYearNumber = 0;
        actualDate.Format(_T("%y")).ToLong(&ubuntuYearNumber);
        wxString ubuntuYear;
        ubuntuYear.Printf(_T("%ld"),ubuntuYearNumber);
        headerOutput << _T("\t") << def_define_char << prefix << _T("UBUNTU_VERSION_STYLE") << def_array << def_equal << _T(" \"") << ubuntuYear << actualDate.Format(_T(".%m")) << _T("\"") << def_end << _T("\n");
        headerOutput << _T("\t") << _T("\n");
    }

    headerOutput << _T("\t") << _T("//Software Status") << _T("\n");
    headerOutput << _T("\t") << def_define_char << prefix << _T("STATUS") << def_array << def_equal << _T(" \"") << cbC2U(GetVersionState().Status.SoftwareStatus.c_str()) << _T("\"") << def_end << _T("\n");
    headerOutput << _T("\t") << def_define_char << prefix << _T("STATUS_SHORT") << def_array << def_equal << _T(" \"") << cbC2U(GetVersionState().Status.Abbreviation.c_str()) << _T("\"") << def_end << _T("\n");
    headerOutput << _T("\t") << _T("\n");

    wxString myPrintf;
    headerOutput << _T("\t") << _T("//Standard Version Type") << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().Values.Major);
    headerOutput << _T("\t") << def_define_long << prefix << _T("MAJOR ") << def_equal << myPrintf << def_end << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().Values.Minor);
    headerOutput << _T("\t") << def_define_long << prefix << _T("MINOR ") << def_equal << myPrintf << def_end << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().Values.Build);
    headerOutput << _T("\t") << def_define_long << prefix << _T("BUILD ") << def_equal << myPrintf << def_end << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().Values.Revision);
    headerOutput << _T("\t") << def_define_long << prefix << _T("REVISION ") << def_equal << myPrintf << def_end << _T("\n");
    headerOutput << _T("\t") << _T("\n");

    headerOutput << _T("\t") << _T("//Miscellaneous Version Types") << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().Values.BuildCount);
    headerOutput << _T("\t") << def_define_long << prefix << _T("BUILDS_COUNT ") << def_equal << myPrintf << def_end <<_T("\n");

    myPrintf.Printf(_T("%ld,%ld,%ld,%ld"), GetVersionState().Values.Major, GetVersionState().Values.Minor,
            GetVersionState().Values.Build, GetVersionState().Values.Revision);
    headerOutput << _T("\t") << _T("#define ") << prefix << _T("RC_FILEVERSION ") << myPrintf << _T("\n");

    myPrintf.Printf(_T("\"%ld, %ld, %ld, %ld\\0\""), GetVersionState().Values.Major, GetVersionState().Values.Minor,
            GetVersionState().Values.Build, GetVersionState().Values.Revision);
    headerOutput << _T("\t") << _T("#define ") << prefix << _T("RC_FILEVERSION_STRING ") << myPrintf << _T("\n");

    myPrintf.Printf(_T("\"%ld.%ld.%ld.%ld\""), GetVersionState().Values.Major, GetVersionState().Values.Minor,
            GetVersionState().Values.Build, GetVersionState().Values.Revision);
    headerOutput << _T("\t") << def_define_char << prefix << _T("FULLVERSION_STRING ") << def_array << def_equal << myPrintf << def_end << _T("\n");

    if(GetConfig().Settings.Svn)
    {
        wxString revision,date;
        if (!QuerySvn(cbC2U(GetConfig().Settings.SvnDirectory.c_str()), revision, date))
            wxMessageBox(_("Possible Causes:\n-You don't have SVN installed.\n-Incompatible version of SVN.\n-SVN configuration files not found.\n\nVerify the Autoversioning SVN directory."),_("SVN Error"),wxICON_ERROR);
        headerOutput << _T("\t") << _T("\n");
        headerOutput << _T("\t") << _T("//SVN Version") << _T("\n");
        headerOutput << _T("\t") << def_define_char << prefix << _T("SVN_REVISION") << def_array << def_equal << _T("\"") + revision + _T("\"")<< def_end << _T("\n");
        headerOutput << _T("\t") << def_define_char << prefix << _T("SVN_DATE") << def_array << def_equal << _T("\"") + date + _T("\"")<< def_end << _T("\n");
    }

    headerOutput << _T("\t") << _T("\n");
    headerOutput << _T("\t") << _T("//These values are to keep track of your versioning state, don't modify them.") << _T("\n");
    myPrintf.Printf(_T("%ld"), GetVersionState().BuildHistory);
    headerOutput << _T("\t") << def_define_long << prefix << _T("BUILD_HISTORY ") << def_equal << myPrintf << def_end << _T("\n");

    headerOutput << _T("\t") << _T("\n\n");

    if(cbC2U(GetConfig().Settings.Language.c_str()) == _T("C++"))
    {
        headerOutput << _T("}") << _T("\n");
    }

    headerOutput << _T("#endif //") << headerGuard << _T("\n");

    m_versionHeaderPath = FileNormalize(cbC2U(GetConfig().Settings.HeaderPath.c_str()),m_Project->GetBasePath());
    wxFile versionHeaderFile(m_versionHeaderPath, wxFile::write);
    versionHeaderFile.Write(headerOutput);
    versionHeaderFile.Close();

    m_timerStatus->Start(1000);
}

/*! \brief 	Update manifest.xml with the latest version string.
 * \author 	Gary Harris
 * \date		03/03/10
 *
 * \return void
 *
 * This function inserts a new version string into the project's manifest.xml.
 * It searches for the line containing the XML string "<Value version="X.Y.ZZZ" />"
 * and builds and inserts a new version string based on the current AutoVersion
 * values.
 * \note I first used TinyXML to do the XML work but it trashes some text when rewriting the file,
 * in particular text in the "thanks to" field that is on lines following the value field. This is usually
 * done to wrap text onto subsequent lines in the plug-in's About dialogue and is probably not strictly
 * valid XML. TinyXML seems not to like text on following lines.
 */
void AutoVersioning::UpdateManifest()
{
	wxFileName fnManifest(Manager::Get()->GetProjectManager()->GetActiveProject()->GetCommonTopLevelPath() + wxT("manifest.xml"));
	wxString sPathManifest(fnManifest.GetFullPath());
	if (wxFile::Exists(sPathManifest))
	{
		wxTextFile fileManifest(sPathManifest);
		fileManifest.Open();
		if(fileManifest.IsOpened()){
			fileManifest.GetFirstLine();
			wxString sLine;
			size_t i;
			while(!(sLine = fileManifest.GetNextLine()).IsEmpty())
			{
				if(sLine.Find(wxT("<Value version=")) != wxNOT_FOUND)
				{
					i = fileManifest.GetCurrentLine();
					int iFirst, iLast;
					// Find the first double quote.
					iFirst = sLine.Find('"');
					// Find the last double quote.
					iLast = sLine.Find('"', true);
					// Create a new version string...
					wxString sVersion = sLine.SubString(iFirst, iLast);
					wxString sNewVersion = wxString::Format(wxT("\"%ld.%ld.%ld\""), GetVersionState().Values.Major, GetVersionState().Values.Minor, GetVersionState().Values.Build);
					// ...and insert it into the XML.
					sLine.Replace(sVersion, sNewVersion);
					// Remove the existing line and replace it with the new one.
					fileManifest.RemoveLine(i);
					fileManifest.InsertLine(sLine, i);
					fileManifest.Write();
					break;
				}
			}
		}
	}
}

void AutoVersioning::CommitChanges()
{
    if (m_Project && IsAttached() && m_IsVersioned[m_Project])
    {
        if (m_Modified)
        {
            srand((unsigned)time(0));
            GetVersionState().Values.Revision += 1 + rand( ) % GetConfig().Scheme.RevisionRandMax;

            if ((GetConfig().Scheme.RevisionMax != 0) && (GetVersionState().Values.Revision > GetConfig().Scheme.RevisionMax))
            {
                GetVersionState().Values.Revision = 0;
            }

            if (GetConfig().Scheme.BuildMax == 0)
            {
                ++GetVersionState().Values.Build;
            }
            else if(GetVersionState().Values.Build >= GetConfig().Scheme.BuildMax)
            {
                GetVersionState().Values.Build = 0;
            }
            else
            {
                ++GetVersionState().Values.Build;
            }
            if(GetConfig().Settings.Autoincrement)
            {
                ++GetVersionState().BuildHistory;
                if(GetVersionState().BuildHistory >= GetConfig().Scheme.BuildTimesToIncrementMinor)
                {
                    GetVersionState().BuildHistory = 0;
                    ++GetVersionState().Values.Minor;
                }
                if (GetVersionState().Values.Minor > GetConfig().Scheme.MinorMax)
                {
                    GetVersionState().Values.Minor = 0;
                    ++GetVersionState().Values.Major;
                }
            }
            if(GetConfig().ChangesLog.ShowChangesEditor)
            {
                GenerateChanges();
            }
            m_Project->SaveAllFiles();

            m_Modified = false;
            UpdateVersionHeader();
			// GJH 03/03/10 Added manifest updating.
            if(GetConfig().Settings.UpdateManifest)
            {
				UpdateManifest();
            }

        }
    }
}

void AutoVersioning::GenerateChanges()
{
    avChangesDlg changesDlg((wxWindow*) Manager::Get()->GetAppWindow(),0L);
    changesDlg.SetTemporaryChangesFile(m_Project->GetBasePath() + _T("changes.tmp"));
    changesDlg.ShowModal();

    wxString changes = changesDlg.Changes();

    if (!changes.IsEmpty())
    {
        changes.Prepend(_T("        -"));
        changes.Replace(_T("\n"), _T("\n        -"));

        wxDateTime actualDate = wxDateTime::Now();
        wxString changesTitle = cbC2U(GetConfig().ChangesLog.AppTitle.c_str());

        changesTitle.Replace(_T("%d"), actualDate.Format(_T("%d")));
        changesTitle.Replace(_T("%o"), actualDate.Format(_T("%m")));
        changesTitle.Replace(_T("%y"), actualDate.Format(_T("%Y")));

        wxString value;
        value.Printf(_T("%ld"), GetVersionState().Values.Major);
        changesTitle.Replace(_T("%M"), value);

        value.Printf(_T("%ld"), GetVersionState().Values.Minor);
        changesTitle.Replace(_T("%m"), value);

        value.Printf(_T("%ld"), GetVersionState().Values.Build);
        changesTitle.Replace(_T("%b"), value);

        value.Printf(_T("%ld"), GetVersionState().Values.Revision);
        changesTitle.Replace(_T("%r"), value);

        value.Printf(_T("%d"), GetConfig().Settings.Svn?1:0);
        changesTitle.Replace(_T("%s"),value);

        changesTitle.Replace(_T("%T"), cbC2U(GetVersionState().Status.SoftwareStatus.c_str()));
        changesTitle.Replace(_T("%t"), cbC2U(GetVersionState().Status.Abbreviation.c_str()));
        changesTitle.Replace(_T("%p"), m_Project->GetTitle());

        wxString changesFile = FileNormalize(cbC2U(GetConfig().ChangesLog.ChangesLogPath.c_str()), m_Project->GetBasePath());
        wxString changesCurrentContent;
        if (wxFile::Exists(changesFile))
        {
            wxFFile file;

            file.Open(changesFile);
            file.ReadAll(&changesCurrentContent);
            file.Close();
        }

        wxString changesOutput = _T("");

        changesOutput << actualDate.Format(_T("%d %B %Y\n"));
        changesOutput << _T("   ") << changesTitle << _T("\n");
        changesOutput << _T("\n     Change log:\n");
        changesOutput << changes << _T("\n\n");
        changesOutput << changesCurrentContent;

        wxFile file;
        file.Open(changesFile,wxFile::write);
        file.Write(changesOutput);
        file.Close();

    }//If not changes.IsEmpty()
}

wxString AutoVersioning::FileNormalize(const wxString& relativeFile, const wxString& workingDirectory)
{
    wxFileName fileNormalize(relativeFile);
    if(fileNormalize.Normalize(wxPATH_NORM_ABSOLUTE,workingDirectory))
    {
        //If everything is ok returns absolute path
        return fileNormalize.GetFullPath();
    }
    else
    {
        //If something goes wrong return project's cwd and filename with extension.
        return workingDirectory + fileNormalize.GetName() + fileNormalize.GetExt();
    }
}

avConfig& AutoVersioning::GetConfig()
{
    return m_ProjectMap[m_Project];
} // end of GetConfig

avVersionState& AutoVersioning::GetVersionState()
{
    return m_ProjectMapVersionState[m_Project];
} // end of GetVersionState

//}
