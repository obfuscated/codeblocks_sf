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
    #include <wx/spinctrl.h>
    #include <wx/textctrl.h>
    #include <wx/wxscintilla.h>
    #include "editorcolourset.h"
#endif

#include "editkeywordsdlg.h"

BEGIN_EVENT_TABLE(EditKeywordsDlg, wxDialog)
    EVT_SPINCTRL(-1, EditKeywordsDlg::OnSetChange)
END_EVENT_TABLE()

EditKeywordsDlg::EditKeywordsDlg(wxWindow* parent, EditorColourSet* theme, HighlightLanguage lang)
    : m_pTheme(theme),
    m_Lang(lang)
{
    //ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditLangKeywords"));

    spnSet = XRCCTRL(*this, "spnSet", wxSpinCtrl);
	txtKeywords = XRCCTRL(*this, "txtKeywords", wxTextCtrl);

    m_LastSet = spnSet->GetValue() - 1;
	txtKeywords->SetValue(m_pTheme->GetKeywords(m_Lang, m_LastSet));
}

EditKeywordsDlg::~EditKeywordsDlg()
{
    //dtor
}

void EditKeywordsDlg::OnSetChange(wxSpinEvent& event)
{
	m_pTheme->SetKeywords(m_Lang, m_LastSet, txtKeywords->GetValue());
	m_LastSet = spnSet->GetValue() - 1;
	txtKeywords->SetValue(m_pTheme->GetKeywords(m_Lang, m_LastSet));
}
