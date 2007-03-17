#ifndef WXSUSERITEMMANAGER_H
#define WXSUSERITEMMANAGER_H

#include "wxsuseritem.h"

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

        /** \brief Initializing all user-defined items */
        void InitializeUserItems();

        /** \brief Deleting all user-defined items */
        void UninitializeUserItems();

    private:

        /** \brief Hidden ctor (this class may be created as singleton only */
        wxsUserItemManager();

        /** \brief Hidden dctor (this class may be created as singleton only */
        virtual ~wxsUserItemManager();

        /** \brief Structure describing one user-defined item */
        struct UserItemInfo
        {

        };
};



#endif
