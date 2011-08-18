#include "OnlineSpellChecker.h"

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <editormanager.h>
    #include <manager.h>
#endif
#include <editorcolourset.h>
#include <cbstyledtextctrl.h>
#include "SpellCheckEngineInterface.h"
#include "SpellCheckHelper.h"
//#include <hunspell.hxx>




OnlineSpellChecker::OnlineSpellChecker(wxSpellCheckEngineInterface *pSpellChecker, SpellCheckHelper *pSpellHelper):
//OnlineSpellChecker::OnlineSpellChecker():
    //EditorHooks::HookFunctorBase
    HookFunctorBase(),
    alreadychecked(false),
    oldctrl(NULL),
    m_pSpellChecker(pSpellChecker),
    m_pSpellHelper(pSpellHelper),
    m_doChecks(false)
{
}
OnlineSpellChecker::~OnlineSpellChecker()
{
    ClearAllIndications();
}


void OnlineSpellChecker::Call(cbEditor* ctrl, wxScintillaEvent &event) const
{
    // return if this event is not fired from the active editor (is it possible that an editor which is not active fires an event?)
    if ( Manager::Get()->GetEditorManager()->GetActiveEditor() != ctrl  ) return;

    // check the event type if it is an update event
    if ( event.GetEventType() == wxEVT_SCI_UPDATEUI )
        OnEditorUpdateUI(ctrl);
    else if ( event.GetEventType() == wxEVT_SCI_MODIFIED) {
    	if(event.GetModificationType() & wxSCI_MOD_INSERTTEXT) {
			OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition() + event.GetLength());
    	} else if (event.GetModificationType() & wxSCI_MOD_DELETETEXT) {
    		OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition());
    	} else if (event.GetModificationType() & wxSCI_MOD_CHANGESTYLE) {
    		OnEditorChangeTextRange(ctrl, event.GetPosition(), event.GetPosition() + event.GetLength());
    	}
    }
}

void OnlineSpellChecker::OnEditorChange(cbEditor* ctrl) const
{
    // clear internal states to force a refresh at next UpdateUI;
    alreadychecked = false;
}

const int OnlineSpellChecker::GetIndicator()const
{
    const int theIndicator = 11;
    return theIndicator;
}
const wxColor OnlineSpellChecker::GetIndicatorColor()const
{
    //wxColour indicatorColour(cfg->ReadColour(_T("/???/colour"), wxColour(255, 0, 0)));
    return wxColour(255,0,0);
}
void OnlineSpellChecker::OnEditorUpdateUI(cbEditor* ctrl) const
{
    if ( !m_doChecks ) return;
    DoSetIndications(ctrl);
}

void OnlineSpellChecker::OnEditorChangeTextRange(cbEditor* ctrl, int start, int end)const
{
    if ( !m_doChecks ) return;
    if ( alreadychecked && oldctrl == ctrl )
    {
        //only recheck the last word to speed things up

        cbStyledTextCtrl *stc = ctrl->GetLeftSplitViewControl();

        //swap and bound check (maybe not needed, but for safety)
		if (end < start) {
            int t = start;
            start = end;
            end = t;
        }
        if (start < 0) start = 0;
        if (end < 0) end = 0;
        if (start >= stc->GetLength()) start = stc->GetLength() - 1;
        if (end > stc->GetLength()) end = stc->GetLength();

        //find recheck range start:
        if (start > 0) start--;
		for (; start > 0; ) {
            wxString lang = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ctrl->GetLanguage() );
            wxChar ch = stc->GetCharAt(start - 1);
            if ( SpellCheckHelper::IsWhiteSpace(ch) && !SpellCheckHelper::IsEscapeSequenceStart(ch, lang, stc->GetStyleAt(start-1) ) )
                break;
            start--;
        }
        //find recheck range end:
		for (; end < stc->GetLength() ; ) {
            wxChar ch = stc->GetCharAt(end);
            if ( SpellCheckHelper::IsWhiteSpace(ch) )
                break;
            end++;
        }

        if (m_invalidatedRangesStart.GetCount() == 0 || m_invalidatedRangesStart.Last() != start || m_invalidatedRangesEnd.Last() != end) {
            m_invalidatedRangesStart.Add(start);
            m_invalidatedRangesEnd.Add(end);
        }
	} else {
		alreadychecked = false;
    }
}

