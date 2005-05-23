#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "../wxsproperties.h"

class wxsStringPropertyWindow;

class wxsStringProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param String - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsStringProperty(wxsProperties* Properties,wxString& String, bool AlwaysUpdate);
		
		/** Dctor */
		virtual ~wxsStringProperty();
		
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
	
        wxString& Value;
        bool AlwUpd;
        wxsStringPropertyWindow* Window;
        
        friend class wxsStringPropertyWindow;
};

#endif // WXSSTRINGPROPERTY_H
