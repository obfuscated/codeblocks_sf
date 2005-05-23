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
        
        /** This function must create window which will be responsible for
         *  editing property's value */
        virtual wxWindow* BuildEditWindow(wxWindow* Parent);
        
        /** This funcytion must update content of currently created editor window
         *  taking it's value prop current property
         */
        virtual void UpdateEditWindow();
        
	private:
	
        int& Style;
        wxsStyle* Styles;
        wxsStylePropertyWindow* Window;
        friend class wxsStylePropertyWindow;
};

#endif // WXSSTYLEPROPERTY_H
