/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASKTYPEDLG_H
#define ASKTYPEDLG_H

#include "scrollingdialog.h"

enum TypeCorrection
{
    tcMove = 0, // keep style and move to end of line
    tcStay,     // keep style and use the current position
    tcSwitch,   // switch to stream style
};

class AskTypeDlg : public wxScrollingDialog
{
    public:
        AskTypeDlg(wxWindow* parent, const wxString StreamStart = _T(""), const wxString StreamEnd = _T("") );
        virtual ~AskTypeDlg();

        TypeCorrection GetTypeCorrection() const;
};

#endif // ASKTYPEDLG_H

