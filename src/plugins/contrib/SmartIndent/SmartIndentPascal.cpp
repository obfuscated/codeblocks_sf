#include "SmartIndentPascal.h"

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
    PluginRegistrant<SmartIndentPascal> reg(wxT("SmartIndentPascal"));
}

void SmartIndentPascal::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
{
    if (!ed) return;

    // check the event type and the currently set language
    // if it is not a CharAdded event or the language is not Pascal return
    wxEventType type = event.GetEventType();
    if ( type != wxEVT_SCI_CHARADDED ) return;

    cbStyledTextCtrl* stc = ed->GetControl();
    if ( !stc ) return;

    // check if smart indent is enabled
    const bool smartIndent = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/smart_indent"), true);
    if ( !smartIndent ) return;

    wxString langname = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    if ( langname != wxT("Pascal") ) return;

    ed->AutoIndentDone(); // we are responsible.

    wxChar ch = event.GetKey();
    if ( (ch == wxT('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == wxT('\r')) ) )
        DoIndent(ed, langname);   // indent because \n added
    else if ( ch != wxT(' ') )
        DoUnIndent(ed, langname); // un-indent because not a newline added

    if ( SelectionBraceCompletionEnabled() || stc->IsBraceShortcutActive() )
        ed->DoSelectionBraceCompletion(stc, ch);
}

void SmartIndentPascal::DoIndent(cbEditor* ed, const wxString& WXUNUSED(langname)) const
{
    cbStyledTextCtrl* stc = ed->GetControl();

    const int pos = stc->GetCurrentPos();
    const int currLine = stc->LineFromPosition(pos);
    if (currLine == 0)
        return;

    if(AutoIndentEnabled())
    {
        wxString indent = ed->GetLineIndentString(currLine-1);
        //wxChar b = GetLastNonWhitespaceChar(ed);

        wxString lw = GetLastNonCommentWord(ed).Lower();
        if ( lw.IsSameAs( wxT("begin") ) )
            Indent(stc, indent);

        stc->BeginUndoAction();
        stc->InsertText(pos, indent);
        stc->GotoPos(pos + indent.Length());
        stc->ChooseCaretX();
        stc->EndUndoAction();
    }
}

void SmartIndentPascal::DoUnIndent(cbEditor* ed, const wxString& WXUNUSED(langname)) const
{
    cbStyledTextCtrl* stc = ed->GetControl();

    wxString str = stc->GetLine(stc->GetCurrentLine()).Lower();
    str.Trim(false);
    str.Trim(true);

    int pos = stc->GetCurrentPos() - 4;
    if ( str.Matches( wxT("end")) )
        pos = FindBlockStart(stc, pos, wxT("begin"), wxT("end"), false );
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
