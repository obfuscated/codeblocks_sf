/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef wxUSE_CHOICEDLG
    #define wxUSE_CHOICEDLG 1
#endif

#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

#ifndef CB_PRECOMP
    #include <wx/dir.h>

    #include "cbproject.h" // class's header file
    #include "cbeditor.h"
    #include "cbtreectrl.h"
    #include "compiler.h" // GetSwitches
    #include "compilerfactory.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "filemanager.h"
    #include "globals.h"
    #include "infowindow.h"
    #include "logmanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "pluginmanager.h"
    #include "projectbuildtarget.h"
    #include "projectfile.h"
    #include "projectmanager.h"
    #include "sdk_events.h"
#endif

#include <map>
#include "projectloader.h"
#include "projectlayoutloader.h"
#include "selecttargetdlg.h"
#include "filegroupsandmasks.h"
#include "filefilters.h"
#include "annoyingdialog.h"
#include "genericmultilinenotesdlg.h"
#include "compilercommandgenerator.h"
#include "cbcolourmanager.h"

// class constructor
cbProject::cbProject(const wxString& filename) :
    m_CustomMakefile(false),
    m_Globs(),
    m_FileArray(ProjectFile::CompareProjectFiles),
    m_Loaded(false),
    m_CurrentlyLoading(false),
    m_PCHMode(pchSourceFile),
    m_CurrentlyCompilingTarget(nullptr),
    m_ExtendedObjectNamesGeneration(false),
    m_AutoShowNotesOnLoad(false),
    m_CheckForExternallyModifiedFiles(true),
    m_pExtensionsElement(nullptr)
{
    SetCompilerID(CompilerFactory::GetDefaultCompilerID());
    SetModified(false);

    wxString realFile = realpath(filename);

    m_Files.clear();
    m_FileArray.Clear();
    if (!realFile.IsEmpty() && (wxFileExists(realFile) || wxDirExists(realFile)))
    {
        // existing project
        m_Filename = realFile;
        m_BasePath = GetBasePath();
        Open();
    }
    else
    {
        // new project
        SetModified(true);
        if (realFile.IsEmpty())
        {
            m_Filename = CreateUniqueFilename();
            m_Loaded   = SaveAs();
        }
        else
        {
            m_Filename = realFile;
            m_Loaded   = Save();
        }
        if (m_Loaded)
        {
            wxFileName fname(m_Filename);
            m_Title              = fname.GetName();
            m_BasePath           = GetBasePath();
            m_CommonTopLevelPath = GetBasePath() + wxFileName::GetPathSeparator();

            // moved to ProjectManager::LoadProject()
            // see explanation there...
//            NotifyPlugins(cbEVT_PROJECT_OPEN);
        }
    }
}

// class destructor
cbProject::~cbProject()
{
    // moved to ProjectManager::CloseProject()
    // see explanation there...
//    NotifyPlugins(cbEVT_PROJECT_CLOSE);

    ClearAllProperties();
}

void cbProject::NotifyPlugins(wxEventType type, const wxString& targetName, const wxString& oldTargetName)
{
    CodeBlocksEvent event(type);
    event.SetProject(this);
    event.SetBuildTargetName(targetName);
    event.SetOldBuildTargetName(oldTargetName);
    Manager::Get()->ProcessEvent(event);
}

void cbProject::SetCompilerID(const wxString& id)
{
// TODO (mandrav##): Is this needed? The project's compiler has nothing to do with the targets' compilers...

    CompileTargetBase::SetCompilerID(id);
    if (id != GetCompilerID())
    {
        // update object filenames
        for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = m_Targets[i];
            if (target)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
                if (!compiler)
                    continue;

                for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
                {
                    ProjectFile* pf = *it;
                    wxFileName obj(pf->GetObjName());
                    if (   (FileTypeOf(pf->relativeFilename) != ftResource)
                        && (obj.GetExt() == compiler->GetSwitches().objectExtension) )
                    {
                        obj.SetExt(compiler->GetSwitches().objectExtension);
                        pf->SetObjName(obj.GetFullName());
                    }
                }
            }
        }
    }
}

bool cbProject::GetModified() const
{
    // check base options
    if (CompileOptionsBase::GetModified())
        return true;

    // check targets
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target->GetModified())
            return true;
    }

    return false;
}

void cbProject::SetModified(bool modified)
{
    CompileOptionsBase::SetModified(modified);

    // modify targets
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        target->SetModified(modified);
    }

    if (!modified)
        m_LastSavedActiveTarget = m_ActiveTarget;
}

void cbProject::SetMakefileCustom(bool custom)
{
    if (m_CustomMakefile != custom)
    {
        m_CustomMakefile = custom;
        SetModified(true);
    }
}

wxString cbProject::CreateUniqueFilename()
{
    const wxString prefix = _("Untitled");
    wxString tmp;
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    int projCount = arr->GetCount();
    int iter = 1;
    bool ok = false;
    tmp << prefix << wxString::Format(_T("%d"), iter);
    while (!ok)
    {
        tmp.Clear();
        tmp << prefix << wxString::Format(_T("%d"), iter);

        ok = true;
        for (int i = 0; i < projCount; ++i)
        {
            cbProject* prj = arr->Item(i);
            wxFileName fname(prj->GetFilename());

            if (fname.GetName().Matches(tmp))
            {
                ok = false;
                break;
            }
        }
        if (ok)
            break;
        ++iter;
    }
    return tmp << _T(".") << FileFilters::CODEBLOCKS_EXT;
}

void cbProject::ClearAllProperties()
{
    Delete(m_pExtensionsElement);

    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end();++it)
        delete(*it);
    m_Files.clear();
    m_FileArray.Clear();
    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_IncludeDirs.Clear();
    m_LibDirs.Clear();

    while (m_Targets.GetCount())
    {
        ProjectBuildTarget* target = m_Targets[0];
        delete target;
        m_Targets.RemoveAt(0);
    }
    SetModified(true);

    NotifyPlugins(cbEVT_BUILDTARGET_SELECTED);
}

