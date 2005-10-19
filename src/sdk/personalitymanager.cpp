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
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include "personalitymanager.h"
#include "manager.h"
#include "managerproxy.h"
#include "messagemanager.h"
#include "configmanager.h"

#include <wx/intl.h>

PersonalityManager* PersonalityManager::Get()
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        PersonalityManager::Free();
    else if (!PersonalityManagerProxy::Get())
        PersonalityManagerProxy::Set( new PersonalityManager() );
    return PersonalityManagerProxy::Get();
}

void PersonalityManager::Free()
{
	if (PersonalityManagerProxy::Get())
	{
		delete PersonalityManagerProxy::Get();
		PersonalityManagerProxy::Set( 0L );
	}
}

PersonalityManager::PersonalityManager()
    : m_CurrentPersonalityIdx(0)
{
	//ctor
	ReadPersonalities();
}

PersonalityManager::~PersonalityManager()
{
	//dtor
}

void PersonalityManager::ReadPersonalities()
{
    m_Personalities.Clear();
    m_Personalities.Add(_("Full IDE (default)"));

	wxString str;
	long cookie;

	ConfigManager::Get()->SetPath(GetPersonalitiesRoot());
	bool cont = ConfigManager::Get()->GetFirstGroup(str, cookie);
	while (cont)
	{
        m_Personalities.Add(str);
		cont = ConfigManager::Get()->GetNextGroup(str, cookie);
	}
	ConfigManager::Get()->SetPath(_T("/"));
}

void PersonalityManager::SetPersonality(const wxString& personality, bool createIfNotExist)
{
    if (personality.IsEmpty())
    {
        m_CurrentPersonalityIdx = 0;
        return;
    }

    m_CurrentPersonalityIdx = m_Personalities.Index(personality);
    if (m_CurrentPersonalityIdx == -1)
    {
        if (createIfNotExist)
        {
            m_Personalities.Add(personality);
            m_CurrentPersonalityIdx = m_Personalities.GetCount() - 1;
        }
        else
            m_CurrentPersonalityIdx = 0;
    }
}

const wxString& PersonalityManager::GetPersonality()
{
    static wxString pers;
    pers = m_CurrentPersonalityIdx > 0 && m_CurrentPersonalityIdx < (int)m_Personalities.GetCount()
            ? m_Personalities[m_CurrentPersonalityIdx]
            : wxT("");
    return pers;
}

const wxArrayString& PersonalityManager::GetPersonalitiesList()
{
    return m_Personalities;
}

const wxString& PersonalityManager::GetPersonalitiesRoot()
{
    static wxString root = _T("/personalities");
    return root;
}

const wxString& PersonalityManager::GetPersonalityKey()
{
    static wxString key;
    key = m_CurrentPersonalityIdx <= 0
            ? wxT("") // default personality
            : GetPersonalitiesRoot() + _T("/") + GetPersonality();
    return key;
}
