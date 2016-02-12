/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MENUITEMSMANAGER_H
#define MENUITEMSMANAGER_H

#include "settings.h"
#include <wx/menu.h>
#include <list>


/** \brief base class to manage the menu items
 */
class Menu_point_base
{
    public:
        Menu_point_base() : m_deleted(false)   {};
        virtual ~Menu_point_base()  {};

        /** \brief Delete the managed menu item from the menus
         *
         * This is a pure virtual function and has to be overwritten in the
         * derived classes with the specialized function
         * \return virtual void
         *
         */
        virtual void Delete() = 0;
    protected:
        bool m_deleted;  /**<  indicates if the menu is removed*/
};

/** \brief This class represents a sub menu
 */
class Menu_point_sub_menu : public Menu_point_base
{
    public:
    Menu_point_sub_menu(wxMenuItem *item, wxMenu *parent) : m_item(item), m_parent(parent)
    {
    };

    virtual ~Menu_point_sub_menu()
    {
    };

    void Delete()
    {
        if(m_deleted)
            return;
        if(m_parent == nullptr || m_item == nullptr)
            return;

        if(m_item->GetSubMenu() != nullptr)
            return;

        m_parent->Destroy(m_item);
        m_deleted = true;
    };

    private:
        wxMenuItem *m_item;
        wxMenu     *m_parent;
};

/** \brief This class represents a Menu item
 */
class Menu_point_item : public Menu_point_base
{
    public:
    Menu_point_item(wxMenuItem *item, wxMenu *parent) : m_item(item), m_parent(parent) {};

    virtual ~Menu_point_item()  {};

    void Delete()
    {
        if(m_deleted)
            return;
        if(m_parent == nullptr || m_item == nullptr)
            return;

        m_parent->Destroy(m_item);
        m_deleted = true;
    };

    private:
    wxMenuItem *m_item;
    wxMenu     *m_parent;
};

/** \brief This class represents a sub menu in the menu bar
 */
class Menu_point_menubar : public Menu_point_base
{
    public:
    Menu_point_menubar(wxMenuBar* bar,int pos) : m_bar(bar) , m_pos(pos) {};
    virtual ~Menu_point_menubar()    {};

    void Delete()
    {
        if(m_deleted || m_bar == nullptr)
            return;

        wxMenu* to_delete = m_bar->Remove(m_pos);
        if(to_delete != nullptr)
        {
            delete to_delete;
            m_deleted = true;
        }

    }

    private:
        wxMenuBar* m_bar;
        int m_pos;
};


typedef std::list<Menu_point_base*> menu_point_list;    /**< A list to save the managed objects */

/**
  * @brief Manager for wxMenuItem pointers.
  *
  * This class manages an array of wxMenuItem pointers. Usually used by
  * classes that need to create menu items in the app and, at some point,
  * remove them *without* messing with other menu items, created by other
  * classes. Useful for plugins.\n
  * To use it, add a MenuItemsManager variable in your class and then
  * use MenuItemsManager::Add() to add menu items to a menu (instead of
  * wxMenu::Append). When you no longer want those menu items, call
  * MenuItemsManager::Clear(). That's it.
  *
  * @author Yiannis Mandravellos
  */
class DLLIMPORT MenuItemsManager
{
    public:
        MenuItemsManager(bool autoClearOnDestroy = true);
        virtual ~MenuItemsManager();

        virtual int CreateFromString(const wxString& menuPath, int id);

        virtual wxMenuItem* InsertSubmenu(wxMenu* parent, int id, int pos, const wxString& caption, const wxString& helptext);
        virtual wxMenuItem* Add(wxMenu* parent, int id, const wxString& caption, const wxString& helptext);
        virtual wxMenuItem* Insert(wxMenu* parent, int index, int id, const wxString& caption, const wxString& helptext);
        virtual void Clear();
    protected:
        bool m_AutoClearOnDestroy; // if true, the menus are cleared in the destructor
        menu_point_list m_menu_points;  // The managed array of menu items to be deleted
    private:
};

#endif // MENUITEMSMANAGER_H

