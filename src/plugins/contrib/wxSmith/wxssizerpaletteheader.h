#ifndef WXSSIZERPALETTEHEADER_H
#define WXSSIZERPALETTEHEADER_H

//(*Headers(wxsSizerPaletteHeader)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
//*)
#include <wx/timer.h>

class WXSCLASS wxsWidget;
class WXSCLASS wxsSizerExtraParams;

class WXSCLASS wxsSizerPaletteHeader: public wxPanel
{
	public:

		wxsSizerPaletteHeader(wxWindow* parent,wxsWidget* Modified,wxsSizerExtraParams* Params,wxWindowID Id=-1);
		virtual ~wxsSizerPaletteHeader();

		//(*Identifiers(wxsSizerPaletteHeader)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_CHECKBOX2,
            ID_CHECKBOX3,
            ID_CHECKBOX4,
            ID_CHECKBOX5,
            ID_CHECKBOX6,
            ID_RADIOBUTTON10,
            ID_RADIOBUTTON11,
            ID_RADIOBUTTON12,
            ID_RADIOBUTTON4,
            ID_RADIOBUTTON5,
            ID_RADIOBUTTON6,
            ID_RADIOBUTTON7,
            ID_RADIOBUTTON8,
            ID_RADIOBUTTON9,
            ID_SPINCTRL1
        };
        //*)

	protected:

		//(*Handlers(wxsSizerPaletteHeader)
		void OnBrdChange(wxCommandEvent& event);
        void OnBrdSizeChange(wxSpinEvent& event);
        void OnPlaceChange(wxCommandEvent& event);
        void OnTimer(wxTimerEvent& event);
        //*)

		//(*Declarations(wxsSizerPaletteHeader)
        wxFlexGridSizer* FlexGridSizer1;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer2;
        wxGridSizer* GridSizer1;
        wxCheckBox* BrdTop;
        wxCheckBox* BrdLeft;
        wxCheckBox* BrdRight;
        wxCheckBox* BrdBottom;
        wxSpinCtrl* BrdSize;
        wxStaticBoxSizer* StaticBoxSizer2;
        wxFlexGridSizer* FlexGridSizer3;
        wxGridSizer* GridSizer2;
        wxRadioButton* PlaceLT;
        wxRadioButton* PlaceCT;
        wxRadioButton* PlaceRT;
        wxRadioButton* PlaceLC;
        wxRadioButton* PlaceCC;
        wxRadioButton* PlaceRC;
        wxRadioButton* PlaceLB;
        wxRadioButton* PlaceCB;
        wxRadioButton* PlaceRB;
        wxBoxSizer* BoxSizer1;
        wxCheckBox* PlaceShp;
        wxCheckBox* PlaceExp;
        //*)

	private:

        void ReadData();
        void SaveData();

        wxsWidget* Widget;
        wxsSizerExtraParams* Params;
        wxTimer Timer;

		DECLARE_EVENT_TABLE()
};

#endif
