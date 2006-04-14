#ifndef WXSSETTINGSDLG_H
#define WXSSETTINGSDLG_H

#include "wxsglobals.h"

//(*Headers(wxsSettingsDlg)
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

class wxsSettingsDlg: public cbConfigurationPanel
{
    public:

        wxsSettingsDlg(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxsSettingsDlg();

        //(*Identifiers(wxsSettingsDlg)
        enum Identifiers
        {
            ID_BUTTON1 = 0x1000,
            ID_BUTTON2,
            ID_COMBOBOX1,
            ID_RADIOBUTTON1,
            ID_RADIOBUTTON2,
            ID_RADIOBUTTON3,
            ID_RADIOBUTTON4,
            ID_SPINCTRL1,
            ID_STATICTEXT1,
            ID_STATICTEXT2,
            ID_STATICTEXT3,
            ID_STATICTEXT4,
            ID_STATICTEXT5,
            ID_STATICTEXT6
        };
        //*)

        wxString GetTitle() const { return _("wxSmith settings"); }
        wxString GetBitmapBaseName() const { return _T("wxsmith"); }
        void OnApply();
        void OnCancel(){}
    protected:

        //(*Handlers(wxsSettingsDlg)
        void OnDragTargetColClick(wxCommandEvent& event);
        void OnDragParentColClick(wxCommandEvent& event);
        void OnDragAssistTypeSelect(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsSettingsDlg)
        wxFlexGridSizer* FlexGridSizer1;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer2;
        wxComboBox* DragAssistType;
        wxButton* DragTargetCol;
        wxButton* DragParentCol;
        wxFlexGridSizer* FlexGridSizer3;
        wxRadioButton* Icons16;
        wxRadioButton* Icons32;
        wxStaticText* StaticText6;
        wxFlexGridSizer* FlexGridSizer4;
        wxRadioButton* TIcons16;
        wxRadioButton* TIcons32;
        wxStaticBoxSizer* StaticBoxSizer2;
        wxFlexGridSizer* FlexGridSizer5;
        wxSpinCtrl* PrevFetchDelay;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
