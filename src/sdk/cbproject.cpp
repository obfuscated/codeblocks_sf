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
#include <wx/settings.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/textdlg.h>

#ifndef CB_PRECOMP
    #include "cbproject.h" // class's header file
    #include "compiler.h" // GetSwitches
    #include "sdk_events.h"
    #include "manager.h"
    #include "cbeditor.h"
    #include "globals.h"
    #include "pluginmanager.h"
    #include "projectmanager.h"
    #include "macrosmanager.h"
    #include "logmanager.h"
    #include "editormanager.h"
    #include "filemanager.h"
    #include "configmanager.h"
    #include "compilerfactory.h"
    #include "projectbuildtarget.h"
    #include "projectfile.h"
    #include "infowindow.h"
#endif

#include <map>
#include "projectoptionsdlg.h"
#include "projectloader.h"
#include "projectlayoutloader.h"
#include "selecttargetdlg.h"
#include "filegroupsandmasks.h"
#include "filefilters.h"
#include "annoyingdialog.h"
#include "genericmultilinenotesdlg.h"


namespace compatibility { typedef TernaryCondTypedef<wxMinimumVersion<2,5>::eval, wxTreeItemIdValue, long int>::eval tree_cookie_t; };


// class constructor
cbProject::cbProject(const wxString& filename)
    : m_CustomMakefile(false),
    m_Loaded(false),
    m_CurrentlyLoading(false),
    m_PCHMode(pchSourceFile),
    m_CurrentlyCompilingTarget(0),
    m_ExtendedObjectNamesGeneration(false),
    m_AutoShowNotesOnLoad(false),
    m_pExtensionsElement(0)
{
    SetCompilerID(CompilerFactory::GetDefaultCompilerID());
    SetModified(false);

    m_Files.Clear();
    if (!filename.IsEmpty() && wxFileExists(filename) || wxDirExists(filename))
    {
        // existing project
        m_Filename = filename;
        m_BasePath = GetBasePath();
        Open();
    }
    else
    {
        // new project
        SetModified(true);
        if (filename.IsEmpty())
        {
            m_Filename = CreateUniqueFilename();
            m_Loaded = SaveAs();
        }
        else
        {
            m_Filename = filename;
            m_Loaded = Save();
        }
        if (m_Loaded)
        {
            wxFileName fname(m_Filename);
            m_Title = fname.GetName();
            m_BasePath = GetBasePath();
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

                int count = GetFilesCount();
                for (int i = 0; i < count; ++i)
                {
                    ProjectFile* pf = GetFile(i);
                    wxFileName obj(pf->GetObjName());
                    if (FileTypeOf(pf->relativeFilename) != ftResource &&
                        obj.GetExt() == compiler->GetSwitches().objectExtension)
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

    m_Files.DeleteContents(true);
    m_Files.Clear();
    m_Files.DeleteContents(false);
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
            Manager::Get()->GetLogManager()->Log(_("done"));
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
    // for projects where the project file is in a subdir, files will have ..
    // in their paths
    wxString sep = wxFileName::GetPathSeparator();
    wxFileName base = GetBasePath() + sep;
    Manager::Get()->GetLogManager()->DebugLog(_T("Project's base path: ") + base.GetFullPath());

    // this loop takes ~30ms for 1000 project files
    // it's as fast as it can get, considered that it used to take ~1200ms ;)
    // don't even bother making it faster - you can't :)
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        wxString tmp = f->relativeFilename;
        wxString tmpbase = m_BasePath;

        size_t pos = 0;
        while (pos < tmp.Length() &&
            (tmp.GetChar(pos) == _T('.') || tmp.GetChar(pos) == _T('/') || tmp.GetChar(pos) == _T('\\')))
        {
            ++pos;
        }
        if (pos > 0 && pos < tmp.Length())
        {
            tmpbase << sep << tmp.Left(pos) << sep;
            f->relativeToCommonTopLevelPath = tmp.Right(tmp.Length() - pos);
        }
        else
            f->relativeToCommonTopLevelPath = tmp;
        f->SetObjName(f->relativeToCommonTopLevelPath);

        wxFileName tmpbaseF(tmpbase);
        tmpbaseF.Normalize(wxPATH_NORM_DOTS);
        if (tmpbaseF.GetDirCount() < base.GetDirCount())
            base = tmpbaseF;
    }

    m_CommonTopLevelPath = base.GetFullPath();
    Manager::Get()->GetLogManager()->DebugLog(_T("Project's common toplevel path: ") + m_CommonTopLevelPath);
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
                    wxSAVE | wxOVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return false;
    m_Filename = dlg.GetPath();
    fname.Assign(m_Filename);

    // make sure the project file uses the correct extension
    // we don't use wxFileName::SetExt() because if the user has added a dot
    // in the filename, the part after it would be interpeted as extension
    // (and it might not be)
    // so we just append the correct extension
    if (!fname.GetExt().Matches(FileFilters::CODEBLOCKS_EXT))
        fname.Assign(m_Filename + _T('.') + FileFilters::CODEBLOCKS_EXT);

//    Manager::Get()->GetProjectManager()->GetTree()->SetItemText(m_ProjectNode, fname.GetFullName());
    if (!m_Loaded)
        AddDefaultBuildTarget();
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

    wxFileName fname(m_Filename);
    fname.SetExt(_T("layout"));
    ProjectLayoutLoader loader(this);
    return loader.Save(fname.GetFullPath());
}

bool cbProject::LoadLayout()
{
   if (m_Filename.IsEmpty())
        return false;
    int openmode = Manager::Get()->GetConfigManager(_T("project_manager"))->ReadInt(_T("/open_files"), (long int)1);
    bool result = false;

    if(openmode==2)
    {
        // Do not open any files
        result = true;
    }
    else
    {
        Manager::Get()->GetEditorManager()->HideNotebook();
        if(openmode == 0) // Open all files
        {
            FilesList::Node* node = m_Files.GetFirst();
            while(node)
            {
                ProjectFile* f = node->GetData();
                Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath(),0,f);
                node = node->GetNext();
            }
            result = true;
        }
        else if(openmode == 1)// Open last open files
        {
            wxFileName fname(m_Filename);
            fname.SetExt(_T("layout"));
            ProjectLayoutLoader loader(this);
            if (loader.Open(fname.GetFullPath()))
            {
                typedef std::map<int, ProjectFile*> open_files_map;
                open_files_map open_files;

                // Get all files to open and sort them according to their tab-position:
                FilesList::Node* node = m_Files.GetFirst();
                while(node)
                {
                    ProjectFile* f = node->GetData();
                    if (f->editorOpen)
                        open_files[f->editorTabPos] = f;
                    node = node->GetNext();
                }

                // Load all requested files
                std::vector<LoaderBase*> filesInMemory;
                for (open_files_map::iterator it = open_files.begin(); it != open_files.end(); ++it)
                {
                    filesInMemory.push_back(Manager::Get()->GetFileManager()->Load((*it).second->file.GetFullPath()));
                }
                // Open all requested files:
                size_t i = 0;
                for (open_files_map::iterator it = open_files.begin(); it != open_files.end(); ++it)
                {
                    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filesInMemory[i], (*it).second->file.GetFullPath(),0,(*it).second);
                    if (ed)
                        ed->SetProjectFile((*it).second);
                    ++i;
                }

                ProjectFile* f = loader.GetTopProjectFile();
                if (f)
                {
                    Manager::Get()->GetLogManager()->DebugLog(_T("Top Editor: ") + f->file.GetFullPath());
                    EditorBase* eb = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
                    if(eb)
                        eb->Activate();
                }
//                Manager::Get()->GetAppWindow()->Thaw();
            }
            result = true;
        }
        else
            result = false;
        Manager::Get()->GetEditorManager()->ShowNotebook();
    }
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

ProjectFile* cbProject::AddFile(int targetIndex, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
//  NOTE (Rick#1#): When loading the project, do not search for existing files
//  (Assumming that there are no duplicate entries in the .cbp file)
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
    ProjectFile* f = m_ProjectFilesMap[UnixFilename(filename)];
    if (f)
        return f;

    // create file
    f = new ProjectFile(this);
    bool localCompile, localLink;
    wxFileName fname(filename);
    wxString ext;

    FileType ft = FileTypeOf(filename);

    ext = filename.AfterLast(_T('.')).Lower();
    if (ext.IsSameAs(FileFilters::C_EXT))
        f->compilerVar = _T("CC");
    else if (platform::windows && ext.IsSameAs(FileFilters::RESOURCE_EXT))
        f->compilerVar = _T("WINDRES");
    else
        f->compilerVar = _T("CPP"); // default

    if (!m_Targets.GetCount())
    {
        // no targets in project; add default
        AddDefaultBuildTarget();
        if (!m_Targets.GetCount())
        {
            delete f;
            return 0L; // if that failed, fail addition of file...
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
		Compiler* c = CompilerFactory::GetCompiler(GetCompilerID());
		if (c)
		{
			const CompilerTool* t = &c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
			if (t->generatedFiles.GetCount())
			{
				GenFilesHackMap[c] = t;
			}
		}

		for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
		{
			Compiler* c = CompilerFactory::GetCompiler(m_Targets[i]->GetCompilerID());
			if (GenFilesHackMap.find(c) != GenFilesHackMap.end())
				continue; // compiler already in map

			if (c)
			{
				const CompilerTool* t = &c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
				if (t->generatedFiles.GetCount())
				{
					GenFilesHackMap[c] = t;
				}
			}
		}
	}
	else
	{
		// targetIndex is valid: just add a single entry to the map
		Compiler* c = CompilerFactory::GetCompiler(m_Targets[targetIndex]->GetCompilerID());
		if (c)
		{
			const CompilerTool* t = &c->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, fname.GetExt());
			if (t->generatedFiles.GetCount())
			{
				GenFilesHackMap[c] = t;
			}
		}
	}
	// so... now, if GenFilesHackMap is not empty, we know
	// 1) this file generates other files and
	// 2) iterating the map will give us the generated file names :)

    // add the build target
    if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
        f->AddBuildTarget(m_Targets[targetIndex]->GetTitle());

    localCompile = compile &&
                    (ft == ftSource ||
                    ft == ftResource ||
                    !GenFilesHackMap.empty());
    localLink = link &&
                (ft == ftSource ||
                ft == ftResource ||
                ft == ftObject ||
                ft == ftResourceBin ||
                ft == ftStaticLib);

    f->compile = localCompile;
    f->link = localLink;

    wxString local_filename = filename;

#ifdef __WXMSW__
    // for windows, make sure the filename is not on another drive...
    if (local_filename.Length() > 1 &&
        local_filename.GetChar(1) == _T(':') && // quick test to avoid the costly wxFileName ctor below
        fname.GetVolume() != wxFileName(m_Filename).GetVolume())
    {
        fname.Assign(filename);
    }
    else
#endif
    {
        // make sure the filename is relative to the project's base path
        if (fname.IsAbsolute())
        {
            fname.MakeRelativeTo(GetBasePath());
            local_filename = fname.GetFullPath();
        }
        fname.Assign(GetBasePath() + wxFILE_SEP_PATH + local_filename);
    }
    fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, GetBasePath());

    wxString fullFilename = fname.GetFullPath();
    f->file.Assign(fname);
    f->relativeFilename = UnixFilename(local_filename);

    // now check if we have already added this file
    // if we have, return the existing file, but add the specified target
    ProjectFile* existing = GetFileByFilename(f->relativeFilename, true, true);
    if (existing == f)
    {
        delete f;
        if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
            existing->AddBuildTarget(m_Targets[targetIndex]->GetTitle());
        return existing;
    }

    m_Files.Append(f);
    if (!m_CurrentlyLoading)
    {
        // check if we really need to recalculate the common top-level path for the project
        if (!fullFilename.StartsWith(m_CommonTopLevelPath))
            CalculateCommonTopLevelPath();
        else
        {
            // set f->relativeToCommonTopLevelPath
            f->relativeToCommonTopLevelPath = fullFilename.Right(fullFilename.Length() - m_CommonTopLevelPath.Length());
        }
    }
    SetModified(true);
    m_ProjectFilesMap[UnixFilename(f->relativeFilename)] = f; // add to hashmap

    if (!wxFileExists(fullFilename))
        f->SetFileState(fvsMissing);
    else if (!wxFile::Access(fullFilename.c_str(), wxFile::write)) // readonly
        f->SetFileState(fvsReadOnly);

    if (!GenFilesHackMap.empty())
    {
        // auto-generated files!
        wxFileName tmp = f->file;
		for (std::map<Compiler*, const CompilerTool*>::const_iterator it = GenFilesHackMap.begin(); it != GenFilesHackMap.end(); ++it)
		{
			const CompilerTool* tool = it->second;
			for (size_t i = 0; i < tool->generatedFiles.GetCount(); ++i)
			{
				tmp.SetFullName(tool->generatedFiles[i]);
				wxString tmps = tmp.GetFullPath();
				// any macro replacements here, should also be done in
				// CompilerCommandGenerator::GenerateCommandLine !!!
				tmps.Replace(_T("$file_basename"), f->file.GetName()); // old way - remove later
				tmps.Replace(_T("$file_name"), f->file.GetName());
				tmps.Replace(_T("$file_dir"), f->file.GetPath());
				tmps.Replace(_T("$file_ext"), f->file.GetExt());
				tmps.Replace(_T("$file"), f->file.GetFullName());
				Manager::Get()->GetMacrosManager()->ReplaceMacros(tmps);

				ProjectFile* pfile = AddFile(targetIndex, UnixFilename(tmps));
				if (!pfile)
					Manager::Get()->GetLogManager()->DebugLog(_T("Can't add auto-generated file ") + tmps);
				else
				{
					f->generatedFiles.push_back(pfile);
					pfile->autoGeneratedBy = f;
				}
			}
		}
    }

    return f;
}

