#ifndef EDITKEYWORDSDLG_H
#define EDITKEYWORDSDLG_H

#include <wx/dialog.h>
#include "globals.h"

class wxSpinCtrl;
class wxTextCtrl;
class EditorColourSet;

class EditKeywordsDlg : public wxDialog
{
    public:
        EditKeywordsDlg(wxWindow* parent, EditorColourSet* theme, HighlightLanguage lang);
        ~EditKeywordsDlg();
    protected:
        void OnSetChange(wxSpinEvent& event);

        wxSpinCtrl* spnSet;
    public:
        EditorColourSet* m_pTheme;
        HighlightLanguage m_Lang;
        wxTextCtrl* txtKeywords;
        int m_LastSet;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITKEYWORDSDLG_H
