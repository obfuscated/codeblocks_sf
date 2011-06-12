/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>

    #include "globals.h" // GetArrayFromString
    #include "configmanager.h"
    #include "manager.h"
    #include "logmanager.h"
#endif

#include "filegroupsandmasks.h"

FilesGroupsAndMasks::FilesGroupsAndMasks()
{
    //ctor
    Load();

    if (m_Groups.GetCount() == 0)
        SetDefault(false); // No need to clear any groups
}

FilesGroupsAndMasks::FilesGroupsAndMasks(const FilesGroupsAndMasks& rhs)
{
    // copy ctor
    CopyFrom(rhs);
}

FilesGroupsAndMasks::~FilesGroupsAndMasks()
{
    //dtor
    Save();
    Clear();
}

void FilesGroupsAndMasks::CopyFrom(const FilesGroupsAndMasks& rhs)
{
    Clear();
    for (unsigned int i = 0; i < rhs.m_Groups.GetCount(); ++i)
    {
        FileGroups* fg      = new FileGroups;
        FileGroups* otherfg = rhs.m_Groups[i];
        fg->groupName = otherfg->groupName;
        fg->fileMasks = otherfg->fileMasks;

        m_Groups.Add(fg);
    }
}

void FilesGroupsAndMasks::SetDefault(bool do_clear)
{
    if (do_clear)
        Clear();

    // only add default groups if none were loaded...
    unsigned int group;

    group = AddGroup(_("Sources"));
    SetFileMasks(group, _T("*.c;*.cpp;*.cc;*.cxx") );

    group = AddGroup(_("D Sources"));
    SetFileMasks(group, _T("*.d") );

    group = AddGroup(_("Fortran Sources"));
    SetFileMasks(group, _T("*.f;*.f77;*.f90;*.f95") );

    group = AddGroup(_("Java Sources"));
    SetFileMasks(group, _T("*.java") );

    group = AddGroup(_("Headers"));
    SetFileMasks(group, _T("*.h;*.hpp;*.hh;*.hxx") );

    group = AddGroup(_("ASM Sources"));
    SetFileMasks(group, _T("*.asm;*.s;*.ss;*.s62") );

    group = AddGroup(_("Resources"));
    SetFileMasks(group, _T("*.res;*.xrc;*.rc;*.wxs") );

    group = AddGroup(_("Scripts"));
    SetFileMasks(group, _T("*.script") );
}

void FilesGroupsAndMasks::Load()
{
    Clear();
    ConfigManager* conf = Manager::Get()->GetConfigManager(_T("project_manager"));
    wxArrayString list = conf->EnumerateSubPaths(_T("/file_groups"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        // new way (reading groups)
        wxString key = _T("/file_groups/") + list[i];
        unsigned int group = AddGroup(conf->Read(key + _T("/name")));
        SetFileMasks(group, conf->Read(key + _T("/mask")));
    }
}

void FilesGroupsAndMasks::Save()
{
    ConfigManager* conf = Manager::Get()->GetConfigManager(_T("project_manager"));
    conf->DeleteSubPath(_T("/file_groups"));
    for (unsigned int i = 0; i < m_Groups.GetCount(); ++i)
    {
        FileGroups* fg = m_Groups[i];
        wxString key;
        key << _T("/file_groups/group") << wxString::Format(_T("%d"), i) << _T("/") << _T("name");
        conf->Write(key, fg->groupName);

        key.Clear();
        key << _T("/file_groups/group") << wxString::Format(_T("%d"), i) << _T("/") << _T("mask");
        conf->Write(key, GetStringFromArray(CleanUpDoublets(fg->fileMasks), _T(";")));
    }
}

void FilesGroupsAndMasks::Clear()
{
    for (unsigned int i = 0; i < m_Groups.GetCount(); ++i)
    {
        FileGroups* fg = m_Groups[i];
        if (fg)
            delete fg;
    }
    m_Groups.Clear();
}

unsigned int FilesGroupsAndMasks::AddGroup(const wxString& name)
{
    FileGroups* fg = new FileGroups;
    fg->groupName = name;
    m_Groups.Add(fg);
    return m_Groups.GetCount() - 1;
}

void FilesGroupsAndMasks::RenameGroup(unsigned int group, const wxString& newName)
{
    if (group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
    fg->groupName = newName;
}

void FilesGroupsAndMasks::DeleteGroup(unsigned int group)
{
    if (group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
    delete fg;
    m_Groups.Remove(fg);
}

void FilesGroupsAndMasks::SetFileMasks(unsigned int group, const wxString& masks)
{
    if (group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
    fg->fileMasks = CleanUpDoublets( GetArrayFromString(masks, _T(";")) );
}

unsigned int FilesGroupsAndMasks::GetGroupsCount() const
{
    return m_Groups.GetCount();
}

wxString FilesGroupsAndMasks::GetGroupName(unsigned int group) const
{
    if (group >= m_Groups.GetCount())
        return wxEmptyString;
    const FileGroups* fg = m_Groups[group];
    return fg->groupName;
}

wxString FilesGroupsAndMasks::GetFileMasks(unsigned int group) const
{
    if (group >= m_Groups.GetCount())
        return wxEmptyString;
    const FileGroups* fg = m_Groups[group];

    return GetStringFromArray( CleanUpDoublets(fg->fileMasks) );
}

bool FilesGroupsAndMasks::MatchesMask(const wxString& ext, unsigned int group) const
{
    if (ext.IsEmpty() || group >= m_Groups.GetCount())
        return false;
    const FileGroups* fg = m_Groups[group];
    for (unsigned int i = 0; i < fg->fileMasks.GetCount(); ++i)
    {
        if (ext.Lower().Matches(fg->fileMasks[i].Lower()))
            return true;
    }
    return false;
}

wxArrayString FilesGroupsAndMasks::CleanUpDoublets(const wxArrayString& masks) const
{
    wxArrayString masks_cleaned_up;
    for (unsigned int i = 0; i < masks.GetCount(); ++i)
    {
        if (masks_cleaned_up.Index(masks[i].Lower()) == wxNOT_FOUND)
            masks_cleaned_up.Add(masks[i].Lower());
    }
    return masks_cleaned_up;
}
