#ifndef HelpConfigDialog_H
#define HelpConfigDialog_H

#include <wx/dialog.h>
#include "help_common.h"

class HelpConfigDialog : public wxDialog
{
	public:
		HelpConfigDialog();
		virtual ~HelpConfigDialog();
	protected:
	    void Add(wxCommandEvent& event);
	    void Rename(wxCommandEvent& event);
	    void Delete(wxCommandEvent& event);
	    void Browse(wxCommandEvent& event);
	    void ListChange(wxCommandEvent& event);
	    void UpdateUI(wxUpdateUIEvent& event);
        void Ok();
		void Cancel();
	private:
        void UpdateEntry(int index);
        void ChooseFile();
        HelpFilesMap m_Map;
        int m_LastSel;
	DECLARE_EVENT_TABLE()
};

#endif // HelpConfigDialog
