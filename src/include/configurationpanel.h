#ifndef CONFIGURATIONPANEL_H
#define CONFIGURATIONPANEL_H

#include "settings.h"
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/string.h>

class wxButton;
class wxWindow;

/** @brief Base class for plugin configuration panels. */
class DLLIMPORT cbConfigurationPanel : public wxPanel
{
    public:
        cbConfigurationPanel(){}
        virtual ~cbConfigurationPanel(){}

        /// @return the panel's title.
        virtual wxString GetTitle() const = 0;
        /// @return the panel's bitmap base name. You must supply two bitmaps: <basename>.png and <basename>-off.png...
        virtual wxString GetBitmapBaseName() const = 0;
        /// Called when the user chooses to apply the configuration.
        virtual void OnApply() = 0;
        /// Called when the user chooses to cancel the configuration.
        virtual void OnCancel() = 0;
};

/// @brief A simple dialog that wraps a cbConfigurationPanel.
class DLLIMPORT cbConfigurationDialog : public wxDialog
{
	public:
		cbConfigurationDialog(wxWindow* parent, int id, const wxString& title);
		void AttachConfigurationPanel(cbConfigurationPanel* panel);
		~cbConfigurationDialog();

		void EndModal(int retCode);
	protected:
        cbConfigurationPanel* m_pPanel;
        wxButton* m_pOK;
        wxButton* m_pCancel;
	private:

};

#endif // CONFIGURATIONPANEL_H
