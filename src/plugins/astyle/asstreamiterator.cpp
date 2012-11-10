/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "asstreamiterator.h"
#include "globals.h"

ASStreamIterator::ASStreamIterator(cbEditor *cbe, const wxChar* in)
: m_cbe(cbe), m_In(in), m_PeekStart(0), m_curline(0), m_foundBookmark(false),
m_foundBreakpoint(false)
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

std::string ASStreamIterator::nextLine(cb_unused bool emptyLineWasDeleted)
{
    // hack: m_curline = 0 is a special case we should not evaluate here
    if (m_cbe && m_curline && m_cbe->HasBookmark(m_curline))
    {
        m_foundBookmark = true;
    }
    if (m_cbe && m_curline && m_cbe->HasBreakpoint(m_curline))
    {
        m_foundBreakpoint = true;
    }

    return readLine();
}

std::string ASStreamIterator::readLine()
{
    m_buffer.clear();

    while (*m_In != 0)
    {
        if (!IsEOL(*m_In))
        {
            m_buffer.push_back(*m_In);
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

    m_buffer.push_back(0);
    ++m_curline;

    return std::string(cbU2C(&m_buffer[0]));
}

std::string ASStreamIterator::peekNextLine()
{
    if (!m_PeekStart)
    {
        m_PeekStart = m_In;
    }

    return readLine();
}

void ASStreamIterator::peekReset()
{
    m_In = m_PeekStart;
    m_PeekStart = 0;
}
