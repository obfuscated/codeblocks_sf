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

#include "sdk_precomp.h"
#include "projectbuildtarget.h" // class's header file
#include "cbproject.h"
#include "compilerfactory.h"
#include "projectfileoptionsdlg.h"
#include "manager.h"
#include "projectmanager.h"
#include "macrosmanager.h"
#include "globals.h"
#include "customvars.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(FilesList);

// class constructor
ProjectBuildTarget::ProjectBuildTarget(cbProject* parentProject) : m_Project(parentProject)
{
    m_BuildWithAll = false;
    m_CreateStaticLib = true;
    m_CreateDefFile = true;
    m_UseConsoleRunner = true;
}

// class destructor
ProjectBuildTarget::~ProjectBuildTarget()
{
}

cbProject* ProjectBuildTarget::GetParentProject() {
    return m_Project;
}

wxString ProjectBuildTarget::GetFullTitle() {
    return m_Project->GetTitle() + _T(" - ") + GetTitle();
}

const wxString & ProjectBuildTarget::GetExternalDeps()
{
    return m_ExternalDeps;
}

void ProjectBuildTarget::SetExternalDeps(const wxString& deps)
{
    if (m_ExternalDeps != deps)
    {
        m_ExternalDeps = deps;
        SetModified(true);
    }
}

const wxString & ProjectBuildTarget::GetAdditionalOutputFiles()
{
    return m_AdditionalOutputFiles;
}

void ProjectBuildTarget::SetAdditionalOutputFiles(const wxString& files)
{
    if (m_AdditionalOutputFiles != files)
    {
        m_AdditionalOutputFiles = files;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetIncludeInTargetAll()
{
	return m_BuildWithAll;
}

void ProjectBuildTarget::SetIncludeInTargetAll(bool buildIt)
{
	if (m_BuildWithAll != buildIt)
	{
        m_BuildWithAll = buildIt;
        SetModified(true);
	}
}

bool ProjectBuildTarget::GetCreateDefFile()
{
    return m_CreateDefFile;
}

void ProjectBuildTarget::SetCreateDefFile(bool createIt)
{
    if (m_CreateDefFile != createIt)
    {
        m_CreateDefFile = createIt;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetCreateStaticLib()
{
    return m_CreateStaticLib;
}

void ProjectBuildTarget::SetCreateStaticLib(bool createIt)
{
    if (m_CreateStaticLib != createIt)
    {
        m_CreateStaticLib = createIt;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetUseConsoleRunner()
{
    return GetTargetType() == ttConsoleOnly ? m_UseConsoleRunner : false;
}

void ProjectBuildTarget::SetUseConsoleRunner(bool useIt)
{
    if (GetTargetType() == ttConsoleOnly && useIt != m_UseConsoleRunner)
    {
        m_UseConsoleRunner = useIt;
        SetModified(true);
    }
}

void ProjectBuildTarget::SetTargetType(const TargetType& pt)
{
	TargetType ttold = GetTargetType();
	CompileTargetBase::SetTargetType(pt);
	if (ttold != GetTargetType() && GetTargetType() == ttConsoleOnly)
        SetUseConsoleRunner(true); // by default, use console runner
}

// target dependencies: targets to be compiled (if necessary) before this one
void ProjectBuildTarget::AddTargetDep(ProjectBuildTarget* target) {
	m_TargetDeps.Add(target);
}

// get the list of dependency targets of this target
BuildTargets& ProjectBuildTarget::GetTargetDeps() {
	return m_TargetDeps;
}


//// PROJECTFILE //////////////////////

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