bool cbProject::RemoveFile(ProjectFile* pf)
{
    if (!pf)
        return false;
    m_ProjectFilesMap.erase(UnixFilename(pf->relativeFilename)); // remove from hashmap
    Manager::Get()->GetEditorManager()->Close(pf->file.GetFullPath());

    FilesList::Node* node = m_Files.Find(pf);
    if (!node)
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("Can't locate node for ProjectFile* !"));
    }
    else
    {
        m_Files.DeleteNode(node);
    }

    // remove this file from all targets too
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target)
        {
            target->GetFilesList().DeleteObject(pf);
        }
    }

    // if this is auto-generated, inform "parent"
    if (pf->autoGeneratedBy)
    {
        ProjectFilesVector::iterator it = std::find(pf->autoGeneratedBy->generatedFiles.begin(), pf->autoGeneratedBy->generatedFiles.end(), pf);
        pf->autoGeneratedBy->generatedFiles.erase(it);
    }

    // also remove generated files (see above code: files will empty the vector)
    while (pf->generatedFiles.size())
    {
        RemoveFile(pf->generatedFiles[0]);
    }
    pf->generatedFiles.clear();

    delete pf;

    SetModified(true);
    return true;
}

bool cbProject::RemoveFile(int index)
{
    if (index < 0 || index >= (int)m_Files.GetCount())
        return false; // invalid index
    ProjectFile* f = m_Files[index];
    return RemoveFile(f);
}

