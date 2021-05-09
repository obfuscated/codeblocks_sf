/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/checklst.h>
    #include <wx/combobox.h>
    #include <wx/dir.h>
    #include <wx/image.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/panel.h>
    #include <wx/radiobox.h>
    #include <wx/sizer.h>
    #include <wx/spinctrl.h>
    #include <wx/stattext.h>
    #include <wx/wizard.h>
    #include <wx/xrc/xmlres.h>

    #include <wx/wxscintilla.h> // CB Header
    #include <cbexception.h>
    #include <cbproject.h>
    #include <compiler.h>
    #include <compilerfactory.h>
    #include <configmanager.h>
    #include <filefilters.h>
    #include <globals.h>
    #include <infowindow.h>
    #include "logmanager.h"
    #include <manager.h>
    #include <projectbuildtarget.h>
    #include <projectmanager.h>
    #include <scriptingmanager.h>
#endif // CB_PRECOMP

#include "wiz.h"
#include "wizpage.h"

#include "scripting/bindings/sc_utils.h"
#include "scripting/bindings/sc_typeinfo_all.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(Wizards); // TODO: find out why this causes a shadow warning for 'Item'

namespace
{
    PluginRegistrant<Wiz> reg(_T("ScriptedWizard"));
}

namespace ScriptBindings
{

template<>
struct TypeInfo<Wiz> {
    // FIXME (squirrel) Add a system for this
    static const uint32_t typetag = uint32_t(TypeTag::UserVariableManager) + 100;
    static constexpr const SQChar *className = _SC("Wiz");
    using baseClass = void;
};
} // namespace ScriptBindings

Wiz::Wiz()
    : m_pWizard(nullptr),
    m_pWizProjectPathPanel(nullptr),
    m_pWizFilePathPanel(nullptr),
    m_pWizCompilerPanel(nullptr),
    m_pWizBuildTargetPanel(nullptr),
    m_LaunchIndex(0)
{
    //ctor
}

Wiz::~Wiz()
{
    //dtor
}

void Wiz::OnAttach()
{
    // make sure the VM is initialized
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();
    HSQUIRRELVM vm = scriptMgr->GetVM();
    if (!vm)
    {
        cbMessageBox(_("Project wizard disabled: scripting not initialized"), _("Error"),
                     wxICON_ERROR);
        return;
    }

    // read configuration
    RegisterWizard(vm);

    // run main wizard script
    // this registers all available wizard scripts with us

    // user script first
    wxString templatePath = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/");
    wxString script = templatePath + _T("/config.script");
    if (!wxFileExists(script))
    {
        // global script next
        templatePath = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/");
        script = templatePath + _T("/config.script");
        if (!wxFileExists(script))
            script = wxString();
    }

    if (!script.empty())
    {
        if (scriptMgr->LoadScript(script))
        {
            ScriptBindings::Caller caller(vm);
            if (!caller.CallByName0(_SC("RegisterWizards")))
                scriptMgr->DisplayErrors(true); // FIXME (squirrel) This error is not displayed properly!
        }
        else
            scriptMgr->DisplayErrors(true);
    }

    // default compiler settings (returned if no compiler page is added in the wizard)
    wxString sep = wxString(wxFILE_SEP_PATH);
    m_DefCompilerID = CompilerFactory::GetDefaultCompilerID();
    m_WantDebug = true;
    m_DebugName = _T("Debug");
    m_DebugOutputDir = _T("bin") + sep + _T("Debug") + sep;
    m_DebugObjOutputDir = _T("obj") + sep + _T("Debug") + sep;
    m_WantRelease = true;
    m_ReleaseName = _T("Release");
    m_ReleaseOutputDir = _T("bin") + sep + _T("Release") + sep;
    m_ReleaseObjOutputDir = _T("obj") + sep + _T("Release") + sep;
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
    return _("A generic scripted wizard");
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
    //return this wizard's script relative filename
    cbAssert(index >= 0 && index < GetCount());
    return m_Wizards[index].script;
}

void Wiz::Clear()
{
    if (m_pWizard)
        m_pWizard->Destroy();
    m_pWizard = nullptr;
    m_Pages.Clear();

// if the ABI is not sufficient, we 're in trouble the next time the wizard runs...
#if wxABI_VERSION > 20601
    if (!m_LastXRC.IsEmpty())
        wxXmlResource::Get()->Unload(m_LastXRC);
#endif

    m_pWizProjectPathPanel = nullptr;
    m_pWizCompilerPanel = nullptr;
    m_pWizBuildTargetPanel = nullptr;
    m_pWizFilePathPanel = nullptr;
}

