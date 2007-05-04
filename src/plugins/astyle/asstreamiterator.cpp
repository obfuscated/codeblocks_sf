#include "asstreamiterator.h"
#include "globals.h"

ASStreamIterator::ASStreamIterator(cbEditor *cbe, const wxChar* in)
: m_cbe(cbe), m_In(in), m_curline(0), m_foundBookmark(false)
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

inline bool ASStreamIterator::IsEOL(wxChar ch)
{
  if (ch == _T('\r') || ch == _T('\n'))
  {
      return true;
  }

  return false;
}

std::string ASStreamIterator::nextLine()
{
  // hack: m_curline = 0 is a special case we should not evaluate here
  if (m_cbe && m_curline && m_cbe->HasBookmark(m_curline))
  {
    m_foundBookmark = true;
  }

  wxChar *filterPtr;

  memset(buffer, 0, sizeof(buffer));
  filterPtr = buffer;

  while (*m_In != 0)
  {
    if (!IsEOL(*m_In))
    {
      *filterPtr++ = *m_In;
    }

    ++m_In;

    if (IsEOL(*m_In))
    {
      // if CRLF (two chars) peek next char (avoid duplicating empty-lines)
      if (*m_In != *(m_In + 1) && IsEOL(*(m_In + 1)))
      {
        ++m_In;
      }

      break;
    }
  }

  *filterPtr = 0;
  ++m_curline;

  return std::string(cbU2C(buffer));
}
