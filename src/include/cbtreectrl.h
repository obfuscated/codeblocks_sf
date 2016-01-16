/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBTREECTRL_H
#define CBTREECTRL_H

#include <wx/treectrl.h>

class ProjectFile;
class wxKeyEvent;
class wxMouseEvent;
class wxTreeItemId;
class wxWindow;

/*
    This is a "proxy" wxTreeCtrl descendant handles several usage limitations.
*/
class DLLIMPORT cbTreeCtrl : public wxTreeCtrl
{
    public:
        cbTreeCtrl();
        cbTreeCtrl(wxWindow* parent, int id);
        void SetCompareFunction(const int ptvs);
/*
        GetPrevVisible appears to be faulty, so override.
*/
        virtual wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;
    protected:
#ifndef __WXMSW__
/*
        Under wxGTK, wxTreeCtrl is not sending an EVT_COMMAND_RIGHT_CLICK
        event when right-clicking on the client area.
*/
        void OnRightClick(wxMouseEvent& event);
#endif // !__WXMSW__
/*
        Under all platforms there is no reaction when pressing "ENTER".
        Expected would be e.g. to open the file in an editor.
*/
        void OnKeyDown(wxKeyEvent& event);

        static int filesSort(const ProjectFile* arg1, const ProjectFile* arg2);
        static int filesSortNameOnly(const ProjectFile* arg1, const ProjectFile* arg2);
        int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
        int (*Compare)(const ProjectFile* arg1, const ProjectFile* arg2);

        DECLARE_DYNAMIC_CLASS(cbTreeCtrl)
        DECLARE_EVENT_TABLE()
};

#endif // CBTREECTRL_H
