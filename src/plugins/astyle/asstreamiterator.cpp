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

    while (*m_In != 0)
    {
        if (*m_In != '\r' && *m_In != '\n')
            *filterPtr++ = *m_In;
        ++m_In;
        if (*m_In == '\r' || *m_In == '\n')
        {
            // peek next char (avoid duplicating empty-lines)
            if (*(m_In + 1) == '\r' || *(m_In + 1) == '\n')
                ++m_In;
            break;
        }
    }
    *filterPtr = 0;

    return std::string(buffer);
}
