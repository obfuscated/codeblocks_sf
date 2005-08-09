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
        
        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual void PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
	private:
	
        int &BorderFlags;
        
        #ifdef __NO_PROPGRGID
            wxsBorderPropertyWindow* Window;
            friend class wxsBorderPropertyWindow;
        #else
            wxPGId PGId;
        #endif
};

#endif // WXSBORDERPROPERTY_H
