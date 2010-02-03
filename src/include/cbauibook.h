/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBAUIBOOK_H_INCLUDED
#define CBAUIBOOK_H_INCLUDED

#include <wx/aui/auibook.h>
#include <wx/dynarray.h>

WX_DEFINE_ARRAY_PTR(wxAuiTabCtrl*,cbAuiTabCtrlArray);

class cbAuiNotebook : public wxAuiNotebook
{
    public:
        cbAuiNotebook(wxWindow* pParent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE);

        // Advances the selection, generation page selection events
        void AdvanceSelection(bool forward = true);
        wxString SavePerspective();
        bool LoadPerspective(const wxString& layout);
        int GetTabPositionFromIndex(int index);

    protected:
        void OnNavigationKey(wxNavigationKeyEvent& event);
        void UpdateTabControlsArray();
        cbAuiTabCtrlArray m_TabCtrls;

        DECLARE_EVENT_TABLE()
};

#endif // CBAUIBOOK_H_INCLUDED
