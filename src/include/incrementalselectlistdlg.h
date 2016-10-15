/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTDLG_H
#define INCREMENTALSELECTLISTDLG_H

#include "settings.h"
#include "scrollingdialog.h"
#include "incrementalselectlistbase.h"
#include <wx/textctrl.h>
#include <wx/listbox.h>

class DLLIMPORT IncrementalSelectListDlg : public IncrementalSelectListBase
{
    public:
        IncrementalSelectListDlg(wxWindow* parent,
                                 const IncrementalSelectIterator& iterator,
                                 const wxString& caption = wxEmptyString,
                                 const wxString& message = wxEmptyString);
        virtual ~IncrementalSelectListDlg();
        wxString GetStringSelection();
        wxIntPtr GetSelection();
    protected:
        // override the virtual functions from base class
        void GetCurrentSelection(int &selected, int &selectedMax) override;
        void UpdateCurrentSelection(int selected, int selectedPrevious) override;
        void FillData() override;
    private:
        DECLARE_EVENT_TABLE();
};

#endif // INCREMENTALSELECTLISTDLG_H

