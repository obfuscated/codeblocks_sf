/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASSTREAMITERATOR_H
#define ASSTREAMITERATOR_H

#include <iostream>
#include <wx/string.h>
#include <cbeditor.h>
#include <vector>
#include "astyle/astyle.h"

class ASStreamIterator : public astyle::ASSourceIterator
{
    public:
        ASStreamIterator(cbEditor *cbe, const wxChar *in);
        virtual ~ASStreamIterator();

        bool hasMoreLines() const;
        std::string nextLine(bool emptyLineWasDeleted = false);
        std::string peekNextLine();
        void peekReset();
        bool FoundBookmark() const { return m_foundBookmark; }
        void ClearFoundBookmark() { m_foundBookmark = false; }
        bool FoundBreakpoint() const { return m_foundBreakpoint; }
        void ClearFoundBreakpoint() { m_foundBreakpoint = false; }

    protected:
        bool IsEOL(wxChar ch);
        cbEditor *m_cbe;
        const wxChar *m_In;
        const wxChar *m_PeekStart;
        std::vector<wxChar> m_buffer;
        int m_curline;
        bool m_foundBookmark;
        bool m_foundBreakpoint;

    private:
        std::string readLine();
};

#endif // ASSTREAMITERATOR_H
