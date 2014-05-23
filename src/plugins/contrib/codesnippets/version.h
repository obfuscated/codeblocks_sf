// ----------------------------------------------------------------------------
/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// ----------------------------------------------------------------------------
// RCS-ID: $Id$

#ifndef VERSION_H
#define VERSION_H
// ---------------------------------------------------------------------------
// Logging / debugging
// ---------------------------------------------------------------------------
//debugging control
#include <wx/log.h>

#define LOGIT wxLogDebug
#if defined(LOGGING)
 #define LOGGING 1
 #undef LOGIT
 #define LOGIT wxLogMessage
 #define TRAP asm("int3")
#endif

// ----------------------------------------------------------------------------
   #if LOGGING
	extern wxLogWindow*    m_pLog;
   #endif
// ----------------------------------------------------------------------------
class AppVersion
// ----------------------------------------------------------------------------
{
    public:
        AppVersion();
       ~AppVersion();

    wxString GetVersion(){return m_version;}

    wxString m_version;

    protected:
    private:
};

#include <wx/string.h>
//-----Release-Feature-Fix------------------
#define VERSION wxT("1.4.0 2014/02/8 dbg")
//------------------------------------------
// Release - Current development level
// Feature - User interface level
// Fix     - bug fix or non UI breaking addition
#endif // VERSION_H