CompileTargetBase* Wiz::Launch(int index, wxString* pFilename)
{
    cbAssert(index >= 0 && index < GetCount());

    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();

    // clear previous script's context
    static const wxString clearout_wizscripts =  _T("function BeginWizard(){};\n"
                                                    "function SetupProject(project){return false;};\n"
                                                    "function SetupTarget(target,is_debug){return false;};\n"
                                                    "function SetupCustom(){return false;};\n"
                                                    "function CreateFiles(){return _T(\"\");};\n"
                                                    "function GetFilesDir(){return _T(\"\");};\n"
                                                    "function GetGeneratedFile(index){return _T(\"\");};\n"
                                                    "function GetTargetName() { return _T(\"\"); }\n");
    scriptMgr->LoadBuffer(clearout_wizscripts, _T("ClearWizState"));

    // early check: build target wizards need an active project
    if (m_Wizards[index].output_type == totTarget &&
        !Manager::Get()->GetProjectManager()->GetActiveProject())
    {
        cbMessageBox(_("You need to open (or create) a project first!"), _("Error"), wxICON_ERROR);
        return nullptr;
    }

    m_LaunchIndex = index;

    m_LastXRC = m_Wizards[index].xrc;
    if (wxFileExists(m_LastXRC))
    {
        if (!wxXmlResource::Get()->Load(m_LastXRC))
        {
            wxString message = wxString::Format(_("%s has failed to load XRC resource from '%s'"),
                                                m_Wizards[index].title.wx_str(),
                                                m_LastXRC.wx_str());
            cbMessageBox(message, _("Error"), wxICON_ERROR);
        }
    }
    else
        m_LastXRC.Clear();

    // create wizard
    m_pWizard = new wxWizard;

    m_pWizard->Create(Manager::Get()->GetAppWindow(), wxID_ANY, m_Wizards[index].title,
                      m_Wizards[index].wizardPNG, wxDefaultPosition,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    const wxString global_commons = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/common_functions.script");
    const wxString user_commons = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/common_functions.script");
    if (!scriptMgr->LoadScript(global_commons) && // load global common functions
        !scriptMgr->LoadScript(user_commons)) // and/or load user common functions
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        InfoWindow::Display(_("Error"), _("Failed to load the common functions script.\nPlease check the debug log for details..."));
        return nullptr;
    }

    // locate the script
    wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + m_Wizards[index].script;
    if (!wxFileExists(script))
        script = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + m_Wizards[index].script;

    if (!scriptMgr->LoadScript(script)) // build and run script
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        InfoWindow::Display(_("Error"), _("Failed to load the wizard's script.\nPlease check the debug log for details..."));
        return nullptr;
    }

    // Set wizard folder name for GetWizardScriptFolder() calls
    m_WizardScriptFolder = wxEmptyString;
    wxArrayString scriptDirs = wxFileName(script).GetDirs();
    if (scriptDirs.GetCount())
        m_WizardScriptFolder = scriptDirs[scriptDirs.GetCount()-1];

    try
    {
        ScriptBindings::Caller caller(scriptMgr->GetVM());
        if (!caller.CallByName0(_SC("BeginWizard")))
        {
            scriptMgr->DisplayErrors(true);
            Clear();
            return nullptr;
        }
    }
    catch (cbException& e)
    {
        e.ShowErrorMessage(false);
        Clear();
        return nullptr;
    }

    // check if *any* pages were added
    if (m_Pages.GetCount() == 0)
    {
        cbMessageBox(m_Wizards[index].title + _(" has failed to run..."), _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
    }

    // check if *mandatory* pages (i.e. used by the following code) were added
    // currently, project path is a mandatory page for new projects...
    if (m_Wizards[index].output_type == totProject && !m_pWizProjectPathPanel)
    {
        cbMessageBox(_("This wizard is missing the following mandatory wizard page:\n\n"
                        "Project path selection\n"
                        "Execution aborted..."), _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
    }

    // build the wizard pages
    Finalize();

    // run wizard
    CompileTargetBase* base = nullptr; // ret value
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
    cbProject* theproject = nullptr;

    // first get the project filename
    wxString prjname = GetProjectFullFilename();

    // create the dir for the project
    wxFileName fname(prjname);
    wxString prjdir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!CreateDirRecursively(prjdir))
    {
        cbMessageBox(_("Couldn't create the project directory:\n") + prjdir, _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
    }

    // now create the project
    // make sure to respect the compiler chosen by the user for the project, too
    wxString defCompilerID = CompilerFactory::GetDefaultCompilerID();
    CompilerFactory::SetDefaultCompiler(GetCompilerID());
    // create the project with the (probably) updated compiler
    theproject = Manager::Get()->GetProjectManager()->NewProject(prjname);
    // setup the old default compiler again
    CompilerFactory::SetDefaultCompiler(defCompilerID);
    if (!theproject)
    {
        cbMessageBox(_("Couldn't create the new project:\n") + prjdir, _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
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

    // if no targets were created (due to user misconfiguration probably),
    // create a "default" target
    if (theproject->GetBuildTargetsCount() == 0)
    {
        ProjectBuildTarget* target = theproject->AddBuildTarget(_T("default"));
        if (target)
        {
            target->SetCompilerID(GetCompilerID());
            target->SetIncludeInTargetAll(false);
        }
    }

    // add all the template files
    // first get the dirs with the files by calling GetFilesDir()

    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();

    const wxString *srcDir = nullptr;
    ScriptBindings::Caller caller(scriptMgr->GetVM());

    if (!caller.CallByNameAndReturn0(_SC("GetFilesDir"), srcDir))
    {
        scriptMgr->DisplayErrors(true);
        Clear();
        return nullptr;
    }
    if (srcDir && !srcDir->empty())
    {
        // now break them up (remember: semicolon-separated list of dirs)
        wxArrayString tmpsrcdirs = GetArrayFromString(*srcDir, ";", true);
        // and copy files from each source dir we got
        for (size_t i = 0; i < tmpsrcdirs.GetCount(); ++i)
            CopyFiles(theproject, prjdir, tmpsrcdirs[i]);
    }
    caller.Finish();

    // add generated files
    if (caller.SetupFunc(_SC("GetGeneratedFile")))
    {
        wxArrayString files;
        wxArrayString contents;

        // safety limit to avoid infinite loops because of badly written scripts: 50 files
        for (int idx = 0; idx < 50; ++idx)
        {
            const wxString *fileAndContents = nullptr;
            if (!caller.CallAndReturn1(fileAndContents, idx))
            {
                scriptMgr->DisplayErrors(true);
                Clear();
                return nullptr;
            }

            if (fileAndContents == nullptr || fileAndContents->empty())
                break;
            wxString tmpFile = fileAndContents->BeforeFirst(_T(';'));
            wxString tmpContents = fileAndContents->AfterFirst(_T(';'));
            tmpFile.Trim();
            tmpContents.Trim();
            if (tmpFile.IsEmpty() || tmpContents.IsEmpty())
                break;
            files.Add(tmpFile);
            contents.Add(tmpContents);

            // Do this last to prepare for the next iteration.
            // Make sure you do this after the last use of fileAndContents.
            caller.PopResult();
        }

        if (files.GetCount() != 0 && contents.GetCount() == files.GetCount())
        {
            // prepare the list of targets to add this file to (i.e. all of them)
            wxArrayInt targetIndices;
            for (int x = 0; x < theproject->GetBuildTargetsCount(); ++x)
                targetIndices.Add(x);

            theproject->BeginAddFiles();

            // ok, we have to generate some files here
            size_t count = files.GetCount();
            for (size_t i = 0; i < count; ++i)
            {
                // GenerateFile() performs sanity and security checks
                wxString actual = GenerateFile(theproject->GetBasePath(), files[i], contents[i]);

                if (!actual.IsEmpty())
                {
                    // Add the file only if it does not exist
                    if (theproject->GetFileByFilename(files[i], true, true) == NULL)
                    {
                        Manager::Get()->GetLogManager()->DebugLog(_T("Generated file ") + actual);
                        // add it to the project
                        Manager::Get()->GetProjectManager()->AddFileToProject(actual, theproject, targetIndices);
                    }
                    else
                    {
                        Manager::Get()->GetLogManager()->DebugLog(F(_T("File %s exists"), actual.wx_str()));
                    }
                }
            }

            theproject->EndAddFiles();
        }

        caller.Finish();
    }

    // ask the script to setup the new project (edit targets, setup options, etc)
    // call SetupProject()
    bool result = false;
    if (!caller.CallByNameAndReturn1(_SC("SetupProject"), result, theproject))
    {
        scriptMgr->DisplayErrors(true);
        Clear();
        return nullptr;
    }

    if (!result)
    {
        cbMessageBox(wxString::Format(_("Couldn't setup project options:\n%s"), prjdir.c_str()),
                     _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
    }

    // save the project and...
    theproject->Save();

    if (pFilename)
        *pFilename = theproject->GetFilename();

    // finally, make sure everything looks ok
    Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
    Manager::Get()->GetProjectManager()->GetUI().GetTree()->Expand(theproject->GetProjectNode());
    return theproject;
}

CompileTargetBase* Wiz::RunTargetWizard(cb_unused wxString* pFilename)
{
    cbProject* theproject = Manager::Get()->GetProjectManager()->GetActiveProject(); // can't fail; if no project, the wizard didn't even run
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();

    bool isDebug = false;
    wxString targetName;

    if (m_pWizBuildTargetPanel)
    {
        targetName = GetTargetName();
        isDebug = GetTargetEnableDebug();
    }
    else
    {
        ScriptBindings::Caller caller(scriptMgr->GetVM());

        wxString *result = nullptr;
        if (caller.CallByNameAndReturn0(_SC("GetTargetName"), result)) {
            if (!result || result->empty())
            {
                cbMessageBox(_("GetTargetName returned empty string. Failing!"), _("Error"), wxICON_ERROR);
                Clear();
                return nullptr;
            }
            targetName = *result;
        }
        else
        {
            scriptMgr->DisplayErrors(true);
            Clear();
            return nullptr;
        }

        isDebug = false;
    }

    ProjectBuildTarget* target = theproject->AddBuildTarget(targetName);
    if (!target)
    {
        cbMessageBox(_("Failed to create build target!"), _("Error"), wxICON_ERROR);
        Clear();
        return nullptr;
    }

    // Setup the compiler and other target parameters only if there is a BuildTarget panel.
    // If not leave all this task to the script.
    if (m_pWizBuildTargetPanel)
    {
        // check the compiler Id
        wxString CompilerId = GetTargetCompilerID();
        if(CompilerId == wxEmptyString)
        {    // no compiler had been specified
            // fall back 1 : the poject one
            CompilerId = theproject->GetCompilerID();
            if(CompilerId == wxEmptyString)
            {    // even the project does not have one
                // fall back 2 : CB default
                CompilerId = CompilerFactory::GetDefaultCompilerID();
                cbMessageBox(    _("No compiler had been specified. The new target will use the default compiler."),
                    _("Fallback compiler selected"),
                    wxOK | wxICON_INFORMATION,
                    Manager::Get()->GetAppWindow());
            }
            else
            {
                cbMessageBox(    _("No compiler had been specified. The new target will use the same compiler as the project."),
                    _("Fallback compiler selected"),
                    wxOK | wxICON_INFORMATION,
                    Manager::Get()->GetAppWindow());
            }
        }
        // setup the target
        target->SetCompilerID(CompilerId);
        target->SetIncludeInTargetAll(false);
        target->SetObjectOutput(GetTargetObjectOutputDir());
        target->SetWorkingDir(GetTargetOutputDir());
    }
    // Assign this target to all project files
    for (FilesList::iterator it = theproject->GetFilesList().begin(); it != theproject->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        if (pf)
            pf->AddBuildTarget(targetName);
    }

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
//        return nullptr;
//    }

    ScriptBindings::Caller caller(scriptMgr->GetVM());

    // Ask the script to setup the new target (setup options, etc) by calling its SetupTarget().
    bool result = false;
    if (caller.CallByNameAndReturn2(_SC("SetupTarget"), result, target, isDebug)) {
        if (!result)
        {
            cbMessageBox(_("Couldn't setup target options:"), _("Error"), wxICON_ERROR);
            Clear();
            return nullptr;
        }
    }
    else
    {
        scriptMgr->DisplayErrors(true);
        Clear();
        return nullptr;
    }

    return target;
}

CompileTargetBase* Wiz::RunFilesWizard(wxString* pFilename)
{
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();
    ScriptBindings::Caller caller(scriptMgr->GetVM());

    wxString *result = nullptr;
    if (caller.CallByNameAndReturn0(_SC("CreateFiles"), result))
    {
        if (!result || result->empty())
            cbMessageBox(_("Wizard failed... (returned no files)"), _("Error"), wxICON_ERROR);
        else
        {
            const wxString &filename = result->BeforeFirst(_T(';'));
            if (pFilename)
                *pFilename = filename;
            EditorBase *editor = Manager::Get()->GetEditorManager()->GetEditor(filename);
            if (editor && editor->IsBuiltinEditor())
                static_cast<cbEditor*>(editor)->SetEditorStyle();
        }
    }
    else
        scriptMgr->DisplayErrors(true);

    Clear();
    return nullptr;
}

CompileTargetBase* Wiz::RunCustomWizard(cb_unused wxString* pFilename)
{
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();
    ScriptBindings::Caller caller(scriptMgr->GetVM());

    bool result = false;
    if (caller.CallByNameAndReturn0(_SC("SetupCustom"), result))
    {
        if (!result)
            cbMessageBox(_("Wizard failed..."), _("Error"), wxICON_ERROR);
    }
    else
        scriptMgr->DisplayErrors(true);

    Clear();
    return nullptr;
}

wxString Wiz::GenerateFile(const wxString& basePath, const wxString& filename, const wxString& contents)
{
    wxFileName fname(filename);

    // extension sanity check
    FileType ft = FileTypeOf(fname.GetFullPath());
    switch (ft)
    {
        case ftCodeBlocksProject:
        case ftCodeBlocksWorkspace:
        case ftExecutable:
        case ftDynamicLib:
        case ftStaticLib:
        case ftResourceBin:
        case ftObject:
//        case ftOther:
            Manager::Get()->GetLogManager()->DebugLog(_T("Attempt to generate a file with forbidden extension!\nFile: ") + fname.GetFullPath());
            return wxEmptyString;
        default: break;
    }

    // make sure filename is relative
    if (!fname.IsRelative())
        fname.MakeRelativeTo(basePath);

    // make sure filename is located inside the project path (should already be)
    const wxArrayString& Dirs = fname.GetDirs();
    int IntDirCount = 0;
    for ( size_t i=0; i<Dirs.Count(); i++ )
    {
        if ( Dirs[i] == _T("..") )
        {
            if ( IntDirCount-- == 0 )
            {
                // attempt to create file outside the project dir
                // remove any path info from the filename
                fname = fname.GetFullName();
                Manager::Get()->GetLogManager()->DebugLog(F(_T("Attempt to generate a file outside the project base dir:\nOriginal: %s\nConverted to:%s"), filename.wx_str(), fname.GetFullPath().wx_str()));
                break;
            }
        }
        else if ( Dirs[i] != _T(".") )
            IntDirCount++;
    }

    fname = basePath + wxFILE_SEP_PATH + fname.GetFullPath();
    if ( fname.FileExists() )
    {
        wxString query_overwrite;
        query_overwrite.Printf(
          _T("Warning:\n")
          _T("The wizard is about to OVERWRITE the following existing file:\n")+
          fname.GetFullPath()+_T("\n\n") +
          _T("Are you sure that you want to OVERWRITE the file?\n\n")+
          _T("(If you answer 'No' the existing file will be kept.)"));
        if (cbMessageBox(query_overwrite, _T("Confirmation"),
                         wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_NO)
        {
            return fname.GetFullPath();
        }
    }

    // create the file with the passed contents
    wxFileName::Mkdir(fname.GetPath(),0777,wxPATH_MKDIR_FULL);
    wxFile f(fname.GetFullPath(), wxFile::write);

    if ( cbWrite(f, contents + GetEOLStr(), wxFONTENCODING_UTF8) )
        return fname.GetFullPath(); // success

    return wxEmptyString; // failed
}

void Wiz::CopyFiles(cbProject* theproject, const wxString&  prjdir, const wxString& srcdir)
{
    // first get the dir with the files
    wxArrayString filesList;
    wxString enumdirs = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + srcdir;
    if ( !wxDirExists(enumdirs + _T("/")) )
        enumdirs = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + srcdir;
    wxString basepath = wxFileName(enumdirs).GetFullPath();

    if ( wxDirExists(enumdirs + _T("/")) )
    {
        // recursively enumerate all files under srcdir
        wxDir::GetAllFiles(enumdirs, &filesList);
    }

    // prepare the list of targets to add this file to (i.e. all of them)
    wxArrayInt targetIndices;
    for (int x = 0; x < theproject->GetBuildTargetsCount(); ++x)
        targetIndices.Add(x);

    theproject->BeginAddFiles();

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
        bool do_copy = true; // default case: file most likely does *not* exist
        if (wxFileName::FileExists(dstfile))
        {
            wxString query_overwrite;
            query_overwrite.Printf(
              _T("Warning:\n")
              _T("The wizard is about to OVERWRITE the following existing file:\n")+
              wxFileName(dstfile).GetFullPath()+_T("\n\n")+
              _T("Are you sure that you want to OVERWRITE the file?\n\n")+
              _T("(If you answer 'No' the existing file will be kept.)"));
            if (cbMessageBox(query_overwrite, _T("Confirmation"),
                             wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxID_YES)
            {
                do_copy = false; // keep the old (existing) file
            }
        }
        if (do_copy) wxCopyFile(srcfile, dstfile, true);

        // and add it to the project
        fname.MakeRelativeTo(prjdir);
        Manager::Get()->GetProjectManager()->AddFileToProject(fname.GetFullPath(), theproject, targetIndices);
    }

    theproject->EndAddFiles();
}

////////////////////////
// Scripting - BEGIN
////////////////////////

wxString Wiz::FindTemplateFile(const wxString& filename)
{
    wxString f = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + filename;
    if (!wxFileExists(f))
        f = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + filename;
    return f;
}

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
                Compiler* compiler = CompilerFactory::GetCompiler(i);
                if (compiler)
                    win->Append(compiler->GetName());
            }
            Compiler* compiler = CompilerFactory::GetDefaultCompiler();
            if (compiler)
                win->SetSelection(win->FindString(compiler->GetName()));
        }
    }
}

void Wiz::FillContainerWithCompilers(const wxString& name, const wxString& compilerID, const wxString& validCompilerIDs)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>(page->FindWindowByName(name, page));
        if (win && win->GetCount() == 0)
        {
            Wizard::FillCompilerControl(win, compilerID, validCompilerIDs);
        }
    }
}

