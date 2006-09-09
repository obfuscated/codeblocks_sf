#ifndef WXSPARENT_H
#define WXSPARENT_H

#include "wxsitem.h"

/** \brief This class represents widget with child items
 *
 * Each item may have some additional configuration stored in wxsPropertyContainer
 * class. These informations must be created in BuildExtra function and can
 * be to keep some extra properties for each child (like configuration of item
 * in sizer). Returned container should properly read / write items from / to
 * xml data since this will be used in some special cases.
 *
 */
class wxsParent: public wxsItem
{
    public:

        /** \brief Ctor */
        wxsParent(wxsWindowRes* Resource);

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
         *  \param Safe set to true if there's no certainity that Child pointer is valid one. It will work slower but won't seg fault.
         */
        bool IsGrandChild(wxsItem* Child,bool Safe=false);

        /** \brief Function storing extra info for child in xml node */
        void StoreExtraData(int Index,TiXmlElement* Element);

        /** \brief Function setting up child's extra data from xml node */
        void RestoreExtraData(int Index,TiXmlElement* Element);

        /** \brief Rewritten xml reading function - it will add support for children loading */
        virtual bool XmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        /** \brief Rewritten xml writing function - it will add support for children saving */
        virtual bool XmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        /** \brief Function getting extra data for given child */
        wxsPropertyContainer* GetChildExtra(int Index);

        /** \brief Function checking if given item can be added to this one
         * \param Item checked item
         * \param ShowMessage if true and item can not be added, show message
         *        explaining why it can not be done
         */
        virtual bool CanAddChild(wxsItem* Item,bool ShowMessage) { return true; }

    protected:

        /** \brief Function building extra data block for item
         *
         * This function may be overridden in child classes to add extra
         * configuration for each child, f.ex. configuration of sizer enteries.
         */
        virtual wxsPropertyContainer* BuildExtra() { return NULL; }

        /** \brief Function enumerating properties for given child
         *
         * This function should create properties for child item, it's done
         * in parent item because of possible extra data. Parent can add
         * here some additional properties before and/or after properties of
         * child item.
         *
         * Default implementation calls Child->MyEnumProperties(Flags)
         * and Extra->EnumProperties at the end.
         *
         * \note This function MUST call Child->MyEnumProperties(Flags)
         *       somewhere in the code.
         */
        virtual void EnumChildProperties(wxsItem* Child,long Flags);

        /** \brief Function adding panels for child to wxsAdvQPP class
         *
         * Analogically to EnumChildProperties, this function should create
         * additional panels and register them inside wxsAdvQPP class and
         * should call Child->MyAddQPP(QPP) somewhere inside.
         */
        virtual void AddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);

        /** \brief Function loading child from given xml node
         *
         * This function will be called for each <object...> nodes inside
         * parent. It must validate this node and add children if this can
         * be done. By default, this function simply load new class, but some
         * containers require extended objects (like sizeritem) to store
         * additional data.
         */
        virtual bool XmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Function saving child to goven xml node
         *
         * This function will be called for each child. Element passed
         * as param is pointer to newly created <object...> node where
         * child should be stored.
         */
        virtual bool XmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Returning name of additional object created for child items
         *
         * This function affects behaviour of standard XmlReadChild and XmlWriteChild
         * functions. If it returns non-empty string, child items will have
         * additional <object...> xml node created and it will use StoreExtraData and
         * RestoreExtraData to save extra informations.
         */
        virtual wxString XmlGetExtraObjectClass() { return wxEmptyString; }

    private:

        /** \brief Function converting this item to wxsParent class.
         *
         * Function is private to make sure that no child classes will
         * overload it.
         */
        virtual wxsParent* ToParent() { return this; }

        WX_DEFINE_ARRAY(wxsItem*,wxArrayItem);
        WX_DEFINE_ARRAY(wxsPropertyContainer*,wxArrayExtra);

        wxArrayItem  Children;   ///< \brief Array of child items
        wxArrayExtra Extra;      ///< \brief Array of extra data stored in generic void pointer

        friend class wxsItem;

};

#endif
