#ifndef WXSBOOLPROPERTY_H
#define WXSBOOLPROPERTY_H

#include "../wxsproperties.h"

class wxsBoolPropertyWindow;
class wxsBoolProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param Bool - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsBoolProperty(wxsProperties* Properties,bool &Bool);
		
		/** Dctor */
		virtual ~wxsBoolProperty();
		
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
	
        bool& Value;
        wxsBoolPropertyWindow* Window;
        
        friend class wxsBoolPropertyWindow;
};

#endif // WXSBOOLPROPERTY_H