void Wiz::FillContainerWithSelectCompilers( const wxString& name, const wxString& validCompilerIDs )
{
    // Fill the named window with compilers matching a mask/filter
    // Example: FillContainerWithSelectCompilers(_T("GenericChoiceList"), _T("*arm*;rx*;mips*"));

    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>( page->FindWindowByName( name.IsEmpty() ? _T("GenericChoiceList") : name , page ) );
        if (win)
        {
            wxArrayString valids = GetArrayFromString(validCompilerIDs, _T(";"), true);
            win->Clear();
            for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(i);
                if (compiler)
                {
                    for (size_t n = 0; n < valids.GetCount(); ++n)
                    {
                        // match not only if IDs match, but if ID inherits from it too
                        if (CompilerFactory::CompilerInheritsFrom(compiler, valids[n]))
                        {
                            win->Append(compiler->GetName());
                            break;
                        }
                    }
                }
            }
            Compiler* compiler = CompilerFactory::GetDefaultCompiler();
            if (compiler)
                win->SetSelection(win->FindString(compiler->GetName()));
        }
    }
}

void Wiz::AppendContainerWithSelectCompilers( const wxString& name, const wxString& validCompilerIDs )
{
    // Add to the named window the compilers matching a mask/filter
    // Example: AppendContainerWithSelectCompilers(_T("GenericChoiceList"), _T("*arm*;rx*;mips*"));

    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>( page->FindWindowByName( name.IsEmpty() ? _T("GenericChoiceList") : name , page ) );
        if (win)
        {
            wxArrayString valids = GetArrayFromString(validCompilerIDs, _T(";"), true);
            size_t iItemsCount = win->GetCount();
            wxString nameInItems = _T(";");
            for( size_t i = 0; i < iItemsCount; ++i )
            {
                nameInItems += win->GetString(i) + _T(";");
            }
            for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(i);
                if (compiler)
                {
                    wxString compilerName = compiler->GetName();
                    if( wxNOT_FOUND != nameInItems.Find( _T(";") + compilerName + _T(";") ) )
                        continue;
                    for (size_t n = 0; n < valids.GetCount(); ++n)
                    {
                        // match not only if IDs match, but if ID inherits from it too
                        if (CompilerFactory::CompilerInheritsFrom(compiler, valids[n]))
                        {
                            win->Append( compilerName );
                            nameInItems += compilerName + _T(";");
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Wiz::EnableWindow(const wxString& name, bool enable)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxWindow* win = page->FindWindowByName(name, page);
        if (win)
            win->Enable(enable);
    }
}

void Wiz::SetComboboxSelection(const wxString& name, int sel)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>(page->FindWindowByName(name, page));
        if (win)
            win->SetSelection(sel);
    }
}

void Wiz::SetComboboxValue(const wxString& name, const wxString& value)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win)
            win->SetValue(value);
    }
}

