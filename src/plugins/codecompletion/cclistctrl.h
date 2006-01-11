#ifndef CCLISTCTRL_H
#define CCLISTCTRL_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <cbeditor.h>
#include "parser/parser.h"
#include "ccrenderer.h"

class CCListCtrl : public wxGrid
{
	public:
		CCListCtrl(wxWindow* parent, int id, Parser* parser, const wxString& initial = wxEmptyString);
		virtual ~CCListCtrl();

		Token* GetTokenAt(unsigned int pos);
		Token* GetSelectedToken();
		void AddChar(const wxChar& ch);
		void RemoveLastChar();
	protected:
		void OnChar(wxKeyEvent& event);

		void PrepareTokens();
	private:
		wxWindow* m_pParent;
		Parser* m_pParser;
		cbStyledTextCtrl* m_pEditor;
		int m_Id;
		wxString m_Initial;
		TokenList m_CCTokens;
		CCTable* m_pGridTable;
		CCRenderer* m_pRenderer;

		DECLARE_EVENT_TABLE()
};

#endif // CCLISTCTRL_H
