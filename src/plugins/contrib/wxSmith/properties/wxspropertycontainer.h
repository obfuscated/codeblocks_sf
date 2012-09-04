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

#ifndef WXSPROPERTYCONTAINER_H
#define WXSPROPERTYCONTAINER_H

#include "wxsproperty.h"
#include "wxsquickpropspanel.h"

class wxsPropertyGridManager;

/** \brief Base class for objects using properties
 *
 * This class is responsible for operating on properties.
 * It also creates property window.
 */
class wxsPropertyContainer
{
    public:

        /** \brief Ctor */
        wxsPropertyContainer();

        /** \brief Dctor */
        virtual ~wxsPropertyContainer();

        /** \brief Changing Property Grid focus to this object
         *
         * This function can be used when this object is selected
         * to make main property grid point to it.
         */
        void ShowInPropertyGrid();

        /** \brief Function reading all properties from Xml node
         *  \param Element Xml element containing all elements of properties
         *         as child nodes
         */
        void XmlRead(TiXmlElement* Element);

        /** \brief Function writing all properties to Xml node
         *  \param Element Xml element where all elements of properties
         *         will be added
         */
        void XmlWrite(TiXmlElement* Element);

        /** \brief Function reading all properties from property stream */
        void PropStreamRead(wxsPropertyStream* Stream);

        /** \brief Function writing all properties to property stream */
        void PropStreamWrite(wxsPropertyStream* Stream);

        /** \brief Function building new quick properties panel.
         *
         * If there was panel before, it will be unbinded from current object
         * (and thus become useless) and new one will be binded.
         */
        wxsQuickPropsPanel* BuildQuickPropertiesPanel(wxWindow* Parent);

        /** \brief Function notifying about property change
         *
         * This function must be called when value of any property
         * has been changed externally (not from property grid nor
         * quick properties - there are special functions for them)
         * It causes property grid and quick properties panel to
         * reread it's content.
         *
         * \param CallPropertyChangeHandler - if true, PropertyChangeHandler will be called.
         */
        void NotifyPropertyChange(bool CallPropertyChangeHandler=false);

        /** \brief Operator reading from property stream */
        inline wxsPropertyStream& operator>>(wxsPropertyStream& Stream)
        {
            PropStreamRead(&Stream);
            return Stream;
        }

        /** \brief Operator writing to property stream */
        inline wxsPropertyStream& operator<<(wxsPropertyStream& Stream)
        {
            PropStreamWrite(&Stream);
            return Stream;
        }

        /** \brief Function returning flags of availability for this object
         *
         * These flags are passed to OnEnumProperties function and are used for
         * filtering flags inside Property function.
         * Using flags one can easily filter used properties.
         *
         * There are special reserved flags, which are put over properties
         * returned here before using flags:
         *  \li flPropGrid - this flag is set when operating on property grid
         *  \li flXml - this flag is set when current operation is XmlRead or XmlWrite
         *  \li flPropStream - this flag is set when operating on property stream
         * These flags are available only from OnEnumProperties or Property
         * functions, there's no way to check them outside.
         */
        inline long GetPropertiesFlags() { return OnGetPropertiesFlags(); }

    protected:

        /** \copydoc wxsPropertyContainer::GetPropertiesFlags() */
        virtual long OnGetPropertiesFlags() { return -1; }

        /** \brief Function enumerating properties
         *
         * This function MUST call Property or one of SubContainer functions
         * for each  property or sub container which will be used. Properties
         * must be declared as static objects - static class members or static
         * function variables (the second one is preferred since properties will
         * be created when they are really needed).
         *
         * Example of registering function:
         *  \code
         *  void SomeClass::OnEnumProperties(long Flags)
         *  {
         *      static wxsLongProperty Prop1(_("Long value"),_T("value"),wxsOFFSET(SomeClass,VariableName));
         *      if ( Flags & flXrc )
         *      {
         *          Property(Prop1);
         *      }
         *      ... or equal to previous 3 lines ...
         *      Property(Prop1,flXrc);
         *  }
         *  \endcode
         */
        virtual void OnEnumProperties(long Flags) = 0;

        /** \brief Function registering property
         *
         * \note This function may be called only inside OnEnumProperties function
         *
         * \param Prop reference to property object which will be used
         * \param Priority priority of this property, it's used while creating property grid
         *        to arrange properties.
         */
        void Property(wxsProperty& Prop);

        /** \brief Function enumerating properties of other container
         * \note This function may be called only inside OnEnumProperties function
         */
        inline void SubContainer(wxsPropertyContainer* Container)
        {
            if ( Container ) SubContainer(Container,Container->GetPropertiesFlags());
        }

