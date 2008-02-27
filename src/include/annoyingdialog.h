/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "settings.h"
#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/artprov.h>

class wxCheckBox;

/**
Dialog that contains a "Don't annoy me" checkbox.

Using this dialog, the user can select not to display this dialog again.
The dialog can be then re-enabled in "Settings->Environment->Dialogs"...
*/
class DLLIMPORT AnnoyingDialog : public wxDialog
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

        AnnoyingDialog(const wxString& caption, const wxString& message, const wxArtID icon = wxART_INFORMATION,
                       dStyle style = YES_NO, int defaultReturn = wxID_YES, bool separate = true,
                       const wxString& b1 = wxEmptyString, const wxString& b2 = wxEmptyString, const wxString& b3 = wxEmptyString);
        virtual ~AnnoyingDialog(){}
        virtual int ShowModal();
    private:
        void OnButton(wxCommandEvent& event);
        wxCheckBox *cb;
        bool dontAnnoy;
        int defRet;
        DECLARE_EVENT_TABLE()
};
