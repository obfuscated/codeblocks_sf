/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "highlighter.h"

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <algorithm>
    #include <configmanager.h>
    #include <cbeditor.h>
    #include <editormanager.h>
#endif

#include <cbstyledtextctrl.h>
#include <cbcolourmanager.h>

enum Indicator : int {
    Background = 10,
    TextBackground,
    Selection,
    TextSelection
};

Highlighter::Highlighter(std::set<wxString> &texts):
    m_Texts(texts),
    m_AlreadyChecked(false),
    m_OldCtrl(NULL),
    m_HighlightSelectedOccurencesLastPos(-1,-1),
    m_OldHighlightSelectionCtrl(NULL)
{
}

Highlighter::~Highlighter()
{
    ClearAllIndications();
}

void Highlighter::Call(cbEditor* ctrl, wxScintillaEvent &event) const
{
    // return if this event is not fired from the active editor (is it possible that an editor which is not active fires an event?)
    if ( Manager::Get()->GetEditorManager()->GetActiveEditor() != ctrl  ) return;

    // check the event type if it is an update event
    if ( event.GetEventType() == wxEVT_SCI_UPDATEUI ||
         event.GetEventType() == wxEVT_SCI_PAINTED )
    {
        HighlightOccurrencesOfSelection(ctrl);
        OnEditorUpdateUI(ctrl);
    }
    else if ( event.GetEventType() == wxEVT_SCI_MODIFIED)
    {
        if(event.GetModificationType() & wxSCI_MOD_INSERTTEXT)
        {
            OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition() + event.GetLength());
        }
        else if (event.GetModificationType() & wxSCI_MOD_DELETETEXT)
        {
            OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition());
        }
        else if (event.GetModificationType() & wxSCI_MOD_CHANGESTYLE)
        {
            OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition() + event.GetLength());
        }
    }

}

void Highlighter::OnEditorUpdateUI(cbEditor* ctrl) const
{
    DoSetIndications(ctrl);
}

void Highlighter::OnEditorChangeTextRange(cbEditor* ctrl, int start, int end)const
{
    //if ( !m_doChecks ) return;
    if ( m_AlreadyChecked && m_OldCtrl == ctrl )
    {
        //only recheck the last word to speed things up

        cbStyledTextCtrl *stc = ctrl->GetLeftSplitViewControl();

        //swap and bound check
        if (end < start)
        {
            int t = start;
            start = end;
            end = t;
        }

        //find recheck range start:
        start = stc->PositionFromLine(stc->LineFromPosition(start));

        //find recheck range end:
        end = stc->GetLineEndPosition(stc->LineFromPosition(end));

        if (   m_InvalidatedRangesStart.GetCount() == 0
            || m_InvalidatedRangesStart.Last() != start
            || m_InvalidatedRangesEnd.Last() != end )
        {
            m_InvalidatedRangesStart.Add(start);
            m_InvalidatedRangesEnd.Add(end);
        }
    }
    else
    {
        m_AlreadyChecked = false;
    }
}

void Highlighter::TextsChanged()const
{
    EditorManager *edmgr = Manager::Get()->GetEditorManager();
    if(!edmgr)
        return;

    for ( int index = 0 ; index < edmgr->GetEditorsCount() ; ++index )
    {
        cbEditor *ed = edmgr->GetBuiltinEditor(index);
        if ( ed )
        {
            m_AlreadyChecked = false;
            m_OldCtrl = NULL;
            DoSetIndications(ed);
        }
    }
}

static void SetupIndicator(cbStyledTextCtrl *control, int indicator, const wxColor &colour,
                           int alpha, int borderAlpha, bool overrideText)
{
    control->IndicatorSetForeground(indicator, colour);
    control->IndicatorSetStyle(indicator, wxSCI_INDIC_ROUNDBOX);
    control->IndicatorSetAlpha(indicator, alpha);
    control->IndicatorSetOutlineAlpha(indicator, borderAlpha);
    if (overrideText)
        control->IndicatorSetUnder(indicator, true); // make sure the box is below the text
    else
    {
#ifndef wxHAVE_RAW_BITMAP
        // If wxWidgets is build without rawbitmap-support, the indicators become opaque
        // and hide the text, so we show them under the text.
        // Not enabled as default, because the readability is a little bit worse.
        control->IndicatorSetUnder(indicator, true);
#endif
    }
}

static void SetupTextIndicator(cbStyledTextCtrl *control, int indicator, const wxColor &colour)
{
    control->IndicatorSetForeground(indicator, colour);
    control->IndicatorSetStyle(indicator, wxSCI_INDIC_TEXTFORE);
    control->IndicatorSetUnder(indicator, false);
}