        /** \brief Function enumerating properties of other container from it's reference
         * \note This function may be called only inside OnEnumProperties function
         */
        inline void SubContainer(wxsPropertyContainer& Container)
        {
            SubContainer(&Container);
        }

        /** \brief Function enumerating properties of other container overriding it's flags
         * \note This function may be called only inside OnEnumProperties function
         */
        void SubContainer(wxsPropertyContainer* Container,long NewFlags);

        /** \brief Function enumerating properties of other container overriding it's flags
         * \note This function may be called only inside OnEnumProperties function
         */
        inline void SubContainer(wxsPropertyContainer& Container,long NewFlags)
        {
            SubContainer(&Container,NewFlags);
        }

        /** \brief Function building quick properties window */
        virtual wxsQuickPropsPanel* OnCreateQuickProperties(wxWindow* Parent) { return 0; }

        /** \brief Function notifying that one of properties has changed
         *
         * This function may be redefined in derived classes to get notified
         * about change of it's properties.
         */
        virtual void OnPropertyChanged() {}

        /** \brief Function notifying that one of properties in sub-containers has changed
         *
         * This function may be redefined in derived classes to get notified
         * about change of sub-container property change.
         * Sub-container is other container called in OnEnumProperties using
         * SubContainer method.
         */
        virtual void OnSubPropertyChanged(wxsPropertyContainer* SubContainer) {}

        /** \brief Flag set when operating on property grid */
        static const long flPropGrid   = 0x40000000;

        /** \brief Flag set when operating on xml */
        static const long flXml        = 0x20000000;

        /** \brief Flag set when operating on property stream */
        static const long flPropStream = 0x10000000;

        /** \brief In this function derived classes may add extra tabs to property grid
         *
         * \warning This function is not called for sub-containers.
         */
        virtual void OnAddExtraProperties(wxsPropertyGridManager* Grid) {}

        /** \brief Function notifying that some extra property (which could be added
         *         inside OnAddExtraProperties call) has changed.
         */
        virtual void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id) {}

    private:

        /** \brief Function notifying change of any property inside property grid */
        void NotifyPropertyChangeFromPropertyGrid();

        /** \brief Function notifying change of any property inside quick properties panel */
        void NotifyPropertyChangeFromQuickProps();

        wxsQuickPropsPanel* CurrentQP;          /// \brief Currently associated quick properties panel
        bool BlockChangeCallback;               /// \brief set to true if should block NotifyChange functions -
                                                ///        it will be used to avoid recursive calling of property
                                                ///        change when updating editors
        static long Flags;                      /// \brief Local cache of availability flags
        static bool IsRead;                     /// \brief Used to determine whether we are reading or writing data
        static TiXmlElement* CurrentElement;    /// \brief Local cache of current xml node
        static wxsPropertyStream* CurrentStream;/// \brief Local cache of current property stream
        static wxMutex Mutex;                   /// \brief Mutex used when operating on properties

        friend class wxsPropertyGridManager;
        friend class wxsQuickPropsPanel;
        friend class wxsProperty;
};

#endif

