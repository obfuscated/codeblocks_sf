#include <wx/intl.h>
#include "globals.h"
#include "configmanager.h"
#include "filegroupsandmasks.h"
#include "manager.h"
#include "messagemanager.h"

#define CONF_GROUP "/project_manager/file_groups/"

FilesGroupsAndMasks::FilesGroupsAndMasks()
{
	//ctor
	Load();
	
	if (m_Groups.GetCount() == 0)
	{
		// only add default groups if none were loaded...
		unsigned int group = AddGroup(_("Sources"));
		SetFileMasks(group, "*.c;*.cpp;*.cc;*.cxx;*.C;*.CPP;*.CC;*.CXX");
		group = AddGroup(_("Headers"));
		SetFileMasks(group, "*.h;*.hpp;*.hh;*.hxx;*.H;*.HPP;*.HH;*.HXX");
		group = AddGroup(_("Resources"));
		SetFileMasks(group, "*.res;*.xrc;*.RES;*.XRC");
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
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
		unsigned int group = AddGroup(entry);
		SetFileMasks(group, conf->Read(entry));
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);
}

void FilesGroupsAndMasks::Save()
{
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup(CONF_GROUP);
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	for (unsigned int i = 0; i < m_Groups.GetCount(); ++i)
	{
        FileGroups* fg = m_Groups[i];
		conf->Write(fg->groupName, GetStringFromArray(fg->fileMasks, ";"));
	}
	conf->SetPath(oldPath);
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
    if (group < 0 || group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
	fg->groupName = newName;
}

void FilesGroupsAndMasks::DeleteGroup(unsigned int group)
{
    if (group < 0 || group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
	delete fg;
	m_Groups.Remove(fg);
}

void FilesGroupsAndMasks::SetFileMasks(unsigned int group, const wxString& masks)
{
    if (group < 0 || group >= m_Groups.GetCount())
        return;

    FileGroups* fg = m_Groups[group];
    fg->fileMasks = GetArrayFromString(masks, ";");
}

unsigned int FilesGroupsAndMasks::GetGroupsCount() const
{
    return m_Groups.GetCount();
}

wxString FilesGroupsAndMasks::GetGroupName(unsigned int group) const
{
    if (group < 0 || group >= m_Groups.GetCount())
        return wxEmptyString;
    FileGroups* fg = m_Groups[group];
    return fg->groupName;
}

wxString FilesGroupsAndMasks::GetFileMasks(unsigned int group) const
{
    if (group < 0 || group >= m_Groups.GetCount())
        return wxEmptyString;
    FileGroups* fg = m_Groups[group];
    return GetStringFromArray(fg->fileMasks);
}

bool FilesGroupsAndMasks::MatchesMask(const wxString& ext, unsigned int group)
{
    if (ext.IsEmpty() || group < 0 || group >= m_Groups.GetCount())
        return false;
    FileGroups* fg = m_Groups[group];
    for (unsigned int i = 0; i < fg->fileMasks.GetCount(); ++i)
    {
        if (ext.Matches(fg->fileMasks[i]))
            return true;
    }
    return false;
}
