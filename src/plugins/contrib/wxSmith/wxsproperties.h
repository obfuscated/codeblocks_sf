#ifndef WXSPROPERTIES_H
#define WXSPROPERTIES_H

#include <wx/string.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <vector>

#include <wx/scrolwin.h>
#include <wx/propgrid/propgrid.h>

#include "wxsglobals.h"

/* Predefined classes */
class wxsWidget;
class wxsProperties;
class wxsPropertyGrid;


/** Class used to handle one property
 *
 *  The idea of this class is to give object which would be able to
 *  create window responsible for editing given property
 */
class wxsProperty
{
    public:

        /** ctor */
        wxsProperty(wxsProperties* Properties): Props(Properties)
        {}

        /** dctor */
        virtual ~wxsProperty()
        {}

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName() = 0;

        /** This function notifies properties manager that value of this property
         *  has changed, it must be called every time we need to notice other
         *  components that the value has changed
         */
        virtual bool ValueChanged(bool Check);

    protected:

        /** Function adding entry for this property inside wxPropertyGrid class */
        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name) = 0;

        /** Function notifying about property change
         *
         * \return If false, changed property has invalid value
         */
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id) = 0;

        /** Function updating value of this property insided property grid */
        virtual void UpdatePropGrid(wxPropertyGrid* Grid) = 0;

        /** This function returns wxsProperties object which uses this property */
        inline wxsProperties* GetProperties() { return Props; }

    private:

        /** Properties object handling this property */
        wxsProperties* Props;
        
        friend class wxsProperties;
        friend class wxsPropertyGrid;
};


/** Class managing base properties of widget.
 *
 *  This class handles set of properties and manages them
 */
class wxsProperties
{
	public:

		wxsProperties(wxsWidget* _Widget);

		virtual ~wxsProperties();

		/** Adding new string property */
		virtual void AddProperty(const wxString& Name,wxString& Value,int Position=-1);

		/** Adding new integer property */
		virtual void AddProperty(const wxString& Name,int& Value,int Position=-1);

		/** Adding bool property */
		virtual void AddProperty(const wxString& Name,bool& Value,int Position=-1);

		/** Adding new 2xinteger property */
		virtual void Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position=-1);

		/** Adding new wxArrayStrting property */
		virtual void AddProperty(const wxString& Name,wxArrayString& Array,int Position=-1);

		/** Adding new wxArrayStrting property with additional "selected" flag */
		virtual void AddProperty(const wxString& Name,wxArrayString& Array,int& Selected,int SortFlag,int Position=-1);

		/** Adding custom property */
		virtual void AddProperty(const wxString& Name,wxsProperty* Property,int Position=-1);

		/** Generating properties window for this properties */
		virtual wxWindow* GenerateWindow(wxWindow* Parent);

		/** Updating content of current properties window */
		virtual void UpdateProperties();

		/** Getting widget associated with this properties obiject */
		inline wxsWidget* GetWidget() { return Widget; }

    protected:

        /** Function clearing the array of properties */
        void ClearArray();

	private:

        /** Notifying about property change
         *
         * \param Check - if true, properties will be adidtionally checked
         * \return False when Check==true and not all properties were valid, true otherwise
         */
        virtual bool NotifyChange(bool Check);

        /** Structure holding one property */
        struct VectorElem
        {
            wxString Name;          ///< Property's name
            wxsProperty* Property;  ///< Property objecct
        };

        /** Vector types */
        typedef std::vector<VectorElem*> VectorT;
        typedef VectorT::iterator VectorI;

        /** Vector of properties */
        VectorT Properties;

        /** Widget which is associated with this properties */
        wxsWidget* Widget;

        wxsPropertyGrid* Grid;

        /** Used for blocking update callbacks */
        bool BlockUpdates;

        friend class wxsProperty;
        friend class wxsPropertyGrid;
};

#endif // WXSBASEPROPERTIES_H
