#include <wx/intl.h>
#include "globals.h"
#include "configmanager.h"
#include "filegroupsandmasks.h"
#include "manager.h"
#include "messagemanager.h"

#define CONF_GROUP _T("/project_manager/file_groups/")

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
		SetFileMasks(group, _T("*.res;*.xrc;*.RES;*.XRC") );
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
	if (conf->GetNumberOfGroups(false) == 0)
	{
		// old way (reading keys)
        bool cont = conf->GetFirstEntry(entry, cookie);
        while (cont)
        {
            unsigned int group = AddGroup(entry);
            SetFileMasks(group, conf->Read(entry));
            cont = conf->GetNextEntry(entry, cookie);
        }
	}
	else
	{
		// new way (reading groups)
        bool cont = conf->GetFirstGroup(entry, cookie);
        while (cont)
        {
            unsigned int group = AddGroup(conf->Read(entry + _T("/Name")));
            SetFileMasks(group, conf->Read(entry + _T("/Mask")));
            cont = conf->GetNextGroup(entry, cookie);
        }
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
        wxString key;
        key << i << _T("/") << _T("Name");
		conf->Write(key, fg->groupName);
        key.Clear();
        key << i << _T("/") << _T("Mask");
		conf->Write(key, GetStringFromArray(fg->fileMasks, _T(";")));
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
    fg->fileMasks = GetArrayFromString(masks, _T(";"));
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
