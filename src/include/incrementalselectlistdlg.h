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
        // overwrite the virtual functions from base class
        virtual void GetCurrentSelection(int &sel, size_t &selMax);
        virtual void UpdateCurrentSelection(int sel, size_t selPrevious);
        virtual void FillData();
    private:
        DECLARE_EVENT_TABLE();
};

#endif // INCREMENTALSELECTLISTDLG_H

