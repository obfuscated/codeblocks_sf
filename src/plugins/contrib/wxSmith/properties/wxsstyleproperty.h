#ifndef WXSSTYLEPROPERTY_H
#define WXSSTYLEPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class wxsStylePropertyWindow;
class wxsStyleProperty : public wxsProperty
{
	public:
	
        /** Ctor */
		wxsStyleProperty(wxsProperties* Properties,int &Style,wxsStyle*Styles);
		
		/** DCtor */
		virtual ~wxsStyleProperty();
		
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
	
        int& Style;
        wxsStyle* Styles;
        
        #ifdef __NO_PROPGRGID
            wxsStylePropertyWindow* Window;
            friend class wxsStylePropertyWindow;
        #else
            wxPGId PGId;
        #endif
};

#endif // WXSSTYLEPROPERTY_H
