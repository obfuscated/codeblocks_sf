/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DATABREAKPOINTDLG_H
#define DATABREAKPOINTDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(DataBreakpointDlg)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include "scrollingdialog.h"
//*)

class DataBreakpointDlg: public wxScrollingDialog
{
    public:

        DataBreakpointDlg(wxWindow *parent, const wxString& dataExpression, bool enabled, int selection);
        virtual ~DataBreakpointDlg();

        //(*Identifiers(DataBreakpointDlg)
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL_DATA_EXPRESION;
        static const long ID_RADIOBOX1;
        //*)

        bool IsBreakpointEnabled();
        int GetSelection();
        wxString GetDataExpression() const;

    protected:

        //(*Handlers(DataBreakpointDlg)
        //*)

        //(*Declarations(DataBreakpointDlg)
        wxCheckBox* CheckBox1;
        wxTextCtrl* m_dataExpressionCtrl;
        wxBoxSizer* BoxSizer1;
        wxRadioBox* RadioBox1;
        wxStdDialogButtonSizer* StdDialogButtonSizer1;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
