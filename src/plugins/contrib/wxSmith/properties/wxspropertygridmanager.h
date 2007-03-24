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
            long style = wxPGMAN_DEFAULT_STYLE,
            const wxChar* name = wxPropertyGridManagerNameStr);

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
         *        if NULL, container has been unbinded and manager must be cleared.
         */
        virtual void OnContainerChanged(wxsPropertyContainer* NewContainer) {}

    private:

        /** \brief Function clearing current content of property grid */
        void UnbindAll();

        /** \brief Function unbinding given container
         *
         * This function destroys all property enteries using given container.
         * It's automatically called in container's destructor but it may
         * be used in other places too.
         *
         * \param PC pointer to property container
         */
        void UnbindPropertyContainer(wxsPropertyContainer* PC);

        /** \brief Function updating content of property grid
         *  \param PC container which changed it's content, if NULL, content
         *         will always be updated, no matter if it's shown in grid
         */
        void Update(wxsPropertyContainer* PC);

        /** \brief Function used when adding new property to grid
         *
         * It should be called from wxsProperty::PGRegister() only !!!
         */
        long Register(wxsPropertyContainer* Container,wxsProperty* Property,wxPGId Id,long Index);

        /** \brief Function setting new main property container */
        void SetNewMainContainer(wxsPropertyContainer* Container);

        /** \brief Handler for roporting change event */
        void OnChange(wxPropertyGridEvent& event);

        /** \brief Singleton object */
        static wxsPropertyGridManager* Singleton;

        WX_DEFINE_ARRAY(wxPGId,wxArrayPGId);
        WX_DEFINE_ARRAY(wxsProperty*,wxArrayProps);
        WX_DEFINE_ARRAY(wxsPropertyContainer*,wxArrayCont);
        WX_DECLARE_HASH_SET(wxsPropertyContainer*,wxPointerHash,wxPointerEqual,wxSetCont);

        wxArrayPGId  PGIDs;                     /// \brief Array of property identifiers
        wxArrayProps PGEnteries;                /// \brief Array mapping enteries in grid to properties
        wxArrayLong  PGIndexes;                 /// \brief Array of internal property indexes used inside wxsProperty
        wxArrayCont  PGContainers;              /// \brief Array of container objects associated with properties
        wxSetCont    PGContainersSet;           /// \brief Set of used containers, will be used to quickly determine if given container is used in manager
        wxsProperty* PreviousProperty;          /// \brief Previous property used in Register()
        long         PreviousIndex;             /// \brief Previous index used when automatically calculating property indexes

        wxsPropertyContainer* MainContainer;    /// \brief Main container

        DECLARE_EVENT_TABLE()

        friend class wxsProperty;
        friend class wxsPropertyContainer;
};

/** \brief Macro for easy acces to property grid manager */
#define wxsPGRID() wxsPropertyGridManager::Get()

#endif
