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

#endif // CB_PRECOMP

#include "cbcolourmanager.h"

template<> ColourManager* Mgr<ColourManager>::instance = nullptr;
template<> bool  Mgr<ColourManager>::isShutdown = false;

void ColourManager::Load()
{
    ConfigManager *config = Manager::Get()->GetConfigManager(wxT("colours"));
    const wxArrayString &colours = config->EnumerateKeys(wxT("list"));
    for (size_t ii = 0; ii < colours.GetCount(); ++ii)
    {
        const wxString &id = colours[ii].Lower();
        const wxColour &colour = config->ReadColour(wxT("list/") + id);

        ColourDefMap::iterator it = m_colours.find(id);
        if (it != m_colours.end())
            it->second.value = colour;
        else
        {
            ColourDef def;
            def.value = def.defaultValue = colour;
            m_colours[id] = def;
        }
    }
}

void ColourManager::Save()
{
    ConfigManager *config = Manager::Get()->GetConfigManager(wxT("colours"));
    for (ColourDefMap::const_iterator it = m_colours.begin(); it != m_colours.end(); ++it)
    {
        if (it->second.value != it->second.defaultValue)
            config->Write(wxT("list/") + it->first, it->second.value);
    }
}

void ColourManager::RegisterColour(const wxString &category, const wxString &name,
                                   const wxString &id, const wxColour &defaultColour)
{
    wxString lowerID =  id.Lower();
    ColourDefMap::iterator it = m_colours.find(lowerID);
    if (it != m_colours.end())
    {
        it->second.name = name;
        it->second.category = category;
        it->second.defaultValue = defaultColour;
    }
    else
    {
        ColourDef def;
        def.name = name;
        def.category = category;
        def.value = def.defaultValue = defaultColour;
        m_colours[lowerID] = def;
    }
}

wxColour ColourManager::GetColour(const wxString &id) const
{
    ColourDefMap::const_iterator it = m_colours.find(id);
    return it != m_colours.end() ? it->second.value : *wxBLACK;
}

void ColourManager::SetColour(const wxString &id, const wxColour &colour)
{
    ColourDefMap::iterator it = m_colours.find(id);
    if (it != m_colours.end())
        it->second.value = colour;
}

const ColourManager::ColourDefMap& ColourManager::GetColourDefinitions() const
{
    return m_colours;
}
