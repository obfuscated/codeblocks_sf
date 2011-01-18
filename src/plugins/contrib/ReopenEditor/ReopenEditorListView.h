/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef REOPENEDITORLISTVIEW_H
#define REOPENEDITORLISTVIEW_H

#include "loggers.h"


class wxListEvent;
class cbProject;
class wxPanel;

class ReopenEditorListView : public wxPanel
{
    public:
        ReopenEditorListView(const wxArrayString& titles, const wxArrayInt& widths);
        ~ReopenEditorListView();

        virtual size_t GetItemsCount() const;
        virtual void Prepend(const wxString& msg, Logger::level lv = Logger::info);
        virtual void Prepend(const wxArrayString& colValues, Logger::level lv = Logger::info);
        virtual void RemoveAt(size_t i);
        virtual wxArrayString GetItemAsArray(long item) const;
        virtual wxString GetFilename(long item) const;
        virtual cbProject* GetProject(long item) const;
        virtual void SetProject(long item, cbProject* prj) const;
    private:
        wxListCtrl* m_pListControl;
        wxArrayString titles;
        wxArrayInt widths;
        struct ListStyles
        {
            wxFont font;
            wxColour colour;
        };
        ListStyles style[Logger::num_levels];
        void OnDoubleClick(wxListEvent& event);
//        void OnRightClick(wxListEvent& event);
        void OnContextMenu(wxContextMenuEvent& /*event*/);
        void OnReopenItems(wxCommandEvent& event);
        void OnRemoveItems(wxCommandEvent& event);
        void OnReopenAll(wxCommandEvent& /*event*/);
        void OnRemoveAll(wxCommandEvent& /*event*/);
        void DoShowContextMenu();
        void DoOpen(wxString fname);
        void DoOpen(wxArrayString fnames);

//        wxWindow* m_pPanel;

        DECLARE_EVENT_TABLE()
};

#endif // REOPENEDITORLISTVIEW_H

