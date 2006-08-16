#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/combobox.h>
#include <wx/xrc/xmlres.h>
#endif
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

