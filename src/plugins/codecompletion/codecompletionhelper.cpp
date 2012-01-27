#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/string.h>

    #include <cbeditor.h>
#endif

#include <cbstyledtextctrl.h>

namespace CodeCompletionHelper
{
    bool GotoTokenPosition(cbEditor* editor, const wxString& target, int line)
    {
        if (!editor)
            return false;

        cbStyledTextCtrl* control = editor->GetControl();
        if (line > control->GetLineCount())
            return false;

    //    control->GotoLine(line); // old style
        editor->GotoLine(line, true); // center function on screen
        editor->SetFocus(); // ...and set focus to the editor
        const int startPos = control->GetCurrentPos();
        const int endPos = startPos + control->LineLength(line);
        if (endPos <= startPos)
            return false;

        int tokenPos = control->FindText(startPos, endPos, target,
                                         wxSCI_FIND_WHOLEWORD | wxSCI_FIND_MATCHCASE, nullptr);
        if (tokenPos != wxSCI_INVALID_POSITION)
            control->SetSelectionInt(tokenPos, tokenPos + target.Len());
        else
            control->GotoPos(startPos);

        return true;
    }
} // namespace CodeCompletionHelper
