#include "PythonSmartIndent.h"

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
    PluginRegistrant<PythonSmartIndent> reg(wxT("PythonSmartIndent"));
}

bool PythonSmartIndent::InComment(const wxString& WXUNUSED(LanguageName), int style) const
{
    return style == wxSCI_P_COMMENTBLOCK ||
           style == wxSCI_P_COMMENTLINE;
}

void PythonSmartIndent::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
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

    const wxString langname = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    if ( langname != wxT("Python") )
        return;

    ed->AutoIndentDone(); // we are responsible.

    const int pos = stc->GetCurrentPos();
    int currLine = stc->LineFromPosition(pos);

    if ( currLine == 0)
        return;

    const wxChar ch = event.GetKey();

    // indent
    if ( (ch == wxT('\n')) || ( (stc->GetEOLMode() == wxSCI_EOL_CR) && (ch == wxT('\r')) ) )
    {
        if (AutoIndentEnabled())
        {
            wxString indent = ed->GetLineIndentString(currLine-1);
            const wxChar b = GetLastNonWhitespaceChar(ed);

            if (b == wxT(':'))
                Indent(stc, indent);
            stc->BeginUndoAction();
            stc->InsertText(pos, indent);
            stc->GotoPos(pos + indent.Length());
            stc->ChooseCaretX();
            stc->EndUndoAction();
        }
    }
}
