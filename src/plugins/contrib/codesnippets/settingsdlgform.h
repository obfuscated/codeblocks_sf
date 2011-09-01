///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __settingsdlgform__
#define __settingsdlgform__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include "scrollingdialog.h"

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_EXTEDITORBUTTON 1000
#define ID_SNIPPETFILE 1001
#define ID_SNIPPETFILEBUTTON 1002

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlgForm
///////////////////////////////////////////////////////////////////////////////
class SettingsDlgForm : public wxScrollingDialog
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

		wxCheckBox* m_EditorsStayOnTopChkBox;
		wxCheckBox* m_ToolTipsChkBox;
		wxStaticText* m_CfgFolderStaticText;
		wxStaticText* m_IniFolderStaticText;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;

	public:
		wxTextCtrl* m_CfgFolderTextCtrl;
		wxTextCtrl* m_IniFolderTextCtrl;
		SettingsDlgForm( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 462,389 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SettingsDlgForm();

};

#endif //__settingsdlgform__