int filesSort(const ProjectFile** arg1, const ProjectFile** arg2)
{
    return (*arg1)->file.GetFullPath().CompareTo((*arg2)->file.GetFullPath());
}

void cbProject::BuildTree(wxTreeCtrl* tree, const wxTreeItemId& root, bool categorize, bool useFolders, FilesGroupsAndMasks* fgam)
{
    if (!tree)
        return;

    int fldIdx = Manager::Get()->GetProjectManager()->FolderIconIndex();
    int vfldIdx = Manager::Get()->GetProjectManager()->VirtualFolderIconIndex();
    bool read_only = (!wxFile::Access(GetFilename().c_str(), wxFile::write));
    int prjIdx = Manager::Get()->GetProjectManager()->ProjectIconIndex(read_only);

    //sort list of files
    m_Files.Sort(filesSort);

    // add our project's root item
    FileTreeData* ftd = new FileTreeData(this, FileTreeData::ftdkProject);
    m_ProjectNode = tree->AppendItem(root, GetTitle(), prjIdx, prjIdx, ftd);
    wxTreeItemId others = m_ProjectNode;
    wxTreeItemId generated = m_ProjectNode;

    // create file-type categories nodes (if enabled)
    wxTreeItemId* pGroupNodes = 0L;
    if (categorize && fgam)
    {
        pGroupNodes = new wxTreeItemId[fgam->GetGroupsCount()];
        for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
        {
            ftd = new FileTreeData(this, FileTreeData::ftdkVirtualGroup);
            ftd->SetFolder(fgam->GetGroupName(i));
            pGroupNodes[i] = tree->AppendItem(m_ProjectNode, fgam->GetGroupName(i), fldIdx, fldIdx, ftd);
        }
        // add a default category "Generated" for all auto-generated file types
        ftd = new FileTreeData(this, FileTreeData::ftdkVirtualGroup);
        generated = tree->AppendItem(m_ProjectNode, _("Auto-generated"), fldIdx, fldIdx, ftd);
        // add a default category "Others" for all non-matching file-types
        ftd = new FileTreeData(this, FileTreeData::ftdkVirtualGroup);
        others = tree->AppendItem(m_ProjectNode, _("Others"), fldIdx, fldIdx, ftd);
    }
    // Now add any virtual folders
    for (size_t i = 0; i < m_VirtualFolders.GetCount(); ++i)
    {
        ftd = new FileTreeData(this, FileTreeData::ftdkVirtualFolder);
        ftd->SetFolder(m_VirtualFolders[i]);
        AddTreeNode(tree, m_VirtualFolders[i], m_ProjectNode, true, FileTreeData::ftdkVirtualFolder, true, vfldIdx, ftd);
    }

    // iterate all project files and add them to the tree
    int count = 0;
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        ftd = new FileTreeData(this, FileTreeData::ftdkFile);
        ftd->SetFileIndex(count++);
        ftd->SetProjectFile(f);
        ftd->SetFolder(f->file.GetFullPath());

        wxFileName nodefile = f->file;
        nodefile.MakeRelativeTo(m_CommonTopLevelPath);
        wxString nodetext = nodefile.GetFullPath();
        FileTreeData::FileTreeDataKind folders_kind = FileTreeData::ftdkFolder;

        wxTreeItemId parentNode = m_ProjectNode;
        // check if files grouping is enabled and find the group parent
        // Also make a check that the file is not under virtual folder
        if (categorize && pGroupNodes && fgam && f->virtual_path.IsEmpty())
        {
            bool found = false;
            // auto-generated files end up all together
            if (f->autoGeneratedBy)
            {
                parentNode = generated;
                found = true;
            }

            // else try to match a group
            if (!found)
            {
                for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
                {
                    wxFileName fname(f->relativeToCommonTopLevelPath);
                    if (fgam->MatchesMask(fname.GetFullName(), i))
                    {
                        parentNode = pGroupNodes[i];
                        found = true;
                        break;
                    }
                }
            }

            // if not matched a group, put it in "Others" group
            if (!found)
                parentNode = others;
        }
        else if ((!categorize || !pGroupNodes || !fgam) && f->virtual_path.IsEmpty())
        {
            parentNode = m_ProjectNode;
        }
        /* Else put the file under virtual folder */
        else if (!f->virtual_path.IsEmpty())
        {
            nodetext = f->virtual_path + wxFILE_SEP_PATH + f->file.GetFullName();
            folders_kind = FileTreeData::ftdkVirtualFolder;
            wxString slash = f->virtual_path.Last() == wxFILE_SEP_PATH ? _T("") : wxString(wxFILE_SEP_PATH);
            ftd->SetFolder(f->virtual_path);

            if (m_VirtualFolders.Index(f->virtual_path + slash) == wxNOT_FOUND)
                m_VirtualFolders.Add(f->virtual_path + slash);
        }

        // add file in the tree
        f->m_TreeItemId = AddTreeNode(tree, nodetext, parentNode, useFolders || folders_kind == FileTreeData::ftdkVirtualFolder, folders_kind, f->compile, (int)f->m_VisualState, ftd);
    }

    // remove empty tree nodes (like empty groups)
    if (categorize && fgam)
    {
        for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
        {
            if (tree->GetChildrenCount(pGroupNodes[i], false) == 0)
                tree->Delete(pGroupNodes[i]);
        }
        if (tree->GetChildrenCount(others, false) == 0)
            tree->Delete(others);
        if (tree->GetChildrenCount(generated, false) == 0)
            tree->Delete(generated);
    }
    delete[] pGroupNodes;

    tree->Expand(m_ProjectNode);
}