void Highlighter::DoSetIndications(cbEditor* ctrl)const
{
    cbStyledTextCtrl *stc = ctrl->GetLeftSplitViewControl();
    //Returns a pointer to the left (or top) split-view cbStyledTextCtrl. This function always returns a valid pointer.
    cbStyledTextCtrl *stcr =ctrl->GetRightSplitViewControl();

    if (m_AlreadyChecked && m_OldCtrl == ctrl)
    {
        if ( m_InvalidatedRangesStart.GetCount() == 0)
            return;
    }
    else
    {
        //clear:
        m_InvalidatedRangesStart.Clear();
        m_InvalidatedRangesEnd.Clear();
        //add whole document
        m_InvalidatedRangesStart.Add(0);
        m_InvalidatedRangesEnd.Add(stc->GetLength());
    }

    m_AlreadyChecked = true;

    // Set Styling:

    //if(stc->SelectionIsRectangle() || (stcr && stcr->SelectionIsRectangle())) return;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));// /highlight_occurrence"));

    const int alpha = cfg->ReadInt(_T("/highlight_occurrence/alpha_permanently"), 100);
    const int borderAlpha = cfg->ReadInt(_T("/highlight_occurrence/border_alpha_permanently"), 255);
    const bool overrideText = cfg->ReadBool(_T("/highlight_occurrence/override_text_permanently"), false);

    if (m_OldCtrl != ctrl)
    {
        ColourManager *colourManager = Manager::Get()->GetColourManager();
        const wxColour background = colourManager->GetColour(wxT("editor_highlight_occurrence_permanently"));
        const wxColour text = colourManager->GetColour(wxT("editor_highlight_occurrence_permanently_text"));

        SetupIndicator(stc, Indicator::Background, background, alpha, borderAlpha, overrideText);
        SetupTextIndicator(stc, Indicator::TextBackground, text);

        if (stcr)
        {
            SetupIndicator(stcr, Indicator::Background, background, alpha, borderAlpha, overrideText);
            SetupTextIndicator(stcr, Indicator::TextBackground, text);
        }
    }

    m_OldCtrl = ctrl;

    int flag = 0;
    if (cfg->ReadBool(_T("/highlight_occurrence/case_sensitive_permanently"), true))
        flag |= wxSCI_FIND_MATCHCASE;

    if (cfg->ReadBool(_T("/highlight_occurrence/whole_word_permanently"), true))
        flag |= wxSCI_FIND_WHOLEWORD;

    for (int i = 0; i < (int)m_InvalidatedRangesStart.GetCount(); i++)
    {
        int start = m_InvalidatedRangesStart[i];
        int end = m_InvalidatedRangesEnd[i];
        //bound:
        if (start < 0) start = 0;
        if (end   < 0) end   = 0;

        if (start >= stc->GetLength()) start = stc->GetLength() - 1;
        if (end   >  stc->GetLength()) end   = stc->GetLength();

        if (start != end)
        {
            //remove styling:
            stc->SetIndicatorCurrent(Indicator::Background);
            stc->IndicatorClearRange(start, end - start);
            stc->SetIndicatorCurrent(Indicator::TextBackground);
            stc->IndicatorClearRange(start, end - start);
            stc->SetIndicatorCurrent(Indicator::Background);

            for (std::set<wxString>::iterator it = m_Texts.begin();
                 it != m_Texts.end(); it++ )
            {
                wxString text = *it;

                // search for occurrences
                int startpos = start;
                int endpos = end;

                int endPos = 0; // we need this to work properly with multibyte characters
                for ( int pos = stc->FindText(startpos, endpos, text, flag, &endPos);
                    pos != wxSCI_INVALID_POSITION ;
                    pos = stc->FindText(pos+=text.Len(), endpos, text, flag, &endPos) )
                {
                    if (overrideText)
                    {
                        stc->SetIndicatorCurrent(Indicator::Background);
                        stc->IndicatorFillRange(pos, endPos - pos);

                        stc->SetIndicatorCurrent(Indicator::TextBackground);
                        stc->IndicatorFillRange(pos, endPos - pos);
                    }
                    else
                        stc->IndicatorFillRange(pos, endPos - pos);
                }
            }
        }
    }
    m_InvalidatedRangesStart.Clear();
    m_InvalidatedRangesEnd.Clear();
}

void Highlighter::ClearAllIndications(cbStyledTextCtrl* stc)const
{
    if ( stc )
    {
        stc->SetIndicatorCurrent(Indicator::Background);
        stc->IndicatorClearRange(0, stc->GetLength());
        stc->SetIndicatorCurrent(Indicator::TextBackground);
        stc->IndicatorClearRange(0, stc->GetLength());
    }
}

void Highlighter::ClearAllIndications()const
{
    EditorManager *edm = Manager::Get()->GetEditorManager();
    for ( int i = 0 ; i < edm->GetEditorsCount() ; ++i)
    {
        cbEditor *ed = edm->GetBuiltinEditor( i );
        if ( ed )
            ClearAllIndications(ed->GetControl());
    }
}

