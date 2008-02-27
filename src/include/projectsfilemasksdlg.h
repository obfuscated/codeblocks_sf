/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTSFILEMASKSDLG_H
#define PROJECTSFILEMASKSDLG_H

#include <wx/dialog.h>
#include "filegroupsandmasks.h"

class ProjectsFileMasksDlg : public wxDialog
{
    public:
        ProjectsFileMasksDlg(wxWindow* parent, FilesGroupsAndMasks* fgam);
        virtual ~ProjectsFileMasksDlg();

        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnAdd(wxCommandEvent& event);
        void OnEdit(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnSetDefault(wxCommandEvent& event);
        void OnListChanged(wxCommandEvent& event);
    protected:
    private:
        void RebuildList();
        void ListChange();

        virtual void EndModal(int retCode);
        FilesGroupsAndMasks m_FileGroups;
        FilesGroupsAndMasks* m_pOrigFileGroups;
        int m_LastListSelection;

        DECLARE_EVENT_TABLE();
};

#endif // PROJECTSFILEMASKSDLG_H
