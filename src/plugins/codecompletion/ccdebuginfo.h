#ifndef CCDEBUGINFO_H
#define CCDEBUGINFO_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(CCDebugInfo)
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class Parser;
class Token;

class CCDebugInfo: public wxDialog
{
	public:

		CCDebugInfo(wxWindow* parent, Parser* parser, Token* token);
		virtual ~CCDebugInfo();

		Parser* m_pParser;
		Token* m_pToken;

		void FillFiles();
		void DisplayTokenInfo();
		void FillChildren();
		void FillAncestors();
		void FillDescendants();

		//(*Identifiers(CCDebugInfo)
		enum Identifiers
		{
		    ID_NOTEBOOK1 = 0x1000,
		    ID_PANEL1,
		    ID_STATICTEXT29,
		    ID_TEXTCTRL1,
		    ID_BUTTON1,
		    ID_STATICLINE1,
		    ID_STATICTEXT17,
		    ID_STATICTEXT18,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_STATICTEXT9,
		    ID_STATICTEXT10,
		    ID_STATICTEXT11,
		    ID_STATICTEXT12,
		    ID_STATICTEXT3,
		    ID_STATICTEXT4,
		    ID_STATICTEXT5,
		    ID_STATICTEXT6,
		    ID_STATICTEXT7,
		    ID_STATICTEXT8,
		    ID_STATICTEXT13,
		    ID_STATICTEXT14,
		    ID_STATICTEXT15,
		    ID_STATICTEXT16,
		    ID_STATICTEXT32,
		    ID_STATICTEXT33,
		    ID_STATICTEXT19,
		    ID_STATICTEXT20,
		    ID_STATICTEXT22,
		    ID_STATICTEXT24,
		    ID_BUTTON4,
		    ID_STATICTEXT30,
		    ID_COMBOBOX3,
		    ID_BUTTON5,
		    ID_STATICTEXT21,
		    ID_COMBOBOX2,
		    ID_BUTTON3,
		    ID_STATICTEXT23,
		    ID_COMBOBOX1,
		    ID_BUTTON2,
		    ID_STATICTEXT25,
		    ID_STATICTEXT26,
		    ID_STATICTEXT27,
		    ID_STATICTEXT28,
		    ID_PANEL2,
		    ID_LISTBOX1,
		    ID_STATICTEXT31,
		    ID_STATICLINE2
		};
		//*)

	protected:

		//(*Handlers(CCDebugInfo)
		void OnInit(wxInitDialogEvent& event);
		void OnFindClick(wxCommandEvent& event);
		void OnGoAscClick(wxCommandEvent& event);
		void OnGoDescClick(wxCommandEvent& event);
		void OnGoParentClick(wxCommandEvent& event);
		void OnGoChildrenClick(wxCommandEvent& event);
		//*)

		//(*Declarations(CCDebugInfo)
		wxNotebook* Notebook1;
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer4;
		wxStaticText* StaticText29;
		wxTextCtrl* txtFilter;
		wxButton* btnFind;
		wxStaticLine* StaticLine1;
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* StaticText17;
		wxStaticText* txtID;
		wxStaticText* StaticText1;
		wxStaticText* txtName;
		wxStaticText* StaticText9;
		wxStaticText* txtKind;
		wxStaticText* StaticText11;
		wxStaticText* txtScope;
		wxStaticText* StaticText3;
		wxStaticText* txtType;
		wxStaticText* StaticText5;
		wxStaticText* txtActualType;
		wxStaticText* StaticText7;
		wxStaticText* txtArgs;
		wxStaticText* StaticText13;
		wxStaticText* txtIsOp;
		wxStaticText* StaticText15;
		wxStaticText* txtIsLocal;
		wxStaticText* StaticText32;
		wxStaticText* txtIsTemp;
		wxStaticText* StaticText19;
		wxStaticText* txtNamespace;
		wxStaticText* StaticText22;
		wxBoxSizer* BoxSizer7;
		wxStaticText* txtParent;
		wxButton* btnGoParent;
		wxStaticText* StaticText30;
		wxBoxSizer* BoxSizer8;
		wxComboBox* cmbChildren;
		wxButton* btnGoChildren;
		wxStaticText* StaticText21;
		wxBoxSizer* BoxSizer6;
		wxComboBox* cmbAncestors;
		wxButton* btnGoAsc;
		wxStaticText* StaticText23;
		wxBoxSizer* BoxSizer5;
		wxComboBox* cmbDescendants;
		wxButton* btnGoDesc;
		wxStaticText* StaticText25;
		wxStaticText* txtDeclFile;
		wxStaticText* StaticText27;
		wxStaticText* txtImplFile;
		wxPanel* Panel2;
		wxBoxSizer* BoxSizer3;
		wxListBox* lstFiles;
		wxStaticText* lblInfo;
		wxStaticLine* StaticLine2;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
