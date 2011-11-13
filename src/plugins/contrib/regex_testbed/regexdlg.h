#ifndef REGEXDLG_H
#define REGEXDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <set>

//(*Headers(regex_dialog)
#include <wx/checkbox.h>
#include "scrollingdialog.h"
#include <wx/choice.h>
#include <wx/html/htmlwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/html/htmlwin.h>

#ifndef CB_PRECOMP
  #include <wx/listctrl.h> // wxListEvent
  #include <wx/choice.h>
  #include <wx/checkbox.h>
  #include <wx/regex.h>
#endif

class RegExDlg: public wxScrollingDialog
{
	public:

		RegExDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~RegExDlg();

		//(*Identifiers(regex_dialog)
		//*)

		void EndModal(int retCode);

		static void ReleaseAll();

	protected:

		//(*Handlers(regex_dialog)
		void RunBenchmark(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		void OnRegExItemActivated(wxListEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
		void OnClose(wxCloseEvent& event);
		//*)

	private:


        wxArrayString GetBuiltinMatches(const wxString& text);
        wxArrayString GetPregMatches(const wxString& text);

        //(*Declarations(regex_dialog)
        wxCheckBox* m_nocase;
        wxHtmlWindow* m_output;
        wxTextCtrl* m_text;
        wxTextCtrl* m_quoted;
        wxTextCtrl* m_regex;
        wxCheckBox* m_newlines;
        wxChoice* m_library;
        //*)

        wxRegEx m_wxre;

        typedef std::set<RegExDlg*> VisibleDialogs;
        static VisibleDialogs m_visible_dialogs;

		DECLARE_EVENT_TABLE()
};

#endif