void cbProject::Open()
{
    m_Loaded = false;
    m_ProjectFilesMap.clear();
    Delete(m_pExtensionsElement);

    if (!wxFileName::FileExists(m_Filename) && !wxFileName::DirExists(m_Filename))
    {
        wxString msg;
        msg.Printf(_("Project '%s' does not exist..."), m_Filename.c_str());
        cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

    bool fileUpgraded = false;
    bool fileModified = false;
    wxFileName fname(m_Filename);
    FileType ft = FileTypeOf(m_Filename);
    if (ft == ftCodeBlocksProject)
    {
        Manager::Get()->GetLogManager()->Log(_("Opening ") + m_Filename);
        m_CurrentlyLoading = true;
        ProjectLoader loader(this);
        m_Loaded = loader.Open(m_Filename, &m_pExtensionsElement);
        fileUpgraded = loader.FileUpgraded();
        fileModified = loader.FileModified();
        m_CurrentlyLoading = false;

        if (m_Loaded)
        {
            CalculateCommonTopLevelPath();
            Manager::Get()->GetLogManager()->Log(_("Done."));
            if (!m_Targets.GetCount())
                AddDefaultBuildTarget();
            // in case of batch build discard upgrade messages
            fileUpgraded = fileUpgraded && !Manager::IsBatchBuild();
            SetModified(ft != ftCodeBlocksProject || fileUpgraded || fileModified);

            // moved to ProjectManager::LoadProject()
            // see explanation there...
    //        NotifyPlugins(cbEVT_PROJECT_OPEN);

            if (fileUpgraded)
            {
                InfoWindow::Display(m_Title,
                  _("The loaded project file was generated\n"
                    "with an older version of Code::Blocks.\n\n"
                    "Code::Blocks can import older project files,\n"
                    "but will always save in the current format."), 12000, 2000);
            }
            m_LastModified = fname.GetModificationTime();
        }
    }
} // end of Open

void cbProject::CalculateCommonTopLevelPath()
{
    // find the common toplevel path
    // for simple projects, this might be the path to the project file
    // for projects where the project file is in a subdir, files will
    // have ".." in their paths
    wxString   sep            = wxFileName::GetPathSeparator();
    wxFileName base           = GetBasePath() + sep;
    wxString   vol            = base.GetVolume();
    bool       prjHasUNCName  = base.GetFullPath().StartsWith(_T("\\\\"));
    bool       fileHasUNCName = false;

    Manager::Get()->GetLogManager()->DebugLog(_T("Project's base path: ") + base.GetFullPath());

    // This loop takes ~30ms for 1000 project files
    // it's as fast as it can get, considered that it used to take ~1200ms ;)
    // don't even bother making it faster - you can't :)
#ifdef ctlp_measuring
    wxStopWatch sw;
#endif
    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
    {
        ProjectFile* f = (*it);
        if (!f)
            continue;

        if ( !vol.IsSameAs(f->file.GetVolume()) )
            continue;

        fileHasUNCName = f->file.GetFullPath().StartsWith(_T("\\\\"));

        if (   (!prjHasUNCName &&  fileHasUNCName)
            || ( prjHasUNCName && !fileHasUNCName) )
        {
            continue;
        }

        wxString tmp     = f->relativeFilename;
        wxString tmpbase = m_BasePath;

        size_t pos = 0;
        while (   (pos < tmp.Length())
               && (   (tmp.GetChar(pos) == _T('.'))
                   || (tmp.GetChar(pos) == _T('/'))
                   || (tmp.GetChar(pos) == _T('\\')) ) )
        {
            ++pos;
        }
        if ( (pos > 0) && (pos < tmp.Length()) )
            tmpbase << sep << tmp.Left(pos) << sep;

        wxFileName tmpbaseF(tmpbase); tmpbaseF.Normalize(wxPATH_NORM_DOTS);
        if (   (tmpbaseF.GetDirCount() < base.GetDirCount())
            && (base.GetPath().StartsWith(tmpbaseF.GetPath())) )
        {
            base = tmpbaseF;
        }
    }
#ifdef ctlp_measuring
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("%s::%s:%d  took : %d ms"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__, (int)sw.Time()));
#endif

    m_CommonTopLevelPath = base.GetFullPath();
    Manager::Get()->GetLogManager()->DebugLog(_T("Project's common toplevel path: ") + m_CommonTopLevelPath);

    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
    {
        ProjectFile* f = (*it);
        if (!f)
            continue;

        wxString fileName = f->file.GetFullPath();
        fileHasUNCName = fileName.StartsWith(_T("\\\\"));

        if (   (prjHasUNCName && fileHasUNCName)
            || (   !prjHasUNCName
                && !fileHasUNCName
                && vol.IsSameAs(f->file.GetVolume()) ) )
        {
            wxFileName relFileCTLP(f->file);
            relFileCTLP.MakeRelativeTo( m_CommonTopLevelPath );
            wxFileName relFileBase(f->file);
            relFileBase.MakeRelativeTo( GetBasePath() );

            // The commented (old) method to obtain the relativeToCommonTopLevelPath is fast, but does *not* work, if you save
            // the project on a different drive in a sub-folder of an existing source file on that (different) drive:
            // I.e.: Project on C:\Folder\Project.cbp has file C:\Folder\SubFolder\foo.cpp and D:\Folder\bar.cpp
            // Saved the project under D:\Folder\SubFolder\ProjectNew.cbp would cause a wrong computation of bar.cpp otherwise!!!
//            f->relativeToCommonTopLevelPath = fileName.Right(fileName.Length() - m_CommonTopLevelPath.Length());
            // Using wxFileName instead, although its costly:
            f->relativeToCommonTopLevelPath = relFileCTLP.GetFullPath();
            f->relativeFilename             = relFileBase.GetFullPath();
        }
        else
        {
            f->relativeToCommonTopLevelPath = fileName;
            f->relativeFilename             = fileName;
        }

        f->SetObjName(f->relativeToCommonTopLevelPath);
    }
}

wxString cbProject::GetCommonTopLevelPath() const
{
    return m_CommonTopLevelPath;
}

void cbProject::Touch()
{
    m_LastModified = wxDateTime::Now();
}

bool cbProject::SaveAs()
{
    wxFileName fname;
    fname.Assign(m_Filename);
    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                     _("Save file"),
                     fname.GetPath(),
                     fname.GetFullName(),
                     FileFilters::GetFilterString(_T('.') + FileFilters::CODEBLOCKS_EXT),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return false;

    wxFileName newName(dlg.GetPath());

    // if the filename has changed, we need to recalculate the common toplevel path
    bool pathChanged = !newName.GetPath().IsSameAs(fname.GetPath());

    m_Filename = newName.GetFullPath();
    fname.Assign(m_Filename);

    // make sure the project file uses the correct extension
    // we don't use wxFileName::SetExt() because if the user has added a dot
    // in the filename, the part after it would be interpreted as extension
    // (and it might not be)
    // so we just append the correct extension
    if (!fname.GetExt().Matches(FileFilters::CODEBLOCKS_EXT))
        fname.Assign(m_Filename + _T('.') + FileFilters::CODEBLOCKS_EXT);

//    Manager::Get()->GetProjectManager()->GetTree()->SetItemText(m_ProjectNode, fname.GetFullName());
    if (!m_Loaded)
        AddDefaultBuildTarget();
    if (pathChanged)
        CalculateCommonTopLevelPath();
    ProjectLoader loader(this);
    if (loader.Save(m_Filename, m_pExtensionsElement))
    {
        fname = m_Filename;
        m_LastModified = fname.GetModificationTime();
        NotifyPlugins(cbEVT_PROJECT_SAVE);
        return true;
    }

    cbMessageBox(_("Couldn't save project ") + m_Filename + _("\n(Maybe the file is write-protected?)"), _("Warning"), wxICON_WARNING);
    return false;
}

