#include "CppSmartIndent.h"

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <editorcolourset.h>
    #include <manager.h>
#endif

#include <cbstyledtextctrl.h>
#include <wx/regex.h>


// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<CppSmartIndent> reg(_T("CppSmartIndent"));
}


bool CppSmartIndent::InComment(const wxString& LanguageName, int style ) const
{
    if ( LanguageName == _T("C/C++") )
        return style == wxSCI_C_COMMENT ||
               style == wxSCI_C_COMMENTDOC ||
               style == wxSCI_C_COMMENTDOCKEYWORD ||
               style == wxSCI_C_COMMENTDOCKEYWORDERROR ||
               style == wxSCI_C_COMMENTLINE;
    else //if ( LanguageName == _T("D") )
        return style == wxSCI_D_COMMENT ||
               style == wxSCI_D_COMMENTDOC ||
               style == wxSCI_D_COMMENTDOCKEYWORD ||
               style == wxSCI_D_COMMENTDOCKEYWORDERROR ||
               style == wxSCI_D_COMMENTLINE ||
               style == wxSCI_D_COMMENTLINEDOC ||
               style == wxSCI_D_COMMENTNESTED;
}

void CppSmartIndent::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
{

    // check if smart indent is enabled
    // check the event type and the currently set language
    // if it is not a CharAdded event or the language is not python return

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
    if ( langname != _T("C/C++") && langname != _T("D") ) return;

    ed->AutoIndentDone(); // we are responsible.

    const int pos = stc->GetCurrentPos();
    int currLine = stc->LineFromPosition(pos);

    if ( currLine == 0)
        return;

    const wxChar ch = event.GetKey();

    if ( SelectionBraceCompletionEnabled() || stc->IsBraceShortcutActive() )
        DoSelectionBraceCompletion(stc, ch);

    DoSmartIndent(ed, ch);

    if ( BraceCompletionEnabled() )
        DoBraceCompletion(stc, ch);
}
void CppSmartIndent::DoSmartIndent(cbEditor* ed, const wxChar &ch)const
{
    static bool autoIndentStart = false;
    static bool autoIndentDone = true;
    static int autoIndentLine = -1;
    static int autoIndentLineIndent = -1;

    static bool autoUnIndent = false; // for public: / case: / etc..
    static int autoUnIndentValue = -1;
    static int autoUnIndentLine = -1;

    cbStyledTextCtrl* stc = ed->GetControl();

    const int pos = stc->GetCurrentPos();
    // indent
    if ( (ch == _T('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == _T('\r')) ) )
    {
        stc->BeginUndoAction();
        // new-line: adjust indentation
        bool autoIndent = AutoIndentEnabled();
        bool smartIndent = SmartIndentEnabled();
        int currLine = stc->LineFromPosition(pos);
        if (autoIndent && currLine > 0)
        {
            wxString indent = ed->GetLineIndentString(currLine - 1);
            if (smartIndent)
            {
                wxChar b = GetLastNonWhitespaceChar(ed);
                // if the last entered char before newline was an opening curly brace,
                // increase indentation level (the closing brace is handled in another block)

                if (!BraceIndent(stc, indent))
                {
                    if (b == _T('{'))
                    {
                        int nonblankpos;
                        wxChar c = GetNextNonWhitespaceCharOfLine(stc, pos, &nonblankpos);

                        if ( c != _T('}') )
                            Indent(stc, indent);
                        else
                        {
                            if ( pos != nonblankpos )
                            {
                                stc->SetCurrentPos(nonblankpos);
                                stc->DeleteBack();
                            }
                        }
                    }
                    else if (b == _T(':'))
                        Indent(stc, indent);
                }

            }
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
        }

        // smart indent
        if (smartIndent && currLine > 0)
        {
            if (!autoIndentDone)
            {
                bool valid = true;
                int line = stc->GetCurrentLine();
                if (line < autoIndentLine)
                    valid = false;
                else
                {
                    while (--line > autoIndentLine)
                    {
                        if (stc->GetLineIndentation(line) < autoIndentLineIndent)
                        {
                            valid = false;
                            break;
                        }
                    }
                }

                if (!valid)
                {
                    autoIndentStart = false;
                    autoIndentDone = true;
                    autoIndentLine = -1;
                    autoIndentLineIndent = -1;
                }
            }

            if (autoIndentDone)
            {
                const int pos = stc->GetLineIndentPosition(currLine - 1);
                const wxString text = stc->GetTextRange(pos, stc->WordEndPosition(pos, true));
                if (   text == _T("if")
                    || text == _T("else")
                    || text == _T("for")
                    || text == _T("while")
                    || text == _T("do") )
                {
                    const wxChar ch = GetLastNonWhitespaceChar(ed);
                    if (ch != _T(';') && ch != _T('}'))
                    {
                        autoIndentDone = false;
                        autoIndentLine = currLine - 1;
                        autoIndentLineIndent = stc->GetLineIndentation(currLine - 1);
                    }
                }
            }

            if (!autoIndentDone)
            {
                if (autoIndentStart)
                {
                    const wxChar ch = GetLastNonWhitespaceChar(ed);
                    if (ch == _T(';') || ch == _T('}'))
                    {
                        stc->SetLineIndentation(currLine, autoIndentLineIndent);
                        stc->GotoPos(stc->GetLineEndPosition(currLine));

                        autoIndentStart = false;
                        autoIndentDone = true;
                        autoIndentLine = -1;
                        autoIndentLineIndent = -1;
                    }
                }
                else
                {
                    int lastLine = currLine;
                    while (--lastLine >= 0)
                    {
                        const int lineIndentPos = stc->GetLineIndentPosition(lastLine);
                        const int start = stc->WordStartPosition(lineIndentPos, true);
                        const int end = stc->WordEndPosition(lineIndentPos, true);
                        const wxString last = stc->GetTextRange(start, end);

                        if (   last == _T("if")
                            || last == _T("else")
                            || last == _T("for")
                            || last == _T("while")
                            || last == _T("do") )
                        {
                            const wxString text = stc->GetTextRange(lineIndentPos + last.Len(), pos);
                            int level = 0;
                            for (size_t i = 0; i < text.Len(); ++i)
                            {
                                if (text[i] == _T('('))
                                {
                                    const int style = stc->GetStyleAt(pos - text.Len() + i);
                                    if (   stc->IsString(style)
                                        || stc->IsCharacter(style)
                                        || stc->IsComment(style) )
                                    {
                                        continue;
                                    }
                                    ++level;
                                }
                                else if (text[i] == _T(')'))
                                {
                                    const int style = stc->GetStyleAt(pos - text.Len() + i);
                                    if (   stc->IsString(style)
                                        || stc->IsCharacter(style)
                                        || stc->IsComment(style) )
                                    {
                                        continue;
                                    }
                                    --level;
                                }
                            }

                            if (!level)
                            {
                                autoIndentStart = true;

                                int nonblankpos;
                                wxChar c = GetNextNonWhitespaceCharOfLine(stc, pos, &nonblankpos);
                                if (c == _T('}') && currLine == stc->LineFromPosition(nonblankpos))
                                {
                                    stc->NewLine();
                                    stc->GotoPos(pos);

                                    autoIndentStart = false;
                                    autoIndentDone = true;
                                    autoIndentLine = -1;
                                    autoIndentLineIndent = -1;
                                }

                                stc->Tab();
                            }

                            break;
                        }
                    }
                }
            }

            // smart un-indent
            if (autoUnIndent)
            {
                if ( GetLastNonWhitespaceChar(ed) == _T(':'))
                {
                    stc->SetLineIndentation(autoUnIndentLine, autoUnIndentValue);
                    stc->SetLineIndentation(currLine, autoUnIndentValue);
                    stc->Tab();
                }

                autoUnIndent = false;
                autoUnIndentValue = -1;
                autoUnIndentLine = -1;
            }
        }

        stc->EndUndoAction();
    }

    // unindent
    else if (ch == _T('{'))
    {
        if (autoIndentStart)
        {
            bool valid = true;
            const int start = stc->PositionFromLine(autoIndentLine);
            const wxString text = stc->GetTextRange(start, pos);
            if (text.Find(_T('{')) != int(text.Len() - 1))
                valid = false;
            else
            {
                int line = stc->GetCurrentLine();
                if (line < autoIndentLine)
                    valid = false;
                else
                {
                    while (--line > autoIndentLine)
                    {
                        if (stc->GetLineIndentation(line) < autoIndentLineIndent)
                        {
                            valid = false;
                            break;
                        }
                    }
                }
            }

            if (valid)
            {
                stc->BeginUndoAction();
                stc->SetLineIndentation(stc->GetCurrentLine(), autoIndentLineIndent);
                stc->EndUndoAction();
            }

            autoIndentStart = false;
            autoIndentDone = true;
            autoIndentLine = -1;
            autoIndentLineIndent = -1;
        }
    }

    // unindent
    else if (ch == _T('}'))
    {
        bool smartIndent = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/smart_indent"), true);
        if ( smartIndent && ( (stc->GetLexer() == wxSCI_LEX_CPP) || (stc->GetLexer() == wxSCI_LEX_D) ) )
        {
            stc->BeginUndoAction();
            // undo block indentation, if needed
            wxString str = stc->GetLine(stc->GetCurrentLine());
            str.Trim(false);
            str.Trim(true);
            if (str.Matches(_T("}")))
            {
                // just the brace here; unindent
                // find opening brace (skipping nested blocks)
                int pos = stc->GetCurrentPos() - 2;
                pos = FindBlockStart(stc, pos, _T('{'), _T('}'));
                if (pos != -1)
                {
                    wxString indent = ed->GetLineIndentString(stc->LineFromPosition(pos));
                    indent << _T('}');
                    stc->DelLineLeft();
                    stc->DelLineRight();
                    pos = stc->GetCurrentPos();
                    stc->InsertText(pos, indent);
                    stc->GotoPos(pos + indent.Length());
                    stc->ChooseCaretX();
                }
            }
            stc->EndUndoAction();
        }
    }

    // unindent
    else if (ch == _T(':'))
    {
        bool smartIndent = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/smart_indent"), true);
        if (smartIndent && stc->GetLexer() == wxSCI_LEX_CPP && !autoUnIndent)
        {
            const int curLine = stc->GetCurrentLine();
            const int pos = stc->GetLineIndentPosition(curLine);
            const wxString text = stc->GetTextRange(pos, stc->WordEndPosition(pos, true));
            if (   text == _T("public")
                || text == _T("protected")
                || text == _T("private")
                || text == _T("case")
                || text == _T("default") )
            {
                const bool isSwitch = (text == _T("case") || text == _T("default"));
                int lastLine = curLine;
                int lastLineIndent = -1;
                while (--lastLine >= 0)
                {
                    const int lineIndentPos = stc->GetLineIndentPosition(lastLine);
                    const int start = stc->WordStartPosition(lineIndentPos, true);
                    const int end = stc->WordEndPosition(lineIndentPos, true);

                    const wxString last = stc->GetTextRange(start, end);
                    if (last.IsEmpty())
                        continue;

                    if (isSwitch)
                    {
                        if (last == _T("case"))
                        {
                            lastLineIndent = stc->GetLineIndentation(lastLine);
                            break;
                        }
                        else if (last == _T("switch"))
                            break;
                    }
                    else
                    {
                        if (   last == _T("public")
                            || last == _T("protected")
                            || last == _T("private") )
                        {
                            lastLineIndent = stc->GetLineIndentation(lastLine);
                            break;
                        }
                        else if (last == _T("class"))
                            break;
                    }
                }

                if (lastLineIndent != -1)
                {
                    autoUnIndent = true;
                    autoUnIndentValue = lastLineIndent;
                    autoUnIndentLine = curLine;
                }
                else
                {
                    const int curLineIndent = stc->GetLineIndentation(curLine);
                    const int tabWidth = stc->GetTabWidth();
                    if (curLineIndent >= tabWidth)
                    {
                        autoUnIndent = true;
                        autoUnIndentValue = curLineIndent - tabWidth;
                        autoUnIndentLine = curLine;
                    }
                }
            }
        }
    }

}

bool CppSmartIndent::BraceIndent(cbStyledTextCtrl *stc, wxString &indent)const
{
    if ( BraceSmartIndentEnabled() )
    {
        int style = 0;
        if (stc->GetLexer() == wxSCI_LEX_CPP)
            style = wxSCI_C_STRING;
        else // wxSCI_LEX_D
            style = wxSCI_D_STRING;

        int brace_position = GetFirstBraceInLine(stc, style);
        return Indent(stc, indent, brace_position);
    }
    return false;
}
void CppSmartIndent::DoSelectionBraceCompletion(cbStyledTextCtrl* control, const wxChar &ch)const
{
    if (!control->GetLastSelectedText().IsEmpty())
    {

        const int pos = control->GetCurrentPos();
        wxString selectedText = control->GetLastSelectedText();
        switch (ch)
        {
            case _T('\''):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                selectedText.Replace(wxT("\\'"), wxT("'"));
                selectedText.Replace(wxT("'"), wxT("\\'"));
                control->AddText(wxT("'") + selectedText + wxT("'"));
                control->EndUndoAction();
                return;
            }
            case _T('"'):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                selectedText.Replace(wxT("\\\""), wxT("\""));
                selectedText.Replace(wxT("\""), wxT("\\\""));
                control->AddText(wxT("\"") + selectedText + wxT("\""));
                control->SetSelectionVoid(pos - 1, pos + selectedText.Length() + 1);
                int startLine = control->LineFromPosition(control->GetSelectionStart());
                int endLine = control->LineFromPosition(control->GetSelectionEnd());
                if (startLine != endLine)
                {
                    int selectionEnd = pos + selectedText.Length() + 1;
                    for (int i = endLine; i > startLine; i--)
                    {
                        control->Home();
                        for (int j = control->GetCurrentPos(); control->GetCharAt(j) == _T(' ') || control->GetCharAt(j) == _T('\t'); j++)
                            control->CharRight();
                        control->AddText(wxT("\""));
                        control->SetEmptySelection(control->GetLineEndPosition(i - 1));
                        control->AddText(wxT("\""));
                        selectionEnd += control->GetIndent() + 2;
                    }
                    control->SetSelectionVoid(pos - 1, selectionEnd);
                }
                control->EndUndoAction();
                return;
            }
            case _T('('):
            case _T(')'):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                control->InsertText(pos - 1, wxT("(") + selectedText + wxT(")"));
                if (ch == _T(')'))
                    control->SetEmptySelection(pos + selectedText.Length() + 1);
                control->EndUndoAction();
                return;
            }
            case _T('['):
            case _T(']'):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                control->InsertText(pos - 1, wxT("[") + selectedText + wxT("]"));
                if (ch == _T(']'))
                    control->SetEmptySelection(pos + selectedText.Length() + 1);
                control->EndUndoAction();
                return;
            }
            case _T('<'):
            case _T('>'):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                control->InsertText(pos - 1, wxT("<") + selectedText + wxT(">"));
                if (ch == _T('>'))
                    control->SetEmptySelection(pos + selectedText.Length() + 1);
                control->EndUndoAction();
                return;
            }
            case _T('{'):
            case _T('}'):
            {
                control->BeginUndoAction();
                control->DeleteBack();
                control->AddText(selectedText);
                control->SetSelectionVoid(pos - 1, pos + selectedText.Length() - 1);
                int startLine = control->LineFromPosition(control->GetSelectionStart());
                int endLine = control->LineFromPosition(control->GetSelectionEnd());
                if (startLine == endLine)
                    control->Home();
                control->Tab();
                control->SetEmptySelection(control->GetLineEndPosition(endLine));
                control->NewLine();
                control->BackTab();
                control->AddText(wxT("}"));
                control->SetEmptySelection(control->GetLineEndPosition(startLine - 1));
                control->NewLine();
                control->InsertText(control->GetCurrentPos(), wxT("{"));
                if (ch == _T('}'))
                    control->SetEmptySelection(control->GetLineEndPosition(endLine + 2));
                control->EndUndoAction();
                return;
            }
        }
    } // SelectionBraceCompletion
}

