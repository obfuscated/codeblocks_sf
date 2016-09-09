
/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef INCREMENTALSELECTLISTBASE_H
#define INCREMENTALSELECTLISTBASE_H

#include "settings.h"
#include "scrollingdialog.h"
#include <wx/textctrl.h>
#include <wx/listbox.h>

class IncrementalSelectIterator; // forward decl

class DLLIMPORT IncrementalSelectListBase : public wxScrollingDialog
{
    public:
        IncrementalSelectListBase(wxWindow* parent,
                                 const IncrementalSelectIterator& iterator,
                                 const wxString& dialog,
                                 const wxString& caption = wxEmptyString,
                                 const wxString& message = wxEmptyString);
        virtual ~IncrementalSelectListBase();
        void OnSearch(wxCommandEvent& event);
        void OnSelect(wxCommandEvent& event);
        void OnKeyDown(wxKeyEvent& event);
    protected:
        /** fetch the search key word from the text control, and filter the items */
        void FilterItems();

        /** handle user pressed some special key in the editor ctrl
         *  For example, if user press the UP key, we have to select some text before the cursor
         */
        void KeyDownAction(wxKeyEvent& event, int &sel, size_t selMax);

        /** fill the m_List with filtered items */
        void FillList();

        /** refresh the list */
        virtual void FillData(){};
        virtual void GetCurrentSelection(int &sel, size_t &selMax){};
        virtual void UpdateCurrentSelection(int sel, size_t selPrevious){};
        virtual wxIntPtr GetSelection(){return 0;};
        // Controls
        wxListBox* m_List;   /// for showing the filtered items
        wxTextCtrl* m_Text;  /// for entering search keys
        // Data
        wxArrayString m_Result;  /// the filtered items
        wxArrayLong m_Indexes;   /// the filtered item index in the list
        bool m_Promoted;
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

#endif // INCREMENTALSELECTLISTBASE_H