bool cbProject::Save()
{
    if (m_Filename.IsEmpty())
        return SaveAs();
    ProjectLoader loader(this);
    if (loader.Save(m_Filename, m_pExtensionsElement))
    {
        wxFileName fname(m_Filename);
        m_LastModified = fname.GetModificationTime();
        NotifyPlugins(cbEVT_PROJECT_SAVE);
        return true;
    }

    cbMessageBox(_("Couldn't save project ") + m_Filename + _("\n(Maybe the file is write-protected?)"), _("Warning"), wxICON_WARNING);
    return false;
}

bool cbProject::SaveLayout()
{
    if (m_Filename.IsEmpty())
        return false;

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/enable_project_layout"), true) == false)
        return true;

    wxFileName fname(m_Filename);
    fname.SetExt(_T("layout"));
    ProjectLayoutLoader loader(this);
    return loader.Save(fname.GetFullPath());
}

bool cbProject::LoadLayout()
{
    if (m_Filename.IsEmpty())
        return false;

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/enable_project_layout"), true) == false)
        return true;

    int openmode = Manager::Get()->GetConfigManager(_T("project_manager"))->ReadInt(_T("/open_files"), (long int)1);
    if (openmode==2)
        return true; // Do not open any files

    Manager::Get()->GetEditorManager()->HideNotebook();

    bool result = false;
    if (openmode == 0) // Open all files
    {
        FilesList::iterator it = m_Files.begin();
        while (it != m_Files.end())
        {
            ProjectFile* f = *it++;
            Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath(),0,f);
        }
        result = true;
    }
    else if (openmode == 1)// Open last open files
    {
        wxFileName fname(m_Filename);
        fname.SetExt(_T("layout"));
        ProjectLayoutLoader loader(this);
        if (loader.Open(fname.GetFullPath()))
        {
            typedef std::map<int, ProjectFile*> open_files_map;
            open_files_map open_files;

            // Get all files to open and sort them according to their tab-position:
            FilesList::iterator it = m_Files.begin();
            while (it != m_Files.end())
            {
                ProjectFile* f = *it++;
                // do not try to open files that do not exist, but have fileOpen set to true
                if (f->editorOpen && wxFileExists(f->file.GetFullPath()))
                    open_files[f->editorTabPos] = f;
                else
                    f->editorOpen = false;
            }

            // Load all requested files
            std::vector<LoaderBase*> filesInMemory;
            for (open_files_map::iterator ofm_it = open_files.begin(); ofm_it != open_files.end(); ++ofm_it)
                filesInMemory.push_back(Manager::Get()->GetFileManager()->Load((*ofm_it).second->file.GetFullPath()));

            // Open all requested files:
            size_t i = 0;
            for (open_files_map::iterator ofm_it = open_files.begin(); ofm_it != open_files.end(); ++ofm_it)
            {
                cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filesInMemory[i], (*ofm_it).second->file.GetFullPath(),0,(*ofm_it).second);
                if (ed)
                    ed->SetProjectFile((*ofm_it).second);
                ++i;
            }

            ProjectFile* f = loader.GetTopProjectFile();
            if (f)
            {
                Manager::Get()->GetLogManager()->DebugLog(_T("Top Editor: ") + f->file.GetFullPath());
                EditorBase* eb = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
                if (eb)
                    eb->Activate();
            }
            loader.LoadNotebookLayout();
        }
        result = true;
    }

    Manager::Get()->GetEditorManager()->ShowNotebook();

    return result;
}

void cbProject::BeginAddFiles()
{
    CodeBlocksEvent event(cbEVT_PROJECT_BEGIN_ADD_FILES);
    event.SetProject(this);
    Manager::Get()->ProcessEvent(event);
}

void cbProject::EndAddFiles()
{
    CodeBlocksEvent event(cbEVT_PROJECT_END_ADD_FILES);
    event.SetProject(this);
    Manager::Get()->ProcessEvent(event);
}

void cbProject::BeginRemoveFiles()
{
    CodeBlocksEvent event(cbEVT_PROJECT_BEGIN_REMOVE_FILES);
    event.SetProject(this);
    Manager::Get()->ProcessEvent(event);
}

void cbProject::EndRemoveFiles()
{
    CodeBlocksEvent event(cbEVT_PROJECT_END_REMOVE_FILES);
    event.SetProject(this);
    Manager::Get()->ProcessEvent(event);
}

ProjectFile* cbProject::AddFile(const wxString& targetName, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
    int idx = IndexOfBuildTargetName(targetName);
    return AddFile(idx, filename, compile, link, weight);
}

