/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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
