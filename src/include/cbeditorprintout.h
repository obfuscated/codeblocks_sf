/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBEDITORPRINTOUT_H
#define CBEDITORPRINTOUT_H

#include <wx/print.h>

class cbStyledTextCtrl;

class cbEditorPrintout : public wxPrintout
{
    public:
        cbEditorPrintout(const wxString& title, cbStyledTextCtrl* control, bool selectionOnly);
        ~cbEditorPrintout();
        bool OnPrintPage(int page);
        bool HasPage(int page);
        void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
        bool OnBeginDocument(int startPage, int endPage);
    protected:
        bool ScaleDC(wxDC *dc);
        cbStyledTextCtrl* m_TextControl;
        wxRect m_pageRect;
        wxRect m_printRect;
        int m_printed;
        int m_SelStart;
        int m_SelEnd;
        wxArrayInt* m_pPageSelStart;
};

#endif // CBEDITORPRINTOUT_H