ProjectFile* cbProject::AddFile(int targetIndex, const wxString& filename, bool compile, bool link, cb_unused unsigned short int weight)
{
//  NOTE (Rick#1#): When loading the project, do not search for existing files
//  (Assuming that there are no duplicate entries in the .cbp file)
//  This saves us a lot of processing when loading large projects.
//  Remove the if to do the search anyway

//  NOTE (mandrav#1#): We can't ignore that because even if we can rely on .cbp
//  containing discrete files, we can't do that for imported projects...
//  This means we have to search anyway.
//  NP though, I added a hashmap for fast searches in GetFileByFilename()

/* NOTE (mandrav#1#): Calling GetFileByFilename() twice, is costly.
    Instead of searching for duplicate files when entering here,
    we 'll search before exiting.
    The rationale is that by then, we 'll have the relative filename
    in our own representation and this will make everything quicker
    (check GetFileByFilename implementation to understand why)...
*/
//    f = GetFileByFilename(filename, true, true);
//    if (!f)
//        f = GetFileByFilename(filename, false, true);
//    if (f)
//    {
//        if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
//            f->AddBuildTarget(m_Targets[targetIndex]->GetTitle());
//        return f;
//    }

    // quick test
    ProjectFile* pf = m_ProjectFilesMap[UnixFilename(filename)];
    if (pf)
        return pf;

    // create file
    pf = new ProjectFile(this);
    bool localCompile, localLink;
    wxFileName fname(filename);
    wxString ext;

    ext = filename.AfterLast(_T('.')).Lower();
    if (ext.IsSameAs(FileFilters::C_EXT))
        pf->compilerVar = _T("CC");
    else if (platform::windows && ext.IsSameAs(FileFilters::RESOURCE_EXT))
        pf->compilerVar = _T("WINDRES");
    else
        pf->compilerVar = _T("CPP"); // default

    if (!m_Targets.GetCount())
    {
        // no targets in project; add default
        AddDefaultBuildTarget();
        if (!m_Targets.GetCount())
        {
            delete pf;
            return nullptr; // if that failed, fail addition of file...
        }
    }

    bool isResource = FileTypeOf(filename) == ftResource;

// NOTE (mandrav#1#): targetIndex == -1 means "don't add file to any targets"
// This case gives us problems though because then we don't know the compiler
// this file will be using (per-target) which means we can't decide if it
// generates any files...
// We solve this issue with a hack (only for the targetIndex == -1 case!):
// We iterate all available target compilers tool and use generatedFiles from
// all of them. It works and is also safe.
    std::map<Compiler*, const CompilerTool*> GenFilesHackMap;
    if (targetIndex < 0 || targetIndex >= (int)m_Targets.GetCount())
    {
        Compiler* c = CompilerFactory::GetCompiler( GetCompilerID() );
        if (c)
        {
            const CompilerTool* t = c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
            if (t && t->generatedFiles.GetCount())
                GenFilesHackMap[c] = t;
        }

        for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
        {
            c = CompilerFactory::GetCompiler(m_Targets[i]->GetCompilerID());
            if (GenFilesHackMap.find(c) != GenFilesHackMap.end())
                continue; // compiler already in map

            if (c)
            {
                const CompilerTool* t = c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
                if (t && t->generatedFiles.GetCount())
                    GenFilesHackMap[c] = t;
            }
        }
    }
    else
    {
        // targetIndex is valid: just add a single entry to the map
        Compiler* c = CompilerFactory::GetCompiler(m_Targets[targetIndex]->GetCompilerID());
        if (c)
        {
            const CompilerTool* t = c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
            if (t && t->generatedFiles.GetCount())
                GenFilesHackMap[c] = t;
        }
    }

    // so... now, if GenFilesHackMap is not empty, we know
    // 1) this file generates other files and
    // 2) iterating the map will give us the generated file names :)

    // add the build target
    if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
        pf->AddBuildTarget(m_Targets[targetIndex]->GetTitle());

    FileType ft = FileTypeOf(filename);
    localCompile =    compile
                   && (   ft == ftSource
                       || ft == ftResource
                       || !GenFilesHackMap.empty() );
    localLink =    link
                && (   ft == ftSource
                    || ft == ftResource
                    || ft == ftObject
                    || ft == ftResourceBin
                    || ft == ftStaticLib );

    pf->compile = localCompile;
    pf->link    = localLink;

    wxString local_filename = filename;

#ifdef __WXMSW__
    // for Windows, make sure the filename is not on another drive...
    if (   (local_filename.Length() > 1)
        && (local_filename.GetChar(1) == _T(':'))
        && (fname.GetVolume() != wxFileName(m_Filename).GetVolume()) )
        // (this is a quick test to avoid the costly wxFileName ctor below)
    {
        fname.Assign(filename);
    }
    else if (fname.GetFullPath().StartsWith(_T("\\\\"))) // UNC path
    {
        fname.Assign(filename);
    }
    else
#endif
    {
        // make sure the filename is relative to the project's base path
        if (fname.IsAbsolute())
        {
            fname.MakeRelativeTo( GetBasePath() );
            local_filename = fname.GetFullPath();
        }
        // this call is costly (wxFileName ctor):
        fname.Assign(GetBasePath() + wxFILE_SEP_PATH + local_filename);
    }
    fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, GetBasePath());

    wxString fullFilename = realpath(fname.GetFullPath());
    pf->file              = fullFilename;
    pf->relativeFilename  = UnixFilename(local_filename);

    // now check if we have already added this file
    // if we have, return the existing file, but add the specified target
    ProjectFile* existing = GetFileByFilename(pf->relativeFilename, true, true);
    if (existing == pf)
    {
        delete pf;
        if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
            existing->AddBuildTarget(m_Targets[targetIndex]->GetTitle());
        return existing;
    }

    m_Files.insert(pf);
    if (!m_CurrentlyLoading)
    {
        // Only add the file, if we are not currently loading the project and
        // m_FileArray is already initialised.
        // Initialising is done in the getter-function (GetFile(index), to save time,
        // because in many cases m_FileArray is not needed
        if ( m_FileArray.GetCount() > 0 )
            m_FileArray.Add(pf);
        // check if we really need to recalculate the common top-level path for the project
        if ( !fullFilename.StartsWith(m_CommonTopLevelPath) )
            CalculateCommonTopLevelPath();
        else
        {
            // set f->relativeToCommonTopLevelPath
            pf->relativeToCommonTopLevelPath = fullFilename.Right(fullFilename.Length() - m_CommonTopLevelPath.Length());
        }
    }
    SetModified(true);
    m_ProjectFilesMap[UnixFilename(pf->relativeFilename)] = pf; // add to hashmap

    if (!wxFileExists(fullFilename))
        pf->SetFileState(fvsMissing);
    else if (!wxFile::Access(fullFilename.c_str(), wxFile::write)) // readonly
        pf->SetFileState(fvsReadOnly);

    if ( !GenFilesHackMap.empty() )
    {
        // auto-generated files!
        wxFileName tmp = pf->file;
        for (std::map<Compiler*, const CompilerTool*>::const_iterator it = GenFilesHackMap.begin(); it != GenFilesHackMap.end(); ++it)
        {
            const CompilerTool* tool = it->second;
            for (size_t i = 0; i < tool->generatedFiles.GetCount(); ++i)
            {
                tmp.SetFullName(tool->generatedFiles[i]);
                wxString tmps = tmp.GetFullPath();
                // any macro replacements here, should also be done in
                // CompilerCommandGenerator::GenerateCommandLine !!!
                tmps.Replace(_T("$file_basename"), pf->file.GetName()); // old way - remove later
                tmps.Replace(_T("$file_name"),     pf->file.GetName());
                tmps.Replace(_T("$file_dir"),      pf->file.GetPath());
                tmps.Replace(_T("$file_ext"),      pf->file.GetExt());
                tmps.Replace(_T("$file"),          pf->file.GetFullName());
                Manager::Get()->GetMacrosManager()->ReplaceMacros(tmps);

                ProjectFile* pfile = AddFile(targetIndex, UnixFilename(tmps));
                if (!pfile)
                    Manager::Get()->GetLogManager()->DebugLog(_T("Can't add auto-generated file ") + tmps);
                else
                {
                    pf->generatedFiles.push_back(pfile);
                    pfile->SetAutoGeneratedBy(pf);
                }
            }
        }
    }

    return pf;
}

