#include "SmartIndentXML.h"

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <editorcolourset.h>
    #include <manager.h>
#endif

#include <wx/regex.h>

#include <cbstyledtextctrl.h>

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<SmartIndentXML> reg(wxT("SmartIndentXML"));
}

void SmartIndentXML::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
{
    // check if smart indent is enabled
    // check the event type and the currently set language
    // if it is not a CharAdded event or the language is neither XML nor HTML return

    if (!ed)
        return;

    if ( !SmartIndentEnabled() )
        return;

    wxEventType type = event.GetEventType();
    if ( type != wxEVT_SCI_CHARADDED )
        return;

    cbStyledTextCtrl* stc = ed->GetControl();
    if (!stc)
        return;

    const int lexer = stc->GetLexer();
    if (lexer != wxSCI_LEX_XML && lexer != wxSCI_LEX_HTML)
        return;

    ed->AutoIndentDone(); // we are responsible

    int pos = stc->GetCurrentPos();
    int currLine = stc->LineFromPosition(pos);

    const wxChar ch = event.GetKey();
    wxRegEx reTag(wxT("***:<[ \t]*?(|/)[ \t]*?([a-zA-Z][a-zA-Z0-9_-]*).*?(|/)[ \t]*?>"));

    bool complQuote = true;
    if ( SelectionBraceCompletionEnabled() || stc->IsBraceShortcutActive() )
    {
        if (stc->DoSelectionBraceCompletion(ch))
        {
            complQuote = false;
            pos = stc->GetCurrentPos();
            currLine = stc->LineFromPosition(pos);
        }
    }

    if (BraceCompletionEnabled())
    {
        int curSty = stc->GetStyleAt(pos);
        // finish XML/HTML tag
        if ( ch == wxT('>') && !stc->IsString(curSty) )
        {
            wxString tag;
            for (int i = pos - 2; i > 0; --i)
            {
                if (stc->GetCharAt(i) == wxT('<') || stc->GetCharAt(i) == wxT('>'))
                {
                    tag = stc->GetTextRange(i, pos);
                    break;
                }
            }
            // is open tag
            if (reTag.Matches(tag) && reTag.GetMatch(tag, 1).IsEmpty() && reTag.GetMatch(tag, 3).IsEmpty())
                stc->InsertText(pos, wxT("</") + reTag.GetMatch(tag, 2) + wxT(">"));
        }
        // close string
        else if (complQuote && (ch == wxT('"') || ch == wxT('\'')))
        {
            if (stc->GetCharAt(pos) == ch)
            {
                stc->DeleteBack();
                stc->CharRight();
            }
            else if (!stc->IsString(stc->GetStyleAt(pos - 2)))
                stc->InsertText(pos, ch);
        }
        else if (ch == wxT('?') && stc->GetCharAt(pos - 2) == wxT('<'))
        {
            wxString tag;
            for (int i = pos; i < stc->GetLength(); ++i)
            {
                if (stc->GetCharAt(i) == wxT('<') || stc->GetCharAt(i) == wxT('>'))
                {
                    tag = stc->GetTextRange(i - 1, i + 1);
                    break;
                }
            }
            if (tag != wxT("?>"))
                stc->InsertText(pos, wxT(" ?>"));
        }
        // close CDATA
        else if (ch == wxT('[') && pos > 8 && stc->GetTextRange(pos - 9, pos - 1) == wxT("<![CDATA"))
        {
            wxString tag;
            for (int i = pos; i < stc->GetLength(); ++i)
            {
                if (stc->GetCharAt(i) == wxT('<') || stc->GetCharAt(i) == wxT('>'))
                {
                    tag = stc->GetTextRange(i - 2, i + 1);
                    break;
                }
            }
            if (tag != wxT("]]>"))
                stc->InsertText(pos, wxT("]]>"));
        }
        // embedded languages
        else if (complQuote && curSty >= wxSCI_HJ_START && curSty <= wxSCI_HPHP_OPERATOR && !stc->IsString(curSty))
        {
            stc->DoBraceCompletion(ch);
        }
    }
    // indent
    if (   AutoIndentEnabled()
        && ( (ch == wxT('\n')) || ((stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == wxT('\r'))) ) )
    {
        wxString indent = ed->GetLineIndentString(currLine - 1);
        stc->BeginUndoAction();
        if (SmartIndentEnabled()) // smart indent
        {
            int idx = stc->GetLine(currLine - 1).Find(wxT('>'), true);
            if (idx != wxNOT_FOUND)
            {
                wxString tag;
                for (int i = stc->PositionFromLine(currLine - 1) + idx - 1; i > 0; --i)
                {
                    if (stc->GetCharAt(i) == wxT('<'))
                    {
                        tag = stc->GetTextRange(i, pos);
                        break;
                    }
                }
                // tag found
                if (reTag.Matches(tag))
                {
                    indent = ed->GetLineIndentString(stc->LineFromPosition(pos - tag.Length()));
                    // is open tag
                    if (reTag.GetMatch(tag, 1).IsEmpty() && reTag.GetMatch(tag, 3).IsEmpty())
                    {
                        wxString tagId = reTag.GetMatch(tag, 2);
                        // is close-tag after cursor?
                        tag = stc->GetLine(currLine);
                        if (reTag.Matches(tag) && !reTag.GetMatch(tag, 1).IsEmpty() && reTag.GetMatch(tag, 2) == tagId)
                        {
                            if (wxIsspace(stc->GetCharAt(pos)))
                                stc->DelWordRight();
                            stc->InsertText(pos, indent);
                            stc->NewLine();
                        }
                        Indent(stc, indent);
                    }
                }
            }
            else if (stc->GetStyleAt(pos) >= wxSCI_HJ_START && stc->GetStyleAt(pos) <= wxSCI_HPHP_OPERATOR)
            {
                // embedded language, indent braces
                const wxString lineSuffix = stc->GetLine(currLine).Strip(wxString::both);
                if (lineSuffix == wxT('}') || lineSuffix == wxT(']'))
                {
                    stc->InsertText(pos, GetEOLStr(stc->GetEOLMode()) + indent);
                    Indent(stc, indent);
                }
                else if (GetLastNonWhitespaceChar(ed) == wxT('{'))
                    Indent(stc, indent);
            }
            else
            {
                // align attributes
                for (int i = stc->PositionFromLine(currLine - 1); i < stc->GetLineEndPosition(currLine - 1); ++i)
                {
                    if (stc->GetStyleAt(i) == wxSCI_H_ATTRIBUTE || stc->GetStyleAt(i) == wxSCI_H_ATTRIBUTEUNKNOWN)
                    {
                        Indent(stc, indent, i - stc->PositionFromLine(currLine - 1));
                        if (stc->GetLineEndPosition(currLine) > pos + 2 && wxIsspace(stc->GetCharAt(pos)))
                            stc->DelWordRight();
                        break;
                    }
                }
            }
        }
        stc->InsertText(pos, indent);
        stc->GotoPos(pos + indent.Length());
        stc->ChooseCaretX();
        stc->EndUndoAction();
    }
}
