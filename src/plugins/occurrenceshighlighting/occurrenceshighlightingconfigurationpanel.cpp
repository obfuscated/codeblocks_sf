/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 8868 $
 * $Id: OccurrencesHighlightingConfigurationPanel.cpp 8868 2013-02-26 23:49:49Z tpetrov $
 * $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/abbreviations/OccurrencesHighlightingConfigurationPanel.cpp $
 */

#include <sdk.h>
#include "occurrenceshighlightingconfigurationpanel.h"

#ifndef CB_PRECOMP
    #include <algorithm>
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/colordlg.h>
    #include <wx/stattext.h>
    #include <wx/spinctrl.h>.h>

    #include <configmanager.h>
    #include <editormanager.h>
    #include <logmanager.h>
#endif

//#include <wx/choicdlg.h>

#include "cbcolourmanager.h"

BEGIN_EVENT_TABLE(OccurrencesHighlightingConfigurationPanel, cbConfigurationPanel)
    EVT_BUTTON(XRCID("btnHighlightColour"),            OccurrencesHighlightingConfigurationPanel::OnChooseColour)
    EVT_BUTTON(XRCID("btnHighlightPermanentlyColour"), OccurrencesHighlightingConfigurationPanel::OnChooseColour)
    EVT_CHECKBOX(XRCID("chkHighlightOccurrences"),     OccurrencesHighlightingConfigurationPanel::OnCheckHighlightOccurrences)

END_EVENT_TABLE()

OccurrencesHighlightingConfigurationPanel::OccurrencesHighlightingConfigurationPanel(wxWindow* parent)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    if(!cfg)
        return;

    if(!wxXmlResource::Get()->LoadObject(this, parent, _T("OccurrencesHighlightingConfigurationPanel"), _T("wxPanel")))
        return;
    // Highlight Occurrence
    bool highlightEnabled = cfg->ReadBool(_T("/highlight_occurrence/enabled"), true);
    XRCCTRL(*this, "chkHighlightOccurrences",              wxCheckBox)->SetValue(highlightEnabled);
    XRCCTRL(*this, "chkHighlightOccurrencesCaseSensitive", wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_occurrence/case_sensitive"), true));
    XRCCTRL(*this, "chkHighlightOccurrencesCaseSensitive", wxCheckBox)->Enable(highlightEnabled);
    XRCCTRL(*this, "chkHighlightOccurrencesWholeWord",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_occurrence/whole_word"), true));
    XRCCTRL(*this, "chkHighlightOccurrencesWholeWord",     wxCheckBox)->Enable(highlightEnabled);

    wxColour highlightColour = Manager::Get()->GetColourManager()->GetColour(wxT("editor_highlight_occurrence"));
    XRCCTRL(*this, "btnHighlightColour",                   wxButton)->SetBackgroundColour(highlightColour);
    XRCCTRL(*this, "stHighlightColour",                    wxStaticText)->Enable(highlightEnabled);
    XRCCTRL(*this, "btnHighlightColour",                   wxButton)->Enable(highlightEnabled);

    wxSpinCtrl *minLength = XRCCTRL(*this, "spnHighlightLength", wxSpinCtrl);
    minLength->SetValue(cfg->ReadInt(_T("/highlight_occurrence/min_length"), 3));
    minLength->Enable(highlightEnabled);
    XRCCTRL(*this, "stHighlightLength", wxStaticText)->Enable(highlightEnabled);


    XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyCaseSensitive", wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_occurrence/case_sensitive_permanently"), true));
    //XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyCaseSensitive", wxCheckBox)->Enable(true);
    XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyWholeWord",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_occurrence/whole_word_permanently"), true));
    //XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyWholeWord",     wxCheckBox)->Enable(true);
    highlightColour = Manager::Get()->GetColourManager()->GetColour(wxT("editor_highlight_occurrence_permanently"));
    XRCCTRL(*this, "btnHighlightPermanentlyColour",                   wxButton)->SetBackgroundColour(highlightColour);
    //XRCCTRL(*this, "stHighlightPermanentlyColour",                    wxStaticText)->Enable(permanentlyHighlightEnabled);
    //XRCCTRL(*this, "btnHighlightPermanentlyColour",                   wxButton)->Enable(permanentlyHighlightEnabled);

}

OccurrencesHighlightingConfigurationPanel::~OccurrencesHighlightingConfigurationPanel()
{
}

void OccurrencesHighlightingConfigurationPanel::OnApply()
{
    // save any changes
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    cfg->Write(_T("/highlight_occurrence/enabled"),        XRCCTRL(*this, "chkHighlightOccurrences",              wxCheckBox)->GetValue());
    cfg->Write(_T("/highlight_occurrence/case_sensitive"), XRCCTRL(*this, "chkHighlightOccurrencesCaseSensitive", wxCheckBox)->GetValue());
    cfg->Write(_T("/highlight_occurrence/whole_word"),     XRCCTRL(*this, "chkHighlightOccurrencesWholeWord",     wxCheckBox)->GetValue());
    wxColour highlightColour = XRCCTRL(*this, "btnHighlightColour", wxButton)->GetBackgroundColour();
    Manager::Get()->GetColourManager()->SetColour(wxT("editor_highlight_occurrence"), highlightColour);
    cfg->Write(_T("/highlight_occurrence/min_length"),     XRCCTRL(*this, "spnHighlightLength",                   wxSpinCtrl)->GetValue());


    cfg->Write(_T("/highlight_occurrence/case_sensitive_permanently"), XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyCaseSensitive", wxCheckBox)->GetValue());
    cfg->Write(_T("/highlight_occurrence/whole_word_permanently"),     XRCCTRL(*this, "chkHighlightOccurrencesPermanentlyWholeWord",     wxCheckBox)->GetValue());
    highlightColour = XRCCTRL(*this, "btnHighlightPermanentlyColour", wxButton)->GetBackgroundColour();
    Manager::Get()->GetColourManager()->SetColour(wxT("editor_highlight_occurrence_permanently"), highlightColour);
}

void OccurrencesHighlightingConfigurationPanel::OnCancel()
{}

wxString OccurrencesHighlightingConfigurationPanel::GetTitle() const
{ return _("Occurrences Highlighting"); }

wxString OccurrencesHighlightingConfigurationPanel::GetBitmapBaseName() const
{ return _T("occurrenceshighlighting"); }

void OccurrencesHighlightingConfigurationPanel::OnChooseColour(wxCommandEvent& event)
{
    wxColourData data;
    wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

    wxColourDialog dlg(this, &data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour colour = dlg.GetColourData().GetColour();
        sender->SetBackgroundColour(colour);
        sender->SetLabel(wxEmptyString);
    }
}

void OccurrencesHighlightingConfigurationPanel::OnCheckHighlightOccurrences(wxCommandEvent& event)
{
    XRCCTRL(*this, "chkHighlightOccurrencesCaseSensitive", wxCheckBox)->Enable( event.IsChecked() );
    XRCCTRL(*this, "chkHighlightOccurrencesWholeWord", wxCheckBox)->Enable( event.IsChecked() );
    XRCCTRL(*this, "stHighlightColour", wxStaticText)->Enable( event.IsChecked() );
    XRCCTRL(*this, "btnHighlightColour", wxButton)->Enable( event.IsChecked() );
    XRCCTRL(*this, "spnHighlightLength", wxSpinCtrl)->Enable( event.IsChecked() );
}