bool cbProject::RemoveFile(ProjectFile* pf)
{
    if (!pf)
        return false;
    m_ProjectFilesMap.erase(UnixFilename(pf->relativeFilename)); // remove from hashmap
    Manager::Get()->GetEditorManager()->Close(pf->file.GetFullPath());

    {
		FilesList::iterator it = m_Files.find(pf);

		if (it == m_Files.end())
		{
			Manager::Get()->GetLogManager()->DebugLog(_T("Can't locate node for ProjectFile* !"));
		}
		else
		{
			if (!m_FileArray.IsEmpty())
				m_FileArray.Remove(*it);

			m_Files.erase(it);
		}
	}
	// remove this file from all targets too
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        if (ProjectBuildTarget* target = m_Targets[i])
            target->RemoveFile(pf);
    }

    // if this is auto-generated, inform "parent"
    if (pf->AutoGeneratedBy())
    {
        ProjectFilesVector::iterator it = std::find(pf->AutoGeneratedBy()->generatedFiles.begin(),
            pf->AutoGeneratedBy()->generatedFiles.end(), pf);
        pf->AutoGeneratedBy()->generatedFiles.erase(it);
    }

    // also remove generated files (see above code: files will empty the vector)
    while (pf->generatedFiles.size())
        RemoveFile(pf->generatedFiles[0]);
    pf->generatedFiles.clear();

    delete pf;

    SetModified(true);
    return true;
}

const wxArrayString& cbProject::GetVirtualFolders() const
{
    return m_VirtualFolders;
}

bool cbProject::AppendUniqueVirtualFolder(const wxString &folder)
{
    if (m_VirtualFolders.Index(folder)==wxNOT_FOUND)
    {
        m_VirtualFolders.push_back(folder);
        return true;
    }
    else
        return false;
}

void cbProject::RemoveVirtualFolders(const wxString &folder)
{
    for (int i = (int)m_VirtualFolders.GetCount() - 1; i >= 0; --i)
    {
        if (m_VirtualFolders[i].StartsWith(folder))
            m_VirtualFolders.RemoveAt(i);
    }
    // now loop all project files and remove them from this virtual folder
    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
    {
        ProjectFile* f = *it;
        if (f && !f->virtual_path.IsEmpty())
        {
            if (f->virtual_path.StartsWith(folder)) // need 2 checks because of last separator
                f->virtual_path.Clear();
        }
    }

    SetModified(true);
}

void cbProject::ReplaceVirtualFolder(const wxString &oldFolder, const wxString &newFolder)
{
    int idx = m_VirtualFolders.Index(oldFolder);
    if (idx != wxNOT_FOUND)
        m_VirtualFolders[idx] = newFolder;
    else
        m_VirtualFolders.Add(newFolder);

    // now loop all project files and rename this virtual folder
    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
    {
        ProjectFile* f = *it;
        if (f && !f->virtual_path.IsEmpty())
        {
            if (f->virtual_path.StartsWith(oldFolder))
                f->virtual_path.Replace(oldFolder, newFolder);
        }
    }

    SetModified(true);
}


void cbProject::SetVirtualFolders(const wxArrayString& folders)
{
    m_VirtualFolders = folders;
    for (size_t i = 0; i < m_VirtualFolders.GetCount(); ++i)
    {
        m_VirtualFolders[i].Replace(_T("/"), wxString(wxFILE_SEP_PATH));
        m_VirtualFolders[i].Replace(_T("\\"), wxString(wxFILE_SEP_PATH));
    }
}

void cbProject::SaveTreeState(wxTreeCtrl* tree)
{
    ::SaveTreeState(tree, m_ProjectNode, m_ExpandedNodes, m_SelectedNodes);
}

void cbProject::RestoreTreeState(wxTreeCtrl* tree)
{
    ::RestoreTreeState(tree, m_ProjectNode, m_ExpandedNodes, m_SelectedNodes);
}

const wxString& cbProject::GetMakefile()
{
    if (!m_Makefile.IsEmpty())
        return m_Makefile;

    wxFileName makefile(m_Makefile);
    makefile.Assign(m_Filename);
    makefile.SetName(_T("Makefile"));
    makefile.SetExt(_T(""));
    makefile.MakeRelativeTo( GetBasePath() );

    m_Makefile = makefile.GetFullPath();

    return m_Makefile;
}

void cbProject::SetMakefileExecutionDir(const wxString& dir)
{
    if (m_MakefileExecutionDir != dir)
    {
        m_MakefileExecutionDir = dir;
        SetModified(true);
    }
}

wxString cbProject::GetMakefileExecutionDir()
{
    if (m_MakefileExecutionDir.IsEmpty())
    {
        wxFileName execution_dir( GetBasePath() );
        m_MakefileExecutionDir = execution_dir.GetFullPath();
    }
    return m_MakefileExecutionDir;
}

wxString cbProject::GetExecutionDir()
{
    if (!m_CustomMakefile)
        return GetBasePath();

    return GetMakefileExecutionDir();
}

ProjectFile* cbProject::GetFile(int index)
{
    if (m_FileArray.GetCount() == 0)
    {
        for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        {
            if (!*it)
                continue;
            m_FileArray.Add((ProjectFile*)*it);
        }
    }

    if (index < 0 || index >= static_cast<int>(m_Files.size()))
        return NULL;

    return m_FileArray.Item(index);
}

ProjectFile* cbProject::GetFileByFilename(const wxString& filename, bool isRelative, bool isUnixFilename)
{
    // m_ProjectFilesMap keeps UnixFilename(ProjectFile::relativeFilename)
    wxString tmp = filename;
    if (!isRelative)
    {
        // if the search is not relative, make it
        wxFileName fname(realpath(filename));
        fname.MakeRelativeTo( GetBasePath() );
        tmp = fname.GetFullPath();
    }
    else
    {
        // make sure filename doesn't start with ".\"
        // our own relative files don't have it, so the search would fail
        // this happens when importing MS projects...
        if (tmp.StartsWith(_T(".\\")) ||
            tmp.StartsWith(_T("./")))
        {
            tmp.Remove(0, 2);
        }
    }

    if (isUnixFilename)
        return m_ProjectFilesMap[tmp];

    return m_ProjectFilesMap[UnixFilename(tmp)];
}

bool cbProject::QueryCloseAllFiles()
{
    FilesList::iterator it = m_Files.begin();
    while (it != m_Files.end())
    {
        ProjectFile* f = *it++;
        cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(f->file.GetFullPath());
        if (ed && ed->GetModified())
        {
            if (!Manager::Get()->GetEditorManager()->QueryClose(ed))
                return false;
        }
    }
    return true;
}