// helper function used by AddTreeNode
static wxString GetRelativeFolderPath(wxTreeCtrl* tree, wxTreeItemId parent)
{
    wxString fld;
    while (parent.IsOk())
    {
        FileTreeData* ftd = (FileTreeData*)tree->GetItemData(parent);
        if (!ftd || (ftd->GetKind() != FileTreeData::ftdkFolder && ftd->GetKind() != FileTreeData::ftdkVirtualFolder))
            break;
        fld.Prepend(tree->GetItemText(parent) + wxFILE_SEP_PATH);
        parent = tree->GetItemParent(parent);
    }
    return fld;
}

wxTreeItemId cbProject::AddTreeNode(wxTreeCtrl* tree,
                                    const wxString& text,
                                    const wxTreeItemId& parent,
                                    bool useFolders,
                                    FileTreeData::FileTreeDataKind folders_kind,
                                    bool compiles,
                                    int image,
                                    FileTreeData* data)
{
    // see if the text contains any path info, e.g. plugins/compilergcc/compilergcc.cpp
    // in that case, take the first element (plugins in this example), create a sub-folder
    // with the same name and recurse with the result...

    wxTreeItemId ret;

    if (text.IsEmpty())
        return ret;

    wxString path = text;

    // special case for windows and files on a different drive
    if (platform::windows && path.Length() > 1 && path.GetChar(1) == _T(':'))
        path.Remove(1, 1);

    int pos = path.Find(_T('/'));
    if (pos == -1)
        pos = path.Find(_T('\\'));
    if (useFolders && pos >= 0)
    {
        // ok, we got it. now split it up and recurse
        wxString folder = path.Left(pos);
        // avoid consecutive path separators
        while (path.GetChar(pos + 1) == _T('/') || path.GetChar(pos + 1) == _T('\\'))
            ++pos;
        path = path.Right(path.Length() - pos - 1);

        compatibility::tree_cookie_t cookie = 0;

        wxTreeItemId newparent = tree->GetFirstChild(parent, cookie);
        while (newparent)
        {
            wxString itemText = tree->GetItemText(newparent);
            if (itemText.Matches(folder))
                break;
            newparent = tree->GetNextChild(parent, cookie);
        }

        if (!newparent)
        {
            // in order not to override wxTreeCtrl to sort alphabetically but the
            // folders be always on top, we just search here where to put the new folder...
            int fldIdx = Manager::Get()->GetProjectManager()->FolderIconIndex();
            int vfldIdx = Manager::Get()->GetProjectManager()->VirtualFolderIconIndex();

            newparent = FindNodeToInsertAfter(tree, folder, parent, true);

            FileTreeData* ftd = new FileTreeData(*data);
            ftd->SetKind(folders_kind);
            if (folders_kind != FileTreeData::ftdkVirtualFolder)
                ftd->SetFolder(m_CommonTopLevelPath + GetRelativeFolderPath(tree, parent) + folder + wxFILE_SEP_PATH);
            else
                ftd->SetFolder(GetRelativeFolderPath(tree, parent) + folder + wxFILE_SEP_PATH);
            ftd->SetProjectFile(0);
            int idx = folders_kind != FileTreeData::ftdkVirtualFolder ? fldIdx : vfldIdx;
            newparent = tree->InsertItem(parent, newparent, folder, idx, idx, ftd);
        }
        //tree->SortChildren(parent);
        ret = AddTreeNode(tree, path, newparent, true, folders_kind, compiles, image, data);
    }
    else
    {
        ret = tree->AppendItem(parent, text, image, image, data);
        if (!compiles)
            tree->SetItemTextColour(ret, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    }
    return ret;
}

wxTreeItemId cbProject::FindNodeToInsertAfter(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool in_folders)
{
    wxTreeItemId result;

    if (tree && parent.IsOk())
    {
        compatibility::tree_cookie_t cookie = 0;

        int fldIdx = Manager::Get()->GetProjectManager()->FolderIconIndex();
        int vfldIdx = Manager::Get()->GetProjectManager()->VirtualFolderIconIndex();
        wxTreeItemId last;
        bool last_is_folder = false;
        wxTreeItemId child = tree->GetFirstChild(parent, cookie);
        while (child)
        {
            bool is_folder = tree->GetItemImage(child) == fldIdx || tree->GetItemImage(child) == vfldIdx;

            if (in_folders)
            {
                if (!is_folder || text.CmpNoCase(tree->GetItemText(child)) < 0)
                {
                    result = last;
                    break;
                }
            }
            else
            {
                if (!is_folder && text.CmpNoCase(tree->GetItemText(child)) < 0)
                {
                    result = last;
                    break;
                }
            }

            last = child;
            last_is_folder = is_folder;
            child = tree->GetNextChild(parent, cookie);
        }
        if (!result.IsOk())
            result = last;
    }

    return result;
}

void cbProject::CopyTreeNodeRecursively(wxTreeCtrl* tree, const wxTreeItemId& item, const wxTreeItemId& new_parent)
{
    // first, some sanity checks
    if (!tree || !item.IsOk() || !new_parent.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(item);
    FileTreeData* ftd_moved = ftd ? new FileTreeData(*ftd) : 0;
    int idx = tree->GetItemImage(item); // old image
    wxColour col = tree->GetItemTextColour(item); // old colour

    wxTreeItemId insert = FindNodeToInsertAfter(tree, tree->GetItemText(item), new_parent, ftd && ftd->GetKind() == FileTreeData::ftdkVirtualFolder);
    wxTreeItemId target = tree->InsertItem(new_parent, insert, tree->GetItemText(item), idx, idx, ftd_moved);
    tree->SetItemTextColour(target, col);

    // recurse for folders
    if (tree->ItemHasChildren(item))
    {
        // vfolder: recurse for files all contained files virtual path
        wxTreeItemIdValue cookie;
        wxTreeItemId child = tree->GetFirstChild(item, cookie);
        while (child.IsOk())
        {
            CopyTreeNodeRecursively(tree, child, target);
            child = tree->GetNextChild(item, cookie);
        }
    }

    if (!tree->IsExpanded(new_parent))
        tree->Expand(new_parent);

    if (ftd_moved->GetProjectFile())
        ftd_moved->GetProjectFile()->virtual_path = GetRelativeFolderPath(tree, new_parent);
}

const wxArrayString& cbProject::GetVirtualFolders() const
{
    return m_VirtualFolders;
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

bool cbProject::CanDragNode(wxTreeCtrl* tree, wxTreeItemId node)
{
    // what item do we start dragging?
    if (!node.IsOk())
        return false;

    // if no data associated with it, disallow
    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(node);
    if (!ftd)
        return false;

    // if not ours, disallow
    if (ftd->GetProject() != this)
        return false;

    // allow only if it is a file or a virtual folder
    return ftd->GetKind() == FileTreeData::ftdkFile || ftd->GetKind() == FileTreeData::ftdkVirtualFolder;
}

bool cbProject::NodeDragged(wxTreeCtrl* tree, wxTreeItemId from, wxTreeItemId to)
{
    // what items did we drag?
    if (!from.IsOk() || !to.IsOk())
        return false;

    // if no data associated with it, disallow
    FileTreeData* ftd1 = (FileTreeData*)tree->GetItemData(from);
    FileTreeData* ftd2 = (FileTreeData*)tree->GetItemData(to);
    if (!ftd1 || !ftd2)
        return false;

    // if not ours, disallow
    if (ftd1->GetProject() != this || ftd2->GetProject() != this)
        return false;

    // allow only if a file or vfolder was dragged on a file, another vfolder or the project itself
    if ((ftd1->GetKind() != FileTreeData::ftdkFile &&
        ftd1->GetKind() != FileTreeData::ftdkVirtualFolder) ||
        (ftd2->GetKind() != FileTreeData::ftdkFile &&
         ftd2->GetKind() != FileTreeData::ftdkVirtualFolder &&
         ftd2->GetKind() != FileTreeData::ftdkProject))
    {
        return false;
    }

    // don't drag under the same parent
    wxTreeItemId parent1 = ftd1->GetKind() == FileTreeData::ftdkFile ? tree->GetItemParent(from) : from;
    wxTreeItemId parent2 = ftd2->GetKind() == FileTreeData::ftdkFile ? tree->GetItemParent(to) : to;
    if (parent1 == parent2)
        return false;

    // A special check for virtual folders.
    if (ftd1->GetKind() == FileTreeData::ftdkVirtualFolder && ftd2->GetKind() == FileTreeData::ftdkVirtualFolder)
    {
        wxTreeItemId root = tree->GetRootItem();
        wxTreeItemId toParent = tree->GetItemParent(to);
        while (toParent != root)
        {
            if (toParent == from)
                return false;
            toParent = tree->GetItemParent(toParent);
        }
    }

    // finally; make the move
    CopyTreeNodeRecursively(tree, from, parent2);
    // remove old node
    tree->Delete(from);

    SetModified(true);

    return true;
}

bool cbProject::VirtualFolderAdded(wxTreeCtrl* tree, wxTreeItemId parent_node, const wxString& virtual_folder)
{
    wxString foldername = GetRelativeFolderPath(tree, parent_node);
    foldername << virtual_folder;
    foldername.Replace(_T("/"), wxString(wxFILE_SEP_PATH), true);
    foldername.Replace(_T("\\"), wxString(wxFILE_SEP_PATH), true);
    if (foldername.Last() != wxFILE_SEP_PATH)
        foldername << wxFILE_SEP_PATH;

    for (size_t i = 0; i < m_VirtualFolders.GetCount(); ++i)
    {
        if (m_VirtualFolders[i].StartsWith(foldername))
        {
            cbMessageBox(_("A virtual folder with the same name already exists."),
                        _("Error"), wxICON_WARNING);
            return false;
        }
    }
    m_VirtualFolders.Add(foldername);

    FileTreeData* ftd = new FileTreeData(this, FileTreeData::ftdkVirtualFolder);
    ftd->SetProjectFile(0);
    ftd->SetFolder(foldername);

    int vfldIdx = Manager::Get()->GetProjectManager()->VirtualFolderIconIndex();

    AddTreeNode(tree, foldername, m_ProjectNode, true, FileTreeData::ftdkVirtualFolder, true, vfldIdx, ftd);
    if (!tree->IsExpanded(parent_node))
        tree->Expand(parent_node);

    SetModified(true);

//    Manager::Get()->GetLogManager()->DebugLog(F(_T("VirtualFolderAdded: %s: %s"), foldername.c_str(), GetStringFromArray(m_VirtualFolders, _T(";")).c_str()));
    return true;
}

void cbProject::VirtualFolderDeleted(wxTreeCtrl* tree, wxTreeItemId node)
{
    // what item do we start dragging?
    if (!node.IsOk())
        return;

    // if no data associated with it, disallow
    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(node);
    if (!ftd)
        return;

    // if not ours, disallow
    if (ftd->GetProject() != this)
        return;

    wxString foldername = GetRelativeFolderPath(tree, node);
    wxString parent_foldername = GetRelativeFolderPath(tree, tree->GetItemParent(node));

    // now loop all project files and remove them from this virtual folder
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        if (f && !f->virtual_path.IsEmpty())
        {
            if (f->virtual_path.StartsWith(foldername)) // need 2 checks because of last separator
                f->virtual_path.Clear();
        }
    }

    for (int i = (int)m_VirtualFolders.GetCount() - 1; i >= 0; --i)
    {
        if (m_VirtualFolders[i].StartsWith(foldername))
            m_VirtualFolders.RemoveAt(i);
    }
    if (!parent_foldername.IsEmpty() && m_VirtualFolders.Index(parent_foldername) == wxNOT_FOUND)
        m_VirtualFolders.Add(parent_foldername);

    SetModified(true);
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("VirtualFolderDeleted: %s: %s"), foldername.c_str(), GetStringFromArray(m_VirtualFolders, _T(";")).c_str()));
}

