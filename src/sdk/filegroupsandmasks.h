#ifndef FILEGROUPSANDMASKS_H
#define FILEGROUPSANDMASKS_H

#include <wx/dynarray.h>
#include <settings.h>

struct FileGroups
{
    wxString groupName;
    wxArrayString fileMasks;
};

WX_DEFINE_ARRAY(FileGroups*, FileGroupsArray);

class DLLIMPORT FilesGroupsAndMasks
{
    public:
        FilesGroupsAndMasks();
        FilesGroupsAndMasks(FilesGroupsAndMasks& copy);
        ~FilesGroupsAndMasks();
		void CopyFrom(FilesGroupsAndMasks& copy); // copy ctor helper

        unsigned int AddGroup(const wxString& name); // returns the group index
		void RenameGroup(unsigned int group, const wxString& newName);
		void DeleteGroup(unsigned int group);
        void SetFileMasks(unsigned int group, const wxString& masks);

        void Clear();

        unsigned int GetGroupsCount() const;
        wxString GetGroupName(unsigned int group) const;
        wxString GetFileMasks(unsigned int group) const;
        bool MatchesMask(const wxString& ext, unsigned int group);

		void Save();
    private:
		void Load();
        FileGroupsArray m_Groups;
};


#endif // FILEGROUPSANDMASKS_H

