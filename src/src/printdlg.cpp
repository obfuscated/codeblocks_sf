#include "printdlg.h"
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/radiobox.h>

PrintDialog::PrintDialog(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, "dlgPrint");
}

PrintDialog::~PrintDialog()
{
	//dtor
}

PrintScope PrintDialog::GetPrintScope()
{
    return (PrintScope)XRCCTRL(*this, "rbScope", wxRadioBox)->GetSelection();
}

PrintColorMode PrintDialog::GetPrintColorMode()
{
    return (PrintColorMode)XRCCTRL(*this, "rbColorMode", wxRadioBox)->GetSelection();
}
