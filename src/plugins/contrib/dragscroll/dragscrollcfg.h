#ifndef DragScrollCfg_H
#define DragScrollCfg_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "dragscroll.h"

//(*Headers(cbDragScrollCfg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
//*)
#include "cbplugin.h"
#include <wx/settings.h>

// ----------------------------------------------------------------------------
class cbDragScrollCfg: public cbConfigurationPanel
// ----------------------------------------------------------------------------
{

	public:
		cbDragScrollCfg(wxWindow* parent, cbDragScroll* pOwnerClass, wxWindowID id = -1);
		virtual ~cbDragScrollCfg();
    public:
        // virtual routines required by cbConfigurationPanel
        wxString GetTitle(){ return _("Mouse Drag Scrolling"); }
        //wxString GetBitmapBaseName(){ return _T("generic-plugin"); }
        //wxString GetBitmapBaseName(){ return _T("dragscroll"); }
        wxString GetBitmapBaseName();
        void OnApply();
        void OnCancel(){}

        // pointer to owner of the configuration diaglog needed to
        // complete the OnApply/OnCancel EndModal() logic
        cbDragScroll* pOwnerClass;


		//(*Identifiers(cbDragScrollCfg)
		enum Identifiers
		{
		    ID_DONEBUTTON = 0x1000,
		    ID_ENABLEDCHECKBOX,
		    ID_ENABLEDFOCUS,
		    ID_KEYCHOICE,
		    ID_RADIOBOX1,
		    ID_SENSITIVITY,
		    ID_MOUSETOLINERATIO,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4
		};
		//*)

        bool GetMouseDragScrollEnabled() { return ScrollEnabled->GetValue(); }
        bool GetMouseEditorFocusEnabled(){ return EditorFocusEnabled->GetValue(); }
        int  GetMouseDragDirection()     { return ScrollDirection->GetSelection(); }
        int  GetMouseDragKey()           { return MouseKeyChoice->GetSelection(); }
        int  GetMouseDragSensitivity()   { return Sensitivity->GetValue(); }
        int  GetMouseToLineRatio()       { return MouseToLineRatio->GetValue(); }


        void SetMouseDragScrollEnabled(bool value)
                { ScrollEnabled->SetValue(value); }
        void SetMouseEditorFocusEnabled(bool value)
                { EditorFocusEnabled->SetValue(value); }
        void SetMouseDragDirection(int selection)
                { ScrollDirection->SetSelection(selection); }
        void SetMouseDragKey(int selection)
                { MouseKeyChoice->SetSelection(selection); }
        void SetMouseDragSensitivity(int value)
                { Sensitivity->SetValue(value); }
        void SetMouseToLineRatio(int value)
                { MouseToLineRatio->SetValue(value); }

	protected:

		//(*Handlers(cbDragScrollCfg)
		void OnDoneButtonClick(wxCommandEvent& event);
		//*)

		//(*Declarations(cbDragScrollCfg)
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* StaticText1;
		wxCheckBox* ScrollEnabled;
		wxCheckBox* EditorFocusEnabled;
		wxRadioBox* ScrollDirection;
		wxStaticText* StaticText2;
		wxChoice* MouseKeyChoice;
		wxStaticText* StaticText3;
		wxSlider* Sensitivity;
        wxStaticText* StaticText4;
		wxSlider* MouseToLineRatio;
		wxButton* DoneButton;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // DragScrollCfg_H
