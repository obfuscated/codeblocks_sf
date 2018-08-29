#ifndef DLGMANAGEBLOB_H
#define DLGMANAGEBLOB_H

//(*Headers(dlgmanageblob)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
//*)

class ManageGlobsDlg: public wxDialog
{
	public:

		ManageGlobsDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ManageGlobsDlg();

		//(*Declarations(ManageGlobsDlg)
		wxButton* btnAdd;
		wxButton* btnCancel;
		wxButton* btnDelete;
		wxButton* btnEdit;
		wxButton* btnOk;
		wxListCtrl* lstGlobsList;
		//*)

	protected:

		//(*Identifiers(ManageGlobsDlg)
		static const long ID_LISTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(ManageGlobsDlg)
		void OnAddClick(wxCommandEvent& event);
		void OnDeleteClick(wxCommandEvent& event);
		void OnEditClick(wxCommandEvent& event);
		void OnOkClick(wxCommandEvent& event);
		//*)

		void PopulateList();
		bool GlobsChanged();

		std::vector<cbProject::Glob> m_GlobList;

		DECLARE_EVENT_TABLE()
};

#endif
