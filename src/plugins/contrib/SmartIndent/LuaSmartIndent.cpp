#include "LuaSmartIndent.h"

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
    PluginRegistrant<LuaSmartIndent> reg(wxT("LuaSmartIndent"));
}
void LuaSmartIndent::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
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

    cbStyledTextCtrl *stc = ed->GetControl();
    if (!stc)
        return;

    wxString langname = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    if ( langname != wxT("Lua") )
        return;

    ed->AutoIndentDone(); // we are responsible.

    // if a newline was added
    const int pos = stc->GetCurrentPos();
    int currLine = stc->LineFromPosition(pos);

    if (currLine == 0)
        return;

    wxChar ch = event.GetKey();

    // indent
    if ( (ch == wxT('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == wxT('\r')) ) )
    {
        if (AutoIndentEnabled())
        {
            stc->BeginUndoAction();
            wxString indent = ed->GetLineIndentString(currLine - 1);
            BraceIndent(stc, indent);
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
            stc->EndUndoAction();
        }
    }
}

bool LuaSmartIndent::BraceIndent(cbStyledTextCtrl *stc, wxString &indent)const
{
    if ( BraceSmartIndentEnabled() )
    {
        int style = wxSCI_LUA_STRING;

        int brace_position = GetFirstBraceInLine(stc, style);
        return Indent(stc, indent, brace_position);
    }
    return false;
}
