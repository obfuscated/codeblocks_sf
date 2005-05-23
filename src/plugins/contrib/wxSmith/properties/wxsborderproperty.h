#ifndef WXSBORDERPROPERTY_H
#define WXSBORDERPROPERTY_H

#include "../wxsproperties.h"

class wxsBorderPropertyWindow;

class wxsBorderProperty : public wxsProperty
{
	public:
	
        /** Ctor */
		wxsBorderProperty(wxsProperties* Properties,int& Flag);

        /** Dctor */
		virtual ~wxsBorderProperty();

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
	
        wxsBorderPropertyWindow* Window;
        
        int &BorderFlags;
        
        friend class wxsBorderPropertyWindow;
};

#endif // WXSBORDERPROPERTY_H