wxString Wiz::GetComboboxValue(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxComboBox* win = dynamic_cast<wxComboBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetValue();
    }
    return wxEmptyString;
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
        wxItemContainer* win = dynamic_cast<wxItemContainer*>(page->FindWindowByName(name, page));
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
        wxItemContainer* win = dynamic_cast<wxItemContainer*>(page->FindWindowByName(name, page));
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

int Wiz::GetListboxSelection(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxListBox* win = dynamic_cast<wxListBox*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetSelection();
    }
    return -1;
}


wxString Wiz::GetListboxSelections(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxListBox* lbox = dynamic_cast<wxListBox*>(page->FindWindowByName(name, page));
        if (lbox)
        {
            wxString result;
            size_t i;
            wxArrayInt selections;
            lbox->GetSelections(selections);
            for (i = 0; i < selections.GetCount(); ++i)
                result.Append(wxString::Format(_T("%d;"), selections[i]));
            return result;
        }
    }
    return wxEmptyString;
}


wxString Wiz::GetListboxStringSelections(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxListBox* lbox = dynamic_cast<wxListBox*>(page->FindWindowByName(name, page));
        if (lbox)
        {
            wxString result;
            size_t i;
            wxArrayInt selections;
            lbox->GetSelections(selections);
            for (i = 0; i < selections.GetCount(); ++i)
                result.Append(lbox->GetString(selections[i]) + _T(";"));
            return result;
        }
    }
    return wxEmptyString;
}


void Wiz::SetListboxSelection(const wxString& name, int sel)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxListBox* win = dynamic_cast<wxListBox*>(page->FindWindowByName(name, page));
        if (win)
            win->SetSelection(sel);
    }
}

wxString Wiz::GetCheckListboxChecked(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckListBox* clb = dynamic_cast<wxCheckListBox*>(page->FindWindowByName(name, page));
        if (clb)
        {
            wxString result;
            unsigned int i;
            for (i = 0; i < clb->GetCount(); ++i)
            {
                if (clb->IsChecked(i))
                    result.Append(wxString::Format(_T("%u;"), i));
            }
            return result;
        }
    }
    return wxEmptyString;
}

wxString Wiz::GetCheckListboxStringChecked(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckListBox* clb = dynamic_cast<wxCheckListBox*>(page->FindWindowByName(name, page));
        if (clb)
        {
            wxString result;
            unsigned int i;
            for (i = 0; i < clb->GetCount(); ++i)
            {
                if (clb->IsChecked(i))
                    result.Append(wxString::Format(_T("%s;"), clb->GetString(i).wx_str()));
            }
            return result;
        }
    }
    return wxEmptyString;
}

bool Wiz::IsCheckListboxItemChecked(const wxString& name, unsigned int item)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckListBox* clb = dynamic_cast<wxCheckListBox*>(page->FindWindowByName(name, page));
        if (clb)
            return clb->IsChecked(item);
    }
    return false;
}

void Wiz::CheckCheckListboxItem(const wxString& name, unsigned int item, bool check)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxCheckListBox* clb = dynamic_cast<wxCheckListBox*>(page->FindWindowByName(name, page));
        if (clb)
            clb->Check(item, check);
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

void Wiz::SetSpinControlValue(const wxString& name, int value)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxSpinCtrl* win = dynamic_cast<wxSpinCtrl*>(page->FindWindowByName(name, page));
        if (win)
            win->SetValue(value);
    }
}

int Wiz::GetSpinControlValue(const wxString& name)
{
    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxSpinCtrl* win = dynamic_cast<wxSpinCtrl*>(page->FindWindowByName(name, page));
        if (win)
            return win->GetValue();
    }
    return -1;
}

void Wiz::AddInfoPage(const wxString& pageId, const wxString& intro_msg)
{
    // we don't track this; can add more than one
    WizPageBase* page = new WizInfoPanel(pageId, intro_msg, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!page->SkipPage())
        m_Pages.Add(page);
    else
        delete page;
}

void Wiz::AddFilePathPage(bool showHeaderGuard)
{
    if (m_pWizFilePathPanel)
        return; // already added
    m_pWizFilePathPanel = new WizFilePathPanel(showHeaderGuard, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!m_pWizFilePathPanel->SkipPage())
        m_Pages.Add(m_pWizFilePathPanel);
    else
    {
        delete m_pWizFilePathPanel;
        m_pWizFilePathPanel = nullptr;
    }
}

void Wiz::AddProjectPathPage()
{
    if (m_pWizProjectPathPanel)
        return; // already added
    m_pWizProjectPathPanel = new WizProjectPathPanel(m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!m_pWizProjectPathPanel->SkipPage())
        m_Pages.Add(m_pWizProjectPathPanel);
    else
    {
        delete m_pWizProjectPathPanel;
        m_pWizProjectPathPanel = nullptr;
    }
}

void Wiz::AddCompilerPage(const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange, bool allowConfigChange)
{
    if (m_pWizCompilerPanel)
        return; // already added
    m_pWizCompilerPanel = new WizCompilerPanel(compilerID, validCompilerIDs, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG, allowCompilerChange, allowConfigChange);
    if (!m_pWizCompilerPanel->SkipPage())
        m_Pages.Add(m_pWizCompilerPanel);
    else
    {
        delete m_pWizCompilerPanel;
        m_pWizCompilerPanel = nullptr;
    }
}

void Wiz::AddBuildTargetPage(const wxString& targetName, bool isDebug, bool showCompiler, const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange)
{
    if (m_pWizBuildTargetPanel)
        return; // already added
    m_pWizBuildTargetPanel = new WizBuildTargetPanel(targetName, isDebug, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG, showCompiler, compilerID, validCompilerIDs, allowCompilerChange);
    if (!m_pWizBuildTargetPanel->SkipPage())
        m_Pages.Add(m_pWizBuildTargetPanel);
    else
    {
        delete m_pWizBuildTargetPanel;
        m_pWizBuildTargetPanel = nullptr;
    }
}

void Wiz::AddGenericSingleChoiceListPage(const wxString& pageName, const wxString& descr, const wxString& choices, int defChoice)
{
    // we don't track this; can add more than one
    WizPageBase* page = new WizGenericSingleChoiceList(pageName, descr, GetArrayFromString(choices, _T(";")), defChoice, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!page->SkipPage())
        m_Pages.Add(page);
    else
        delete page;
}

void Wiz::AddGenericSelectPathPage(const wxString& pageId, const wxString& descr, const wxString& label, const wxString& defValue)
{
    // we don't track this; can add more than one
    WizPageBase* page = new WizGenericSelectPathPanel(pageId, descr, label, defValue, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!page->SkipPage())
        m_Pages.Add(page);
    else
        delete page;
}

