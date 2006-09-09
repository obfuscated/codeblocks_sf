#ifndef WXSITEMMANAGER_H
#define WXSITEMMANAGER_H

class wxsItemInfo;
class wxsItem;
class wxsWindowRes;

/** \brief Class managing items in one widgets set
 *
 * There's one global wxsItemFactory class which is responsible for managing
 * all supported items.
 * Items are provided in packs (currently there's one pack with set of default
 * widgets), and one pack is managed inside wxsItemManager class.
 */
class wxsItemManager
{
    public:

        /** \brief Ctor */
        wxsItemManager() {}

        /** \brief Dctor */
        virtual ~wxsItemManager() {}

        /** \brief Function initializing manager
         *
         * This function may f.ex. load icons for items.
         * \return true when initiqalization successfull, false otherwise.
         */
        virtual bool Initialize() { return true; }

        /** \brief Returns number of handled items */
        virtual int GetCount() = 0;

        /** \brief Getting item's info
         * \param Number zero-based item number in range ( 0 .. GetCount()-1 )
         * \return reference to item info
         */
        virtual const wxsItemInfo& GetItemInfo(int Number) = 0;

        /** \brief Getting new item
         *
         * This function should return item directly from it's constructor.
         * Items require second step of initialization - this is done by calling
         * wxsItem::Create() function, but wxsItemFactory calls this function.
         *
         * \param Number zero-based item number in range ( 0 .. GetCount()-1 )
         * \param Res resource owning item
         */
        virtual wxsItem* ProduceItem(int Number,wxsWindowRes* Res) = 0;

        /** \brief Function registering this manager into main widget's factory
         *
         * The best way is to register inside constructor and create one static
         * class for item
         *
         * \return true if registration successfull, false otherwise
         *         (but failure should be rare case)
         */
        bool RegisterInFactory();
};

#endif
