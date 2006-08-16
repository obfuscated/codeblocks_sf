#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/string.h>
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
    return static_cast<TypeCorrection>(XRCCTRL(*this, "chcCorrect", wxChoice)->GetSelection());
}

