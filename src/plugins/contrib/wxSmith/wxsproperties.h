#ifndef WXSPROPERTIES_H
#define WXSPROPERTIES_H

#include <wx/string.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <vector>

/* Predefined classes */
class wxsWidget;
class wxsProperties;


/** Class used to handle one property
 *
 *  The idea of this class is to give object which would be able to
 *  create window responsible for editing given property
 */
class wxsProperty
{
    public:
        
        /** ctor */
        wxsProperty(wxsProperties* Properties):
            Props(Properties),
            IsReshaping(true),
            IsRecreating(false)
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
        virtual void ValueChanged();
        
    protected:
        
        /** This function must create window which will be responsible for
         *  editing property's value */
        virtual wxWindow* BuildEditWindow(wxWindow* Parent) = 0;
        
        /** This funcytion must update content of currently created editor window
         *  taking it's value prop current property
         */
        virtual void UpdateEditWindow() = 0;
        
        /** This function returns wxsProperties object which uses this property */
        inline wxsProperties* GetProperties() { return Props; }
        
    private:
        wxsProperties* Props;
        
        /** These members are used inside wxsProperties object */
        bool IsReshaping;
        bool IsRecreating;
        
        friend class wxsProperties;
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
		virtual void AddProperty(const wxString& Name,wxString& Value,int Position=-1,bool Reshape=true,bool Recreate=false);
		
		/** Adding new integer property */
		virtual void AddProperty(const wxString& Name,int& Value,int Position=-1,bool Reshape=true,bool Recreate=false);
		
		/** Adding bool property */
		virtual void AddProperty(const wxString& Name,bool& Value,int Position=-1,bool Reshape=true,bool Recreate=false);
		
		/** Adding new 2xinteger property */
		virtual void Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position=-1,bool Reshape=true,bool Recreate=false);
		
		/** Adding custon property */
		virtual void AddProperty(const wxString& Name,wxsProperty* Property,int Position=-1,bool Reshape=true,bool Recreate=false);
		
		/** Generating properties window for this properties */
		virtual wxWindow* GenerateWindow(wxWindow* Parent,wxSizer** Sizer);
		
		/** Updating content of current properties window */
		virtual void UpdateProperties();
		
    protected:
    
        /** Function clearing the array of properties */
        void ClearArray();
		
	private:

        /** Notifying about property change */
        virtual void NotifyChange(wxsProperty* Prop);
        
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
        
        /** Used for blocking update callbacks */
        bool BlockUpdates;
        
        friend class wxsProperty;
        
};

#endif // WXSBASEPROPERTIES_H
