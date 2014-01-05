/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITKEYWORDSDLG_H
#define EDITKEYWORDSDLG_H

#include "scrollingdialog.h"
#include "globals.h"

class wxSpinCtrl;
class wxTextCtrl;
class EditorColourSet;

class EditKeywordsDlg : public wxScrollingDialog
{
    public:
        EditKeywordsDlg(wxWindow* parent, EditorColourSet* theme, HighlightLanguage lang, const wxArrayString& descr);
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
        void UpdateDlg();
        const wxArrayString& descriptions;
        DECLARE_EVENT_TABLE()
};

#endif // EDITKEYWORDSDLG_H
