///////////////////////////////////////////////////////////////////////////////
// Name:		fnb_customize_dlg.h
// Purpose:     declaration of the customize dialog used by wxFlatNotebook
// Author:      Eran Ifrah <erani.ifrah@gmail.com>
// Created:     30/12/2005
// Modified:    01/01/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license <http://www.wxwidgets.org/licence3.txt>
///////////////////////////////////////////////////////////////////////////////
#ifndef __fnb_customize_dlg__
#define __fnb_customize_dlg__

#include <wx/wxFlatNotebook/wxFlatNotebookSDK.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "wx/dialog.h"
#include "wx/panel.h"

class wxCheckBox;
class wxRadioBox;

///////////////////////////////////////////////////////////////////////////////
/// Class wxFNBCustomizeDialog
///////////////////////////////////////////////////////////////////////////////
class WXDLLIMPEXP_FNB wxFNBCustomizeDialog : public wxDialog
{
protected:
	wxStaticLine* m_staticline2;
	wxButton* m_close;
	long m_options;

	// Option's page members
	wxRadioBox* m_styles;
	wxRadioBox* m_tabVPosition;
	wxRadioBox* m_navigationStyle;
	wxCheckBox* m_tabBorder;
	wxCheckBox* m_hideCloseButton;
	wxCheckBox* m_mouseMiddleCloseTab;
	wxCheckBox* m_xButtonOnTab;
	wxCheckBox* m_dlbClickCloseTab;
	wxCheckBox* m_smartTabbing;
	wxCheckBox* m_allowDragAndDrop;
	wxCheckBox* m_foreignDnD;
	wxCheckBox* m_gradient;
	wxCheckBox* m_colorfulTab;

public:
	wxFNBCustomizeDialog( wxWindow* parent, long options, int id = wxID_ANY, wxString title = _("Customize"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );

protected:
	void OnClose(wxCommandEvent &event);
	void OnStyle(wxCommandEvent &event);

private:
	wxPanel *CreateOptionsPage();
	void ConnectEvents();
};

#endif //__fnb_customize_dlg__

