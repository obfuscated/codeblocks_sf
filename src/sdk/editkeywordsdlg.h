#ifndef EDITKEYWORDSDLG_H
#define EDITKEYWORDSDLG_H

#include <wx/dialog.h>
#include "editorcolorset.h"

class EditKeywordsDlg : public wxDialog
{
    public:
        EditKeywordsDlg(wxWindow* parent, EditorColorSet* theme, HighlightLanguage lang);
        ~EditKeywordsDlg();

        wxString GetLangKeywords();
        wxString GetDocKeywords();
        wxString GetUserKeywords();
    protected:
    private:
};

#endif // EDITKEYWORDSDLG_H
