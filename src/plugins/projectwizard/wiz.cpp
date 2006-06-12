/***************************************************************
 * Name:      wiz.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    <>
 * Copyright: (c)
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "wiz.h"
#endif

#include "wiz.h"
#include <wx/dir.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/wizard.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>

#include <globals.h>
#include <cbexception.h>
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <scriptingmanager.h>
#include <scriptingcall.h>
#include <compilerfactory.h>
#include <projectbuildtarget.h>
#include <filefilters.h>
#include <licenses.h> // defines some common licenses (like the GPL)

#include "wizpage.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(Wizards);

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(Wiz, "Generic wizard");

Wiz::Wiz()
    : m_pWizard(0),
    m_pWizProjectPathPanel(0),
    m_pWizCompilerPanel(0),
    m_pWizLanguagePanel(0)
{
	//ctor
	m_PluginInfo.name = _T("Wizard");
	m_PluginInfo.title = _("Generic wizard");
	m_PluginInfo.version = _T("0.1a");
	m_PluginInfo.description = _("A generic scripted wizard");
	m_PluginInfo.author = _T("");
	m_PluginInfo.authorEmail = _T("");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _("");
	m_PluginInfo.license = LICENSE_GPL;

	m_TemplatePath = ConfigManager::GetDataFolder() + _T("/templates/wizard/");
}

Wiz::~Wiz()
{
	//dtor
}

void Wiz::OnAttach()
{
    // read configuration
    RegisterWizard();

    // run main wizard script
    // this registers all available wizard scripts with us
    Manager::Get()->GetScriptingManager()->LoadScript(m_TemplatePath + _T("config.script"), _T("WizardMainModule"));
    Manager::Get()->GetScriptingManager()->Compile(_T("WizardMainModule"), true);
    Manager::Get()->GetScriptingManager()->GetEngine()->Discard("WizardModule");
}

int Wiz::GetCount() const
{
    // return the number of template wizards contained in this plugin
    return m_Wizards.GetCount();
}

wxString Wiz::GetTitle(int index) const
{
	//return this wizard's title
	//this will appear in the new-project dialog
	//make sure you set this!
	cbAssert(index >= 0 && index < GetCount());
	return m_Wizards[index].title;
}

wxString Wiz::GetDescription(int index) const
{
	//return this wizard's description
	//make sure you set this!
	cbAssert(index >= 0 && index < GetCount());
	return m_PluginInfo.description;
}

wxString Wiz::GetCategory(int index) const
{
	//return this wizard's category
	//try to match an existing category
	//make sure you change this!
	cbAssert(index >= 0 && index < GetCount());
	return m_Wizards[index].cat;
}

const wxBitmap& Wiz::GetBitmap(int index) const
{
	//return this wizard's bitmap
	//this will appear in the new-project dialog
	cbAssert(index >= 0 && index < GetCount());
	return m_Wizards[index].templatePNG;
}

void Wiz::Clear()
{
    if (m_pWizard)
        m_pWizard->Destroy();
    m_pWizard = 0;
    m_Pages.Clear();

// if the ABI is not sufficient, we 're in trouble the next time the wizard runs...
#if wxABI_VERSION > 20601
    if (!m_LastXRC.IsEmpty())
        wxXmlResource::Get()->Unload(m_LastXRC);
#endif

	m_pWizProjectPathPanel = 0;
	m_pWizCompilerPanel = 0;
	m_pWizLanguagePanel = 0;
}

int Wiz::Launch(int index)
{
	cbAssert(index >= 0 && index < GetCount());
	m_LaunchIndex = index;

    wxString script = m_TemplatePath + m_Wizards[index].script;
    wxString commons = m_TemplatePath + _T("common_functions.script");

	m_LastXRC = m_TemplatePath + m_Wizards[index].xrc;
	if (wxFileExists(m_LastXRC))
        wxXmlResource::Get()->Load(m_LastXRC);
    else
        m_LastXRC.Clear();

	// create wizard
    m_pWizard = new wxWizard;
	m_pWizard->Create(Manager::Get()->GetAppWindow(), wxID_ANY,
                    m_Wizards[index].title,
                    m_Wizards[index].wizardPNG,
                    wxDefaultPosition,
                    wxDEFAULT_DIALOG_STYLE);

    // access the scripting engine core
    asIScriptEngine* engine = Manager::Get()->GetScriptingManager()->GetEngine();

    engine->Discard("WizardModule");
    if (!Manager::Get()->GetScriptingManager()->LoadScript(commons, _T("WizardModule")) || // load common functions
        !Manager::Get()->GetScriptingManager()->LoadScript(script, _T("WizardModule")) || // load selected script
        Manager::Get()->GetScriptingManager()->Compile(_T("WizardModule"), false) != 0) // build and run scripts
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        return -1;
    }

    // get the IDs of all the mandatory functions
    int funcBeginWizard = Manager::Get()->GetScriptingManager()->FindFunctionByDeclaration(_T("void BeginWizard()"), _T("WizardModule"));
    int funcSetupPrj = Manager::Get()->GetScriptingManager()->FindFunctionByDeclaration(_T("bool SetupProject(Project@)"), _T("WizardModule"));
    int funcGetFiles = Manager::Get()->GetScriptingManager()->FindFunctionByDeclaration(_T("bool GetFilesDir(wxString& out)"), _T("WizardModule"));
    if (funcBeginWizard <= 0 ||
        funcSetupPrj <= 0 ||
        funcGetFiles <= 0)
    {
        wxString msg = _("At least one of the following mandatory script functions is missing or didn't compile:\n\n");
        cbMessageBox(msg +
                    _T(
                        "void BeginWizard()\n"
                        "bool SetupProject(Project@)\n"
                        "bool GetFilesDir(wxString& out)\n"
                    ),
                    _("Error"), wxICON_ERROR);
        Clear();
        return -1;
    }

    // call BeginWizard()
    VoidExecutor<> vexec(funcBeginWizard);
    vexec.Call();

    // check if *any* pages were added
    if (m_Pages.GetCount() == 0)
    {
        cbMessageBox(m_Wizards[index].title + _(" has failed to run..."), _("Error"), wxICON_ERROR);
        Clear();
        return -1;
    }

    // check if *mandatory* pages (i.e. used by the following code) were added
    // currently, project path is a mandatory page...
    if (!m_pWizProjectPathPanel)
    {
        cbMessageBox(_("This wizard is missing the following mandatory wizard page:\n\n"
                        "Project path selection\n"
                        "Execution aborted..."), _("Error"), wxICON_ERROR);
        Clear();
        return -1;
    }

    // build the wizard pages
    Finalize();

    // run wizard
    bool success = false;
    if (m_pWizard->RunWizard(m_Pages[0]))
    {
        // ok, wizard done

        // first construct the project filename
        wxString prjname = GetProjectPath() + wxFILE_SEP_PATH +
                            GetProjectName() + wxFILE_SEP_PATH +
                            GetProjectName() + _T('.') + FileFilters::CODEBLOCKS_EXT;

        // create the dir for the project
        wxFileName fname(prjname);
        wxString prjdir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        if (!CreateDirRecursively(prjdir))
        {
            cbMessageBox(_("Couldn't create the project directory:\n") + prjdir, _("Error"), wxICON_ERROR);
            Clear();
            return -1;
        }

        // now create the project
        cbProject* theproject = Manager::Get()->GetProjectManager()->NewProject(prjname);
        if (!theproject)
        {
            cbMessageBox(_("Couldn't create the new project:\n") + prjdir, _("Error"), wxICON_ERROR);
            Clear();
            return -1;
        }

        // set the project title and project-wide compiler
        theproject->SetTitle(GetProjectName());
        theproject->SetCompilerID(GetCompilerID());

        // create the targets
        if (GetWantDebug())
        {
            ProjectBuildTarget* target = theproject->AddBuildTarget(GetDebugName());
            if (target)
            {
                target->SetCompilerID(GetCompilerID());
                target->SetIncludeInTargetAll(false);
//                target->SetOutputFilename(GetDebugOutputDir() + wxFILE_SEP_PATH + GetProjectName() + ext);
                target->SetObjectOutput(GetDebugObjectOutputDir());
            }
        }

        if (GetWantRelease())
        {
            ProjectBuildTarget* target = theproject->AddBuildTarget(GetReleaseName());
            if (target)
            {
                target->SetCompilerID(GetCompilerID());
                target->SetIncludeInTargetAll(false);
//                target->SetOutputFilename(GetReleaseOutputDir() + wxFILE_SEP_PATH + GetProjectName() + ext);
                target->SetObjectOutput(GetReleaseObjectOutputDir());
            }
        }

        // ask the script to setup the new project (edit targets, setup options, etc)
        // call SetupProject()
        Executor<bool, cbProject*> execSetupPrj(funcSetupPrj);
        success = execSetupPrj.Call(theproject);
        if (!success)
        {
            cbMessageBox(wxString::Format(_("Couldn't setup project options:\n%s\n\nScript error:\n%s"),
                                        prjdir.c_str(),
                                        execSetupPrj.CreateErrorString().c_str()),
                        _("Error"), wxICON_ERROR);
            Clear();
            return -1;
        }

        // add all the template files
        // first get the dirs with the files by calling GetFilesDir()
        wxString srcdir;
        Executor<bool, wxString&> execFiles(funcGetFiles);
        if (execFiles.Call(srcdir))
        {
            // now break them up (remember: semicolon-separated list of dirs)
            wxArrayString tmpsrcdirs = GetArrayFromString(srcdir, _T(";"), true);
            // and copy files from each source dir we got
            for (size_t i = 0; i < tmpsrcdirs.GetCount(); ++i)
                CopyFiles(theproject, prjdir, tmpsrcdirs[i]);
        }

        if (srcdir.IsEmpty())
            cbMessageBox(_("The wizard didn't provide any files to copy!"), _("Warning"), wxICON_WARNING);

        // save the project and...
        theproject->Save();

        // finally, make sure everything looks ok
        Manager::Get()->GetProjectManager()->RebuildTree();
        Manager::Get()->GetProjectManager()->GetTree()->Expand(theproject->GetProjectNode());
        success = true;
    }
    Clear();
    return success ? 0 : -1;
}

void Wiz::CopyFiles(cbProject* theproject, const wxString&  prjdir, const wxString& srcdir)
{
    // first get the dir with the files
    wxArrayString filesList;
    wxString enumdirs = GetTemplatePath() + srcdir;
    wxString basepath = wxFileName(enumdirs).GetFullPath();

    // recursively enumerate all files under srcdir
    wxDir::GetAllFiles(enumdirs, &filesList);

    // now get each file and copy it to the destination directory,
    // adding it to all targets in the project
    for (unsigned int i = 0; i < filesList.GetCount(); ++i)
    {
        wxString srcfile = filesList[i];

        wxString dstfile = srcfile;
        // fixup destination filename (remove srcdir from path)
        dstfile.Replace(basepath, prjdir);

        // make sure the destination directory exists
        wxFileName fname(dstfile);
        wxString dstdir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        CreateDirRecursively(dstdir);

        // copy the file
        wxCopyFile(srcfile, dstfile, true);

        // and add it to the project
        fname.MakeRelativeTo(prjdir);
        ProjectFile* pf = theproject->AddFile(0, fname.GetFullPath());
        // to all targets...
        for (int x = 1; x < theproject->GetBuildTargetsCount(); ++x)
        {
            pf->AddBuildTarget(theproject->GetBuildTarget(x)->GetTitle());
        }
    }
}

////////////////////////
// Scripting - BEGIN
////////////////////////

void Wiz::FillComboboxWithCompilers(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win && win->GetCount() == 0)
        {
            for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
            {
                win->Append(CompilerFactory::GetCompiler(i)->GetName());
            }
            win->SetSelection(win->FindString(CompilerFactory::GetDefaultCompiler()->GetName()));
        }
    }
}

void Wiz::SetComboboxSelection(const wxString& name, int sel)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win)
            win->SetSelection(sel);
    }
}

wxString Wiz::GetCompilerFromCombobox(const wxString& name)
{
    int id = GetComboboxSelection(name);
    Compiler* compiler = CompilerFactory::GetCompiler(id);
    if (compiler)
        return compiler->GetID();
    return wxEmptyString;
}

wxString Wiz::GetComboboxStringSelection(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetStringSelection();
    }
    return wxEmptyString;
}

int Wiz::GetComboboxSelection(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetSelection();
    }
    return -1;
}

int Wiz::GetRadioboxSelection(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxRadioBox* win = dynamic_cast<wxRadioBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetSelection();
    }
    return -1;
}

void Wiz::SetRadioboxSelection(const wxString& name, int sel)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxRadioBox* win = dynamic_cast<wxRadioBox*>(page->FindWindowByName(name, page));
        if (win)
            win->SetSelection(sel);
    }
}

void Wiz::CheckCheckbox(const wxString& name, bool check)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckBox* win = dynamic_cast<wxCheckBox*>(page->FindWindowByName(name, page));
        if (win)
            win->SetValue(check);
    }
}

bool Wiz::IsCheckboxChecked(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckBox* win = dynamic_cast<wxCheckBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->IsChecked();
    }
    return false;
}

void Wiz::SetTextControlValue(const wxString& name, const wxString& value)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxTextCtrl* win = dynamic_cast<wxTextCtrl*>(page->FindWindowByName(name, page));
        if (win)
            win->SetValue(value);
    }
}

wxString Wiz::GetTextControlValue(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxTextCtrl* win = dynamic_cast<wxTextCtrl*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetValue();
    }
    return wxEmptyString;
}

void Wiz::AddIntroPage(const wxString& intro_msg)
{
    WizIntroPanel* page = new WizIntroPanel(intro_msg, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(page);
}

void Wiz::AddProjectPathPage()
{
    if (m_pWizProjectPathPanel)
        return; // already added
    m_pWizProjectPathPanel = new WizProjectPathPanel(m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(m_pWizProjectPathPanel);
}

void Wiz::AddCompilerPage(const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange, bool allowConfigChange)
{
    if (m_pWizCompilerPanel)
        return; // already added
    m_pWizCompilerPanel = new WizCompilerPanel(compilerID, validCompilerIDs, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG, allowCompilerChange, allowConfigChange);
    m_Pages.Add(m_pWizCompilerPanel);
}

void Wiz::AddLanguagePage(const wxString& langs, int defLang)
{
    if (m_pWizLanguagePanel)
        return; // already added
    m_pWizLanguagePanel = new WizLanguagePanel(GetArrayFromString(langs, _T(";")), defLang, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(m_pWizLanguagePanel);
}

void Wiz::AddPage(const wxString& panelName)
{
    WizPage* page = new WizPage(panelName, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(page);
}

void Wiz::Finalize()
{
    // chain pages
    for (size_t i = 1; i < m_Pages.GetCount(); ++i)
        wxWizardPageSimple::Chain(m_Pages[i - 1], m_Pages[i]);

    // allow the wizard to size itself around the pages
    for (size_t i = 1; i < m_Pages.GetCount(); ++i)
        m_pWizard->GetPageAreaSizer()->Add(m_Pages[i]);
}

void Wiz::AddWizard(const wxString& title,
                    const wxString& cat,
                    const wxString& script,
                    const wxString& templatePNG,
                    const wxString& wizardPNG,
                    const wxString& xrc)
{
    WizardInfo info;
    info.title = title;
    info.cat = cat;
    info.script = script;
    info.templatePNG.LoadFile(m_TemplatePath + templatePNG, wxBITMAP_TYPE_PNG);
    info.wizardPNG.LoadFile(m_TemplatePath + wizardPNG, wxBITMAP_TYPE_PNG);
    info.xrc = xrc;
    m_Wizards.Add(info);
    Manager::Get()->GetMessageManager()->DebugLog(_T("Project wizard added for '%s'"), title.c_str());
}

wxString Wiz::GetProjectPath()
{
    if (m_pWizProjectPathPanel)
        return m_pWizProjectPathPanel->GetPath();
    return wxEmptyString;
}

wxString Wiz::GetProjectName()
{
    if (m_pWizProjectPathPanel)
        return m_pWizProjectPathPanel->GetName();
    return wxEmptyString;
}

wxString Wiz::GetCompilerID()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetCompilerID();
    return wxEmptyString;
}

bool Wiz::GetWantDebug()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetWantDebug();
    return false;
}

wxString Wiz::GetDebugName()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugName();
    return wxEmptyString;
}

wxString Wiz::GetDebugOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugOutputDir();
    return wxEmptyString;
}

wxString Wiz::GetDebugObjectOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugObjectOutputDir();
    return wxEmptyString;
}

bool Wiz::GetWantRelease()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetWantRelease();
    return false;
}

wxString Wiz::GetReleaseName()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseName();
    return wxEmptyString;
}

wxString Wiz::GetReleaseOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseOutputDir();
    return wxEmptyString;
}

wxString Wiz::GetReleaseObjectOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseObjectOutputDir();
    return wxEmptyString;
}

int Wiz::GetLanguageIndex() const
{
    if (m_pWizLanguagePanel)
        return m_pWizLanguagePanel->GetLanguage();
    return -1;
}

void Wiz::RegisterWizard()
{
    asIScriptEngine* engine = Manager::Get()->GetScriptingManager()->GetEngine();
    engine->RegisterObjectType("Wiz", 0, asOBJ_CLASS);

    engine->RegisterObjectMethod("Wiz", "void AddWizard(const wxString& in,const wxString& in,const wxString& in,const wxString& in,const wxString& in,const wxString& in)", asMETHOD(Wiz, AddWizard), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "void AddIntroPage(const wxString& in)", asMETHOD(Wiz, AddIntroPage), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void AddProjectPathPage()", asMETHOD(Wiz, AddProjectPathPage), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void AddCompilerPage(const wxString& in, const wxString& in, bool, bool)", asMETHOD(Wiz, AddCompilerPage), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void AddLanguagePage(const wxString& in, int)", asMETHOD(Wiz, AddLanguagePage), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void AddPage(const wxString& in)", asMETHOD(Wiz, AddPage), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "void SetTextControlValue(const wxString& in, const wxString& in)", asMETHOD(Wiz, SetTextControlValue), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetTextControlValue(const wxString& in)", asMETHOD(Wiz, GetTextControlValue), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "void CheckCheckbox(const wxString& in, bool)", asMETHOD(Wiz, CheckCheckbox), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "bool IsCheckboxChecked(const wxString& in)", asMETHOD(Wiz, IsCheckboxChecked), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void FillComboboxWithCompilers(const wxString& in)", asMETHOD(Wiz, FillComboboxWithCompilers), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetCompilerFromCombobox(const wxString& in)", asMETHOD(Wiz, GetCompilerFromCombobox), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetComboboxStringSelection(const wxString& in)", asMETHOD(Wiz, GetComboboxStringSelection), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "int GetComboboxSelection(const wxString& in)", asMETHOD(Wiz, GetComboboxSelection), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void SetComboboxSelection(const wxString& in,int)", asMETHOD(Wiz, SetComboboxSelection), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "int GetRadioboxSelection(const wxString& in)", asMETHOD(Wiz, GetRadioboxSelection), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "void SetRadioboxSelection(const wxString& in,int)", asMETHOD(Wiz, SetRadioboxSelection), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "wxString& GetTemplatePath()", asMETHOD(Wiz, GetTemplatePath), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetProjectPath()", asMETHOD(Wiz, GetProjectPath), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetProjectName()", asMETHOD(Wiz, GetProjectName), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetCompilerID()", asMETHOD(Wiz, GetCompilerID), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "bool GetWantDebug()", asMETHOD(Wiz, GetWantDebug), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetDebugName()", asMETHOD(Wiz, GetDebugName), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetDebugOutputDir()", asMETHOD(Wiz, GetDebugOutputDir), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetDebugObjectOutputDir()", asMETHOD(Wiz, GetDebugObjectOutputDir), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "bool GetWantRelease()", asMETHOD(Wiz, GetWantRelease), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetReleaseName()", asMETHOD(Wiz, GetReleaseName), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetReleaseOutputDir()", asMETHOD(Wiz, GetReleaseOutputDir), asCALL_THISCALL);
    engine->RegisterObjectMethod("Wiz", "wxString GetReleaseObjectOutputDir()", asMETHOD(Wiz, GetReleaseObjectOutputDir), asCALL_THISCALL);

    engine->RegisterObjectMethod("Wiz", "int GetLanguageIndex()", asMETHOD(Wiz, GetLanguageIndex), asCALL_THISCALL);

    engine->RegisterGlobalProperty("Wiz Wizard", this);
}

////////////////////////
// Scripting - END
////////////////////////
