/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTDLG_H
#define INCREMENTALSELECTLISTDLG_H

#include "settings.h"
#include "scrollingdialog.h"
#include <wx/textctrl.h>
#include <wx/listbox.h>

class IncrementalSelectIterator; // forward decl


class DLLIMPORT IncrementalSelectListDlg : public wxScrollingDialog
{
    public:
        IncrementalSelectListDlg(wxWindow* parent,
                                 const IncrementalSelectIterator& iterator,
                                 const wxString& caption = wxEmptyString,
                                 const wxString& message = wxEmptyString);
        virtual ~IncrementalSelectListDlg();
        wxString GetStringSelection();
        wxIntPtr GetSelection();
    protected:
        void FillList();
        void OnSearch(wxCommandEvent& event);
        void OnSelect(wxCommandEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        wxListBox* m_List;
        wxTextCtrl* m_Text;
        const IncrementalSelectIterator &m_Iterator;
    private:
        DECLARE_EVENT_TABLE();
};

class DLLIMPORT IncrementalSelectIterator
{
    public:
        virtual ~IncrementalSelectIterator() {}

        virtual long GetCount() const = 0;
        virtual wxString GetItem(long index) const = 0;
        virtual wxString GetDisplayItem(long index) const { return GetItem(index); }
};

class DLLIMPORT IncrementalSelectIteratorStringArray : public IncrementalSelectIterator
{
    public:
        IncrementalSelectIteratorStringArray(const wxArrayString& array) : m_Array(array)
        {
        }

        virtual long GetCount() const { return m_Array.GetCount(); }
        virtual wxString GetItem(long index) const { return m_Array[index]; }
    private:
        const wxArrayString& m_Array;
};

#endif // INCREMENTALSELECTLISTDLG_H

