#include "projectfile.h"
#include "projectbuildtarget.h"
#include "cbproject.h"
#include "projectfileoptionsdlg.h"
#include "compilerfactory.h"
#include "manager.h"
#include "projectmanager.h"
#include "macrosmanager.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(FilesList);

ProjectFile::ProjectFile(cbProject* prj)
    : project(prj)
{
    compile = false;
    link = false;
    weight = 50;
    editorOpen = false;
    editorPos = 0;
    editorTopLine = 0;
    useCustomBuildCommand = false;
    autoDeps = true;
    m_VisualState = fvsNormal;
}

ProjectFile::~ProjectFile()
{
    // clear PFDMap
    for (PFDMap::iterator it = m_PFDMap.begin(); it != m_PFDMap.end(); ++it)
    {
        delete it->second;
    }
    m_PFDMap.clear();
}

void ProjectFile::AddBuildTarget(const wxString& targetName)
{
    if (buildTargets.Index(targetName) == wxNOT_FOUND)
        buildTargets.Add(targetName);

    // add this file to the target's list of files
    if (project)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(targetName);
        if (target && !target->m_Files.Find(this))
            target->m_Files.Append(this);
    }
}

void ProjectFile::RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName)
{
    int idx = buildTargets.Index(oldTargetName);
    if (idx != wxNOT_FOUND)
        buildTargets[idx] = newTargetName;
}

void ProjectFile::RemoveBuildTarget(const wxString& targetName)
{
    buildTargets.Remove(targetName);

    // remove this file from the target's list of files
    if (project)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(targetName);
        if (target)
        {
            wxFilesListNode* node = target->m_Files.Find(this);
            if (node)
                target->m_Files.Erase(node);
        }
    }
}

bool ProjectFile::ShowOptions(wxWindow* parent)
{
    ProjectFileOptionsDlg dlg(parent, this);
    return dlg.ShowModal() == wxID_OK;
}

wxString ProjectFile::GetBaseName()
{
    wxFileName fname(relativeFilename);
    fname.SetExt(wxEmptyString);
    return fname.GetFullPath();
}

const wxString& ProjectFile::GetObjName()
{
    if (m_ObjName.IsEmpty())
        SetObjName(relativeToCommonTopLevelPath);
    return m_ObjName;
}

void ProjectFile::SetObjName(const wxString& name)
{
    wxFileName fname(name);
    FileType ft = FileTypeOf(name);
    if (ft == ftResource || ft == ftResourceBin)
        fname.SetExt(RESOURCEBIN_EXT);
    else if (ft == ftHeader) // support precompiled headers
        fname.SetExt(fname.GetExt() + _T(".gch"));
    else
    {
        if (project && CompilerFactory::CompilerIndexOK(project->GetCompilerIndex()))
            fname.SetExt(CompilerFactory::Compilers[project->GetCompilerIndex()]->GetSwitches().objectExtension);
        else
            fname.SetExt(_T(".o")); // fallback?
    }
    m_ObjName = fname.GetFullPath();
}

// map target to pfDetails
void ProjectFile::UpdateFileDetails(ProjectBuildTarget* target)
{
    if (!project)
        return;

    if (!compile && !link)
        return;

    if (!target) // update all targets
    {
        int tcount = project->GetBuildTargetsCount();
        for (int x = 0; x < tcount; ++x)
        {
            ProjectBuildTarget* bt = project->GetBuildTarget(x);
            DoUpdateFileDetails(bt);
        }
    }
    else
        DoUpdateFileDetails(target);
}

