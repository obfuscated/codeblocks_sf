#ifndef MULTISELECTDLG_H
#define MULTISELECTDLG_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/dialog.h>

class MultiSelectDlg : public wxDialog
{
	public:
		MultiSelectDlg(wxWindow* parent, const wxArrayString& items, bool selectall = false);
		virtual ~MultiSelectDlg();
		
		wxArrayString GetSelectedStrings();
		wxArrayInt GetSelectedIndices();
	protected:
	private:
};

#endif // MULTISELECTDLG_H
