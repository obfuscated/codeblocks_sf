#ifndef WXSUSERITEMMANAGER_H
#define WXSUSERITEMMANAGER_H

#include "wxsuseritemdescription.h"

/** \brief This class does manage simplified user-defined items
 *
 * It does read items from configuration file, allows to manipulate them
 * and writes proper configuration files
 */
class wxsUserItemManager
{
    public:

        /** \brief Getting singleton object */
        static wxsUserItemManager& Get();

        /** \brief Initializing all user-defined items
         *
         * This function reads all user items from configuration
         * and creates proper objects for them.
         */
        void InitializeUserItems();

        /** \brief Deleting all user-defined items
         *
         * \note This function does not save items into configuration
         */
        void UninitializeUserItems();

        /** \brief Storing data of all user-defined items inside configuration */
        void StoreItemsInsideConfiguration();

        /** \brief Getting number of manager items */
        int GetItemsCount();

        /** \brief Getting description of item at given index */
        wxsUserItemDescription* GetDescription(int Index);

        /** \brief Deleting descrption at given index */
        void DeleteDescription(int Index);

        /** \brief Unregistering all items from factory */
        void UnregisterAll();

        /** \brief Reregistering all items inside factory */
        void ReregisterAll();

        /** \brief Adding new user item description */
        void AddDescription(wxsUserItemDescription* Description);

    private:

        /** \brief Hidden ctor (this class may be created as singleton only */
        wxsUserItemManager();

        /** \brief Hidden dctor (this class may be created as singleton only */
        ~wxsUserItemManager();

        WX_DEFINE_ARRAY(wxsUserItemDescription*,DescriptionsT);

        /** \brief Descriptions for all user-defined items */
        DescriptionsT m_Descriptions;

};



#endif
