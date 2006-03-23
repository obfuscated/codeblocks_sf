#ifndef WXSBITMAPICONEDITORDLG_H
#define WXSBITMAPICONEDITORDLG_H

//(*Headers(wxsBitmapIconEditorDlg)
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)
#include <wx/timer.h>

#include "wxsbitmapiconproperty.h"

class wxsBitmapIconEditorDlg: public wxDialog
{
	public:

		wxsBitmapIconEditorDlg(wxWindow* parent,wxsBitmapIconData& Data,const wxString& DefaultClient,wxWindowID id = -1);
		virtual ~wxsBitmapIconEditorDlg();

		//(*Identifiers(wxsBitmapIconEditorDlg)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON3,
		    ID_COMBOBOX1,
		    ID_COMBOBOX2,
		    ID_RADIOBUTTON1,
		    ID_RADIOBUTTON2,
		    ID_RADIOBUTTON3,
		    ID_STATICBITMAP1,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_TEXTCTRL1
		};
		//*)

	protected:

		//(*Handlers(wxsBitmapIconEditorDlg)
		void OnTimer(wxTimerEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnFileNameText(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnArtIdSelect(wxCommandEvent& event);
		void OnUpdatePreview(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsBitmapIconEditorDlg)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer5;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer1;
		wxRadioButton* NoImage;
		wxRadioButton* ImageFile;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* FileName;
		wxButton* Button3;
		wxRadioButton* ImageArt;
		wxBoxSizer* BoxSizer4;
		wxFlexGridSizer* FlexGridSizer2;
		wxStaticText* StaticText1;
		wxComboBox* ArtId;
		wxStaticText* StaticText2;
		wxComboBox* ArtClient;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxStaticBitmap* Preview;
		wxBoxSizer* BoxSizer3;
		wxButton* Button1;
		wxButton* Button2;
		//*)
		wxTimer* Timer1;

	private:
	
        void UpdatePreview();
        void WriteData(wxsBitmapIconData& Data);
        void ReadData(wxsBitmapIconData& Data);
	
        wxString DefaultClient;
        wxsBitmapIconData& Data;

		DECLARE_EVENT_TABLE()
};

#endif
