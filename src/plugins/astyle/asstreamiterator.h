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
#include "./astyle/astyle.h"

class ASStreamIterator : public astyle::ASSourceIterator
{
	public:
		ASStreamIterator(cbEditor *cbe, const wxChar *in);
		virtual ~ASStreamIterator();

    bool hasMoreLines() const;
    std::string nextLine();
    bool FoundBookmark() const { return m_foundBookmark; }
    void ClearFoundBookmark() { m_foundBookmark = false; }

	protected:
        bool IsEOL(wxChar ch);
        cbEditor *m_cbe;
        const wxChar *m_In;
        std::vector<wxChar> m_buffer;
        int m_curline;
        bool m_foundBookmark;
	private:
};

#endif // ASSTREAMITERATOR_H
