#include <sdk_precomp.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include "editkeywordsdlg.h"

EditKeywordsDlg::EditKeywordsDlg(wxWindow* parent, EditorColorSet* theme, HighlightLanguage lang)
{
    //ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditLangKeywords"));

	XRCCTRL(*this, "txtKeywords1", wxTextCtrl)->SetValue(theme->GetKeywords(lang, 0));
	XRCCTRL(*this, "txtKeywords2", wxTextCtrl)->SetValue(theme->GetKeywords(lang, 1));
	XRCCTRL(*this, "txtKeywords3", wxTextCtrl)->SetValue(theme->GetKeywords(lang, 2));
}

EditKeywordsDlg::~EditKeywordsDlg()
{
    //dtor
}

wxString EditKeywordsDlg::GetLangKeywords()
{
    return XRCCTRL(*this, "txtKeywords1", wxTextCtrl)->GetValue();
}

wxString EditKeywordsDlg::GetDocKeywords()
{
    return XRCCTRL(*this, "txtKeywords2", wxTextCtrl)->GetValue();
}

wxString EditKeywordsDlg::GetUserKeywords()
{
    return XRCCTRL(*this, "txtKeywords3", wxTextCtrl)->GetValue();
}