bool cbProject::VirtualFolderRenamed(wxTreeCtrl* tree, wxTreeItemId node, const wxString& new_name)
{
    if (new_name.IsEmpty())
        return false;

    if (new_name.First(_T(';')) != wxNOT_FOUND ||
        new_name.First(_T('/')) != wxNOT_FOUND ||
        new_name.First(_T('\\')) != wxNOT_FOUND)
    {
        cbMessageBox(_("A virtual folder name cannot contain these special characters: \";\", \"\\\" or \"/\"."),
                    _("Error"), wxICON_WARNING);
        return false;
    }

    // what item are we renaming?
    if (!node.IsOk())
        return false;

    // is it a different name?
    if (tree->GetItemText(node) == new_name)
        return false;

    // if no data associated with it, disallow
    FileTreeData* ftd = (FileTreeData*)tree->GetItemData(node);
    if (!ftd)
        return false;

    // if not ours, disallow
    if (ftd->GetProject() != this)
        return false;

    wxString old_foldername = GetRelativeFolderPath(tree, node);
    wxString new_foldername = GetRelativeFolderPath(tree, tree->GetItemParent(node)) + new_name + wxFILE_SEP_PATH;

    for (size_t i = 0; i < m_VirtualFolders.GetCount(); ++i)
    {
        if (m_VirtualFolders[i].StartsWith(new_foldername))
        {
            cbMessageBox(_("A virtual folder with the same name already exists."),
                        _("Error"), wxICON_WARNING);
            return false;
        }
    }
    int idx = m_VirtualFolders.Index(old_foldername);
    if (idx != wxNOT_FOUND)
        m_VirtualFolders[idx] = new_foldername;
    else
        m_VirtualFolders.Add(new_foldername);

    // now loop all project files and rename this virtual folder
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        if (f && !f->virtual_path.IsEmpty())
        {
            if (f->virtual_path.StartsWith(old_foldername))
                f->virtual_path.Replace(old_foldername, new_foldername);
        }
    }

    SetModified(true);

