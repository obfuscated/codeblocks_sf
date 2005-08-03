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
        
        /** This function must create window which will be responsible for
         *  editing property's value */
        virtual wxWindow* BuildEditWindow(wxWindow* Parent);

        /** This funcytion must update content of currently created editor window
         *  taking it's value prop current property
         */
        virtual void UpdateEditWindow();
        
        /** Function displaying dialog with list editor */
        void EditList();
        
	private:

		wxArrayString& Array;
		int* Selected;
		friend class wxsStringListPropertyWindow;
};

#endif
