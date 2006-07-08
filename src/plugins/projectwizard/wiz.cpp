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
#include <compilerfactory.h>
#include <compiler.h>
#include <projectbuildtarget.h>
#include <filefilters.h>
#include <licenses.h> // defines some common licenses (like the GPL)

#include <scripting/bindings/sc_base_types.h>

#include "wizpage.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(Wizards);

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(Wiz, "Generic wizard");

// scripting support
DECLARE_INSTANCE_TYPE(Wiz);

Wiz::Wiz()
    : m_pWizard(0),
    m_pWizProjectPathPanel(0),
    m_pWizFilePathPanel(0),
    m_pWizCompilerPanel(0),
    m_pWizBuildTargetPanel(0),
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
    // make sure the VM is initialized
    Manager::Get()->GetScriptingManager();

    if (!SquirrelVM::GetVMPtr())
    {
        cbMessageBox(_("Project wizard disabled: scripting not initialized"), _("Error"), wxICON_ERROR);
        return;
    }

    // read configuration
    RegisterWizard();

    // run main wizard script
    // this registers all available wizard scripts with us
    Manager::Get()->GetScriptingManager()->LoadScript(m_TemplatePath + _T("config.script"));
    try
    {
        SqPlus::SquirrelFunction<void> f("RegisterWizards");
        f();
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
    }
}

int Wiz::GetCount() const
{
    // return the number of template wizards contained in this plugin
    return m_Wizards.GetCount();
}