void Wiz::AddPage(const wxString& panelName)
{
    WizPage* page = new WizPage(panelName, m_pWizard, m_Wizards[m_LaunchIndex].wizardPNG);
    if (!page->SkipPage())
        m_Pages.Add(page);
    else
        delete page;
}

void Wiz::Finalize()
{
    // chain pages
    for (size_t i = 1; i < m_Pages.GetCount(); ++i)
        wxWizardPageSimple::Chain(m_Pages[i - 1], m_Pages[i]);

    // allow the wizard to size itself around the pages
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
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
    // check that this isn't registered already
    // keys are otype and title
    for (size_t i = 0; i < m_Wizards.GetCount(); ++i)
    {
        WizardInfo& info = m_Wizards[i];
        if (info.output_type == otype && info.title == title)
        {
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Wizard already registered. Skipping... (%s)"), title.wx_str()));
            return;
        }
    }

    // locate the images and XRC
    wxString tpng = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + templatePNG;
    if (!wxFileExists(tpng))
        tpng = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + templatePNG;
    wxString wpng = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + wizardPNG;
    if (!wxFileExists(wpng))
        wpng = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + wizardPNG;
    wxString _xrc = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + xrc;
    if (!wxFileExists(_xrc))
        _xrc = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + xrc;

    WizardInfo info;
    info.output_type = otype;
    info.title = title;
    info.cat = cat;
    info.script = script;
    info.templatePNG = cbLoadBitmap(tpng, wxBITMAP_TYPE_PNG);

    // wx3.0 asserts when the image is smaller than 32x32, so we need to resize it.
    if (info.templatePNG.Ok() && (info.templatePNG.GetWidth() != 32 || info.templatePNG.GetHeight() != 32))
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_("Resizing image '%s' to fit 32x32 (original size is %dx%d)"),
                                                      tpng.wx_str(), info.templatePNG.GetWidth(),
                                                      info.templatePNG.GetHeight()));
        wxImage temp = info.templatePNG.ConvertToImage();
        temp.Resize(wxSize(32, 32), wxPoint(0, 0), -1, -1, -1);
        info.templatePNG = wxBitmap(temp);
    }

    info.wizardPNG = cbLoadBitmap(wpng, wxBITMAP_TYPE_PNG);
    info.xrc = _xrc;
    m_Wizards.Add(info);

    wxString typS;
    switch (otype)
    {
        case totProject: typS = _T("Project");      break;
        case totTarget:  typS = _T("Build-target"); break;
        case totFiles:   typS = _T("File(s)");      break;
        case totUser:    typS = _T("User");         break;
        case totCustom:  typS = _T("Custom");       break;
        default: break;
    }

    Manager::Get()->GetLogManager()->DebugLog(F(typS + _T(" wizard added for '%s'"), title.wx_str()));
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
    if (m_pWizCompilerPanel && GetWizardType() == totProject)
        return m_pWizCompilerPanel->GetCompilerID();
    else if (GetWizardType() == totTarget && m_pWizBuildTargetPanel)
        return m_pWizBuildTargetPanel->GetCompilerID();
    return m_DefCompilerID;
}

bool Wiz::GetWantDebug()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetWantDebug();
    return m_WantDebug;
}

wxString Wiz::GetDebugName()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugName();
    return m_DebugName;
}

wxString Wiz::GetDebugOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugOutputDir();
    return m_DebugOutputDir;
}

wxString Wiz::GetDebugObjectOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetDebugObjectOutputDir();
    return m_DebugObjOutputDir;
}

bool Wiz::GetWantRelease()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetWantRelease();
    return m_WantRelease;
}

wxString Wiz::GetReleaseName()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseName();
    return m_ReleaseName;
}

wxString Wiz::GetReleaseOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseOutputDir();
    return m_ReleaseOutputDir;
}

wxString Wiz::GetReleaseObjectOutputDir()
{
    if (m_pWizCompilerPanel)
        return m_pWizCompilerPanel->GetReleaseObjectOutputDir();
    return m_ReleaseObjOutputDir;
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

void Wiz::SetCompilerDefault(cb_unused const wxString& defCompilerID)
{
    // default compiler settings (returned if no compiler page is added in the wizard)
    m_DefCompilerID = CompilerFactory::GetDefaultCompilerID();
}

void Wiz::SetDebugTargetDefaults(bool wantDebug,
                                    const wxString& debugName,
                                    const wxString& debugOut,
                                    const wxString& debugObjOut)
{
    // default compiler settings (returned if no compiler page is added in the wizard)
    m_WantDebug = wantDebug;
    m_DebugName = debugName;
    m_DebugOutputDir = debugOut;
    m_DebugObjOutputDir = debugObjOut;
}

void Wiz::SetReleaseTargetDefaults(bool wantRelease,
                                    const wxString& releaseName,
                                    const wxString& releaseOut,
                                    const wxString& releaseObjOut)
{
    // default compiler settings (returned if no compiler page is added in the wizard)
    m_WantRelease = wantRelease;
    m_ReleaseName = releaseName;
    m_ReleaseOutputDir = releaseOut;
    m_ReleaseObjOutputDir = releaseObjOut;
}

int Wiz::FillContainerWithChoices( const wxString& name, const wxString& choices )
{
    // Fill the named window with a semi-colon separated set of strings
    // Eg: FillContainerWithChoices(_T("GenericChoiceList"), _T("this;that;another"));

    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>( page->FindWindowByName( name.IsEmpty() ? _T("GenericChoiceList") : name , page ) );
        if (win)
        {
            win->Clear();
            wxArrayString items = GetArrayFromString( choices, _T(";") );
            unsigned int nItems = items.GetCount();
            for ( unsigned int i = 0; i < nItems; i++ )
            {
                win->Append( items[i] );
            }

            return 0;
        }
    }
    return -1;
}

int Wiz::AppendContainerWithChoices( const wxString& name, const wxString& choices )
{
    // Add to the named window, a semi-colon separated set of strings
    // Eg: AppendContainerWithChoices(_T("GenericChoiceList"), _T("this;that;another"));

    wxWizardPage* page = m_pWizard->GetCurrentPage();
    if (page)
    {
        wxItemContainer* win = dynamic_cast<wxItemContainer*>( page->FindWindowByName( name.IsEmpty() ? _T("GenericChoiceList") : name , page ) );
        if (win)
        {
            wxArrayString items = GetArrayFromString( choices, _T(";") );
            size_t iItemsCount = win->GetCount();
            wxString nameInItems = _T(";");
            for( size_t i = 0; i < iItemsCount; ++i )
            {
                nameInItems += win->GetString(i) + _T(";");
            }
            unsigned int nItems = items.GetCount();
            for ( unsigned int i = 0; i < nItems; i++ )
            {
                wxString tItemsName = items[i];
                if( wxNOT_FOUND != nameInItems.Find( _T(";") + tItemsName + _T(";") ) )
                    continue;
                win->Append( tItemsName );
                nameInItems += tItemsName + _T(";");
            }

            return 0;
        }
    }
    return -1;
}

wxString Wiz::GetWizardScriptFolder(void)
{
    // Return the name only of the current wizard folder (this is not a path)
    //ie., would return only _T("arm") for ...\trunk\src\output\share\CodeBlocks\templates\wizard\arm

    return m_WizardScriptFolder;
}

