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
    m_pDialogsParent(dialogsparent)
{
    //ctor
}

Thesaurus::Thesaurus(wxWindow *dialogsparent,const wxString idxpath, const wxString datpath):
    m_pT(NULL),
    m_pDialogsParent(dialogsparent)
{
    //ctor
    SetFiles(idxpath, datpath);
}

Thesaurus::~Thesaurus()
{
    //dtor
    delete m_pT;
    m_pT = NULL;
}

void Thesaurus::SetFiles(wxString idxpath, const wxString datpath)
{
    delete m_pT;
    m_pT = NULL;

    if ( wxFile::Exists(idxpath) && wxFile::Exists(datpath) )
    {
        m_pT = new wxThes( idxpath, datpath );
    }
    else
    {
        Manager::Get()->GetLogManager()->Log(_T("SpellChecker: Thesaurus files '") + idxpath + _T("' not found!"));
        if (!wxDirExists(idxpath.BeforeLast(wxFILE_SEP_PATH)) || !wxDirExists(datpath.BeforeLast(wxFILE_SEP_PATH)))
            return; // path does not exist, silence invalid directory warnings
        wxString altIdx = wxFindFirstFile(idxpath.BeforeLast(wxT('.')) + wxT("*.idx"), wxFILE); // "*_v2.idx"
        if (altIdx.IsEmpty()) // try again with more wildcards
        {
            altIdx = idxpath.AfterLast(wxFILE_SEP_PATH).BeforeLast(wxT('.')) + wxT("*.idx");
            altIdx.Replace(wxT("_"), wxT("*"));
            altIdx.Replace(wxT("-"), wxT("*"));
            altIdx = wxFindFirstFile(idxpath.BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH + altIdx, wxFILE);
        }
        if (altIdx.IsEmpty()) // try to find the thesaurus of a related language (something is better than nothing)
        {
            altIdx = idxpath.AfterLast(wxFILE_SEP_PATH);
            altIdx.Replace(wxT("_"), wxT("*"));
            altIdx.Replace(wxT("-"), wxT("*"));
            altIdx = altIdx.BeforeLast(wxT('*')) + wxT("*.idx");
            altIdx = wxFindFirstFile(idxpath.BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH + altIdx, wxFILE);
        }

        wxString altDat = wxFindFirstFile(datpath.BeforeLast(wxT('.')) + wxT("*.dat"), wxFILE); // "*_v2.dat"
        if (altDat.IsEmpty()) // try again with more wildcards
        {
            altDat = datpath.AfterLast(wxFILE_SEP_PATH).BeforeLast(wxT('.')) + wxT("*.dat");
            altDat.Replace(wxT("_"), wxT("*"));
            altDat.Replace(wxT("-"), wxT("*"));
            altDat = wxFindFirstFile(datpath.BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH + altDat, wxFILE);
        }
        if (altDat.IsEmpty()) // try to find the thesaurus of a related language (something is better than nothing)
        {
            altDat = datpath.AfterLast(wxFILE_SEP_PATH);
            altDat.Replace(wxT("_"), wxT("*"));
            altDat.Replace(wxT("-"), wxT("*"));
            altDat = altDat.BeforeLast(wxT('*')) + wxT("*.dat");
            altDat = wxFindFirstFile(datpath.BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH + altDat, wxFILE);
        }

        if (!altIdx.IsEmpty() && !altDat.IsEmpty() && wxFileExists(altIdx) && wxFileExists(altDat))
        {
            m_pT = new  wxThes(altIdx, altDat);
            Manager::Get()->GetLogManager()->Log(wxT("SpellChecker: Loading '") + altIdx + wxT("' instead..."));
        }
    }
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

synonyms Thesaurus::GetSynonyms(const wxString& Word)
{
    synonyms syn;
    if (m_pT)
        syn = m_pT->Lookup(Word);
    return syn;
}

bool Thesaurus::IsOk()
{
    return (m_pT != NULL);
}
