#include "SmartIndentFortran.h"

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <editorcolourset.h>
    #include <manager.h>
#endif

#include <cbstyledtextctrl.h>

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<SmartIndentFortran> reg(wxT("SmartIndentFortran"));
}

void SmartIndentFortran::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
{
    // check if smart indent is enabled
    // check the event type and the currently set language
    // if it is not a CharAdded event or the language is not Fortran return

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

    wxString langname = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    if ( langname != wxT("Fortran") && langname != wxT("Fortran77") ) return;

    ed->AutoIndentDone(); // we are responsible.

    const int pos = stc->GetCurrentPos();
    wxChar ch = event.GetKey();

    if ( (ch == _T('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == _T('\r')) ) )
    {
        const bool autoIndent = AutoIndentEnabled();

        stc->BeginUndoAction();

        int currLine = stc->LineFromPosition(pos);

        // auto indent
        if (autoIndent && currLine > 0)
        {
            wxString indent = ed->GetLineIndentString(currLine - 1);
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
        }

        // smart indent
        bool smartIndent = SmartIndentEnabled();
        if (smartIndent && currLine > 0)
        {
            int start = stc->GetLineIndentPosition(currLine - 1);
            int endt  = stc->WordEndPosition(start, true);
            wxString text = stc->GetTextRange(start, endt).Lower();
            wxString lineText = stc->GetLine(currLine - 1).BeforeFirst('!').Trim().Lower();
            wxString lastText = lineText.AfterLast(')').Trim().Trim(false);
            wxString secText = lineText.Trim(false).Mid(text.Length()).Trim(false);
            if (  (text == _T("if") && lastText == _T("then"))
                || text == _T("else")
                || text == _T("elseif")
                || text == _T("enum")
                ||(text == _T("where") && lastText.IsEmpty())
                || text == _T("elsewhere")
                || text == _T("block")
                || text == _T("blockdata")
                ||(text == _T("forall") && lastText.IsEmpty())
                || text == _T("while")
                || text == _T("case")
                || text == _T("associate")
                || text == _T("block")
                || text == _T("critical")
                || text == _T("do")
                ||(text == _T("type") && !secText.StartsWith(_T("(")))
                || text == _T("program")
                || text == _T("function")
                || text == _T("subroutine")
                || text == _T("interface")
                ||(    text == _T("module")
                   && !secText.StartsWith(_T("procedure "))
                   && !secText.StartsWith(_T("procedure:")) )
                ||(    text == _T("change")
                   && (secText.StartsWith(_T("team ")) || secText.StartsWith(_T("team("))) )
               )
            {
                stc->Tab();
            }
        }

        stc->EndUndoAction();
    }

    bool braceCompleted = false;
    if ( SelectionBraceCompletionEnabled() || stc->IsBraceShortcutActive() )
        braceCompleted = stc->DoSelectionBraceCompletion(ch);
    if (!braceCompleted && BraceCompletionEnabled())
        DoBraceCompletion(stc, ch);
}

void SmartIndentFortran::DoBraceCompletion(cbStyledTextCtrl* control, const wxChar& ch) const
{
    if (!control)
        return;

    // Variable "ch" is a character which was just typed. Current position is after ch.
    int pos = control->GetCurrentPos();
    int style = control->GetStyleAt(pos);

    // Don't do anything if we are in a comment or in a preprocessor line.
    if (control->IsComment(style) || control->IsPreprocessor(style))
        return;
    if (ch == _T('\'') || ch == _T('"'))
    {
        // Take care about ' and ".
        if ((control->GetCharAt(pos) == ch) && (control->GetCharAt(pos - 2) != _T('\\')))
        {
            control->DeleteBack();
            control->GotoPos(pos);
        }
        else
        {
            const wxChar left = control->GetCharAt(pos - 2);
            const wxChar right = control->GetCharAt(pos);
            if (control->IsCharacter(style) || control->IsString(style) || left == _T('\\'))
                return;
            else if ((left > _T(' '))  && (left != _T('(')) && (left != _T('=')))
                return;
            else if ((right > _T(' ')) && (right != _T(')')) )
                return;
            control->AddText(ch);
            control->GotoPos(pos);
        }
        return;
    }
    if (control->IsCharacter(style) || control->IsString(style))
        return;
    const wxString leftBrace(_T("([{"));
    const wxString rightBrace(_T(")]}"));
    int index = leftBrace.Find(ch);
    const wxString unWant(_T(");\n\r\t\b "));
    const wxChar nextChar = control->GetCharAt(pos);
#if wxCHECK_VERSION(3, 0, 0)
    if ((index != wxNOT_FOUND)
        && ((unWant.Find(wxUniChar(nextChar)) != wxNOT_FOUND) || (pos == control->GetLength())))
#else
    if ((index != wxNOT_FOUND)
        && ((unWant.Find(nextChar) != wxNOT_FOUND) || (pos == control->GetLength())))
#endif
    {
        // add closing brace
        control->AddText(rightBrace.GetChar(index));
        control->GotoPos(pos);
    }
    else
    {
        index = rightBrace.Find(ch);
        if (index != wxNOT_FOUND)
        {
            // closing brace was just typed
            if (control->GetCharAt(pos) == ch)
            {
                control->DeleteBack();
                control->GotoPos(pos);
                return;
            }
        }
    }
}
