/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EDITWATCHDLG_H
#define EDITWATCHDLG_H

#include "scrollingdialog.h"

#include "debugger_defs.h"

class EditWatchDlg : public wxScrollingDialog
{
    public:
        EditWatchDlg(cb::shared_ptr<GDBWatch> w, wxWindow* parent);
        virtual ~EditWatchDlg();

    protected:
        void EndModal(int retCode);

        cb::shared_ptr<GDBWatch> m_watch;
};

#endif // EDITWATCHDLG_H
