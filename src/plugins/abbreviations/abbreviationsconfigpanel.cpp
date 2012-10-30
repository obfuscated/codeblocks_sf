/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "abbreviationsconfigpanel.h"

#include <editormanager.h>
#include <editorcolourset.h>
#include <configmanager.h>

#include <wx/xrc/xmlres.h>
#include <wx/fontutil.h>


BEGIN_EVENT_TABLE(AbbreviationsConfigPanel, cbConfigurationPanel)
END_EVENT_TABLE()

AbbreviationsConfigPanel::AbbreviationsConfigPanel(wxWindow* parent, Abbreviations* plugin) :
    m_AutoCompTextControl(0L),
    m_LastAutoCompKeyword(-1),
    m_Plugin(plugin)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("AbbreviationsConfigPanel"), _T("wxPanel"));

    InitCompText();
    m_Keyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    m_Keyword->Clear();
    m_AutoCompMap = m_Plugin->m_AutoCompleteMap;

    for (AutoCompleteMap::iterator it = m_AutoCompMap.begin(); it != m_AutoCompMap.end(); ++it)
        m_Keyword->Append(it->first);

    if (!m_AutoCompMap.empty())
    {
        m_LastAutoCompKeyword = 0;
        m_Keyword->SetSelection(0);
    }
    m_AutoCompTextControl->SetText(m_AutoCompMap[m_Keyword->GetString(m_Keyword->GetSelection())]);

    Connect(XRCID("lstAutoCompKeyword"), wxEVT_COMMAND_LISTBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompKeyword);
    Connect(XRCID("btnAutoCompAdd"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompAdd);
    Connect(XRCID("btnAutoCompDelete"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompDelete);
}

AbbreviationsConfigPanel::~AbbreviationsConfigPanel()
{
    Disconnect(XRCID("lstAutoCompKeyword"), wxEVT_COMMAND_LISTBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompKeyword);
    Disconnect(XRCID("btnAutoCompAdd"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompAdd);
    Disconnect(XRCID("btnAutoCompDelete"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompDelete);

    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;
}

void AbbreviationsConfigPanel::InitCompText()
{
    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;

    m_AutoCompTextControl = new cbStyledTextCtrl(this, wxID_ANY);
    m_AutoCompTextControl->SetTabWidth(4);
    m_AutoCompTextControl->SetMarginType(0, wxSCI_MARGIN_NUMBER);
    m_AutoCompTextControl->SetMarginWidth(0, 32);
    m_AutoCompTextControl->SetViewWhiteSpace(1);
    m_AutoCompTextControl->SetMinSize(wxSize(50, 50));

    wxColor ccolor = Manager::Get()->GetConfigManager(_T("editor"))->ReadColour(_T("/caret/colour"), *wxBLACK );
    m_AutoCompTextControl->SetCaretForeground( ccolor );

    ApplyColours();
    wxXmlResource::Get()->AttachUnknownControl(_T("txtAutoCompCode"), m_AutoCompTextControl);
}

void AbbreviationsConfigPanel::ApplyColours()
{
    EditorColourSet* theme = Manager::Get()->GetEditorManager()->GetColourSet();
    if (theme)
    {
        wxString FontString = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
        wxFont tmpFont(10, wxMODERN, wxNORMAL, wxNORMAL);

        if (!FontString.IsEmpty())
        {
            wxNativeFontInfo nfi;
            nfi.FromString(FontString);
            tmpFont.SetNativeFontInfo(nfi);
        }
        if (m_AutoCompTextControl)
        {
            m_AutoCompTextControl->StyleSetFont(wxSCI_STYLE_DEFAULT, tmpFont);
            theme->Apply(theme->GetHighlightLanguage(_T("C/C++")), m_AutoCompTextControl);
        }
    }
}

void AbbreviationsConfigPanel::AutoCompUpdate(int index)
{
    if (index != -1)
    {
        wxString lastSel = m_Keyword->GetString(index);
        m_AutoCompMap[lastSel] = m_AutoCompTextControl->GetText();
    }
}

void AbbreviationsConfigPanel::OnAutoCompAdd(wxCommandEvent& event)
{
    wxString key = wxGetTextFromUser(_("Please enter the new keyword"), _("Add keyword"));
    if (!key.IsEmpty())
    {
        AutoCompleteMap::iterator it = m_AutoCompMap.find(key);
        if (it != m_AutoCompMap.end())
        {
            cbMessageBox(_("This keyword already exists!"), _("Error"), wxICON_ERROR, this);
            return;
        }
        m_AutoCompMap[key] = wxEmptyString;
        m_LastAutoCompKeyword = m_Keyword->Append(key);
        m_Keyword->SetSelection( m_LastAutoCompKeyword );
        AutoCompUpdate(m_Keyword->GetSelection());
        m_AutoCompTextControl->SetText(wxEmptyString);
    }
}

void AbbreviationsConfigPanel::OnAutoCompDelete(wxCommandEvent& event)
{
    if (m_Keyword->GetSelection() == -1)
        return;

    if (cbMessageBox(_("Are you sure you want to delete this keyword?"), _("Confirmation"),
                     wxICON_QUESTION | wxYES_NO, this) == wxID_NO)
        return;

    int sel = m_Keyword->GetSelection();
    AutoCompleteMap::iterator it = m_AutoCompMap.find(m_Keyword->GetString(sel));
    if (it != m_AutoCompMap.end())
    {
        m_AutoCompMap.erase(it);
        m_Keyword->Delete(sel);
        if (sel >= (int)(m_Keyword->GetCount()))
            sel = m_Keyword->GetCount() - 1;
        m_Keyword->SetSelection(sel);
        if (sel != -1)
        {
            m_AutoCompTextControl->SetText(m_AutoCompMap[m_Keyword->GetString(sel)]);
            m_LastAutoCompKeyword = sel;
        }
        else
            m_AutoCompTextControl->SetText(wxEmptyString);
    }
}

void AbbreviationsConfigPanel::OnAutoCompKeyword(wxCommandEvent& event)
{
    if (m_Keyword->GetSelection() == m_LastAutoCompKeyword)
        return;

    AutoCompUpdate(m_LastAutoCompKeyword);
    // list new keyword's code
    m_AutoCompTextControl->SetText(m_AutoCompMap[m_Keyword->GetString(m_Keyword->GetSelection())]);
    m_LastAutoCompKeyword = m_Keyword->GetSelection();
}

void AbbreviationsConfigPanel::OnApply()
{
    // save any changes in auto-completion
    AutoCompUpdate(m_Keyword->GetSelection());
    Abbreviations::ExchangeTabAndSpaces(m_AutoCompMap);
    m_Plugin->m_AutoCompleteMap = m_AutoCompMap;
}

void AbbreviationsConfigPanel::OnCancel()
{
}

wxString AbbreviationsConfigPanel::GetTitle() const
{
  return _("Abbreviations");
}

wxString AbbreviationsConfigPanel::GetBitmapBaseName() const
{
  return _T("abbrev");
}
