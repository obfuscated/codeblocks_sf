#ifndef WXSITEMFACTORY_H
#define WXSITEMFACTORY_H

#include <wx/string.h>
#include <wx/hashmap.h>

class wxsItem;
class wxsItemInfo;
class wxsItemManager;
class wxsWindowRes;

/** \brief Class managing creation, destruction and enumeration of supported
 *         items
 */
class wxsItemFactory
{
	public:

        /** \brief Function creating singleton object if there's no one */
        static void Create();

        /** \brief Getting singleton object */
        static inline wxsItemFactory* Get() { return Singleton; }

        /** \brief Returning info for item with given name
         * \return Pointer to info or NULL if there's no such item
         */
        const wxsItemInfo* GetInfo(const wxString& Name);

        /** \brief Creating item with given name */
        wxsItem* Generate(const wxString& Name,wxsWindowRes* Res);

        /** \brief Destroying given item */
        void Kill(wxsItem* Widget);

        /** \brief Getting first item info */
        const wxsItemInfo* GetFirstInfo();

        /** \brief Getting next item info */
        const wxsItemInfo* GetNextInfo();

        /** \brief Registring manager inside this factory */
        void RegisterManager(wxsItemManager* Manager);

	private:

        /** \brief Ctor - hidden because this is singleton class */
		wxsItemFactory();

		/** \brief Dctor, not virtual since this class can not be derived from */
		~wxsItemFactory();

		/** \brief Small structure used for mapping item name */
		struct ItemData
		{
		    wxsItemManager* Manager;
		    int Number;
		    const wxsItemInfo* Info;
		};

        /** \brief Map used to handle all types of widgets */
        WX_DECLARE_STRING_HASH_MAP(ItemData,ItemMapT);
        typedef ItemMapT::iterator ItemMapI;

        /** \brief Map for widgets */
        ItemMapT Items;

        /** \brief Internal iterator */
        ItemMapI Iterator;

        /** \brief Singleton object */
        static wxsItemFactory* Singleton;
};

#endif