void OnlineSpellChecker::DoSetIndications(cbEditor* ctrl)const
{
    //Returns a pointer to the left (or top) split-view cbStyledTextCtrl. This function always returns a valid pointer.
    cbStyledTextCtrl *stc = ctrl->GetLeftSplitViewControl();

    cbStyledTextCtrl *stcr =ctrl->GetRightSplitViewControl();

    // whatever the current state is, we've already done it once
    if ( alreadychecked && oldctrl == ctrl )
    {
        if (m_invalidatedRangesStart.GetCount() == 0)
            return;
    }
    else
    {
        //clear:
        m_invalidatedRangesStart.Clear();
        m_invalidatedRangesEnd.Clear();
        //add whole document
        m_invalidatedRangesStart.Add(0);
        m_invalidatedRangesEnd.Add(stc->GetLength());
    }
    alreadychecked = true;
    oldctrl = ctrl;

    // Set Styling:
    stc->SetIndicatorCurrent(GetIndicator());
    stc->IndicatorSetStyle(GetIndicator(), wxSCI_INDIC_SQUIGGLE);
    stc->IndicatorSetForeground(GetIndicator(), GetIndicatorColor() );
#ifndef wxHAVE_RAW_BITMAP
    // If wxWidgets is build without rawbitmap-support, the indicators become opaque
    // and hide the text, so we show them under the text.
    // Not enabled as default, because the readability is a little bit worse.
    stc->IndicatorSetUnder(GetIndicator(),true);
#endif
    if ( stcr )
    {
        stcr->IndicatorSetStyle(GetIndicator(), wxSCI_INDIC_SQUIGGLE);
        stcr->IndicatorSetForeground(GetIndicator(), GetIndicatorColor() );
#ifndef wxHAVE_RAW_BITMAP
        stcr->IndicatorSetUnder(GetIndicator(),true);
#endif
    }

    // Manager::Get()->GetLogManager()->Log(wxT("OSC: update regions"));

    for (int i = 0; i < (int)m_invalidatedRangesStart.GetCount(); i++)
    {
        int start = m_invalidatedRangesStart[i];
        int end = m_invalidatedRangesEnd[i];
        //bound:
        if (start < 0) start = 0;
        if (end < 0) end = 0;
        if (start >= stc->GetLength()) start = stc->GetLength() - 1;
        if (end > stc->GetLength()) end = stc->GetLength();

        if (start != end)
        {
            //remove styling:
            stc->IndicatorClearRange(start, end - start);

            int wordstart = start;
            int wordend = wordstart;
            for( int pos = wordstart ;  pos < end ; )
            {
                wxString lang = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ctrl->GetLanguage() );
                wxChar ch = stc->GetCharAt(pos);
                // treat chars which don't have the correct style as whitespace:
                bool isEscape = SpellCheckHelper::IsEscapeSequenceStart(ch, lang, stc->GetStyleAt(pos));
                if ( isEscape || SpellCheckHelper::IsWhiteSpace(ch) || !m_pSpellHelper->HasStyleToBeChecked(lang, stc->GetStyleAt(pos)))
                {
                    if (wordstart != wordend)
                        DissectWordAndCheck(stc, wordstart, wordend);
                    pos++;
                    if ( isEscape ) pos++;
                    wordstart = pos;
                    wordend = pos;
                }
                else
                {
                    pos++;
                    wordend = pos;
                }
            }
            if (wordstart != wordend)
            {
                wxString lang = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ctrl->GetLanguage() );
                if ( m_pSpellHelper->HasStyleToBeChecked(lang, stc->GetStyleAt(wordstart))  )
                {
                    DissectWordAndCheck(stc, wordstart, wordend);
                }
            }
        }
    }
    m_invalidatedRangesStart.Clear();
    m_invalidatedRangesEnd.Clear();
}

void OnlineSpellChecker::DissectWordAndCheck(cbStyledTextCtrl *stc, int wordstart, int wordend)const
{
    wxString word = stc->GetTextRange(wordstart, wordend);
    //Manager::Get()->GetLogManager()->Log(wxT("dissecting: \"") + word + wxT("\""));
    //and now decide whether the word is an abbreviation and split words when case changes to uppercase
    bool upper = wxIsupper(word[0]) != 0;
    int a, b, c;
    a = 0; b = 0; c = 0;
    for (; c < word.length();) {
        wxChar cc = word[c];
        if (upper == (wxIsupper(cc) != 0)) {
            //same case
            c++;
            b = c;
        } else {
            //case changed...
            if (upper) {
                if (b - a == 1) {
                    //start of word
                    c++;
                    b = c;
                } else {
                    //abbreviation, ignore
                    a = c;
                    c++;
                    b = c;
                }
                upper = false;
            } else {
                //check the word:
                //Manager::Get()->GetLogManager()->Log(wxT("checking: \"") + word.Mid(a, b - a) + wxT("\""));
                if ( !m_pSpellChecker->IsWordInDictionary(word.Mid(a, b - a)) )
                    stc->IndicatorFillRange(wordstart + a, b - a);
                //next:
                a = c;
                c++;
                b = c;
                upper = true;
            }
        }
    }
    //check the remaining letters
    if (upper == false || b - a == 1) {
        //check the word:
        //Manager::Get()->GetLogManager()->Log(wxT("checking: \"") + word.Mid(a, b - a) + wxT("\""));
        if ( !m_pSpellChecker->IsWordInDictionary(word.Mid(a, b - a)) )
            stc->IndicatorFillRange(wordstart + a, b - a);
    }
}


void OnlineSpellChecker::EnableOnlineChecks(bool check)
{
    m_doChecks = check;

    alreadychecked = false;

    EditorManager *edm = Manager::Get()->GetEditorManager();
    for ( int i = 0 ; i < edm->GetEditorsCount() ; ++i)
    {
        cbEditor *ed = edm->GetBuiltinEditor( i );

        if ( !ed ) continue;
        if ( check == false )
            // clear all indications set in a previous run
            ClearAllIndications(ed->GetControl());
        else
            OnEditorUpdateUI(ed);
    }
}

void OnlineSpellChecker::ClearAllIndications(cbStyledTextCtrl* stc)const
{
    if ( stc )
    {
        stc->SetIndicatorCurrent(GetIndicator());
        stc->IndicatorClearRange(0, stc->GetLength());
    }
}

void OnlineSpellChecker::ClearAllIndications()const
{
    EditorManager *edm = Manager::Get()->GetEditorManager();
    for ( int i = 0 ; i < edm->GetEditorsCount() ; ++i)
    {
        cbEditor *ed = edm->GetBuiltinEditor( i );
        if ( ed )
            ClearAllIndications(ed->GetControl());
    }
}
