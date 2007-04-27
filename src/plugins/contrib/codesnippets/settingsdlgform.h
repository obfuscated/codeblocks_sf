///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __settingsdlgform__
#define __settingsdlgform__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/statline.h>
#include <wx/slider.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_EXTEDITORBUTTON 1000
#define ID_SNIPPETFILE 1001
#define ID_SNIPPETFILEBUTTON 1002

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlgForm
///////////////////////////////////////////////////////////////////////////////
class SettingsDlgForm : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_ExtEditorTextCtrl;
		wxButton* m_ExtEditorButton;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_SnippetFileTextCtrl;
		wxButton* m_SnippetFileButton;
		wxStaticText* m_staticText7;
		wxRadioButton* m_RadioFloatBtn;
		wxRadioButton* m_RadioDockBtn;
		wxRadioButton* m_RadioExternalBtn;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText4;
		wxSlider* m_MouseSpeedSlider;
		wxStaticText* m_staticText5;
		wxSlider* m_MouseScrollSlider;
		wxStaticText* m_staticText6;
		wxSlider* m_MouseDelaylider;
		wxStdDialogButtonSizer* m_sdbSizer;
	
	public:
		SettingsDlgForm( wxWindow* parent, int id = ID_DEFAULT, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 462,492 ), int style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	
};

#endif //__settingsdlgform__
