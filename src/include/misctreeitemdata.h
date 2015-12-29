/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MISCTREEITEMDATA_H
#define MISCTREEITEMDATA_H

#include <wx/treectrl.h>

/* New class: MiscTreeItemData.
 * This class allows a TreeData to be created specifying
 * which Event Handler should process its related events.
 */

class DLLIMPORT MiscTreeItemData : public wxTreeItemData
{
    public:
        MiscTreeItemData():m_owner(0L) {}
        wxEvtHandler *GetOwner() { return m_owner; }
        static bool OwnerCheck(wxTreeEvent& event,wxTreeCtrl *tree,wxEvtHandler *handler,bool strict=false);
        virtual ~MiscTreeItemData() { m_owner=0L; }
    protected:
        void SetOwner(wxEvtHandler *owner) { m_owner=owner; }
    private:
        wxEvtHandler *m_owner;
};

#endif // MISCTREEITEMDATA_H
