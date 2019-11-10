#ifndef DEFSDOWNLOADDLG_H
#define DEFSDOWNLOADDLG_H

//(*Headers(DefsDownloadDlg)
#include "scrollingdialog.h"
class wxTextCtrl;
class wxStaticBoxSizer;
class wxListBox;
class wxButton;
class wxTreeEvent;
class wxBoxSizer;
class wxStaticText;
class wxTreeCtrl;
class wxCheckBox;
//*)


class DefsDownloadDlg: public wxScrollingDialog
{
	public:

		DefsDownloadDlg(wxWindow* parent);
		virtual ~DefsDownloadDlg();

	private:

		//(*Declarations(DefsDownloadDlg)
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxButton* m_Remove;
		wxTreeCtrl* m_KnownLibrariesTree;
		wxTextCtrl* m_Filter;
		wxButton* m_Add;
		wxCheckBox* m_Tree;
		wxListBox* m_UsedLibraries;
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
