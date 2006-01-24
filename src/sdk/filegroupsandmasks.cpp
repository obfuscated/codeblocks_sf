#include "sdk_precomp.h"
#include <wx/intl.h>
#include "globals.h"
#include "configmanager.h"
#include "filegroupsandmasks.h"
#include "manager.h"
#include "messagemanager.h"

FilesGroupsAndMasks::FilesGroupsAndMasks()
{
	//ctor
	Load();

	if (m_Groups.GetCount() == 0)
	{
		// only add default groups if none were loaded...
		unsigned int group = AddGroup(_("Sources"));
		SetFileMasks(group, _T("*.c;*.cpp;*.cc;*.cxx;*.C;*.CPP;*.CC;*.CXX") );
		group = AddGroup(_("Headers"));
		SetFileMasks(group, _T("*.h;*.hpp;*.hh;*.hxx;*.H;*.HPP;*.HH;*.HXX") );
		group = AddGroup(_("Resources"));
		SetFileMasks(group, _T("*.res;*.xrc;*.rc;*.RES;*.XRC;*.RC") );
	}
}

FilesGroupsAndMasks::FilesGroupsAndMasks(FilesGroupsAndMasks& copy)
{
	// copy ctor
	CopyFrom(copy);
}

FilesGroupsAndMasks::~FilesGroupsAndMasks()
{
	//dtor
	Save();
    Clear();
}

void FilesGroupsAndMasks::CopyFrom(FilesGroupsAndMasks& copy)
{
    Clear();
    for (unsigned int i = 0; i < copy.m_Groups.GetCount(); ++i)
    {
		FileGroups* fg = new FileGroups;
        FileGroups* otherfg = copy.m_Groups[i];
		fg->groupName = otherfg->groupName;
		fg->fileMasks = otherfg->fileMasks;

		m_Groups.Add(fg);
    }
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
        key << _("/file_groups/group") << wxString::Format(_T("%d"), i) << _T("/") << _T("name");
		conf->Write(key, fg->groupName);
        key.Clear();
        key << _("/file_groups/group") << wxString::Format(_T("%d"), i) << _T("/") << _T("mask");
		conf->Write(key, GetStringFromArray(fg->fileMasks, _T(";")));
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
    fg->fileMasks = GetArrayFromString(masks, _T(";"));
}

unsigned int FilesGroupsAndMasks::GetGroupsCount() const
{
    return m_Groups.GetCount();
}

wxString FilesGroupsAndMasks::GetGroupName(unsigned int group) const
{
    if (group >= m_Groups.GetCount())
        return wxEmptyString;
    FileGroups* fg = m_Groups[group];
    return fg->groupName;
}

wxString FilesGroupsAndMasks::GetFileMasks(unsigned int group) const
{
    if (group >= m_Groups.GetCount())
        return wxEmptyString;
    FileGroups* fg = m_Groups[group];
    return GetStringFromArray(fg->fileMasks);
}

bool FilesGroupsAndMasks::MatchesMask(const wxString& ext, unsigned int group)
{
    if (ext.IsEmpty() || group >= m_Groups.GetCount())
        return false;
    FileGroups* fg = m_Groups[group];
    for (unsigned int i = 0; i < fg->fileMasks.GetCount(); ++i)
    {
        if (ext.Matches(fg->fileMasks[i]))
            return true;
    }
    return false;
}
