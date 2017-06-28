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
#ifndef THESAURUS_H
#define THESAURUS_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>
typedef std::map<wxString, std::vector< wxString > > synonyms;

class wxThes;
class Thesaurus
{
public:
    Thesaurus(wxWindow *dialogsparent,const wxString idxpath, const wxString datpath);
    Thesaurus(wxWindow *dialogsparent);
    virtual ~Thesaurus();

    bool GetSynonym(const wxString Word, wxString &Syn);
    synonyms GetSynonyms(const wxString& Word);
    void SetFiles(wxString idxpath, const wxString datpath);
    bool IsOk();

protected:
private:
    wxThes *m_pT;
    wxWindow *m_pDialogsParent;
};

#endif // THESAURUS_H