void CppSmartIndent::DoBraceCompletion(cbStyledTextCtrl* control, const wxChar& ch)const
{
    int pos = control->GetCurrentPos();
    int style = control->GetStyleAt(pos);
    if ( control->IsComment(style) || control->IsPreprocessor(style) )
        return;
    if (ch == _T('\'') || ch == _T('"'))
    {
        if (   (control->GetCharAt(pos) == ch)
            && (control->GetCharAt(pos - 2) != _T('\\')) )
        {
            control->DeleteBack();
            control->GotoPos(pos);
        }
        else
        {
            const wxChar left = control->GetCharAt(pos - 2);
            const wxChar right = control->GetCharAt(pos);
            if (   control->IsCharacter(style)
                || control->IsString(style)
                || left == _T('\\')
                || (   (left > _T(' '))
                    && (left != _T('('))
                    && (left != _T('=')) )
                || (   (right > _T(' '))
                    && (right != _T(')')) ) )
            {
                return;
            }
            control->AddText(ch);
            control->GotoPos(pos);
        }
        return;
    }
    if ( control->IsCharacter(style) || control->IsString(style) )
        return;
    const wxString leftBrace(_T("([{"));
    const wxString rightBrace(_T(")]}"));
    int index = leftBrace.Find(ch);
    const wxString unWant(_T(");\n\r\t\b "));
    const wxChar nextChar = control->GetCharAt(pos);
    #if wxCHECK_VERSION(2, 9, 0)
    if ((index != wxNOT_FOUND) && ((unWant.Find(wxUniChar(nextChar)) != wxNOT_FOUND) || (pos == control->GetLength())))
    #else
    if ((index != wxNOT_FOUND) && ((unWant.Find(nextChar) != wxNOT_FOUND) || (pos == control->GetLength())))
    #endif
    {
        control->AddText(rightBrace.GetChar(index));
        control->GotoPos(pos);
        if (ch == _T('{'))
        {
            const int curLine = control->GetCurrentLine();
            int keyLine = curLine;
            wxString text;
            do
            {
                int keyPos = control->GetLineIndentPosition(keyLine);
                int start = control->WordStartPosition(keyPos, true);
                int end = control->WordEndPosition(keyPos, true);
                text = control->GetTextRange(start, end);
            }
            while (   (text.IsEmpty() || text == _T("public") || text == _T("protected") || text == _T("private"))
                   && (text != _T("namespace"))
                   && (--keyLine >= 0) );

            if (text == _T("class") || text == _T("struct") || text == _T("enum") || text == _T("union"))
                control->InsertText(control->GetLineEndPosition(curLine), _T(";"));

            const wxRegEx reg(_T("^[ \t]*{}[ \t]*"));
            if (reg.Matches(control->GetCurLine()))
            {
                control->NewLine();
                control->GotoPos(pos);
                control->NewLine();
                return;
            }
        }
    }
    else
    {
        index = rightBrace.Find(ch);
        if (index != wxNOT_FOUND)
        {
            if (control->GetCharAt(pos) == ch)
            {
                control->DeleteBack();
                control->GotoPos(pos);
                return;
            }
        }
    }
}


