/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSCODINGLANG_H
#define WXSCODINGLANG_H

#include <wx/string.h>

/** \brief This enumeration contains all coding languages supported in wxSmith */
enum wxsCodingLang
{
    wxsCPP             = 0x0001,
    wxsUnknownLanguage = 0x8000
};

/** \brief Namespace with useful common functions for supported coding languages */
namespace wxsCodeMarks
{
    /** \brief Getting wxString name of  coding language */
    wxString Name(wxsCodingLang Id);

    /** \brief Getting id of coding language's name */
    wxsCodingLang Id(const wxString& Name);

    /** \brief Getting coding language from file's extension */
    wxsCodingLang IdFromExt(const wxString& Extension);

    /** \brief Helper function for generating block begin */
    wxString Beg(wxsCodingLang Lang,const wxString& BlockName);

    /** \brief Helper function for generating block begin */
    wxString Beg(wxsCodingLang Lang,const wxString& BlockName,const wxString& Param);

    /** \brief Helper function for generating block end */
    wxString End(wxsCodingLang Lang);

    /** \brief Converting given string into representation of string in source code */
    wxString String(wxsCodingLang Lang,const wxString& Source);

    /** \brief Converting given string into Wx-like representation */
    wxString WxString(wxsCodingLang Lang,const wxString& Source,bool WithTranslation);

    /** \brief Checking if given string can be valid identifier in selected language
     *         (can be name of variable)
     */
    bool ValidateIdentifier(wxsCodingLang Lang,const wxString& Identifier);

    /** \brief Posting notification message about unknown coding language */
    void Unknown(const wxString& Function,wxsCodingLang Lang);
}

#endif
