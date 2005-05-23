#ifndef WXSPLACEMENTPROPERTY_H
#define WXSPLACEMENTPROPERTY_H

#include "../wxsproperties.h"

class wxsPlacementPropertyWindow;

class wxsPlacementProperty : public wxsProperty
{
	public:
	
        /** Ctor */
		wxsPlacementProperty(wxsProperties* Properties,int& Placement,bool& Expand,bool& Shaped);

        /** Dctor */
		virtual ~wxsPlacementProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();
        
    protected:
        
        /** This function must create window which will be responsible for
         *  editing property's value */
        virtual wxWindow* BuildEditWindow(wxWindow* Parent);
        
        /** This funcytion must update content of currently created editor window
         *  taking it's value prop current property
         */
        virtual void UpdateEditWindow();
        
	private:
	
        wxsPlacementPropertyWindow* Window;
        
        int &PlacementType;
        bool &Expand;
        bool &Shaped;
        
        friend class wxsPlacementPropertyWindow;
};

#endif 
