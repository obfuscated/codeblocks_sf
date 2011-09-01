/***************************************************************
 * Name:      CodeSnippetsEvent
 *
 * Purpose:   This class implements the events sent by/for a
 *            CodeSnippets request to the
 *            CodeSnippets Manager to request services such as
 *            Tree item focus and/or editing services.
 *            wxCommandEvent m_id contains a code snippets id.
 *
 * Author:    Pecan
 * Created:   2008/4/01
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/
#ifndef CODESNIPPETS_EVENT_H
#define CODESNIPPETS_EVENT_H

#include <wx/event.h>

// ----------------------------------------------------------------------------
class CodeSnippetsEvent : public wxCommandEvent
// ----------------------------------------------------------------------------
{
public:
	/** Constructor. */
	CodeSnippetsEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

	/** Copy constructor. */
	CodeSnippetsEvent( const CodeSnippetsEvent& event);

	/** Destructor. */
	~CodeSnippetsEvent();

	virtual wxEvent* Clone() const { return new CodeSnippetsEvent(*this);}

	DECLARE_DYNAMIC_CLASS(CodeSnippetsEvent);

	int      GetSnippetID() const {return m_SnippetID;}
	void     SetSnippetID( const int itemid ) {m_SnippetID = itemid;}
	wxString GetSnippetString() const {return m_SnippetString;}
	void     SetSnippetString( const wxString string ) {m_SnippetString = string;}
	wxString GetEventTypeLabel() const {return m_EventTypeLabel;}
    bool     PostCodeSnippetsEvent(const CodeSnippetsEvent& event);
    bool     ProcessCodeSnippetsEvent(const CodeSnippetsEvent& event);

private:
	int       m_SnippetID;
	wxString  m_SnippetString;
	wxString  m_EventTypeLabel;
};

typedef void (wxEvtHandler::*CodeSnippetsEventFunction)(CodeSnippetsEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_CODESNIPPETS_SELECT, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_CODESNIPPETS_EDIT, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_CODESNIPPETS_NEW_INDEX, wxID_ANY)
	DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_CODESNIPPETS_GETFILELINKS, wxID_ANY)
END_DECLARE_EVENT_TYPES()

#define EVT_CODESNIPPETS_SELECT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_CODESNIPPETS_SELECT, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (CodeSnippetsEventFunction) & fn,(wxObject *) NULL ),

#define EVT_CODESNIPPETS_EDIT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_CODESNIPPETS_EDIT, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (CodeSnippetsEventFunction) & fn,(wxObject *) NULL ),

#define EVT_CODESNIPPETS_NEW_INDEX(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_CODESNIPPETS_NEW_INDEX, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (CodeSnippetsEventFunction) & fn,(wxObject *) NULL ),

#define EVT_CODESNIPPETS_GETFILELINKS(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_CODESNIPPETS_GETFILELINKS, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) (CodeSnippetsEventFunction) & fn,(wxObject *) NULL ),

#endif // CODESNIPPETS_EVENT_H


