/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef __DLGHELPSYSTEMINFORMATION_H__
#define __DLGHELPSYSTEMINFORMATION_H__

#include <scrollingdialog.h>

class wxWindow;

class dlgHelpSystemInformation : public wxScrollingDialog
{
    public:
        // class constructor
        dlgHelpSystemInformation(wxWindow* parent);
        // class destructor
        ~dlgHelpSystemInformation();
    private:
        void CopyInfoToClipbaord(cb_unused wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};

#endif // __DLGHELPSYSTEMINFORMATION_H__