void ProjectFile::DoUpdateFileDetails(ProjectBuildTarget* target)
{
    // if we don't belong in this target, abort
    if (!target || buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
        return;
    // delete old PFD
    pfDetails* pfd = m_PFDMap[target];
    if (pfd)
        pfd->Update(target, this);
    else
    {
        pfd = new pfDetails(target, this);
        m_PFDMap[target] = pfd;
    }
}

const pfDetails& ProjectFile::GetFileDetails(ProjectBuildTarget* target)
{
    pfDetails* pfd = m_PFDMap[target];
    if (!pfd)
    {
        DoUpdateFileDetails(target);
        pfd = m_PFDMap[target];
    }
    return *pfd;
}

FileVisualState ProjectFile::GetFileState()
{
    return m_VisualState;
}

void ProjectFile::SetFileState(FileVisualState state)
{
    if (state != m_VisualState)
    {
        m_VisualState = state;
        wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
        if (tree && m_TreeItemId.IsOk())
        {
            tree->SetItemImage(m_TreeItemId, (int)state, wxTreeItemIcon_Normal);
            tree->SetItemImage(m_TreeItemId, (int)state, wxTreeItemIcon_Selected);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// pfDetails
////////////////////////////////////////////////////////////////////////////////

pfDetails::pfDetails(ProjectBuildTarget* target, ProjectFile* pf)
{
    Update(target, pf);
}

void pfDetails::Update(ProjectBuildTarget* target, ProjectFile* pf)
{
    wxString sep = wxFILE_SEP_PATH;
    wxFileName tmp;

    wxFileName prjbase(target->GetParentProject()->GetBasePath());

    source_file_native = pf->relativeFilename;
    source_file_absolute_native = pf->file.GetFullPath();

    tmp = pf->GetObjName();

    // support for precompiled headers
    if (target && FileTypeOf(pf->relativeFilename) == ftHeader)
    {
        switch (target->GetParentProject()->GetModeForPCH())
        {
            case pchSourceDir:
            {
                // if PCH is for a file called all.h, we create
                // all.h.gch/<target>_all.h.gch
                // (that's right: a directory)
                wxString new_gch = target->GetTitle() +
                                    _T("_") +
                                    pf->GetObjName();
                // make sure we 're not generating subdirs
                new_gch.Replace(_T("/"), _T("_"));
                new_gch.Replace(_T("\\"), _T("_"));
                new_gch.Replace(_T(" "), _T("_"));

                object_file_native = source_file_native + _T(".gch") +
                                    wxFILE_SEP_PATH +
                                    new_gch;
                break;
            }

            case pchObjectDir:
            {
                object_file_native = (target ? target->GetObjectOutput() : _T(".")) +
                                      sep +
                                      tmp.GetFullPath();
                break;
            }

            case pchSourceFile:
            {
                object_file_native = pf->GetObjName();
                break;
            }
        }
    }
    else
        object_file_native = (target ? target->GetObjectOutput() : _T(".")) +
                              sep +
                              tmp.GetFullPath();
    wxFileName o_file(object_file_native);
    o_file.MakeAbsolute(prjbase.GetFullPath());
    object_dir_native = o_file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    object_file_absolute_native = o_file.GetFullPath();
    tmp.SetExt(_T("depend"));
    dep_file_native = (target ? target->GetDepsOutput() : _T(".")) +
                      sep +
                      tmp.GetFullPath();
    wxFileName d_file(dep_file_native);
    d_file.MakeAbsolute(prjbase.GetFullPath());
    dep_dir_native = d_file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    dep_file_absolute_native = o_file.GetFullPath();

    source_file = UnixFilename(source_file_native);
    QuoteStringIfNeeded(source_file);

    object_file = UnixFilename(object_file_native);
    QuoteStringIfNeeded(object_file);

    dep_file = UnixFilename(dep_file_native);
    QuoteStringIfNeeded(dep_file);

    object_dir = UnixFilename(object_dir_native);
    QuoteStringIfNeeded(object_dir);

    dep_dir = UnixFilename(dep_dir_native);
    QuoteStringIfNeeded(dep_dir);

    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(object_file_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(object_dir_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(object_file_absolute_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dep_file_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dep_dir_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dep_file_absolute_native);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dep_dir);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(object_dir);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dep_file);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(object_file);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(source_file);
}
