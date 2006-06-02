#ifndef ASSTREAMITERATOR_H
#define ASSTREAMITERATOR_H

#include <iostream>
#include <wx/string.h>
#include "./astyle/astyle.h"

class ASStreamIterator : public astyle::ASSourceIterator
{
	public:
		ASStreamIterator(const wxChar *in, const wxString &eolChars);
		virtual ~ASStreamIterator();

        bool hasMoreLines() const;
        std::string nextLine();
	protected:
        bool IsEOL(wxChar ch);
        const wxChar *m_In;
        const wxString m_EOL;
        wxChar buffer[2048];
	private:
};

#endif // ASSTREAMITERATOR_H
