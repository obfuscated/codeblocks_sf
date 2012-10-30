#include "HDLSmartIndent.h"

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
    PluginRegistrant<HDLSmartIndent> reg(wxT("HDLSmartIndent"));
}

void HDLSmartIndent::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
{
    // check if smart indent is enabled
    // check the event type and the currently set language
    // if it is not a CharAdded event or the language is neither VHDL nor Verilog return

    if (!ed)
        return;

    if ( !SmartIndentEnabled() )
        return;

    wxEventType type = event.GetEventType();
    if ( type != wxEVT_SCI_CHARADDED )
        return;

    cbStyledTextCtrl *stc = ed->GetControl();
    if (!stc)
        return;

    wxString langname = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    if ( (langname != wxT("VHDL") ) && (langname != wxT("Verilog") ) )
        return;

    ed->AutoIndentDone(); // we are responsible.

    wxChar ch = event.GetKey();
    if ( (ch == wxT('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == wxT('\r')) ) )
        DoIndent(ed, langname);   // indent because \n added
    else if ( ch != wxT(' ') )
        DoUnIndent(ed, langname); // un-indent because not a newline added

    if ( SelectionBraceCompletionEnabled() || stc->IsBraceShortcutActive() )
        ed->DoSelectionBraceCompletion(stc, ch);
}

int HDLSmartIndent::FindBlockStartVHDL(cbEditor* ed, int position, wxString block) const
{
    cbStyledTextCtrl* stc = ed->GetControl();
    if (!stc) return -1;

    int lvl = 0;
    int pos = position;

    do
    {
        pos = stc->FindText(pos, 0, block, wxSCI_FIND_WHOLEWORD);
        if ( pos != -1 )
        {
            if (  GetLastNonCommentWord(ed, pos, 1).Lower().IsSameAs( wxT("end") ) )
                ++lvl;
            else
            {
                if ( lvl == 0 )
                    return pos;
                --lvl;
                continue;
            }
        }
    }
    while( pos != -1 );

    return -1;
}

void HDLSmartIndent::DoIndent(cbEditor* ed, const wxString& langname) const
{
    cbStyledTextCtrl* stc = ed->GetControl();

    int pos = stc->GetCurrentPos();
    const int currLine = stc->LineFromPosition(pos);
    if (currLine == 0)
        return;

    stc->BeginUndoAction();

    // always do auto indent if smart indent is enabled
    wxString indent = ed->GetLineIndentString(currLine - 1);
    stc->InsertText(pos, indent);
    pos += indent.Length();
    stc->GotoPos(pos);
    stc->ChooseCaretX();

    wxChar b = GetLastNonWhitespaceChar(ed);
    bool newlineindent = false;

    if ( langname == wxT("VHDL") )
    {
        if (b == wxT('('))// generic/port list/map of entity/component
            newlineindent = true;
        else
        {
            wxString lw = GetLastNonCommentWord(ed).Lower();
            wxString l2w = GetLastNonCommentWord(ed, -1, 2).Lower();
            //Manager::Get()->GetLogManager()->Log(wxT("l2w: ") + l2w);
            l2w =  l2w.Mid(0,3);
            bool IsEnd = l2w.IsSameAs(wxT("end"));


            wxString lc = GetLastNonWhitespaceChars(ed, -1, 2);
            if (
                lw == wxT("is")                  || // architecture/entity ... is
                                                   // case ... is
                                                   // block ... is
                lw == wxT("block")               ||
                lw == wxT("begin")               || // body of architecture
               (lw == wxT("if") && !IsEnd)       || // indent the following condition
                lw == wxT("elsif")               || // indent the following condition
                //lw == wxT("case")                 || // indent the following condition
                lw == wxT("then")                || // if/elsif ... then
                lw == wxT("else")                || // indent body
               (lw == wxT("loop") && !IsEnd)     || // indent body of loop
                lw == wxT("for")                 || // indent condition
                lw == wxT("while")               || // indent condition

                lw == wxT("with")                || // indent expression
                lw == wxT("select")              || // indent target <= waveform
               (lw == wxT("generate") && !IsEnd) ||
               (lw == wxT("process") && !IsEnd)  ||

                lw == wxT("block")               ||
                lw == wxT("entity")              ||
                lw == wxT("architecture")        ||
                lw == wxT("component")           ||
                lw == wxT("package")             ||
                lw == wxT("configuration")       ||
                lw == wxT("procedure")           ||
                lw == wxT("function")            ||
                lw == wxT("record")              ||
                lc == wxT("=>")
            ) // if
            {
                newlineindent = true;
            }
        }
    }

    if ( langname == wxT("Verilog") )
    {
        wxString lw = GetLastNonCommentWord(ed);
        if ( lw.IsSameAs( wxT("begin") ) )
            newlineindent = true;
    }

    if ( newlineindent )
    {
        wxString indent;
        Indent(stc, indent);
        stc->InsertText(pos, indent);
        stc->GotoPos(pos + indent.Length());
        stc->ChooseCaretX();
    }
    stc->EndUndoAction();
}

void HDLSmartIndent::DoUnIndent(cbEditor* ed, const wxString& langname) const
{
    cbStyledTextCtrl* stc = ed->GetControl();

    // not a whitespace char added
    if ( langname == wxT("VHDL") )
    {
        wxString strWithSpaces = stc->GetLine(stc->GetCurrentLine());
        strWithSpaces.Trim(false).Trim(true);
        wxString str(strWithSpaces.Lower());
        str.Replace(wxT("\t"), wxT(" "));
        // to be sure that there are only single spaces:
        size_t r;
        do
            r = str.Replace(wxT("  "), wxT(" "), true);
        while (r);

        int pos = stc->GetCurrentPos() - 2;
        if (
            str.IsSameAs(wxT("then"))   ||
            str.IsSameAs(wxT("elsif"))  ||
            str.IsSameAs(wxT("else"))   ||
            str.IsSameAs(wxT("end if"))
        )
            pos = FindBlockStartVHDL(ed, pos-1, wxT("if") );
        else if (
            str.IsSameAs(wxT("when"))     ||
            str.IsSameAs(wxT("end case"))
        )
            pos = FindBlockStartVHDL(ed, pos-3, wxT("case") );
        else if ( str.IsSameAs(wxT("end process")) )
            pos = FindBlockStartVHDL(ed, pos-6, wxT("process") );
        else if ( str.IsSameAs(wxT("end record") ) )
            pos = FindBlockStartVHDL(ed, pos-5, wxT("record") );
        else if ( str.IsSameAs( wxT("end for") ) )
            // is it possible to have a "wait for ..." or
            // "for ... generate" or "for ... loop"
            // in a block configuration? i believe not
            // so this should be enough:
            pos = FindBlockStartVHDL(ed, pos-2, wxT("for") );
        else if ( str.IsSameAs( wxT("end component") ) )
            pos = FindBlockStartVHDL(ed, pos-8, wxT("component") );
        else if ( str.IsSameAs( wxT(")") ) )
            pos = FindBlockStart(stc, pos, wxT('('), wxT(')'));
        else if ( str.IsSameAs(wxT("end loop")) )
            // assuming that "for/while" and "loop" are on the same line:
            pos = FindBlockStartVHDL(ed, pos-3, wxT("loop"));
        else if ( str.IsSameAs( wxT("end generate") ) )
            // assuming that "if/for ..." and "generate" are on the same line:
            pos = FindBlockStartVHDL(ed, pos-7, wxT("generate") );
        else if ( str.IsSameAs( wxT("begin") ) )
        {
            pos = -1;
        }
        else
            pos = -1;

        if (pos != -1)
        {
            wxString indent = ed->GetLineIndentString(stc->LineFromPosition(pos));
            indent += strWithSpaces;
            stc->BeginUndoAction();
            stc->DelLineLeft();
            stc->DelLineRight();
            pos = stc->GetCurrentPos();
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
            stc->EndUndoAction();
        }
    }
    if ( langname == wxT("Verilog") )
    {
        wxString str = stc->GetLine(stc->GetCurrentLine());
        str.Trim(false).Trim(true);

        int pos = stc->GetCurrentPos() - 4;
        if ( str.Matches( wxT("end")) )
            pos = FindBlockStart(stc, pos, wxT("begin"), wxT("end") );
        else
            pos = -1;
        if (pos != -1)
        {
            wxString indent = ed->GetLineIndentString(stc->LineFromPosition(pos));
            indent += str;
            stc->BeginUndoAction();
            stc->DelLineLeft();
            stc->DelLineRight();
            pos = stc->GetCurrentPos();
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
            stc->EndUndoAction();
        }
    }
}
