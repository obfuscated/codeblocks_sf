/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ANNOYINGDIALOG_H
#define ANNOYINGDIALOG_H

#include "settings.h"
#include "scrollingdialog.h"
#include <wx/string.h>
#include <wx/artprov.h>

class wxCheckBox;

/**
Dialog that contains a "Don't annoy me" checkbox.

Using this dialog, the user can select not to display this dialog again.
The dialog can be then re-enabled in "Settings->Environment->Dialogs"...
*/
class DLLIMPORT AnnoyingDialog : public wxScrollingDialog
{
    public:
        enum dStyle
        {
            OK,
            YES_NO,
            YES_NO_CANCEL,
            OK_CANCEL,
            ONE_BUTTON,
            TWO_BUTTONS,
            THREE_BUTTONS
        };

        enum dReturnType
        {
            rtINVALID = -1,
            rtSAVE_CHOICE,
            rtONE,
            rtTWO,
            rtTHREE,
            rtYES,
            rtNO,
            rtOK,
            rtCANCEL
        };

        AnnoyingDialog(const wxString& caption, const wxString& message, const wxArtID icon = wxART_INFORMATION,
                       dStyle style = YES_NO, dReturnType defaultReturn = rtYES,
                       const wxString& b1 = wxEmptyString, const wxString& b2 = wxEmptyString, const wxString& b3 = wxEmptyString);
        AnnoyingDialog(const wxString& caption, const wxString &id, const wxString& message, const wxArtID icon,
                       dStyle style, dReturnType defaultReturn,
                       const wxString& b1 = wxEmptyString, const wxString& b2 = wxEmptyString, const wxString& b3 = wxEmptyString);
        virtual ~AnnoyingDialog(){}
        virtual int ShowModal();
    private:
        void Init(const wxString &caption, const wxString &id, const wxString& message, const wxArtID icon,
                  dStyle style, const wxString& b1, const wxString& b2, const wxString& b3);
        void OnButton(wxCommandEvent& event);
    private:
        wxString m_Id;
        wxCheckBox *m_CheckBox;
        dReturnType m_DefRet;
        bool m_DontAnnoy;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // ANNOYINGDIALOG_H
