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
#ifndef WXTHES_H
#define WXTHES_H

// some maximum sizes for buffers
//#define MAX_WD_LEN 200
//#define MAX_LN_LEN 16384


// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/string.h>
#include <map>
#include <vector>

class MyThes;
typedef std::map< wxString, std::vector< wxString > > synonyms;
class wxThes
{
    public:
        wxThes(const wxString idxpath, const wxString datpath);
        virtual ~wxThes();

        synonyms Lookup(const wxString &Text);


        //void CleanUpAfterLookup(mentry** pme, int nmean);
        //char* get_th_encoding();
        wxString GetEncoding();
    private:
        // Open index and dat files and load list array
        //bool Initialize(const wxString indxpath, const wxString datpath);

        // internal close and cleanup dat and idx files
        //bool Cleanup();

        // read a text line (\n terminated) stripping off line terminator
        //int readLine(FILE * pf, wxString &buf, int nc);

        MyThes *m_pMT;
    private:
        wxThes();
        wxThes(const wxThes &);
        wxThes & operator = (const wxThes &);
    protected:
    private:
};

#endif // WXTHES_H





