#ifndef CCRENDERER_H
#define CCRENDERER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include "parser/token.h"

class CCTable : public wxGridTableBase
{
	public:
		CCTable(TokensArray* tokens) : m_Tokens(tokens) {}
		void SetTokens(TokensArray* tokens) { m_Tokens = tokens; }
	
		int GetNumberRows() { return m_Tokens->GetCount(); }
		int GetNumberCols() { return 1; }
		wxString GetValue( int row, int col )
		{
			return m_Tokens->Item(row)->m_Name;
		}
	
		void SetValue( int, int, const wxString&) {}
		bool IsEmptyCell( int , int  ) { return false; }

	private:
		TokensArray* m_Tokens;
};

class CCRenderer : public wxGridCellStringRenderer
{
	public:
		CCRenderer(TokensArray* tokens) : m_Tokens(tokens) {}
		void SetTokens(TokensArray* tokens) { m_Tokens = tokens; }
		virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
		virtual wxGridCellRenderer* Clone() const {	return new CCRenderer(m_Tokens); }
	private:
		void DoDrawText(wxGrid& grid, wxDC& dc, wxRect& rect, const wxString& text, const wxColour& fore, int weight, int useThisOffset = -1);
		TokensArray* m_Tokens;
		wxFont m_Font;
};

#endif // CCRENDERER_H
