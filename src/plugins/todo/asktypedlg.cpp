#include <wx/xrc/xmlres.h>
#include <wx/combobox.h>
#include "asktypedlg.h"


AskTypeDlg::AskTypeDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAskType"));
}

AskTypeDlg::~AskTypeDlg()
{
	//dtor
}

TypeCorrection AskTypeDlg::GetTypeCorrection() const
{
    return static_cast<TypeCorrection>(XRCCTRL(*this, "cmbCorrect", wxComboBox)->GetSelection());
}

#if 0
void AskTypeDlg::EndModal(int retVal)
{
	wxDialog::EndModal(retVal);
}
#endif