//    Manager::Get()->GetLogManager()->DebugLog(F(_T("VirtualFolderRenamed: %s to %s: %s"), old_foldername.c_str(), new_foldername.c_str(), GetStringFromArray(m_VirtualFolders, _T(";")).c_str()));
    return true;
}

void cbProject::RenameInTree(const wxString &newname)
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    if(!tree || !m_ProjectNode)
        return;
    tree->SetItemText(m_ProjectNode, newname);
}

void cbProject::SaveTreeState(wxTreeCtrl* tree)
{
    ::SaveTreeState(tree, m_ProjectNode, m_ExpandedNodes);
}

void cbProject::RestoreTreeState(wxTreeCtrl* tree)
{
    ::RestoreTreeState(tree, m_ProjectNode, m_ExpandedNodes);
}

const wxString& cbProject::GetMakefile()
{
    if (!m_Makefile.IsEmpty())
        return m_Makefile;

    wxFileName makefile(m_Makefile);
    makefile.Assign(m_Filename);
    makefile.SetName(_T("Makefile"));
    makefile.SetExt(_T(""));
    makefile.MakeRelativeTo(GetBasePath());

    m_Makefile = makefile.GetFullPath();

    return m_Makefile;
}

ProjectFile* cbProject::GetFile(int index)
{
    FilesList::Node* node = m_Files.Item(index);
    if (node)
        return node->GetData();

    return NULL;
}

