#ifndef SELECTSTOREDEXPRESSIONDLG_H
#define SELECTSTOREDEXPRESSIONDLG_H

#include <map>

//(*Headers(SelectStoredExpressionDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/timer.h>
//*)

class SelectStoredExpressionDlg: public wxDialog
{
	public:

		SelectStoredExpressionDlg( wxWindow* parent, const wxString& startingExpresion = wxEmptyString );
		virtual ~SelectStoredExpressionDlg();

		wxString GetExpression() { return m_Expression; }

	private:

        wxString m_Expression;

		//(*Declarations(SelectStoredExpressionDlg)
		wxButton* Button4;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxButton* Button2;
		wxListBox* m_Expressions;
		wxButton* Button3;
		wxTextCtrl* m_Filter;
		wxTimer Timer1;
		//*)

		//(*Identifiers(SelectStoredExpressionDlg)
		static const long ID_LISTBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_TIMER1;
		//*)

		//(*Handlers(SelectStoredExpressionDlg)
		void OnOkClick(wxCommandEvent& event);
		void OnCancelClick(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void Onm_ExpressionsSelect(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void Onm_FilterText(wxCommandEvent& event);
		void Onm_FilterTextEnter(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void Onm_ExpressionsDClick(wxCommandEvent& event);
		//*)

		void BuildContent(wxWindow* parent);
		void ReadExpressions();
		void StoreExpressions();
		void RecreateExpressionsList( const wxString& selectionHint = wxEmptyString );

        typedef std::map< wxString, wxString > CacheT;
        CacheT m_Cache;
        bool m_CacheChanged;

        /** \brief Helper class to identify items on expression list */
        class ListData: public wxClientData
        {
            public:

                /** \brief Ctor */
                inline ListData( const CacheT::iterator& i ): m_Iterator( i )
                {}

                /** \brief Fetch iterator */
                inline const CacheT::iterator& GetIterator()
                {
                    return m_Iterator;
                }

                /** \brief Fetch key */
                inline const wxString& GetKey()
                {
                    return m_Iterator->first;
                }

                /** \brief Fetch value */
                inline const wxString& GetValue()
                {
                    return m_Iterator->second;
                }

            private:

                CacheT::iterator m_Iterator;        ///< \brief Item's iterator
        };

        ListData* GetSelection();

        void AddingExpression( const wxString& defaultName, const wxString& defaultValue );

        void FilterUpdated();

        void StoreExpressionsQuery();

        static wxString GetListName( const wxString& name, const wxString& expr );


		DECLARE_EVENT_TABLE()
};

#endif
