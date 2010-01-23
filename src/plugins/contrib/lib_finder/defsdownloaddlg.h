#ifndef DEFSDOWNLOADDLG_H
#define DEFSDOWNLOADDLG_H

//(*Headers(DefsDownloadDlg)
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)


class DefsDownloadDlg: public wxScrollingDialog
{
	public:

		DefsDownloadDlg(wxWindow* parent);
		virtual ~DefsDownloadDlg();

	private:

		//(*Declarations(DefsDownloadDlg)
		wxCheckBox* m_Tree;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxTreeCtrl* m_KnownLibrariesTree;
		wxTextCtrl* m_Filter;
		wxButton* m_Add;
		wxListBox* m_UsedLibraries;
		wxButton* m_Remove;
		//*)

		//(*Identifiers(DefsDownloadDlg)
		static const long ID_LISTBOX1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_TREECTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		static const long ID_CHECKBOX1;
		static const long ID_BUTTON3;
		//*)

		//(*Handlers(DefsDownloadDlg)
		//*)

		void FetchList();

		DECLARE_EVENT_TABLE()
};

#endif
