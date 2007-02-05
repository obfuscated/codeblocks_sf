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

#ifndef CB_PRECOMP
    #include <wx/frame.h> // GetAppWindow
#endif

#include "sdk_precomp.h"
#include "manager.h"
#include "menuitemsmanager.h"
#include <wx/regex.h>

namespace
{
    wxRegEx reInsert(_T("([0-9]+):.+"));
};

MenuItemsManager::MenuItemsManager(bool autoClearOnDestroy)
    : m_AutoClearOnDestroy(autoClearOnDestroy)
{
    //ctor
}

MenuItemsManager::~MenuItemsManager()
{
    //dtor
    if (m_AutoClearOnDestroy)
    {
        Clear();
        m_MenuItems.Clear();
    }
}

/** @brief Add a menu item
  *
  * @param parent The menu to append the menu item to
  * @param id The menu item ID (use wxID_SEPARATOR for adding a separator)
  * @param caption The caption for the new menu item
  * @param helptext The help text for the new menu item
  * @return The new menu item or NULL for failure.
  */
wxMenuItem* MenuItemsManager::Add(wxMenu* parent, int id, const wxString& caption, const wxString& helptext)
{
    if (!parent)
        return 0;
    wxMenuItem* ni = new wxMenuItem(parent, id, caption, helptext);
    m_MenuItems.Add(ni);
    parent->Append(ni);
    return ni;
}

/** @brief Insert a menu item
  *
  * @param parent The menu to insert the menu item to
  * @param index The index where to insert the menu item
  * @param id The menu item ID (use wxID_SEPARATOR for adding a separator)
  * @param caption The caption for the new menu item
  * @param helptext The help text for the new menu item
  * @return The new menu item or NULL for failure.
  */
wxMenuItem* MenuItemsManager::Insert(wxMenu* parent, int index, int id, const wxString& caption, const wxString& helptext)
{
    if (!parent)
        return 0;
    wxMenuItem* ni = new wxMenuItem(parent, id, caption, helptext);
    m_MenuItems.Add(ni);
    parent->Insert(index, ni);
    return ni;
} // end of Insert

/** @brief Clear all managed menu items
  */
void MenuItemsManager::Clear()
{
    for (unsigned int i = 0; i < m_MenuItems.Count(); ++i)
    {
        wxMenuItem* ni = m_MenuItems[i];
        wxMenu* menu = ni->GetMenu();
        wxMenu* subMenu = ni->GetSubMenu();
        if (menu)
        {
            // only delete if it's not a submenu or, if it is but it's empty
            if (!subMenu || subMenu->GetMenuItemCount() == 0)
                menu->Delete(ni);
        }
    }
    m_MenuItems.Clear();
} // end of Clear

/** @brief Create menu path from string
  * @param menuPath The full menu path. This can be separated by slashes (/)
  *                 to create submenus (e.g. "MyScripts/ASubMenu/MyItem").
  *                 If the last part of the string ("MyItem" in the example)
  *                 starts with a dash (-) (e.g. "-MyItem") then a menu
  *                 separator is prepended before the actual menu item.
  * @param id The menu item ID (use wxID_SEPARATOR for adding a separator)
  * @return The newly created menu item or NULL for failure.
  */
wxMenuItem* MenuItemsManager::CreateFromString(const wxString& menuPath, int id)
{
    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    wxMenu* menu = 0;
    size_t pos = 0;
    while (true)
    {
        // ignore consecutive slashes
        while (pos < menuPath.Length() && menuPath.GetChar(pos) == _T('/'))
        {
            ++pos;
        }

        // find next slash
        size_t nextPos = pos;
        while (nextPos < menuPath.Length() && menuPath.GetChar(++nextPos) != _T('/'))
            ;

        wxString current = menuPath.Mid(pos, nextPos - pos);
        if (current.IsEmpty())
            break;
        bool isLast = nextPos >= menuPath.Length();

        bool insert = false;
        unsigned long insertIndex = 0;
        if (reInsert.Matches(current))
        {
            // insert menu instead append (format "insertIndex:menuName")
            wxString indexS = reInsert.GetMatch(current, 1);
            if (indexS.ToULong(&insertIndex, 10))
            {
                current.Remove(0, indexS.Length() + 1); // +1 to remove the ":" too
                insert = true;
            }
        }

        if (!menu)
        {
            if (isLast)
                return 0;

            // for first entry we must search on the menubar
            int menuPos = mbar->FindMenu(current);
            if (menuPos == wxNOT_FOUND)
            {
                menu = new wxMenu();
                mbar->Insert(insert ? insertIndex : mbar->GetMenuCount() - 2, menu, current); // -2 to be inserted before "Settings"
            }
            else
                menu = mbar->GetMenu(menuPos);
        }
        else
        {
            bool needsSep = current.StartsWith(_T("-"));
            if (needsSep)
                current.Remove(0, 1); // remove dash (-)

            wxMenu* existingMenu = 0;
            int existing = menu->FindItem(current);
            if (existing != wxNOT_FOUND)
            {
                // existing menu
                // if it is the final item we want to create, display error and stop
                if (isLast || existing >= (int)menu->GetMenuItemCount())
                    return 0;

                // else just keep the menu pointer updated
                existingMenu = menu->GetMenuItems()[existing]->GetSubMenu();
                if (!existingMenu)
                    return 0;

                menu = existingMenu;
            }
            else
            {
                if (needsSep)
                {
                    wxMenuItem* item = new wxMenuItem(menu, wxID_SEPARATOR);
                    menu->Insert(insert ? insertIndex : menu->GetMenuItemCount(), item);
                }

                if (current.IsEmpty()) // may be now if it was just a separator (-)
                    break;

                if (isLast)
                    return Insert(menu, insert ? insertIndex : menu->GetMenuItemCount(), id, current, wxEmptyString);
                else
                {
                    wxMenu* sub = new wxMenu;
                    wxMenuItem* item = new wxMenuItem(menu, -1, current, wxEmptyString, wxITEM_NORMAL, sub);
                    menu->Insert(insert ? insertIndex : menu->GetMenuItemCount(), item);
                    menu = sub;
                }
            }
        }

        pos = nextPos; // prepare for next loop
    }
    return 0;
} // end of CreateFromString
