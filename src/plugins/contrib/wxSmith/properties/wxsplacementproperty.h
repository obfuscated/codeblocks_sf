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
        
        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual void PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
	private:
	
        int &PlacementType;
        bool &Expand;
        bool &Shaped;

        #ifdef __NO_PROPGRGID
            wxsPlacementPropertyWindow* Window;
            friend class wxsPlacementPropertyWindow;
        #else
            wxPGId PGId;
            wxPGId ExpandId;
            wxPGId ShapedId;
        #endif
};

#endif 