namespace ScriptBindings
{
SQInteger Wiz_AddWizard(HSQUIRRELVM v)
{
    // this, otype, title, cat, script, templatePNG, wizardPNG, xrc
    ExtractParams8<Wiz*, SQInteger, const wxString *, const wxString *, const wxString *,
                   const wxString *, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::AddWizard"))
        return extractor.ErrorMessage();
    if (extractor.p1 < SQInteger(totProject) || extractor.p1 > SQInteger(totUser))
        return sq_throwerror(v, _SC("Wiz::AddWizard: Value of otype parameter is out of range"));
    const TemplateOutputType otype = TemplateOutputType(extractor.p1);
    extractor.p0->AddWizard(otype, *extractor.p2, *extractor.p3, *extractor.p4, *extractor.p5,
                            *extractor.p6, *extractor.p7);
    return 0;
}

SQInteger Wiz_AddInfoPage(HSQUIRRELVM v)
{
    // this, pageId, intro_msg
    ExtractParams3<Wiz*, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::AddInfoPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddInfoPage(*extractor.p1, *extractor.p2);
    return 0;
}

SQInteger Wiz_AddProjectPathPage(HSQUIRRELVM v)
{
    // this
    ExtractParams1<Wiz*> extractor(v);
    if (!extractor.Process("Wiz::AddProjectPathPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddProjectPathPage();
    return 0;
}

SQInteger Wiz_AddFilePathPage(HSQUIRRELVM v)
{
    // this, showHeaderGuard
    ExtractParams2<Wiz*, bool> extractor(v);
    if (!extractor.Process("Wiz::AddFilePathPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddFilePathPage(extractor.p1);
    return 0;
}

SQInteger Wiz_AddCompilerPage(HSQUIRRELVM v)
{
    // this, compilerId, validCompilerIDs, allowCompilerChange=true, allowConfigChange=true
    ExtractParams5<Wiz*, const wxString *, const wxString *, bool, bool> extractor(v);
    if (!extractor.Process("Wiz::AddCompilerPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddCompilerPage(*extractor.p1, *extractor.p2, extractor.p3, extractor.p4);
    return 0;
}

SQInteger Wiz_AddBuildTargetPage(HSQUIRRELVM v)
{
    // this, targetName, isDebug, showCompiler, compilerID = "", validCompilerIDs = "*",
    // allowCompilerChange
    ExtractParams7<Wiz*, const wxString *, bool, bool, const wxString *, const wxString *, bool> extractor(v);
    if (!extractor.Process("Wiz::AddBuildTargetPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddBuildTargetPage(*extractor.p1, extractor.p2, extractor.p3, *extractor.p4,
                                     *extractor.p5, extractor.p6);
    return 0;
}

SQInteger Wiz_AddGenericSingleChoiceListPage(HSQUIRRELVM v)
{
    // this, pageName, descr, choices, defChoice
    ExtractParams5<Wiz*, const wxString *, const wxString *, const wxString *, SQInteger> extractor(v);
    if (!extractor.Process("Wiz::AddGenericSingleChoiceListPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddGenericSingleChoiceListPage(*extractor.p1, *extractor.p2, *extractor.p3,
                                                 extractor.p4);
    return 0;
}

SQInteger Wiz_AddGenericSelectPathPage(HSQUIRRELVM v)
{
    // this, pageId, descr, label, defValue
    ExtractParams5<Wiz*, const wxString *, const wxString *, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::AddGenericSelectPathPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddGenericSelectPathPage(*extractor.p1, *extractor.p2, *extractor.p3,
                                           *extractor.p4);
    return 0;
}

SQInteger Wiz_AddPage(HSQUIRRELVM v)
{
    // this, panelName
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::AddPage"))
        return extractor.ErrorMessage();
    extractor.p0->AddPage(*extractor.p1);
    return 0;
}

SQInteger Wiz_SetCompilerDefault(HSQUIRRELVM v)
{
    // this, defCompilerID
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::SetCompilerDefault"))
        return extractor.ErrorMessage();
    extractor.p0->SetCompilerDefault(*extractor.p1);
    return 0;
}

template<void (Wiz::*func)(bool, const wxString &, const wxString &, const wxString &)>
SQInteger Wiz_SetTargetDefaults(HSQUIRRELVM v)
{
    // this, want, name, out, objOut
    ExtractParams5<Wiz*, bool, const wxString *, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::SetTargetDefaults"))
        return extractor.ErrorMessage();
    (extractor.p0->*func)(extractor.p1, *extractor.p2, *extractor.p3, *extractor.p4);
    return 0;
}

SQInteger Wiz_EnableWindow(HSQUIRRELVM v)
{
    // this, name, enable
    ExtractParams3<Wiz*, const wxString *, bool> extractor(v);
    if (!extractor.Process("Wiz::EnableWindow"))
        return extractor.ErrorMessage();
    extractor.p0->EnableWindow(*extractor.p1, extractor.p2);
    return 0;
}

SQInteger Wiz_SetTextControlValue(HSQUIRRELVM v)
{
    // this, name, value
    ExtractParams3<Wiz*, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::SetTextControlValue"))
        return extractor.ErrorMessage();
    extractor.p0->SetTextControlValue(*extractor.p1, *extractor.p2);
    return 0;
}

template<wxString (Wiz::*func)(const wxString &)>
SQInteger Wiz_GetStringFromString(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz_GetStringFromString"))
        return extractor.ErrorMessage();
    const wxString &result = (extractor.p0->*func)(*extractor.p1);
    return ConstructAndReturnInstance(v, result);
}

template<wxString (Wiz::*func)()>
SQInteger Wiz_GetString(HSQUIRRELVM v)
{
    // this
    ExtractParams1<Wiz*> extractor(v);
    if (!extractor.Process("Wiz_GetString"))
        return extractor.ErrorMessage();
    const wxString &result = (extractor.p0->*func)();
    return ConstructAndReturnInstance(v, result);
}

template<bool (Wiz::*func)()>
SQInteger Wiz_GetBool(HSQUIRRELVM v)
{
    // this
    ExtractParams1<Wiz*> extractor(v);
    if (!extractor.Process("Wiz_GetBool"))
        return extractor.ErrorMessage();
    sq_pushbool(v, (extractor.p0->*func)());
    return 1;
}

SQInteger Wiz_SetSpinControlValue(HSQUIRRELVM v)
{
    // this, name, value
    ExtractParams3<Wiz*, const wxString *, SQInteger> extractor(v);
    if (!extractor.Process("Wiz::SetSpinControlValue"))
        return extractor.ErrorMessage();
    extractor.p0->SetSpinControlValue(*extractor.p1, extractor.p2);
    return 0;
}

SQInteger Wiz_GetSpinControlValue(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::GetSpinControlValue"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, extractor.p0->GetSpinControlValue(*extractor.p1));
    return 1;
}

SQInteger Wiz_CheckCheckbox(HSQUIRRELVM v)
{
    // this, name, check
    ExtractParams3<Wiz*, const wxString *, bool> extractor(v);
    if (!extractor.Process("Wiz::CheckCheckbox"))
        return extractor.ErrorMessage();
    extractor.p0->CheckCheckbox(*extractor.p1, extractor.p2);
    return 0;
}

SQInteger Wiz_IsCheckboxChecked(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::IsCheckboxChecked"))
        return extractor.ErrorMessage();
    sq_pushbool(v, extractor.p0->IsCheckboxChecked(*extractor.p1));
    return 1;
}

SQInteger Wiz_FillComboboxWithCompilers(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::FillComboboxWithCompilers"))
        return extractor.ErrorMessage();
    extractor.p0->FillComboboxWithCompilers(*extractor.p1);
    return 0;
}

SQInteger Wiz_GetCompilerFromCombobox(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::GetCompilerFromCombobox"))
        return extractor.ErrorMessage();
    const wxString &result = extractor.p0->GetCompilerFromCombobox(*extractor.p1);
    return ConstructAndReturnInstance(v, result);
}

SQInteger Wiz_FillContainerWithCompilers(HSQUIRRELVM v)
{
    // this, name, compilerID, validCompilerIDs
    ExtractParams4<Wiz*, const wxString *, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::FillContainerWithCompilers"))
        return extractor.ErrorMessage();
    extractor.p0->FillContainerWithCompilers(*extractor.p1, *extractor.p2, *extractor.p3);
    return 0;
}

template<int (Wiz::*func)(const wxString &name)>
SQInteger Wiz_GetSelection(HSQUIRRELVM v)
{
    // this, name
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz_GetSelection"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, (extractor.p0->*func)(*extractor.p1));
    return 1;
}

template<void (Wiz::*func)(const wxString &, int)>
SQInteger Wiz_SetSelection(HSQUIRRELVM v)
{
    // this, name, sel
    ExtractParams3<Wiz*, const wxString *, SQInteger> extractor(v);
    if (!extractor.Process("Wiz_SetSelection"))
        return extractor.ErrorMessage();
    (extractor.p0->*func)(*extractor.p1, extractor.p2);
    return 0;
}

SQInteger Wiz_SetComboboxValue(HSQUIRRELVM v)
{
    // this, name, value
    ExtractParams3<Wiz*, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::SetComboboxValue"))
        return extractor.ErrorMessage();
    extractor.p0->SetComboboxValue(*extractor.p1, *extractor.p2);
    return 0;
}

SQInteger Wiz_IsCheckListboxItemChecked(HSQUIRRELVM v)
{
    // this, name, item
    ExtractParams3<Wiz*, const wxString *, SQInteger> extractor(v);
    if (!extractor.Process("Wiz::IsCheckListboxItemChecked"))
        return extractor.ErrorMessage();
    if (extractor.p2 < 0)
        return sq_throwerror(v, _SC("Wiz::IsCheckListboxItemChecked: item parameter given negative value!"));
    sq_pushbool(v, extractor.p0->IsCheckListboxItemChecked(*extractor.p1, extractor.p2));
    return 1;
}

SQInteger Wiz_CheckCheckListboxItem(HSQUIRRELVM v)
{
    // this, name, item, check
    ExtractParams4<Wiz*, const wxString *, SQInteger, bool> extractor(v);
    if (!extractor.Process("Wiz::CheckCheckListboxItem"))
        return extractor.ErrorMessage();
    if (extractor.p2 < 0)
        return sq_throwerror(v, _SC("Wiz::CheckCheckListboxItem: item parameter given negative value!"));
    extractor.p0->CheckCheckListboxItem(*extractor.p1, extractor.p2, extractor.p3);
    return 0;
}

SQInteger Wiz_GetWizardType(HSQUIRRELVM v)
{
    // this
    ExtractParams1<Wiz*> extractor(v);
    if (!extractor.Process("Wiz::GetWizardType"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, extractor.p0->GetWizardType());
    return 1;
}

SQInteger Wiz_GetFileTargetIndex(HSQUIRRELVM v)
{
    // this
    ExtractParams1<Wiz*> extractor(v);
    if (!extractor.Process("Wiz::GetFileTargetIndex"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, extractor.p0->GetFileTargetIndex());
    return 1;
}

SQInteger Wiz_SetFilePathSelectionFilter(HSQUIRRELVM v)
{
    // this, filter
    ExtractParams2<Wiz*, const wxString *> extractor(v);
    if (!extractor.Process("Wiz::SetFilePathSelectionFilter"))
        return extractor.ErrorMessage();
    extractor.p0->SetFilePathSelectionFilter(*extractor.p1);
    return 0;
}

template<void (Wiz::*func)(const wxString &, const wxString &)>
SQInteger Wiz_ContainerWithSelectCompilers(HSQUIRRELVM v)
{
    // this, name, validCompilerIDs
    ExtractParams3<Wiz*, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz_ContainerWithSelectCompilers"))
        return extractor.ErrorMessage();
    (extractor.p0->*func)(*extractor.p1, *extractor.p2);
    return 0;
}

template<int (Wiz::*func)(const wxString &, const wxString &)>
SQInteger Wiz_ContainerWithChoices(HSQUIRRELVM v)
{
    // this, name, choices
    ExtractParams3<Wiz*, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("Wiz_ContainerWithChoices"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, (extractor.p0->*func)(*extractor.p1, *extractor.p2));
    return 1;
}

} // namespace ScriptBindings

void Wiz::RegisterWizard(HSQUIRRELVM v)
{
    using namespace ScriptBindings;

    PreserveTop preserveTop(v);

    sq_pushroottable(v);

    {
        // Register Wiz
        const SQInteger classDecl = CreateClassDecl<Wiz>(v, _SC("Wiz"));

        // register new wizards
        BindMethod(v, _SC("AddWizard"), Wiz_AddWizard, _SC("Wiz::AddWizard"));

        // add wizard pages
        BindMethod(v, _SC("AddInfoPage"), Wiz_AddInfoPage, _SC("Wiz::AddInfoPage"));
        BindMethod(v, _SC("AddProjectPathPage"), Wiz_AddProjectPathPage, _SC("Wiz::AddProjectPathPage"));
        BindMethod(v, _SC("AddFilePathPage"), Wiz_AddFilePathPage, _SC("Wiz::AddFilePathPage"));
        BindMethod(v, _SC("AddCompilerPage"), Wiz_AddCompilerPage, _SC("Wiz::AddCompilerPage"));
        BindMethod(v, _SC("AddBuildTargetPage"), Wiz_AddBuildTargetPage, _SC("Wiz::AddBuildTargetPage"));
        BindMethod(v, _SC("AddGenericSingleChoiceListPage"), Wiz_AddGenericSingleChoiceListPage, _SC("Wiz::AddGenericSingleChoiceListPage"));
        BindMethod(v, _SC("AddGenericSelectPathPage"), Wiz_AddGenericSelectPathPage, _SC("Wiz::AddGenericSelectPathPage"));
        BindMethod(v, _SC("AddPage"), Wiz_AddPage, _SC("Wiz::AddPage"));

        // compiler defaults
        BindMethod(v, _SC("SetCompilerDefault"), Wiz_SetCompilerDefault, _SC("Wiz::SetCompilerDefault"));
        BindMethod(v, _SC("SetDebugTargetDefaults"), Wiz_SetTargetDefaults<&Wiz::SetDebugTargetDefaults>, _SC("Wiz::SetDebugTargetDefaults"));
        BindMethod(v, _SC("SetReleaseTargetDefaults"), Wiz_SetTargetDefaults<&Wiz::SetReleaseTargetDefaults>, _SC("Wiz::SetReleaseTargetDefaults"));

        // GUI controls
        BindMethod(v, _SC("EnableWindow"), Wiz_EnableWindow, _SC("Wiz::EnableWindow"));
        BindMethod(v, _SC("SetTextControlValue"), Wiz_SetTextControlValue, _SC("Wiz::SetTextControlValue"));
        BindMethod(v, _SC("GetTextControlValue"), Wiz_GetStringFromString<&Wiz::GetTextControlValue>, _SC("Wiz::GetTextControlValue"));
        BindMethod(v, _SC("SetSpinControlValue"), Wiz_SetSpinControlValue, _SC("Wiz::SetSpinControlValue"));
        BindMethod(v, _SC("GetSpinControlValue"), Wiz_GetSpinControlValue, _SC("Wiz::GetSpinControlValue"));
        BindMethod(v, _SC("CheckCheckbox"), Wiz_CheckCheckbox, _SC("Wiz::CheckCheckbox"));
        BindMethod(v, _SC("IsCheckboxChecked"), Wiz_IsCheckboxChecked, _SC("Wiz::IsCheckboxChecked"));
        BindMethod(v, _SC("FillComboboxWithCompilers"), Wiz_FillComboboxWithCompilers, _SC("Wiz::FillComboboxWithCompilers"));
        BindMethod(v, _SC("GetCompilerFromCombobox"), Wiz_GetCompilerFromCombobox, _SC("Wiz::GetCompilerFromCombobox"));
        BindMethod(v, _SC("FillContainerWithCompilers"), Wiz_FillContainerWithCompilers, _SC("Wiz::FillContainerWithCompilers"));

        // these three are deprecated, the ItemContainer versions should be used instead as they are more generic.
        BindMethod(v, _SC("GetComboboxStringSelection"), Wiz_GetStringFromString<&Wiz::GetComboboxStringSelection>, _SC("Wiz::GetComboboxStringSelection"));
        BindMethod(v, _SC("GetComboboxSelection"), Wiz_GetSelection<&Wiz::GetComboboxSelection>, _SC("Wiz::GetComboboxSelection"));
        BindMethod(v, _SC("SetComboboxSelection"), Wiz_SetSelection<&Wiz::SetComboboxSelection>, _SC("Wiz::SetComboboxSelection"));

        BindMethod(v, _SC("SetComboboxValue"), Wiz_SetComboboxValue, _SC("Wiz::SetComboboxValue"));
        BindMethod(v, _SC("GetComboboxValue"), Wiz_GetStringFromString<&Wiz::GetComboboxValue>, _SC("Wiz::GetComboboxValue"));
        BindMethod(v, _SC("GetItemContainerStringSelection"), Wiz_GetStringFromString<&Wiz::GetComboboxStringSelection>, _SC("Wiz::GetItemContainerStringSelection"));
        BindMethod(v, _SC("GetItemContainerSelection"), Wiz_GetSelection<&Wiz::GetComboboxSelection>, _SC("Wiz::GetItemContainerSelection"));
        BindMethod(v, _SC("SetItemContainerSelection"), Wiz_SetSelection<&Wiz::SetComboboxSelection>, _SC("Wiz::SetItemContainerSelection"));
        BindMethod(v, _SC("GetRadioboxSelection"), Wiz_GetSelection<&Wiz::GetRadioboxSelection>, _SC("Wiz::GetRadioboxSelection"));
        BindMethod(v, _SC("SetRadioboxSelection"), Wiz_SetSelection<&Wiz::SetRadioboxSelection>, _SC("Wiz::SetRadioboxSelection"));
        BindMethod(v, _SC("GetListboxSelection"), Wiz_GetSelection<&Wiz::GetListboxSelection>, _SC("Wiz::GetListboxSelection"));
        BindMethod(v, _SC("GetListboxSelections"), Wiz_GetStringFromString<&Wiz::GetListboxSelections>, _SC("Wiz::GetListboxSelections"));
        BindMethod(v, _SC("GetListboxStringSelections"), Wiz_GetStringFromString<&Wiz::GetListboxStringSelections>, _SC("Wiz::GetListboxStringSelections"));
        BindMethod(v, _SC("SetListboxSelection"), Wiz_SetSelection<&Wiz::SetListboxSelection>, _SC("Wiz::SetListboxSelection"));
        BindMethod(v, _SC("GetCheckListboxChecked"), Wiz_GetStringFromString<&Wiz::GetCheckListboxChecked>, _SC("Wiz::GetCheckListboxChecked"));
        BindMethod(v, _SC("GetCheckListboxStringChecked"), Wiz_GetStringFromString<&Wiz::GetCheckListboxStringChecked>, _SC("Wiz::GetCheckListboxStringChecked"));
        BindMethod(v, _SC("IsCheckListboxItemChecked"), Wiz_IsCheckListboxItemChecked, _SC("Wiz::IsCheckListboxItemChecked"));
        BindMethod(v, _SC("CheckCheckListboxItem"), Wiz_CheckCheckListboxItem, _SC("Wiz::CheckCheckListboxItem"));

        // get various common info
        BindMethod(v, _SC("GetWizardType"), Wiz_GetWizardType, _SC("Wiz::GetWizardType"));
        BindMethod(v, _SC("FindTemplateFile"), Wiz_GetStringFromString<&Wiz::FindTemplateFile>, _SC("Wiz::FindTemplateFile"));

        // project path page
        BindMethod(v, _SC("GetProjectPath"), Wiz_GetString<&Wiz::GetProjectPath>, _SC("Wiz::GetProjectPath"));
        BindMethod(v, _SC("GetProjectName"), Wiz_GetString<&Wiz::GetProjectName>, _SC("Wiz::GetProjectName"));
        BindMethod(v, _SC("GetProjectFullFilename"), Wiz_GetString<&Wiz::GetProjectFullFilename>, _SC("Wiz::GetProjectFullFilename"));
        BindMethod(v, _SC("GetProjectTitle"), Wiz_GetString<&Wiz::GetProjectTitle>, _SC("Wiz::GetProjectTitle"));

        // compiler page
        BindMethod(v, _SC("GetCompilerID"), Wiz_GetString<&Wiz::GetCompilerID>, _SC("Wiz::GetCompilerID"));

        // + debug target
        BindMethod(v, _SC("GetWantDebug"), Wiz_GetBool<&Wiz::GetWantDebug>, _SC("Wiz::GetWantDebug"));
        BindMethod(v, _SC("GetDebugName"), Wiz_GetString<&Wiz::GetDebugName>, _SC("Wiz::GetDebugName"));
        BindMethod(v, _SC("GetDebugOutputDir"), Wiz_GetString<&Wiz::GetDebugOutputDir>, _SC("Wiz::GetDebugOutputDir"));
        BindMethod(v, _SC("GetDebugObjectOutputDir"), Wiz_GetString<&Wiz::GetDebugObjectOutputDir>, _SC("Wiz::GetDebugObjectOutputDir"));

        // + release target
        BindMethod(v, _SC("GetWantRelease"), Wiz_GetBool<&Wiz::GetWantRelease>, _SC("Wiz::GetWantRelease"));
        BindMethod(v, _SC("GetReleaseName"), Wiz_GetString<&Wiz::GetReleaseName>, _SC("Wiz::GetReleaseName"));
        BindMethod(v, _SC("GetReleaseOutputDir"), Wiz_GetString<&Wiz::GetReleaseOutputDir>, _SC("Wiz::GetReleaseOutputDir"));
        BindMethod(v, _SC("GetReleaseObjectOutputDir"), Wiz_GetString<&Wiz::GetReleaseObjectOutputDir>, _SC("Wiz::GetReleaseObjectOutputDir"));

        // build target page
        BindMethod(v, _SC("GetTargetCompilerID"), Wiz_GetString<&Wiz::GetTargetCompilerID>, _SC("Wiz::GetTargetCompilerID"));
        BindMethod(v, _SC("GetTargetEnableDebug"), Wiz_GetBool<&Wiz::GetTargetEnableDebug>, _SC("Wiz::GetTargetEnableDebug"));
        BindMethod(v, _SC("GetTargetName"), Wiz_GetString<&Wiz::GetTargetName>, _SC("Wiz::GetTargetName"));
        BindMethod(v, _SC("GetTargetOutputDir"), Wiz_GetString<&Wiz::GetTargetOutputDir>, _SC("Wiz::GetTargetOutputDir"));
        BindMethod(v, _SC("GetTargetObjectOutputDir"), Wiz_GetString<&Wiz::GetTargetObjectOutputDir>, _SC("Wiz::GetTargetObjectOutputDir"));

        // file path page
        BindMethod(v, _SC("GetFileName"), Wiz_GetString<&Wiz::GetFileName>, _SC("Wiz::GetFileName"));
        BindMethod(v, _SC("GetFileHeaderGuard"), Wiz_GetString<&Wiz::GetFileHeaderGuard>, _SC("Wiz::GetFileHeaderGuard"));
        BindMethod(v, _SC("GetFileAddToProject"), Wiz_GetBool<&Wiz::GetFileAddToProject>, _SC("Wiz::GetFileAddToProject"));
        BindMethod(v, _SC("GetFileTargetIndex"), Wiz_GetFileTargetIndex, _SC("Wiz::GetFileTargetIndex"));
        BindMethod(v, _SC("SetFilePathSelectionFilter"), Wiz_SetFilePathSelectionFilter, _SC("Wiz::SetFilePathSelectionFilter"));

        // Fill the named window with compilers matching a mask/filter
        BindMethod(v, _SC("FillContainerWithSelectCompilers"), Wiz_ContainerWithSelectCompilers<&Wiz::FillContainerWithSelectCompilers>, _SC("Wiz::FillContainerWithSelectCompilers"));

        // Add to the named window the compilers matching a mask/filter
        BindMethod(v, _SC("AppendContainerWithSelectCompilers"), Wiz_ContainerWithSelectCompilers<&Wiz::AppendContainerWithSelectCompilers>, _SC("Wiz::AppendContainerWithSelectCompilers"));

        // Fill the named window with a semi-colon separated set of strings
        BindMethod(v, _SC("FillContainerWithChoices"), Wiz_ContainerWithChoices<&Wiz::FillContainerWithChoices>, _SC("Wiz::FillContainerWithChoices"));

        // Add to the named window, a semi-colon separated set of strings
        BindMethod(v, _SC("AppendContainerWithChoices"), Wiz_ContainerWithChoices<&Wiz::AppendContainerWithChoices>, _SC("Wiz::AppendContainerWithChoices"));

        // Return the name only of the current wizard folder (this is not a path)
        BindMethod(v, _SC("GetWizardScriptFolder"), Wiz_GetString<&Wiz::GetWizardScriptFolder>, _SC("Wiz::GetWizardScriptFolder"));

        BindDefaultInstanceCmp<FileTreeData>(v);
        // Put the class in the root table. This must be last!
        sq_newslot(v, classDecl, SQFalse);
    }

    {
        sq_pushstring(v, _SC("Wizard"), -1);
        // Setup global variable Wizard
        if (!CreateNonOwnedPtrInstance(v, this))
            cbAssert(false);
        sq_newslot(v, -3, SQFalse);
    }

    sq_poptop(v); // Pop root table
}

////////////////////////
// Scripting - END
////////////////////////
