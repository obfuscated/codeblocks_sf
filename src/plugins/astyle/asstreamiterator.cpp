#include "asstreamiterator.h"

ASStreamIterator::ASStreamIterator(const wxChar* in)
    : m_In(in)
{
	//ctor
}

ASStreamIterator::~ASStreamIterator()
{
	//dtor
}

bool ASStreamIterator::hasMoreLines() const
{
    return (*m_In) != 0;
}

std::string ASStreamIterator::nextLine()
{
    wxChar* filterPtr;

    memset(buffer, 0, sizeof(buffer));
    filterPtr = buffer;

    while (*m_In != 0 && *m_In != '\r' && *m_In != '\n')
    {
        *filterPtr++ = *m_In++;
    }
    *filterPtr = 0;

    while (*m_In == '\r' || *m_In == '\n')
        ++m_In;

    return std::string(buffer);
}
