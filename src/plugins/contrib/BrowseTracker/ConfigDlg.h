///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ConfigDlg__
#define __ConfigDlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigDlg
///////////////////////////////////////////////////////////////////////////////
class ConfigDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
	
	public:
		wxCheckBox* Cfg_BrowseMarksEnabled;
		wxRadioBox* Cfg_MarkStyle;
		wxRadioBox* Cfg_ToggleKey;
		wxStaticText* m_staticText4;
		wxSlider* Cfg_LeftMouseDelay;
		wxRadioBox* Cfg_ClearAllKey;
		ConfigDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("BrowseTracker Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 380,504 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL );
		~ConfigDlg();
	
};

#endif //__ConfigDlg__
