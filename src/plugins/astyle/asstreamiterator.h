#ifndef ASSTREAMITERATOR_H
#define ASSTREAMITERATOR_H

#include <iostream>
#include <wx/string.h>
#include <cbeditor.h>
#include "./astyle/astyle.h"

class ASStreamIterator : public astyle::ASSourceIterator
{
	public:
		ASStreamIterator(cbEditor *cbe, const wxChar *in, const wxString &eolChars);
		virtual ~ASStreamIterator();

    bool hasMoreLines() const;
    std::string nextLine();
    bool FoundBookmark() const { return m_foundBookmark; }
    void ClearFoundBookmark() { m_foundBookmark = false; }

	protected:
        bool IsEOL(wxChar ch);
        cbEditor *m_cbe;
        const wxChar *m_In;
        const wxString m_EOL;
        wxChar buffer[2048];
        int m_curline;
        bool m_foundBookmark;
	private:
};

#endif // ASSTREAMITERATOR_H
