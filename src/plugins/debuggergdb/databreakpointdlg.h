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
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include "scrollingdialog.h"
//*)

class DataBreakpointDlg: public wxScrollingDialog
{
    public:

        DataBreakpointDlg(wxWindow* parent,wxWindowID id = -1,bool enabled=true,int selection=1);
        virtual ~DataBreakpointDlg();

        //(*Identifiers(DataBreakpointDlg)
        static const long ID_CHECKBOX1;
        static const long ID_RADIOBOX1;
        //*)

        bool IsEnabled();
        int GetSelection();

    protected:

        //(*Handlers(DataBreakpointDlg)
        //*)

        //(*Declarations(DataBreakpointDlg)
        wxCheckBox* CheckBox1;
        wxBoxSizer* BoxSizer1;
        wxRadioBox* RadioBox1;
        wxStdDialogButtonSizer* StdDialogButtonSizer1;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
