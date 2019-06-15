//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clKeyboardManager.h
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
// Modifed for Code::Blocks by pecan

#include <wx/filename.h>

#include "manager.h"
#include "personalitymanager.h"
#include "configmanager.h"

#include "clKeyboardBindingConfig.h"
#include "json_node.h"

// ----------------------------------------------------------------------------
clKeyboardBindingConfig::clKeyboardBindingConfig() {}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
clKeyboardBindingConfig::~clKeyboardBindingConfig() {}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
clKeyboardBindingConfig& clKeyboardBindingConfig::Load()
// ----------------------------------------------------------------------------
{
    wxFileName fn(ConfigManager::GetConfigFolder(), _T("cbKeyBinder20.conf"));
    //-fn.AppendDir("config");
    wxString personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    fn.SetName(personality + _T(".") + fn.GetName());
    if(not fn.FileExists()) return *this;

    m_bindings.clear();
    JSONRoot root(fn);
    // unused typedef std::unordered_multimap<wxString, wxString> GlobalAccelMap_t;     //(2019/04/3)
    MenuItemDataMap_t globalBindings;                                                   //(2019/04/3)

    {//Block
        JSONElement menus = root.toElement().namedObject(_T("menus"));
        int arrSize = menus.arraySize();
        for(int i = 0; i < arrSize; ++i) {
            JSONElement item = menus.arrayItem(i);
            MenuItemData binding;
            binding.action = item.namedObject(_T("description")).toString();
            binding.accel = item.namedObject(_T("accelerator")).toString();
            binding.parentMenu = item.namedObject(_T("parentMenu")).toString();
            binding.resourceID = item.namedObject(_T("resourceID")).toString();
            if (binding.parentMenu.empty()) //Empty parent menu means a <global> accelerator
            {
                // Hold global key bindings until later
                globalBindings.insert(std::make_pair(binding.resourceID, binding));
                continue;
            }
            // insert regular menu items before globals
            m_bindings.insert(std::make_pair(binding.resourceID, binding));
        }//endif for arrSize

        // Append the global key bindings to the end of the menu bindings
        for(MenuItemDataMap_t::const_iterator iter = globalBindings.begin(); iter != globalBindings.end(); ++iter)
        {
            #if defined(LOGGING) //debugging
                wxString resourceID = iter->first;
                MenuItemData binding = iter->second;
            #endif
            m_bindings.insert(std::make_pair(iter->first, iter->second) );
        }

    }//end Block
    return *this;
}
// ----------------------------------------------------------------------------
clKeyboardBindingConfig& clKeyboardBindingConfig::Save()
// ----------------------------------------------------------------------------
{
    JSONRoot root(cJSON_Object);
    JSONElement mainObj = root.toElement();
    JSONElement menuArr = JSONElement::createArray(_T("menus"));
    mainObj.append(menuArr);
    for(MenuItemDataMap_t::iterator iter = m_bindings.begin(); iter != m_bindings.end(); ++iter) {
        JSONElement binding = JSONElement::createObject();
        binding.addProperty(_T("description"), iter->second.action);
        binding.addProperty(_T("accelerator"), iter->second.accel);
        binding.addProperty(_T("resourceID"), iter->second.resourceID);
        binding.addProperty(_T("parentMenu"), iter->second.parentMenu);
        menuArr.arrayAppend(binding);
    }
    wxString configDir = ConfigManager::GetConfigFolder();
    wxFileName fn(configDir, _T("cbKeyBinder20.conf"));
    wxString personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    fn.SetName(personality + _T(".") + fn.GetName());

    root.save(fn);
    return *this;
}
