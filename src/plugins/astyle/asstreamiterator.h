#ifndef ASSTREAMITERATOR_H
#define ASSTREAMITERATOR_H

#include <iostream>
#include <wx/string.h>
#include <astyle.h>

class ASStreamIterator : public astyle::ASSourceIterator
{
	public:
		ASStreamIterator(const wxChar* in);
		virtual ~ASStreamIterator();

        bool hasMoreLines() const;
        std::string nextLine();
	protected:
        const wxChar* m_In;
        char buffer[2048];
	private:
};

#endif // ASSTREAMITERATOR_H

