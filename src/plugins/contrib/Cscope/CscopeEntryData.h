#ifndef CSCOPE_ENTRY_DATA
#define CSCOPE_ENTRY_DATA

#include "wx/string.h"
#include <map>
#include <vector>

enum {
	KindFileNode = 0,
	KindSingleEntry
};

class CscopeEntryData
{
	wxString m_file;
	int m_line;
	wxString m_pattern;
	wxString m_scope;
	int m_kind;

public:
	CscopeEntryData();
	~CscopeEntryData();


//Setters
	void SetFile(const wxString& file) {
		m_file = file;
	}
	void SetKind(const int& kind) {
		m_kind = kind;
	}
	void SetLine(const int& line) {
		m_line = line;
	}
	void SetPattern(const wxString& pattern) {
		m_pattern = pattern;
	}
	void SetScope(const wxString& scope) {
		m_scope = scope;
	}
//Getters
	const wxString& GetFile() const {
		return m_file;
	}
	const int& GetKind() const {
		return m_kind;
	}
	const int& GetLine() const {
		return m_line;
	}
	const wxString& GetPattern() const {
		return m_pattern;
	}
	const wxString& GetScope() const {
		return m_scope;
	}

};

typedef std::vector< CscopeEntryData > CscopeResultTable;

#endif // CSCOPE_ENTRY_DATA
