/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EDITWATCHESDLG_H
#define EDITWATCHESDLG_H

#include <wx/dialog.h>
#include "debugger_defs.h"

class EditWatchesDlg : public wxDialog
{
    public:
        EditWatchesDlg(WatchesArray& arr, wxWindow* parent = 0);
        virtual ~EditWatchesDlg();
    protected:
        void FillWatches();
        void FillRecord(int sel);
        void EndModal(int retCode);

        void OnAdd(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);
        void OnListboxClick(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        int m_LastSel;
        WatchesArray& m_Watches;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITWATCHESDLG_H
