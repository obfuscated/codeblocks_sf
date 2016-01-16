/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef GENERICMULTILINENOTESDLG_H
#define GENERICMULTILINENOTESDLG_H

#include "scrollingdialog.h"
#include <wx/intl.h>


class GenericMultiLineNotesDlg : public wxScrollingDialog
{
    public:
        GenericMultiLineNotesDlg(wxWindow* parent, const wxString& caption = _("Notes"), const wxString& notes = wxEmptyString, bool readOnly = true);
        virtual ~GenericMultiLineNotesDlg();
		GenericMultiLineNotesDlg& operator=(const GenericMultiLineNotesDlg&){ return *this; } // just to satisfy script bindings (never used)

        const wxString& GetNotes(){ return m_Notes; }
        void EndModal(int retCode);
    protected:
        wxString m_Notes;
        bool m_ReadOnly;
    private:
        DECLARE_EVENT_TABLE()
};



#endif // GENERICMULTILINENOTESDLG_H