ProjectFile* cbProject::GetFileByFilename(const wxString& filename, bool isRelative, bool isUnixFilename)
{
    // m_ProjectFilesMap keeps UnixFilename(ProjectFile::relativeFilename)
    wxString tmp = filename;
    if (!isRelative)
    {
        // if the search is not relative, make it
        wxFileName fname(filename);
        fname.MakeRelativeTo(GetBasePath());
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
    FilesList::Node* node;
    node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(f->file.GetFullPath());
        if (ed && ed->GetModified())
        {
            if (!Manager::Get()->GetEditorManager()->QueryClose(ed))
                return false;
        }
        node = node->GetNext();
    }
    return true;
}

bool cbProject::CloseAllFiles(bool dontsave)
{
    // first try to close modified editors

    if(!dontsave)
        if(!QueryCloseAllFiles())
            return false;

    // now free the rest of the project files
    Manager::Get()->GetEditorManager()->HideNotebook();
    FilesList::Node* node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath(),true);
        delete f;
        delete node;
        node = m_Files.GetFirst();
    }
    Manager::Get()->GetEditorManager()->ShowNotebook();
    return true;
}

bool cbProject::SaveAllFiles()
{
    int count = m_Files.GetCount();
    FilesList::Node* node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        if (Manager::Get()->GetEditorManager()->Save(f->file.GetFullPath()))
            --count;
        node = node->GetNext();
    }
    return count == 0;
}