bool cbProject::CloseAllFiles(bool dontsave)
{
    // first try to close modified editors

    if (!dontsave && !QueryCloseAllFiles())
            return false;

    // now free the rest of the project files
    Manager::Get()->GetEditorManager()->HideNotebook();
    for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
    {
        ProjectFile* f = *it;
        if (f)
            Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath(),true);
        delete f;
    }
    m_FileArray.Clear();
    m_Files.clear();
    Manager::Get()->GetEditorManager()->ShowNotebook();

    return true;
}

bool cbProject::SaveAllFiles()
{
    int count = m_Files.size();
    FilesList::iterator it = m_Files.begin();
    while (it != m_Files.end())
    {
        ProjectFile* f = *it++;
        if (Manager::Get()->GetEditorManager()->Save(f->file.GetFullPath()))
            --count;
    }
    return count == 0;
}

int cbProject::SelectTarget(int initial, bool evenIfOne)
{
    if (!evenIfOne && GetBuildTargetsCount() == 1)
        return 0;

    SelectTargetDlg dlg(nullptr, this, initial);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        return dlg.GetSelection();

    return -1;
}

// Build targets

ProjectBuildTarget* cbProject::AddDefaultBuildTarget()
{
    return AddBuildTarget(_T("default"));
}

ProjectBuildTarget* cbProject::AddBuildTarget(const wxString& targetName)
{
    if (GetBuildTarget(targetName)) // Don't add the target if it exists
        return nullptr;
    ProjectBuildTarget* target = new ProjectBuildTarget(this);
    target->m_Filename = m_Filename; // really important
    target->SetTitle(targetName);
    target->SetCompilerID(GetCompilerID()); // same compiler as project's
    target->SetOutputFilename(wxFileName(GetOutputFilename()).GetFullName());
    target->SetWorkingDir(_T("."));
    target->SetObjectOutput(_T(".objs"));
    target->SetDepsOutput(_T(".deps"));
    m_Targets.Add(target);

    // remove any virtual targets with the same name
    if (HasVirtualBuildTarget(targetName))
    {
        RemoveVirtualBuildTarget(targetName);
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Deleted existing virtual target '%s' because real target was added with the same name"), targetName.wx_str()));
    }

    SetModified(true);

    NotifyPlugins(cbEVT_BUILDTARGET_ADDED, targetName);
    NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
    return target;
}

bool cbProject::RenameBuildTarget(int index, const wxString& targetName)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        wxString oldTargetName = target->GetTitle();

        // rename target if referenced in any virtual target too
        for (VirtualBuildTargetsMap::iterator it = m_VirtualTargets.begin(); it != m_VirtualTargets.end(); ++it)
        {
            wxArrayString& tgts = it->second;
            index = tgts.Index(target->GetTitle());
            if (index != -1)
                tgts[index] = targetName;
        }

        // rename target for all files that reference it
        for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        {
            ProjectFile* pf = *it;
            pf->RenameBuildTarget(target->GetTitle(), targetName);
        }

        // finally rename the target
        target->SetTitle(targetName);
        SetModified(true);
        NotifyPlugins(cbEVT_BUILDTARGET_RENAMED, targetName, oldTargetName);
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
        return true;
    }
    return false;
}

bool cbProject::RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName)
{
    return RenameBuildTarget(IndexOfBuildTargetName(oldTargetName), newTargetName);
}

ProjectBuildTarget* cbProject::DuplicateBuildTarget(int index, const wxString& newName)
{
    ProjectBuildTarget* newTarget = nullptr;
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        newTarget = new ProjectBuildTarget(*target);
        wxString newTargetName = !newName.IsEmpty() ? newName : (_("Copy of ") + target->GetTitle());
        newTarget->SetTitle(newTargetName);
        // just notify the files of this target that they belong to the new target too
        for (FilesList::iterator it = newTarget->GetFilesList().begin(); it != newTarget->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            pf->AddBuildTarget(newTargetName);
        }
        SetModified(true);
        m_Targets.Add(newTarget);
        // send also the old target name, so plugins see that the target is duplicated and not a new one added
        // so that plugin specific parameters can be copied, too.
        NotifyPlugins(cbEVT_BUILDTARGET_ADDED, newName, target->GetTitle());
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
    }
    return newTarget;
}

ProjectBuildTarget* cbProject::DuplicateBuildTarget(const wxString& targetName, const wxString& newName)
{
    return DuplicateBuildTarget(IndexOfBuildTargetName(targetName), newName);
}

bool cbProject::ExportTargetAsProject(int index)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (!target)
        return false;
    return ExportTargetAsProject(target->GetTitle());
}

bool cbProject::ExportTargetAsProject(const wxString& targetName)
{
    ProjectBuildTarget* target = GetBuildTarget(targetName);
    if (!target)
        return false;

    // ask for the new project's name
    wxString newName = cbGetTextFromUser(_("Please enter the new project's name (no path, no extension)."),
                                        _("Export target as new project"),
                                        target->GetTitle());
    if (newName.IsEmpty())
        return false;
    wxFileName fname(GetFilename());
    fname.SetName(newName);

    Save();
    bool alreadyModified = GetModified();
    wxString oldTitle = GetTitle();
    SetTitle(targetName);

    ProjectLoader loader(this);
    bool ret = loader.ExportTargetAsProject(fname.GetFullPath(), target->GetTitle(), m_pExtensionsElement);

    SetTitle(oldTitle);
    if (!alreadyModified)
        SetModified(false);

    return ret;
}

bool cbProject::RemoveBuildTarget(int index)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        wxString oldTargetName = target->GetTitle();

        // remove target from any virtual targets it belongs to
        for (VirtualBuildTargetsMap::iterator it = m_VirtualTargets.begin(); it != m_VirtualTargets.end(); ++it)
        {
            wxArrayString& tgts = it->second;
            int virt_idx = tgts.Index(target->GetTitle());
            if (virt_idx != -1)
                tgts.RemoveAt(virt_idx);
        }

        // remove target from any project files that reference it
        for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        {
            ProjectFile* pf = *it;
            pf->RemoveBuildTarget(target->GetTitle());
        }

        // notify plugins, before the target is deleted, to make a cleanup possible before the target is really deleted
        NotifyPlugins(cbEVT_BUILDTARGET_REMOVED, oldTargetName);
        // finally remove the target
        delete target;
        m_Targets.RemoveAt(index);
        SetModified(true);
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
        return true;
    }
    return false;
}

bool cbProject::RemoveBuildTarget(const wxString& targetName)
{
    return RemoveBuildTarget(IndexOfBuildTargetName(targetName));
}

int cbProject::IndexOfBuildTargetName(const wxString& targetName) const
{
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target->GetTitle().Matches(targetName))
            return i;
    }
    return -1;
}

