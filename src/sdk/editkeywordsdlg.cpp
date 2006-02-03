/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk_precomp.h>

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
#endif

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
