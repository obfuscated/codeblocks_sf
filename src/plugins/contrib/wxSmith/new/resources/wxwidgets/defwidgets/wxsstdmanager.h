#ifndef WXSSTDMANAGER_H
#define WXSSTDMANAGER_H

#include "../wxsitemmanager.h"

class wxsStdManagerT : public wxsItemManager
{
	public:

        /** \brief Ctor*/
		wxsStdManagerT();

		/** \brief Dctor */
		virtual ~wxsStdManagerT();

		/** \brief Initializing manager
		 *
		 * During initialization, manager loads all icons
		 */
		virtual bool Initialize();

		/** \brief Getting number of handled widgets */
        virtual int GetCount();

        /** \brief Getting item's info */
        virtual const wxsItemInfo& GetItemInfo(int Number);

        /** \brief Getting new item */
        virtual wxsItem* ProduceItem(int Number,wxsWindowRes* Res);
};

extern wxsStdManagerT wxsStdManager;

#endif
