/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/string.h>
    #include "configmanager.h"
    #include "manager.h"
    #include "messagemanager.h"
#endif
#include "messagelog.h" // class's header file

// class constructor
MessageLog::MessageLog()
    : wxPanel(Manager::Get()->GetAppWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    m_PageId = -1;
}

// class destructor
MessageLog::~MessageLog()
{
}

wxFont MessageLog::GetDefaultLogFont(bool fixedPitchFont) const
{
#ifdef __WXMAC__
    // 8 point is not readable on Mac OS X, increase font size:
    int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), 10);
#else
    int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), 8);
#endif
    wxFont font(size,
                fixedPitchFont ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT,
                wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_NORMAL);
    return font;
}