bool cbProject::BuildTargetValid(const wxString& name, bool virtuals_too) const
{
    if (virtuals_too && HasVirtualBuildTarget(name))
        return true;
    else if (IndexOfBuildTargetName(name) != -1)
        return true;

    return false;
}

wxString cbProject::GetFirstValidBuildTargetName(bool virtuals_too) const
{
    if (virtuals_too && !m_VirtualTargets.empty())
        return m_VirtualTargets.begin()->first;
    else if (m_Targets.GetCount() && m_Targets[0])
        return m_Targets[0]->GetTitle();

    return wxEmptyString;
}

bool cbProject::SetActiveBuildTarget(const wxString& name)
{
    if (name == m_ActiveTarget)
        return true;

    wxString oldActiveTarget = m_ActiveTarget;
    m_ActiveTarget = name;

    bool valid = BuildTargetValid(name);

    if (!valid) // no target (virtual or real) by that name
        m_ActiveTarget = GetFirstValidBuildTargetName();

    NotifyPlugins(cbEVT_BUILDTARGET_SELECTED, m_ActiveTarget, oldActiveTarget);

    return valid;
}

const wxString& cbProject::GetActiveBuildTarget() const
{
    return m_ActiveTarget;
}

void cbProject::SetDefaultExecuteTarget(const wxString& name)
{
    if (name == m_DefaultExecuteTarget)
        return;

    m_DefaultExecuteTarget = name;
    SetModified(true);
}

const wxString& cbProject::GetDefaultExecuteTarget() const
{
    return m_DefaultExecuteTarget;
}

ProjectBuildTarget* cbProject::GetBuildTarget(int index)
{
    if (index >= 0 && index < (int)m_Targets.GetCount())
        return m_Targets[index];
    return nullptr;
}

ProjectBuildTarget* cbProject::GetBuildTarget(const wxString& targetName)
{
    int idx = IndexOfBuildTargetName(targetName);
    return GetBuildTarget(idx);
}

void cbProject::ReOrderTargets(const wxArrayString& nameOrder)
{
    LogManager* msgMan = Manager::Get()->GetLogManager();
    if (nameOrder.GetCount() != m_Targets.GetCount())
    {
        msgMan->DebugLog(F(_T("cbProject::ReOrderTargets() : Count does not match (%lu sent, %lu had)..."),
                           static_cast<unsigned long>(nameOrder.GetCount()),
                           static_cast<unsigned long>(m_Targets.GetCount())));
        return;
    }

    for (unsigned int i = 0; i < nameOrder.GetCount(); ++i)
    {
        ProjectBuildTarget* target = GetBuildTarget(nameOrder[i]);
        if (!target)
        {
            msgMan->DebugLog(F(_T("cbProject::ReOrderTargets() : Target \"%s\" not found..."), nameOrder[i].wx_str()));
            break;
        }

        m_Targets.Remove(target);
        m_Targets.Insert(target, i);

        // we have to re-order the targets which are kept inside
        // the virtual targets array too!
        VirtualBuildTargetsMap::iterator it;
        for (it = m_VirtualTargets.begin(); it != m_VirtualTargets.end(); ++it)
        {
            wxArrayString& vt = it->second;
            if (vt.Index(nameOrder[i]) != wxNOT_FOUND)
            {
                vt.Remove(nameOrder[i]);
                vt.Insert(nameOrder[i], (vt.Count()<=i) ? vt.Count()-1 : i);
            }
        }
    }
    SetModified(true);
}

void cbProject::SetCurrentlyCompilingTarget(ProjectBuildTarget* bt)
{
    m_CurrentlyCompilingTarget = bt;
}

bool cbProject::DefineVirtualBuildTarget(const wxString& alias, const wxArrayString& targets)
{
    if (targets.GetCount() == 0)
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Can't define virtual build target '%s': Group of build targets is empty!"), alias.wx_str()));
        return false;
    }

    ProjectBuildTarget* existing = GetBuildTarget(alias);
    if (existing)
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Can't define virtual build target '%s': Real build target exists with that name!"), alias.wx_str()));
        return false;
    }

    m_VirtualTargets[alias] = targets;
    SetModified(true);
    NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
    return true;
}

bool cbProject::HasVirtualBuildTarget(const wxString& alias) const
{
    return m_VirtualTargets.find(alias) != m_VirtualTargets.end();
}

bool cbProject::RemoveVirtualBuildTarget(const wxString& alias)
{
    VirtualBuildTargetsMap::iterator it = m_VirtualTargets.find(alias);
    if (it == m_VirtualTargets.end())
        return false;

    m_VirtualTargets.erase(it);
    SetModified(true);
    NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
    return true;
}

wxArrayString cbProject::GetVirtualBuildTargets() const
{
    wxArrayString result;
    for (VirtualBuildTargetsMap::const_iterator it = m_VirtualTargets.begin(); it != m_VirtualTargets.end(); ++it)
        result.Add(it->first);

    return result;
}

const wxArrayString& cbProject::GetVirtualBuildTargetGroup(const wxString& alias) const
{
    static wxArrayString resultIfError;

    VirtualBuildTargetsMap::const_iterator it = m_VirtualTargets.find(alias);
    if (it == m_VirtualTargets.end())
        return resultIfError;

    return it->second;
}

wxArrayString cbProject::GetExpandedVirtualBuildTargetGroup(const wxString& alias) const
{
    wxArrayString result;

    VirtualBuildTargetsMap::const_iterator it = m_VirtualTargets.find(alias);
    if (it == m_VirtualTargets.end())
        return result;
    ExpandVirtualBuildTargetGroup(alias, result);

    return result;
}

bool cbProject::CanAddToVirtualBuildTarget(const wxString& alias, const wxString& target)
{
    // virtual not there?
    if (!HasVirtualBuildTarget(alias))
        return false;

    // real targets can be added safely (as long as they 're unique)
    if (!HasVirtualBuildTarget(target))
        return true;

    // virtual targets are checked in two ways:
    // 1) it is checked if it contains alias
    // 2) all its virtual targets are recursively checked if they contain alias
    const wxArrayString& group = GetVirtualBuildTargetGroup(target);
    if (group.Index(alias) != wxNOT_FOUND)
        return false;

    for (size_t i = 0; i < group.GetCount(); ++i)
    {
        // only virtuals
        if (HasVirtualBuildTarget(group[i]))
        {
            if (!CanAddToVirtualBuildTarget(group[i], alias))
                return false;
        }
    }
    return true;
}

