/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef GOTOFUNCTIONDLG_H
#define GOTOFUNCTIONDLG_H

#include "settings.h"
#include "scrollingdialog.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>

#include "incrementalselectlistbase.h"

/** the goto function dialog allow user to type a function name, and filter out the functions
 *
 * For example, you have three functions in a cpp file
 * @code
 *  int fa1();
 *  int fa2();
 *  int fb1();
 * @endcode
 * When user type "fa" in the text ctrl, only "fa1" and "fa2" will shown in the list.
 * The mode option defines how the list is shown. If user want to show the function
 * return type, function name and function parameters in different columns, then
 * the check box should be selected
 */
class GotoFunctionDlg : public IncrementalSelectListBase
{
    public:
        GotoFunctionDlg(wxWindow* parent,
                                 const IncrementalSelectIterator& iterator);
        virtual ~GotoFunctionDlg();
        wxIntPtr GetSelection();
        wxListCtrl* GetListCtrl();
    protected:
        void SetupMode();
        void FillData() override;
        /** for functions like int f(), we need three columns to display
         * the "int", "f", and "()".
         */
        void FillColumn();
        void GetCurrentSelection(int &selected, int &selectedMax) override;
        void UpdateCurrentSelection(int selected, int selectedPrevious) override;

        void OnColumnSelect(cb_unused wxListEvent& event);
        void OnModeChange(cb_unused wxCommandEvent& event);

        /** the control show filtered functions */
        wxListCtrl* m_ListColumn;
        /** a check box to switch different mode of list ctrl */
        wxCheckBox* m_Mode;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // GOTOFUNCTIONDLG_H

