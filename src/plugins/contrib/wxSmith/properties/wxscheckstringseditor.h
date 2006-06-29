#ifndef WXSCHECKSTRINGSEDITOR_H
#define WXSCHECKSTRINGSEDITOR_H

#include "wxsstringlistcheckproperty.h"

//(*Headers(wxsCheckStringsEditor)
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
//*)

class wxsCheckStringsEditor: public wxDialog
{
	public:

		wxsCheckStringsEditor(
            wxWindow* parent,
            const wxArrayString& Strings,
            const wxsArrayBool& Bools,
            bool Sorted,
            wxWindowID id = -1);
            
		virtual ~wxsCheckStringsEditor();

		//(*Identifiers(wxsCheckStringsEditor)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON2,
		    ID_BUTTON3,
		    ID_BUTTON4,
		    ID_BUTTON5,
		    ID_CHECKLISTBOX1,
		    ID_STATICLINE1,
		    ID_STATICLINE2,
		    ID_TEXTCTRL1
		};
		//*)
		
		wxArrayString Strings;
		wxsArrayBool Bools;
		bool Sorted;

	protected:

		//(*Handlers(wxsCheckStringsEditor)
		void OnButton7Click(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton5Click(wxCommandEvent& event);
		void OnListClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsCheckStringsEditor)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* EditArea;
		wxButton* Button1;
		wxStaticLine* StaticLine1;
		wxBoxSizer* BoxSizer3;
		wxCheckListBox* StringList;
		wxBoxSizer* BoxSizer4;
		wxButton* Button2;
		wxButton* Button4;
		wxButton* Button3;
		wxButton* Button5;
		wxStaticLine* StaticLine2;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

	private:
	
        struct SortItem
        {
            wxString String;
            int InitialIndex;
            SortItem(wxString S,int I): String(S), InitialIndex(I) {}
        };
        
        static int SortCmpFunc(SortItem** First,SortItem** Second)
        {
            if ( (*First)->String < (*Second)->String ) return -1;
            if ( (*First)->String > (*Second)->String ) return 1;
            return 0;
        }
        
        WX_DEFINE_ARRAY(SortItem*,SortArray);
	
        void InitialRemapBools();
        void FinalRemapBools();

		DECLARE_EVENT_TABLE()
};

#endif
