#ifndef MouseSapCfg_H
#define MouseSapCfg_H

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/settings.h>

#include "configurationpanel.h"
#include "MouseSap.h"


// ----------------------------------------------------------------------------
class cbMouseSapCfg: public cbConfigurationPanel
// ----------------------------------------------------------------------------
{

	public:
		cbMouseSapCfg(wxWindow* parent, MouseSap* pOwnerClass, wxWindowID id = -1);
		virtual ~cbMouseSapCfg();
    public:
        // virtual routines required by cbConfigurationPanel
        wxString GetTitle() const { return _("MouseSap"); }
        wxString GetBitmapBaseName() const;
        void OnApply();
        void OnCancel(){}
        virtual void InitDialog() { } /*trap*/

        // pointer to owner of the configuration diaglog needed to
        // complete the OnApply/OnCancel EndModal() logic
        MouseSap* pOwnerClass;

        bool GetMouseSapEnabled() { return m_pluginEnabled->GetValue(); }

        void SetMouseSapEnabled(bool value)
                { m_pluginEnabled->SetValue(value); }

	protected:

		void OnDoneButtonClick(wxCommandEvent& event);

		//Declarations(cbMouseSapCfg)
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* StaticText1;
		wxCheckBox* m_pluginEnabled;

	private:

		DECLARE_EVENT_TABLE()
};

#endif // MouseSapCfg_H
//
