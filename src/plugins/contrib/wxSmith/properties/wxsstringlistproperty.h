#ifndef WXSSTRINGLISTPROPERTY_H
#define WXSSTRINGLISTPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"


class wxsStringListProperty : public wxsProperty
{
	public:
	
        /** Ctor */
		wxsStringListProperty(wxsProperties* Properties,wxArrayString& Array);
		
        /** Ctor */
		wxsStringListProperty(wxsProperties* Properties,wxArrayString& Array,int& Selected);
		
		/** DCtor */
		virtual ~wxsStringListProperty();
		
        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();
        
    protected:
        
        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
            void EditList();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
	private:

		wxArrayString& Array;
		int* Selected;
		
		#ifdef __NO_PROPGRGID
            friend class wxsStringListPropertyWindow;
        #else
            wxPGId PGId;
        #endif
};

#endif