void cbProject::ExpandVirtualBuildTargetGroup(const wxString& alias, wxArrayString& result) const
{
    const wxArrayString& group = GetVirtualBuildTargetGroup(alias);
    for (size_t i = 0; i < group.GetCount(); ++i)
    {
        // real targets get added
        if (IndexOfBuildTargetName(group[i]) != -1)
        {
            if (result.Index(group[i]) == wxNOT_FOUND)
                result.Add(group[i]);
        }
        // virtual targets recurse
        else
            ExpandVirtualBuildTargetGroup(group[i], result);
    }
}

void cbProject::SetExtendedObjectNamesGeneration(bool ext)
{
    bool changed = m_ExtendedObjectNamesGeneration != ext;

    // update it now because SetObjName() below will call GetExtendedObjectNamesGeneration()
    // so it must be up-to-date
    m_ExtendedObjectNamesGeneration = ext;

    if (changed)
    {
        for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        {
            ProjectFile* f = *it;
            f->SetObjName(f->relativeToCommonTopLevelPath);
            f->UpdateFileDetails();
        }

        SetModified(true);
    }
}

bool cbProject::GetExtendedObjectNamesGeneration() const
{
    return m_ExtendedObjectNamesGeneration;
}

void cbProject::SetNotes(const wxString& notes)
{
    if (m_Notes != notes)
    {
        m_Notes = notes;
        SetModified(true);
    }
}

const wxString& cbProject::GetNotes() const
{
    return m_Notes;
}

void cbProject::SetShowNotesOnLoad(bool show)
{
    if (m_AutoShowNotesOnLoad != show)
    {
        m_AutoShowNotesOnLoad = show;
        SetModified(true);
    }
}

bool cbProject::GetShowNotesOnLoad() const
{
    return m_AutoShowNotesOnLoad;
}

void cbProject::SetCheckForExternallyModifiedFiles(bool check)
{
    if (m_CheckForExternallyModifiedFiles != check)
    {
        m_CheckForExternallyModifiedFiles = check;
        SetModified(true);
    }
}

bool cbProject::GetCheckForExternallyModifiedFiles() const
{
    return m_CheckForExternallyModifiedFiles;
}

void cbProject::ShowNotes(bool nonEmptyOnly, bool editable)
{
    if (!editable && nonEmptyOnly && m_Notes.IsEmpty())
        return;

    GenericMultiLineNotesDlg dlg(Manager::Get()->GetAppWindow(),
                                _("Notes about ") + m_Title,
                                m_Notes,
                                !editable);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (editable)
            SetNotes(dlg.GetNotes());
    }
}

void cbProject::SetTitle(const wxString& title)
{
    if ( title != GetTitle() )
    {
        CompileTargetBase::SetTitle(title);
        NotifyPlugins(cbEVT_PROJECT_RENAMED);
    }
}

TiXmlNode* cbProject::GetExtensionsNode()
{
    if (!m_pExtensionsElement)
        m_pExtensionsElement = new TiXmlElement(cbU2C(_T("Extensions")));
    return m_pExtensionsElement;
}

void cbProject::AddToExtensions(const wxString& stringDesc)
{
    // sample stringDesc:
    // node/+subnode/subsubnode:attr=val

    TiXmlElement* elem = GetExtensionsNode()->ToElement();
    size_t pos = 0;
    while (true)
    {
        // ignore consecutive slashes
        while (pos < stringDesc.Length() && stringDesc.GetChar(pos) == _T('/'))
        {
            ++pos;
        }

        // find next slash or colon
        size_t nextPos = pos;
        while (nextPos < stringDesc.Length() && stringDesc.GetChar(++nextPos) != _T('/') && stringDesc.GetChar(nextPos) != _T(':'))
            ;

        wxString current = stringDesc.Mid(pos, nextPos - pos);
        if (current.IsEmpty() || current[0] == _T(':')) // abort on invalid case: "node/:attr=val" (consecutive "/:")
            break;

        // find or create the subnode
        bool forceAdd = current[0] == _T('+');
        if (forceAdd)
            current.Remove(0, 1); // remove '+'
        TiXmlElement* sub = !forceAdd ? elem->FirstChildElement(cbU2C(current)) : nullptr;
        if (!sub)
        {
            sub = elem->InsertEndChild(TiXmlElement(cbU2C(current)))->ToElement();
            SetModified(true);
        }
        elem = sub;

        // last node?
        if (stringDesc.GetChar(nextPos) == _T(':'))
        {
            // yes, just parse the attribute now
            pos = nextPos + 1; // skip the colon
            nextPos = pos;
            while (nextPos < stringDesc.Length() && stringDesc.GetChar(++nextPos) != _T('='))
                ;
            if (pos == nextPos || nextPos == stringDesc.Length())
            {
                // invalid attribute
            }
            else
            {
                wxString key = stringDesc.Mid(pos, nextPos - pos);
                wxString val = stringDesc.Mid(nextPos + 1, stringDesc.Length() - nextPos - 1);
                sub->SetAttribute(cbU2C(key), cbU2C(val));
                SetModified(true);
            }

            // all done
            break;
        }

        pos = nextPos; // prepare for next loop
    }
}

void cbProject::ProjectFileRenamed(ProjectFile* pf)
{
    for (ProjectFiles::iterator it = m_ProjectFilesMap.begin(); it != m_ProjectFilesMap.end(); ++it)
    {
        ProjectFile* itpf = it->second;
        if (itpf == pf)
        {
            // got it
            m_ProjectFilesMap.erase(it);
            m_ProjectFilesMap[UnixFilename(pf->file.GetFullPath())] = pf;
            break;
        }
    }
}

void cbProject::SetGlobs(const std::vector<Glob>& globs)
{
    m_Globs = globs;
}

std::vector<cbProject::Glob> cbProject::GetGlobs() const
{
    return m_Globs;
}

wxString cbGetDynamicLinkerPathForTarget(cbProject *project, ProjectBuildTarget* target)
{
    if (!target)
        return wxEmptyString;

    Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
    if (compiler)
    {
        CompilerCommandGenerator* generator = compiler->GetCommandGenerator(project);
        wxString libPath;
        const wxString libPathSep = platform::windows ? _T(";") : _T(":");
        libPath << _T(".") << libPathSep;
        libPath << GetStringFromArray(generator->GetLinkerSearchDirs(target), libPathSep);
        if (!libPath.IsEmpty() && libPath.Mid(libPath.Length() - 1, 1) == libPathSep)
            libPath.Truncate(libPath.Length() - 1);

        delete generator;
        return libPath;
    }
    return wxEmptyString;
}

wxString cbMergeLibPaths(const wxString &oldPath, const wxString &newPath)
{
    wxString result = newPath;
    const wxString libPathSep = platform::windows ? _T(";") : _T(":");
    if (!newPath.IsEmpty() && newPath.Mid(newPath.Length() - 1, 1) != libPathSep)
        result << libPathSep;
    result << oldPath;
    return result;
}
