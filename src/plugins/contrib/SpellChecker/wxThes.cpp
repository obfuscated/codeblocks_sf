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
#include "wxThes.h"

#include "mythes.hxx"


wxThes::~wxThes()
{
    //dtor
    if ( m_pMT )
        delete m_pMT;
    m_pMT = NULL;
}
wxThes::wxThes(const wxString idxpath, const wxString datpath):
    m_pMT(NULL)
{
    m_pMT = new MyThes( idxpath.char_str(), datpath.char_str() );
}

synonyms wxThes::Lookup(const wxString &Text)
{
    synonyms syn;
    mentry * pmean;
    int count = m_pMT->Lookup(Text.char_str(), Text.length(),&pmean);
    // don't change value of pmean
    // or count since needed for CleanUpAfterLookup routine
    mentry* pm = pmean;
    if (count)
    {
        //fprintf(stdout,"%s has %d meanings\n",buf,count);
        std::vector< wxString > s;
        for (int  i=0; i < count; i++)
        {
            //fprintf(stdout,"   meaning %d: %s\n",i,pm->defn);
            for (int j=0; j < pm->count; j++)
            {
                //fprintf(stdout,"       %s\n",pm->psyns[j]);
                s.push_back(wxString(pm->psyns[j], wxConvUTF8));
            }
            //fprintf(stdout,"\n");
            syn[wxString( pm->defn, wxConvUTF8 )] = s;

            pm++;
        }
        //fprintf(stdout,"\n\n");
        // now clean up all allocated memory
        m_pMT->CleanUpAfterLookup(&pmean,count);
    }
//    else
//    {
//        fprintf(stdout,"\"%s\" is not in thesaurus!\n",buf);
//    }
    return syn;
}

wxString wxThes::GetEncoding()
{
    return wxString( m_pMT->get_th_encoding(), wxConvUTF8);
}







