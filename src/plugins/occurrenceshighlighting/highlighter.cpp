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

HighlightedEditorPositions::HighlightedEditorPositions():
    linea(-1),
    lineb(-1),
    ed(NULL)
{
}

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

void Highlighter::OnEditorChange(cbEditor* ctrl) const
{
    // clear internal states to force a refresh at next UpdateUI;
    m_AlreadyChecked = false;
}

const int Highlighter::GetIndicator()const
{
    const int theIndicator = 12;
    return theIndicator;
}

const wxColor Highlighter::GetIndicatorColor()const
{
    return Manager::Get()->GetColourManager()->GetColour(wxT("editor_highlight_occurrence_permanently"));
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
    stc->SetIndicatorCurrent(GetIndicator());

    //if(stc->SelectionIsRectangle() || (stcr && stcr->SelectionIsRectangle())) return;

    if (m_OldCtrl != ctrl)
    {
        stc->IndicatorSetStyle(GetIndicator(), wxSCI_INDIC_HIGHLIGHT);
        stc->IndicatorSetForeground(GetIndicator(), GetIndicatorColor() );
#ifndef wxHAVE_RAW_BITMAP
        // If wxWidgets is build without rawbitmap-support, the indicators become opaque
        // and hide the text, so we show them under the text.
        // Not enabled as default, because the readability is a little bit worse.
        stc->IndicatorSetUnder(theIndicator,true);
#endif
    }

    if (stcr)
    {
        if (m_OldCtrl != ctrl)
        {
            stcr->SetIndicatorCurrent(GetIndicator());
            stcr->IndicatorSetStyle(GetIndicator(), wxSCI_INDIC_HIGHLIGHT);
            stcr->IndicatorSetForeground(GetIndicator(), GetIndicatorColor() );
#ifndef wxHAVE_RAW_BITMAP
            stcr->IndicatorSetUnder(theIndicator,true);
#endif
        }
    }

    m_OldCtrl = ctrl;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
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
            stc->IndicatorClearRange(start, end - start);

            for (std::set<wxString>::iterator it = m_Texts.begin();
                 it != m_Texts.end(); it++ )
            {
                wxString text = *it;

                // search for occurrences
                int startpos = start;
                int endpos = end;

                int lengthFound = 0; // we need this to work properly with multibyte characters
                for ( int pos = stc->FindText(startpos, endpos, text, flag, &lengthFound);
                    pos != wxSCI_INVALID_POSITION ;
                    pos = stc->FindText(pos+=text.Len(), endpos, text, flag, &lengthFound) )
                {
                    stc->IndicatorFillRange(pos, lengthFound);
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
        stc->SetIndicatorCurrent(GetIndicator());
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
    const int theIndicator = 10;

    std::pair<long, long> curr;
    control->GetSelection(&curr.first, &curr.second);

    control->SetIndicatorCurrent(theIndicator);

    if (m_OldHighlightSelectionCtrl == control && m_HighlightSelectedOccurencesLastPos == curr) // whatever the current state is, we've already done it once
        return;

    m_HighlightSelectedOccurencesLastPos = curr;
    m_OldHighlightSelectionCtrl = control;

    int eof = control->GetLength();

    // Set Styling:
    // clear all style indications set in a previous run (is also done once after text gets unselected)
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
        wxColour highlightColour(Manager::Get()->GetColourManager()->GetColour(wxT("editor_highlight_occurrence")));

        if ( ctrl->GetLeftSplitViewControl() )
        {
            ctrl->GetLeftSplitViewControl()->IndicatorSetStyle(theIndicator, wxSCI_INDIC_HIGHLIGHT);
            ctrl->GetLeftSplitViewControl()->IndicatorSetForeground(theIndicator, highlightColour );
#ifndef wxHAVE_RAW_BITMAP
            // If wxWidgets is build without rawbitmap-support, the indicators become opaque
            // and hide the text, so we show them under the text.
            // Not enabled as default, because the readability is a little bit worse.
            ctrl->GetLeftSplitViewControl()->IndicatorSetUnder(theIndicator,true);
#endif
        }
        if ( ctrl->GetRightSplitViewControl() )
        {
            ctrl->GetRightSplitViewControl()->IndicatorSetStyle(theIndicator, wxSCI_INDIC_HIGHLIGHT);
            ctrl->GetRightSplitViewControl()->IndicatorSetForeground(theIndicator, highlightColour );
#ifndef wxHAVE_RAW_BITMAP
            ctrl->GetRightSplitViewControl()->IndicatorSetUnder(theIndicator,true);
#endif
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
        int lengthFound = 0; // we need this to work properly with multibyte characters
        for ( int pos = control->FindText(0, eof, selectedText, flag, &lengthFound);
            pos != wxSCI_INVALID_POSITION ;
            pos = control->FindText(pos+=selectedText.Len(), eof, selectedText, flag, &lengthFound) )
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
                if (pos + lengthFound >= currSelection->first)
                    skip = true;
                break;
            }
            if (skip)
                continue;

            // does not make sense anymore: check that the found occurrence is not the same as the selected,
            // since it is not selected in the second view -> so highlight it
            control->IndicatorFillRange(pos, lengthFound);
        }
    }
}
