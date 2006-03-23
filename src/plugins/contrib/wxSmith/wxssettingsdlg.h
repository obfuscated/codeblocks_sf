#ifndef WXSSETTINGSDLG_H
#define WXSSETTINGSDLG_H

#include "wxsglobals.h"

//(*Headers(wxsSettingsDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/xrc/xmlres.h>
//*)

class wxsSettingsDlg: public cbConfigurationPanel
{
    public:

        wxsSettingsDlg(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxsSettingsDlg();

        //(*Identifiers(wxsSettingsDlg)
        //*)

        wxString GetTitle(){ return _("wxSmith settings"); }
        wxString GetBitmapBaseName(){ return _T("wxsmith"); }
        void OnApply();
        void OnCancel(){}
    protected:

        //(*Handlers(wxsSettingsDlg)
        void OnDragTargetColClick(wxCommandEvent& event);
        void OnDragParentColClick(wxCommandEvent& event);
        void OnDragAssistTypeSelect(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsSettingsDlg)
        wxComboBox* DragAssistType;
        wxButton* DragTargetCol;
        wxButton* DragParentCol;
        wxRadioButton* Icons16;
        wxRadioButton* Icons32;
        wxStaticText* StaticText6;
        wxRadioButton* TIcons16;
        wxRadioButton* TIcons32;
        wxPanel* Panel3;
        wxStaticText* StaticText7;
        wxSpinCtrl* spinProportion;
        wxStaticText* StaticText8;
        wxCheckBox* chkTop;
        wxCheckBox* chkBottom;
        wxCheckBox* chkLeft;
        wxCheckBox* chkRight;
        wxStaticText* StaticText9;
        wxCheckBox* chkExpand;
        wxStaticText* StaticText10;
        wxCheckBox* chkShaped;
        wxStaticText* StaticText12;
        wxChoice* choicePlacement;
        wxStaticText* StaticText13;
        wxSpinCtrl* spinBorder;
        wxCheckBox* chkAutoSelect;
        wxSpinCtrl* PrevFetchDelay;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
