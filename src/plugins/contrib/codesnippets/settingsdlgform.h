#ifndef SNIPPETSCFGFORM_H
#define SNIPPETSCFGFORM_H


///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 24 2006)
// http://wxformbuilder.sourceforge.net/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
//#ifdef WX_GCH
//#include <wx_pch.h>
//#else
//#include <wx/wx.h>
//#endif

// For compilers that support precompilation, includes <wx/wx.h>
//#include <wx/wxprec.h>
//
//#ifndef WX_PRECOMP
//	#include <wx/wx.h>
//#endif

#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_EXTEDITORBUTTON 1000
#define ID_SNIPPETFILE 1001
#define ID_SNIPPETFILEBUTTON 1002

/**
 * Class SettingsDlgForm
 */
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
		wxStdDialogButtonSizer* m_sdbSizer;

	public:
		SettingsDlgForm( wxWindow* parent, int id = -1, wxString title = wxT(""), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 483,190 ), int style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

};
#endif // SNIPPETSCFGFORM_H
