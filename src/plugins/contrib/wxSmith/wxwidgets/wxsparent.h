/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSPARENT_H
#define WXSPARENT_H

#include "wxsitem.h"

/** \brief This class represents widget with child items
 *
 * Each item may have some additional configuration stored in wxsPropertyContainer
 * class. These informations must be created in OnBuildExtra function and can
 * be used to keep some extra properties for each child (like configuration of item
 * in sizer). Returned container should properly read / write items from / to
 * xml data since this will be used.
 *
 */
class wxsParent: public wxsItem
{
    public:

        /** \brief Ctor */
        wxsParent(wxsItemResData* Data,const wxsItemInfo* Info,long PropertiesFlags,const wxsEventDesc* Events,const wxsStyleSet* StyleSet);

        /** \brief Dctor */
        virtual ~wxsParent();

        /** \brief Getting number of children */
        inline int GetChildCount() { return (int)Children.Count(); }

        /** \brief Getting child at specified index */
        wxsItem* GetChild(int Index);

        /** \brief Adding new child item */
        bool AddChild(wxsItem* Child,int Position=-1);

        /** \brief Unbinding child item by index (0-based number of child)
         *  \note This function does not delete item but only remove the connection
         *        between item and parent.
         */
        void UnbindChild(int Index);

        /** \brief Unbinding child item by pointer
         *  \note This function does not delete item but only remove the connection
         *        between item and parent.
         */
        void UnbindChild(wxsItem* Child);

        /** \brief Moving item from one position to another
         *  \param OldIndex old position of item
         *  \param NewIndex new position of item
         *  \return Real positionof item after moving it (and after checking
         *          boundings) or -1, if OldIndex was invalid.
         */
        int MoveChild(int OldIndex,int NewIndex);

        /** \brief Getting child index
         *  \return index of child or -1 if there's no such child
         */
        int GetChildIndex(wxsItem* Child);

        /** \brief Function checking if give item is grandchild (chld or child of child or...) of this item
         *  \param Child searched child
         *  \param Safe set to true if there's no certainity that Child pointer is valid one.
         *         It will work slower but won't produse seg faults.
         */
        bool IsGrandChild(wxsItem* Child,bool Safe=false);

        /** \brief Function storing extra info for child in xml node */
        void StoreExtraData(int Index,TiXmlElement* Element);

        /** \brief Function setting up child's extra data from xml node */
        void RestoreExtraData(int Index,TiXmlElement* Element);

        /** \brief Function getting extra data for given child */
        wxsPropertyContainer* GetChildExtra(int Index);

        /** \brief Function checking if given item can be added to this one
         * \note This is only a wrapper to OnCanAddChild virtual function
         */
        inline bool CanAddChild(wxsItem* Item,bool ShowMessage) { return OnCanAddChild(Item,ShowMessage); }

        /** \brief Function checking if given child is visible in editor
         * \note This function is only a wrapper to OnIsChildPreviewVisible
         */
        inline bool IsChildPreviewVisible(wxsItem* Child) { return OnIsChildPreviewVisible(Child); }

        /** \brief Ensuring that child item is visible in preview
         * \note This is only a wrapper to OnEnsureChildPreviewVisible
         */
        inline bool EnsureChildPreviewVisible(wxsItem* Child) { return OnEnsureChildPreviewVisible(Child); }

    protected:

        /* *********************************************************************** */
        /*  Followig functions may be used to easy create wxsParent-derived class  */
        /* *********************************************************************** */

        /** \brief Function building extra data block for item
         *
         * This function may be overridden in child classes to add extra
         * configuration for each child, f.ex. configuration of sizer enteries.
         */
        virtual wxsPropertyContainer* OnBuildExtra() { return 0; }

        /** \brief Returning name of additional object created for child items
         *
         * This function affects behaviour of standard OnXmlReadChild and OnXmlWriteChild
         * functions. If it returns non-empty string, child items will have
         * additional <object...> xml node created and it will use StoreExtraData and
         * RestoreExtraData to save extra informations.
         */
        virtual wxString OnXmlGetExtraObjectClass() { return wxEmptyString; }

        /** \brief Function checking if given item can be added to this one
         * \param Item checked item
         * \param ShowMessage if true and item can not be added, show message
         *        explaining why it can not be done
         */
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage) { return true; }

        /** \brief Function adding panels for child to wxsAdvQPP class
         *
         * Analogically to EnumChildProperties, this function should create
         * additional panels and register them inside wxsAdvQPP class and
         * should call Child->MyAddQPP(QPP) somewhere inside.
         */
        virtual void OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);

        /* **************************************************************** */
        /*  Following functions may be also overridden but it shouldn't be  */
        /*  necessarry                                                      */
        /* **************************************************************** */

        /** \brief Function enumerating properties for given child
         *
         * This function should create properties for child item, it's done
         * in parent item because of possible extra data. Parent can add
         * here some additional properties before and/or after properties of
         * child item.
         *
         * Default implementation calls Child->EnumItemProperties(Flags)
         * and Extra->EnumProperties at the end.
         *
         * \note This function MUST call Child->EnumItemProperties(Flags)
         *       somewhere in the code.
         */
        void OnEnumChildProperties(wxsItem* Child,long Flags);

        /** \brief Rewritten xml reading function - it will add support for children loading */
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        /** \brief Rewritten xml writing function - it will add support for children saving */
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        /** \brief Function loading child from given xml node
         *
         * This function will be called for each <object...> nodes inside
         * parent. It must validate this node and add children if this can
         * be done. By default, this function simply load new class, but some
         * containers require extended objects (like sizeritem) to store
         * additional data.
         */
        virtual bool OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Function saving child to goven xml node
         *
         * This function will be called for each child. Element passed
         * as param is pointer to newly created <object...> node where
         * child should be stored.
         */
        virtual bool OnXmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Function checking if given child preview is visible in editor
         *
         * This function may be used by items like wxNotebook which show only
         * one child item. It's used to avoid operating on invisible items
         * inside editor
         * \note this function does not check if parent item is visible
         * \param Child pointer to child
         * \return true if child visible, false if not
         */
        virtual bool OnIsChildPreviewVisible(wxsItem* Child) { return true; }

        /** \brief Function ensuring that given child of this item is visible inside editor
         *
         * This function is used to automatically switch visible child for containers
         * like wxNotebook (only one chid is show, other are hidden) when somebody
         * clicks on it on resource browser.
         * \note This function does not make sure that parent is visible
         * \return false if nothing has changed, true if preview must be refreshed
         */
        virtual bool OnEnsureChildPreviewVisible(wxsItem* Child) { return false; }

    private:

        /** \brief Function converting this item to wxsParent class.
         *
         * Function is private to make sure that no child classes will
         * overload it.
         */
        virtual wxsParent* ConvertToParent() { return this; }

        WX_DEFINE_ARRAY(wxsItem*,wxArrayItem);
        WX_DEFINE_ARRAY(wxsPropertyContainer*,wxArrayExtra);

        wxArrayItem  Children;   ///< \brief Array of child items
        wxArrayExtra Extra;      ///< \brief Array of extra data stored in generic void pointer

        friend class wxsItem;
};

#endif
