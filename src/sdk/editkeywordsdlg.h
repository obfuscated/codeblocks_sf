#ifndef EDITKEYWORDSDLG_H
#define EDITKEYWORDSDLG_H

#include <wx/dialog.h>
#include "editorcolorset.h"

class wxSpinCtrl;
class wxTextCtrl;

class EditKeywordsDlg : public wxDialog
{
    public:
        EditKeywordsDlg(wxWindow* parent, EditorColorSet* theme, HighlightLanguage lang);
        ~EditKeywordsDlg();
    protected:
        void OnSetChange(wxSpinEvent& event);

        EditorColorSet* m_pTheme;
        HighlightLanguage m_Lang;
        wxSpinCtrl* spnSet;
        wxTextCtrl* txtKeywords;
        int m_LastSet;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITKEYWORDSDLG_H
