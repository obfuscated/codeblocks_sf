/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASKTYPEDLG_H
#define ASKTYPEDLG_H

#include <wx/dialog.h>

enum TypeCorrection
{
    tcCppMove = 0, // keep C++ style and move to end of line
    tcCppStay, // keep C++ style and use the current position
    tcCpp2C, // switch to c style
};

class AskTypeDlg : public wxDialog
{
    public:
        AskTypeDlg(wxWindow* parent);
        virtual ~AskTypeDlg();

        TypeCorrection GetTypeCorrection() const;
};

#endif // ASKTYPEDLG_H

