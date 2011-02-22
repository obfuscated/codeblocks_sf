#ifndef AVCHANGESDLG_H
#define AVCHANGESDLG_H

//(*Headers(avChangesDlg)
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class avChangesDlg: public wxScrollingDialog
{
	public:

        const wxString& Changes() const;
        void SetTemporaryChangesFile(const wxString& fileName);

		avChangesDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~avChangesDlg();

		//(*Identifiers(avChangesDlg)
		static const long ID_ADD_BUTTON;
		static const long ID_EDIT_BUTTON;
		static const long ID_DELETE_BUTTON;
		static const long ID_CHANGES_GRID;
		static const long ID_SAVE_BUTTON;
		static const long ID_WRITE_BUTTON;
		static const long ID_CANCEL_BUTTON;
		//*)

	protected:

		//(*Handlers(avChangesDlg)
		void OnBtnAcceptClick(wxCommandEvent& event);
		void OnBtnSaveClick(wxCommandEvent& event);
		void OnBtnAddClick(wxCommandEvent& event);
		void OnBtnEditClick(wxCommandEvent& event);
		void OnBtnDeleteClick(wxCommandEvent& event);
		void OnBtnWriteClick(wxCommandEvent& event);
		void OnBtnCancelClick(wxCommandEvent& event);
		//*)

		//(*Declarations(avChangesDlg)
		wxButton* btnSave;
		wxButton* btnCancel;
		wxButton* btnWrite;
		wxButton* btnDelete;
		wxButton* btnAdd;
		wxGrid* grdChanges;
		wxBoxSizer* BoxSizer1;
		wxButton* btnEdit;
		//*)

	private:

        //Members
        wxString m_changes;
        wxString m_tempChangesFile;

        DECLARE_EVENT_TABLE()
};

#endif
