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
    #include "personalitymanager.h"
    #include "manager.h"
    #include "configmanager.h"
    #include <wx/intl.h>
    #include <wx/dir.h>
    #include <wx/filename.h>
#endif

#include <wx/arrstr.h>

PersonalityManager::PersonalityManager()
{
    PersonalityManager::pers = _T("default");
}

void PersonalityManager::SetPersonality(const wxString& personality, bool createIfNotExist)
{
	pers = personality;
}

const wxString PersonalityManager::GetPersonality()
{
    return pers;
}

const wxArrayString PersonalityManager::GetPersonalitiesList()
{
	wxArrayString list;
	wxDir::GetAllFiles(ConfigManager::GetFolder(sdConfig), &list, _T("*.conf"), wxDIR_FILES);

	for(size_t i = 0; i < list.GetCount(); ++i)
        list[i] = wxFileName(list[i]).GetName();

	return list;
}

wxString PersonalityManager::pers;

