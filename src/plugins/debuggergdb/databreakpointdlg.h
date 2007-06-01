#ifndef DATABREAKPOINTDLG_H
#define DATABREAKPOINTDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(DataBreakpointDlg)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
//*)

class DataBreakpointDlg: public wxDialog
{
	public:

		DataBreakpointDlg(wxWindow* parent,wxWindowID id = -1,bool enabled=true,int selection=1);
		virtual ~DataBreakpointDlg();

		//(*Identifiers(DataBreakpointDlg)
		static const long ID_CHECKBOX1;
		static const long ID_RADIOBOX1;
		//*)
		
		bool IsEnabled();
		int GetSelection();

	protected:

		//(*Handlers(DataBreakpointDlg)
		//*)

		//(*Declarations(DataBreakpointDlg)
		wxBoxSizer* BoxSizer1;
		wxCheckBox* CheckBox1;
		wxRadioBox* RadioBox1;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
