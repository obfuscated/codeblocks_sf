/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

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
        FilesGroupsAndMasks(const FilesGroupsAndMasks& rhs);
        ~FilesGroupsAndMasks();
        /** copy ctor helper */
        void CopyFrom(const FilesGroupsAndMasks& rhs); // copy ctor helper

        /** Set the default file groups and masks
          * \param do_clear Clear any old groups/masks before */
        void SetDefault(bool do_clear = true);
        /** Save groups/masks to config */
        void Save();
        /** Clear any groups/masks */
        void Clear();

        /** Add a file group
          * \param name File group name
          * \return The group index */
        unsigned int AddGroup(const wxString& name);
        /** Rename a group
          * \param group Group index to rename
          * \param newName New name for the group */
        void RenameGroup(unsigned int group, const wxString& newName);
        /** Delete a group
          * \param group Group index to delete */
        void DeleteGroup(unsigned int group);
        /** Set file mask for a group (e.g. *.c;*.cpp)
          * \param group Group index to set
          * \param masks File mask to set */
        void SetFileMasks(unsigned int group, const wxString& masks);

        /** Return total number of groups
          * \return Total number of groups */
        unsigned int GetGroupsCount() const;
        /** Return a specific group name
          * \param group Group index to query
          * \return The group's name */
        wxString GetGroupName(unsigned int group) const;
        /** Return a specific group file mask
          * \param group Group index to query
          * \return The group's file masks */
        wxString GetFileMasks(unsigned int group) const;
        /** Return whether a file extension matches a file mask (group)
          * \param ext The extension to query
          * \param group Group index to match
          * \return Extension matches the group in question? */
        bool MatchesMask(const wxString& ext, unsigned int group);

    private:
        /** Load groups/masks from config */
        void Load();
        FileGroupsArray m_Groups; //!< Internal storage for file groups ans masks
};

#endif // FILEGROUPSANDMASKS_H
