#ifndef BYOCONF_H
#define BYOCONF_H

#include <configurationpanel.h>

//(*Headers(byoConf)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

class byoConf: public cbConfigurationPanel
{
	public:

		byoConf(wxWindow* parent,wxWindowID id = -1);
		virtual ~byoConf();

        wxString GetTitle() const { return _("C::B games"); }
        wxString GetBitmapBaseName() const { return _T("generic-plugin"); }
        void OnApply();
        void OnCancel(){}

	protected:

		//(*Identifiers(byoConf)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON2,
		    ID_BUTTON3,
		    ID_BUTTON4,
		    ID_BUTTON5,
		    ID_BUTTON6,
		    ID_CHECKBOX1,
		    ID_CHECKBOX2,
		    ID_CHECKBOX3,
		    ID_SPINCTRL1,
		    ID_SPINCTRL2,
		    ID_SPINCTRL3,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4,
		    ID_STATICTEXT5,
		    ID_STATICTEXT6
		};
		//*)
		//(*Handlers(byoConf)
		void BTWSRefresh(wxCommandEvent& event);
		void ColChangeClick(wxCommandEvent& event);
		//*)
		//(*Declarations(byoConf)
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer2;
		wxCheckBox* m_MaxPlaytimeChk;
		wxSpinCtrl* m_MaxPlaytimeSpn;
		wxCheckBox* m_MinWorkChk;
		wxSpinCtrl* m_MinWorkSpn;
		wxCheckBox* m_OverworkChk;
		wxSpinCtrl* m_OverworkSpn;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer3;
		wxStaticText* StaticText1;
		wxButton* m_Col1;
		wxStaticText* StaticText2;
		wxButton* m_Col3;
		wxStaticText* StaticText3;
		wxButton* m_Col5;
		wxStaticText* StaticText4;
		wxButton* m_Col2;
		wxStaticText* StaticText5;
		wxButton* m_Col4;
		wxStaticText* StaticText6;
		wxButton* m_Col6;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
