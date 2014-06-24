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
    #include <manager.h>
    #include <projectbuildtarget.h>
    #include <projectmanager.h>
    #include <scriptingmanager.h>
#endif // CB_PRECOMP
#include <scripting/bindings/sc_base_types.h>

#include "wiz.h"
#include "wizpage.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(Wizards); // TODO: find out why this causes a shadow warning for 'Item'

namespace
{
    PluginRegistrant<Wiz> reg(_T("ScriptedWizard"));
}

// scripting support
DECLARE_INSTANCE_TYPE(Wiz);

Wiz::Wiz()
    : m_pWizard(0),
    m_pWizProjectPathPanel(0),
    m_pWizFilePathPanel(0),
    m_pWizCompilerPanel(0),
    m_pWizBuildTargetPanel(0),
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

    // user script first
    wxString templatePath = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/");
    wxString script = templatePath + _T("/config.script");
    if (wxFileExists(script))
    {
        Manager::Get()->GetScriptingManager()->LoadScript(script);
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
    else
    {
        // global script next
        templatePath = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/");
        script = templatePath + _T("/config.script");
        if (wxFileExists(script))
        {
            Manager::Get()->GetScriptingManager()->LoadScript(script);
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
    m_pWizFilePathPanel = 0;
}

CompileTargetBase* Wiz::Launch(int index, wxString* pFilename)
{
    cbAssert(index >= 0 && index < GetCount());

    // clear previous script's context
    static const wxString clearout_wizscripts =  _T("function BeginWizard(){};\n"
                                                    "function SetupProject(project){return false;};\n"
                                                    "function SetupTarget(target,is_debug){return false;};\n"
                                                    "function SetupCustom(){return false;};\n"
                                                    "function CreateFiles(){return _T(\"\");};\n"
                                                    "function GetFilesDir(){return _T(\"\");};\n"
                                                    "function GetGeneratedFile(index){return _T(\"\");};\n");
    Manager::Get()->GetScriptingManager()->LoadBuffer(clearout_wizscripts, _T("ClearWizState"));

    // early check: build target wizards need an active project
    if (m_Wizards[index].output_type == totTarget &&
        !Manager::Get()->GetProjectManager()->GetActiveProject())
    {
        cbMessageBox(_("You need to open (or create) a project first!"), _("Error"), wxICON_ERROR);
        return 0;
    }

    m_LaunchIndex = index;

    wxString global_commons = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/common_functions.script");
    wxString user_commons = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/common_functions.script");

    m_LastXRC = m_Wizards[index].xrc;
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
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    if (!Manager::Get()->GetScriptingManager()->LoadScript(global_commons) && // load global common functions
        !Manager::Get()->GetScriptingManager()->LoadScript(user_commons)) // and/or load user common functions
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        InfoWindow::Display(_("Error"), _("Failed to load the common functions script.\nPlease check the debug log for details..."));
        return 0;
    }

    // locate the script
    wxString script = ConfigManager::GetFolder(sdDataUser) + _T("/templates/wizard/") + m_Wizards[index].script;
    if (!wxFileExists(script))
        script = ConfigManager::GetFolder(sdDataGlobal) + _T("/templates/wizard/") + m_Wizards[index].script;

    if (!Manager::Get()->GetScriptingManager()->LoadScript(script)) // build and run script
    {
        // any errors have been displayed by ScriptingManager
        Clear();
        InfoWindow::Display(_("Error"), _("Failed to load the wizard's script.\nPlease check the debug log for details..."));
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
    catch (cbException& e)
    {
        e.ShowErrorMessage(false);
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
    wxString srcdir;
    try
    {
        SqPlus::SquirrelFunction<wxString&> f("GetFilesDir");
        if (!f.func.IsNull())
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

    // add generated files
    try
    {
        SqPlus::SquirrelFunction<wxString&> f("GetGeneratedFile");
        if (!f.func.IsNull())
        {
            wxArrayString files;
            wxArrayString contents;
            int idx = 0;
            // safety limit to avoid infinite loops because of badly written scripts: 50 files
            while (idx < 50)
            {
                wxString fileAndContents = f(idx++);
                if (fileAndContents.IsEmpty())
                    break;
                wxString tmpFile = fileAndContents.BeforeFirst(_T(';'));
                wxString tmpContents = fileAndContents.AfterFirst(_T(';'));
                tmpFile.Trim();
                tmpContents.Trim();
                if (tmpFile.IsEmpty() || tmpContents.IsEmpty())
                    break;
                files.Add(tmpFile);
                contents.Add(tmpContents);
            };

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
    Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
    Manager::Get()->GetProjectManager()->GetUI().GetTree()->Expand(theproject->GetProjectNode());
    return theproject;
}

CompileTargetBase* Wiz::RunTargetWizard(cb_unused wxString* pFilename)
{
    cbProject* theproject = Manager::Get()->GetProjectManager()->GetActiveProject(); // can't fail; if no project, the wizard didn't even run
    ProjectBuildTarget* target = theproject->AddBuildTarget(GetTargetName());
    if (!target)
    {
        cbMessageBox(_("Failed to create build target!"), _("Error"), wxICON_ERROR);
        Clear();
        return 0;
    }

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
    // Assign this target to all project files
    for (FilesList::iterator it = theproject->GetFilesList().begin(); it != theproject->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        if (pf)
            pf->AddBuildTarget(GetTargetName());
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

CompileTargetBase* Wiz::RunCustomWizard(cb_unused wxString* pFilename)
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
          _T("The wizard is about OVERWRITE the following existing file:\n")+
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
              _T("The wizard is about OVERWRITE the following existing file:\n")+
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
        m_pWizFilePathPanel = 0;
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
        m_pWizProjectPathPanel = 0;
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
        m_pWizCompilerPanel = 0;
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
        m_pWizBuildTargetPanel = 0;
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

void Wiz::RegisterWizard()
{
    SqPlus::SQClassDef<Wiz>("Wiz").
            // register new wizards
            func(&Wiz::AddWizard, "AddWizard").
            // add wizard pages
            func(&Wiz::AddInfoPage, "AddInfoPage").
            func(&Wiz::AddProjectPathPage, "AddProjectPathPage").
            func(&Wiz::AddFilePathPage, "AddFilePathPage").
            func(&Wiz::AddCompilerPage, "AddCompilerPage").
            func(&Wiz::AddBuildTargetPage, "AddBuildTargetPage").
            func(&Wiz::AddGenericSingleChoiceListPage, "AddGenericSingleChoiceListPage").
            func(&Wiz::AddGenericSelectPathPage, "AddGenericSelectPathPage").
            func(&Wiz::AddPage, "AddPage").
            // compiler defaults
            func(&Wiz::SetCompilerDefault, "SetCompilerDefault").
            func(&Wiz::SetDebugTargetDefaults, "SetDebugTargetDefaults").
            func(&Wiz::SetReleaseTargetDefaults, "SetReleaseTargetDefaults").
            // GUI controls
            func(&Wiz::EnableWindow, "EnableWindow").
            func(&Wiz::SetTextControlValue, "SetTextControlValue").
            func(&Wiz::GetTextControlValue, "GetTextControlValue").
            func(&Wiz::SetSpinControlValue, "SetSpinControlValue").
            func(&Wiz::GetSpinControlValue, "GetSpinControlValue").
            func(&Wiz::CheckCheckbox, "CheckCheckbox").
            func(&Wiz::IsCheckboxChecked, "IsCheckboxChecked").
            func(&Wiz::FillComboboxWithCompilers, "FillComboboxWithCompilers").
            func(&Wiz::GetCompilerFromCombobox, "GetCompilerFromCombobox").
            func(&Wiz::FillContainerWithCompilers, "FillContainerWithCompilers").
            // these three are deprecated, the ItemContainer versions should be used instead as they are more generic.
            func(&Wiz::GetComboboxStringSelection, "GetComboboxStringSelection").
            func(&Wiz::GetComboboxSelection, "GetComboboxSelection").
            func(&Wiz::SetComboboxSelection, "SetComboboxSelection").
            func(&Wiz::GetComboboxStringSelection, "GetItemContainerStringSelection").
            func(&Wiz::GetComboboxSelection, "GetItemContainerSelection").
            func(&Wiz::SetComboboxSelection, "SetItemContainerSelection").
            func(&Wiz::GetRadioboxSelection, "GetRadioboxSelection").
            func(&Wiz::SetRadioboxSelection, "SetRadioboxSelection").
            func(&Wiz::GetListboxSelection, "GetListboxSelection").
            func(&Wiz::GetListboxSelections, "GetListboxSelections").
            func(&Wiz::GetListboxStringSelections, "GetListboxStringSelections").
            func(&Wiz::SetListboxSelection, "SetListboxSelection").
            func(&Wiz::GetCheckListboxChecked, "GetCheckListboxChecked").
            func(&Wiz::GetCheckListboxStringChecked, "GetCheckListboxStringChecked").
            func(&Wiz::IsCheckListboxItemChecked, "IsCheckListboxItemChecked").
            func(&Wiz::CheckCheckListboxItem, "CheckCheckListboxItem").
            // get various common info
            func(&Wiz::GetWizardType, "GetWizardType").
            func(&Wiz::FindTemplateFile, "FindTemplateFile").
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
