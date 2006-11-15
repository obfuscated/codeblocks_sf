/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "cbexception.h"
    #include "globals.h"
    #include "configmanager.h"
#endif

#include <wx/intl.h>

cbException::cbException(const wxString& msg, const wxString& file, int line)
  : Message(msg),
    File(file),
    Line(line)
{}

cbException::~cbException()
{}


void cbException::ShowErrorMessage(bool safe)
{
    wxString gccvers;
#ifdef __GNUC__
    gccvers.Printf(_T("gcc %d.%d.%d"), __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    wxString title = _("Exception");
    wxString err;
    err.Printf(_("An exception has been raised!\n\n"
                 "The application encountered an error at %s, on line %d.\n"
                 "The error message is:\n\n%s\n\n"
                 "Code::Blocks Version revision %d (%s, "
                 "build: %s %s)"),
               File.c_str(), Line, Message.c_str(),
               ConfigManager::GetRevisionNumber(), gccvers.c_str(),
               wxT(__DATE__), wxT(__TIME__));
    if (safe)
        wxSafeShowMessage(title, err);
    else
        cbMessageBox(err, title, wxICON_ERROR);
}
