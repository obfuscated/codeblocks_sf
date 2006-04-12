#ifndef CCLIST_H
#define CCLIST_H

#include <wx/frame.h>
#include <wx/string.h>
#include <wx/grid.h>
#include <cbeditor.h>
#include <sdk_events.h>
#include "parser/parser.h"
#include "parser/token.h"
#include "cclistctrl.h"

class CCList : public wxFrame
{
	public:
		static CCList* Get(wxEvtHandler* parent, cbStyledTextCtrl* editor, Parser* parser);
		static void Free();
		void SelectCurrent(wxChar ch = _T('\0'));
		int GetStartPos() const { return m_StartPos; }
	protected:
		CCList(wxEvtHandler* parent, cbStyledTextCtrl* editor, Parser* parser);
		virtual ~CCList();

		void PositionMe();

		void OnActivate(wxActivateEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnLeftClick(wxGridEvent& event);
		void OnLeftDClick(wxGridEvent& event);
		void OnChar(wxKeyEvent& event);
		void OnCellChanged(wxGridEvent& event);

		wxEvtHandler* m_pParent;
		cbStyledTextCtrl* m_pEditor;
		Parser* m_pParser;
		CCListCtrl* m_pList;
		int m_StartPos;
		bool m_IsCtrlPressed;
	private:
		DECLARE_EVENT_TABLE()
};

extern const wxEventType csdEVT_CCLIST_CODECOMPLETE;
#define EVT_CCLIST_CODECOMPLETE(fn) DECLARE_EVENT_TABLE_ENTRY( csdEVT_CCLIST_CODECOMPLETE, -1, -1, (wxObjectEventFunction)(wxEventFunction)(wxNotifyEventFunction)&fn, (wxObject *) NULL ),

#endif // CCLIST_H

