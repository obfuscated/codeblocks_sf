
/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTBASE_H
#define INCREMENTALSELECTLISTBASE_H

#include "settings.h"
#include <wx/string.h>
#include <wx/event.h>
#include <vector>

class wxCommandEvent;
class wxDialog;
class wxKeyEvent;
class wxListCtrl;
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


#endif // INCREMENTALSELECTLISTBASE_H

