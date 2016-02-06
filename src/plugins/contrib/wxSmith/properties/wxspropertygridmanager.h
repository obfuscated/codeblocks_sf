/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSPROPERTYGRIDMANAGER_H
#define WXSPROPERTYGRIDMANAGER_H

#include <wx/scrolwin.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/hashset.h>
#include <wx/dcclient.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>

#include <prep.h>

#if wxCHECK_VERSION(3, 0, 0)
#define wxPGId wxPGProperty*
#endif

class wxsPropertyContainer;
class wxsProperty;

/** \brief Custom property grid manager
 *
 * This class was added to handle OnChange event from property window
 * because parent event handler rather won't be accessible.
 * This object is singleton and always one and not more than one should
 * be created at one time.
 */
class wxsPropertyGridManager: public wxPropertyGridManager
{
    DECLARE_CLASS(wxsPropertyGridManager)

    public:

        /** \brief Ctor */
        wxsPropertyGridManager(
            wxWindow* parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxPGMAN_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER,
            #if wxCHECK_VERSION(3, 0, 0)
            const char* name = wxPropertyGridManagerNameStr);
            #else
            const wxChar* name = wxPropertyGridManagerNameStr);
            #endif

        /** \brief Dctor */
        virtual ~wxsPropertyGridManager();

        /** \brief Getting singleton object */
        static inline wxsPropertyGridManager* Get() { return Singleton; }

        /** \brief Getting main container
         *
         * Returned container is the one which was activated using
         * ShowInPropertyGrid() method.
         */
        inline wxsPropertyContainer* GetContainer() { return MainContainer; }

    protected:

        /** \brief Function notifying about change of main wxsPropertyContainer
         *
         * \param NewContainer new container associated with this manager,
         *        if 0, container has been unbinded and manager must be cleared.
         */
        virtual void OnContainerChanged(cb_unused wxsPropertyContainer* NewContainer) {}

    private:

        /** \brief Function clearing current content of property grid */
        void UnbindAll();

        /** \brief Function unbinding given container
         *
         * This function destroys all property entries using given container.
         * It's automatically called in container's destructor but it may
         * be used in other places too.
         *
         * \param PC pointer to property container
         */
        void UnbindPropertyContainer(wxsPropertyContainer* PC, bool doFreeze = true);

        /** \brief Function updating content of property grid
         *  \param PC container which changed it's content, if 0, content
         *         will always be updated, no matter if it's shown in grid
         */
        void Update(wxsPropertyContainer* PC);

        /** \brief Function used when adding new property to grid
         *
         * It should be called from wxsProperty::PGRegister() only !!!
         */
        long Register(wxsPropertyContainer* Container,wxsProperty* Property,wxPGId Id,long Index);

        /** \brief Starting enumeration of properties for new property container */
        void NewPropertyContainerStart();

        /** \brief Adding new property into temporary list of properties */
        void NewPropertyContainerAddProperty(wxsProperty* Property,wxsPropertyContainer* Container);

        /** \brief Finish enumeration of properties for new property container */
        void NewPropertyContainerFinish(wxsPropertyContainer* Container);

        /** \brief Deleting temporary list used while changing container */
        void DeleteTemporaryPropertiesList();

        /** \brief Changing main property container */
        void SetNewMainContainer(wxsPropertyContainer* NewMain);

        /** \brief Handler for reporting change event */
        void OnChange(wxPropertyGridEvent& event);

        /** \brief Handler for jumping to event handler when user double click on the event name */
        void OnDoubleClick(wxPropertyGridEvent& event);

        /** \brief Data of selected property */
        struct SelectionData
        {
            int m_PageIndex;
            wxString m_PropertyName;
        };

        /** \brief Storing currently selected property
         *
         * \param Data place where selection should be stored, if NULL,
         *        selection will be stored in internal variable
         */
        void StoreSelected(SelectionData* Data=0);

        /** \brief Restoring selected property
         *
         * \param Data structure containing selection data, if NULL,
         *        selection will be restored from internal variable
         */
        void RestoreSelected(const SelectionData* Data=0);

        /** \brief Singleton object */
        static wxsPropertyGridManager* Singleton;

        struct TemporaryPropertiesList
        {
            wxsProperty* Property;
            wxsPropertyContainer* Container;
            int Priority;
            TemporaryPropertiesList* Next;
        };

        WX_DEFINE_ARRAY(wxPGId,wxArrayPGId);
        WX_DEFINE_ARRAY(wxsProperty*,wxArrayProps);
        WX_DEFINE_ARRAY(wxsPropertyContainer*,wxArrayCont);
        WX_DECLARE_HASH_SET(wxsPropertyContainer*,wxPointerHash,wxPointerEqual,wxSetCont);

        wxArrayPGId  PGIDs;                         ///< \brief Array of property identifiers
        wxArrayProps PGEntries;                     ///< \brief Array mapping entries in grid to properties
        wxArrayLong  PGIndexes;                     ///< \brief Array of internal property indexes used inside wxsProperty
        wxArrayCont  PGContainers;                  ///< \brief Array of container objects associated with properties
        wxSetCont    PGContainersSet;               ///< \brief Set of used containers, will be used to quickly determine if given container is used in manager
        wxsProperty* PreviousProperty;              ///< \brief Previous property used in Register()
        long         PreviousIndex;                 ///< \brief Previous index used when automatically calculating property indexes

        wxsPropertyContainer* MainContainer;        ///< \brief Main container

        TemporaryPropertiesList* PropertiesList;    ///< \brief List used while generating sorted properties list

        SelectionData LastSelection;                ///< \brief Internal selection structure used to restore selected property

        DECLARE_EVENT_TABLE()

        friend class wxsProperty;
        friend class wxsPropertyContainer;
};

/** \brief Macro for easy acces to property grid manager */
#define wxsPGRID() wxsPropertyGridManager::Get()

#endif
