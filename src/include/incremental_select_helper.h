
/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTBASE_H
#define INCREMENTALSELECTLISTBASE_H

#include "settings.h"
#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/event.h>
#include <vector>

#ifndef CB_PRECOMP
    #include <wx/listctrl.h>
#endif // CB_PRECOMP

class wxCommandEvent;
class wxKeyEvent;
class wxListEvent;
class wxTextCtrl;


class DLLIMPORT IncrementalSelectIterator
{
    public:
        virtual ~IncrementalSelectIterator();

        virtual int GetFilteredCount() const = 0;
        virtual void Reset() = 0;
        virtual void AddIndex(int index) = 0;
        virtual int GetUnfilteredIndex(int index) const = 0;

        virtual int GetTotalCount() const = 0;
        virtual const wxString& GetItemFilterString(int index) const = 0;
        virtual wxString GetDisplayText(int index, int column) const = 0;

        virtual int GetColumnWidth(int column) const;
        virtual void CalcColumnWidth(wxListCtrl &list);

};

class DLLIMPORT IncrementalSelectIteratorIndexed : public IncrementalSelectIterator
{
    public:
        virtual ~IncrementalSelectIteratorIndexed() {}

        int GetFilteredCount() const override;
        void Reset() override;
        void AddIndex(int index) override;
        int GetUnfilteredIndex(int index) const override;
    protected:
        std::vector<int> m_indices;
};

class DLLIMPORT IncrementalSelectHandler : public wxEvtHandler
{
    public:
        IncrementalSelectHandler(wxDialog* parent, IncrementalSelectIterator *iterator);
        ~IncrementalSelectHandler() override;

        void Init(wxListCtrl *list, wxTextCtrl *text);
        void DeInit(wxWindow *window);
        void FilterItems();
        int GetSelection();

    private:
        void OnKeyDown(wxKeyEvent &event);
        void OnTextChanged(wxCommandEvent &event);
        void OnItemActivated(wxListEvent &event);

    private:
        wxDialog *m_parent;
        wxListCtrl *m_list;
        wxTextCtrl *m_text;
        IncrementalSelectIterator *m_iterator;
};

/// Class that implements a virtual list control that uses an IncrementalSelectIterator to populate the list items.
class DLLIMPORT IncrementalListCtrl : public wxListCtrl
{
    public:
        IncrementalListCtrl(wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize, long style = wxLC_ICON,
                            const wxValidator &validator = wxDefaultValidator,
                            const wxString &name = wxListCtrlNameStr);

        wxString OnGetItemText(long item, long column) const override;
        void SetIterator(IncrementalSelectIterator *iterator);
    private:
        IncrementalSelectIterator *m_Iterator;
};

/// Simple iterator that uses wxArrayString as data source.
class DLLIMPORT IncrementalSelectArrayIterator : public IncrementalSelectIteratorIndexed
{
    public:
        IncrementalSelectArrayIterator(const wxArrayString &items);

        int GetTotalCount() const;
        const wxString& GetItemFilterString(int index) const;
        wxString GetDisplayText(int index, int column) const;
        int GetColumnWidth(int column) const;
        void CalcColumnWidth(wxListCtrl &list);
    private:
        const wxArrayString &m_items;
        int m_columnWidth;
};

/// Simple incremental select dialog that shows a single column and doesn't have much ui elements,
/// except the text and list controls.
class DLLIMPORT IncrementalSelectDialog : public wxDialog
{
    public:
        IncrementalSelectDialog(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                                const wxString &message);
        ~IncrementalSelectDialog() override;

        int GetSelection();
    private:
        IncrementalSelectHandler m_handler;
    private:
        IncrementalListCtrl* m_resultList;
        wxTextCtrl* m_text;
    protected:
        void BuildContent(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                          const wxString &message);

        DECLARE_EVENT_TABLE()
};

#endif // INCREMENTALSELECTLISTBASE_H