/**
 \page wxSmith
 \section wxsProperties Properties system

    All items inside window resource (and maybe other items in future) are using
 internal wxSmith's properties system. Properties are used for four basic
 purposes:

    \li Integrating with property grid (reading / writing / managing content)
    \li Integrating with xml files using xrc structure (reading / writing)
    \li Integrating with properties streams - streams were designed for wide
        range of actions, mainly reading, writing and correcting data in
        user-defined form
    \li Integrating with quick properties system (this does not depend on
        separate properties but uses whole container)

 Each object using properties system has list of properties objects - classes
 which manage data. Each property object is responsible for first three purposes
 described above. Mainly property object takes one or more variables from
 class and provides procedures operating on them. These procedures are unified
 and thus operating on properties is identical in all cases, no matter what's
 type of data.

 \subsection wxsPropertyGridManager Property Grid Manager

    There should be one instance of wxsPropertyGridManager widget somewhere in
 system. This is singleton-like class which will be used as main property grid.
 When this class is not provided, properties will work fine, but will be
 generally useless.

 \subsection wxsPropertyContainer Property containers

    Objects using properties must be derived from wxsPropertyContainer class.
 Basically this class provides array of properties objects (wxsProperty*
 classes), and it provides all four groups of actions described at the beginning
 of this page.

    Deriving from wxsPropertyContainer class require defining GetProperties
 method. This method is used to get list of properties associated with this
 class. This function should return static array of wxsProperty* objects
 (properties should be created once for class, not for instance of class)

   wxsPropertyContainer additionally provides NotifyPropertyChange function
 which is called whenever value of any property is changed. This function may
 be used to refresh other components and to validate property values. When
 value of property is changed inside NotifyPropertyChange, it will be refreshed
 automatically after return.

 Other functions provided in wxsPropertyContainer are:
 \li \link wxsPropertyContainer::ShowInPropertyGrid ShowInPropertyGrid \endlink
        this function is used to select item currently edited in main property
        grid
 \li \link wxsPropertyContainer::NotifyPropertyChange NotifyPropertyChange
        \endlink this function should be called when any of properties is
        changed outside. It is rereading data in property grid, quick properties
        panel and may raise internal notification about property change.
 \li \link wxsPropertyContainer::XmlRead XmlRead \endlink this function reads
        values of all properties from given Xml node
 \li \link wxsPropertyContainer::XmlWrite XmlWrite \endlink this function writes
        values of all properties to given Xml node
 \li \link wxsPropertyContainer::PropStreamRead PropStreamRead \endlink this
        function reads values of all properties from given property stream
        ( \link wxsPropertyContainer::operator>> operator >> \endlink may be
        used instead)
 \li \link wxsPropertyContainer::PropStreamWrite PropStreamWrite \endlink this
        function writes vaues of all properties to given
        property stream ( \link wxsPropertyContainer::operator<< operator <<
        \endlink may be used instead)
 \li \link wxsPropertyContainer::BuildQuickPropertiesPanel
        BuildQuickPropertiesPanel \endlink which creates and associates new
        quick properties panel (described below)


 \subsection wxsPropertyObject property object

    Property object is object responsible for managing one or more variables in
 class and for providing unified functions operating on it. These functions can
 be divided to three groups (almost same as property container base tasks,
 without support for quick properties):
    \li Functions operating on property grid
        - \link wxsProperty::PGCreate PGCreate \endlink function creating entry
          in property grid and filling it with proper data
        - \link wxsProperty::PGRead PGRead \endlink function reading values from
          property grid and storing them in real variables
        - \link wxsProperty::PGWrite PGWrite \endlink function updating value
          of property in property grid (reading from real variables and storing
          it inside property grid)
    \li Functions operating on xml data
        - \link wxsProperty::XmlRead XmlRead \endlink function reading value
          from given xml node and storing it inside real variables (or resetting
          values to default ones on read errors)
        - \link wxsProperty::XmlWrite XmlWrite \endlink function writing value
          to given xml node (read from real variable), also see note at the end
          of this subsection
    \li Functions operating on property streams
        - \link wxsProperty::PropStreamRead PropStreamRead \endlink function
          reading value from property stream and storing it inside real
          variables
        - \link wxsProperty::PropStreamWrite PropStreamWrite \endlink function
          writing value of real variables to property stream

    Adding new property require defining all those functions in new property
 class. Additionally wxsCutomEditorProperty class may be used. This class
 defines all PG... functions, but other are added:
    \li \link wxsCustomEditorProperty::ShowEditor ShowEditor \endlink inside
        this function, edit dialog should be created and showed in modal. Dialog
        should be destroyed before exiting (in usual case). This function is
        declared public to allow other items than property grid to show
        property editor.
    \li \link wxsCustomEditorProperty::GetStr GetStr \endlink this function
        returns string built from property value. This function and
        next ones are not required but may be added to increase property
        functionality.
    \li \link wxsCustomEditorProperty::ParseStr ParseStr \endlink this function
        should parse given string and fetch property value from it. It's not
        required.
    \li \link wxsCustomEditorProperty::CanParseStr CanParseStr \endlink this
        function notifies if this property is able to parse string (if ParseStr
        function has been defined). If yes, user may edit property using
        property editor or by writing string in property grid. This function
        is not required.

    \note Usually when saving data to Xml node, it should not be stored when
        values are equal to default ones. If it's not done in such way, this
        must be notified in class documentation.

 \subsection wxsQuickPropsPanel Quick Properties Panel

    Quick properties panel is additional editor area where properties can be
 changed. It's purpose is to allow editing most useful properties in easier
 way than in property grid  and to make the edition more intuitive (very
 standardized treatment in property grid does not leave much place to any kind
 of invention).

    Each property container may have one quick properties editor at one time.
 Panel may be taken using wxsPropertyContainer::BuildQuickPropsPanel function.
 To create panel inside class, OnCreateQuickProperties member function must be
 re-declared and must return pointer to newly created quick properties panel.
 Valid wxsPropertyContainer pointer must be passed to wxsQuickPropsPanel
 constructor.

    Note that Quick Properties Panel must be managed outside property container.
 Container won't delete it. Only bindings are deleted.

 \warning Do not use OnCreateQuickProperties function for getting panel since it
 does not unbind previous quick properties panel and does not bind new one with
 container correctly.

 */