bool cbProject::ShowOptions()
{
    ProjectOptionsDlg dlg(Manager::Get()->GetAppWindow(), this);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        // update file details
        FilesList::Node* node = m_Files.GetFirst();
        while(node)
        {
            ProjectFile* f = node->GetData();
            f->UpdateFileDetails();
            node = node->GetNext();
        }
        return true;
    }
    return false;
}

int cbProject::SelectTarget(int initial, bool evenIfOne)
{
    if (!evenIfOne && GetBuildTargetsCount() == 1)
        return 0;

    SelectTargetDlg dlg(0L, this, initial);
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
        return 0L;
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
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Deleted existing virtual target '%s' because real target was added with the same name"), targetName.c_str()));
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
            int index = tgts.Index(target->GetTitle());
            if (index != -1)
            {
                tgts[index] = targetName;
            }
        }

        // rename target for all files that reference it
        int count = GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* pf = GetFile(i);
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
    ProjectBuildTarget* newTarget = 0;
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        newTarget = new ProjectBuildTarget(*target);
        wxString newTargetName = !newName.IsEmpty() ? newName : (_("Copy of ") + target->GetTitle());
        newTarget->SetTitle(newTargetName);
        // just notify the files of this target that they belong to the new target too
        for (FilesList::Node* it = newTarget->GetFilesList().GetFirst(); it; it = it->GetNext())
        {
            ProjectFile* pf = it->GetData();
            pf->AddBuildTarget(newTargetName);
        }
        SetModified(true);
        m_Targets.Add(newTarget);
        NotifyPlugins(cbEVT_BUILDTARGET_ADDED, newName);
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
    wxString newName = wxGetTextFromUser(_("Please enter the new project's name (no path, no extension)."),
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
            int index = tgts.Index(target->GetTitle());
            if (index != -1)
            {
                tgts.RemoveAt(index);
            }
        }

        // remove target from any project files that reference it
        int count = GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* pf = GetFile(i);
            pf->RemoveBuildTarget(target->GetTitle());
        }

        // finally remove the target
        delete target;
        m_Targets.RemoveAt(index);
        SetModified(true);
        NotifyPlugins(cbEVT_BUILDTARGET_REMOVED, oldTargetName);
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

    if (!valid)
    {
        // no target (virtual or real) by that name
        m_ActiveTarget = GetFirstValidBuildTargetName();
    }

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
    return 0L;
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
        msgMan->DebugLog(F(_T("cbProject::ReOrderTargets() : Count does not match (%d sent, %d had)..."), nameOrder.GetCount(), m_Targets.GetCount()));
        return;
    }

    for (unsigned int i = 0; i < nameOrder.GetCount(); ++i)
    {
        ProjectBuildTarget* target = GetBuildTarget(nameOrder[i]);
        if (!target)
        {
            msgMan->DebugLog(F(_T("cbProject::ReOrderTargets() : Target \"%s\" not found..."), nameOrder[i].c_str()));
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
                vt.Insert(nameOrder[i], i);
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
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Can't define virtual build target '%s': Group of build targets is empty!"), alias.c_str()));
        return false;
    }

    ProjectBuildTarget* existing = GetBuildTarget(alias);
    if (existing)
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Can't define virtual build target '%s': Real build target exists with that name!"), alias.c_str()));
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

#ifdef USE_OPENFILES_TREE
bool MiscTreeItemData::OwnerCheck(wxTreeEvent& event,wxTreeCtrl *tree,wxEvtHandler *handler,bool strict)
{
    if(!tree)   // No tree to get data from - ignore event
        return false;

    MiscTreeItemData* data =
        (MiscTreeItemData*)tree->GetItemData(event.GetItem());
    if(!data)
    {
        if(!strict)
            return true; // On doubt, allow event
        else
        {
            event.Skip();
            return false;
        }
    }
    wxEvtHandler *h = data->GetOwner();
    if((h && h!=handler) || (strict && !h))
    {   // Tree Item belongs to another handler - skip
        event.Skip();
        return false;
    }
    return true;
}
#endif

void cbProject::SetExtendedObjectNamesGeneration(bool ext)
{
    bool changed = m_ExtendedObjectNamesGeneration != ext;

    // update it now because SetObjName() below will call GetExtendedObjectNamesGeneration()
    // so it must be up-to-date
    m_ExtendedObjectNamesGeneration = ext;

    if (changed)
    {
        for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
        {
            ProjectFile* f = node->GetData();
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
        TiXmlElement* sub = !forceAdd ? elem->FirstChildElement(cbU2C(current)) : 0;
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
