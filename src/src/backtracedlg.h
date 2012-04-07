/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CB_BACKTRACEDLG_H
#define CB_BACKTRACEDLG_H

#include <wx/panel.h>

#include <cbdebugger_interfaces.h>
#include <manager.h>

class wxCommandEvent;
class wxListCtrl;
class wxListEvent;
class wxWindow;

class BacktraceDlg : public wxPanel, public cbBacktraceDlg
{
    public:
        BacktraceDlg(wxWindow* parent);

        wxWindow* GetWindow() { return this; }

        void Reload();
        void EnableWindow(bool enable);
    private:
        void OnListRightClick(wxListEvent& event);
        void OnDoubleClick(wxListEvent& event);
        void OnJump(wxCommandEvent& event);
        void OnSwitchFrame(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnCopyToClipboard(wxCommandEvent& event);

        void OnSettingJumpDefault(wxCommandEvent& event);
        void OnSettingSwitchDefault(wxCommandEvent& event);

        void OnUpdateUI(wxUpdateUIEvent &event);

    private:
        DECLARE_EVENT_TABLE();
    private:
        wxListCtrl *m_list;
};

#endif // CB_BACKTRACEDLG_H
