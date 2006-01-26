#ifndef CONFIGURATIONPANEL_H
#define CONFIGURATIONPANEL_H

#include "settings.h"
#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>

/** @brief Base class for plugin configuration panels. */
class DLLIMPORT cbConfigurationPanel : public wxPanel
{
    public:
        cbConfigurationPanel(){}
        virtual ~cbConfigurationPanel(){}

        /// @return the panel's title.
        virtual wxString GetTitle() = 0;
        /// @return the panel's bitmap base name. You must supply two bitmaps: <basename>.png and <basename>-off.png...
        virtual wxString GetBitmapBaseName() = 0;
        /// Called when the user chooses to apply the configuration.
        virtual void OnApply() = 0;
        /// Called when the user chooses to cancel the configuration.
        virtual void OnCancel() = 0;
};

/// @brief A simple dialog that wraps a cbConfigurationPanel.
class DLLIMPORT cbConfigurationDialog : public wxDialog
{
	public:
		cbConfigurationDialog(wxWindow* parent, int id, const wxString& title, cbConfigurationPanel* panel)
            : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize),
            m_pPanel(panel)
		{
		    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
            bs->Add(m_pPanel, 1, wxGROW | wxRIGHT | wxTOP | wxBOTTOM, 8);

            wxStaticLine* line = new wxStaticLine(this);
            bs->Add(line, 0, wxGROW | wxLEFT | wxRIGHT, 8);

            m_pOK = new wxButton(this, wxID_OK, _("&OK"));
            m_pOK->SetDefault();
            m_pCancel = new wxButton(this, wxID_CANCEL, _("&Cancel"));
            wxStdDialogButtonSizer* but = new wxStdDialogButtonSizer;
            but->AddButton(m_pOK);
            but->AddButton(m_pCancel);
            but->Realize();
            bs->Add(but, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 8);

            SetSizer(bs);
            bs->SetSizeHints(this);
            CenterOnParent();
		}
		~cbConfigurationDialog(){}

		void EndModal(int retCode)
		{
		    if (retCode == wxID_OK)
                m_pPanel->OnApply();
            else
                m_pPanel->OnCancel();
            wxDialog::EndModal(retCode);
		}
	protected:
        cbConfigurationPanel* m_pPanel;
        wxButton* m_pOK;
        wxButton* m_pCancel;
	private:

};

#endif // CONFIGURATIONPANEL_H
