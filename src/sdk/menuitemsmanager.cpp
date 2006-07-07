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
#include "menuitemsmanager.h"


MenuItemsManager::MenuItemsManager()
    : m_Menu(0L)
{
	//ctor
}

MenuItemsManager::~MenuItemsManager()
{
	//dtor
	Clear(m_Menu);
	m_MenuItems.Clear();
}

/** @brief Add a menu item
  *
  * @param parent The menu to append the menu item to
  * @param id The menu item ID (use wxID_SEPARATOR for adding a separator)
  * @param caption The caption for the new menu item
  * @param helptext The help text for the new menu item
  */
void MenuItemsManager::Add(wxMenu* parent, int id, const wxString& caption, const wxString& helptext)
{
    if (!parent)
        return;
    m_Menu = parent;
    wxMenuItem* ni = new wxMenuItem(parent, id, caption, helptext);
    m_MenuItems.Add(ni);
    parent->Append(ni);
}

/** @brief Insert a menu item
  *
  * @param parent The menu to insert the menu item to
  * @param index The index where to insert the menu item
  * @param id The menu item ID (use wxID_SEPARATOR for adding a separator)
  * @param caption The caption for the new menu item
  * @param helptext The help text for the new menu item
  */
void MenuItemsManager::Insert(wxMenu* parent, int index, int id, const wxString& caption, const wxString& helptext)
{
    if (!parent)
        return;
    m_Menu = parent;
    wxMenuItem* ni = new wxMenuItem(0L, id, caption, helptext);
    m_MenuItems.Add(ni);
    parent->Insert(index, ni);
}

/** @brief Clear all managed menu items
  * @param menu The menu that holds the menu items
  */
void MenuItemsManager::Clear(wxMenu* menu)
{
    if (!menu)
        return;
    for (unsigned int i = 0; i < m_MenuItems.Count(); ++i)
    {
        wxMenuItem* ni = m_MenuItems[i];
        menu->Delete(ni);
    }
    m_MenuItems.Clear();
}
