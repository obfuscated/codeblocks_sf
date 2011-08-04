/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef SPELLCHECKHELPER_H
#define SPELLCHECKHELPER_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <map>
#include <set>


// The spell check helper is used to detect if a word has to get spell-checked
//
// The language is the langauge of the source file (C/C++, VHDL, Verilog, LaTeX ... not
// the natural language). It is possible to configure which style (mostly comments) has to get checked.

class SpellCheckHelper
{
    public:
        SpellCheckHelper();
        virtual ~SpellCheckHelper();

        static bool IsWhiteSpace(const wxChar &ch);
        bool HasStyleToBeChecked(wxString langname, int style)const;
        void LoadConfiguration();

    protected:
    private:
        std::map<wxString, std::set<long> > m_LanguageIndices;
};

#endif // SPELLCHECKHELPER_H
