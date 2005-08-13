#ifndef WXSENUMPROPERTY_H
#define WXSENUMPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class wxsEnumPropertyWindow;
class wxsEnumProperty : public wxsProperty
{
	public:
	
        /** Ctor */
		wxsEnumProperty(wxsProperties* Properties,int &Value,const wxChar** Names,const long* Values);
		
		/** DCtor */
		virtual ~wxsEnumProperty();
		
        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();
        
    protected:
    
        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
	private:
	
        int& Value;
        const wxChar** Names;
        const long* Values;
        
        #ifdef __NO_PROPGRGID
            wxsEnumPropertyWindow* Window;
            friend class wxsEnumPropertyWindow;
        #else
            wxPGId PGId;
        #endif
};

#endif 
