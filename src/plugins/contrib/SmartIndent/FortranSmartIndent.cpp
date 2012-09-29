#include "FortranSmartIndent.h"

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
    PluginRegistrant<FortranSmartIndent> reg(wxT("FortranSmartIndent"));
}

bool FortranSmartIndent::InComment(const wxString& LanguageName, int style) const
{
    return style == wxSCI_F_COMMENT;
}

void FortranSmartIndent::OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const
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
            wxString lineText = stc->GetLine(currLine - 1).BeforeFirst('!').Lower();
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
                   && !secText.StartsWith(_T("procedure:")) ) )
            {
                stc->Tab();
            }
        }

        stc->EndUndoAction();
    }
}
