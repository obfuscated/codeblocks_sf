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