TemplateOutputType Wiz::GetOutputType(int index) const
{
	//return this wizard's output type
	//make sure you set this!
	cbAssert(index >= 0 && index < GetCount());
	return m_Wizards[index].output_type;
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

wxString Wiz::GetScriptFilename(int index) const
{
	//return this wizard's script filename
	cbAssert(index >= 0 && index < GetCount());
	return m_Wizards[index].script;
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
	m_pWizBuildTargetPanel = 0;
	m_pWizLanguagePanel = 0;
	m_pWizFilePathPanel = 0;
}

CompileTargetBase* Wiz::Launch(int index, wxString* pFilename)
{
    // TODO: pFilename is not used

	cbAssert(index >= 0 && index < GetCount());

	// early check: build target wizards need an active project
	if (m_Wizards[index].output_type == totTarget &&
        !Manager::Get()->GetProjectManager()->GetActiveProject())
    {
        cbMessageBox(_("You need to open (or create) a project first!"), _("Error"), wxICON_ERROR);
        return 0;
    }

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

    if (!Manager::Get()->GetScriptingManager()->LoadScript(commons) || // load common functions
        !Manager::Get()->GetScriptingManager()->LoadScript(script)) // build and run script
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        return 0;
    }

    // call BeginWizard()
    try
    {
        SqPlus::SquirrelFunction<void> f("BeginWizard");
        f();
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        Clear();
        return 0;
    }

    // check if *any* pages were added
    if (m_Pages.GetCount() == 0)
    {
        cbMessageBox(m_Wizards[index].title + _(" has failed to run..."), _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

    // check if *mandatory* pages (i.e. used by the following code) were added
    // currently, project path is a mandatory page for new projects...
    if (m_Wizards[index].output_type == totProject && !m_pWizProjectPathPanel)
    {
        cbMessageBox(_("This wizard is missing the following mandatory wizard page:\n\n"
                        "Project path selection\n"
                        "Execution aborted..."), _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

    // build the wizard pages
    Finalize();

    // run wizard
    CompileTargetBase* base = 0; // ret value
    if (m_pWizard->RunWizard(m_Pages[0]))
    {
        // ok, wizard done
        switch (m_Wizards[index].output_type)
        {
            case totProject:     base = RunProjectWizard(pFilename); break;
            case totTarget:      base = RunTargetWizard(pFilename); break;
            case totFiles:       base = RunFilesWizard(pFilename); break;
            case totCustom:      base = RunCustomWizard(pFilename); break;
            default: break;
        }
    }
    Clear();
    return base;
}

CompileTargetBase* Wiz::RunProjectWizard(wxString* pFilename)
{
    cbProject* theproject = 0;

    // first get the project filename
    wxString prjname = GetProjectFullFilename();

    // create the dir for the project
    wxFileName fname(prjname);
    wxString prjdir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!CreateDirRecursively(prjdir))
    {
        cbMessageBox(_("Couldn't create the project directory:\n") + prjdir, _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

    // now create the project
    theproject = Manager::Get()->GetProjectManager()->NewProject(prjname);
    if (!theproject)
    {
        cbMessageBox(_("Couldn't create the new project:\n") + prjdir, _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

    // set the project title and project-wide compiler
    theproject->SetTitle(GetProjectTitle());
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

    // add all the template files
    // first get the dirs with the files by calling GetFilesDir()
    wxString srcdir;
    try
    {
        SqPlus::SquirrelFunction<wxString&> f("GetFilesDir");
        srcdir = f();
        if (!srcdir.IsEmpty())
        {
            // now break them up (remember: semicolon-separated list of dirs)
            wxArrayString tmpsrcdirs = GetArrayFromString(srcdir, _T(";"), true);
            // and copy files from each source dir we got
            for (size_t i = 0; i < tmpsrcdirs.GetCount(); ++i)
                CopyFiles(theproject, prjdir, tmpsrcdirs[i]);
        }
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        Clear();
        return 0;
    }

//    if (srcdir.IsEmpty())
//        cbMessageBox(_("The wizard didn't provide any files to copy!"), _("Warning"), wxICON_WARNING);

    // ask the script to setup the new project (edit targets, setup options, etc)
    // call SetupProject()
    try
    {
        SqPlus::SquirrelFunction<bool> f("SetupProject");
        if (!f(theproject))
        {
            cbMessageBox(wxString::Format(_("Couldn't setup project options:\n%s"),
                                        prjdir.c_str()),
                        _("Error"), wxICON_ERROR);
            Clear();
            return 0;
        }
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        Clear();
        return 0;
    }

    // save the project and...
    theproject->Save();

    if (pFilename)
        *pFilename = theproject->GetFilename();

    // finally, make sure everything looks ok
    Manager::Get()->GetProjectManager()->RebuildTree();
    Manager::Get()->GetProjectManager()->GetTree()->Expand(theproject->GetProjectNode());
    return theproject;
}

CompileTargetBase* Wiz::RunTargetWizard(wxString* pFilename)
{
    cbProject* theproject = Manager::Get()->GetProjectManager()->GetActiveProject(); // can't fail; if no project, the wizard didn't even run
    ProjectBuildTarget* target = theproject->AddBuildTarget(GetTargetName());
    if (!target)
    {
        cbMessageBox(_("Failed to create build target!"), _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

    // setup the target
    target->SetCompilerID(GetTargetCompilerID());
    target->SetIncludeInTargetAll(false);
    target->SetObjectOutput(GetTargetObjectOutputDir());
    target->SetWorkingDir(GetTargetOutputDir());

    // add all the template files (if any)
    // first get the dirs with the files by calling GetFilesDir()
//    wxString srcdir;
//    try
//    {
//        SqPlus::SquirrelFunction<wxString&> f("GetFilesDir");
//        srcdir = f();
//        if (!srcdir.IsEmpty())
//        {
//            // now break them up (remember: semicolon-separated list of dirs)
//            wxArrayString tmpsrcdirs = GetArrayFromString(srcdir, _T(";"), true);
//            // and copy files from each source dir we got
//            for (size_t i = 0; i < tmpsrcdirs.GetCount(); ++i)
//                CopyFiles(theproject, prjdir, tmpsrcdirs[i]);
//        }
//    }
//    catch (SquirrelError& e)
//    {
//        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
//        Clear();
//        return 0;
//    }

    // ask the script to setup the new target (setup options, etc)
    // call SetupTarget()
    try
    {
        SqPlus::SquirrelFunction<bool> f("SetupTarget");
        if (!f(target, GetTargetEnableDebug()))
        {
            cbMessageBox(_("Couldn't setup target options:"), _("Error"), wxICON_ERROR);
            Clear();
            return 0;
        }
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        Clear();
        return 0;
    }

    return target;
}

CompileTargetBase* Wiz::RunFilesWizard(wxString* pFilename)
{
    try
    {
        SqPlus::SquirrelFunction<wxString&> f("CreateFiles");
        wxString files = f();
        if (files.IsEmpty())
            cbMessageBox(_("Wizard failed..."), _("Error"), wxICON_ERROR);
        else
        {
            if (pFilename)
                *pFilename = files.BeforeFirst(_T(';'));
        }
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
    }
    Clear();
    return 0;
}

CompileTargetBase* Wiz::RunCustomWizard(wxString* pFilename)
{
    try
    {
        SqPlus::SquirrelFunction<bool> f("SetupCustom");
        if (!f())
            cbMessageBox(_("Wizard failed..."), _("Error"), wxICON_ERROR);
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
    }
    Clear();
    return 0;
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

TemplateOutputType Wiz::GetWizardType()
{
    cbAssert(m_LaunchIndex >= 0 && m_LaunchIndex < GetCount());
    return m_Wizards[m_LaunchIndex].output_type;
}

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

void Wiz::AddFilePathPage(bool showHeaderGuard)
{
    if (m_pWizFilePathPanel)
        return; // already added
    m_pWizFilePathPanel = new WizFilePathPanel(showHeaderGuard, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(m_pWizFilePathPanel);
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

void Wiz::AddBuildTargetPage(const wxString& targetName, bool isDebug, bool showCompiler, const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange)
{
    if (m_pWizBuildTargetPanel)
        return; // already added
    m_pWizBuildTargetPanel = new WizBuildTargetPanel(targetName, isDebug, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG, showCompiler, compilerID, validCompilerIDs, allowCompilerChange);
    m_Pages.Add(m_pWizBuildTargetPanel);
}

void Wiz::AddLanguagePage(const wxString& langs, int defLang)
{
    if (m_pWizLanguagePanel)
        return; // already added
    m_pWizLanguagePanel = new WizLanguagePanel(GetArrayFromString(langs, _T(";")), defLang, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    m_Pages.Add(m_pWizLanguagePanel);
}

void Wiz::AddGenericSelectPathPage(const wxString& pageId, const wxString& descr, const wxString& label, const wxString& defValue)
{
    // we don't track this; can add more than one
    m_Pages.Add(new WizGenericSelectPathPanel(pageId, descr, label, defValue, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG));
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

    m_pWizard->Fit();
}

void Wiz::AddWizard(TemplateOutputType otype,
                    const wxString& title,
                    const wxString& cat,
                    const wxString& script,
                    const wxString& templatePNG,
                    const wxString& wizardPNG,
                    const wxString& xrc)
{
    WizardInfo info;
    info.output_type = otype;
    info.title = title;
    info.cat = cat;
    info.script = script;
    info.templatePNG.LoadFile(m_TemplatePath + templatePNG, wxBITMAP_TYPE_PNG);
    info.wizardPNG.LoadFile(m_TemplatePath + wizardPNG, wxBITMAP_TYPE_PNG);
    info.xrc = xrc;
    m_Wizards.Add(info);

    wxString typS;
    switch (otype)
    {
        case totProject: typS = _T("Project"); break;
        case totTarget: typS = _T("Build-target"); break;
        case totFiles: typS = _T("File(s)"); break;
        case totCustom: typS = _T("Custom"); break;
        default: break;
    }

    Manager::Get()->GetMessageManager()->DebugLog(typS + _T(" wizard added for '%s'"), title.c_str());
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

wxString Wiz::GetProjectFullFilename()
{
    if (m_pWizProjectPathPanel)
        return m_pWizProjectPathPanel->GetFullFileName();
    return wxEmptyString;
}

wxString Wiz::GetProjectTitle()
{
    if (m_pWizProjectPathPanel)
        return m_pWizProjectPathPanel->GetTitle();
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

wxString Wiz::GetTargetCompilerID()
{
    if (m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetCompilerID();
    return wxEmptyString;
}

bool Wiz::GetTargetEnableDebug()
{
    if (m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetEnableDebug();
    return false;
}

wxString Wiz::GetTargetName()
{
    if (m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetTargetName();
    return wxEmptyString;
}

wxString Wiz::GetTargetOutputDir()
{
    if (m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetTargetOutputDir();
    return wxEmptyString;
}

wxString Wiz::GetTargetObjectOutputDir()
{
    if (m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetTargetObjectOutputDir();
    return wxEmptyString;
}

int Wiz::GetLanguageIndex()
{
    if (m_pWizLanguagePanel)
        return m_pWizLanguagePanel->GetLanguage();
    return -1;
}

wxString Wiz::GetFileName()
{
    if (m_pWizFilePathPanel)
        return m_pWizFilePathPanel->GetFilename();
    return wxEmptyString;
}

wxString Wiz::GetFileHeaderGuard()
{
    if (m_pWizFilePathPanel)
        return m_pWizFilePathPanel->GetHeaderGuard();
    return wxEmptyString;
}

bool Wiz::GetFileAddToProject()
{
    if (m_pWizFilePathPanel)
        return m_pWizFilePathPanel->GetAddToProject();
    return false;
}

int Wiz::GetFileTargetIndex()
{
    if (m_pWizFilePathPanel)
        return m_pWizFilePathPanel->GetTargetIndex();
    return -1;
}

void Wiz::SetFilePathSelectionFilter(const wxString& filter)
{
    if (m_pWizFilePathPanel)
        m_pWizFilePathPanel->SetFilePathSelectionFilter(filter);
}

void Wiz::RegisterWizard()
{
    SqPlus::SQClassDef<Wiz>("Wiz").
            // register new wizards
            func(&Wiz::AddWizard, "AddWizard").
            // add wizard pages
            func(&Wiz::AddIntroPage, "AddIntroPage").
            func(&Wiz::AddProjectPathPage, "AddProjectPathPage").
            func(&Wiz::AddFilePathPage, "AddFilePathPage").
            func(&Wiz::AddCompilerPage, "AddCompilerPage").
            func(&Wiz::AddBuildTargetPage, "AddBuildTargetPage").
            func(&Wiz::AddLanguagePage, "AddLanguagePage").
            func(&Wiz::AddGenericSelectPathPage, "AddGenericSelectPathPage").
            func(&Wiz::AddPage, "AddPage").
            // GUI controls
            func(&Wiz::SetTextControlValue, "SetTextControlValue").
            func(&Wiz::GetTextControlValue, "GetTextControlValue").
            func(&Wiz::CheckCheckbox, "CheckCheckbox").
            func(&Wiz::IsCheckboxChecked, "IsCheckboxChecked").
            func(&Wiz::FillComboboxWithCompilers, "FillComboboxWithCompilers").
            func(&Wiz::GetCompilerFromCombobox, "GetCompilerFromCombobox").
            func(&Wiz::GetComboboxStringSelection, "GetComboboxStringSelection").
            func(&Wiz::GetComboboxSelection, "GetComboboxSelection").
            func(&Wiz::SetComboboxSelection, "SetComboboxSelection").
            func(&Wiz::GetRadioboxSelection, "GetRadioboxSelection").
            func(&Wiz::SetRadioboxSelection, "SetRadioboxSelection").
            // get various common info
            func(&Wiz::GetWizardType, "GetWizardType").
            func(&Wiz::GetTemplatePath, "GetTemplatePath").
            // project path page
            func(&Wiz::GetProjectPath, "GetProjectPath").
            func(&Wiz::GetProjectName, "GetProjectName").
            func(&Wiz::GetProjectFullFilename, "GetProjectFullFilename").
            func(&Wiz::GetProjectTitle, "GetProjectTitle").
            // compiler page
            func(&Wiz::GetCompilerID, "GetCompilerID").
            // + debug target
            func(&Wiz::GetWantDebug, "GetWantDebug").
            func(&Wiz::GetDebugName, "GetDebugName").
            func(&Wiz::GetDebugOutputDir, "GetDebugOutputDir").
            func(&Wiz::GetDebugObjectOutputDir, "GetDebugObjectOutputDir").
            // + release target
            func(&Wiz::GetWantRelease, "GetWantRelease").
            func(&Wiz::GetReleaseName, "GetReleaseName").
            func(&Wiz::GetReleaseOutputDir, "GetReleaseOutputDir").
            func(&Wiz::GetReleaseObjectOutputDir, "GetReleaseObjectOutputDir").
            // build target page
            func(&Wiz::GetTargetCompilerID, "GetTargetCompilerID").
            func(&Wiz::GetTargetEnableDebug, "GetTargetEnableDebug").
            func(&Wiz::GetTargetName, "GetTargetName").
            func(&Wiz::GetTargetOutputDir, "GetTargetOutputDir").
            func(&Wiz::GetTargetObjectOutputDir, "GetTargetObjectOutputDir").
            // language page
            func(&Wiz::GetLanguageIndex, "GetLanguageIndex").
            // file path page
            func(&Wiz::GetFileName, "GetFileName").
            func(&Wiz::GetFileHeaderGuard, "GetFileHeaderGuard").
            func(&Wiz::GetFileAddToProject, "GetFileAddToProject").
            func(&Wiz::GetFileTargetIndex, "GetFileTargetIndex").
            func(&Wiz::SetFilePathSelectionFilter, "SetFilePathSelectionFilter");

    SqPlus::BindVariable(this, "Wizard", SqPlus::VAR_ACCESS_READ_ONLY);
}

////////////////////////
// Scripting - END
////////////////////////
