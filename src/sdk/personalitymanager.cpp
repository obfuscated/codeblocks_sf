/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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

template<> PersonalityManager* Mgr<PersonalityManager>::instance = 0;
template<> bool  Mgr<PersonalityManager>::isShutdown = false;

PersonalityManager::PersonalityManager()
{
    PersonalityManager::pers = _T("default");
}

void PersonalityManager::SetPersonality(const wxString& personality, cb_unused bool createIfNotExist)
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

