///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 24 2006)
// http://wxformbuilder.sourceforge.net/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __messageBoxForm__
#define __messageBoxForm__

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

#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

/**
 * Class messageBoxForm
 */
class messageBoxForm : public wxDialog
{
	public:
		wxTextCtrl* m_messageBoxTextCtrl;
		wxStaticLine* m_staticline;
		wxStdDialogButtonSizer* m_sdbSizer;

	public:
		messageBoxForm( wxWindow* parent, int id = -1, wxString title = wxT("Properties"),
                    wxPoint pos = wxDefaultPosition,
                    wxSize size = wxSize( 537,228 ),
                    int dlgStyle = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP,
                    int txtStyle = wxTE_CENTRE|wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER );

    wxButton* m_pYesButton;
    wxButton* m_pNoButton;
    wxButton* m_pOkButton;
    wxButton* m_pCancelButton;
};

#endif //__messageBoxForm__
