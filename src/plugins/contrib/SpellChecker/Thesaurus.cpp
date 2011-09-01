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
#include "Thesaurus.h"
#include "ThesaurusDialog.h"

#include "wxThes.h"

#include <logmanager.h>
#include <wx/file.h>


Thesaurus::Thesaurus(wxWindow *dialogsparent):
    m_pT(NULL),
    m_pDialogsParent(dialogsparent),
    m_ok(false)
{
    //ctor
}
Thesaurus::Thesaurus(wxWindow *dialogsparent,const wxString idxpath, const wxString datpath):
    m_pT(NULL),
    m_pDialogsParent(dialogsparent),
    m_ok(false)
{
    //ctor
    SetFiles(idxpath, datpath);
}

Thesaurus::~Thesaurus()
{
    //dtor
    if ( m_pT )
        delete m_pT;

    m_pT = NULL;
}

void Thesaurus::SetFiles(wxString idxpath, const wxString datpath)
{
    if ( m_pT )
        delete m_pT;
    m_pT = NULL;
    m_ok = false;

    if ( wxFile::Exists(idxpath) && wxFile::Exists(datpath) )
    {
        m_pT = new wxThes( idxpath, datpath );
        m_ok = true;
    }
    else
        Manager::Get()->GetLogManager()->Log(_T("SpellChecker: Thesaurus files '") + idxpath + _T("' not found!"));
}


bool Thesaurus::GetSynonym(const wxString Word, wxString &Syn)
{
    if ( m_pT )
    {
        synonyms syn = m_pT->Lookup(Word);
        if ( syn.size() )
        {
            Syn = wxEmptyString;
            ThesaurusDialog dlg(m_pDialogsParent, Word, syn);
            if ( dlg.ShowModal() == wxID_OK )
            {
                Syn = dlg.GetSelection();
            }
            return true;
        }
    }
    return false;

}


bool Thesaurus::IsOk()
{
    return m_ok;
}