void Highlighter::HighlightOccurrencesOfSelection(cbEditor* ctrl)const
{
    // chosen a high value for indicator, hoping not to interfere with the indicators used by some lexers
    // if they get updated from deprecated oldstyle indicators somedays.
    cbStyledTextCtrl *control = ctrl->GetControl();

    std::pair<long, long> curr;
    control->GetSelection(&curr.first, &curr.second);

    if (m_OldHighlightSelectionCtrl == control && m_HighlightSelectedOccurencesLastPos == curr) // whatever the current state is, we've already done it once
        return;

    m_HighlightSelectedOccurencesLastPos = curr;
    m_OldHighlightSelectionCtrl = control;

    int eof = control->GetLength();

    // Set Styling:
    // clear all style indications set in a previous run (is also done once after text gets unselected)
    control->SetIndicatorCurrent(Indicator::Selection);
    control->IndicatorClearRange(0, eof);
    control->SetIndicatorCurrent(Indicator::TextSelection);
    control->IndicatorClearRange(0, eof);

    // if there is no text selected, it stops here and does not hog the cpu further
    if (curr.first == curr.second)
        return;
    // check if the selected text has space, tab or new line in it
    wxString selectedText(control->GetTextRange(curr.first, curr.second));
    if (selectedText.find_first_of(wxT(" \t\n")) != wxString::npos)
        return;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // check if the feature is enabled
    if (!cfg->ReadBool(_T("/highlight_occurrence/enabled"), true))
        return;

    // selected text has a minimal length of controlled by the user (by default it is 3)
    wxString::size_type minLength = std::max(cfg->ReadInt(_T("/highlight_occurrence/min_length"), 3), 1);
    if (selectedText.length() >= minLength)
    {
        ColourManager *colourManager = Manager::Get()->GetColourManager();
        const wxColour highlightColour(colourManager->GetColour(wxT("editor_highlight_occurrence")));
        const wxColour highlightTextColour(colourManager->GetColour(wxT("editor_highlight_occurrence_text")));

        const int alpha = cfg->ReadInt(_T("/highlight_occurrence/alpha"), 100);
        const int borderAlpha = cfg->ReadInt(_T("/highlight_occurrence/border_alpha"), 255);
        const bool overrideText = cfg->ReadBool(_T("/highlight_occurrence/override_text"), false);
        if (!overrideText)
            control->SetIndicatorCurrent(Indicator::Selection);

        if ( auto *stc = ctrl->GetLeftSplitViewControl() )
        {
            SetupIndicator(stc, Indicator::Selection, highlightColour, alpha, borderAlpha,
                           overrideText);
            SetupTextIndicator(stc, Indicator::TextSelection, highlightTextColour);
        }
        if ( auto *stc = ctrl->GetRightSplitViewControl() )
        {
            SetupIndicator(stc, Indicator::Selection, highlightColour, alpha, borderAlpha,
                           overrideText);
            SetupTextIndicator(stc, Indicator::TextSelection, highlightTextColour);
        }

        int flag = 0;
        if (cfg->ReadBool(_T("/highlight_occurrence/case_sensitive"), true))
        {
            flag |= wxSCI_FIND_MATCHCASE;
        }
        if (cfg->ReadBool(_T("/highlight_occurrence/whole_word"), true))
        {
            flag |= wxSCI_FIND_WHOLEWORD;
        }

        // list all selections and sort them
        typedef std::vector<std::pair<long, long> > Selections;
        Selections selections;
        int count = control->GetSelections();
        for (int ii = 0; ii < count; ++ii)
        {
            selections.push_back(Selections::value_type(control->GetSelectionNStart(ii),
                                                        control->GetSelectionNEnd(ii)));
        }
        std::sort(selections.begin(), selections.end());
        Selections::const_iterator currSelection = selections.begin();

        // search for every occurence
        int endPos = 0; // we need this to work properly with multibyte characters
        for ( int pos = control->FindText(0, eof, selectedText, flag, &endPos);
            pos != wxSCI_INVALID_POSITION ;
            pos = control->FindText(pos+=selectedText.Len(), eof, selectedText, flag, &endPos) )
        {
            // check if the found text is selected
            // if it is don't add indicator for it, because it looks ugly
            bool skip = false;
            for (; currSelection != selections.end(); ++currSelection)
            {
                // the found text is after the current selection, go to the next one
                if (currSelection->second < pos)
                    continue;
                // if the end of the found text is not before the current selection start
                // then it must match and it should be skipped
                if (endPos >= currSelection->first)
                    skip = true;
                break;
            }
            if (skip)
                continue;

            if (overrideText)
            {
                control->SetIndicatorCurrent(Indicator::Selection);
                control->IndicatorFillRange(pos, endPos - pos);
                control->SetIndicatorCurrent(Indicator::TextSelection);
                control->IndicatorFillRange(pos, endPos - pos);
            }
            else
                control->IndicatorFillRange(pos, endPos - pos);
        }
    }
}
