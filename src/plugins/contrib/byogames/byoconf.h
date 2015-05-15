#ifndef BYOCONF_H
#define BYOCONF_H

#include <wx/intl.h>
#include "configurationpanel.h"

class wxCommandEvent;
class wxFlexGridSizer;
class wxStaticBoxSizer;
class wxCheckBox;
class wxSpinCtrl;
class wxStaticText;
class wxButton;

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
		static const long ID_CHECKBOX1;
		static const long ID_SPINCTRL1;
		static const long ID_CHECKBOX2;
		static const long ID_SPINCTRL2;
		static const long ID_CHECKBOX3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT2;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON3;
		static const long ID_STATICTEXT4;
		static const long ID_BUTTON4;
		static const long ID_STATICTEXT5;
		static const long ID_BUTTON5;
		static const long ID_STATICTEXT6;
		static const long ID_BUTTON6;
		//*)
		//(*Handlers(byoConf)
		void BTWSRefresh(wxCommandEvent& event);
		void ColChangeClick(wxCommandEvent& event);
		//*)
		//(*Declarations(byoConf)
		wxButton* m_Col1;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer1;
		wxButton* m_Col5;
		wxFlexGridSizer* FlexGridSizer2;
		wxSpinCtrl* m_MinWorkSpn;
		wxButton* m_Col2;
		wxButton* m_Col4;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxCheckBox* m_MaxPlaytimeChk;
		wxFlexGridSizer* FlexGridSizer3;
		wxCheckBox* m_OverworkChk;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxButton* m_Col3;
		wxStaticText* StaticText6;
		wxButton* m_Col6;
		wxSpinCtrl* m_OverworkSpn;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxSpinCtrl* m_MaxPlaytimeSpn;
		wxCheckBox* m_MinWorkChk;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // BYOCONF_H
