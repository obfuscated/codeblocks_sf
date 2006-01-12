#ifndef WXSPROPERTIES_H
#define WXSPROPERTIES_H

#include <wx/string.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <vector>

#include <wx/scrolwin.h>
#include <wx/propgrid/propgrid.h>


#include "wxsglobals.h"

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
        wxsProperty(): Props(NULL)
        {}

        /** dctor */
        virtual ~wxsProperty()
        {}

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

        /** This function notifies properties manager that value of this property
         *  has changed, it must be called every time we need to notice other
         *  components that the value has changed.
         *
         *  \param Check if true, it's possible that property has unacceptable
         *         value and should be checker
         *  \return true if change was accepted, false if value is incorrect
         */
        bool ValueChanged(bool Check);

    private:

        /** Properties object handling this property */
        wxsProperties* Props;

        friend class wxsProperties;
        friend class wxsPropertyGrid;
};

/** Class managing properties of widget. */
class wxsProperties
{
	public:

        /** Ctor */
		wxsProperties(wxsWidget* _Widget);

        /** Dctor */
		~wxsProperties();

		/** Adding new string property */
		void AddProperty(const wxString& Name,wxString& Value,int Position=-1);

		/** Adding new integer property */
		void AddProperty(const wxString& Name,int& Value,int Position=-1);

		/** Adding bool property */
		void AddProperty(const wxString& Name,bool& Value,int Position=-1);

		/** Adding new 2xinteger property */
		void Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position=-1);

		/** Adding new wxArrayString property */
		void AddProperty(const wxString& Name,wxArrayString& Array,int Position=-1);

		/** Adding new wxArrayString property with additional "selected" flag */
		void AddProperty(const wxString& Name,wxArrayString& Array,int& Selected,int SortFlag,int Position=-1);

		/** Adding new AdvImageProperty property , adding a type to differentiate with wxsStringProperty*/
		void AddProperty(const wxString& Name,wxString& Value,bool imageflag, int Position=-1);

		/** Adding custom property */
		void AddProperty(const wxString& Name,wxsProperty* Property,int Position=-1);

		/** Generating new prop grid window */
		wxWindow* GenerateWindow(wxWindow* Parent);

		/** Getting prop grid window previously created */
		wxWindow* GetWindow();

		/** Updating content of current properties window */
		void UpdateProperties();

		/** Getting widget associated with this properties object */
		inline wxsWidget* GetWidget() { return Widget; }

	private:

        /** Notifying about property change
         *
         * \param Check - if true, properties will be adidtionally checked
         * \return False when Check==true and not all properties were valid, true otherwise
         */
        bool NotifyChange(bool Check);

        /** Function clearing the array of properties */
        void ClearArray();

        /** Structure holding one property */
        struct VectorElem
        {
            wxString Name;          ///< Property's name
            wxsProperty* Property;  ///< Property object
        };

        typedef std::vector<VectorElem*> VectorT;
        typedef VectorT::iterator VectorI;

        /** Vector of properties */
        VectorT Properties;

        /** Widget which is associated with this properties */
        wxsWidget* Widget;

        /** Curent property grid */
        wxsPropertyGrid* Grid;

        /** Used for blocking update callbacks */
        bool BlockUpdates;

        friend class wxsPropertyGrid;
        friend class wxsProperty;       // Allowing NotifyChange from property
};

#endif // WXSBASEPROPERTIES_H
